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
    explicit addCommand(std::shared_ptr<dataModel> model, const order& newOrder)
        : baseCommand(model), newOrder_(newOrder)
    {
    }

    virtual ~addCommand(){};

    virtual void execute()
    {
        model_->addOrder(newOrder_);
    };

   private:
    order newOrder_;
};
