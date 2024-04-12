
#include <string>

#include <QAction>

#include "commandManager.hpp"
#include "commands.hpp"

std::string commandManager::undoScript_;
std::string commandManager::redoScript_;

commandManager* commandManager::getInstance()
{
    static commandManager instance;
    return &instance;
}

void commandManager::setUndoScript(const std::string& script)
{
    undoScript_ = script;
}

void commandManager::setRedoScript(const std::string& script)
{
    redoScript_ = script;
}

commandManager::commandManager() : commandStack_(new QUndoStack), isRecording_(false), recordingList_()
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
    auto action = new QAction(parent);
    action->setEnabled(commandStack_->canUndo());

    connect(commandStack_, &QUndoStack::canUndoChanged, action, &QAction::setEnabled);
    connect(action, &QAction::triggered, this, &commandManager::undo);

    return action;
}

QAction* commandManager::createRedoAction(QObject* parent, const QString& prefix) const
{
    auto action = new QAction(parent);
    action->setEnabled(commandStack_->canRedo());

    connect(commandStack_, &QUndoStack::canRedoChanged, action, &QAction::setEnabled);
    connect(action, &QAction::triggered, this, &commandManager::redo);

    return action;
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
    if (this->isRecoring())
    {
        this->insertRecording(QString::fromStdString(redoScript_));
    }
}

void commandManager::undo()
{
    commandStack_->undo();
    if (this->isRecoring())
    {
        this->insertRecording(QString::fromStdString(undoScript_));
    }
}

void commandManager::startRecording()
{
    isRecording_ = true;
    recordingList_.clear();

    emit recordingStarted();
}

void commandManager::stopRecording()
{
    isRecording_ = false;

    emit recordingStopped();
}

bool commandManager::isRecoring() const
{
    return isRecording_;
}

void commandManager::insertRecording(const QString& record)
{
    recordingList_.append(record);

    emit recordingInserted(record);
}

[[nodiscard]] QStringList commandManager::getRecordings() const
{
    return recordingList_;
}
