#pragma once

#include <queue>
#include <memory>
#include "Commands.h"

class Worker {
 public:
  static void Run(std::queue<std::shared_ptr<BaseCommand>> &queue, bool &isWorking);
};
