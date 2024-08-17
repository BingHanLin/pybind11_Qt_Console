#pragma once

#include "chatItemWidget.hpp"
#include <QWidget>

namespace Ui
{
class chatItemWidget;
}

enum class chatItemRole
{
    ASSISTANT,
    USER
};

class chatItemWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit chatItemWidget(QWidget *parent = nullptr);
    ~chatItemWidget() override;

    void setMessage(const QString &message, const chatItemRole &role);

  private:
    Ui::chatItemWidget *ui;
};
