#include <QIcon>
#include <QTime>

#include "chatItemWidget.hpp"
#include "ui_chatItemWidget.h"

chatItemWidget::chatItemWidget(QWidget* parent) : QWidget(parent), ui(new Ui::chatItemWidget)
{
    ui->setupUi(this);
    ui->messageLabel->setWordWrap(true);
}

chatItemWidget::~chatItemWidget()
{
    delete ui;
}

void chatItemWidget::setMessage(const QString& message, const chatItemRole& role)
{
    if (chatItemRole::ASSISTANT == role)
    {
        ui->timeLabel->setAlignment(Qt::AlignLeft);
        ui->messageLabel->setAlignment(Qt::AlignLeft);
        ui->avatorLabel->setAlignment(Qt::AlignLeft);

        QPixmap pixmap(":/icons/ai");  // Load your image
        ui->avatorLabel->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // Set background color
        // this->setStyleSheet("chatItemWidget{background-color: rgb(199, 252, 255);}");
    }
    else if (chatItemRole::USER == role)
    {
        ui->timeLabel->setAlignment(Qt::AlignRight);
        ui->messageLabel->setAlignment(Qt::AlignRight);
        ui->avatorLabel->setAlignment(Qt::AlignRight);

        QPixmap pixmap(":/icons/user");  // Load your image
        ui->avatorLabel->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    ui->messageLabel->setText(message);
    ui->timeLabel->setText(QTime::currentTime().toString("HH:mm"));

    this->adjustSize();
}
