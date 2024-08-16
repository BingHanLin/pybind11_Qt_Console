#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <qboxlayout.h>

#include "LLMConsole.hpp"
#include "nlohmann/json.hpp"
#include "openai.hpp"

LLMConsole::LLMConsole(QWidget* parent) : QDialog(parent)
{
    auto layout = new QVBoxLayout(this);

    auto lineEdit = new QLineEdit(this);
    auto sendButton = new QPushButton("Send", this);

    layout->addWidget(lineEdit);

    auto hLayout = new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(sendButton);

    layout->addLayout(hLayout);

    connect(sendButton, &QPushButton::clicked, this,
            [this, lineEdit]()
            {
                const QString message = lineEdit->text();
                this->sendMessage(message);
            });
}

void LLMConsole::sendMessage(const QString& message)
{
    openai::start();

    auto messagesJSON = nlohmann::json::array();
    {
        messagesJSON.push_back(R"({
        "role" : "system",
        "content" : "You are a helpful customer support assistant. Use the supplied tools to assist the user."
    })"_json);

        auto userMessage = nlohmann::json::object();
        userMessage["role"] = "user";
        userMessage["content"] = message.toStdString();
        messagesJSON.push_back(userMessage);
    }

    auto toolsJSON = nlohmann::json::array();
    toolsJSON.push_back(R"({
        "type": "function",
        "function": {
            "name": "get_current_weather",
            "description": "Get the current weather in a given location",
            "parameters": {
                "type": "object",
                "properties": {
                    "location": { "type": "string", "description": "The location to get the weather for" }
                },
                "required": ["location"]
            }
        }
    })"_json);

    auto paylod = R"({
        "model": "gpt-3.5-turbo",
        "max_tokens": 500,
        "temperature": 0
    })"_json;

    paylod["messages"] = messagesJSON;
    paylod["tools"] = toolsJSON;

    auto completion = openai::chat().create(paylod);

    std::cout << "Response is:\n" << completion.dump(4) << std::endl;
}
