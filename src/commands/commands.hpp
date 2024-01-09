#pragma once

#include <memory>

#include "dataModel.hpp"

class baseCommand
{
   public:
    explicit baseCommand(std::shared_ptr<dataModel> model) : model_(model){};
    virtual ~baseCommand(){};

    virtual void execute() = 0;

   protected:
    std::shared_ptr<dataModel> model_;
};

class addCommand : public baseCommand
{
   public:
    explicit addCommand(std::shared_ptr<dataModel> model,
                        std::shared_ptr<order> newOrder)
        : baseCommand(model), newOrder_(newOrder)
    {
    }

    virtual ~addCommand(){};

    virtual void execute()
    {
        model_->addOrder(newOrder_);
    };

   private:
    std::shared_ptr<order> newOrder_;
};

class removeCommand : public baseCommand
{
   public:
    explicit removeCommand(std::shared_ptr<dataModel> model, const int id)
        : baseCommand(model), id_(id)
    {
    }

    virtual ~removeCommand(){};

    virtual void execute()
    {
        model_->removeOrder(id_);
    };

   private:
    const int id_;
};

class updateCommand : public baseCommand
{
   public:
    explicit updateCommand(std::shared_ptr<dataModel> model, const int id,
                           const int amount, const double price)
        : baseCommand(model), id_(id), newAmount_(amount), newPrice_(price)
    {
    }

    virtual ~updateCommand(){};

    virtual void execute()
    {
        model_->updateOrder(id_, newAmount_, newPrice_);
    };

   private:
    const int id_;
    const int newAmount_;
    const double newPrice_;
};
