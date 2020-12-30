#include "Worker.h"
#include <boost/log/trivial.hpp>

void Worker::Run(std::queue<std::shared_ptr<BaseCommand>> &queue, bool &isWorking) {
  isWorking = true;

  while (!queue.empty()) {
    BOOST_LOG_TRIVIAL(info) << "Worker: new command";
    auto command = queue.front();
    queue.pop();
    command->Do();
  }

  isWorking = false;
}
