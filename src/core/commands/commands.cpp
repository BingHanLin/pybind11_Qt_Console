#include "commands.hpp"

addOrderCommand::scriptCallbackType addOrderCommand::scriptCallback_ = nullptr;

removeOrderCommand::scriptCallbackType removeOrderCommand::scriptCallback_ = nullptr;

updateOrderCommand::scriptCallbackType updateOrderCommand::scriptCallback_ = nullptr;

clearAllOrdersCommand::scriptCallbackType clearAllOrdersCommand::scriptCallback_ = nullptr;