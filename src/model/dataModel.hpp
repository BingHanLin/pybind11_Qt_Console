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
    dataModel();
    ~dataModel();

    void addOrder(const std::shared_ptr<order> o);

    void removeOrder(const int id);

    void updateOrder(const int id, const int amount, const double price);

    std::map<int, std::shared_ptr<order>> orders() const
    {
        return orders_;
    };

   signals:
    void dataChanged();

   private:
    std::map<int, std::shared_ptr<order>> orders_;
};
