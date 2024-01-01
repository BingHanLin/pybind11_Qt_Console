#pragma once

#include <memory>

#pragma push_macro( \
    "slots")  // solve slots variable name conflits between python and qt.
#undef slots
#include <pybind11/embed.h>  // everything needed for embedding
#pragma pop_macro("slots")

#include "commands.hpp"
#include "dataModel.hpp"

class pythonCommands
{
   public:
    pythonCommands();
    ~pythonCommands();

    static std::shared_ptr<dataModel> model_;
};
