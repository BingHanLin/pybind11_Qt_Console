#pragma once

#include "group.hpp"
#include "object.hpp"

class root : public object
{
  public:
    explicit root() : object("Root", nullptr){};
    ~root() = default;

    [[nodiscard]] std::vector<std::shared_ptr<object>> getChildren() const override
    {
        std::vector<std::shared_ptr<object>> results;
        results.reserve(groups_.size());
        for (const auto& group : groups_)
        {
            results.push_back(group);
        }

        return results;
    };

    std::vector<std::shared_ptr<group>> getGroups()
    {
        return groups_;
    }

    void addGroup(const std::shared_ptr<group>& g)
    {
        groups_.push_back(g);
    }

    void removeGroup(const std::shared_ptr<group>& g)
    {
        groups_.erase(std::remove(groups_.begin(), groups_.end(), g), groups_.end());
    }

  private:
    std::vector<std::shared_ptr<group>> groups_;
};
