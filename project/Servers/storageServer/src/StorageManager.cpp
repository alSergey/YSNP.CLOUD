#include "StorageManager.h"
#include <boost/log/trivial.hpp>

std::shared_ptr<pt::ptree> StorageManager::GetRequest(const pt::ptree &val) {
  std::unique_ptr<Command> command;
  int userId = 0;

  try {
    userId = val.get<int>("requestId");
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(warning) << "StorageManager: " << er.what();
  }

  try {
    command = getCommand(val);
  } catch (NoCommand &er) {
    BOOST_LOG_TRIVIAL(error) << "StorageManager: " << er.what();
    auto answer = SerializerAnswer(userId, er.what());
    return std::make_shared<pt::ptree>(answer.GetJson());
  } catch (WrongCommand &er) {
    BOOST_LOG_TRIVIAL(error) << "StorageManager: " << er.what();
    auto answer = SerializerAnswer(userId, er.what());
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  return command->Do();
}

std::unique_ptr<Command> StorageManager::getCommand(const pt::ptree &val) {
  BOOST_LOG_TRIVIAL(debug) << "StorageManager: getCommand";
  std::string command;

  try {
    command = val.get<std::string>("command");
    BOOST_LOG_TRIVIAL(info) << "StorageManager: command = " << command;
  } catch (pt::ptree_error &er) {
    throw NoCommand(er.what());
  }

  auto sh = std::make_shared<pt::ptree>(val);
  if (command == "DownloadChunk") {
    return std::make_unique<DownloadChunkCommand>(sh);
  }

  if (command == "UploadChunk") {
    return std::make_unique<UploadChunkCommand>(sh);
  }

  throw WrongCommand("Wrong command");
}
