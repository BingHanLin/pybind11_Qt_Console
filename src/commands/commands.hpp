#pragma once

#include <memory>
#include <utility>

#include <QUndoCommand>

#include "dataModel.hpp"

class baseCommand : public QUndoCommand
{
  public:
    explicit baseCommand(std::shared_ptr<dataModel> model, const QString &text, QUndoCommand *parent = nullptr)
        : QUndoCommand(text, parent), model_(std::move(model)){};

    ~baseCommand() override = default;

  protected:
    std::shared_ptr<dataModel> model_;
};

class addCommand : public baseCommand
{
  public:
    explicit addCommand(std::shared_ptr<dataModel> model, std::shared_ptr<order> newOrder)
        : baseCommand(std::move(model), QObject::tr("Add Order")), newOrder_(std::move(newOrder))
    {
    }

    ~addCommand() override = default;

    void redo() override
    {
        model_->addOrder(newOrder_);
    }

    void undo() override
    {
        model_->removeOrder(newOrder_->id_);
    }

  private:
    std::shared_ptr<order> newOrder_;
};

class removeCommand : public baseCommand
{
  public:
    explicit removeCommand(std::shared_ptr<dataModel> model, const int id)
        : baseCommand(std::move(model), QObject::tr("Remove Order")), id_(id), removedOrder_(nullptr)
    {
    }

    ~removeCommand() override = default;

    void redo() override
    {
        removedOrder_ = model_->getOrder(id_);
        model_->removeOrder(id_);
    }

    void undo() override
    {
        model_->addOrder(removedOrder_);
    }

  private:
    const int id_;
    std::shared_ptr<order> removedOrder_;
};

class updateCommand : public baseCommand
{
  public:
    explicit updateCommand(std::shared_ptr<dataModel> model, const int id, const int amount, const double price)
        : baseCommand(std::move(model), QObject::tr("Update Order")), id_(id), newAmount_(amount), newPrice_(price)
    {
    }

    ~updateCommand() override = default;

    void redo() override
    {
        auto oldOrder = model_->getOrder(id_);
        oldAmount_ = oldOrder->amount_;
        oldPrice_ = oldOrder->price_;

        model_->updateOrder(id_, newAmount_, newPrice_);
    }

    void undo() override
    {
        model_->updateOrder(id_, oldAmount_, oldPrice_);
    }

  private:
    const int id_;
    const int newAmount_;
    const double newPrice_;

    int oldAmount_{};
    double oldPrice_{};
};
