#pragma once

#include <exception>
#include <string>
#include <utility>

class MongoExceptions : public std::exception {
 public:
  explicit MongoExceptions(std::string msg)
      : _msg(std::move(msg)) {
  }

  const char *what() const noexcept override {
    return _msg.c_str();
  }

 private:
  std::string _msg;
};
