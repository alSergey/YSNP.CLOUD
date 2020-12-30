#include "Commands.h"
#include <boost/log/trivial.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

BaseCommand::BaseCommand(std::function<void(const std::string &msg)> callbackOk,
                         std::function<void(const std::string &msg)> callbackError,
                         std::shared_ptr<InternalDB> internalDB)
    : callbackOk(std::move(callbackOk)),
      callbackError(std::move(callbackError)),
      _internalDB(std::move(internalDB)) {
  BOOST_LOG_TRIVIAL(debug) << "BaseCommand: create command";
}

void BaseCommand::sendAndReceive(ClientNetwork &network,
                                 const NetworkConfig &config,
                                 const pt::ptree &request,
                                 pt::ptree &response) noexcept(false) {
  try {
    network.Connect(config.host, config.port);
    network.SendJSON(request);
    response = network.ReceiveJSON();
    BOOST_LOG_TRIVIAL(info) << "BaseCommand: send and receive";
  } catch (ClientNetworkExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "BaseCommand: " << er.what();
    throw NetworkException(er.what());
  }
  network.Disconnect();
}

bool BaseCommand::visitAnswer(const std::variant<StatusOk, StatusError> &response) {
  bool isError = false;
  std::visit(overloaded{
      [&](const StatusOk &val) {
        BOOST_LOG_TRIVIAL(info) << "Visit Answer: Status Ok sync";
      },
      [&](const StatusError &val) {
        BOOST_LOG_TRIVIAL(error) << "Visit Answer: Status Error sync";
        auto error = std::get<StatusError>(response);
        callbackError(error.msg);
        isError = true;
      }
  }, response);

  return isError;
}

LoginUserCommand::LoginUserCommand(std::function<void(const std::string &)> callbackOk,
                                   std::function<void(const std::string &)> callbackError,
                                   std::shared_ptr<InternalDB> internalDB,
                                   std::string login,
                                   std::string password)
    : BaseCommand(std::move(callbackOk), std::move(callbackError), std::move(internalDB)),
      _login(std::move(login)),
      _password(std::move(password)) {
  BOOST_LOG_TRIVIAL(debug) << "LoginUserCommand: create command";
}

void LoginUserCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "LoginUserCommand: Do";

  auto syncConfig = ClientConfig::getSyncConfig();
  auto network = ClientNetwork();

  auto request = SerializerUserInfo(0, UserInfo{.login = _login, .password = _password}, "LoginUser").GetJson();

  std::stringstream ssRequest;
  pt::write_json(ssRequest, request);
  BOOST_LOG_TRIVIAL(info) << "LoginUserCommand: request " << ssRequest.str();

  pt::ptree response;
  try {
    sendAndReceive(network, syncConfig, request, response);
  } catch (NetworkException &er) {
    callbackError(er.what());
    return;
  }

  std::stringstream ssResponse;
  pt::write_json(ssResponse, response);
  BOOST_LOG_TRIVIAL(info) << "LoginUserCommand: response " << ssResponse.str();

  try {
    auto userIds = SerializerUserIds(response).GetUserIds();

    std::string path = std::getenv("HOME");
    path += "/cloud";
    boost::filesystem::create_directories(path);
    _internalDB->InsertUser(User{
        .userId = userIds.id,
        .login = _login,
        .password = _password,
        .syncFolder = path});

    callbackOk("Добро пожаловать!");
    return;
  } catch (ParseException &er) {
    BOOST_LOG_TRIVIAL(error) << "LoginUserCommand: " << er.what();
    auto responseSerializer = SerializerAnswer(response);
    auto error = std::get<StatusError>(responseSerializer.GetStatus());
    callbackError(error.msg);
    return;
  }
}

RegisterUserCommand::RegisterUserCommand(std::function<void(const std::string &)> callbackOk,
                                         std::function<void(const std::string &)> callbackError,
                                         std::shared_ptr<InternalDB> internalDB,
                                         std::string login,
                                         std::string password)
    : BaseCommand(std::move(callbackOk), std::move(callbackError), std::move(internalDB)),
      _login(std::move(login)),
      _password(std::move(password)) {
  BOOST_LOG_TRIVIAL(debug) << "RegisterUserCommand: create command";
}

void RegisterUserCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "RegisterUserCommand: Do";

  auto syncConfig = ClientConfig::getSyncConfig();
  auto network = ClientNetwork();

  auto request = SerializerUserInfo(0, UserInfo{.login = _login, .password = _password}, "RegisterUser").GetJson();

  std::stringstream ssRequest;
  pt::write_json(ssRequest, request);
  BOOST_LOG_TRIVIAL(info) << "RegisterUserCommand: request " << ssRequest.str();

  pt::ptree response;
  try {
    sendAndReceive(network, syncConfig, request, response);
  } catch (NetworkException &er) {
    callbackError(er.what());
    return;
  }

  std::stringstream ssResponse;
  pt::write_json(ssResponse, response);
  BOOST_LOG_TRIVIAL(info) << "RegisterUserCommand: response " << ssResponse.str();

  try {
    auto userIds = SerializerUserIds(response).GetUserIds();

    std::string path = std::getenv("HOME");
    path += "/cloud";
    boost::filesystem::create_directories(path);
    _internalDB->InsertUser(User{
        .userId = userIds.id,
        .login = _login,
        .password = _password,
        .syncFolder = path});

    callbackOk("Вы зарегестрированны");
    return;
  } catch (ParseException &er) {
    BOOST_LOG_TRIVIAL(error) << "RegisterUserCommand: " << er.what();
    auto responseSerializer = SerializerAnswer(response);
    auto error = std::get<StatusError>(responseSerializer.GetStatus());
    callbackError(error.msg);
    return;
  }
}

RefreshCommand::RefreshCommand(std::function<void(const std::string &msg)> callbackOk,
                               std::function<void(const std::string &msg)> callbackError,
                               std::shared_ptr<InternalDB> internalDB,
                               std::function<void()> stopWatcher,
                               std::function<void()> runWatcher)
    : BaseCommand(std::move(callbackOk), std::move(callbackError), std::move(internalDB)),
      stopWatcher(std::move(stopWatcher)),
      runWatcher(std::move(runWatcher)) {
  BOOST_LOG_TRIVIAL(debug) << "RefreshCommand: create command";
}

void RefreshCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "RefreshCommand: Do";

  auto syncConfig = ClientConfig::getSyncConfig();
  auto network = ClientNetwork();

  auto userDate = _internalDB->GetLastUpdate();
  auto request = SerializerUserDate(0, userDate).GetJson();

  std::stringstream ssRequest;
  pt::write_json(ssRequest, request);
  BOOST_LOG_TRIVIAL(info) << "RefreshCommand: request " << ssRequest.str();

  pt::ptree response;
  try {
    sendAndReceive(network, syncConfig, request, response);
  } catch (NetworkException &er) {
    callbackError(er.what());
    return;
  }

  std::stringstream ssResponse;
  pt::write_json(ssResponse, response);
  BOOST_LOG_TRIVIAL(info) << "RefreshCommand: response " << ssResponse.str();

  try {
    auto fileInfo = SerializerFileInfo(response).GetFileInfo();
    BOOST_LOG_TRIVIAL(info) << "RefreshCommand: get fileInfo";

    std::for_each(fileInfo.begin(), fileInfo.end(), [&](FileInfo &oneFileInfo) {
      oneFileInfo.file.isDownload = false;

      if (oneFileInfo.file.isDeleted) {
        File::Delete(_internalDB->GetSyncFolder() + oneFileInfo.file.GetFilePath());
        _internalDB->DeleteFile(oneFileInfo.file.fileId);
        return;
      }

      if (_internalDB->IsFileExist(oneFileInfo.file.fileId)) {
        auto fileDB = _internalDB->SelectFile(oneFileInfo.file.fileId);
        if (fileDB.isDownload) {
          stopWatcher();
          File::Delete(_internalDB->GetSyncFolder() + fileDB.GetFilePath());
          runWatcher();
        }

        if (oneFileInfo.fileChunksMeta.empty()) {
          _internalDB->RenameFileInfo(oneFileInfo);
          return;
        }

        _internalDB->UpdateFileInfo(oneFileInfo);
        return;
      }

      _internalDB->InsertFileInfo(oneFileInfo);
    });

    _internalDB->SaveLastUpdate();
    callbackOk("Обновление завершено");
    return;
  } catch (ParseException &er) {
    BOOST_LOG_TRIVIAL(error) << "RefreshCommand: " << er.what();
    auto responseSerializer = SerializerAnswer(response);
    auto error = std::get<StatusError>(responseSerializer.GetStatus());
    callbackError(error.msg);
    return;
  }
}

