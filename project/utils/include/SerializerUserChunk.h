#pragma once

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "structs/UserChunk.h"
#include "SerializerInterface.h"
#include "SerializerExceptions.h"

namespace pt = boost::property_tree;

class SerializerUserChunk : public SerializerInterface {
 public:
  SerializerUserChunk() = default;
  explicit SerializerUserChunk(int requestId, std::vector<UserChunk> val);
  explicit SerializerUserChunk(const pt::ptree &val);

  int GetRequestId() const;
  std::vector<UserChunk> GetChunk() noexcept(false);
  pt::ptree GetJson();

 private:
  void serialize() override;
  void deserialize() override;

 private:
  int _requestId{};
  std::vector<UserChunk> _chunkVector;
  pt::ptree _json;
};
