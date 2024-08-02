#pragma once

#include <iostream>
#include <memory>
#include <vector>

class object
{
  public:
    explicit object(const std::string& name, object* parent) : parent_(parent), name_(name){};
    ~object() = default;

    [[nodiscard]] object* getParent() const
    {
        return parent_;
    }

    [[nodiscard]] virtual std::vector<std::shared_ptr<object>> getChildren() const = 0;

    [[nodiscard]] std::shared_ptr<object> findChild(const std::string& name) const
    {
        for (const auto& child : this->getChildren())
        {
            if (child->getName() == name)
            {
                return child;
            }
        }

        return nullptr;
    }

    [[nodiscard]] std::string getName() const
    {
        return name_;
    }

  private:
    object* parent_;
    std::string name_;
};
