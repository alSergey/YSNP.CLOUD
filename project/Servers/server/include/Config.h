#pragma once

#include <string>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

struct ServerConfig {
  size_t workersCount;
};

struct NetworkConfig {
  size_t port;
  size_t backlog;
};

struct ClientWorkerConfig {
  size_t requestWorkersCount;
  size_t responseWorkersCount;
};

class Config {
 public:
  int Run(const std::string &path);
  ServerConfig &GetServerConfig();
  NetworkConfig &GetNetworkConfig();
  ClientWorkerConfig &GetClientWorkerConfig();
  static Config &GetInstance();

 private:
  Config();
  ~Config();
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;

  static void parseLogConfig(const pt::ptree &log);
  void parseStorageConfig(const pt::ptree &storage);
  void parseNetworkConfig(const pt::ptree &network);
  void parseClientWorkerConfig(const pt::ptree &clientWorker);

 private:
  ServerConfig _serverConfig{};
  NetworkConfig _networkConfig{};
  ClientWorkerConfig _clientWorkerConfig{};
};
