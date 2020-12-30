#include "SyncServer.h"
#include <boost/log/trivial.hpp>

SyncServer::SyncServer()
    : _config(Config::GetInstance()),
      _networkServer(std::make_shared<NetworkServer>(_config.GetNetworkConfig().port,
                                                    _config.GetNetworkConfig().backlog)) {
  BOOST_LOG_TRIVIAL(debug) << "SyncServer: create storage server";
}

SyncServer::~SyncServer() {
  BOOST_LOG_TRIVIAL(debug) << "StorageServer: delete storage server";
  stopWorkers();
}

void SyncServer::Run() {
  BOOST_LOG_TRIVIAL(info) << "SyncServer: Run";
  startWorkers();
  try {
    _networkServer->StartServer();
  } catch (ErrorRunServerException &er) {
    stopWorkers();
  }
}

void SyncServer::startWorkers() {
  BOOST_LOG_TRIVIAL(debug) << "SyncServer: startWorkers";

  auto workersCount = _config.GetServerConfig().workersCount;
  _workerThreads.reserve(workersCount);

  for (int i = 0; i < workersCount; ++i) {
    _workerThreads.emplace_back(&Worker::Run,
                                std::move(Worker(std::make_shared<SyncManager>(), _networkServer)));

    BOOST_LOG_TRIVIAL(info) << "SyncServer: add new worker with id = " << _workerThreads[i].get_id();
  }
}

void SyncServer::stopWorkers() {
  BOOST_LOG_TRIVIAL(debug) << "SyncServer: stopWorkers";
  if (!_workerThreads.empty()) {
    for (auto &thread : _workerThreads) {
      thread.join();
      BOOST_LOG_TRIVIAL(info) << "SyncServer: join worker with id = " << thread.get_id();
    }

    _workerThreads.clear();
  }
}

