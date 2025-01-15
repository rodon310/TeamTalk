#!/bin/bash
# author: xiaominfc
# date: 07/04/2021
# decription: compile log4cxx

LOG4CXX_VERSION=0.10.0
LOG4CXX=apache-log4cxx-${LOG4CXX_VERSION}
LOG4CXX_PATH=https://archive.apache.org/dist/logging/log4cxx/${LOG4CXX_VERSION}/${LOG4CXX}.tar.gz
pushd .


script_dir=$(dirname $0)
cd ${script_dir}
mkdir -p ./log4cxx
cd log4cxx

if [ ! -f ${LOG4CXX}.tar.gz ];then
	echo "download file:"${LOG4CXX_PATH}
	wget ${LOG4CXX_PATH}
fi

if [ ! -d $LOG4CXX ];then
    tar -xf $LOG4CXX.tar.gz
fi

APR_DIR=/usr
APR_UTIL_DIR=/usr

sed_cmd=sed

if [[ ! -z $(which yum) || -f /usr/bin/yum ]]; then 
		echo "found yum, the os maybe centos! install deps by yum"
		yum -y install apr-devel apr-util-devel
elif [ ! -z  $(which brew) ]; then
		echo "found brew, the os maybe macos! install deps by brew"
		export HOMEBREW_NO_AUTO_UPDATE=1 #skip update
		brew install apr apr-util gsed
		APR_DIR=/usr/local/opt/apr 
		APR_UTIL_DIR=/usr/local/opt/apr-util
		sed_cmd=gsed
elif [ ! -z $(which apt) ]; then
		echo "found apt, the os maybe ubuntu! install deps by apt"
		sudo apt-get -y install libapr1-dev libaprutil1-dev
fi

INSTALL_DIR=$(pwd)
cd $LOG4CXX

## pitch

${sed_cmd} -i '26c #include <cstring>' ./src/main/include/log4cxx/logstring.h

${sed_cmd} -i '151c unsigned char prolog[] = {' ./src/main/cpp/locationinfo.cpp
${sed_cmd} -i '164c os.writeProlog("org.apache.log4j.spi.LocationInfo", 2, (char*)prolog, sizeof(prolog), p);' ./src/main/cpp/locationinfo.cpp


${sed_cmd} -i '239c unsigned char classDesc[] = {' ./src/main/cpp/loggingevent.cpp
${sed_cmd} -i '295c 8, (char*)classDesc, sizeof(classDesc), p);' ./src/main/cpp/loggingevent.cpp

${sed_cmd} -i '39c unsigned char start[] = { 0xAC, 0xED, 0x00, 0x05 };' ./src/main/cpp/objectoutputstream.cpp
${sed_cmd} -i '40c ByteBuffer buf((char*)start, sizeof(start));' ./src/main/cpp/objectoutputstream.cpp
${sed_cmd} -i '84c unsigned char prolog[] = {' ./src/main/cpp/objectoutputstream.cpp
${sed_cmd} -i '93c writeProlog("java.util.Hashtable", 1, (char*)prolog, sizeof(prolog), p);' ./src/main/cpp/objectoutputstream.cpp
${sed_cmd} -i '96c unsigned char data[] = { 0x3F, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,' ./src/main/cpp/objectoutputstream.cpp
${sed_cmd} -i '98c ByteBuffer dataBuf((char*)data, sizeof(data));' ./src/main/cpp/objectoutputstream.cpp

${sed_cmd} -i '193c const logchar fname[] = { 0x6F, 0x75, 0x74, 0x70, 0x75, 0x74, 0x2F, 0x64, 0x6F, 0x6D, static_cast<logchar>(0xC2), static_cast<logchar>(0xB3), 0 };' src/test/cpp/xml/domtestcase.cpp
${sed_cmd} -i '195c const logchar fname[] = { 0x6F, 0x75, 0x74, 0x70, 0x75, 0x74, 0x2F, 0x64, 0x6F, 0x6D, static_cast<logchar>(0xB3), 0 };' src/test/cpp/xml/domtestcase.cpp
${sed_cmd} -i '212c const logchar fname[] = { 0x6F, 0x75, 0x74, 0x70, 0x75, 0x74, 0x2F, 0x64, 0x6F, 0x6D, static_cast<logchar>(0xE3), static_cast<logchar>(0x86), static_cast<logchar>(0x95), 0 };' src/test/cpp/xml/domtestcase.cpp
${sed_cmd} -i '214c const logchar fname[] = { 0x6F, 0x75, 0x74, 0x70, 0x75, 0x74, 0x2F, 0x64, 0x6F, 0x6D, static_cast<logchar>(0x3195), 0 };' src/test/cpp/xml/domtestcase.cpp

if [ ! -z  $(which brew) ]; then
	 # for mac os fix locale bug
   ${sed_cmd} -i '32c #include <locale>' ./src/main/include/log4cxx/helpers/simpledateformat.h
fi


./configure --prefix=$INSTALL_DIR --with-apr=${APR_DIR} --with-apr-util=${APR_UTIL_DIR}
make -j 4
make install

popd

