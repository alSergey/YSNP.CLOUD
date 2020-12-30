#include "gtest_utils.h"

struct visitor {
  void operator()(const StatusOk &val) const {
    std::cout << "StatusOk: " << val.requestId << std::endl;
  }

  void operator()(const StatusError &val) const {
    std::cout << "StatusError: " << val.requestId << std::endl;
  }
};

TEST(Serializer, AnswerOk) {
  std::stringstream ss;

  auto sendOK = SerializerAnswer(std::rand() % 100);
  auto sendJsonOK = sendOK.GetJson();
  pt::write_json(ss, sendJsonOK);

  pt::ptree recvJsonOK;
  pt::read_json(ss, recvJsonOK);
  EXPECT_EQ(sendJsonOK, recvJsonOK);

  auto recvOK = SerializerAnswer(recvJsonOK);
  auto statusOK = recvOK.GetStatus();
  std::visit(visitor{}, statusOK);
}

TEST(Serializer, AnswerError) {
  std::stringstream ss;

  auto sendERROR = SerializerAnswer(std::rand() % 100, "Error");
  auto sendJsonERROR = sendERROR.GetJson();
  pt::write_json(ss, sendJsonERROR);

  pt::ptree recvJsonERROR;
  pt::read_json(ss, recvJsonERROR);
  EXPECT_EQ(sendJsonERROR, recvJsonERROR);

  auto recvERROR = SerializerAnswer(recvJsonERROR);
  auto statusERROR = recvERROR.GetStatus();
  std::visit(visitor{}, statusERROR);
}

TEST(Serializer, AnswerErrorMap) {
  std::stringstream ss;

  std::map<int, std::string> err;
  for (int i = 0; i < 2; ++i) {
    err.emplace(std::rand() % 100, "op, error");
  }

  auto sendERROR = SerializerAnswer(std::rand() % 100, "Error", err);
  auto sendJsonERROR = sendERROR.GetJson();
  pt::write_json(ss, sendJsonERROR);

  pt::ptree recvJsonERROR;
  pt::read_json(ss, recvJsonERROR);
  EXPECT_EQ(sendJsonERROR, recvJsonERROR);

  auto recvERROR = SerializerAnswer(recvJsonERROR);
  auto statusERROR = recvERROR.GetStatus();
  std::visit(visitor{}, statusERROR);
}