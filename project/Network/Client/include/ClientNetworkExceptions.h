#pragma once

#include <exception>
#include <string>
#include <utility>

class ClientNetworkExceptions : public std::exception {
 public:
  explicit ClientNetworkExceptions(std::string msg)
      : _msg(std::move(msg)) {
  }

  const char *what() const noexcept override {
    return _msg.c_str();
  }

 private:
  std::string _msg;
};

class ConnectServerException : public ClientNetworkExceptions {
 public:
  explicit ConnectServerException(std::string msg)
      : ClientNetworkExceptions(std::move(msg)) {
  }
};

class ReadFromServerException : public ClientNetworkExceptions {
 public:
  explicit ReadFromServerException(std::string msg)
      : ClientNetworkExceptions(std::move(msg)) {
  }
};

class SendToServerException : public ClientNetworkExceptions {
 public:
  explicit SendToServerException(std::string msg)
      : ClientNetworkExceptions(std::move(msg)) {
  }
};

class ReadFromServerParseJsonException : public ClientNetworkExceptions {
 public:
  explicit ReadFromServerParseJsonException(std::string msg)
      : ClientNetworkExceptions(std::move(msg)) {
  }
};

class SendToServerParseJsonException : public ClientNetworkExceptions {
 public:
  explicit SendToServerParseJsonException(std::string msg)
      : ClientNetworkExceptions(std::move(msg)) {
  }
};
