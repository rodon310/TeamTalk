/*
 * AiClient.h
 * Copyright (C) 2020 xiaominfc(武汉鸣鸾信息科技有限公司)
 * Email: xiaominfc@gmail.com
 * Distributed under terms of the MIT license.
 */

#ifndef AICLIENT_H
#define AICLIENT_H
#include "HttpConn.h"
#include "HttpPdu.h"
#include "Client.h"
#include "ClientConn.h"
#include "IM.BaseDefine.pb.h"
#include "HttpQuery.h"
#include "Thread.h"

using namespace std;

#define HTTP_JSON_QUEYR_HEADER "HTTP/1.1 200 OK\r\n"\
		"Cache-Control:no-cache\r\n"\
		"Connection:close\r\n"\
		"Content-Length:%d\r\n"\
		"Content-Type:application/json;charset=utf-8\r\n\r\n%s"


class ReConnectThread;
class AiClient;


class AiClient:public CClient{

public:
	AiClient(const string& strName, const string& strPass, const string strDomain="http://im.xiaominfc.com:8080");
	~AiClient();
	void connect();
	void doLogin(uint32_t handle,const string &callbackurl);
	virtual void onLogin(uint32_t nSeqNo, uint32_t nResultCode, string& strMsg, IM::BaseDefine::UserInfo* pUser = NULL);
	void OnLoginOk();
	void ErrorLogin(const char *format, ...);
	void onClose();
	void onRecvMsg(uint32_t nSeqNo, uint32_t nFromId, uint32_t nToId, uint32_t nMsgId, uint32_t nCreateTime, IM::BaseDefine::MsgType nMsgType, const string& strMsgData);
	void toJson(Json::Value &json_data);
public:
	int m_try_time;
	uint32_t http_handle;
private:
	ReConnectThread* workThread;
	string m_callbackurl;
};


class ReConnectThread:public CThread {
public:
	ReConnectThread(AiClient *aiclient);
	~ReConnectThread();
	void OnThreadRun(void);

private:
	AiClient* m_aiClient;
};

typedef hash_map<uint32_t, AiClient*> AiClientMap_t;

AiClient* FindAiClientById(uint32_t uid);

void RemoveAiClientById(uint32_t uid);

void InitLastLoginData(string &filepath);
char* OnlineInfo();



#endif /* !AICLIENT_H */
