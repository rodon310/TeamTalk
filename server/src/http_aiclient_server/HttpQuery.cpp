/*
 * HttpQuery.cpp
 *
 * Created on: 2013-10-22
 * Author: ziteng@mogujie.com
 */
#include <sstream>

#include "HttpQuery.h"
#include "HttpPdu.h"
#include "public_define.h"
#include "AttachData.h"
#include "IM.Message.pb.h"
#include "IM.Buddy.pb.h"
#include "IM.SwitchService.pb.h"
#include "IM.Group.pb.h"
#include "EncDec.h"
static uint32_t g_total_query = 0;
static uint32_t g_last_year = 0;
static uint32_t g_last_month = 0;
static uint32_t g_last_mday = 0;

CHttpQuery* CHttpQuery::m_query_instance = NULL;

hash_map<string, auth_struct*> g_hm_http_auth;
extern CAes *pAes;

static bool checkValueIsNullForJson(Json::Value &json_obj, string key)
{
	bool result = json_obj[key].isNull();
	if(result) {
		log("not value for key:%s", key.c_str());
	}
	return result;
}



static void paramErrorOut(CHttpConn* pHttpConn)
{
	char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
	pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
	pHttpConn->Close();
}


void http_query_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	(void)callback_data;
	(void)msg;
	(void)handle;
	(void)pParam;
	struct tm* tm;
	time_t currTime;

	time(&currTime);
	tm = localtime(&currTime);

	uint32_t year = tm->tm_year + 1900;
	uint32_t mon = tm->tm_mon + 1;
	uint32_t mday = tm->tm_mday;
	if (year != g_last_year || mon != g_last_month || mday != g_last_mday) {
		// a new day begin, clear the count
		log("a new day begin, g_total_query=%u ", g_total_query);
		g_total_query = 0;
		g_last_year = year;
		g_last_month = mon;
		g_last_mday = mday;
	}
}

CHttpQuery* CHttpQuery::GetInstance()
{
	if (!m_query_instance) {
		m_query_instance = new CHttpQuery();
		netlib_register_timer(http_query_timer_callback, NULL, 1000);
	}

	return m_query_instance;
}

void CHttpQuery::DispatchQuery(std::string& url, std::string& post_data, CHttpConn* pHttpConn)
{
	++g_total_query;
	log("DispatchQuery, url=%s, content=%s ", url.c_str(), post_data.c_str());

	Json::Reader reader;
	Json::Value value;
	Json::Value root;

	if (!reader.parse(post_data, value) ) {
		log("json parse failed, post_data=%s ", post_data.c_str());
		paramErrorOut(pHttpConn);
		return;
	}

	string strErrorMsg;
	string strAppKey;
	HTTP_ERROR_CODE nRet = HTTP_ERROR_SUCCESS;
	try
	{
		string strInterface(url.c_str() + strlen("/aichat/"));
		strAppKey = value["app_key"].asString();
		string strIp = pHttpConn->GetPeerIP();
		uint32_t nUserId = value["req_user_id"].asUInt();
		nRet = _CheckAuth(strAppKey, nUserId, strInterface, strIp);
	}
	catch ( std::runtime_error msg)
	{
		nRet = HTTP_ERROR_INTERFACE;
	}

	if(HTTP_ERROR_SUCCESS != nRet)
	{
		if(nRet < HTTP_ERROR_MAX)
		{
			root["error_code"] = nRet;
			root["error_msg"] = HTTP_ERROR_MSG[nRet];
		}
		else
		{
			root["error_code"] = -1;
			root["error_msg"] = "未知错误";
		}
		//string strResponse = root.toStyledString();
		char *response_data = PackSendResult(root["error_code"].asUInt(),root["error_msg"].asCString());
		pHttpConn->Send(response_data, strlen(response_data));
		return;
	}

	if(strcmp(url.c_str(), "/aichat/Login") == 0)
	{
		_Login(strAppKey, value, pHttpConn);	
	}else if(strcmp(url.c_str(), "/aichat/SendMsg") == 0)
	{
		_SendMessage(strAppKey, value, pHttpConn);	
	}else if(strcmp(url.c_str(), "/aichat/CallBack") == 0)
	{
		_TestCallBack(strAppKey, value, pHttpConn);	
	}
	else {
		log("url not support for post:%s",url.c_str());
		pHttpConn->Close();
		return;
	}
}


void CHttpQuery::DispatchGetQuery(std::string& url, CHttpConn* pHttpConn){
	
	if(strcmp(url.c_str(), "/aichat/Online") == 0)
	{
		_Online(pHttpConn);	
	}
	else {
		log("url not support for get:%s",url.c_str());
		pHttpConn->Close();
		return;
	}
}

