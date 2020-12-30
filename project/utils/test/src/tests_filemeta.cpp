#include "gtest_utils.h"

TEST(FileMeta, Struct) {
  auto file = FileMeta{.fileId = 1,
      .version = 1,
      .fileName = "test",
      .fileExtension = "txt",
      .filePath = "static/",
      .fileSize = 1,
      .chunksCount = 1,
      .isCurrent = true,
      .updateDate = "31.12.1970",
      .createDate = "31.12.1970"};

  std::vector<ChunkMeta> chunksMetaVector;
  for (int i = 0; i < 2; ++i) {
    auto chunkMeta = ChunkMeta{.chunkId = i};
    chunksMetaVector.push_back(chunkMeta);
  }

  std::vector<FileChunksMeta> fileChunksMetaVector;
  for (int i = 0; i < 2; ++i) {
    auto fileChunkMeta = FileChunksMeta{.chunkId = i, .chunkOrder = i};
    fileChunksMetaVector.push_back(fileChunkMeta);
  }

  auto fileInfo = FileInfo{.file = file, .chunkMeta = chunksMetaVector, .fileChunksMeta = fileChunksMetaVector};

  std::vector<FileInfo> vec;
  vec.push_back(fileInfo);

  auto serializer = SerializerFileInfo(1, vec);

  std::stringstream ss;
  pt::write_json(ss, serializer.GetJson());
  std::cout << ss.str() << std::endl;
}

TEST(FileInfo, FileMeta) {
  std::vector<FileMeta> vec;
  auto file1 = FileMeta{
      .fileId = 1,
      .version = 1,
      .fileName = "file1",
      .fileExtension = "txt",
      .filePath = "static/",
      .fileSize = 150,
      .chunksCount = 12,
      .isDownload = false,
      .isDeleted = false,
      .isCurrent = true,
      .updateDate = "31.12.1970",
      .createDate = "31.11.1970"};
  vec.push_back(file1);

  auto file2 = FileMeta{
      .fileId = 2,
      .version = 10,
      .fileName = "file2",
      .fileExtension = "txt",
      .filePath = "static/",
      .fileSize = 76,
      .chunksCount = 25,
      .isDownload = true,
      .isDeleted = false,
      .isCurrent = true,
      .updateDate = "31.10.1970",
      .createDate = "31.09.1970"};
  vec.push_back(file2);

  auto file3 = FileMeta{
      .fileId = 3,
      .version = 6,
      .fileName = "file3",
      .fileExtension = "docx",
      .filePath = "static/uploads",
      .fileSize = 89,
      .chunksCount = 17,
      .isDownload = true,
      .isDeleted = false,
      .isCurrent = true,
      .updateDate = "31.08.1970",
      .createDate = "31.07.1970"};
  vec.push_back(file3);

  auto file4 = FileMeta{
      .fileId = 4,
      .version = 15,
      .fileName = "file4",
      .fileExtension = "jpeg",
      .filePath = "static/uploads2",
      .fileSize = 123,
      .chunksCount = 10,
      .isDownload = false,
      .isDeleted = false,
      .isCurrent = true,
      .updateDate = "31.06.1970",
      .createDate = "31.05.1970"};
  vec.push_back(file4);

  auto file5 = FileMeta{
      .fileId = 5,
      .version = 4,
      .fileName = "file5",
      .fileExtension = "png",
      .filePath = "papka/",
      .fileSize = 50,
      .chunksCount = 15,
      .isDownload = true,
      .isDeleted = false,
      .isCurrent = true,
      .updateDate = "31.04.1970",
      .createDate = "31.03.1970"};
  vec.push_back(file5);

  auto file6 = FileMeta{
      .fileId = 6,
      .version = 2,
      .fileName = "file6",
      .fileExtension = "html",
      .filePath = "static/uploads",
      .fileSize = 124,
      .chunksCount = 1,
      .isDownload = true,
      .isDeleted = false,
      .isCurrent = true,
      .updateDate = "31.02.1970",
      .createDate = "31.01.1970"};
  vec.push_back(file6);

  auto file7 = FileMeta{
      .fileId = 7,
      .version = 51,
      .fileName = "file7",
      .fileExtension = "cpp",
      .filePath = "papka3/",
      .fileSize = 900,
      .chunksCount = 3,
      .isDownload = false,
      .isDeleted = false,
      .isCurrent = true,
      .updateDate = "31.12.1976",
      .createDate = "31.12.1974"};
  vec.push_back(file7);
}