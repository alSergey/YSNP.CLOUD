#pragma once

class Server {
 public:
  virtual ~Server() = default;
  virtual void Run() = 0;

 private:
  virtual void startWorkers() = 0;
  virtual void stopWorkers() = 0;
};
