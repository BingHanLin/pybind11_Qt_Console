#pragma once

#include <memory>
#include <utility>

#include <QUndoCommand>

#include "commandManager.hpp"
#include "dataModel.hpp"

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

class addCommand : public baseCommand
{
  public:
    using scriptCallbackType = std::function<std::string(int, int, double)>;
    static void setScriptCallback(scriptCallbackType callback)
    {
        scriptCallback_ = std::move(callback);
    }

  private:
    // static std::function<std::string(std::string, int, int, int)> scriptCallback_;
    static scriptCallbackType scriptCallback_;

  public:
    explicit addCommand(std::shared_ptr<dataModel> model, std::shared_ptr<order> newOrder)
        : baseCommand(std::move(model), QObject::tr("Add Order")), newOrder_(std::move(newOrder))
    {
    }

    ~addCommand() override = default;

    void redo() override
    {
        const auto result = model_->addOrder(newOrder_);
        this->setObsolete(!result);

        if (result && isFirstTime_ && commandManager::getInstance()->isRecoring())
        {
            const auto record = scriptCallback_(newOrder_->id_, newOrder_->amount_, newOrder_->price_);
            commandManager::getInstance()->insertRecording(QString::fromStdString(record));

            isFirstTime_ = false;
        }
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
    using scriptCallbackType = std::function<std::string(int)>;
    static void setScriptCallback(scriptCallbackType callback)
    {
        scriptCallback_ = std::move(callback);
    }

  private:
    static scriptCallbackType scriptCallback_;

  public:
    explicit removeCommand(std::shared_ptr<dataModel> model, const int id)
        : baseCommand(std::move(model), QObject::tr("Remove Order")), id_(id), removedOrder_(nullptr)
    {
    }

    ~removeCommand() override = default;

    void redo() override
    {
        removedOrder_ = model_->getOrder(id_);
        const auto result = model_->removeOrder(id_);
        this->setObsolete(!result);

        if (result && isFirstTime_ && commandManager::getInstance()->isRecoring())
        {
            const auto record = scriptCallback_(id_);
            commandManager::getInstance()->insertRecording(QString::fromStdString(record));

            isFirstTime_ = false;
        }
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
    using scriptCallbackType = std::function<std::string(int, int, double)>;
    static void setScriptCallback(scriptCallbackType callback)
    {
        scriptCallback_ = std::move(callback);
    }

  private:
    static scriptCallbackType scriptCallback_;

  public:
    explicit updateCommand(std::shared_ptr<dataModel> model, const int id, const int amount, const double price)
        : baseCommand(std::move(model), QObject::tr("Update Order")), id_(id), newAmount_(amount), newPrice_(price)
    {
    }

    ~updateCommand() override = default;

    void redo() override
    {
        auto oldOrder = model_->getOrder(id_);

        if (oldOrder == nullptr)
        {
            this->setObsolete(true);
            return;
        }

        oldAmount_ = oldOrder->amount_;
        oldPrice_ = oldOrder->price_;

        const auto result = model_->updateOrder(id_, newAmount_, newPrice_);
        this->setObsolete(!result);

        if (result && isFirstTime_ && commandManager::getInstance()->isRecoring())
        {
            const auto record = scriptCallback_(id_, newAmount_, newPrice_);
            commandManager::getInstance()->insertRecording(QString::fromStdString(record));

            isFirstTime_ = false;
        }
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
