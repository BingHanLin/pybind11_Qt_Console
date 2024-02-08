#pragma once

#include <QUndoCommand>
#include <QUndoStack>

class baseCommand;

class commandManager
{
  public:
    static commandManager* getInstance();

    void operator=(commandManager const&) = delete;
    commandManager(commandManager const&) = delete;

    void clearCommands();

    void runCommand(baseCommand* cmd);

    void setMaxCommandNumber(const int& number);
    [[nodiscard]] int getMaxCommandNumber() const;

    QAction* createUndoAction(QObject* parent, const QString& prefix = QString()) const;
    QAction* createRedoAction(QObject* parent, const QString& prefix = QString()) const;

    [[nodiscard]] bool canRedo() const;
    [[nodiscard]] bool canUndo() const;

  public slots:
    void redo();
    void undo();

  private:
    commandManager();
    ~commandManager();

    QUndoStack* commandStack_ = nullptr;
};
