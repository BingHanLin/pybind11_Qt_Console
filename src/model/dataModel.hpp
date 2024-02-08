#pragma once

#include <map>
#include <memory>

#include <QObject>

struct order
{
    int id_;
    int amount_;
    double price_;
};

class dataModel : public QObject
{
    Q_OBJECT
  public:
    dataModel() = default;
    ~dataModel() override = default;

    void addOrder(const std::shared_ptr<order>& o);

    void removeOrder(int id);

    void updateOrder(const int& id, const int& amount, const double& price);

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

  private:
    std::map<int, std::shared_ptr<order>> orders_;
};