DownloadFileCommand::DownloadFileCommand(std::function<void(const std::string &msg)> callbackOk,
                                         std::function<void(const std::string &msg)> callbackError,
                                         std::shared_ptr<InternalDB> internalDB,
                                         FileMeta file,
                                         std::function<void()> stopWatcher,
                                         std::function<void()> runWatcher)
    : BaseCommand(std::move(callbackOk), std::move(callbackError), std::move(internalDB)),
      _file(std::move(file)),
      stopWatcher(std::move(stopWatcher)),
      runWatcher(std::move(runWatcher)) {
  BOOST_LOG_TRIVIAL(debug) << "DownloadFileCommand: create command";
}

void DownloadFileCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "DownloadFileCommand: Do";

  auto storageConfig = ClientConfig::getStorageConfig();
  auto network = ClientNetwork();

  auto userChunkVector = _internalDB->GetUserChunks(_file.fileId);
  auto request = SerializerUserChunk(0, userChunkVector).GetJson();

  std::stringstream ssRequest;
  pt::write_json(ssRequest, request);
  BOOST_LOG_TRIVIAL(info) << "DownloadFileCommand: request " << ssRequest.str();

  pt::ptree response;
  try {
    sendAndReceive(network, storageConfig, request, response);
  } catch (NetworkException &er) {
    callbackError(er.what());
    return;
  }

  std::stringstream ssResponse;
  pt::write_json(ssResponse, response);
  BOOST_LOG_TRIVIAL(info) << "DownloadFileCommand: response " << ssResponse.str();

  try {
    auto chunks = SerializerChunk(response).GetChunk();
    BOOST_LOG_TRIVIAL(info) << "DownloadFileCommand: get chunks";

    auto filePath = _internalDB->GetSyncFolder() + _file.GetFilePath();
    BOOST_LOG_TRIVIAL(info) << "DownloadFileCommand: download file " << filePath;

    stopWatcher();
    File file(filePath);
    Chunker chunker(file);
    chunker.MergeFile(chunks);
    runWatcher();

    _internalDB->DownloadFile(_file.fileId);
    callbackOk("Загрузка завершена");
    return;
  } catch (ParseException &er) {
    BOOST_LOG_TRIVIAL(error) << "DownloadFileCommand: " << er.what();
    auto responseSerializer = SerializerAnswer(response);
    auto error = std::get<StatusError>(responseSerializer.GetStatus());
    callbackError(error.msg);
    return;
  }
}

CreateFileCommand::CreateFileCommand(std::function<void(const std::string &)> callbackOk,
                                     std::function<void(const std::string &)> callbackError,
                                     std::shared_ptr<InternalDB> internalDB,
                                     fs::path path)
    : BaseCommand(std::move(callbackOk), std::move(callbackError), std::move(internalDB)),
      _filePath(std::move(path)) {
  BOOST_LOG_TRIVIAL(debug) << "CreateFileCommand: create command";
}

void CreateFileCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "CreateFileCommand: do";

  auto storageConfig = ClientConfig::getStorageConfig();
  auto syncConfig = ClientConfig::getSyncConfig();
  auto network = ClientNetwork();

  File file(_filePath.string());
  Chunker chunker(file);
  auto chunkVector = chunker.ChunkFile();

  Indexer indexer(_internalDB);
  auto fileMeta = indexer.CreateFile(_filePath, chunkVector.size());
  auto fileInfo = indexer.GetFileInfo(fileMeta, chunkVector);

  auto storageRequest = SerializerChunk(0, chunkVector).GetJson();

  std::stringstream ssRequestStorage;
  pt::write_json(ssRequestStorage, storageRequest);
  BOOST_LOG_TRIVIAL(info) << "CreateFileCommand: request storage " << ssRequestStorage.str();

  pt::ptree responseStorage;
  try {
    sendAndReceive(network, storageConfig, storageRequest, responseStorage);
  } catch (NetworkException &er) {
    callbackError(er.what());
    return;
  }

  std::stringstream ssResponseStorage;
  pt::write_json(ssResponseStorage, responseStorage);
  BOOST_LOG_TRIVIAL(info) << "CreateFileCommand: response storage " << ssResponseStorage.str();

  if (visitAnswer(SerializerAnswer(responseStorage).GetStatus())) {
    return;
  }

  auto syncRequest = SerializerFileInfo(0, fileInfo).GetJson();

  std::stringstream ssRequestSync;
  pt::write_json(ssRequestSync, syncRequest);
  BOOST_LOG_TRIVIAL(info) << "RenameFileCommand: request sync " << ssRequestSync.str();

  pt::ptree responseSync;
  try {
    sendAndReceive(network, syncConfig, syncRequest, responseSync);
  } catch (NetworkException &er) {
    callbackError(er.what());
    return;
  }

  std::stringstream ssResponseSync;
  pt::write_json(ssResponseSync, responseSync);
  BOOST_LOG_TRIVIAL(info) << "CreateFileCommand: response sync " << ssResponseSync.str();

  if (visitAnswer(SerializerAnswer(responseSync).GetStatus())) {
    return;
  }

  callbackOk("Файл обновлен");
}

ModifyFileCommand::ModifyFileCommand(std::function<void(const std::string &)> callbackOk,
                                     std::function<void(const std::string &)> callbackError,
                                     std::shared_ptr<InternalDB> internalDB,
                                     fs::path path)
    : BaseCommand(std::move(callbackOk), std::move(callbackError), std::move(internalDB)),
      _filePath(std::move(path)) {
  BOOST_LOG_TRIVIAL(debug) << "ModifyFileCommand: create command";
}

void ModifyFileCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "ModifyFileCommand: do";

  auto storageConfig = ClientConfig::getStorageConfig();
  auto syncConfig = ClientConfig::getSyncConfig();
  auto network = ClientNetwork();

  File file(_filePath.string());
  Chunker chunker(file);
  auto chunkVector = chunker.ChunkFile();

  Indexer indexer(_internalDB);
  auto fileMeta = indexer.ModifyFile(_filePath, chunkVector.size());
  auto fileInfo = indexer.GetFileInfo(fileMeta, chunkVector);

  auto storageRequest = SerializerChunk(0, chunkVector).GetJson();

  std::stringstream ssRequestStorage;
  pt::write_json(ssRequestStorage, storageRequest);
  BOOST_LOG_TRIVIAL(info) << "ModifyFileCommand: request storage " << ssRequestStorage.str();

  pt::ptree responseStorage;
  try {
    sendAndReceive(network, storageConfig, storageRequest, responseStorage);
  } catch (NetworkException &er) {
    callbackError(er.what());
    return;
  }

  std::stringstream ssResponseStorage;
  pt::write_json(ssResponseStorage, responseStorage);
  BOOST_LOG_TRIVIAL(info) << "ModifyFileCommand: response storage " << ssResponseStorage.str();

  if (visitAnswer(SerializerAnswer(responseStorage).GetStatus())) {
    return;
  }

  auto syncRequest = SerializerFileInfo(0, fileInfo).GetJson();

  std::stringstream ssRequestSync;
  pt::write_json(ssRequestSync, syncRequest);
  BOOST_LOG_TRIVIAL(info) << "RenameFileCommand: request sync " << ssRequestSync.str();

  pt::ptree responseSync;
  try {
    sendAndReceive(network, syncConfig, syncRequest, responseSync);
  } catch (NetworkException &er) {
    callbackError(er.what());
    return;
  }

  std::stringstream ssResponseSync;
  pt::write_json(ssResponseSync, responseSync);
  BOOST_LOG_TRIVIAL(info) << "ModifyFileCommand: response sync " << ssResponseSync.str();

  if (visitAnswer(SerializerAnswer(responseSync).GetStatus())) {
    return;
  }

  callbackOk("Файл обновлен");
}

