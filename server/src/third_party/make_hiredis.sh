#!/bin/bash
# author: xiaominfc
# date: 07/04/2021
# decription: compile hiredis

HIREDIS_VERSION=1.0.0
HIREDIS_DOWNLOAD_URL=https://github.com/redis/hiredis/archive/refs/tags/v${HIREDIS_VERSION}.tar.gz

pushd .

script_dir=$(dirname $0)
cd ${script_dir}
if [ ! -d hiredis ];then
mkdir -p hiredis
fi

cd hiredis

if [ ! -f v${HIREDIS_VERSION}.tar.gz ];then
	wget ${HIREDIS_DOWNLOAD_URL}
fi

if [ ! -d hiredis-${HIREDIS_VERSION} ];then
   tar -xf v${HIREDIS_VERSION}.tar.gz
fi

cd hiredis-${HIREDIS_VERSION}

make -j 4

mkdir -p ../lib

cp ./libhiredis.a ../lib/

if [ -d ../include ];then
	rm -rf ../include
fi

mkdir -p ../include

cp ./*.h ../include/

popd