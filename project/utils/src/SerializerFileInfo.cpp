#include "SerializerFileInfo.h"
#include <boost/log/trivial.hpp>

SerializerFileInfo::SerializerFileInfo(int requestId, std::vector<FileInfo> fileInfoVector)
    : _requestId(requestId),
      _fileInfoVector(std::move(fileInfoVector)) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerFileInfo: create serializer chunk from file vector";
}

SerializerFileInfo::SerializerFileInfo(int requestId, const FileInfo &fileInfo)
    : _requestId(requestId) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerFileInfo: create serializer chunk from file";
  _fileInfoVector.push_back(fileInfo);
}

SerializerFileInfo::SerializerFileInfo(
    const pt::ptree &val)
    : _json(val) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerFileInfo: create serializer chunk from json";
}

int SerializerFileInfo::GetRequestId() const {
  BOOST_LOG_TRIVIAL(debug) << "SerializerFileInfo: GetRequestId";
  return _requestId;
}

std::vector<FileInfo> SerializerFileInfo::GetFileInfo() noexcept(false) {
  BOOST_LOG_TRIVIAL(debug) << "SerializerFileInfo: GetChunk";
  if (_fileInfoVector.empty()) {
    deserialize();
  }

  return _fileInfoVector;
}

pt::ptree SerializerFileInfo::GetJson() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerFileInfo: GetJson";
  if (_json.empty()) {
    serialize();
  }

  return _json;
}

void SerializerFileInfo::serialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerFileInfo: serialize";

  _json.put("command", "UploadFile");
  _json.put("requestId", _requestId);

  pt::ptree data;
  for (auto &fileMeta : _fileInfoVector) {
    pt::ptree fileChild;
    fileChild.put("userId", fileMeta.userId);

    pt::ptree file;
    file.put("fileId", fileMeta.file.fileId);
    file.put("version", fileMeta.file.version);
    file.put("fileName", fileMeta.file.fileName);
    file.put("fileExtension", fileMeta.file.fileExtension);
    file.put("filePath", fileMeta.file.filePath);
    file.put("fileSize", fileMeta.file.fileSize);
    file.put("chunksCount", fileMeta.file.chunksCount);
    file.put("isCurrent", fileMeta.file.isCurrent);
    file.put("isDeleted", fileMeta.file.isDeleted);
    file.put("updateDate", fileMeta.file.updateDate);
    file.put("createDate", fileMeta.file.createDate);
    fileChild.add_child("file", file);

    pt::ptree chunks;
    for (auto &el : fileMeta.chunkMeta) {
      pt::ptree child;
      child.put("chunkId", el.chunkId);

      chunks.push_back(std::make_pair("", child));
    }
    fileChild.add_child("chunks", chunks);

    pt::ptree fileChunks;
    for (auto &el : fileMeta.fileChunksMeta) {
      pt::ptree child;
      child.put("chunkId", el.chunkId);
      child.put("chunkOrder", el.chunkOrder);

      fileChunks.push_back(std::make_pair("", child));
    }
    fileChild.add_child("fileChunks", fileChunks);

    data.push_back(std::make_pair("", fileChild));
  }

  _json.add_child("files", data);
}

void SerializerFileInfo::deserialize() {
  BOOST_LOG_TRIVIAL(debug) << "SerializerFileInfo: deserialize";

  try {
    _requestId = _json.get<int>("requestId");

    for (auto &fileMeta : _json.get_child("files")) {
      auto oneFileInfo = FileInfo{.userId = fileMeta.second.get<int>("userId")};

      pt::ptree file = fileMeta.second.get_child("file");
      oneFileInfo.file = FileMeta{.fileId = file.get<int>("fileId"),
          .version = file.get<int>("version"),
          .fileName = file.get<std::string>("fileName"),
          .fileExtension = file.get<std::string>("fileExtension"),
          .filePath = file.get<std::string>("filePath"),
          .fileSize = file.get<int>("fileSize"),
          .chunksCount = file.get<int>("chunksCount"),
          .isDeleted = file.get<bool>("isDeleted"),
          .isCurrent = file.get<bool>("isCurrent"),
          .updateDate = file.get<std::string>("updateDate"),
          .createDate = file.get<std::string>("createDate")};

      for (auto &val : fileMeta.second.get_child("chunks")) {
        auto chunkMeta = ChunkMeta{.chunkId = val.second.get<int>("chunkId")};

        oneFileInfo.chunkMeta.push_back(chunkMeta);
      }

      for (auto &val : fileMeta.second.get_child("fileChunks")) {
        auto fileChunkMeta = FileChunksMeta{.chunkId = val.second.get<int>("chunkId"),
            .chunkOrder = val.second.get<int>("chunkOrder")};

        oneFileInfo.fileChunksMeta.push_back(fileChunkMeta);
      }

      _fileInfoVector.push_back(oneFileInfo);
    }
  } catch (pt::ptree_error &er) {
    throw ParseException(er.what());
  }
}
