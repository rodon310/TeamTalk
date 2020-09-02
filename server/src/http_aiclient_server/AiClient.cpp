/*
 * AiClient.cpp
 * Copyright (C) 2020 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "HttpClient.h"
#include "AiClient.h"
#include "Common.h"
#include "json/json.h"
#include<fstream> 
extern CAes *pAes;

static AiClientMap_t g_aiclient_map;

static string lastLoginUsersData;

AiClient* FindAiClientById(uint32_t uid){
	AiClient* aiclient = NULL;
	AiClientMap_t::iterator it = g_aiclient_map.find(uid);

	if (it != g_aiclient_map.end()){
		aiclient = it->second;
	}
	return aiclient;
}

void RemoveAiClientById(uint32_t uid){
	g_aiclient_map.erase(uid);
}

void InitLastLoginData(string &filepath){
	ifstream m_if(filepath);
	Json::Reader reader;
	Json::Value data;
	if(reader.parse(m_if,data)) {
		lastLoginUsersData.clear();
		lastLoginUsersData.append(data.toStyledString());
		int size = data["users"].size();
		for(int i =0 ; i < size; i++) {
			Json::Value user = data["users"][i];
			string account = user["account"].asString();
			string password = user["password"].asString();
			string msgserverurl = user["msgserverurl"].asString();
			string callbackurl = user["callbackurl"].asString();
			AiClient *aiclient  = new AiClient(account,password,msgserverurl);
			aiclient->doLogin(0,callbackurl);
		}
	}
}

void static updateLastLoginUsersData(){
	AiClientMap_t::iterator it = g_aiclient_map.begin();
	Json::Value loginusers;
	while (it != g_aiclient_map.end()){
		AiClient *aiclient = it->second;
		Json::Value user;
		aiclient->toJson(user);
		loginusers.append(user);
		++it;
	}
	Json::Value loginUsersData;
	loginUsersData["users"] = loginusers;

	string data = loginUsersData.toStyledString();
	if(data.compare(lastLoginUsersData) == 0) {
		return;
	}
	printf("update data\n");
	lastLoginUsersData.clear();
	lastLoginUsersData.append(data);
	fstream m_if("./last_users.txt",ios::trunc | ios::out);
	m_if<<lastLoginUsersData;
	m_if.flush();
	m_if.close();
}



char* OnlineInfo(){
	AiClientMap_t::iterator it = g_aiclient_map.begin();
	Json::Value user_list;
	while(it != g_aiclient_map.end()){
		Json::Value infoJson;
		AiClient *client = it->second;
		IM::BaseDefine::UserInfo info = client->m_cSelfInfo;
		infoJson["userId"] = info.user_id();
		infoJson["userName"] = info.user_real_name().c_str();
		user_list.append(infoJson);
		++it;
	}
	Json::Value result;
	result["userList"] = user_list;
	result["userCount"] = (uint32_t)g_aiclient_map.size();
	result["error_code"] = 0;
	string result_str = result.toStyledString();
	size_t content_len = result_str.size();
	size_t max_len = content_len + 2048;
	char *data = (char*)malloc(max_len);
	memset(data,0,max_len);
	//PackSendResult(data,max_len);
	snprintf(data, max_len, HTTP_JSON_QUEYR_HEADER, (int)content_len, result_str.c_str());
	return data;
}


AiClient::AiClient(const string& strName, const string& strPass, const string strDomain):
CClient(strName,strPass,strDomain)
{
	m_try_time = 0;
	workThread = NULL;
}


AiClient::~AiClient(){
	if(workThread){
		delete workThread;
	}
}

void AiClient::doLogin(uint32_t handle,const string &callbackurl){
	http_handle = handle;
	m_callbackurl = callbackurl;
	this->connect();
}

void AiClient::OnLoginOk(){
	
	if(http_handle <=0) {
		return;
	}
	CHttpConn* pHttpConn = FindHttpConnByHandle(http_handle);
	if(!pHttpConn){
		return;
	}
	char *response_buf = PackSendResult(0,"login ok");
	pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
	pHttpConn->Close();
	http_handle = 0;
}

void AiClient::ErrorLogin(const char *format, ...){

	if(http_handle <=0) {
		return;
	}

	CHttpConn* pHttpConn = FindHttpConnByHandle(http_handle);
	if(!pHttpConn){
		return;
	}

	va_list args;
	va_start(args, format);
	char szBuffer[1024];
	vsnprintf(szBuffer, sizeof(szBuffer), format, args);
	va_end(args);
	char *response_buf = PackSendResult(1,szBuffer);
	pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
	pHttpConn->Close();
	http_handle = 0;
}


void AiClient::toJson(Json::Value &json_data){
	json_data["account"] = m_strName.c_str();
	json_data["password"] = m_strPass.c_str();
	json_data["callbackurl"] = m_callbackurl.c_str();
	json_data["msgserverurl"] = m_strLoginDomain.c_str();
}

void AiClient::connect()
{
	CHttpClient httpClient;
	string strUrl = m_strLoginDomain + "/msg_server";
	string strResp;
	CURLcode nRet = httpClient.Get(strUrl, strResp);
	if(nRet != CURLE_OK)
	{
		printf("login falied. access url:%s error:%d\n", strUrl.c_str(), nRet);
		ErrorLogin("login falied. access url:%s error:%d\n", strUrl.c_str(), nRet);
		return;
	}
	Json::Reader reader;
	Json::Value value;
	if(!reader.parse(strResp, value))
	{
		printf("login falied. parse response error:%s\n", strResp.c_str());
		ErrorLogin("login falied. parse response error:%s\n", strResp.c_str());
		return;
	}
	string strPriorIp, strBackupIp;
	uint16_t nPort;
	try {
		uint32_t nRet = value["code"].asUInt();
		if(nRet != 0)
		{
			string strMsg = value["msg"].asString();
			printf("login falied. errorMsg:%s\n", strMsg.c_str());
			ErrorLogin("login falied errorMsg:%s\n", strMsg.c_str());
			//callback
			return;
		}
		strPriorIp = value["priorIP"].asString();
		strBackupIp = value["backupIp"].asString();
		nPort = atoi(value["port"].asString().c_str());
		
	} catch (std::runtime_error emsg) {
		printf("login falied. get json error:%s\n", strResp.c_str());
		//callback
		ErrorLogin("login falied. get json error:%s\n", strResp.c_str());
		return;
	}
	
	g_pConn = new ClientConn();
	g_pConn->setCallBack(this);
	m_nHandle = g_pConn->connect(strPriorIp.c_str(), nPort, m_strName, m_strPass);
	if(m_nHandle != INVALID_SOCKET)
	{
		netlib_register_timer(CClient::TimerCallback, this, 1000);
		m_try_time = 0;
	}
	else
	{
		printf("invalid socket handle\n");
		g_pConn->ReleaseRef();
		g_pConn = NULL;
		m_try_time = m_try_time - 1;
		if(m_try_time > 0) {
			sleep(5);
			printf("try connect again\n");
			connect();
			return;
		}
		log("reconnect failed");
	}
}


void AiClient::onClose(){
	printf("onClose for:%d\n",g_bLogined);
	bool lastLoginState = g_bLogined;
	RemoveAiClientById(m_cSelfInfo.user_id());
	CClient::onClose();
	if(lastLoginState) {
		printf("start reconnect\n");
		m_try_time = 12;
		if(workThread){
			delete workThread;
		}
		workThread = new ReConnectThread(this);
		workThread->StartThread();
	}
}

void AiClient::onRecvMsg(uint32_t nSeqNo, uint32_t nFromId, uint32_t nToId, uint32_t nMsgId, uint32_t nCreateTime, IM::BaseDefine::MsgType nMsgType, const string& strMsgData){
	log("onRecvMsg nSeqNo:%d  from:%d toId:%d msgId:%d createTime:%d nMsgType:%d \n",nSeqNo, nFromId,nToId,nMsgId, nCreateTime, nMsgType);
	if(nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_TEXT || nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_TEXT) {
		char* msg_out = NULL;
		uint32_t msg_out_len = 0;
		if (pAes->Decrypt(strMsgData.c_str(), strMsgData.length(), &msg_out, msg_out_len) == 0)
		{
			string msg_data = string(msg_out, msg_out_len);
			log("onRecvMsg  from:%d  text:%s\n",nFromId, msg_data.c_str());
			Json::Value data;
			data["text"] = msg_data.c_str();
			data["fromId"] = nFromId;
			data["toId"] = nToId;
			data["msgType"] = nMsgType;
			CHttpClient httpClient;
			string result;
			int ret = httpClient.PostJson(m_callbackurl,data.toStyledString(),result);
			log("post data:%s\nto url:%s  ret:%d \nresult:%s",data.toStyledString().c_str(), m_callbackurl.c_str(), ret, result.c_str());
		}
		pAes->Free(msg_out);
	}
}

void AiClient::onLogin(uint32_t nSeqNo, uint32_t nResultCode, string& strMsg, IM::BaseDefine::UserInfo* pUser){
	(void)nSeqNo;
	if(nResultCode != 0)
	{
		printf("login failed.errorCode=%u, msg=%s\n",nResultCode, strMsg.c_str());
		ErrorLogin("login failed.errorCode=%u, msg=%s\n",nResultCode, strMsg.c_str());
		return;
	}
	if(pUser)
	{
		printf("%s%s\n", "login ok for:",pUser->user_real_name().c_str());
		m_cSelfInfo = *pUser;
		g_bLogined = true;
		CClient::getChangedUser();
		AiClient *old = FindAiClientById(pUser->user_id());
		if(old){
			old->close();
			RemoveAiClientById(pUser->user_id());
			//delete old;
		}
		g_aiclient_map.insert(make_pair(pUser->user_id(), this));
		int lostHandle = http_handle;
		this->OnLoginOk();
		if(lostHandle > 0) {
			updateLastLoginUsersData();
		}
	}
	else
	{
		//callback
		printf("pUser is null\n");
		ErrorLogin("pUser is null");
	}
}


ReConnectThread::~ReConnectThread(){

}

ReConnectThread::ReConnectThread(AiClient* aiclient){
	m_aiClient = aiclient;
}

void ReConnectThread::OnThreadRun(){
	printf("run work\n");
	sleep(5);
	m_aiClient->http_handle = 0;
	m_aiClient->connect();
}
