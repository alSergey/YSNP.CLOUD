#pragma once

#include <string>
#include <vector>

#define CHUNK_SIZE       4096
#define CHUNK_MOVE_SIZE  1024

struct Chunk {
  int userId;
  int chunkId;
  int chunkSize = 0;
  std::string sHash;
  std::string rHash;
  std::vector<char> data{};
};
