#include "InternalDB.h"
#include <iomanip>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

InternalDB::InternalDB(std::string databaseName)
    : _databaseName(std::move(databaseName)),
      _userId(-1),
      _deviceId(-1),
      _syncFolder(""),
      _lastUpdate("") {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Init DB";
  if (connect()) {
    createTable();

    if (IsExistUser()) {
      _userId = selectUserId();
      _deviceId = selectDeviceId();
      _syncFolder = selectFolder();
      _lastUpdate = selectLastUpdate();
      _login = selectLogin();
    }

    close();
  }
}

int InternalDB::GetUserId() const {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetUserId";
  return _userId;
}

int InternalDB::GetDeviceId() const {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetDeviceId";
  return _deviceId;
}

std::string InternalDB::GetLogin() {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetLogin";
  return _login;
}

std::string InternalDB::GetPassword() {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetLogin";
  return selectPassword();
}

void InternalDB::InsertUser(const User &user) {
  if (!connect()) { throw InternalExceptions("Don't connect"); }
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Insert User";
  std::string
      query = "INSERT INTO User (user_id, login, password, device_id, device_name, sync_folder, last_update) VALUES ("
      + std::to_string(user.userId)
      + ", '" + user.login
      + "', '" + user.password
      + "', " + std::to_string(user.deviceId)
      + ", '" + user.deviceName
      + "', '" + user.syncFolder
      + "', '1970-12-31 12:30:02"
      + "');";
  insert(query);
  _userId = user.userId;
  _deviceId = user.deviceId;
  _login = user.login;
  _syncFolder = user.syncFolder;
  _lastUpdate = "1970-12-31 12:30:02";
  close();
}

std::string InternalDB::GetSyncFolder() const {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetSyncFolder";
  return _syncFolder;
}

void InternalDB::UpdateSyncFolder(const std::string &newFolder) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: UpdateSyncFolder";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  BOOST_LOG_TRIVIAL(info) << "InternalDB: Prepare to update sync folder";
  auto query = "Update User set sync_folder = \"" + newFolder + "\" where user_id = " + std::to_string(_userId) + ";";
  if (update(query)) {
    _syncFolder = newFolder;
  }

  close();
}

UserDate InternalDB::GetLastUpdate() {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetLastUpdate";

  auto ttime = time(nullptr);
  auto *local_time = localtime(&ttime);
  _lastTMPUpdate = std::to_string(1900 + local_time->tm_year) + "-" + std::to_string(1 + local_time->tm_mon) + "-"
      + std::to_string(local_time->tm_mday) + " " + std::to_string(local_time->tm_hour) + ":"
      + std::to_string(local_time->tm_min) + ":" + std::to_string(local_time->tm_sec);

  return UserDate{.userId = _userId, .date = _lastUpdate};
}

void InternalDB::SaveLastUpdate() {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetLastUpdate";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  BOOST_LOG_TRIVIAL(info) << "InternalDB: Prepare to save last update";
  auto query = "Update User set last_update = \""
      + boost::lexical_cast<std::string>(_lastTMPUpdate)
      + "\" where user_id = " + std::to_string(_userId) + ";";

  update(query);
  _lastUpdate = boost::lexical_cast<std::string>(_lastTMPUpdate);

  close();
}

bool InternalDB::IsFileExist(const int &fileId) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: IsFileExist";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  auto query = "SELECT count(*) FROM Files Where id=" + std::to_string(fileId) + ";";
  BOOST_LOG_TRIVIAL(info) << "InternalDB: Check exist file id=" + std::to_string(fileId);
  int count = selectId(query);

  close();

  return count != 0;
}

int InternalDB::GetFileId(const std::string &path, const std::string &name, const std::string &extension) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetFileId";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  auto query = "SELECT id FROM Files Where file_path like '" + path + "' and file_name like '" + name
      + "' and file_extention like '" + extension + "';";
  auto id = selectId(query);
  close();

  return id;
}

FileMeta InternalDB::GetFile(const std::string &path, const std::string &name, const std::string &extension) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetFile";
  return SelectFile(GetFileId(path, name, extension));
}

