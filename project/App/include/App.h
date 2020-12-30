#pragma once

#include <openssl/md5.h>
#include <vector>
#include <queue>
#include <functional>
#include <string>
#include <memory>
#include <boost/filesystem.hpp>
#include "Commands.h"
#include "InternalDB.h"
#include "Worker.h"
#include "AppExceptions.h"
#include "InternalExceptions.h"
#include "structs/CloudEvents.h"
#include "Watcher.h"

namespace fs = boost::filesystem;

class App {
 public:
  App(std::function<void(const std::string &msg)> callbackOk,
      std::function<void(const std::string &msg)> callbackError,
      std::function<void(const std::string &msg)> callbackRefresh);
  ~App();

  bool IsLogin() const;
  std::string GetLogin() const;
  void LoginUser(std::string login, const std::string &password);
  void RegisterUser(std::string login, const std::string &password);
  void ChangePassword(const std::string &password);
  bool IsConfirmPassword(const std::string &password);
  void Logout();

  void Refresh();

  std::vector<FileMeta> GetFiles();
  void DownloadFile(int fileId) noexcept(false);

  void CreateFile(const fs::path &path) noexcept(false);
  void RenameFile(const fs::path &oldPath, const fs::path &newPath) noexcept(false);
  void DeleteFile(const fs::path &path);
  void ModifyFile(const fs::path &path) noexcept(false);

  void UpdateSyncFolder(const fs::path &path) noexcept(false);
  std::string GetSyncFolder();

 private:
  static std::string hash(const std::string &password);

  void createFile(const fs::path &path);
  void renameFile(const fs::path &oldPath, const fs::path &newPath);
  void deleteFile(const fs::path &path);
  void modifyFile(const fs::path &path);

  void runWorker();

  void runWatcher();
  void stopWatcher();
  void execEvent();
  void watcherCallback(const CloudNotification &event);

 private:
  std::function<void(const std::string &msg)> appCallbackOk;
  std::function<void(const std::string &msg)> appCallbackError;
  std::function<void(const std::string &msg)> appCallbackRefresh;

 private:
  bool _isWorkingWorker;

  std::queue<std::shared_ptr<BaseCommand>> _commands;
  std::shared_ptr<InternalDB> _internalDB;

  std::queue<CloudNotification> _events;
  std::thread _watcherThread;
  Watcher _watcher;
};
