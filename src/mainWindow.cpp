#include <QAction>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTextBrowser>
#include <QToolButton>
#include <QVBoxLayout>
#include <qlineedit.h>
#include <qnamespace.h>

#include "commandManager.hpp"
#include "commands.hpp"
#include "dataModel.hpp"
#include "mainWindow.hpp"
#include "pythonCommands.hpp"
#include "pythonConsole.hpp"

mainWindow::mainWindow(QWidget* parent) : QMainWindow(parent)
{
    this->setWindowTitle(tr("pybind11 Qt Console"));

    auto cmdManager = commandManager::getInstance();
    cmdManager->setMaxCommandNumber(1);

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

    auto layout = new QVBoxLayout;
    layout->setSpacing(15);

    auto widget = new QWidget(this);
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
        auto formLayout = new QFormLayout;
        formLayout->setContentsMargins(0, 0, 0, 0);

        auto idEdit = new QSpinBox(this);
        auto amountEdit = new QSpinBox(this);
        auto priceEdit = new QDoubleSpinBox(this);

        formLayout->addRow(new QLabel(tr("ID: ")), idEdit);
        formLayout->addRow(new QLabel(tr("Amount: ")), amountEdit);
        formLayout->addRow(new QLabel(tr("Price: ")), priceEdit);

        auto hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0, 0, 0, 0);

        auto addOrderButton = new QPushButton(tr("Add Order"), this);
        auto removeOrderButton = new QPushButton(tr("Remove Order"), this);
        auto updateOrderButton = new QPushButton(tr("Update Order"), this);

        connect(addOrderButton, &QPushButton::clicked, this,
                [idEdit, amountEdit, priceEdit]()
                {
                    auto newOrder = std::make_shared<order>(idEdit->value(), amountEdit->value(), priceEdit->value());

                    auto command = new addCommand(pythonCommands::model_, newOrder);

                    auto cmdManager = commandManager::getInstance();
                    cmdManager->runCommand(command);
                });

        connect(removeOrderButton, &QPushButton::clicked, this,
                [idEdit]()
                {
                    auto command = new removeCommand(pythonCommands::model_, idEdit->value());

                    auto cmdManager = commandManager::getInstance();
                    cmdManager->runCommand(command);
                });

        connect(updateOrderButton, &QPushButton::clicked, this,
                [idEdit, amountEdit, priceEdit]()
                {
                    auto command = new updateCommand(pythonCommands::model_, idEdit->value(), amountEdit->value(),
                                                     priceEdit->value());

                    auto cmdManager = commandManager::getInstance();
                    cmdManager->runCommand(command);
                });

        hlayout->addWidget(addOrderButton);
        hlayout->addWidget(removeOrderButton);
        hlayout->addWidget(updateOrderButton);
        hlayout->addStretch();

        auto groupBox = new QGroupBox(tr("Order Operations"), this);
        auto groupBoxLayout = new QVBoxLayout(groupBox);

        groupBoxLayout->addLayout(formLayout);
        groupBoxLayout->addLayout(hlayout);

        layout->addWidget(groupBox);
    }

    {
        auto console = new pythonConsole(this);
        connect(undoAction, &QAction::triggered, this, [console]() { console->onMessagePassedIn(tr("Undo.")); });
        connect(redoAction, &QAction::triggered, this, [console]() { console->onMessagePassedIn(tr("Redo.")); });
        connect(model_.get(), &dataModel::messageEmerged, console, &pythonConsole::onMessagePassedIn);

        auto dockWidget = new QDockWidget(tr("Python Console"), this);
        dockWidget->setWidget(console);
        this->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
    }

    {
        auto recordWidget = new QWidget(this);
        auto recordWidgetLayout = new QVBoxLayout(recordWidget);
        recordWidgetLayout->setContentsMargins(9, 9, 9, 9);

        auto recordBtn = new QToolButton(this);
        recordBtn->setText(tr("Start Record"));
        recordBtn->setCheckable(true);
        recordBtn->setChecked(false);
        recordWidgetLayout->addWidget(recordBtn);

        auto recordingBrowser = new QTextBrowser(this);
        recordWidgetLayout->addWidget(recordingBrowser);

        connect(recordBtn, &QToolButton::toggled, this,
                [recordBtn, recordingBrowser](bool checked)
                {
                    if (checked)
                    {
                        commandManager::getInstance()->startRecording();
                        recordBtn->setText(tr("Stop Record"));
                    }
                    else
                    {
                        commandManager::getInstance()->stopRecording();
                        recordBtn->setText(tr("Start Record"));
                    }
                });

        connect(cmdManager, &commandManager::recordingInserted, this,
                [recordingBrowser](const QString& recording) { recordingBrowser->append(recording); });

        connect(cmdManager, &commandManager::recordingStarted, this,
                [recordingBrowser]() { recordingBrowser->append(tr("=== Recording Started ===")); });

        connect(cmdManager, &commandManager::recordingStopped, this,
                [recordingBrowser]() { recordingBrowser->append(tr("=== Recording Stopped ===")); });

        auto dockWidget = new QDockWidget(tr("Recording Browser"), this);
        dockWidget->setWidget(recordWidget);
        this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
    }

    connect(model_.get(), &dataModel::dataChanged, this, &mainWindow::onDataChanged);

    this->onDataChanged();
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
