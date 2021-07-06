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


## 编译
当以上步骤都完成后，可以使用"./build.sh version 1"编译整个TeamTalk工程,一旦编译完成，会在上级目录生成im_server_x.tar.gz包，该压缩包包含的内容有:
	lib: 主要包含各个服务器依赖的第三方库
	restart.sh: 启动脚本，启动方式为./restart.sh msg_server
	login_server:
	websocket_server
	route_server:			
	db_proxy_server:
	file_server:
	push_server:
	http_msg_server:
	http_aiclient_server:
	msfs:

### 辅助编译脚本build_server.sh
可以指定服务编译 但是前提是编译过slog跟base

```
./build_server.sh slog
./build_server.sh base
#上述执行成功一次即可

./build_server.sh {server_name} 
# {server_name} 像 login_server 等之类的

```

本质上 build.sh也是调用 build_server.sh来编译各个模块以及server的
可以看看build_server.sh 具体是在相应的路径下建了一个名为build的文件夹 通过cmake ..在build里生成了Makefile 然后用make执行编译 这样更加便于维护 不会让编译生成的中间产物跟源码都在一个路径里了


## 运行
修改各个server下的conf文件 取保端口未被占用 然后启动


## 各个模块以及服务的基本介绍

### slog
这是一个基于log4cxx二次封装的日志库主要是提供简洁的接口方式日志输出

### base

1. tt各个服务依赖的基础库 
2. 实现了事件驱动的IO模型 
3. 实现tcp socket的监听以及 连接 
4. 定义数据交互协议PDU(16字节头+pb序列化消息)
5. 定义了一些工具类 涉及参数解析  http的get post请求等 解析http协议的工具类
6. 定义了一套实例基于计数器引用的生命周期的实现

### db_proxy_server

1. 几乎所有的跟数据库打交道的业务都是这个服务完成
2. 里面有两种连接池 一种是操作mysql的 还有一种是操作redis
3. mysql连接池设计成读写分离的 对于后续的业务修改 涉及到 读操作的 尽量使用slave 写操作的则用master
4. 对外提供tcp监听 Pdu(16字节头+pb序列化消息)格式化数据交互
5. 支持多机 多实例部署 但是有个同步环节 只需指定一台(指定多台会出现数据重复的情况)

### msg_server

1. 提供tcp服务的监听
2. 维护所有与客户端(android ios pc等t)的长连接
3. 解析并转发客户端的请求到指定的服务进行处理(大多数业务都是转发到db_proxy_server 所以msg_server会与db_proxy_server一直保持连接)
4. 可以多机 多实例部署

### websocket_server

1. 在msg_server的基础拓展websocket的协议 可以支持网页上js的交互
2. 保持对msg_server 所有功能的兼容性

### login_server

1. 会统计各个msg_server(websocket_server)的负载情况
2. 提供http监听 返回负载最小的msg_server(websocket_server)
3. 可以多机 多实例部署

### route_server

1. 作为多机多实例部署各个msg_server(websocket_server)的中级， 让不同msg_server的各个客户端的实现交互
2. 可以多机 多实例部署(通常部署一个就好)


### push_server

1. 跟apple的推送服务建立连接 辅助实现apple设备的消息推送
2. 可以多机 多实例部署

### msfs

1. 一个小型的文件存储系统服务
2. 通过http 实现文件的上传以及访问
3. 可以用别的类似服务替换 客户端实现相应http请求即可


### http_msg_server

1. 类似msg_server 不过它提供的是http的协议
2. 可以拓展其他业务 在没有客户端登陆的情况下 模拟一些客户端的行为
3. 也可以实现一些轻量级的业务处理

### http_aiclient_server

1. 提供http监听
2. 基于test_client中的代码 后端上模拟真实客户端
3. 模拟的客户端可以监听新消息  可以回复消息 
4. 可以拓展实现聊天机器人