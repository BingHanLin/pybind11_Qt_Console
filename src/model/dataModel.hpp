#pragma once

#include <vector>

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

    void addOrder(const order& o);

    std::vector<order> orders() const
    {
        return orders_;
    };

   signals:
    void dataChanged();

   private:
    std::vector<order> orders_;
};
