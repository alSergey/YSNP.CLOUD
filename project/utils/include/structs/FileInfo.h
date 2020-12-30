#pragma once

#include <vector>
#include "FileMeta.h"
#include "ChunkMeta.h"
#include "FileChunksMeta.h"

struct FileInfo {
  int userId;
  FileMeta file;
  std::vector<ChunkMeta> chunkMeta;
  std::vector<FileChunksMeta> fileChunksMeta;
};
