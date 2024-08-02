#pragma once

#include <QCompleter>
#include <QKeyEvent>
#include <QWidget>
#include <memory>
#include <qtextedit.h>

class pythonCommandLine;
class pythonConsole : public QWidget
{
    Q_OBJECT

  public:
    explicit pythonConsole(QWidget* parent = nullptr);

    void setConsoleEnabled(bool enabled);

  public slots:
    void onMessagePassedIn(const QString& message);

  private:
    QTextEdit* outputTextEdit_;
    pythonCommandLine* commandLine_;

  private slots:
    void onCommandInserted(const QString& commands);
    void onCommandStdOutput(const QString& outputs);
    void onCommandParseError(const QString& message);
};
