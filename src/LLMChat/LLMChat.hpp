#include <QDialog>
#include <QListWidget>
#include <vector>

#include "dataModel.hpp"
#include "nlohmann/json.hpp"

class LLMChat : public QDialog
{
    Q_OBJECT

  public:
    explicit LLMChat(std::shared_ptr<dataModel> model, QWidget* parent = nullptr);

  private:
    std::shared_ptr<dataModel> model_;
    QListWidget* chatList_;
    std::vector<nlohmann::json> messageHistory_;

    void appendUserMessage(const QString& message);
    void appendAssistantMessage(const QString& message);

    [[nodiscard]] nlohmann::json generatePayload(const std::vector<nlohmann::json>& newMessage);

    nlohmann::json processResponse(const nlohmann::json& response);

  private slots:
    void onSendMessageClicked(const QString& inputMessage);
};