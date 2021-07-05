cmake_minimum_required(VERSION 2.8.12)
PROJECT(fileserver)

AUX_SOURCE_DIRECTORY(./ SRC_LIST1)
#AUX_SOURCE_DIRECTORY(../base/pb/protocol SRC_LIST2)

#SET(LIBRARY_OUTPUT_PATH ./)
SET(EXECUTABLE_OUTPUT_PATH  ./)

include(${PROJECT_SOURCE_DIR}/../base/base_dep.cmake)

INCLUDE_DIRECTORIES(./)

#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面，否则会报错
#ADD_LIBRARY(${PROJECTNAME} SHARED/STATIC ${SRC_LIST})
ADD_EXECUTABLE(file_server ${SRC_LIST1} ${SRC_LIST2})

TARGET_LINK_LIBRARIES(file_server pthread base protobuf-lite uuid slog ssl crypto)
 
