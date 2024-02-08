#pragma once

#include <QCompleter>
#include <QKeyEvent>
#include <QLineEdit>
#include <QRegularExpression>

#include "pythonInterpreter.hpp"

class pythonCommandLine : public QLineEdit
{
    Q_OBJECT

  public:
    explicit pythonCommandLine(std::shared_ptr<pythonInterpreter> interpreter, QWidget *parent = nullptr);

  protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *e) override;

  private:
    std::shared_ptr<pythonInterpreter> interpreter_;
    QCompleter *completer_;

    [[nodiscard]] QString textUnderCursor() const;
    void updateCompleter();

  private slots:
    void onCompletionActivated(const QString &completion);
};

inline QString extractLastWord(const QString &inputString)
{
    QStringList words = inputString.split(".");

    if (!words.isEmpty())
    {
        return words.last();
    }
    else
    {
        return {};
    }
}
