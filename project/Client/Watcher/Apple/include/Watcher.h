#pragma once

#include "../../include/WatcherInterface.h"

class Watcher : public WatcherInterface {
 public:
  Watcher();

  void Run(const fs::path &path, const std::function<void(const CloudNotification &event)> &callback) override;
  void Stop() override;
  bool IsWorking() override;
};
