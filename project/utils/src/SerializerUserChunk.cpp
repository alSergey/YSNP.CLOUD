#include "SerializerUserChunk.h"
#include <boost/log/trivial.hpp>

SerializerUserChunk::SerializerUserChunk(int requestId, std::vector<UserChunk> val)
    : _requestId(requestId),
      _chunkVector(std::move(val)) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserChunk: create serializer chunk from vector";
}

SerializerUserChunk::SerializerUserChunk(const pt::ptree &val)
    : _json(val) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserChunk: create serializer chunk from json";
}

int SerializerUserChunk::GetRequestId() const {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserChunk: GetRequestId";
  return _requestId;
}

std::vector<UserChunk> SerializerUserChunk::GetChunk() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserChunk: GetChunk";
  if (_chunkVector.empty()) {
    deserialize();
  }

  return std::move(_chunkVector);
}

pt::ptree SerializerUserChunk::GetJson() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserChunk: GetJson";
  if (_json.empty()) {
    serialize();
  }

  return _json;
}

void SerializerUserChunk::serialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserChunk: serialize";

  _json.put("command", "DownloadChunk");
  _json.put("requestId", _requestId);
  pt::ptree data;
  for (auto &el : _chunkVector) {
    pt::ptree child;
    child.put("userId", el.userId);
    child.put("chunkId", el.chunkId);

    data.push_back(std::make_pair("", child));
  }

  _json.add_child("data", data);
}

void SerializerUserChunk::deserialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerUserChunk: deserialize";

  try {
    _requestId = _json.get<int>("requestId");
    for (auto &val : _json.get_child("data")) {
      auto userChunk = UserChunk{.userId = val.second.get<int>("userId"),
          .chunkId = val.second.get<int>("chunkId")};

      _chunkVector.push_back(userChunk);
    }
  } catch (pt::ptree_error &er) {
    throw ParseException(er.what());
  }
}
