#pragma once

#include <string>

struct FileMeta {
  int fileId;
  int version;
  std::string fileName;
  std::string fileExtension;
  std::string filePath;
  int fileSize;
  int chunksCount;
  bool isDownload;
  bool isDeleted;
  bool isCurrent;
  std::string updateDate;
  std::string createDate;

  std::string GetFilePath() const {
    return filePath + '/' + fileName + fileExtension;
  }
};
