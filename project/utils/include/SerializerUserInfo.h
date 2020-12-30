#pragma once

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "structs/UserInfo.h"
#include "SerializerInterface.h"
#include "SerializerExceptions.h"

namespace pt = boost::property_tree;

class SerializerUserInfo : public SerializerInterface {
 public:
  SerializerUserInfo() = default;
  explicit SerializerUserInfo(int requestId, UserInfo userInfo, std::string command);
  explicit SerializerUserInfo(const pt::ptree &val);

  int GetRequestId() const;
  UserInfo GetUserInfo() noexcept(false);
  pt::ptree GetJson();

 private:
  void serialize() override;
  void deserialize() override;

 private:
  int _requestId{};
  std::string _command;
  UserInfo _userInfo;
  pt::ptree _json;
};
