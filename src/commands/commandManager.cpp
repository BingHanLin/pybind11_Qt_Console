
#include "commandManager.hpp"
#include "commands.hpp"

commandManager* commandManager::getInstance()
{
    static commandManager instance;
    return &instance;
}

commandManager::commandManager() : commandStack_(new QUndoStack)
{
}

commandManager::~commandManager()
{
    delete commandStack_;
}

void commandManager::clearCommands()
{
    commandStack_->clear();
}

void commandManager::runCommand(baseCommand* cmd)
{
    commandStack_->push(cmd);
}

void commandManager::setMaxCommandNumber(const int& number)
{
    commandStack_->setUndoLimit(number);
}

int commandManager::getMaxCommandNumber() const
{
    return commandStack_->undoLimit();
}

QAction* commandManager::createUndoAction(QObject* parent, const QString& prefix) const
{
    return commandStack_->createUndoAction(parent, prefix);
}

QAction* commandManager::createRedoAction(QObject* parent, const QString& prefix) const
{
    return commandStack_->createRedoAction(parent, prefix);
}

bool commandManager::canRedo() const
{
    return commandStack_->canRedo();
}

bool commandManager::canUndo() const
{
    return commandStack_->canUndo();
}

void commandManager::redo()
{
    commandStack_->redo();
}

void commandManager::undo()
{
    commandStack_->undo();
}
