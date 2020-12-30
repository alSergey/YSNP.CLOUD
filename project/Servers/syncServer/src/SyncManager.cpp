#include "SyncManager.h"
#include <boost/log/trivial.hpp>

std::shared_ptr<pt::ptree> SyncManager::GetRequest(const pt::ptree &val) {
  std::unique_ptr<Command> command;
  int requestId = 0;

  try {
    requestId = val.get<int>("requestId");
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(warning) << "SyncManager: " << er.what();
  }

  try {
    command = getCommand(val);
  } catch (NoCommand &er) {
    BOOST_LOG_TRIVIAL(error) << "SyncManager: " << er.what();
    auto answer = SerializerAnswer(requestId, er.what());
    return std::make_shared<pt::ptree>(answer.GetJson());
  } catch (WrongCommand &er) {
    BOOST_LOG_TRIVIAL(error) << "SyncManager: " << er.what();
    auto answer = SerializerAnswer(requestId, er.what());
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  return command->Do();;
}

std::unique_ptr<Command> SyncManager::getCommand(const pt::ptree &val) {
  BOOST_LOG_TRIVIAL(debug) << "SyncManager: getCommand";
  std::string command;

  try {
    command = val.get<std::string>("command");
    BOOST_LOG_TRIVIAL(info) << "SyncManager: command = " << command;
  } catch (pt::ptree_error &er) {
    throw NoCommand(er.what());
  }

  auto sh = std::make_shared<pt::ptree>(val);
  if (command == "DateUpdate") {
    return std::make_unique<DataUpdateCommand>(sh);
  }

  if (command == "UploadFile") {
    return std::make_unique<UploadFileCommand>(sh);
  }

  if (command == "LoginUser") {
    return std::make_unique<LoginUser>(sh);
  }

  if (command == "RegisterUser") {
    return std::make_unique<RegisterUser>(sh);
  }

  throw WrongCommand("Wrong command");
}
