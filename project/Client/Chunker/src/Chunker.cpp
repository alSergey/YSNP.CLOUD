#include "Chunker.h"

Chunker::Chunker(const File &file)
    : _file(file) {
}

std::vector<Chunk> Chunker::UpdateChunkFile(const std::vector<Chunk> &old_chunks) {
  std::ifstream fileStream(std::move(_file.Read()));
  int size = fileStream.seekg(0, std::ios::end).tellg();
  fileStream.seekg(0, std::ios::beg);
  std::vector<Chunk> chunks{};
  Chunk new_chunk{};
  std::vector<char> data;
  std::istreambuf_iterator<char> fileIterator(fileStream);
  if (fileStream.is_open()) {
    std::copy_n(fileIterator, size < CHUNK_SIZE ? size : CHUNK_SIZE, std::inserter(data, data.begin()));
    size -= CHUNK_SIZE;
    while (!fileStream.eof()) {
      std::string chunksRHash = getRHash(data.data(), data.size());
      auto match = std::find_if(old_chunks.begin(), old_chunks.end(), [&chunksRHash](const Chunk &old_chunk) {
        return old_chunk.rHash == chunksRHash;
      });
      if (match != old_chunks.end()) {
        std::string chunksSHash = getSHash(data.data(), data.size());
        if (chunksSHash == match->sHash) {
          if (new_chunk.chunkSize != 0) {
            new_chunk.rHash = getRHash(new_chunk.data.data(), new_chunk.chunkSize);
            new_chunk.sHash = getSHash(new_chunk.data.data(), new_chunk.chunkSize);
            chunks.push_back(std::move(new_chunk));
            new_chunk.chunkSize = 0;
          }
          chunks.push_back(*match);
          if (size == -1)
            break;
          data.clear();
          if (size > CHUNK_SIZE) {
            std::copy_n(std::next(fileIterator),
                        CHUNK_SIZE,
                        std::back_inserter(data));
            size -= CHUNK_SIZE;
          } else {
            std::copy_n(std::next(fileIterator),
                        size,
                        std::back_inserter(data));
            size = -1;
          }
        }
      } else {
        if (size == -1)
          break;
        new_chunk.chunkSize +=
            data.size() < CHUNK_MOVE_SIZE ? static_cast<int>(fileStream.gcount()) : CHUNK_MOVE_SIZE;
        std::copy(data.begin(), data.begin() + CHUNK_MOVE_SIZE, std::back_inserter(new_chunk.data));
        if (new_chunk.chunkSize == CHUNK_SIZE) {
          new_chunk.rHash = getRHash(new_chunk.data.data(), new_chunk.chunkSize);
          new_chunk.sHash = getSHash(new_chunk.data.data(), new_chunk.chunkSize);
          chunks.push_back(std::move(new_chunk));
          new_chunk.chunkSize = 0;
        }
        data.erase(data.begin(), data.begin() + CHUNK_MOVE_SIZE);
        if (size > CHUNK_MOVE_SIZE) {
          std::copy_n(std::next(fileIterator),
                      CHUNK_MOVE_SIZE,
                      std::back_inserter(data));
          size -= CHUNK_MOVE_SIZE;
        } else {
          std::copy_n(std::next(fileIterator),
                      size,
                      std::back_inserter(data));
          size = -1;
        }
      }
    }
  }
  fileStream.close();
  std::copy(new_chunk.data.begin(), new_chunk.data.end(), std::inserter(data, data.begin()));
  while (!data.empty()) {
    if (data.size() > CHUNK_SIZE) {
      chunks.push_back(std::move(createChunk(std::vector<char>(data.begin(), data.begin() + CHUNK_SIZE), CHUNK_SIZE)));
      data.erase(data.begin(), data.begin() + CHUNK_SIZE);
    } else {
      chunks.push_back(std::move(createChunk(data, data.size())));
      break;
    }
  }
  return chunks;
}

std::string Chunker::getSHash(char *data, int chunkSize) {
  MD5_CTX ctx;
  MD5_Init(&ctx);

  MD5_Update(&ctx, data, chunkSize);

  unsigned char digest[MD5_DIGEST_LENGTH] = {};
  MD5_Final(digest, &ctx);

  std::stringstream result;
  for (unsigned char i : digest)
    result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);

  return result.str();
}

std::string Chunker::getRHash(char *data, int chunkSize) {
  uint32_t digest = adler32(0, reinterpret_cast<const Bytef *>(data), chunkSize);

  std::stringstream result;
  result << std::hex << static_cast<int>(digest);
  return result.str();
}

std::vector<Chunk> Chunker::ChunkFile() {
  std::ifstream fileStream(std::move(_file.Read()));
  fileStream.seekg(0, std::ios::beg);
  std::vector<Chunk> chunks;
  if (fileStream.is_open()) {
    while (!fileStream.eof()) {
      std::vector<char> data(CHUNK_SIZE);
      fileStream.read(data.data(), CHUNK_SIZE);
      chunks.push_back(std::move(createChunk(data, static_cast<int>(fileStream.gcount()))));
    }
  }
  fileStream.close();
  return chunks;
}

Chunk Chunker::createChunk(std::vector<char> data, int chunkSize) {
  Chunk chunk{
      .chunkSize = chunkSize,
  };
  std::copy(data.begin(), data.begin() + chunkSize, std::back_inserter(chunk.data));
  chunk.rHash = getRHash(chunk.data.data(), chunk.chunkSize);
  chunk.sHash = getSHash(chunk.data.data(), chunk.chunkSize);

  return chunk;
}

void Chunker::MergeFile(const std::vector<Chunk> &chunks) {
  // TODO(Maxim): для беспорядка принимать мапу, где первое поле order
  std::ofstream outputFile(std::move(_file.Write()));

  if (outputFile.is_open()) {
    for (auto chunk : chunks) {
      outputFile.write(chunk.data.data(), chunk.chunkSize);
    }
  }
  outputFile.close();
}
