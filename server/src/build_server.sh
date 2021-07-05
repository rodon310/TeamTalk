#!/bin/bash
###
 # @File: build_server.sh
 # @Author: xiaominfc
 # @Date: 2021-06-28 17:50:29
 # @Description: run for compile source
### 


server_dir=$1
echo "compile: "${server_dir}

build_dir=build


if [[ -d $server_dir  && -f ${server_dir}/CMakeLists.txt ]];then
	cd ${server_dir}
	if [ ! -d ${build_dir} ];then
		mkdir ${build_dir}
		cd ${build_dir}
		cmake ..
		cd ..
	fi
	cd ${build_dir}
	make -j 8
	cd ../..
else
	echo "not such dir or not CMakeLists.txt in dir for: "${server_dir}
fi

if [ $server_dir == 'base' ];then
echo "sync base"
cp -f ./base/${build_dir}/libbase.a ./base/
fi

if [ $server_dir == 'slog' ];then
echo "sync slog"
cp -f ./slog/${build_dir}/libslog.so ./slog/
fi
