FIND_PACKAGE(GTest REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

PROJECT(UtilsTest)
SET(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")

INCLUDE_DIRECTORIES(include)

ADD_EXECUTABLE(${PROJECT_NAME}
        src/main.cpp
        src/tests_answer.cpp
        src/tests_userdate.cpp
        src/tests_filemeta.cpp
        )

TARGET_LINK_LIBRARIES(${PROJECT_NAME} GTest::gtest ${CMAKE_THREAD_LIBS_INIT} Utils-dev)

ADD_TEST(NAME Tests COMMAND ${PROJECT_NAME})
