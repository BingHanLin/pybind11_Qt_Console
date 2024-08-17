#include <QDialog>
#include <QListWidget>

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

    void processResponse(const nlohmann::json& response);

    void appendUserMessage(const QString& message);
    void appendAssistantMessage(const QString& message);

  private slots:
    void onSendMessageClicked(const QString& inputMessage);
};