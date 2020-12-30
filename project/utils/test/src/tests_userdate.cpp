#include "gtest_utils.h"

TEST(UserDate, Struct) {
  auto userDate = UserDate{.userId = 1,
      .date = "31.12.1970"};

  auto serializer = SerializerUserDate(1, userDate);

  std::stringstream ss(R"({"command" : "DateUpdate", "requestId" : 1, "userId" : 1, "date" : "31.12.1970"})");
  pt::ptree root;
  pt::read_json(ss, root);

  EXPECT_EQ(serializer.GetJson(), root);
}

TEST(UserDate, Json) {
  std::stringstream ss(R"({"command" : "DateUpdate", "requestId" : 1, "userId" : 1, "date" : "31.12.1970"})");
  pt::ptree root;
  pt::read_json(ss, root);

  auto serializer = SerializerUserDate(root);

  auto userDate = UserDate{.userId = 1,
      .date = "31.12.1970"};

  auto date = serializer.GetUserDate();
  EXPECT_EQ(userDate.userId, date.userId);
  EXPECT_EQ(userDate.date, date.date);
}