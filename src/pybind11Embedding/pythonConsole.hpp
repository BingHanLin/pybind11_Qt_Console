#pragma once

#include <QCompleter>
#include <QKeyEvent>
#include <QWidget>
#include <qtextedit.h>

class pythonCommandLine;

class pythonConsole : public QWidget
{
    Q_OBJECT

  public:
    explicit pythonConsole(QWidget* parent = nullptr);

  private:
    QTextEdit* outputTextEdit_;

  private slots:
    void onCommandInserted(const QString& commands);
    void onCommandParseError(const QString& message);
};
