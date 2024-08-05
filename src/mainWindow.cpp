#include <QAction>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTextBrowser>
#include <QToolButton>
#include <QVBoxLayout>
#include <memory>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qvariant.h>

#include "commandManager.hpp"
#include "commands.hpp"
#include "dataModel.hpp"
#include "group.hpp"
#include "mainWindow.hpp"
#include "order.hpp"
#include "pythonCommands.hpp"
#include "pythonConsole.hpp"

Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr)
Q_DECLARE_METATYPE(std::shared_ptr<group>)
Q_DECLARE_METATYPE(std::shared_ptr<order>)

mainWindow::mainWindow(QWidget* parent) : QMainWindow(parent)
{
    this->setWindowTitle(tr("pybind11 Qt Console"));

    auto cmdManager = commandManager::getInstance();
    cmdManager->setMaxCommandNumber(10);

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
        tree_ = new QTreeWidget(this);
        tree_->setAlternatingRowColors(true);

        QStringList headers;
        headers << tr("Object") << tr("Amount") << tr("Price");
        tree_->setHeaderLabels(headers);
        tree_->setColumnCount(headers.size());

        layout->addWidget(tree_);
    }

    {
        auto formLayout = new QFormLayout;
        formLayout->setContentsMargins(0, 0, 0, 0);

        auto amountEdit = new QSpinBox(this);
        auto priceEdit = new QDoubleSpinBox(this);

        formLayout->addRow(new QLabel(tr("Amount: ")), amountEdit);
        formLayout->addRow(new QLabel(tr("Price: ")), priceEdit);

        auto hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0, 0, 0, 0);

        auto addOrderButton = new QPushButton(tr("Add Order"), this);
        auto removeOrderButton = new QPushButton(tr("Remove Order"), this);
        auto updateOrderButton = new QPushButton(tr("Update Order"), this);

        connect(addOrderButton, &QPushButton::clicked, this,
                [this, amountEdit, priceEdit]()
                {
                    if (tree_->currentItem() == nullptr)
                    {
                        return;
                    }

                    const auto oneGroup = tree_->currentItem()->data(0, Qt::UserRole).value<std::shared_ptr<group>>();

                    if (oneGroup != nullptr)
                    {
                        auto newOrder = std::make_shared<order>("New Order", amountEdit->value(), priceEdit->value(),
                                                                oneGroup.get());

                        auto command = new addOrderCommand(pythonCommands::model_, oneGroup, newOrder);

                        auto cmdManager = commandManager::getInstance();
                        cmdManager->runCommand(command);
                    }
                    else
                    {
                        QMessageBox::warning(this, tr("Warning"), tr("Please select an group to add."));
                    }
                });

        connect(removeOrderButton, &QPushButton::clicked, this,
                [this]()
                {
                    if (tree_->currentItem() == nullptr)
                    {
                        return;
                    }

                    const auto oneOrder = tree_->currentItem()->data(0, Qt::UserRole).value<std::shared_ptr<order>>();

                    if (oneOrder != nullptr)
                    {
                        const auto oneGroup =
                            tree_->currentItem()->parent()->data(0, Qt::UserRole).value<std::shared_ptr<group>>();

                        auto command = new removeOrderCommand(model_, oneGroup, oneOrder);

                        auto cmdManager = commandManager::getInstance();
                        cmdManager->runCommand(command);
                    }
                    else
                    {
                        QMessageBox::warning(this, tr("Warning"), tr("Please select an order to remove."));
                    }
                });

        connect(updateOrderButton, &QPushButton::clicked, this,
                [this, amountEdit, priceEdit]()
                {
                    if (tree_->currentItem() == nullptr)
                    {
                        return;
                    }

                    const auto oneOrder = tree_->currentItem()->data(0, Qt::UserRole).value<std::shared_ptr<order>>();

                    if (oneOrder != nullptr)
                    {
                        auto command =
                            new updateOrderCommand(model_, oneOrder, amountEdit->value(), priceEdit->value());

                        auto cmdManager = commandManager::getInstance();
                        cmdManager->runCommand(command);
                    }
                    else
                    {
                        QMessageBox::warning(this, tr("Warning"), tr("Please select an order to update."));
                    }
                });

        addOrderButton->setEnabled(false);
        removeOrderButton->setEnabled(false);
        updateOrderButton->setEnabled(false);

        hlayout->addWidget(addOrderButton);
        hlayout->addWidget(removeOrderButton);
        hlayout->addWidget(updateOrderButton);
        hlayout->addStretch();

        auto groupBox = new QGroupBox(tr("Order Operations"), this);
        auto groupBoxLayout = new QVBoxLayout(groupBox);

        groupBoxLayout->addLayout(formLayout);
        groupBoxLayout->addLayout(hlayout);

        layout->addWidget(groupBox);

        connect(tree_, &QTreeWidget::currentItemChanged, this,
                [this, addOrderButton, removeOrderButton, updateOrderButton](QTreeWidgetItem* current)
                {
                    if (current == nullptr)
                    {
                        return;
                    }

                    const auto oneVariaet = current->data(0, Qt::UserRole);

                    addOrderButton->setEnabled(false);
                    removeOrderButton->setEnabled(false);
                    updateOrderButton->setEnabled(false);

                    if (oneVariaet.canConvert<std::shared_ptr<order>>())
                    {
                        removeOrderButton->setEnabled(true);
                        updateOrderButton->setEnabled(true);
                    }
                    else if ((oneVariaet.canConvert<std::shared_ptr<group>>()))
                    {
                        addOrderButton->setEnabled(true);
                    }
                });
    }

    {
        auto console = new pythonConsole(this);
        // connect(undoAction, &QAction::triggered, this, [console]() { console->onMessagePassedIn(tr("Undo.")); });
        // connect(redoAction, &QAction::triggered, this, [console]() { console->onMessagePassedIn(tr("Redo.")); });
        connect(model_.get(), &dataModel::messageEmerged, console, &pythonConsole::onMessagePassedIn);

        {
            auto dockWidget = new QDockWidget(tr("Python Console"), this);
            dockWidget->setWidget(console);
            this->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
        }

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
                [console, recordBtn, recordingBrowser](bool checked)
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
                        recordingBrowser->setStyleSheet("");
                    }

                    console->setConsoleEnabled(!checked);
                });

        connect(cmdManager, &commandManager::recordingInserted, this,
                [recordingBrowser](const QString& recording) { recordingBrowser->append(recording); });

        connect(cmdManager, &commandManager::recordingStarted, this,
                [recordingBrowser]() { recordingBrowser->append(tr("# === Recording Started ===")); });

        connect(cmdManager, &commandManager::recordingStopped, this,
                [recordingBrowser]() { recordingBrowser->append(tr("# === Recording Stopped ===")); });

        {
            auto dockWidget = new QDockWidget(tr("Recording Browser"), this);
            dockWidget->setWidget(recordWidget);
            this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
        }
    }

    connect(model_.get(), &dataModel::dataChanged, this, &mainWindow::onDataChanged);

    this->onDataChanged();
}

void mainWindow::onDataChanged()
{
    tree_->clear();

    int counter = 0;
    const auto root = model_->getRoot();
    for (const auto& oneGroupObject : root->getChildren())
    {
        auto groupItem = new QTreeWidgetItem(tree_, QStringList() << QString::fromStdString(oneGroupObject->getName()));
        auto oneGroup = std::dynamic_pointer_cast<group>(oneGroupObject);
        groupItem->setData(0, Qt::UserRole, QVariant::fromValue(oneGroup));

        tree_->addTopLevelItem(groupItem);

        for (const auto& oneOrderObject : oneGroup->getChildren())
        {
            auto oneOrder = std::dynamic_pointer_cast<order>(oneOrderObject);

            auto orderItem = new QTreeWidgetItem(groupItem, QStringList() << QString::fromStdString(oneOrder->getName())
                                                                          << QString::number(oneOrder->getAmount())
                                                                          << QString::number(oneOrder->getPrice()));
            orderItem->setData(0, Qt::UserRole, QVariant::fromValue(oneOrder));

            groupItem->addChild(orderItem);
        }
    }

    tree_->expandAll();
}
