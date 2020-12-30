#pragma once

#include <vector>
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "structs/FileMeta.h"
#include "structs/Chunk.h"
#include "structs/ChunkMeta.h"
#include "structs/FileChunksMeta.h"
#include "structs/FileInfo.h"
#include "InternalDB.h"

namespace bfs = boost::filesystem;

class Indexer {
 public:
  explicit Indexer(std::shared_ptr<InternalDB> internalDB);

  FileMeta CreateFile(const bfs::path &path, int chunksCount);
  FileMeta ModifyFile(const bfs::path &path, int chunksCount);
  FileMeta DeleteFile(const bfs::path &path);
  FileMeta RenameFile(const bfs::path &oldPath, const bfs::path &newPath);

  FileInfo GetFileInfo(const FileMeta &fileMeta, std::vector<Chunk> &chunksVector);
  FileInfo GetRenameFileInfo(const FileMeta &fileMeta);
  FileInfo GetDeleteFileInfo(const FileMeta &fileMeta);

 private:
  std::vector<FileChunksMeta> createFileChunks(const int &fileId, const int &chunksCount);

 private:
  std::shared_ptr<InternalDB> _internalDB;
};
