#pragma once

#include <utility>
#include <functional>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include "NetworkServer.h"
#include "CommandManager.h"

namespace pt = boost::property_tree;

class Worker {
 public:
  explicit Worker(std::shared_ptr<CommandManager> manager,
                  std::shared_ptr<NetworkServer> server);
  ~Worker();

  void Run();

 private:
  void listening();
  void onConnect(const std::shared_ptr<std::pair<std::shared_ptr<UserSession>, pt::ptree>> &request);

 private:
  std::shared_ptr<NetworkServer> _networkServer;
  std::shared_ptr<CommandManager> _manager;
};
