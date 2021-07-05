cmake_minimum_required(VERSION 2.8.12)
PROJECT(db_proxy_server)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)
AUX_SOURCE_DIRECTORY(./business SRC_LIST)

#SET(LIBRARY_OUTPUT_PATH ./)
SET(EXECUTABLE_OUTPUT_PATH  ./)

SET(MYSQL_LIB /usr/lib64/mysql)
SET(UBUNTU_MYSQL_LIB /usr/lib/x86_64-linux-gnu)
SET(MYSQL_LIB_NAME mysqlclient)

include(${PROJECT_SOURCE_DIR}/../base/base_dep.cmake)

SET(HIREDIS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../third_party/hiredis/include)
SET(HIREDIS_LIB_DIR ${PROJECT_SOURCE_DIR}/../third_party/hiredis/lib)

if(APPLE)
    SET(MYSQL_INCLUDE_DIR /usr/local/mysql/include)
    SET(MYSQL_LIB /usr/lib64/mysql /usr/local/mysql/lib)
    SET(MYSQL_LIB_NAME mysqlclient)
else()
    SET(MYSQL_INCLUDE_DIR /usr/include/mysql)
endif()


INCLUDE_DIRECTORIES(./ ./businiess ${MYSQL_INCLUDE_DIR} ${HIREDIS_INCLUDE_DIR})
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR} ${MYSQL_LIB} ${UBUNTU_MYSQL_LIB} ${HIREDIS_LIB_DIR})

#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面，否则会报错
#ADD_LIBRARY(${PROJECTNAME} SHARED/STATIC ${SRC_LIST})
ADD_EXECUTABLE(db_proxy_server ${SRC_LIST})

TARGET_LINK_LIBRARIES(db_proxy_server pthread base protobuf-lite ${MYSQL_LIB_NAME} hiredis curl slog ssl crypto)
 
