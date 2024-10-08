#pragma once

#include <QMainWindow>
#include <QObject>
#include <QTreeWidget>

#include "dataModel.hpp"
#include "pythonInterpreter.hpp"

class mainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    mainWindow(QWidget* parent = nullptr);

  private:
    std::shared_ptr<dataModel> model_;
    QTreeWidget* tree_;
    QDockWidget* pyConsoleDockWidget_;
    QDockWidget* recordingBrowserWidget_;

    void setDefaultLayout();

  private slots:
    void onDataChanged();
    void onShowLLMChat();
};
