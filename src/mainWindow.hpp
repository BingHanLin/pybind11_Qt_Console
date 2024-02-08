#pragma once

#include <QMainWindow>
#include <QObject>
#include <QTableWidget>

#include "dataModel.hpp"
#include "pythonInterpreter.hpp"

class mainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    mainWindow(QWidget* parent = nullptr);

  private:
    std::shared_ptr<pythonInterpreter> interpreter_;
    std::shared_ptr<dataModel> model_;
    QTableWidget* table_;

  private slots:
    void onDataChanged();
};