std::vector<FileMeta> InternalDB::SelectAllFiles() {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetFile";

  std::vector<FileMeta> files;

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  std::string query = "SELECT * FROM Files;";
  auto pStmt = _stmt.get();
  sqlite3_prepare_v2(_database.get(), query.c_str(), query.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  while (sqlite3_step(_stmt.get()) == SQLITE_ROW) {
    files.push_back(getOneFile());
    BOOST_LOG_TRIVIAL(debug) << "InternalDB: Selected";
  }

  close();
  return files;
}

std::vector<UserChunk> InternalDB::GetUserChunks(const int &fileId) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetUserChunks";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  std::vector<UserChunk> userChunks;
  std::string query = "SELECT id FROM Chunks Where id_file = " + std::to_string(fileId) + ";";
  auto pStmt = _stmt.get();
  sqlite3_prepare_v2(_database.get(), query.c_str(), query.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  while (sqlite3_step(_stmt.get()) == SQLITE_ROW) {
    userChunks.emplace_back(UserChunk{_userId, sqlite3_column_int(_stmt.get(), 0)});
    BOOST_LOG_TRIVIAL(debug) << "InternalDB: Selected";
  }

  close();
  return userChunks;
}

std::vector<FileChunksMeta> InternalDB::GetFileChunksMeta(const int &fileId) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: GetFileChunksMeta";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  std::vector<FileChunksMeta> fileChunks;
  std::string query = "SELECT id, chunk_order FROM Chunks Where id_file = " + std::to_string(fileId) + ";";
  auto pStmt = _stmt.get();
  sqlite3_prepare_v2(_database.get(), query.c_str(), query.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  while (sqlite3_step(_stmt.get()) == SQLITE_ROW) {
    fileChunks.emplace_back(FileChunksMeta{sqlite3_column_int(_stmt.get(), 0), sqlite3_column_int(_stmt.get(), 1)});
    BOOST_LOG_TRIVIAL(debug) << "InternalDB: Selected";
  }
  close();

  return fileChunks;
}

void InternalDB::DeleteFile(const int &fileId) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: DeleteFile";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  auto query = "DELETE FROM Files WHERE id = " + std::to_string(fileId) + ";";
  deleteInfo(query);
  query = "DELETE FROM Chunks WHERE id_file = " + std::to_string(fileId) + ";";
  deleteInfo(query);

  close();
}

void InternalDB::DownloadFile(const int &fileId) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: DownloadFile";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  auto query = "UPDATE Files set is_download = true where id = " +
      std::to_string(fileId) + " ;";
  update(query);

  close();
}

void InternalDB::InsertAndIndexFile(FileMeta &file) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: InsertAndIndexFile";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  file.version = 1;
  auto date = getTime(file.updateDate);
  auto query = "INSERT INTO Files (file_name, file_extention, file_size, file_path,"
               " count_chunks, version, is_download, update_date, create_date) VALUES ('"
      + file.fileName + "', '" + file.fileExtension
      + "', " + std::to_string(file.fileSize)
      + ", '" + file.filePath + "', " +
      std::to_string(file.chunksCount) + ", 1, "
      + std::to_string(file.isDownload) + ", '"
      + date + "', '" + date + "');";
  file.updateDate = date;
  file.createDate = date;

  insert(query);

  file.fileId = selectId("SELECT id FROM Files ORDER  BY  id  DESC Limit 1");
  close();
}

void InternalDB::UpdateAndIndexFile(FileMeta &file) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: UpdateAndIndexFile";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  file.version++;
  auto date = getTime(file.updateDate);
  auto query = "Update Files SET "
               "file_name = '" + file.fileName +
      "', file_extention = '" + file.fileExtension +
      "', file_size = " + std::to_string(file.fileSize) +
      ",file_path = '" + file.filePath +
      "', count_chunks = " + std::to_string(file.chunksCount) +
      ", version=" + std::to_string(file.version) +
      ", update_date = '" + date +
      "'" " WHERE id=" + std::to_string(file.fileId) + ";";
  file.updateDate = date;

  update(query);

  query = "DELETE FROM Chunks WHERE id_file = " + std::to_string(file.fileId) + ";";
  deleteInfo(query);

  close();
}

void InternalDB::DeleteAndIndexFile(FileMeta &file) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: DeleteAndIndexFile";

  file.version++;
  file.updateDate = getTime(file.updateDate);
  DeleteFile(file.fileId);
}

