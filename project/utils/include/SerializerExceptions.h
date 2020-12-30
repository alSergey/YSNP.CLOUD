#pragma once

#include <exception>
#include <string>
#include <utility>

class SerializerExceptions : public std::exception {
 public:
  explicit SerializerExceptions(std::string msg)
      : _msg(std::move(msg)) {
  }

  const char *what() const noexcept override {
    return _msg.c_str();
  }

 private:
  std::string _msg;
};

class ParseException : public SerializerExceptions {
 public:
  explicit ParseException(std::string msg)
      : SerializerExceptions("ParseException: " + std::move(msg)) {
  }
};
