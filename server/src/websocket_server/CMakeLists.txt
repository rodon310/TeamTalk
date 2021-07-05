cmake_minimum_required(VERSION 2.8.12)
PROJECT(websocket_server)

add_subdirectory(${PROJECT_SOURCE_DIR}/websocket)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/../msg_server MSG_SERVER_SRC_LIST)

#SET(LIBRARY_OUTPUT_PATH ./)
SET(EXECUTABLE_OUTPUT_PATH  ./)


INCLUDE(${PROJECT_SOURCE_DIR}/../base/base_dep.cmake)

if(APPLE)
    SET(SECURITY_LIB_DIR ../base/security/lib/mac)
    SET(OTHER_INCLUDE /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift-migrator/sdk/MacOSX.sdk/usr/include/)
elseif(UNIX)
    SET(SECURITY_LIB_DIR ../base/security/lib/linux)
endif()

INCLUDE_DIRECTORIES(./ ../msg_server ../base ./websocket)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/websocket)

#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面，否则会报错
#ADD_LIBRARY(${PROJECTNAME} SHARED/STATIC ${SRC_LIST})

LIST(REMOVE_ITEM MSG_SERVER_SRC_LIST ${PROJECT_SOURCE_DIR}/../msg_server/msg_server.cpp)

ADD_EXECUTABLE(websocket_server ${SRC_LIST} ${MSG_SERVER_SRC_LIST})

TARGET_LINK_LIBRARIES(websocket_server base websocket protobuf-lite slog crypto ssl pthread )
 
