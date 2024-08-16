#include <QDialog>
#include <qdialog.h>
#include <qobjectdefs.h>

class LLMConsole : public QDialog
{
    Q_OBJECT

  public:
    explicit LLMConsole(QWidget* parent = nullptr);

  private slots:
    void sendMessage(const QString& inputMessage);
};