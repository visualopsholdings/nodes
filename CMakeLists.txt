#   CMakeLists.txt
#
#   Author: Paul Hamilton (paul@visualops.com)
#   Date: 21-May-2024
#
#   Input file for Nodes driver for Visual Ops
#
#   Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
#   
#   https://github.com/visualopsholdings/nodes

cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0167 NEW) # Boost
cmake_policy(SET CMP0169 OLD) # FetchContent

project (nodes)
  find_package(Boost 1.85.0 COMPONENTS program_options filesystem unit_test_framework chrono thread log_setup log json REQUIRED)
#  find_package(Boost 1.85.0 COMPONENTS program_options filesystem unit_test_framework chrono thread json REQUIRED)
  find_package(cppzmq REQUIRED)
  find_package(mongocxx REQUIRED)
  find_package(bsoncxx REQUIRED)
  find_package(OpenSSL REQUIRED)
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

# fetch from other repos.

include(FetchContent)

FetchContent_Declare(
  nodes-test
  GIT_REPOSITORY https://github.com/visualopsholdings/nodes-test.git
  GIT_TAG main
)
FetchContent_GetProperties(nodes-test)

if(NOT nodes-test_POPULATED)
  message(STATUS "Fetching nodes-test...")
  FetchContent_Populate(nodes-test)
endif()

FetchContent_Declare(
  base64
  GIT_REPOSITORY https://github.com/tobiaslocker/base64.git
  GIT_TAG master
)
FetchContent_GetProperties(base64)

if(NOT base64_POPULATED)
  message(STATUS "Fetching base64...")
  FetchContent_Populate(base64)
  include_directories(${base64_SOURCE_DIR}/include)
endif()

set(LIBS zmq)
set(MONGOLIBS mongo::mongocxx_shared mongo::bsoncxx_shared)
set(BOOSTLIBS ${Boost_LOG_LIBRARY} ${Boost_JSON_LIBRARY})
#set(BOOSTLIBS ${Boost_JSON_LIBRARY})
set(CORE
  src/json.cpp
  src/date.cpp
  src/data.cpp
)
set(UTILS
  src/vid.cpp 
  src/security.cpp
  src/upstream.cpp
  src/downstream.cpp
  src/encrypter.cpp
  src/log.cpp
)
set(MESSAGES 
  src/msg/login.cpp 
  src/msg/policyusers.cpp
  src/msg/objects.cpp
  src/msg/object.cpp
  src/msg/users.cpp
  src/msg/user.cpp
  src/msg/infos.cpp
  src/msg/setinfo.cpp
  src/msg/site.cpp
  src/msg/setsite.cpp
  src/msg/query.cpp
  src/msg/adduser.cpp
  src/msg/reload.cpp
  src/msg/groups.cpp
  src/msg/group.cpp
  src/msg/members.cpp
  src/msg/searchusers.cpp
  src/msg/addmember.cpp
  src/msg/deletemember.cpp
  src/msg/setuser.cpp
  src/msg/policy.cpp
  src/msg/addobject.cpp
  src/msg/deleteobject.cpp
  src/msg/moveobject.cpp
  src/msg/addgroup.cpp
  src/msg/setgroup.cpp
  src/msg/setobject.cpp
  src/msg/deletegroup.cpp
  src/msg/setobjectpolicy.cpp
  src/msg/setgrouppolicy.cpp
  src/msg/policygroups.cpp
  src/msg/sharelink.cpp
  src/msg/canregister.cpp
  src/msg/deleteuser.cpp
  src/msg/nodes.cpp
  src/msg/node.cpp
  src/msg/addnode.cpp
  src/msg/deletenode.cpp
  src/msg/purgecountgroups.cpp
  src/msg/purgegroups.cpp
  src/msg/purgecount.cpp
  src/msg/purge.cpp
  src/msg/purgecountusers.cpp
  src/msg/purgeusers.cpp
  src/rdrmsg/upstream.cpp
  src/rdrmsg/date.cpp
  src/rdrmsg/sendOn.cpp
  src/rdrmsg/discoverResult.cpp
  src/drmsg/online.cpp
  src/drmsg/discoverLocal.cpp
  src/drmsg/heartbeat.cpp
  src/drmsg/query.cpp
  src/drmsg/upd.cpp
  src/drmsg/add.cpp
  src/rmsmsg/upd.cpp
  src/rmsmsg/add.cpp
  src/handler.cpp
)