void CHttpQuery::_Login(const string&strAppKey, Json::Value& post_json_obj, CHttpConn* pHttpConn){
	(void)strAppKey;
	if(checkValueIsNullForJson(post_json_obj,"account")
		||checkValueIsNullForJson(post_json_obj,"password")
		||checkValueIsNullForJson(post_json_obj,"callbackurl")
		||checkValueIsNullForJson(post_json_obj,"msgserverurl")
		){
		paramErrorOut(pHttpConn);
		return;
	}
	
	try{
		string account = post_json_obj["account"].asString();
		string password = post_json_obj["password"].asString();
		string msgserverurl = post_json_obj["msgserverurl"].asString();
		string callbackurl = post_json_obj["callbackurl"].asString();
		//AiClient *client = new AiClient(account,password,"http://im.xiaominfc.com:8080");
		AiClient *client = new AiClient(account,password,msgserverurl);
		client->doLogin(pHttpConn->GetConnHandle(), callbackurl);
	}catch(std::runtime_error msg)
	{
		log("parse json data failed.");
		paramErrorOut(pHttpConn);
	}
}

void CHttpQuery::_TestCallBack(const string&strAppKey, Json::Value& post_json_obj, CHttpConn* pHttpConn){
	(void)strAppKey;
	if(checkValueIsNullForJson(post_json_obj,"text")
		||checkValueIsNullForJson(post_json_obj,"fromId")
		||checkValueIsNullForJson(post_json_obj,"toId")
		||checkValueIsNullForJson(post_json_obj,"msgType")
		){
		paramErrorOut(pHttpConn);
		return;
	}

	try{
		string text = post_json_obj["text"].asString();
		uint32_t fromId = post_json_obj["fromId"].asUInt();
		uint32_t toId = post_json_obj["toId"].asUInt();
		uint32_t msgType = post_json_obj["msgType"].asUInt();
		log("test callback fromId:%d  toId:%d msgType:%d text:%s",fromId, toId, msgType, text.c_str());
		char *response_buf = PackSendResult(0,"test callback ok");
		pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
		pHttpConn->Close();
	}catch(std::runtime_error msg)
	{
		log("parse json data failed.");
		paramErrorOut(pHttpConn);
	}
}


void CHttpQuery::_SendMessage(const string&strAppKey, Json::Value& post_json_obj, CHttpConn* pHttpConn) {
	(void)strAppKey;
	if(checkValueIsNullForJson(post_json_obj,"senderId")
		||checkValueIsNullForJson(post_json_obj,"toId")
		||checkValueIsNullForJson(post_json_obj,"text")
		){
		paramErrorOut(pHttpConn);
		return;
	}

	try{
		uint32_t uid = post_json_obj["senderId"].asUInt();
		AiClient *aiClient = FindAiClientById(uid);
		if(aiClient) {
			uint32_t toId = post_json_obj["toId"].asUInt();
			string text;
			if(post_json_obj["text"].isString()) {
				text = post_json_obj["text"].asString();
			}else if(post_json_obj["text"].isObject()) {
				text = post_json_obj["text"].toStyledString();
			}else {
				paramErrorOut(pHttpConn);
				return;
			}
			aiClient->sendMsg(toId,IM::BaseDefine::MSG_TYPE_SINGLE_TEXT,text);
			log("sendmsg senderId:%d  toId:%d text:%s",uid, toId, text.c_str());
			char *response_buf = PackSendResult(0,"send msg ok");
			pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
			pHttpConn->Close();
			return;
		}else {
			char *response_buf = PackSendResult(1,"sender no login");
			pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
			pHttpConn->Close();
		}
	}catch(std::runtime_error msg)
	{
		log("parse json data failed.");
		paramErrorOut(pHttpConn);
	}


}



void CHttpQuery::_Online(CHttpConn* pHttpConn) {
	char *data = OnlineInfo();
	pHttpConn->Send(data,(uint32_t)strlen(data));
	pHttpConn->Close();
	free(data);
}

HTTP_ERROR_CODE CHttpQuery::_CheckAuth(const string& strAppKey, const uint32_t userId, const string& strInterface, const string& strIp)
{
	(void)strAppKey;
	(void)userId;
	(void)strInterface;
	(void)strIp;
	return HTTP_ERROR_SUCCESS;
}

HTTP_ERROR_CODE CHttpQuery::_CheckPermission(const string& strAppKey, uint8_t nType, const list<uint32_t>& lsToId, string strMsg)
{
	(void)strAppKey;
	(void)nType;
	(void)lsToId;
	strMsg.clear();
	return HTTP_ERROR_SUCCESS;
}

