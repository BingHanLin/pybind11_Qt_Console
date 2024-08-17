#include <utility>

#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "LLMChat.hpp"
#include "chatUtils.hpp"
#include "commandManager.hpp"
#include "commands.hpp"
#include "nlohmann/json.hpp"
#include "openai.hpp"

LLMChat::LLMChat(std::shared_ptr<dataModel> model, QWidget* parent)
    : QDialog(parent), model_(std::move(model)), chatList_(new QListWidget(this)), popupLabel_(nullptr)
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

    QTimer::singleShot(0,
                       [this]() {
                           this->appendMessage(chatItemRole::ASSISTANT,
                                               tr("Hello! I am a helpful assistant. How can I assist you today?"));
                       });
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

void LLMChat::appendMessage(const chatItemRole& role, const QString& message)
{
    auto item = new QListWidgetItem();

    const auto trimmedMessage = message.trimmed();
    auto widget = new chatItemWidget();

    if (role == chatItemRole::USER)
    {
        widget->setMessage(trimmedMessage, chatItemRole::USER);
    }
    else
    {
        widget->setMessage(trimmedMessage, chatItemRole::ASSISTANT);
        item->setBackground(QColor(199, 252, 255));
    }

    item->setSizeHint(widget->size());

    chatList_->addItem(item);
    chatList_->setItemWidget(item, widget);

    chatList_->scrollToBottom();

    this->updateListItemSizes();

    QApplication::processEvents();
}

void LLMChat::showPopupMessage(const QString& message)
{
    popupLabel_ = new QLabel(message, chatList_);
    popupLabel_->setStyleSheet("QLabel { background-color : yellow; color : black; padding: 5px; }");
    popupLabel_->setAlignment(Qt::AlignLeft);

    const int x = 0;
    const int y = chatList_->height() - popupLabel_->height();

    popupLabel_->setGeometry(x, y, chatList_->width(), popupLabel_->height());

    auto effect = new QGraphicsOpacityEffect(popupLabel_);
    popupLabel_->setGraphicsEffect(effect);

    popupLabel_->show();

    QTimer::singleShot(1500,
                       [=]()
                       {
                           auto fadeOut = new QPropertyAnimation(effect, "opacity");
                           fadeOut->setDuration(500);  // Fade-out duration: 500ms
                           fadeOut->setStartValue(1);
                           fadeOut->setEndValue(0);

                           QObject::connect(fadeOut, &QPropertyAnimation::finished, popupLabel_,
                                            [this]()
                                            {
                                                popupLabel_->deleteLater();
                                                popupLabel_ = nullptr;
                                            });

                           fadeOut->start(QPropertyAnimation::DeleteWhenStopped);
                       });

    QEventLoop loop;
    QObject::connect(popupLabel_, &QLabel::destroyed, &loop, &QEventLoop::quit);
    loop.exec();  // Block the main thread until the popupLabel is destroyed
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

        toolsJSON.push_back(R"({
        "type": "function",
        "function": {
            "name": "add_order",
            "description": "Add an order under the specified group. The following properties should be provided by user: group, name, price, amount.",
            "parameters": {
                "type": "object",
                "properties": {
                    "group": {
                        "type": "string",
                        "description": "The group to add the order to."
                    },
                    "name": {
                        "type": "string",
                        "description": "The name of the order."
                    },
                    "price": {
                        "type": "number",
                        "description": "The price of the order."
                    },
                    "amount": {
                        "type": "number",
                        "description": "The amount of the order."
                    }
                }
            }
        }
        })"_json);

        toolsJSON.push_back(R"({
        "type": "function",
        "function": {
            "name": "delete_order",
            "description": "Delete an order from the specified group. The following properties should be provided by user: group, name.",
            "parameters": {
                "type": "object",
                "properties": {
                    "group": {
                        "type": "string",
                        "description": "The group to add the order to."
                    },
                    "name": {
                        "type": "string",
                        "description": "The name of the order."
                    }
                }
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
                this->showPopupMessage(tr("Clearing all orders..."));

                const auto result = clearAllOrders(model_);

                auto toolResultMessage = R"({
                    "role": "tool",
                    "content": "",
                    "tool_call_id": 0
                })"_json;

                toolResultMessage["tool_call_id"] = toolCall.at("id");

                if (result)
                {
                    toolResultMessage.at("content") = "All orders have been cleared.";
                }
                else
                {
                    toolResultMessage.at("content") = "Failed to clear all orders.";
                }

                newMessages.push_back(toolResultMessage);
            }
            else if (toolName == "show_all_orders")
            {
                this->showPopupMessage(tr("Querying orders information..."));

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
            else if (toolName == "delete_order")
            {
                this->showPopupMessage(tr("Deleting order..."));

                const auto arguments =
                    nlohmann::json::parse(toolCall.at("function").at("arguments").get<std::string>());

                const auto groupName = arguments.at("group").get<std::string>();
                const auto orderName = arguments.at("name").get<std::string>();

                const auto result = deleteOrder(model_, groupName, orderName);

                auto toolResultMessage = R"({
                    "role": "tool",
                    "content": "",
                    "tool_call_id": 0
                })"_json;

                toolResultMessage["tool_call_id"] = toolCall.at("id");

                if (result)
                {
                    toolResultMessage.at("content") = "Order has been deleted.";
                }
                else
                {
                    toolResultMessage.at("content") = "Failed to delete order.";
                }

                newMessages.push_back(toolResultMessage);
            }
            else if (toolName == "add_order")
            {
                this->showPopupMessage(tr("Adding order..."));

                const auto arguments =
                    nlohmann::json::parse(toolCall.at("function").at("arguments").get<std::string>());

                const auto groupName = arguments.at("group").get<std::string>();
                const auto orderName = arguments.at("name").get<std::string>();
                const auto price = arguments.at("price").get<double>();
                const auto amount = arguments.at("amount").get<int>();

                const auto result = addOrder(model_, groupName, orderName, price, amount);

                auto toolResultMessage = R"({
                    "role": "tool",
                    "content": "",
                    "tool_call_id": 0
                })"_json;

                toolResultMessage["tool_call_id"] = toolCall.at("id");

                if (result)
                {
                    toolResultMessage.at("content") = "Order has been added.";
                }
                else
                {
                    toolResultMessage.at("content") = "Failed to add order.";
                }

                newMessages.push_back(toolResultMessage);
            }
        }

        return generatePayload(newMessages);
    }
    else if (choice.at("finish_reason") == "stop")
    {
        const auto& assistantMessage = choice.at("message").at("content");
        this->appendMessage(chatItemRole::ASSISTANT, QString::fromStdString(assistantMessage));

        return {};
    }

    return {};
}

void LLMChat::onSendMessageClicked(const QString& message)
{
    this->appendMessage(chatItemRole::USER, message);

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