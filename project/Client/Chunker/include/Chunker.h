#pragma once

#define CHUNK_SIZE 4096

#include <openssl/md5.h>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <array>
#include <iomanip>
#include "structs/Chunk.h"
#include "File.h"
#include "zlib.h"

class Chunker {
 public:
  explicit Chunker(const File &);

  std::vector<Chunk> ChunkFile();
  std::vector<Chunk> UpdateChunkFile(const std::vector<Chunk> &);
  void MergeFile(const std::vector<Chunk> &);
 private:
  File _file;
  static std::string getRHash(char *, int);
  static std::string getSHash(char *, int);
  Chunk createChunk(std::vector<char>, int);
};
