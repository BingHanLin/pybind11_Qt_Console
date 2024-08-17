#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <qboxlayout.h>
#include <utility>

#include "LLMChat.hpp"
#include "chatItemWidget.hpp"
#include "commandManager.hpp"
#include "commands.hpp"
#include "openai.hpp"

LLMChat::LLMChat(std::shared_ptr<dataModel> model, QWidget* parent)
    : QDialog(parent), model_(std::move(model)), chatList_(new QListWidget(this))
{
    auto layout = new QVBoxLayout(this);
    layout->addWidget(chatList_);

    auto lineEdit = new QLineEdit(this);
    auto sendButton = new QPushButton("Send", this);

    auto hLayout = new QHBoxLayout();
    hLayout->addWidget(lineEdit);
    hLayout->addWidget(sendButton);

    layout->addLayout(hLayout);

    connect(sendButton, &QPushButton::clicked, this,
            [this, lineEdit]()
            {
                const QString message = lineEdit->text();
                lineEdit->clear();
                lineEdit->setFocus();
                this->onSendMessageClicked(message);
            });
}

void LLMChat::appendUserMessage(const QString& message)
{
    const auto trimmedMessage = message.trimmed();
    auto widget = new chatItemWidget();
    widget->setMessage(trimmedMessage, chatItemRole::USER);

    auto item = new QListWidgetItem();
    item->setSizeHint(QSize(0, 65));

    chatList_->addItem(item);
    chatList_->setItemWidget(item, widget);
}

void LLMChat::appendAssistantMessage(const QString& message)
{
    const auto trimmedMessage = message.trimmed();
    auto widget = new chatItemWidget();
    widget->setMessage(trimmedMessage, chatItemRole::ASSISTANT);

    auto item = new QListWidgetItem();
    item->setSizeHint(QSize(0, 65));
    item->setBackground(QColor(167, 255, 237));

    chatList_->addItem(item);
    chatList_->setItemWidget(item, widget);
}

void LLMChat::onSendMessageClicked(const QString& message)
{
    this->appendUserMessage(message);

    openai::start();

    auto messagesJSON = nlohmann::json::array();
    {
        messagesJSON.push_back(R"({
        "role" : "system",
        "content" : "You are a helpful assistant. Use the supplied tools to assist the user if necessary."
    })"_json);

        auto userMessage = nlohmann::json::object();
        userMessage["role"] = "user";
        userMessage["content"] = message.toStdString();
        messagesJSON.push_back(userMessage);
    }

    auto toolsJSON = nlohmann::json::array();
    {
        toolsJSON.push_back(R"({
        "type": "function",
        "function": {
            "name": "clear_all_orders",
            "description": "Clear all orders. Call this when a customer asks to clear all orders, for example 'Clear all orders'.",
            "parameters": {}
        }
        })"_json);
    }

    auto paylod = R"({
        "model": "gpt-3.5-turbo",
        "max_tokens": 500,
        "temperature": 0
    })"_json;

    paylod["messages"] = messagesJSON;
    paylod["tools"] = toolsJSON;

    auto response = openai::chat().create(paylod);

    std::cout << "Response is:\n" << response.dump(4) << std::endl;

    this->processResponse(response);
}

void LLMChat::processResponse(const nlohmann::json& response)
{
    if (!response.contains("choices")) return;
    if (!response["choices"].is_array()) return;
    if (response["choices"].empty()) return;

    const auto& choice = response["choices"][0];

    if (choice.at("finish_reason") == "tool_calls")
    {
        const auto& toolCalls = choice.at("message").at("tool_calls");

        for (const auto& toolCall : toolCalls)
        {
            if (toolCall.at("function").at("name") == "clear_all_orders")
            {
                auto command = new clearAllOrdersCommand(model_);
                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);
            };
        }
    }
}