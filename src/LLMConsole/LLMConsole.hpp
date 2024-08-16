#include <QDialog>

#include "dataModel.hpp"
#include "nlohmann/json.hpp"

class LLMConsole : public QDialog
{
    Q_OBJECT

  public:
    explicit LLMConsole(std::shared_ptr<dataModel> model, QWidget* parent = nullptr);

  private:
    std::shared_ptr<dataModel> model_;

    void processResponse(const nlohmann::json& response);

  private slots:
    void sendMessage(const QString& inputMessage);
};