include_directories(include ${LIBMONGOCXX_INCLUDE_DIR} ${LIBBSONCXX_INCLUDE_DIR})

add_library(StorageLib SHARED 
  src/storage/storage.cpp 
  src/storage/storagei.cpp 
  src/storage/schemai.cpp 
  src/storage/result.cpp 
  src/storage/dynamicrow.cpp 
  src/storage/schema.cpp 
  src/storage/group.cpp 
  ${CORE})
    target_link_libraries(StorageLib ${BOOSTLIBS} ${MONGOLIBS})

add_executable(nodes src/nodes.cpp src/server.cpp ${MESSAGES} ${UTILS})
    target_link_libraries(nodes StorageLib ${Boost_PROGRAM_OPTIONS_LIBRARY} ${LIBS} ${MONGOLIBS} ${OPENSSL_LIBRARIES})

add_executable(Send test/send.cpp src/log.cpp)
  target_link_libraries(Send ${Boost_PROGRAM_OPTIONS_LIBRARY} ${BOOSTLIBS} ${LIBS})

add_executable(Counts test/counts.cpp src/log.cpp)
  target_link_libraries(Counts StorageLib ${Boost_PROGRAM_OPTIONS_LIBRARY} ${BOOSTLIBS} ${LIBS} ${MONGOLIBS})

add_executable(nodesaggregate src/nodesaggregate.cpp src/log.cpp)
  target_link_libraries(nodesaggregate StorageLib ${Boost_PROGRAM_OPTIONS_LIBRARY} ${BOOSTLIBS} ${MONGOLIBS})

# Tests

add_executable(TestStorage test/teststorage.cpp)
  target_link_libraries(TestStorage StorageLib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${BOOSTLIBS} ${MONGOLIBS})

add_test(TestStorage TestStorage)

add_executable(TestResult test/testresult.cpp src/storage/result.cpp ${CORE})
  target_link_libraries(TestResult ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${BOOSTLIBS} ${MONGOLIBS})

add_test(TestResult TestResult)

add_executable(TestVID test/testvid.cpp src/vid.cpp)
  target_link_libraries(TestVID ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${BOOSTLIBS})

add_test(TestVID TestVID)

add_executable(TestSecurity test/testsecurity.cpp src/security.cpp src/vid.cpp src/encrypter.cpp)
  target_link_libraries(TestSecurity StorageLib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${OPENSSL_LIBRARIES} ${BOOSTLIBS} ${MONGOLIBS})

add_test(TestSecurity TestSecurity)

add_executable(TestAggregate test/testaggregate.cpp)
  target_link_libraries(TestAggregate StorageLib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${BOOSTLIBS} ${MONGOLIBS})

add_test(TestAggregate TestAggregate)

add_executable(TestDate test/testdate.cpp src/date.cpp)
  target_link_libraries(TestDate ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${BOOSTLIBS})

add_test(TestDate TestDate)

add_executable(TestJson test/testjson.cpp src/json.cpp src/date.cpp)
  target_link_libraries(TestJson ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${BOOSTLIBS})

add_test(TestJson TestJson)

add_executable(TestEncrypter test/testencrypter.cpp src/encrypter.cpp)
  target_link_libraries(TestEncrypter ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${BOOSTLIBS} ${OPENSSL_LIBRARIES})

add_test(TestEncrypter TestEncrypter)

add_executable(TestData test/testdata.cpp src/data.cpp)
  target_link_libraries(TestData ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${BOOSTLIBS})

add_test(TestData TestData)