RenameFileCommand::RenameFileCommand(std::function<void(const std::string &)> callbackOk,
                                     std::function<void(const std::string &)> callbackError,
                                     std::shared_ptr<InternalDB> internalDB,
                                     fs::path oldPath,
                                     fs::path newPath)
    : BaseCommand(std::move(callbackOk), std::move(callbackError), std::move(internalDB)),
      _fileOldPath(std::move(oldPath)),
      _fileNewPath(std::move(newPath)) {
  BOOST_LOG_TRIVIAL(debug) << "RenameFileCommand: create command";
}

void RenameFileCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "RenameFileCommand: do";

  auto storageConfig = ClientConfig::getStorageConfig();
  auto syncConfig = ClientConfig::getSyncConfig();
  auto network = ClientNetwork();

  Indexer indexer(_internalDB);
  auto fileMeta = indexer.RenameFile(_fileOldPath, _fileNewPath);
  auto fileInfo = indexer.GetRenameFileInfo(fileMeta);

  auto syncRequest = SerializerFileInfo(0, fileInfo).GetJson();

  std::stringstream ssRequest;
  pt::write_json(ssRequest, syncRequest);
  BOOST_LOG_TRIVIAL(info) << "RenameFileCommand: request " << ssRequest.str();

  pt::ptree responseSync;
  try {
    sendAndReceive(network, syncConfig, syncRequest, responseSync);
  } catch (NetworkException &er) {
    callbackError(er.what());
    return;
  }

  std::stringstream ssResponse;
  pt::write_json(ssResponse, responseSync);
  BOOST_LOG_TRIVIAL(info) << "RenameFileCommand: response " << ssResponse.str();

  if (visitAnswer(SerializerAnswer(responseSync).GetStatus())) {
    return;
  }

  callbackOk("Файл обновлен");
}

DeleteFileCommand::DeleteFileCommand(std::function<void(const std::string &)> callbackOk,
                                     std::function<void(const std::string &)> callbackError,
                                     std::shared_ptr<InternalDB> internalDB,
                                     fs::path path)
    : BaseCommand(std::move(callbackOk), std::move(callbackError), std::move(internalDB)),
      _filePath(std::move(path)) {
  BOOST_LOG_TRIVIAL(debug) << "DeleteFileCommand: create command";
}

void DeleteFileCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "RenameFileCommand: do";

  auto syncConfig = ClientConfig::getSyncConfig();
  auto network = ClientNetwork();

  Indexer indexer(_internalDB);
  auto fileMeta = indexer.DeleteFile(_filePath);
  auto fileInfo = indexer.GetDeleteFileInfo(fileMeta);

  auto syncRequest = SerializerFileInfo(0, fileInfo).GetJson();

  std::stringstream ssRequest;
  pt::write_json(ssRequest, syncRequest);
  BOOST_LOG_TRIVIAL(info) << "DeleteFileCommand: request " << ssRequest.str();

  pt::ptree responseSync;
  try {
    sendAndReceive(network, syncConfig, syncRequest, responseSync);
  } catch (NetworkException &er) {
    callbackError(er.what());
    return;
  }

  std::stringstream ssResponse;
  pt::write_json(ssResponse, responseSync);
  BOOST_LOG_TRIVIAL(info) << "DeleteFileCommand: response " << ssResponse.str();

  if (visitAnswer(SerializerAnswer(responseSync).GetStatus())) {
    return;
  }

  callbackOk("Файл обновлен");
}
