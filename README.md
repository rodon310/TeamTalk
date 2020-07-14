# TeamTalk
	TeamTalk is a solution for enterprise IM
	
	具体文档见doc目录下,安装之前请仔细阅读相关文档。

# 改进

~~~~
1.修复android 加载历史消息的bug
2.为了兼容web端播放音频 android／ios改用ogg+opus的格式编码
3.修复php部分bug
4.修复ios部分bug
5.添加ssl的服务端支持
6.升级protobuf到3.5.1
7.ios使用google 官方的objc pb方便统一pb文件的生成
8.也换了android的pb类 以及protobuf的版本 删了一些不用的依赖库
9.android添加离线文件的上传跟下载功能
10.docker 部署编译的支持
11.关联CMsgConn跟CBaseSocket 减少对map的遍历查找
~~~~

~~~~
2018-11-23:
	msg_server 添加线程池处理任务


2019-01-22:
	加上支持监听和连接unix socket的功能 使用后应该会有助于本地连接的稳定性


2019-07-07:
	升级CodeIgniter 至3.1.10



2020-07-07:
	添加一个服务辅助聊天机器人的开发

~~~~

### QQ群讨论(677724403)



# dart 客户端api的实现

[https://github.com/xiaominfc/teamtalk_dart_lib](https://github.com/xiaominfc/teamtalk_dart_lib)


# flutter 实现
[https://github.com/xiaominfc/flutter_tt](https://github.com/xiaominfc/flutter_tt)

# 测试的msg_server

[http://im.xiaominfc.com:8080/msg_server](http://im.xiaominfc.com:8080/msg_server)

## 测试账号 密码
~~~~
xiaoyang 123456
test  123456
~~~~

## 后台管理测试(账号 密码: test test)

### ant版
[http://tt.xiaominfc.com](http://tt.xiaominfc.com)


### 原版
[http://admin.xiaominfc.com](http://admin.xiaominfc.com)




# 在线demo

[http://im.xiaominfc.com/](http://im.xiaominfc.com/)


# 估值一个亿的ai代码测试:
![ai代码测试](https://raw.githubusercontent.com/xiaominfc/TeamTalk/master/android_test_ai.png)



