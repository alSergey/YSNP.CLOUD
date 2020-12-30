#pragma once

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "structs/UserDate.h"
#include "SerializerInterface.h"
#include "SerializerExceptions.h"

namespace pt = boost::property_tree;

class SerializerUserDate : public SerializerInterface {
 public:
  SerializerUserDate() = default;
  explicit SerializerUserDate(int requestId, UserDate userDate);
  explicit SerializerUserDate(const pt::ptree &val);

  int GetRequestId() const;
  UserDate GetUserDate() noexcept(false);
  pt::ptree GetJson();

 private:
  void serialize() override;
  void deserialize() override;

 private:
  int _requestId{};
  UserDate _userDate;
  pt::ptree _json{};
};
