#pragma once

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "structs/Chunk.h"
#include "SerializerInterface.h"
#include "SerializerExceptions.h"

namespace pt = boost::property_tree;

size_t calcDecodeLength(const std::string &b64input);
int Base64Decode(const std::string &b64message, char **buffer, size_t *length);
std::string Base64Encode(char *buffer, size_t length);

class SerializerChunk : public SerializerInterface {
 public:
  SerializerChunk() = default;
  explicit SerializerChunk(int requestId, std::vector<Chunk> val);
  explicit SerializerChunk(const pt::ptree &val);

  int GetRequestId() const;
  std::vector<Chunk> GetChunk() noexcept(false);
  pt::ptree GetJson();

 private:
  void serialize() override;
  void deserialize() override;

 private:
  int _requestId{};
  std::vector<Chunk> _chunkVector;
  pt::ptree _json{};
};
