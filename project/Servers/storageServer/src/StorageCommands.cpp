#include "StorageCommands.h"
#include <boost/log/trivial.hpp>

UploadChunkCommand::UploadChunkCommand(const std::shared_ptr<pt::ptree> &request)
    : _request(request),
      _db(MongoDB::shared()),
      _chunk(*request) {
  BOOST_LOG_TRIVIAL(debug) << "UploadChunkCommand: create command";
}

std::shared_ptr<pt::ptree> UploadChunkCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "UploadChunkCommand: Do";

  try {
    _db.Connect();
    BOOST_LOG_TRIVIAL(info) << "UploadChunkCommand: connect to database";
  } catch (MongoExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "UploadChunkCommand: " << er.what();
    auto answer = SerializerAnswer(_chunk.GetRequestId(), "Database error");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  std::vector<Chunk> requestVector;
  try {
    requestVector = _chunk.GetChunk();
    BOOST_LOG_TRIVIAL(info) << "UploadChunkCommand: parse json";
  } catch (ParseException &er) {
    BOOST_LOG_TRIVIAL(error) << "UploadChunkCommand: " << er.what();
    auto answer = SerializerAnswer(_chunk.GetRequestId(), "Error in json");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  if (requestVector.empty()) {
    BOOST_LOG_TRIVIAL(error) << "UploadChunkCommand: empty chunk vector";
    auto answer = SerializerAnswer(_chunk.GetRequestId(), "Empty json");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  try {
    _db.InsertChunk(requestVector);
    BOOST_LOG_TRIVIAL(info) << "UploadChunkCommand: insert chunks to database";
  } catch (MongoExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "UploadChunkCommand: " << er.what();
    auto answer = SerializerAnswer(_chunk.GetRequestId(), "Fail to insert Chunk");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  BOOST_LOG_TRIVIAL(info) << "UploadChunkCommand: Status Ok";
  auto answer = SerializerAnswer(_chunk.GetRequestId());
  return std::make_shared<pt::ptree>(answer.GetJson());
}

DownloadChunkCommand::DownloadChunkCommand(const std::shared_ptr<pt::ptree> &request)
    : _request(request),
      _db(MongoDB::shared()),
      _userChunk(*request) {
  BOOST_LOG_TRIVIAL(debug) << "DownloadChunkCommand: create command";
}

std::shared_ptr<pt::ptree> DownloadChunkCommand::Do() {
  BOOST_LOG_TRIVIAL(debug) << "DownloadChunkCommand: Do";

  try {
    _db.Connect();
    BOOST_LOG_TRIVIAL(info) << "DownloadChunkCommand: connect to database";
  } catch (MongoExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "DownloadChunkCommand: " << er.what();
    auto answer = SerializerAnswer(_userChunk.GetRequestId(), "Database error");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  std::vector<UserChunk> requestVector;
  try {
    requestVector = _userChunk.GetChunk();
    BOOST_LOG_TRIVIAL(info) << "DownloadChunkCommand: parse json";
  } catch (ParseException &er) {
    BOOST_LOG_TRIVIAL(error) << "DownloadChunkCommand: " << er.what();
    auto answer = SerializerAnswer(_userChunk.GetRequestId(), "Error in json");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  if (requestVector.empty()) {
    BOOST_LOG_TRIVIAL(error) << "DownloadChunkCommand: empty user chunk vector";
    auto answer = SerializerAnswer(_userChunk.GetRequestId(), "Empty json");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  std::vector<Chunk> responseVector;
  try {
    responseVector = _db.GetChunk(requestVector);
    BOOST_LOG_TRIVIAL(info) << "DownloadChunkCommand: get chunks from database";
  } catch (MongoExceptions &er) {
    BOOST_LOG_TRIVIAL(error) << "DownloadChunkCommand: " << er.what();
    auto answer = SerializerAnswer(_userChunk.GetRequestId(), "Fail to get Chunk");
    return std::make_shared<pt::ptree>(answer.GetJson());
  }

  BOOST_LOG_TRIVIAL(info) << "DownloadChunkCommand: Status Ok";
  auto answer = SerializerChunk(_userChunk.GetRequestId(), responseVector);
  return std::make_shared<pt::ptree>(answer.GetJson());
}
