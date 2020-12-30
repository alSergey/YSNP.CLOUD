#include "StorageServer.h"
#include <boost/log/trivial.hpp>

StorageServer::StorageServer()
    : _config(Config::GetInstance()),
      _networkServer(std::make_shared<NetworkServer>(_config.GetNetworkConfig().port,
                                                     _config.GetNetworkConfig().backlog)) {
  BOOST_LOG_TRIVIAL(debug) << "StorageServer: create storage server";
}

StorageServer::~StorageServer() {
  BOOST_LOG_TRIVIAL(debug) << "StorageServer: delete storage server";
  stopWorkers();
}

void StorageServer::Run() {
  BOOST_LOG_TRIVIAL(info) << "StorageServer: Run";
  startWorkers();
  try {
    _networkServer->StartServer();
  } catch (ErrorRunServerException &er) {
    stopWorkers();
  }
}

void StorageServer::startWorkers() {
  BOOST_LOG_TRIVIAL(debug) << "StorageServer: startWorkers";

  auto workersCount = _config.GetServerConfig().workersCount;
  _workerThreads.reserve(workersCount);

  for (int i = 0; i < workersCount; ++i) {
    _workerThreads.emplace_back(&Worker::Run,
                                std::move(Worker(std::make_shared<StorageManager>(), _networkServer)));

    BOOST_LOG_TRIVIAL(info) << "StorageServer: add new worker with id = " << _workerThreads[i].get_id();
  }
}

void StorageServer::stopWorkers() {
  BOOST_LOG_TRIVIAL(debug) << "StorageServer: stopWorkers";
  if (!_workerThreads.empty()) {
    for (auto &thread : _workerThreads) {
      thread.join();
      BOOST_LOG_TRIVIAL(info) << "StorageServer: join worker with id = " << thread.get_id();
    }

    _workerThreads.clear();
  }
}
