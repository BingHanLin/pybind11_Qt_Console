#include <utility>

#include <QLineEdit>
#include <QPushButton>
#include <QTimer.h>
#include <QVBoxLayout>
#include <qboxlayout.h>

#include "LLMChat.hpp"
#include "chatItemWidget.hpp"
#include "chatUtils.hpp"
#include "commandManager.hpp"
#include "commands.hpp"
#include "nlohmann/json.hpp"
#include "openai.hpp"

LLMChat::LLMChat(std::shared_ptr<dataModel> model, QWidget* parent)
    : QDialog(parent), model_(std::move(model)), chatList_(new QListWidget(this))
{
    this->setWindowTitle("LLM Chat");

    chatList_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(chatList_);

    auto lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText("Type your message to interact with the assistant...");

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

    QTimer::singleShot(
        0,
        [this]() { this->appendAssistantMessage(tr("Hello! I am a helpful assistant. How can I assist you today?")); });
}

void LLMChat::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);

    this->updateListItemSizes();
}

void LLMChat::updateListItemSizes()
{
    const int listWidth = chatList_->viewport()->width();
    for (int i = 0; i < chatList_->count(); ++i)
    {
        QListWidgetItem* item = chatList_->item(i);
        QWidget* widget = chatList_->itemWidget(item);
        if (widget)
        {
            widget->setFixedWidth(listWidth);
            item->setSizeHint(widget->sizeHint());
        }
    }
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

    chatList_->scrollToBottom();

    this->updateListItemSizes();
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

    chatList_->scrollToBottom();

    this->updateListItemSizes();
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
            "parameters": {
                "type": "object",
                "properties": {}
            }
        }
        })"_json);

        toolsJSON.push_back(R"({
        "type": "function",
        "function": {
            "name": "show_all_orders",
            "description": "Show all orders and the group they belong to. The result should be a JSON array of objects, where each object represents a group and contains an array of orders. Call this when a customer asks to show all orders, for example 'Show all orders'.",
            "parameters": {
                "type": "object",
                "properties": {}
            }
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
            const auto toolName = toolCall.at("function").at("name");
            if (toolName == "clear_all_orders")
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
            }
            else if (toolName == "show_all_orders")
            {
                const auto data = getAllOrdersInfo(model_);

                auto toolResultMessage = R"({
                    "role": "tool",
                    "content": "",
                    "tool_call_id": 0
                })"_json;

                toolResultMessage["content"] = data.dump(4);
                toolResultMessage["tool_call_id"] = toolCall.at("id");

                newMessages.push_back(toolResultMessage);
            }
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

        std::cout << "Payload is:\n" << payload.dump(4) << std::endl;

        const auto response = openai::chat().create(payload);

        payload = this->processResponse(response);
    }
}