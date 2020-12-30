#include "FileSystemEvent.h"

#include <utility>

FileSystemEvent::FileSystemEvent(
    const int wd,
    uint32_t mask,
    boost::filesystem::path path,
    const std::chrono::steady_clock::time_point &eventTime)
    : wd(wd),
      mask(mask),
      path(std::move(path)),
      eventTime(eventTime) {
}
