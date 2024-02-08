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
    explicit pythonCommandLine(QWidget *parent = nullptr);

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
    // Define a regular expression pattern to match words separated by specified characters
    QRegularExpression re(R"(["~!@#$%^&*()_+{}|:\"<>?,./;'\[\]\\-=]+)");

    // Split the input string by the regular expression pattern
    QStringList words = inputString.split(re, Qt::SkipEmptyParts);

    // Return the last word
    if (!words.isEmpty())
        return words.last();
    else
        return {};  // Return an empty string if no words found
}
