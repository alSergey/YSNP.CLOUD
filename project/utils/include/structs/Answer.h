#pragma once

#include <string>
#include <map>

struct StatusOk {
  int requestId;
};

struct StatusError {
  int requestId;
  std::string msg;
  std::map<int, std::string> errs;
};
