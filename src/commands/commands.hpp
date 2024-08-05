#pragma once

#include <memory>
#include <utility>

#include <QUndoCommand>

#include "commandManager.hpp"
#include "commandUtil.hpp"
#include "dataModel.hpp"
#include "group.hpp"
#include "order.hpp"

class baseCommand : public QUndoCommand
{
  public:
    explicit baseCommand(std::shared_ptr<dataModel> model, const QString &text, QUndoCommand *parent = nullptr)
        : QUndoCommand(text, parent), model_(std::move(model)), isFirstTime_(true){};

    ~baseCommand() override = default;

  protected:
    std::shared_ptr<dataModel> model_;
    bool isFirstTime_;
};

class addOrderCommand : public baseCommand
{
  public:
    using scriptCallbackType = std::function<std::string(const std::shared_ptr<group> &, int, double)>;
    static void setScriptCallback(scriptCallbackType callback)
    {
        scriptCallback_ = std::move(callback);
    }

  private:
    // static std::function<std::string(std::string, int, int, int)> scriptCallback_;
    static scriptCallbackType scriptCallback_;

  public:
    explicit addOrderCommand(std::shared_ptr<dataModel> model, const std::shared_ptr<group> &oneGroup,
                             const std::shared_ptr<order> &newOrder)
        : baseCommand(std::move(model), QObject::tr("Add Order")), group_(oneGroup), newOrder_(newOrder)
    {
        const auto orders = group_->getChildren();
        std::map<std::string, std::shared_ptr<order>> ordersMap;
        for (const auto &oneOrder : orders)
        {
            ordersMap[oneOrder->getName()] = std::dynamic_pointer_cast<order>(oneOrder);
        }

        const auto uniqueName = generateUniqueName(newOrder_->getName(), ordersMap);
        newOrder_->setName(uniqueName);
    }

    ~addOrderCommand() override = default;

    void redo() override
    {
        group_->addOrder(newOrder_);
        emit model_->dataChanged();

        if (isFirstTime_ && commandManager::getInstance()->isRecoring())
        {
            const auto record = scriptCallback_(group_, newOrder_->amount_, newOrder_->price_);
            commandManager::getInstance()->insertRecording(QString::fromStdString(record));

            isFirstTime_ = false;
        }
    }

    void undo() override
    {
        group_->removeOrder(newOrder_);
        emit model_->dataChanged();
    }

  private:
    const std::shared_ptr<group> group_;
    const std::shared_ptr<order> newOrder_;
};

class removeOrderCommand : public baseCommand
{
  public:
    using scriptCallbackType =
        std::function<std::string(const std::shared_ptr<group> &, const std::shared_ptr<order> &)>;
    static void setScriptCallback(scriptCallbackType callback)
    {
        scriptCallback_ = std::move(callback);
    }

  private:
    static scriptCallbackType scriptCallback_;

  public:
    explicit removeOrderCommand(std::shared_ptr<dataModel> model, const std::shared_ptr<group> &oneGroup,
                                const std::shared_ptr<order> &oneOrder)
        : baseCommand(std::move(model), QObject::tr("Remove Order")), group_(oneGroup), order_(oneOrder)
    {
    }

    ~removeOrderCommand() override = default;

    void redo() override
    {
        group_->removeOrder(order_);
        emit model_->dataChanged();

        if (isFirstTime_ && commandManager::getInstance()->isRecoring())
        {
            const auto record = scriptCallback_(group_, order_);
            commandManager::getInstance()->insertRecording(QString::fromStdString(record));

            isFirstTime_ = false;
        }
    }

    void undo() override
    {
        group_->addOrder(order_);
        emit model_->dataChanged();
    }

  private:
    const std::shared_ptr<group> group_;
    const std::shared_ptr<order> order_;
};

class updateOrderCommand : public baseCommand
{
  public:
    using scriptCallbackType = std::function<std::string(const std::shared_ptr<order> &oneOrder, int, double)>;
    static void setScriptCallback(scriptCallbackType callback)
    {
        scriptCallback_ = std::move(callback);
    }

  private:
    static scriptCallbackType scriptCallback_;

  public:
    explicit updateOrderCommand(const std::shared_ptr<dataModel> &model, const std::shared_ptr<order> &oneOrder,
                                const int amount, const double price)
        : baseCommand(model, QObject::tr("Update Order")), order_(oneOrder), newAmount_(amount), newPrice_(price)
    {
    }

    ~updateOrderCommand() override = default;

    void redo() override
    {
        if (order_ == nullptr)
        {
            this->setObsolete(true);
            return;
        }

        oldAmount_ = order_->amount_;
        oldPrice_ = order_->price_;
        order_->amount_ = newAmount_;
        order_->price_ = newPrice_;
        emit model_->dataChanged();

        if (isFirstTime_ && commandManager::getInstance()->isRecoring())
        {
            const auto record = scriptCallback_(order_, newAmount_, newPrice_);
            commandManager::getInstance()->insertRecording(QString::fromStdString(record));

            isFirstTime_ = false;
        }
    }

    void undo() override
    {
        order_->amount_ = oldAmount_;
        order_->price_ = oldPrice_;
        emit model_->dataChanged();
    }

  private:
    const std::shared_ptr<order> order_;
    const int newAmount_;
    const double newPrice_;

    int oldAmount_{};
    double oldPrice_{};
};
