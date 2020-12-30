#pragma once

#include <sys/inotify.h>
#include <cstdlib>
#include <boost/filesystem.hpp>

enum Event : std::uint32_t {
  _access = IN_ACCESS,
  _attrib = IN_ATTRIB,
  _close_write = IN_CLOSE_WRITE,
  _close_nowrite = IN_CLOSE_NOWRITE,
  _close = IN_CLOSE,
  _create = IN_CREATE,
  _remove = IN_DELETE,
  _remove_self = IN_DELETE_SELF,
  _modify = IN_MODIFY,
  _move_self = IN_MOVE_SELF,
  _moved_from = IN_MOVED_FROM,
  _moved_to = IN_MOVED_TO,
  _move = IN_MOVE,
  _open = IN_OPEN,
  _is_dir = IN_ISDIR,
  _unmount = IN_UNMOUNT,
  _q_overflow = IN_Q_OVERFLOW,
  _ignored = IN_IGNORED,
  _oneshot = IN_ONESHOT,
  _all = IN_ALL_EVENTS
};

constexpr Event operator|(Event lhs, Event rhs) {
  return static_cast<Event>(
      static_cast<std::underlying_type<Event>::type>(lhs)
          | static_cast<std::underlying_type<Event>::type>(rhs));
}

constexpr Event operator&(Event lhs, Event rhs) {
  return static_cast<Event>(
      static_cast<std::underlying_type<Event>::type>(lhs)
          & static_cast<std::underlying_type<Event>::type>(rhs));
}

struct Notification {
  Event event;
  boost::filesystem::path path;
  std::chrono::steady_clock::time_point time;
};
