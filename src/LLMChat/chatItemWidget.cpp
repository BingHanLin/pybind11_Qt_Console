#include <QTime>

#include "chatItemWidget.hpp"
#include "ui_chatItemWidget.h"

chatItemWidget::chatItemWidget(QWidget* parent) : QWidget(parent), ui(new Ui::chatItemWidget)
{
    ui->setupUi(this);
}

chatItemWidget::~chatItemWidget()
{
    delete ui;
}

void chatItemWidget::setMessage(const QString& message, const chatItemRole& role)
{
    if (chatItemRole::ASSISTANT == role)
    {
        ui->lblMessage->setAlignment(Qt::AlignLeft);
        ui->lblMessage->setAlignment(Qt::AlignLeft);
        ui->lblMessage->setText(QString(tr("AI\n%1").arg(message)));
    }
    else if (chatItemRole::USER == role)
    {
        ui->lblMessage->setAlignment(Qt::AlignRight);
        ui->lblMessage->setAlignment(Qt::AlignRight);
        ui->lblMessage->setText(QString(tr("You\n%1").arg(message)));
    }

    ui->lblTime->setText(QTime::currentTime().toString("HH:mm"));
}