void InternalDB::RenameAndIndexFile(FileMeta &file) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: UpdateAndIndexFile";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  file.version++;
  auto date = getTime(file.updateDate);
  auto query = "Update Files SET "
               "file_name = '" + file.fileName +
      "', file_extention = '" + file.fileExtension +
      "', file_size = " + std::to_string(file.fileSize) +
      ",file_path = '" + file.filePath +
      "', count_chunks = " + std::to_string(file.chunksCount) +
      ", version=" + std::to_string(file.version) +
      ", update_date = '" + date +
      "'" " WHERE id=" + std::to_string(file.fileId) + ";";
  file.updateDate = date;

  update(query);
  close();
}

void InternalDB::InsertAndIndexFileChunk(FileChunksMeta &fileChunk, const int &fileId) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: InsertAndIndexFileChunk";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  auto query = "INSERT INTO Chunks (id_file, chunk_order) VALUES ("
      + std::to_string(fileId) + ", "
      + std::to_string(fileChunk.chunkOrder) + ");";
  insert(query);

  fileChunk.chunkId = selectId("SELECT id FROM Chunks ORDER  BY  id  DESC Limit 1");;

  close();
}

void InternalDB::InsertFileInfo(const FileInfo &fileInfo) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: InsertFileInfo";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  auto query = "DELETE FROM Chunks WHERE id_file = " + std::to_string(fileInfo.file.fileId) + ";";
  deleteInfo(query);

  insertFileMeta(fileInfo.file);
  insertFileChunksMeta(fileInfo.fileChunksMeta, fileInfo.file.fileId);

  close();
}

void InternalDB::UpdateFileInfo(const FileInfo &fileInfo) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: UpdateFileInfo";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  auto query = "DELETE FROM Chunks WHERE id_file = " + std::to_string(fileInfo.file.fileId) + ";";
  deleteInfo(query);

  updateFileMeta(fileInfo.file);
  insertFileChunksMeta(fileInfo.fileChunksMeta, fileInfo.file.fileId);

  close();
}

void InternalDB::RenameFileInfo(const FileInfo &fileInfo) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: RenameFileInfo";

  if (!connect()) { throw InternalExceptions("Don't connect"); }

  updateFileMeta(fileInfo.file);

  close();
}

FileMeta InternalDB::SelectFile(const int &fileId) {
  FileMeta file;
  if (!connect()) { throw InternalExceptions("Don't connect"); }
  std::string query = "SELECT * FROM Files Where id = " + std::to_string(fileId) + ";";
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Select File by id = " << std::to_string(fileId);
  auto pStmt = _stmt.get();
  sqlite3_prepare_v2(_database.get(), query.c_str(), query.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);

  if (sqlite3_step(_stmt.get()) == SQLITE_ROW) {
    file = getOneFile();
    BOOST_LOG_TRIVIAL(debug) << "InternalDB: Selected";
  } else {
    BOOST_LOG_TRIVIAL(error) << "File by id = " + std::to_string(fileId) + " don't exist";;
    throw InternalExceptions("File by id = " + std::to_string(fileId) + " don't exist");
  }

  close();
  return file;
}

// MARK: Работа с User

void InternalDB::DeleteUser() {
  if (!connect()) { throw InternalExceptions("Don't connect"); }

  std::string query = "DELETE FROM User ;";
  deleteInfo(query);

  close();
}

void InternalDB::UpdatePassword(const std::string &newPassword) {
  if (!connect()) { throw InternalExceptions("Don't connect"); }
  std::string
      query = "Update User set password = \"" + newPassword + "\" where user_id = " + std::to_string(_userId) + ";";
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Prepare to update password";
  update(query);
  close();
}

bool InternalDB::IsExistUser() {
  if (!connect()) { throw InternalExceptions("Don't connect"); }

  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Check exist user";
  auto query = "SELECT count(*) FROM User;";

  auto id = selectId(query);
  close();

  return id != 0;
}

int InternalDB::selectUserId() {
  std::string query = "SELECT user_id FROM User;";
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Prepare to select user_id";
  return selectId(query);
}

int InternalDB::selectDeviceId() {
  std::string query = "SELECT device_id FROM User;";
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Prepare to select device_id";
  return selectId(query);
}

std::string InternalDB::selectLogin() {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: selectLogin";

  auto query = "SELECT login FROM User;";
  BOOST_LOG_TRIVIAL(info) << "InternalDB: Prepare to select login";

  return selectStr(query);
}

