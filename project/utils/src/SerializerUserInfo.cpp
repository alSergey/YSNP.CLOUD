#include "SerializerUserInfo.h"
#include <boost/log/trivial.hpp>

SerializerUserInfo::SerializerUserInfo(int requestId, UserInfo userInfo, std::string command)
    : _requestId(requestId),
      _userInfo(std::move(userInfo)),
      _command(std::move(command)) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserInfo: create serializer user info from struct";
}

SerializerUserInfo::SerializerUserInfo(const pt::ptree &val)
    : _json(val) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserInfo: create serializer user info from json";
}

int SerializerUserInfo::GetRequestId() const {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserInfo: GetRequestId";
  return _requestId;
}

UserInfo SerializerUserInfo::GetUserInfo() noexcept(false) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserInfo: GetUserInfo";

  deserialize();

  return _userInfo;
}

pt::ptree SerializerUserInfo::GetJson() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserInfo: GetJson";
  if (_json.empty()) {
    serialize();
  }

  return _json;
}

void SerializerUserInfo::serialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserInfo: serialize";

  _json.put("command", _command);
  _json.put("requestId", _requestId);
  _json.put("login", _userInfo.login);
  _json.put("password", _userInfo.password);
}

void SerializerUserInfo::deserialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserInfo: deserialize";

  try {
    _requestId = _json.get<int>("requestId");
    _userInfo.login = _json.get<std::string>("login");
    _userInfo.password = _json.get<std::string>("password");
  } catch (pt::ptree_error &er) {
    throw ParseException(er.what());
  }
}
