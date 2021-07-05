# 编译说明

## 编译环境
TeamTalk编译最低需要c++11标准，确保gcc在4.9以上或者能支持C++11

## 第三方库
TeamTalk使用了许多第三方库，包括protobuf,hiredis,mariadb(mysql),log4cxx等等,在第一次编译TeamTalk之前,建议先执行目录下的

### mariadb(mysql)
可以通过yum或apt直接装到系统里

### hiredis
执行 ./third_party/make_hiredis.sh

### log4cxx
执行 ./third_party/make_log4cxx.sh

### protobuf
执行 ./third_party/make_protobuf.sh

## 同步pb(如果需要修改)

```
cd ../../pb

./create.sh
./sync.sh #只同步到服务端(sync_project.sh会一起同步到android ios等工程)

cd ../server/src

```


## 编译TeamTalk服务器
当以上步骤都完成后，可以使用"./build.sh version 1"编译整个TeamTalk工程,一旦编译完成，会在上级目录生成im_server_x.tar.gz包，该压缩包包含的内容有:
	lib: 主要包含各个服务器依赖的第三方库
	restart.sh: 启动脚本，启动方式为./restart.sh msg_server
	login_server:
	msg_server:
	route_server:			
	db_proxy_server:
	file_server:
	push_server:
	msfs:


## 运行
修改各个server下的conf文件 取保端口未被占用 然后启动
