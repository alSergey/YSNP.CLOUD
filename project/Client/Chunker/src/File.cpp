#include "File.h"

File::File(const bfs::path &path)
    : _path(path) {
  if (!bfs::exists(path.parent_path())) {
    bfs::create_directories(path.parent_path());
  }
}

std::ifstream File::Read() {
  std::ifstream fileStream;
  fileStream.open(_path.c_str(), std::ios::in | std::ios::binary);
  return fileStream;
}

std::ofstream File::Write() {
  std::ofstream fileStream;
  fileStream.open(_path.c_str(), std::ios::out | std::ios::binary);
  return fileStream;
}

void File::Rename(const bfs::path &oldPath, const bfs::path &newPath) {
  bfs::rename(oldPath, newPath);
}

void File::Delete(const bfs::path &path) {
  bfs::remove(path);
}
