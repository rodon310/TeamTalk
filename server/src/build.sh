#!/bin/bash


build() {

	if [[ ! -z $(which yum) || -f /usr/bin/yum ]]; then 
		echo "found yum, the os maybe centos! install deps by yum"
		yum -y install cmake
		yum -y install libuuid-devel
		yum -y install openssl-devel
		yum -y install curl-devel
	elif [ ! -z  $(which brew) ]; then
		echo "found brew, the os maybe macos! install deps by brew"
		brew install cmake openssl curl
	elif [ ! -z $(which apt) ]; then
		echo "found apt, the os maybe ubuntu! install deps by apt"
		apt -y install cmake
		apt -y install libuuid-dev 
		apt -y install libcurl4-openssl-dev
		apt -y install openssl-devel
		apt -y  install libcurl-dev 
	fi

	echo "#ifndef __VERSION_H__" > base/version.h
	echo "#define __VERSION_H__" >> base/version.h
	echo "#define VERSION \"$1\"" >> base/version.h
	echo "#endif" >> base/version.h

	base_deps="slog base"
	for dep in ${base_deps}
	do
		./build_server.sh ${dep}
		if [ ! -f ${dep}/build/lib${dep}.* ];then
			echo "compile failed for:"${dep}
			exit 1
		fi
	done

	projects="http_msg_server login_server route_server msg_server msfs websocket_server http_aiclient_server push_server"

	for project in ${projects}
	do 
		#rm -rf ${project}/build
		./build_server.sh ${project}
		if [ ! -f ${project}/build/${project} ];then
			echo "compile failed for:"${project}
			exit 1
		fi
	done


	cd ./tools
	make
	if [ $? -eq 0 ]; then
		echo "make tools successed";
	else
		echo "make tools failed";
		exit;
	fi
	cd ../

	projects="http_msg_server login_server route_server msg_server msfs websocket_server http_aiclient_server push_server"

	for project in ${projects}
	do
		mkdir -p ../run/${project}
		cp ${project}/build/${project} ../run/${project}/
		cp ${project}/*.conf ../run/${project}/
		cp slog/log4cxx.properties ../run/${project}/
	done

	cp tools/daeml ../run/
    
    build_version=im-server-$1
	build_name=${build_version}.tar.gz
    if [ -f ../${build_name} ];then
        build_version=im-server-$1-$(date +%s)
        build_name=${build_version}.tar.gz
    fi

	cp -r ../run ../${build_version}

	mkdir -p ../${build_version}/lib

	cp slog/libslog.so ../${build_version}/lib
	cp ./third_party/log4cxx/lib/liblog4cxx.so* ../${build_version}/lib

	cd ../
	tar zcvf ${build_name} ${build_version}

	rm -rf ${build_version}
}

clean() {
	base_deps="slog base"
	for dep in ${base_deps}
	do
		rm -rf ${project}/build
	done

	projects="http_msg_server login_server route_server msg_server msfs websocket_server http_aiclient_server push_server"

	for project in ${projects}
	do 
		rm -rf ${project}/build
	done
}

print_help() {
	echo "Usage: "
	echo "  $0 clean --- clean all build"
	echo "  $0 version version_str --- build a version"
}

case $1 in
	clean)
		echo "clean all build..."
		clean
		;;
	version)
		if [ $# != 2 ]; then 
			echo $#
			print_help
			exit
		fi

		echo $2
		echo "build..."
		build $2
		;;
	*)
		print_help
		;;
esac
