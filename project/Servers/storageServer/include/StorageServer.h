#pragma once

#include <queue>
#include <utility>
#include <vector>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include "Server.h"
#include "Worker.h"
#include "StorageManager.h"
#include "NetworkServer.h"
#include "ServerNetworkExceptions.h"
#include "Config.h"

namespace pt = boost::property_tree;

class StorageServer : public Server {
 public:
  StorageServer();
  ~StorageServer() override;

  void Run() override;

 private:
  void startWorkers() override;
  void stopWorkers() override;

 private:
  Config &_config;
  std::vector<std::thread> _workerThreads;
  std::shared_ptr<NetworkServer> _networkServer;
};
