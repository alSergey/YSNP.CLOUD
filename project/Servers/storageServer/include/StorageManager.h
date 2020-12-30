#pragma once

#include <memory>
#include <boost/property_tree/ptree.hpp>
#include "CommandManager.h"
#include "StorageExceptions.h"
#include "SerializerAnswer.h"
#include "StorageCommands.h"

namespace pt = boost::property_tree;

class StorageManager : public CommandManager {
 public:
  std::shared_ptr<pt::ptree> GetRequest(const pt::ptree &val) override;

 private:
  std::unique_ptr<Command> getCommand(const pt::ptree &val) override;
};
