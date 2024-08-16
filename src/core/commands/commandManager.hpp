#pragma once

#include <QObject>
#include <QUndoCommand>
#include <QUndoStack>

class baseCommand;

class commandManager : public QObject
{
    Q_OBJECT

  public:
    static commandManager* getInstance();
    static void setUndoScript(const std::string& script);
    static void setRedoScript(const std::string& script);

  private:
    static std::string undoScript_;
    static std::string redoScript_;

  public:
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

    void startRecording();
    void stopRecording();
    [[nodiscard]] bool isRecoring() const;
    void insertRecording(const QString& record);
    [[nodiscard]] QStringList getRecordings() const;

  public slots:
    void redo();
    void undo();

  signals:
    void recordingStarted();
    void recordingStopped();
    void recordingInserted(const QString& record);

  private:
    commandManager();
    ~commandManager() override;

    QUndoStack* commandStack_ = nullptr;
    bool isRecording_;
    QStringList recordingList_;
};
