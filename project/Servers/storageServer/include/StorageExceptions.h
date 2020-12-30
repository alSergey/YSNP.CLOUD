#pragma once

#include <exception>
#include <string>
#include <utility>

class StorageExceptions : public std::exception {
 public:
  explicit StorageExceptions(std::string msg)
      : _msg(std::move(msg)) {
  }

  const char *what() const noexcept override {
    return _msg.c_str();
  }

 private:
  std::string _msg;
};

class WrongCommand : public StorageExceptions {
 public:
  explicit WrongCommand(std::string msg)
      : StorageExceptions("Wrong command: " + std::move(msg)) {
  }
};

class NoCommand : public StorageExceptions {
 public:
  explicit NoCommand(std::string msg)
      : StorageExceptions("No command: " + std::move(msg)) {
  }
};
