#pragma once

#include <exception>
#include <string>
#include <utility>

class SyncExceptions : public std::exception {
 public:
  explicit SyncExceptions(std::string msg)
      : _msg(std::move(msg)) {
  }

  const char *what() const noexcept override {
    return _msg.c_str();
  }

 private:
  std::string _msg;
};

class WrongCommand : public SyncExceptions {
 public:
  explicit WrongCommand(std::string msg)
      : SyncExceptions("Wrong command: " + std::move(msg)) {
  }
};

class NoCommand : public SyncExceptions {
 public:
  explicit NoCommand(std::string msg)
      : SyncExceptions("No command: " + std::move(msg)) {
  }
};
