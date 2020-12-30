#pragma once

#include <utility>
#include <exception>
#include <string>

class WatcherExceptions : public std::exception {
 public:
  explicit WatcherExceptions(std::string msg)
      : _msg(std::move(msg)) {
  }

  const char *what() const noexcept override {
    return _msg.c_str();
  }

 private:
  std::string _msg;
};

class WrongPath : public WatcherExceptions {
 public:
  explicit WrongPath(std::string msg)
      : WatcherExceptions("Can´t watch Path! Path does not exist. Path: " + std::move(msg)) {
  }
};

class InotifyInitError : public WatcherExceptions {
 public:
  InotifyInitError()
      : WatcherExceptions("Can't initialize inotify !") {
  }
};

class EpollInitError : public WatcherExceptions {
 public:
  EpollInitError()
      : WatcherExceptions("Can't initialize epoll !") {
  }
};

class EpollFDError : public WatcherExceptions {
 public:
  EpollFDError()
      : WatcherExceptions("Can't add inotify filedescriptor to epoll !") {
  }
};

class InotifyAddWatchError : public WatcherExceptions {
 public:
  explicit InotifyAddWatchError(std::string msg)
      : WatcherExceptions("Failed to watch! Path:" + std::move(msg)) {
  }
};

class InotifyRemoveWatchError : public WatcherExceptions {
 public:
  InotifyRemoveWatchError()
      : WatcherExceptions("Failed to remove watch!") {
  }
};
