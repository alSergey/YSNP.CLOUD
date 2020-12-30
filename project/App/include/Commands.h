#pragma once

#include <string>
#include <utility>
#include <variant>
#include <memory>
#include <boost/filesystem.hpp>
#include "InternalDB.h"
#include "ClientNetwork.h"
#include "ClientNetworkExceptions.h"
#include "CommandsExceptions.h"
#include "ClientConfig.h"
#include "SerializerAnswer.h"
#include "SerializerChunk.h"
#include "SerializerUserChunk.h"
#include "SerializerUserDate.h"
#include "SerializerFileInfo.h"
#include "SerializerUserInfo.h"
#include "SerializerUserIds.h"
#include "SerializerExceptions.h"
#include "User.h"
#include "Indexer.h"
#include "File.h"
#include "Chunker.h"

namespace fs = boost::filesystem;

class BaseCommand {
 public:
  explicit BaseCommand(std::function<void(const std::string &msg)> callbackOk,
                       std::function<void(const std::string &msg)> callbackError,
                       std::shared_ptr<InternalDB> internalDB);
  virtual ~BaseCommand() = default;
  virtual void Do() = 0;

 protected:
  static void sendAndReceive(ClientNetwork &network,
                             const NetworkConfig &config,
                             const pt::ptree &request,
                             pt::ptree &response) noexcept(false);

  bool visitAnswer(const std::variant<StatusOk, StatusError> &response);

 protected:
  std::function<void(const std::string &msg)> callbackOk;
  std::function<void(const std::string &msg)> callbackError;

 protected:
  std::shared_ptr<InternalDB> _internalDB;
};

class LoginUserCommand : public BaseCommand {
 public:
  explicit LoginUserCommand(std::function<void(const std::string &msg)> callbackOk,
                            std::function<void(const std::string &msg)> callbackError,
                            std::shared_ptr<InternalDB> internalDB,
                            std::string login,
                            std::string password);
  void Do() override;

 private:
  std::string _login;
  std::string _password;
};

class RegisterUserCommand : public BaseCommand {
 public:
  explicit RegisterUserCommand(std::function<void(const std::string &msg)> callbackOk,
                               std::function<void(const std::string &msg)> callbackError,
                               std::shared_ptr<InternalDB> internalDB,
                               std::string login,
                               std::string password);
  void Do() override;

 private:
  std::string _login;
  std::string _password;
};

class RefreshCommand : public BaseCommand {
 public:
  explicit RefreshCommand(std::function<void(const std::string &msg)> callbackOk,
                          std::function<void(const std::string &msg)> callbackError,
                          std::shared_ptr<InternalDB> internalDB,
                          std::function<void()> stopWatcher,
                          std::function<void()> runWatcher);
  void Do() override;

 private:
  std::function<void()> stopWatcher;
  std::function<void()> runWatcher;
};

class DownloadFileCommand : public BaseCommand {
 public:
  explicit DownloadFileCommand(std::function<void(const std::string &msg)> callbackOk,
                               std::function<void(const std::string &msg)> callbackError,
                               std::shared_ptr<InternalDB> internalDB,
                               FileMeta file,
                               std::function<void()> stopWatcher,
                               std::function<void()> runWatcher);
  void Do() override;

 private:
  std::function<void()> stopWatcher;
  std::function<void()> runWatcher;

 private:
  FileMeta _file;
};

class CreateFileCommand : public BaseCommand {
 public:
  explicit CreateFileCommand(std::function<void(const std::string &msg)> callbackOk,
                             std::function<void(const std::string &msg)> callbackError,
                             std::shared_ptr<InternalDB> internalDB,
                             fs::path path);

  void Do() override;

 private:
  fs::path _filePath;
};

class ModifyFileCommand : public BaseCommand {
 public:
  explicit ModifyFileCommand(std::function<void(const std::string &msg)> callbackOk,
                             std::function<void(const std::string &msg)> callbackError,
                             std::shared_ptr<InternalDB> internalDB,
                             fs::path path);

  void Do() override;

 private:
  fs::path _filePath;
};

class RenameFileCommand : public BaseCommand {
 public:
  explicit RenameFileCommand(std::function<void(const std::string &msg)> callbackOk,
                             std::function<void(const std::string &msg)> callbackError,
                             std::shared_ptr<InternalDB> internalDB,
                             fs::path oldPath,
                             fs::path newPath);

  void Do() override;

 private:
  fs::path _fileOldPath;
  fs::path _fileNewPath;
};

class DeleteFileCommand : public BaseCommand {
 public:
  explicit DeleteFileCommand(std::function<void(const std::string &msg)> callbackOk,
                             std::function<void(const std::string &msg)> callbackError,
                             std::shared_ptr<InternalDB> internalDB,
                             fs::path path);

  void Do() override;

 private:
  fs::path _filePath;
};
