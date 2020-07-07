## 介绍

这是用于模拟用户登录以及维护长期登录状态的一个服务 支持新消息回调 便于对话机器人的开发 支持记忆之前登录成功的用户


## 登录

---
path: /aichat/Login
method: post
datatype: json
---


#### example
```
{
	"account":"xiaominfc", //账号
	"password":"123456",   //密码
	"callbackurl":"",      //接收到新消息的回调地址
	"msgserverurl":""	   //登录用的msg_server url的前面部分（去掉“/msg_server”的部分）
}
```

```
curl -H "Content-Type:application/json"  -X POST --data '{"account":"xiaominfc","password":"123456","msgserverurl":"http://im.xiaominfc.com:8080","callbackurl":"http://127.0.0.1:8401/aichat/callback"}'  http://127.0.0.1:8401/aichat/Login

```


## 发消息

---
path: /aichat/SendMsg
method: post
datatype: json
---

#### example
```
{
	"senderId":2,   //发送者ID
	"toId":1,		//接收者ID
	"text":"hi"		//发送的文本内容
}
```

```
curl -H "Content-Type:application/json"  -X POST --data '{"senderId":2,"toId":1,"text":"hi"}' http://127.0.0.1:8401/aichat/SendMsg
```


## 获取当前登录用户表

---
path: /aichat/Online
method: get
---

```
curl http://127.0.0.1:8401/aichat/Online
```