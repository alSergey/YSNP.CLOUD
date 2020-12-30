#pragma once

#include <sqlite3.h>
#include <string>
#include <memory>
#include <vector>
#include "User.h"
#include "InternalExceptions.h"
#include "structs/FileChunksMeta.h"
#include "structs/FileMeta.h"
#include "structs/UserDate.h"
#include "structs/FileInfo.h"
#include "structs/UserChunk.h"
#include "structs/Chunk.h"
#include "SQLiteQuery.h"

struct sqlite3_deleter {
  void operator()(sqlite3 *sql) {
    sqlite3_close_v2(sql);
  }
};

struct sqlite3_stmt_deleter {
  void operator()(sqlite3_stmt *sql) {
    sqlite3_finalize(sql);
  }
};

class InternalDB {
 public:
  explicit InternalDB(std::string databaseName);
  int GetUserId() const;
  int GetDeviceId() const;
  std::string GetLogin();
  std::string GetPassword();
  void InsertUser(const User &user);
  std::string GetSyncFolder() const;
  void UpdateSyncFolder(const std::string &newFolder);

  UserDate GetLastUpdate();
  void SaveLastUpdate();

  bool IsFileExist(const int &fileId);
  int GetFileId(const std::string &path, const std::string &name, const std::string &extension);
  FileMeta GetFile(const std::string &path, const std::string &name, const std::string &extension);
  FileMeta SelectFile(const int &fileId);
  std::vector<FileMeta> SelectAllFiles();

  std::vector<UserChunk> GetUserChunks(const int &fileId);
  std::vector<FileChunksMeta> GetFileChunksMeta(const int &fileId);

  void DeleteFile(const int &fileId);
  void DownloadFile(const int &fileId);

  void InsertAndIndexFile(FileMeta &file);
  void UpdateAndIndexFile(FileMeta &file);
  void DeleteAndIndexFile(FileMeta &file);
  void RenameAndIndexFile(FileMeta &file);
  void InsertAndIndexFileChunk(FileChunksMeta &fileChunk, const int &fileId);

  void InsertFileInfo(const FileInfo &fileInfo);
  void UpdateFileInfo(const FileInfo &fileInfo);
  void RenameFileInfo(const FileInfo &fileInfo);

  bool IsExistUser();
  void DeleteUser();
  void UpdatePassword(const std::string &newPassword);

 private:
  int selectUserId();
  int selectDeviceId();
  std::string selectLogin();
  std::string selectPassword();
  std::string selectFolder();
  std::string selectLastUpdate();

  FileMeta getOneFile();
  void insertFileMeta(const FileMeta &fileMeta);
  void updateFileMeta(const FileMeta &fileMeta);
  void insertFileChunksMeta(const std::vector<FileChunksMeta> &fileChunksMeta, const int &fileId);
  void insertOneFileChunkMeta(const FileChunksMeta &fileChunk, const int &fileId);

  static std::string getTime(std::string &time);

  void insert(const std::string &query);
  bool update(const std::string &query);
  void deleteInfo(const std::string &query);
  int selectId(const std::string &query);
  std::string selectStr(const std::string &query);

  bool connect();
  void close();
  void createTable();

 private:
  std::string _databaseName;
  int _userId;
  int _deviceId;
  std::string _login;
  std::string _syncFolder;
  std::string _lastUpdate;
  std::string _lastTMPUpdate;
  std::unique_ptr<sqlite3, sqlite3_deleter> _database;
  std::unique_ptr<sqlite3_stmt, sqlite3_stmt_deleter> _stmt;
};
