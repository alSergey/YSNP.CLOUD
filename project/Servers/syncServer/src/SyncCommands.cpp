#include "SyncCommands.h"
#include <boost/log/trivial.hpp>

DataUpdateCommand::DataUpdateCommand(const std::shared_ptr<pt::ptree> &request)
    : _request(request),
      _db(MetaDataDB::shared("user=ysnp dbname=ysnpcloud")),
      _userDate(*request) {
  BOOST_LOG_TRIVIAL(debug) << "DataUpdateCommand: create command";
}

std::shared_ptr<pt::ptree> DataUpdateCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "DataUpdateCommand: Do";

  try {
    _db.Connect();
    BOOST_LOG_TRIVIAL(info) << "DataUpdateCommand: connect to database";
  } catch (PostgresExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "DataUpdateCommand: " << er.what();
    auto answer = SerializerAnswer(_userDate.GetRequestId(), "Database error");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  UserDate requestUserDate;
  try {
    requestUserDate = _userDate.GetUserDate();
    BOOST_LOG_TRIVIAL(info) << "DataUpdateCommand: parse json, user = " << requestUserDate.userId;
  } catch (ParseException &er) {
    BOOST_LOG_TRIVIAL(error) << "DataUpdateCommand: " << er.what();
    auto answer = SerializerAnswer(_userDate.GetRequestId(), "Error in json");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  std::vector<FileInfo> responseFileInfo;
  try {
    responseFileInfo = _db.GetUserFilesByTime(requestUserDate);
    BOOST_LOG_TRIVIAL(info) << "DataUpdateCommand: get files from database";
  } catch (PostgresExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "DataUpdateCommand: " << er.what();
    auto answer = SerializerAnswer(_userDate.GetRequestId(), "Fail to get file");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  BOOST_LOG_TRIVIAL(info) << "DataUpdateCommand: Status Ok";
  auto answer = SerializerFileInfo(_userDate.GetRequestId(), responseFileInfo);
  return std::make_shared<pt::ptree>(answer.GetJson());
}

UploadFileCommand::UploadFileCommand(const std::shared_ptr<pt::ptree> &request)
    : _request(request),
      _db(MetaDataDB::shared("user=ysnp dbname=ysnpcloud")),
      _fileInfo(*request) {
  BOOST_LOG_TRIVIAL(debug) << "UploadFileCommand: create command";
}

std::shared_ptr<pt::ptree> UploadFileCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "UploadFileCommand: Do";

  try {
    _db.Connect();
    BOOST_LOG_TRIVIAL(info) << "UploadFileCommand: connect to database";
  } catch (PostgresExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "UploadFileCommand: " << er.what();
    auto answer = SerializerAnswer(_fileInfo.GetRequestId(), "Database error");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  std::vector<FileInfo> requestFileInfo;
  try {
    requestFileInfo = _fileInfo.GetFileInfo();
    BOOST_LOG_TRIVIAL(info) << "UploadFileCommand: parse json, user = " << requestFileInfo[0].userId;
  } catch (ParseException &er) {
    BOOST_LOG_TRIVIAL(error) << "UploadFileCommand: " << er.what();
    auto answer = SerializerAnswer(_fileInfo.GetRequestId(), "Error in json");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  if (requestFileInfo.empty()) {
    BOOST_LOG_TRIVIAL(error) << "UploadFileCommand: empty chunk vector";
    auto answer = SerializerAnswer(_fileInfo.GetRequestId(), "Empty json");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  try {
    _db.InsertFile(requestFileInfo[0]);
    BOOST_LOG_TRIVIAL(info) << "UploadFileCommand: insert file to database";
  } catch (PostgresExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "UploadFileCommand: " << er.what();
    auto answer = SerializerAnswer(_fileInfo.GetRequestId(), "Fail to insert file");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  BOOST_LOG_TRIVIAL(info) << "UploadFileCommand: Status Ok";
  auto answer = SerializerAnswer(_fileInfo.GetRequestId());
  return std::make_shared<pt::ptree>(answer.GetJson());
}

LoginUser::LoginUser(const std::shared_ptr<pt::ptree> &request)
    : _request(request),
      _db(UsersDB::shared("user=ysnp dbname=ysnpcloud")),
      _userInfo(*request) {
  BOOST_LOG_TRIVIAL(debug) << "LoginUser: create command";
}

std::shared_ptr<pt::ptree> LoginUser::Do() {
  BOOST_LOG_TRIVIAL(debug) << "LoginUser: Do";

  try {
    _db.Connect();
    BOOST_LOG_TRIVIAL(info) << "LoginUser: connect to database";
  } catch (PostgresExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "LoginUser: " << er.what();
    auto answer = SerializerAnswer(_userInfo.GetRequestId(), "Database error");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  UserInfo requestUserInfo;
  try {
    requestUserInfo = _userInfo.GetUserInfo();
    BOOST_LOG_TRIVIAL(info) << "LoginUser: parse json, user = " << requestUserInfo.login;
  } catch (ParseException &er) {
    BOOST_LOG_TRIVIAL(error) << "LoginUser: " << er.what();
    auto answer = SerializerAnswer(_userInfo.GetRequestId(), "Error in json");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  UserIds responseUserIds{};
  try {
    responseUserIds = _db.Login(requestUserInfo);
    BOOST_LOG_TRIVIAL(info) << "LoginUser: insert file to database";
  } catch (PostgresExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "LoginUser: " << er.what();
    auto answer = SerializerAnswer(_userInfo.GetRequestId(), "Invalid login or password");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  BOOST_LOG_TRIVIAL(info) << "LoginUser: Status Ok";
  auto answer = SerializerUserIds(_userInfo.GetRequestId(), responseUserIds, "");
  return std::make_shared<pt::ptree>(answer.GetJson());
}

RegisterUser::RegisterUser(const std::shared_ptr<pt::ptree> &request)
    : _request(request),
      _db(UsersDB::shared("user=ysnp dbname=ysnpcloud")),
      _userInfo(*request) {
  BOOST_LOG_TRIVIAL(debug) << "RegisterUser: create command";
}

std::shared_ptr<pt::ptree> RegisterUser::Do() {
  BOOST_LOG_TRIVIAL(debug) << "RegisterUser: Do";

  try {
    _db.Connect();
    BOOST_LOG_TRIVIAL(info) << "RegisterUser: connect to database";
  } catch (PostgresExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "RegisterUser: " << er.what();
    auto answer = SerializerAnswer(_userInfo.GetRequestId(), "Database error");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  UserInfo requestUserInfo;
  try {
    requestUserInfo = _userInfo.GetUserInfo();
    BOOST_LOG_TRIVIAL(info) << "RegisterUser: parse json, user = " << requestUserInfo.login;
  } catch (ParseException &er) {
    BOOST_LOG_TRIVIAL(error) << "RegisterUser: " << er.what();
    auto answer = SerializerAnswer(_userInfo.GetRequestId(), "Error in json");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  UserIds responseUserIds{};
  try {
    responseUserIds = _db.Registration(requestUserInfo);
    BOOST_LOG_TRIVIAL(info) << "RegisterUser: insert file to database";
  } catch (PostgresExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "RegisterUser: " << er.what();
    auto answer = SerializerAnswer(_userInfo.GetRequestId(), "Login is already exist");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  BOOST_LOG_TRIVIAL(info) << "RegisterUser: Status Ok";
  auto answer = SerializerUserIds(_userInfo.GetRequestId(), responseUserIds, "");
  return std::make_shared<pt::ptree>(answer.GetJson());
}
