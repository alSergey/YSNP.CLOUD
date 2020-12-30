#pragma once

#include <functional>
#include <boost/filesystem.hpp>
#include "structs/CloudEvents.h"

namespace fs = boost::filesystem;

class WatcherInterface {
 public:
  virtual ~WatcherInterface() = default;

  virtual void Run(const fs::path &path, const std::function<void(const CloudNotification &event)> &callback) = 0;
  virtual void Stop() = 0;
  virtual bool IsWorking() = 0;
};
