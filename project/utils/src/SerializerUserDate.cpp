#include "SerializerUserDate.h"
#include <boost/log/trivial.hpp>

SerializerUserDate::SerializerUserDate(int requestId, UserDate userDate)
    : _requestId(requestId),
      _userDate(std::move(userDate)) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserDate: create serializer user date from struct";
}

SerializerUserDate::SerializerUserDate(const pt::ptree &val)
    : _json(val) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserDate: create serializer user date from json";
}

int SerializerUserDate::GetRequestId() const {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserDate: GetRequestId";
  return _requestId;
}

UserDate SerializerUserDate::GetUserDate() noexcept(false) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserDate: GetUserDate";

  deserialize();

  return _userDate;
}

pt::ptree SerializerUserDate::GetJson() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserDate: GetJson";
  if (_json.empty()) {
    serialize();
  }

  return _json;
}

void SerializerUserDate::serialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserDate: serialize";

  _json.put("command", "DateUpdate");
  _json.put("requestId", _requestId);
  _json.put("userId", _userDate.userId);
  _json.put("date", _userDate.date);
}

void SerializerUserDate::deserialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserDate: deserialize";

  try {
    _requestId = _json.get<int>("requestId");
    _userDate.userId = _json.get<int>("userId");
    _userDate.date = _json.get<std::string>("date");
  } catch (pt::ptree_error &er) {
    throw ParseException(er.what());
  }
}