std::string InternalDB::selectPassword() {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: selectPassword";

  auto query = "Select password from User where user_id = " + std::to_string(_userId) + ";";
  BOOST_LOG_TRIVIAL(info) << "InternalDB: Prepare to select password";

  return selectStr(query);
}

std::string InternalDB::selectFolder() {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: selectFolder";

  auto query = "SELECT sync_folder FROM User;";
  BOOST_LOG_TRIVIAL(info) << "InternalDB: Prepare to select sync_folder";
  auto folder = selectStr(query);

  return folder;
}

std::string InternalDB::selectLastUpdate() {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Prepare to select last update";
  std::string query = "SELECT last_update FROM User;";
  std::string date = selectStr(query);
  return date;
}

FileMeta InternalDB::getOneFile() {
  FileMeta file;
  file.fileId = sqlite3_column_int(_stmt.get(), 0);
  file.fileName = boost::lexical_cast<std::string>(sqlite3_column_text(_stmt.get(), 1));
  file.fileExtension = boost::lexical_cast<std::string>(sqlite3_column_text(_stmt.get(), 2));
  file.fileSize = sqlite3_column_int(_stmt.get(), 3);
  file.filePath = boost::lexical_cast<std::string>(sqlite3_column_text(_stmt.get(), 4));
  file.chunksCount = sqlite3_column_int(_stmt.get(), 5);
  file.version = sqlite3_column_int(_stmt.get(), 6);
  file.isDownload = sqlite3_column_int(_stmt.get(), 7);
  file.updateDate = boost::lexical_cast<std::string>(sqlite3_column_text(_stmt.get(), 8));
  file.createDate = boost::lexical_cast<std::string>(sqlite3_column_text(_stmt.get(), 9));
  return file;
}

void InternalDB::insertFileMeta(const FileMeta &fileMeta) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: insertFileMeta";

  auto query = "INSERT INTO Files (file_name, file_extention, file_size, file_path,"
               " count_chunks, version, is_download, update_date, create_date) VALUES ('"
      + fileMeta.fileName + "', '" + fileMeta.fileExtension
      + "', " + std::to_string(fileMeta.fileSize)
      + ", '" + fileMeta.filePath + "', " +
      std::to_string(fileMeta.chunksCount) + ", " + std::to_string(fileMeta.version) + ", "
      + std::to_string(fileMeta.isDownload) + ", '"
      + fileMeta.updateDate + "', '" + fileMeta.createDate + "');";

  insert(query);
}

void InternalDB::updateFileMeta(const FileMeta &fileMeta) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: updateFileMeta";

  auto query = "Update Files SET "
               "file_name = '" + fileMeta.fileName +
      "', file_extention = '" + fileMeta.fileExtension +
      "', file_size = " + std::to_string(fileMeta.fileSize) +
      ", file_path = '" + fileMeta.filePath +
      "', count_chunks = " + std::to_string(fileMeta.chunksCount) +
      ", version=" + std::to_string(fileMeta.version) +
      ", is_download=" + std::to_string(fileMeta.isDownload) +
      ", update_date = '" + fileMeta.updateDate +
      "', create_date = '" + fileMeta.createDate +
      "'" " WHERE id=" + std::to_string(fileMeta.fileId) + ";";

  update(query);
}

void InternalDB::insertFileChunksMeta(const std::vector<FileChunksMeta> &fileChunksMeta, const int &fileId) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: insertFileChunks";

  std::for_each(fileChunksMeta.begin(), fileChunksMeta.end(), [&](const auto &fileChunk) {
    insertOneFileChunkMeta(fileChunk, fileId);
  });
}

void InternalDB::insertOneFileChunkMeta(const FileChunksMeta &fileChunk, const int &fileId) {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: insertOneFileChunk";

  auto query = "INSERT INTO Chunks (id_file, chunk_order) VALUES ("
      + std::to_string(fileId) + ", "
      + std::to_string(fileChunk.chunkOrder) + ");";

  insert(query);
}

