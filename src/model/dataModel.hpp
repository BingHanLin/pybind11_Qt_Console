#pragma once

#include <map>
#include <memory>

#include <QObject>

#include "order.hpp"
#include "root.hpp"

class dataModel : public QObject
{
    Q_OBJECT
  public:
    dataModel();
    ~dataModel() override = default;

    [[nodiscard]] std::shared_ptr<root> getRoot() const
    {
        return root_;
    }

  signals:
    void dataChanged();
    void messageEmerged(const QString& message);

  private:
    std::shared_ptr<root> root_;
};
