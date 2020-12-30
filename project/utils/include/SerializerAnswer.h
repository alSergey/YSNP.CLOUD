#pragma once

#include <string>
#include <vector>
#include <variant>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include "structs/Answer.h"
#include "SerializerInterface.h"
#include "SerializerExceptions.h"

template<class... Ts>
struct overloaded : Ts ... {
  using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace pt = boost::property_tree;

class SerializerAnswer : public SerializerInterface {
 public:
  explicit SerializerAnswer(int id);
  explicit SerializerAnswer(int id, std::string msg);
  explicit SerializerAnswer(int id, std::string msg, std::map<int, std::string> errs);
  explicit SerializerAnswer(const pt::ptree &json);

  std::variant<StatusOk, StatusError> GetStatus() noexcept(false);
  pt::ptree GetJson();

 private:
  void serialize() override;
  void deserialize() override;

 private:
  std::variant<StatusOk, StatusError> _status;
  pt::ptree _json;
};
