#!/bin/sh

protobuf_bin_dir=../server/src/third_party/protobuf/bin

if [ ! -d ${protobuf_bin_dir} ];then
if [ ! -z $(which protoc) ];then
	echo "no compile protobuf, use system protoc"
else
	echo "no compile protobuf, please see ../server/src/README.md"
	exit 1 
fi
else 

export PATH=$PATH:${protobuf_bin_dir}
fi

SRC_DIR=./
DST_DIR=./gen

#C++
mkdir -p $DST_DIR/cpp
protoc -I=$SRC_DIR --cpp_out=$DST_DIR/cpp/ $SRC_DIR/*.proto

#JAVA
mkdir -p $DST_DIR/java
protoc -I=$SRC_DIR --java_out=$DST_DIR/java/ $SRC_DIR/*.proto

#OBJC
mkdir -p $DST_DIR/objc
protoc -I=$SRC_DIR --objc_out=$DST_DIR/objc/ $SRC_DIR/*.proto

#PYTHON
mkdir -p $DST_DIR/python
protoc -I=$SRC_DIR --python_out=$DST_DIR/python/ $SRC_DIR/*.proto
