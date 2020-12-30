#pragma once

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

enum CloudEvent {
  CREATE,
  RENAME,
  MODIFY,
  DELETE,
  SIZE
};

struct CloudNotification {
  CloudEvent event;
  boost::filesystem::path path;
  boost::optional<boost::filesystem::path> new_path = boost::none;
  std::chrono::steady_clock::time_point time;
};
