#include "SerializerChunk.h"
#include <boost/log/trivial.hpp>

size_t calcDecodeLength(const std::string &b64input) {
  size_t len = b64input.size(),
      padding = 0;

  if (b64input[len - 1] == '=' && b64input[len - 2] == '=')
    padding = 2;
  else if (b64input[len - 1] == '=')
    padding = 1;

  return (len * 3) / 4 - padding;
}

int Base64Decode(const std::string &b64message, char **buffer, size_t *length) {
  BIO *bio, *b64;

  int decodeLen = calcDecodeLength(b64message);
  *buffer = new char[decodeLen];

  bio = BIO_new_mem_buf(b64message.data(), -1);
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_push(b64, bio);

  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
  *length = BIO_read(bio, *buffer, b64message.size());
  BIO_free_all(bio);

  return (0);
}

std::string Base64Encode(char *buffer, size_t length) {
  BIO *bio, *b64;
  BUF_MEM *bufferPtr;

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new(BIO_s_mem());
  bio = BIO_push(b64, bio);

  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
  BIO_write(bio, buffer, length);
  BIO_flush(bio);
  BIO_get_mem_ptr(bio, &bufferPtr);
  BIO_set_close(bio, BIO_NOCLOSE);
  BIO_free_all(bio);

  return std::string(bufferPtr->data, bufferPtr->length);
}

SerializerChunk::SerializerChunk(int requestId, std::vector<Chunk> val)
    : _requestId(requestId),
      _chunkVector(std::move(val)) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerChunk: create serializer chunk from vector";
}

SerializerChunk::SerializerChunk(const pt::ptree &val)
    : _json(val) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerChunk: create serializer chunk from json";
}

int SerializerChunk::GetRequestId() const {
  BOOST_LOG_TRIVIAL(debug) << "SerializerChunk: GetRequestId";
  return _requestId;
}

std::vector<Chunk> SerializerChunk::GetChunk() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerChunk: GetChunk";
  if (_chunkVector.empty()) {
    deserialize();
  }

  return std::move(_chunkVector);
}

pt::ptree SerializerChunk::GetJson() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerChunk: GetJson";
  if (_json.empty()) {
    serialize();
  }

  return _json;
}

void SerializerChunk::serialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerChunk: serialize";

  _json.put("command", "UploadChunk");
  _json.put("requestId", _requestId);
  pt::ptree data;
  for (auto &el : _chunkVector) {
    pt::ptree child;
    child.put("userId", el.userId);
    child.put("chunkId", el.chunkId);
    child.put("chunkSize", el.chunkSize);
    child.put("sHash", el.sHash);
    child.put("rHash", el.rHash);
    std::string s = Base64Encode(el.data.data(), el.chunkSize);
    child.put("data", s);

    data.push_back(std::make_pair("", child));
  }

  _json.add_child("data", data);
}

void SerializerChunk::deserialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerChunk: deserialize";

  try {
    _requestId = _json.get<int>("requestId");
    for (auto &val : _json.get_child("data")) {
      auto chunk = Chunk{.userId = val.second.get<int>("userId"),
          .chunkId =  val.second.get<int>("chunkId"),
          .chunkSize =  val.second.get<int>("chunkSize"),
          .sHash = val.second.get<std::string>("sHash"),
          .rHash =  val.second.get<std::string>("rHash")
      };

      auto str = val.second.get<std::string>("data");

      char *base64DecodeOutput;
      size_t test = 0;
      Base64Decode(str, &base64DecodeOutput, &test);
      std::vector<char> vec(base64DecodeOutput, base64DecodeOutput + test);
      chunk.data = std::move(vec);

      _chunkVector.push_back(chunk);
    }
  } catch (pt::ptree_error &er) {
    throw ParseException(er.what());
  }
}
