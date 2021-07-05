#!/bin/bash
# author: xiaominfc
# date: 07/04/2021
# decription: compile protobuf

PROTOBUF_VERSION=3.5.1
PROTOBUF_DOWNLOAD_URL=https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-all-${PROTOBUF_VERSION}.tar.gz

pushd .

script_dir=$(dirname $0)
cd ${script_dir}

if [ ! -d protobuf ];then
mkdir -p protobuf
fi
cd protobuf


if [ ! -f protobuf-all-${PROTOBUF_VERSION}.tar.gz ];then
	wget ${PROTOBUF_DOWNLOAD_URL}
fi

if [ ! -d protobuf-${PROTOBUF_VERSION} ];then
tar -xf protobuf-all-${PROTOBUF_VERSION}.tar.gz
fi
INSTALL_DIR=$(pwd)

cd protobuf-${PROTOBUF_VERSION}

./configure --prefix=$INSTALL_DIR --enable-shared=no

make -j 8
make install

popd