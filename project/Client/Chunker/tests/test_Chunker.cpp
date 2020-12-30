#include "test.h"
#include <thread>
#include <sstream>
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "SerializerAnswer.h"
#include "SerializerChunk.h"
#include "SerializerUserChunk.h"
#include "SerializerUserDate.h"
#include "SerializerFileInfo.h"
#include "SerializerExceptions.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

size_t calcDecodeLength(std::vector<char> &b64input) { //Calculates the length of a decoded string
  size_t len = b64input.size(),
      padding = 0;

  if (b64input[len - 1] == '=' && b64input[len - 2] == '=') //last two chars are =
    padding = 2;
  else if (b64input[len - 1] == '=') //last char is =
    padding = 1;

  return (len * 3) / 4 - padding;
}

int Base64Decode(std::vector<char> &b64message, char **buffer, size_t *length) { //Decodes a base64 encoded string
  BIO *bio, *b64;

  int decodeLen = calcDecodeLength(b64message);
  *buffer = new char[decodeLen];//(char*)malloc(decodeLen + 1);
//  (*buffer)[decodeLen] = '\0';

  bio = BIO_new_mem_buf(b64message.data(), -1);
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_push(b64, bio);

  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
  *length = BIO_read(bio, *buffer, b64message.size());
  //assert(*length == decodeLen); //length should equal decodeLen, else something went horribly wrong
  BIO_free_all(bio);

  return (0); //success
}

/*std::vector<char>*/ std::string Base64Encode(char *buffer, size_t length) { //Encodes a binary safe base 64 string
  BIO *bio, *b64;
  BUF_MEM *bufferPtr;

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new(BIO_s_mem());
  bio = BIO_push(b64, bio);

  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
  BIO_write(bio, buffer, length);
  BIO_flush(bio);
  BIO_get_mem_ptr(bio, &bufferPtr);
  BIO_set_close(bio, BIO_NOCLOSE);
  BIO_free_all(bio);
  std::string result(bufferPtr->data, bufferPtr->length);
  //std::vector<char> result(bufferPtr->data, bufferPtr->data + bufferPtr->length);
  //*b64text=(*bufferPtr).data;
  return result;
  //return (bufferPtr->length); //success
}

TEST(TestChunker, RunTest) {
  File file("/home/dantedoyl/watch/qwerty.jpg");
  Chunker test_chunk(file);

  std::vector<Chunk> a(test_chunk.ChunkFile());
  for (int i = 0; i < a.size(); i++) {
    //Base64Encode(a[i].data.data(), a[i].chunkSize, &base64EncodeOutput);
    /*std::vector<char>*/std::string vec = Base64Encode(a[i].data.data(), a[i].chunkSize);
    a[i].data = std::move(std::vector<char>(vec.begin(), vec.end())/*vec*/);
    std::cout << a[i].data.size() << std::endl;
  }
  File file1("/home/dantedoyl/watch/kiwi123.jpg");
  Chunker test_chunk1(file1);
  auto storageRequest = SerializerChunk(0, a).GetJson();
  std::stringstream ssRequestStorage;
  boost::property_tree::write_json(ssRequestStorage, storageRequest);
  boost::property_tree::ptree b;
  boost::property_tree::read_json(ssRequestStorage, b);
  auto test1 = SerializerChunk(b).GetChunk();
  std::cout << std::endl;
  for (int i = 0; i < test1.size(); i++) {
    std::cout << test1[i].data.size() << ' ';
    char *base64DecodeOutput;
    size_t test = 0;
    Base64Decode(test1[i].data, &base64DecodeOutput, &test);
    std::vector<char> vec(base64DecodeOutput, base64DecodeOutput + test);
    test1[i].data = std::move(vec);
    std::cout << test1[i].data.size() << std::endl;
  }
  test_chunk1.MergeFile(test1);
  for (int i = 0; i < a.size(); i++) {
    a[i].chunkId = i;
  }
  test_chunk1.UpdateChunkFile(a);
}