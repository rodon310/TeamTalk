cmake_minimum_required(VERSION 2.8.12)
PROJECT(http_msg_server)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/../test_client SRC_TEST_LIST)

#SET(LIBRARY_OUTPUT_PATH ./)
SET(EXECUTABLE_OUTPUT_PATH  ./)

INCLUDE(${PROJECT_SOURCE_DIR}/../base/base_dep.cmake)

INCLUDE_DIRECTORIES(./ ../http_msg_server ../test_client)

LIST(REMOVE_ITEM SRC_TEST_LIST ${PROJECT_SOURCE_DIR}/../test_client/test_client.cpp)
LIST(REMOVE_ITEM SRC_TEST_LIST ${PROJECT_SOURCE_DIR}/../test_client/playsound.mm)

LIST(APPEND SRC_LIST ${PROJECT_SOURCE_DIR}/../http_msg_server/AttachData.cpp)
LIST(APPEND SRC_LIST ${PROJECT_SOURCE_DIR}/../http_msg_server/HttpPdu.cpp)
LIST(APPEND SRC_LIST ${PROJECT_SOURCE_DIR}/../http_msg_server/HttpConn.cpp)


#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面，否则会报错
#ADD_LIBRARY(${PROJECTNAME} SHARED/STATIC ${SRC_LIST})
ADD_EXECUTABLE(http_aiclient_server ${SRC_LIST} ${SRC_TEST_LIST})

TARGET_LINK_LIBRARIES(http_aiclient_server base protobuf-lite slog  pthread ssl crypto curl) 
 