std::string InternalDB::getTime(std::string &time) {
  time.erase(std::find(time.begin(), time.end(), '.'), time.end());

  std::time_t ttime;
  try {
    ttime = boost::lexical_cast<int>(time);
  } catch (std::exception &exception) {
    struct std::tm tm{};
    std::istringstream ss(time);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    ttime = mktime(&tm);
  }

  auto *local_time = localtime(&ttime);
  std::string date = std::to_string(1900 + local_time->tm_year) + "-" + std::to_string(1 + local_time->tm_mon) + "-"
      + std::to_string(local_time->tm_mday) + " " + std::to_string(local_time->tm_hour) + ":"
      + std::to_string(local_time->tm_min) + ":" + std::to_string(local_time->tm_sec);
  return date;
}

// MARK: Базовые функции

void InternalDB::insert(const std::string &query) {
  auto pStmt = _stmt.get();
  sqlite3_prepare_v2(_database.get(), query.c_str(), query.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  if (sqlite3_step(_stmt.get()) != SQLITE_DONE) {
    BOOST_LOG_TRIVIAL(error) << "InternalDB: Don't Insert";
    throw InternalExceptions("Don't insert");
  }
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Inserted";
}

bool InternalDB::update(const std::string &query) {
  auto pStmt = _stmt.get();
  sqlite3_prepare_v2(_database.get(), query.c_str(), query.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  if (sqlite3_step(_stmt.get()) != SQLITE_DONE) {
    BOOST_LOG_TRIVIAL(error) << "InternalDB: Don't Update";
    return false;
  }
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Updated";
  return true;
}

void InternalDB::deleteInfo(const std::string &query) {
  auto pStmt = _stmt.get();
  sqlite3_prepare_v2(_database.get(), query.c_str(), query.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  if (sqlite3_step(_stmt.get()) != SQLITE_DONE) {
    BOOST_LOG_TRIVIAL(error) << "InternalDB: Don't Deleted";
    return;
  }
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Deleted";
}

int InternalDB::selectId(const std::string &query) {
  int id = -1;
  auto pStmt = _stmt.get();
  sqlite3_prepare_v2(_database.get(), query.c_str(), query.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  while (sqlite3_step(_stmt.get()) == SQLITE_ROW) {
    id = sqlite3_column_int(_stmt.get(), 0);
    BOOST_LOG_TRIVIAL(debug) << "InternalDB: Selected";
  }
  return id;
}

std::string InternalDB::selectStr(const std::string &query) {
  std::string string;
  auto pStmt = _stmt.get();
  sqlite3_prepare_v2(_database.get(), query.c_str(), query.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  while (sqlite3_step(_stmt.get()) == SQLITE_ROW) {
    string = boost::lexical_cast<std::string>(sqlite3_column_text(_stmt.get(), 0));
    BOOST_LOG_TRIVIAL(debug) << "InternalDB: Selected";
  }
  return string;
}

// MARK: Подключение к Базе Данных

bool InternalDB::connect() {
  auto pDB = _database.get();
  if (SQLITE_OK != sqlite3_open_v2(_databaseName.c_str(), &pDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr)) {
    BOOST_LOG_TRIVIAL(error) << "InternalDB: Failed to connect";
    return false;
  }
  _database.reset(pDB);
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Connect";
  return true;
}

// MARK: Отключение  Базы Данных

void InternalDB::close() {
  BOOST_LOG_TRIVIAL(debug) << "InternalDB: Close";
  sqlite3_shutdown();
}

// MARK: Создание табицы

void InternalDB::createTable() {
  auto pStmt = _stmt.get();
  sqlite3_prepare(_database.get(), createQueryFiles.c_str(), createQueryFiles.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  if (sqlite3_step(_stmt.get()) != SQLITE_DONE) {
    std::string error = "InternalDB: Didn't Create Table Files";
    BOOST_LOG_TRIVIAL(error) << error;
    throw InternalExceptions(error);
  }
  sqlite3_prepare(_database.get(), createQueryChunks.c_str(), createQueryChunks.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  if (sqlite3_step(_stmt.get()) != SQLITE_DONE) {
    std::string error = "InternalDB: Didn't Create Table Chunks";
    BOOST_LOG_TRIVIAL(error) << error;
    throw InternalExceptions(error);
  }
  sqlite3_prepare(_database.get(), createQueryUser.c_str(), createQueryUser.size(), &pStmt, nullptr);
  _stmt.reset(pStmt);
  if (sqlite3_step(_stmt.get()) != SQLITE_DONE) {
    std::string error = "InternalDB: Didn't Create Table Users";
    BOOST_LOG_TRIVIAL(error) << error;
    throw InternalExceptions(error);
  }
}
