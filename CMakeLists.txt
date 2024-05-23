#   CMakeLists.txt
#
#   Author: Paul Hamilton (paul@visualops.com)
#   Date: 21-May-2024
#
#   Input file for ZMQChat driver for Visual Ops
#
#   Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
#   
#   https://github.com/visualopsholdings/zmqchat

cmake_minimum_required(VERSION 3.5)

project (zmqchat)
  find_package(Boost 1.85.0 COMPONENTS program_options filesystem unit_test_framework chrono thread log_setup log json REQUIRED)
  find_package(cppzmq REQUIRED)
  find_package(mongocxx REQUIRED)
  find_package(bsoncxx REQUIRED)
  add_definitions(-DBOOST_ALL_DYN_LINK) 
  enable_testing()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

if (UNIX AND NOT APPLE)
  add_definitions(-funwind-tables) 
endif ()
if (APPLE)
  include_directories(/usr/local/include)
  LINK_DIRECTORIES(/opt/homebrew/Cellar/zeromq/4.3.5_1/lib)
endif ()

set(LIBS zmq)
set(MONGOLIBS mongo::mongocxx_shared mongo::bsoncxx_shared)
set(BOOSTLIBS ${Boost_CHRONO_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} 
      ${Boost_FILESYSTEM_LIBRARY} ${Boost_LOG_SETUP_LIBRARY} ${Boost_LOG_LIBRARY} ${Boost_JSON_LIBRARY} )

include_directories(include ${LIBMONGOCXX_INCLUDE_DIR} ${LIBBSONCXX_INCLUDE_DIR})

add_executable(ZMQCHAT src/zmqchat.cpp src/server.cpp src/loginmsg.cpp src/storage.cpp)
    target_link_libraries(ZMQCHAT ${LIBS} ${BOOSTLIBS} ${MONGOLIBS})

add_executable(Send test/send.cpp)
  target_link_libraries(Send ${LIBS} ${BOOSTLIBS})

add_executable(TestJson test/json.cpp)
  target_link_libraries(TestJson ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_JSON_LIBRARY})

add_test(TestJson TestJson)

add_executable(TestStorage test/storage.cpp src/storage.cpp)
  target_link_libraries(TestStorage ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_JSON_LIBRARY} ${MONGOLIBS})

add_test(TestStorage TestStorage)
