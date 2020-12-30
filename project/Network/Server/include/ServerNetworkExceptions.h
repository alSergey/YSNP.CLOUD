#pragma once

#include <exception>
#include <string>
#include <utility>

class ServerNetworkExceptions : public std::exception {
 public:
  explicit ServerNetworkExceptions(std::string msg)
      : _msg(std::move(msg)) {
  }

  const char *what() const noexcept override {
    return _msg.c_str();
  }

 private:
  std::string _msg;
};

class ErrorRunServerException : public ServerNetworkExceptions {
 public:
  explicit ErrorRunServerException(std::string msg)
      : ServerNetworkExceptions(std::move(msg)) {
  }
};
