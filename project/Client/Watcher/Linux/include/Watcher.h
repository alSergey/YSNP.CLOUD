#pragma once

#include <sys/inotify.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <queue>
#include <string>
#include <map>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/bimap.hpp>
#include <boost/optional.hpp>
#include "../../include/WatcherInterface.h"
#include "Notification.h"
#include "FileSystemEvent.h"
#include "WatcherExceptions.h"
#include "structs/CloudEvents.h"

#define EVENT_SIZE  sizeof(inotify_event)
#define MAX_EVENTS 4096

class Watcher : public WatcherInterface {
 public:
  Watcher();
  ~Watcher();

  void Stop() override;
  void Run(const boost::filesystem::path &, const std::function<void(const CloudNotification &event)> &) override;
  bool IsWorking() override;
  std::exception_ptr GetLastException();

 private:
  uint32_t _eventMask;
  int _inotifyFd;
  int _epollFd;
  epoll_event _inotifyEpollEvent;
  epoll_event _stopPipeEpollEvent;
  int _stopPipeFd[2];
  const int _pipeReadIdx;
  const int _pipeWriteIdx;
  boost::bimap<int, boost::filesystem::path> _directorieMap;
  std::queue<FileSystemEvent> _eventQueue;
  std::queue<std::exception_ptr> _exceptionQueue;
  std::vector<uint8_t> _eventBuffer;
  bool _stopped;
  std::chrono::steady_clock::time_point _lastEventTime;
  epoll_event _epollEvents[1];
  std::map<Event, std::function<void(Notification)>> _eventCallbacks;
  std::function<void(CloudNotification)> _eventCallback;
  bool _isReanameEvent;

  void removeWatch(int wd) const;
  boost::filesystem::path wdToPath(int wd);
  void watchFile(boost::filesystem::path file);
  void watchDirectory(boost::filesystem::path path);
  void UnwatcFile(const boost::filesystem::path &file);
  boost::optional<FileSystemEvent> getNextEvent();
  bool hasStopped() const;
  void sendStopSignal();
  void runOnce();
  ssize_t readEventsIntoBuffer(std::vector<uint8_t> &);
  void readEventsFromBuffer(uint8_t *, int, std::vector<FileSystemEvent> &);
};
