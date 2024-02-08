#include <QAction>
#include <QMenuBar>
#include <QVBoxLayout>

#include "commandManager.hpp"
#include "mainWindow.hpp"
#include "pythonCommands.hpp"
#include "pythonConsole.hpp"

mainWindow::mainWindow(QWidget* parent) : QMainWindow(parent)
{
    auto cmdManager = commandManager::getInstance();

    auto undoAction = cmdManager->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setIcon(QIcon(":/icons/undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    auto redoAction = cmdManager->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setIcon(QIcon(":/icons/redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

    this->menuBar()->addAction(undoAction);
    this->menuBar()->addAction(redoAction);

    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout;

    widget->setLayout(layout);
    this->setCentralWidget(widget);

    model_ = std::make_shared<dataModel>();

    pythonCommands::setDataModel(model_);

    {
        table_ = new QTableWidget(this);
        table_->setAlternatingRowColors(true);

        QStringList headers;
        headers << tr("ID") << tr("Amount") << tr("Price");
        table_->setColumnCount(headers.size());
        table_->setHorizontalHeaderLabels(headers);
        table_->resizeRowsToContents();

        layout->addWidget(table_);
    }

    {
        layout->addWidget(new pythonConsole(this));
    }

    layout->setStretch(0, 2);
    layout->setStretch(1, 1);

    connect(model_.get(), &dataModel::dataChanged, this, &mainWindow::onDataChanged);
}

void mainWindow::onDataChanged()
{
    const auto orders = model_->orders();

    table_->clearContents();
    table_->setRowCount((int)orders.size());

    int counter = 0;
    for (const auto& [id, order] : orders)
    {
        table_->setItem(counter, 0, new QTableWidgetItem(QString::number(order->id_)));
        table_->setItem(counter, 1, new QTableWidgetItem(QString::number(order->amount_)));
        table_->setItem(counter, 2, new QTableWidgetItem(QString::number(order->price_)));

        counter++;
    }
}
