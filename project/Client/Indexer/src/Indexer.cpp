#include "Indexer.h"

Indexer::Indexer(std::shared_ptr<InternalDB> internalDB)
    : _internalDB(std::move(internalDB)) {
}

FileMeta Indexer::CreateFile(const bfs::path &path, int chunksCount) {
  auto filePath = path.parent_path().string();
  filePath.erase(filePath.find(_internalDB->GetSyncFolder()), _internalDB->GetSyncFolder().length());

  FileMeta fileMeta{
      .fileName = path.stem().string(),
      .fileExtension = path.extension().string(),
      .filePath = filePath,
      .fileSize = static_cast<int>(boost::filesystem::file_size(path)),
      .chunksCount = chunksCount,
      .isDownload = true,
      .isDeleted = false,
      .isCurrent = true,
      .updateDate = boost::lexical_cast<std::string>(boost::filesystem::last_write_time(path)),
      .createDate = boost::lexical_cast<std::string>(boost::filesystem::last_write_time(path))};

  _internalDB->InsertAndIndexFile(fileMeta);
  return fileMeta;
}

FileMeta Indexer::ModifyFile(const bfs::path &path, int chunksCount) {
  auto filePath = path.parent_path().string();
  filePath.erase(filePath.find(_internalDB->GetSyncFolder()), _internalDB->GetSyncFolder().length());

  auto fileMeta = _internalDB->GetFile(filePath, path.stem().string(), path.extension().string());
  fileMeta.fileSize = static_cast<int>(boost::filesystem::file_size(path));
  fileMeta.chunksCount = chunksCount;
  fileMeta.isCurrent = true;
  fileMeta.isDeleted = false;
  fileMeta.updateDate = boost::lexical_cast<std::string>(boost::filesystem::last_write_time(path));

  _internalDB->UpdateAndIndexFile(fileMeta);
  return fileMeta;
}

FileMeta Indexer::DeleteFile(const bfs::path &path) {
  auto filePath = path.parent_path().string();
  filePath.erase(filePath.find(_internalDB->GetSyncFolder()), _internalDB->GetSyncFolder().length());

  auto fileMeta = _internalDB->GetFile(filePath, path.stem().string(), path.extension().string());
  fileMeta.fileSize = 0;
  fileMeta.chunksCount = 0;
  fileMeta.isCurrent = true;
  fileMeta.isDeleted = true;
  fileMeta.updateDate = boost::lexical_cast<std::string>(std::time(nullptr));

  _internalDB->DeleteAndIndexFile(fileMeta);
  return fileMeta;
}

FileMeta Indexer::RenameFile(const bfs::path &oldPath, const bfs::path &newPath) {
  auto oldFilePath = oldPath.parent_path().string();
  oldFilePath.erase(oldFilePath.find(_internalDB->GetSyncFolder()), _internalDB->GetSyncFolder().length());

  auto newFilePath = newPath.parent_path().string();
  newFilePath.erase(newFilePath.find(_internalDB->GetSyncFolder()), _internalDB->GetSyncFolder().length());

  auto fileMeta = _internalDB->GetFile(oldFilePath, oldPath.stem().string(), oldPath.extension().string());
  fileMeta.fileName = newPath.stem().string();
  fileMeta.fileExtension = newPath.extension().string();
  fileMeta.filePath = newFilePath;
  fileMeta.isCurrent = true;
  fileMeta.isDeleted = false;
  fileMeta.updateDate = boost::lexical_cast<std::string>(std::time(nullptr));

  _internalDB->RenameAndIndexFile(fileMeta);
  return fileMeta;
}

FileInfo Indexer::GetFileInfo(const FileMeta &fileMeta, std::vector<Chunk> &chunksVector) {
  FileInfo fileInfo{
      .userId = _internalDB->GetUserId(),
      .file = fileMeta,
      .fileChunksMeta = createFileChunks(fileMeta.fileId, fileMeta.chunksCount)
  };

  for (int i = 0; i < chunksVector.size(); i++) {
    chunksVector[i].userId = fileInfo.userId;
    chunksVector[i].chunkId = fileInfo.fileChunksMeta[i].chunkId;
  }

  return fileInfo;
}

FileInfo Indexer::GetRenameFileInfo(const FileMeta &fileMeta) {
  FileInfo fileInfo{
      .userId = _internalDB->GetUserId(),
      .file = fileMeta
  };

  return fileInfo;
}

FileInfo Indexer::GetDeleteFileInfo(const FileMeta &fileMeta) {
  FileInfo fileInfo{
      .userId = _internalDB->GetUserId(),
      .file = fileMeta
  };

  return fileInfo;
}

std::vector<FileChunksMeta> Indexer::createFileChunks(const int &fileId, const int &chunksCount) {
  std::vector<FileChunksMeta> fileChunksMeta;
  fileChunksMeta.reserve(chunksCount);

  for (int i = 0; i < chunksCount; ++i) {
    fileChunksMeta.push_back(FileChunksMeta{.chunkOrder = i});
    _internalDB->InsertAndIndexFileChunk(fileChunksMeta[i], fileId);
  }

  return fileChunksMeta;
}
