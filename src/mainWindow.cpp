#include <QAction>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTextBrowser>
#include <QVBoxLayout>

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

    auto onOffAction = new QAction(tr(" Record"), this);
    onOffAction->setCheckable(true);
    onOffAction->setChecked(true);
    this->menuBar()->addAction(onOffAction);

    connect(onOffAction, &QAction::toggled, this,
            [onOffAction](bool checked)
            {
                if (checked)
                {
                    commandManager::getInstance()->startRecording();
                    onOffAction->setText(tr(" Recording"));
                }
                else
                {
                    commandManager::getInstance()->stopRecording();
                    onOffAction->setText(tr(" Record"));
                }
            });

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
        auto hlayout1 = new QHBoxLayout;
        hlayout1->setContentsMargins(0, 0, 0, 0);

        auto idEdit = new QSpinBox(this);
        auto amountEdit = new QSpinBox(this);
        auto priceEdit = new QDoubleSpinBox(this);

        hlayout1->addWidget(new QLabel(tr("ID: "), this));
        hlayout1->addWidget(idEdit);
        hlayout1->addWidget(new QLabel(tr("Amount: "), this));
        hlayout1->addWidget(amountEdit);
        hlayout1->addWidget(new QLabel(tr("Price: "), this));
        hlayout1->addWidget(priceEdit);

        auto hlayout2 = new QHBoxLayout;
        hlayout2->setContentsMargins(0, 0, 0, 0);

        auto addOrderButton = new QPushButton(tr("Add Order"), this);
        auto removeOrderButton = new QPushButton(tr("Remove Order"), this);
        auto updateOrderButton = new QPushButton(tr("Update Order"), this);

        connect(addOrderButton, &QPushButton::clicked, this,
                [idEdit, amountEdit, priceEdit]()
                {
                    auto newOrder =
                        std::make_shared<order>(order{idEdit->value(), amountEdit->value(), priceEdit->value()});

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

        hlayout2->addWidget(addOrderButton);
        hlayout2->addWidget(removeOrderButton);
        hlayout2->addWidget(updateOrderButton);

        layout->addLayout(hlayout1);
        layout->addLayout(hlayout2);
    }

    {
        auto console = new pythonConsole(this);
        connect(undoAction, &QAction::triggered, this, [console]() { console->onMessagePassedIn(tr("Undo.")); });
        connect(redoAction, &QAction::triggered, this, [console]() { console->onMessagePassedIn(tr("Redo.")); });
        connect(model_.get(), &dataModel::messageEmerged, console, &pythonConsole::onMessagePassedIn);

        layout->addWidget(console);
    }

    {
        auto recordingBrowser = new QTextBrowser(this);

        connect(cmdManager, &commandManager::recordingInserted, this,
                [recordingBrowser](const QString& recording) { recordingBrowser->append(recording); });

        connect(cmdManager, &commandManager::recordingStarted, this,
                [recordingBrowser]() { recordingBrowser->clear(); });

        layout->addWidget(recordingBrowser);
    }

    layout->setStretch(0, 2);
    layout->setStretch(1, 1);

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
