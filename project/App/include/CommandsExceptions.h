#pragma once

#include <exception>
#include <string>
#include <utility>

class CommandsException : public std::exception {
 public:
  explicit CommandsException(std::string msg)
      : _msg(std::move(msg)) {
  }

  const char *what() const noexcept override {
    return _msg.c_str();
  }

 private:
  std::string _msg;
};

class NetworkException : public CommandsException {
 public:
  explicit NetworkException(std::string msg)
      : CommandsException("networkException: " + std::move(msg)) {
  }
};
