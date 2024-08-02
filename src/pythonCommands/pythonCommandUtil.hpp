#pragma once

#include <memory>
#include <string>

#include "object.hpp"

struct getterScript
{
    std::string findScript_;
    std::string targetName_;
};

inline getterScript getObjectScript(const std::shared_ptr<object>& o)
{
    std::vector<std::string> paths;
    object* parent = o->getParent();
    object* current = o.get();
    while (parent != nullptr)
    {
        paths.push_back(".findChild(\"" + current->getName() + "\")");
        current = parent;
        parent = parent->getParent();
    }

    getterScript script;
    for (auto it = paths.rbegin(); it != paths.rend(); ++it)
    {
        script.findScript_ += *it;
    }
    script.findScript_ = o->getName() + " = model_root()" + script.findScript_;
    script.targetName_ = o->getName();
    return script;
}