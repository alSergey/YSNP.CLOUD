CMAKE_MINIMUM_REQUIRED(VERSION 3.13.4)
PROJECT(PostgreSQL)

SET(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic")

FIND_PACKAGE(PostgreSQL REQUIRED)

FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(Boost COMPONENTS log REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

INCLUDE_DIRECTORIES(Base/include
        MetaDataDB/include
        UserDB/include
        ../../utils/include)

ADD_LIBRARY(MetDataDB-dev SHARED
        Base/src/PostgreSQLDB.cpp
        MetaDataDB/src/MetaDataDB.cpp
        )
target_include_directories(MetDataDB-dev PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(MetDataDB-dev PRIVATE ${PostgreSQL_LIBRARIES} ${Boost_LIBRARIES})

ADD_LIBRARY(UserDB-dev SHARED
        Base/src/PostgreSQLDB.cpp
        UserDB/src/UsersDB.cpp
        )
target_include_directories(UserDB-dev PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(UserDB-dev PRIVATE ${PostgreSQL_LIBRARIES} ${Boost_LIBRARIES})
