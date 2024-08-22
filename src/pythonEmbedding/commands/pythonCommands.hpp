#pragma once

#include <memory>

#pragma push_macro("slots")  // solve slots variable name conflits between python and qt.
#undef slots
#include <pybind11/embed.h>  // everything needed for embedding
#pragma pop_macro("slots")

class dataModel;

class pythonCommands
{
  public:
    pythonCommands();
    ~pythonCommands();

    static std::shared_ptr<dataModel> model_;

    static void setDataModel(std::shared_ptr<dataModel> model);
};
