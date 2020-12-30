#include "Watcher.h"
#include <boost/log/trivial.hpp>

Watcher::Watcher() {
  BOOST_LOG_TRIVIAL(debug) << "Watcher Apple: create watcher";
}

void Watcher::Run(const fs::path &path, const std::function<void(const CloudNotification &event)> &callback) {
  BOOST_LOG_TRIVIAL(debug) << "Watcher Apple: Run";
}

void Watcher::Stop() {
  BOOST_LOG_TRIVIAL(debug) << "Watcher Apple: Stop";
}

bool Watcher::IsWorking() {
  BOOST_LOG_TRIVIAL(debug) << "Watcher Apple: IsWorking";
  return false;
}
