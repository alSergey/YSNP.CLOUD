#pragma once

#include <exception>
#include <string>
#include <utility>

class AppExceptions : public std::exception {
 public:
  explicit AppExceptions(std::string msg)
      : _msg(std::move(msg)) {
  }

  const char *what() const noexcept override {
    return _msg.c_str();
  }

 private:
  std::string _msg;
};

class FileIdException : public AppExceptions {
 public:
  explicit FileIdException(std::string msg)
      : AppExceptions("Wrong file id: " + std::move(msg)) {
  }
};

class FileDownloadedException : public AppExceptions {
 public:
  explicit FileDownloadedException(std::string msg)
      : AppExceptions("File downloaded: " + std::move(msg)) {
  }
};

class FileNotExistsException : public AppExceptions {
 public:
  explicit FileNotExistsException(std::string msg)
      : AppExceptions("File not exists: " + std::move(msg)) {
  }
};

class FolderNotExistsException : public AppExceptions {
 public:
  explicit FolderNotExistsException(std::string msg)
      : AppExceptions("Folder not exists: " + std::move(msg)) {
  }
};
