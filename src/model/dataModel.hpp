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

    bool addOrder(const std::shared_ptr<order>& o);

    bool removeOrder(int id);

    bool updateOrder(const int& id, const int& amount, const double& price);

    [[nodiscard]] std::shared_ptr<order> getOrder(const int& id) const
    {
        if (orders_.count(id) > 0)
        {
            return orders_.at(id);
        }

        return nullptr;
    };

    [[nodiscard]] std::map<int, std::shared_ptr<order>> orders() const
    {
        return orders_;
    };

  signals:
    void dataChanged();
    void messageEmerged(const QString& message);

  private:
    std::shared_ptr<root> root_;
    std::map<int, std::shared_ptr<order>> orders_;
};
