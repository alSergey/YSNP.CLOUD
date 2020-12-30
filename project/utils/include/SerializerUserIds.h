#pragma once

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "structs/UserIds.h"
#include "SerializerInterface.h"
#include "SerializerExceptions.h"

namespace pt = boost::property_tree;

class SerializerUserIds : public SerializerInterface {
 public:
  SerializerUserIds() = default;
  explicit SerializerUserIds(int requestId, UserIds userIds, std::string command);
  explicit SerializerUserIds(const pt::ptree &val);

  int GetRequestId() const;
  UserIds GetUserIds() noexcept(false);
  pt::ptree GetJson();

 private:
  void serialize() override;
  void deserialize() override;

 private:
  int _requestId{};
  std::string _command;
  UserIds _userIds{};
  pt::ptree _json;
};
