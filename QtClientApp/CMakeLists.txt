cmake_minimum_required(VERSION 3.5)

project(Cloud LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(Boost COMPONENTS system REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

SET(CMAKE_PREFIX_PATH "/Users/s.alekhin/Qt/5.15.2/clang_64/lib/cmake")
find_package(Qt5 COMPONENTS Widgets REQUIRED)

FIND_LIBRARY(App-dev REQUIRED)
INCLUDE_DIRECTORIES(../project/App/include)
INCLUDE_DIRECTORIES(../project/utils/include)
INCLUDE_DIRECTORIES(../project/DataBases/InternalDB/include)
INCLUDE_DIRECTORIES(../project/Network/Client/include)
INCLUDE_DIRECTORIES(../project/Client/Chunker/include)
INCLUDE_DIRECTORIES(../project/Client/Indexer/include)
INCLUDE_DIRECTORIES(../project/Client/Watcher/include)

if (APPLE)
    INCLUDE_DIRECTORIES(../project/Client/Watcher/Apple/include)
    INCLUDE_DIRECTORIES(../project/Client/Watcher/include)
else ()
    INCLUDE_DIRECTORIES(../project/Client/Watcher/include)
    INCLUDE_DIRECTORIES(../project/Client/Watcher/Linux/include)
endif ()

INCLUDE_DIRECTORIES(include)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/resourcePath.cpp
        src/mainwindow.cpp
        include/mainwindow.h
        src/mainwindow.ui
        src/auth_window.ui
        src/auth_window.cpp
        include/auth_window.h
        src/reg_window.cpp
        src/reg_window.ui
        include/reg_window.h
        src/settings.cpp
        src/settings.ui
        include/settings.h
        )

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets App-dev ${Boost_LIBRARIES})
