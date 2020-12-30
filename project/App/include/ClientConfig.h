#pragma once

#include <string>

struct NetworkConfig {
  std::string host;
  int port;
};

class ClientConfig {
 public:
  static ClientConfig &GetInstance();
  static void Log(const std::string &state = "debug", const std::string &file = "");

  static NetworkConfig getStorageConfig();
  static NetworkConfig getSyncConfig();

 private:
  ClientConfig();
  ~ClientConfig();
  ClientConfig(const ClientConfig &);
  ClientConfig &operator=(const ClientConfig &);
};
