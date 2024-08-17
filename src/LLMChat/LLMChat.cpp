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
    this->setWindowTitle("LLM Chat");

    chatList_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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

    lineEdit->setFocus();
}

void LLMChat::appendUserMessage(const QString& message)
{
    const auto trimmedMessage = message.trimmed();
    auto widget = new chatItemWidget();
    widget->setMessage(trimmedMessage, chatItemRole::USER);

    auto item = new QListWidgetItem();
    item->setSizeHint(widget->size());

    chatList_->addItem(item);
    chatList_->setItemWidget(item, widget);
}

void LLMChat::appendAssistantMessage(const QString& message)
{
    const auto trimmedMessage = message.trimmed();
    auto widget = new chatItemWidget();
    widget->setMessage(trimmedMessage, chatItemRole::ASSISTANT);

    auto item = new QListWidgetItem();
    item->setSizeHint(widget->size());
    item->setBackground(QColor(199, 252, 255));

    chatList_->addItem(item);
    chatList_->setItemWidget(item, widget);
}

[[nodiscard]] nlohmann::json LLMChat::generatePayload(const std::vector<nlohmann::json>& newMessages)
{
    auto paylod = R"({
        "model": "gpt-3.5-turbo",
        "max_tokens": 500,
        "temperature": 0
    })"_json;

    // setup tools
    {
        auto toolsJSON = nlohmann::json::array();

        toolsJSON.push_back(R"({
        "type": "function",
        "function": {
            "name": "clear_all_orders",
            "description": "Clear all orders. Only call this when a customer asks to clear all orders, for example 'Clear all orders'.",
            "parameters": {}
        }
        })"_json);

        paylod["tools"] = toolsJSON;
    }

    // setup messages
    {
        for (const auto& oneMessage : newMessages)
        {
            messageHistory_.push_back(oneMessage);
        }

        auto messagesJSON = nlohmann::json::array();

        messagesJSON.push_back(R"({
            "role" : "system",
            "content" : "You are a helpful assistant. Use the supplied tools to assist the user if necessary."
        })"_json);

        for (const auto& message : messageHistory_)
        {
            messagesJSON.push_back(message);
        }

        paylod["messages"] = messagesJSON;
    }

    return paylod;
}

nlohmann::json LLMChat::processResponse(const nlohmann::json& response)
{
    std::cout << "Response is:\n" << response.dump(4) << std::endl;

    if (!response.contains("choices")) return {};
    if (!response["choices"].is_array()) return {};
    if (response["choices"].empty()) return {};

    const auto& choice = response["choices"][0];

    messageHistory_.push_back(choice.at("message"));

    if (choice.at("finish_reason") == "tool_calls")
    {
        const auto& toolCalls = choice.at("message").at("tool_calls");

        std::vector<nlohmann::json> newMessages;

        for (const auto& toolCall : toolCalls)
        {
            if (toolCall.at("function").at("name") == "clear_all_orders")
            {
                auto command = new clearAllOrdersCommand(model_);
                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);

                auto toolResultMessage = R"({
                    "role": "tool",
                    "content": "All orders have been cleared.",
                    "tool_call_id": 0
                })"_json;

                toolResultMessage["tool_call_id"] = toolCall.at("id");

                newMessages.push_back(toolResultMessage);
            };
        }

        return generatePayload(newMessages);
    }
    else if (choice.at("finish_reason") == "stop")
    {
        const auto& assistantMessage = choice.at("message").at("content");
        this->appendAssistantMessage(QString::fromStdString(assistantMessage));

        return {};
    }

    return {};
}

void LLMChat::onSendMessageClicked(const QString& message)
{
    this->appendUserMessage(message);

    auto userMessage = nlohmann::json::object();
    userMessage["role"] = "user";
    userMessage["content"] = message.toStdString();

    auto payload = generatePayload({userMessage});

    while (!payload.empty())
    {
        openai::start();

        const auto response = openai::chat().create(payload);

        payload = this->processResponse(response);
    }
}