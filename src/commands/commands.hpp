#pragma once

#include <memory>
#include <utility>

#include "dataModel.hpp"

class baseCommand
{
  public:
    explicit baseCommand(std::shared_ptr<dataModel> model) : model_(std::move(model)){};
    virtual ~baseCommand() = default;

    virtual void execute() = 0;

  protected:
    std::shared_ptr<dataModel> model_;
};

class addCommand : public baseCommand
{
  public:
    explicit addCommand(std::shared_ptr<dataModel> model, std::shared_ptr<order> newOrder)
        : baseCommand(std::move(model)), newOrder_(std::move(newOrder))
    {
    }

    ~addCommand() override = default;

    void execute() override
    {
        model_->addOrder(newOrder_);
    };

  private:
    std::shared_ptr<order> newOrder_;
};

class removeCommand : public baseCommand
{
  public:
    explicit removeCommand(std::shared_ptr<dataModel> model, const int id) : baseCommand(model), id_(id)
    {
    }

    ~removeCommand() override = default;

    void execute() override
    {
        model_->removeOrder(id_);
    };

  private:
    const int id_;
};

class updateCommand : public baseCommand
{
  public:
    explicit updateCommand(std::shared_ptr<dataModel> model, const int id, const int amount, const double price)
        : baseCommand(std::move(model)), id_(id), newAmount_(amount), newPrice_(price)
    {
    }

    ~updateCommand() override = default;

    void execute() override
    {
        model_->updateOrder(id_, newAmount_, newPrice_);
    };

  private:
    const int id_;
    const int newAmount_;
    const double newPrice_;
};
