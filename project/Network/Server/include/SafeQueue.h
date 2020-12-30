#pragma once

#include <utility>
#include <memory>
#include <queue>

template<class T>
class SafeQueue {
 public:
  SafeQueue();
  ~SafeQueue();

  void Push(T t);
  T Pop();
  bool Empty();

 private:
  std::queue<T> _q;
  mutable std::mutex _m;
  std::condition_variable _c;
};

template<class T>
SafeQueue<T>::SafeQueue() = default;

template<class T>
SafeQueue<T>::~SafeQueue() = default;

template<class T>
void SafeQueue<T>::Push(T t) {
  std::lock_guard<std::mutex> lock(_m);
  _q.push(t);
  _c.notify_one();
}

template<class T>
T SafeQueue<T>::Pop() {
  std::unique_lock<std::mutex> lock(_m);
  while (_q.empty()) {
    _c.wait(lock);
  }
  T val = _q.front();
  _q.pop();
  return val;
}

template<class T>
bool SafeQueue<T>::Empty() {
  return _q.empty();
}
