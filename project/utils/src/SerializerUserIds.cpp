#include "SerializerUserIds.h"
#include <boost/log/trivial.hpp>

SerializerUserIds::SerializerUserIds(int requestId, UserIds userIds, std::string command)
    : _requestId(requestId),
      _userIds(userIds),
      _command(std::move(command)) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserIds: create serializer user ids from struct";
}

SerializerUserIds::SerializerUserIds(const pt::ptree &val)
    : _json(val) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserIds: create serializer user ids from json";
}

int SerializerUserIds::GetRequestId() const {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserIds: GetRequestId";
  return _requestId;
}

UserIds SerializerUserIds::GetUserIds() noexcept(false) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserIds: GetUserIds";

  deserialize();

  return _userIds;
}

pt::ptree SerializerUserIds::GetJson() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserIds: GetJson";
  if (_json.empty()) {
    serialize();
  }

  return _json;
}

void SerializerUserIds::serialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserIds: serialize";

  _json.put("command", _command);
  _json.put("requestId", _requestId);
  _json.put("userId", _userIds.id);
}

void SerializerUserIds::deserialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserIds: deserialize";

  try {
    _requestId = _json.get<int>("requestId");
    _userIds.id = _json.get<int>("userId");
  } catch (pt::ptree_error &er) {
    throw ParseException(er.what());
  }
}
