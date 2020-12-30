#include "MongoDB.h"
#include <boost/log/trivial.hpp>
#include <mongocxx/exception/exception.hpp>

MongoDB::MongoDB() {
  mongocxx::instance _instance{};
}

MongoDB &MongoDB::shared() {
  static MongoDB shared;
  return shared;
}

void MongoDB::InsertChunk(const std::vector<Chunk> &chunks) const {
  BOOST_LOG_TRIVIAL(debug) << "MongoDB: Insert Chunks";
  std::vector<bsoncxx::document::value> inChunks;
  bsoncxx::stdx::optional<bsoncxx::document::value> cursor;
  for (const auto &chunk : chunks) {
    try {
      cursor =
          _database["chunks"].find_one(
              document{} << "id_user" << chunk.userId
                         << "id_chunk" << chunk.chunkId << finalize);
    } catch (mongocxx::exception &exception) {
      BOOST_LOG_TRIVIAL(error) << "MongoDB: " + std::string(exception.what());
      throw MongoExceptions("MongoDB: " + std::string(exception.what()));
    }

    if (cursor) {
      throw MongoExceptions(
          "This user = " + std::to_string(chunk.userId) + " and this chunk id = " + std::to_string(chunk.chunkId)
              + " is already exist");
    }

    bsoncxx::types::b_binary b_binary{bsoncxx::binary_sub_type::k_binary, uint32_t(chunk.data.size()),
                                      (uint8_t *) chunk.data.data()};
    inChunks.push_back(
        document{} << "id_user" << chunk.userId
                   << "id_chunk" << chunk.chunkId
                   << "size_chunk" << chunk.chunkSize
                   << "rapid_hash" << chunk.rHash
                   << "static_hash" << chunk.sHash
                   << "data" << b_binary << finalize);
  }
  try {
    _database["chunks"].insert_many(inChunks);
  } catch (mongocxx::exception &exception) {
    BOOST_LOG_TRIVIAL(error) << "MongoDB: " + std::string(exception.what());
    throw MongoExceptions("MongoDB: " + std::string(exception.what()));
  }
}

std::vector<Chunk> MongoDB::GetChunk(const std::vector<UserChunk> &userChunks) const {
  BOOST_LOG_TRIVIAL(debug) << "MongoDB: Get Chunks";
  std::vector<Chunk> users;
  for (const auto &userChunk : userChunks) {
    Chunk chunk;
    bsoncxx::stdx::optional<bsoncxx::document::value> cursor;
    try {
      cursor =
          _database["chunks"].find_one(
              document{} << "id_user" << userChunk.userId
                         << "id_chunk" << userChunk.chunkId << finalize);
    } catch (mongocxx::exception &exception) {
      BOOST_LOG_TRIVIAL(error) << "MongoDB: " + std::string(exception.what());
      throw MongoExceptions("MongoDB: " + std::string(exception.what()));
    }
    if (cursor) {
      bsoncxx::document::value value = (*cursor);
      bsoncxx::document::view view = value.view();
      chunk.userId = view["id_user"].get_int32();
      chunk.chunkId = view["id_chunk"].get_int32();
      chunk.chunkSize = view["size_chunk"].get_int32();
      chunk.sHash = view["static_hash"].get_utf8().value.to_string();
      chunk.rHash = view["rapid_hash"].get_utf8().value.to_string();
      const unsigned char *data = view["data"].get_binary().bytes;
      std::vector<uint8_t> dataVector;
      dataVector.reserve(view["data"].get_binary().size);
      std::copy(data, data + view["data"].get_binary().size, std::back_inserter(dataVector));
      std::vector<char> dataVec(dataVector.begin(), dataVector.end());
      chunk.data = dataVec;
      users.push_back(chunk);
    } else {
      throw MongoExceptions("This user = " + std::to_string(userChunk.userId) + " and this chunk id = "
                                + std::to_string(userChunk.chunkId) + " is not exist");
    }
  }
  return users;
}
void MongoDB::Connect() {
  BOOST_LOG_TRIVIAL(debug) << "MongoDB: Connect";

  try {
    mongocxx::uri _uri{};
    _client = mongocxx::client(_uri);
    _database = _client["cloud"];
    BOOST_LOG_TRIVIAL(debug) << "MongoDB: Init DB";
  } catch (std::exception &exceptions) {
    BOOST_LOG_TRIVIAL(error) << "MongoDB: " + std::string(exceptions.what());
    throw MongoExceptions("MongoDB: Faild to connect");
  }
}
