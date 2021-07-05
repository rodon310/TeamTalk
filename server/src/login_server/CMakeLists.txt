cmake_minimum_required(VERSION 2.8.12)
PROJECT(loginserver)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)

#SET(LIBRARY_OUTPUT_PATH ./)
SET(EXECUTABLE_OUTPUT_PATH  ./)

INCLUDE(${PROJECT_SOURCE_DIR}/../base/base_dep.cmake)

INCLUDE_DIRECTORIES(./ ../http_msg_server)

LIST(APPEND SRC_LIST ${PROJECT_SOURCE_DIR}/../http_msg_server/HttpConn.cpp)


#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面，否则会报错
#ADD_LIBRARY(${PROJECTNAME} SHARED/STATIC ${SRC_LIST})
ADD_EXECUTABLE(login_server ${SRC_LIST})

TARGET_LINK_LIBRARIES(login_server base protobuf-lite slog pthread ssl crypto) 
