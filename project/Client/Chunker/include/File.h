#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

class File {
 public:
  explicit File(const bfs::path &path);
  ~File() = default;

  std::ifstream Read();
  std::ofstream Write();

  static void Rename(const bfs::path &oldPath, const bfs::path &newPath);
  static void Delete(const bfs::path &path);

 private:
  bfs::path _path;
};
