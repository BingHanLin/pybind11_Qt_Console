#include <vector>

#include <QDialog>
#include <QLabel>
#include <QListWidget>


#include "chatItemWidget.hpp"
#include "dataModel.hpp"
#include "nlohmann/json.hpp"

class LLMChat : public QDialog
{
    Q_OBJECT

  public:
    explicit LLMChat(std::shared_ptr<dataModel> model, QWidget* parent = nullptr);

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    std::shared_ptr<dataModel> model_;
    QListWidget* chatList_;
    QLabel* popupLabel_;
    std::vector<nlohmann::json> messageHistory_;

    void updateListItemSizes();

    void appendMessage(const chatItemRole& role, const QString& message);

    void showPopupMessage(const QString& message);

    [[nodiscard]] nlohmann::json generatePayload(const std::vector<nlohmann::json>& newMessage);

    nlohmann::json processResponse(const nlohmann::json& response);

  private slots:
    void onSendMessageClicked(const QString& inputMessage);
};