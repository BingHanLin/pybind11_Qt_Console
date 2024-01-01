#pragma once

#include <memory>
#include <string>

#pragma push_macro( \
    "slots")  // solve slots variable name conflits between python and qt.
#undef slots
#include <pybind11/embed.h>  // everything needed for embedding
#pragma pop_macro("slots")

#include "dataModel.hpp"

class pythonInterpreter
{
   public:
    pythonInterpreter(std::shared_ptr<dataModel> model);
    ~pythonInterpreter();

    void runCommand(const std::string& cmd) const;

   private:
    pybind11::scoped_interpreter interpreterGuard_;
    pybind11::dict locals_;
};
