/*
 * WorkHttpConn.cpp
 * Copyright (C) 2020 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "WorkHttpConn.h"
#include "json/json.h"
#include "LoginConn.h"
#include "HttpParserWrapper.h"
#include "ipparser.h"
#include "ThreadPool.h"

extern map<uint32_t, msg_serv_info_t*>  g_msg_serv_info;

extern IpParser* pIpParser;
extern string strMsfsUrl;
extern string strDiscovery;
extern string strBaseUrl;

WorkHttpConn::WorkHttpConn():CHttpConn(){

}

WorkHttpConn::~WorkHttpConn(){

}

void WorkHttpConn::HandleWork(){
// 每次请求对应一个HTTP连接，所以读完数据后，不用在同一个连接里面准备读取下个请求
	char* in_buf = (char*)m_in_buf.GetBuffer();
	uint32_t buf_len = m_in_buf.GetWriteOffset();
	in_buf[buf_len] = '\0';

	// 如果buf_len 过长可能是受到攻击，则断开连接
	// 正常的url最大长度为2048，我们接受的所有数据长度不得大于1K
	if(buf_len > 1024)
	{
		log("get too much data:%s ", in_buf);
		Close();
		return;
	}

	//log("OnRead, buf_len=%u, conn_handle=%u\n", buf_len, m_conn_handle); // for debug


	m_HttpParser.ParseHttpContent(in_buf, buf_len);

	if (m_HttpParser.IsReadAll()) {
		string url =  m_HttpParser.GetUrl();
		if (strncmp(url.c_str(), "/msg_server", 11) == 0) {
			string content = m_HttpParser.GetBodyContent();
			_HandleMsgServRequest(url, content);
		}else if(strncmp(url.c_str(), "/status", 7 ) == 0) {
			string content = m_HttpParser.GetBodyContent();
			_HandleStatusReques(url, content);
		}
		else {
			log("url unknown, url=%s ", url.c_str());
			Close();
		}
	}
}

void WorkHttpConn::_HandleStatusReques(string& url, string& post_data)
{
	Json::Value value;
	msg_serv_info_t* pMsgServInfo;
	map<uint32_t, msg_serv_info_t*>::iterator it;
	Json::Value  server_arr_value(Json::arrayValue);
	int index = 0;
	for (it = g_msg_serv_info.begin() ; it != g_msg_serv_info.end(); it++) {
		pMsgServInfo = it->second;
		Json::Value server_value;
		server_value["port"] = pMsgServInfo->port;
		server_value["ip_addr1"] = pMsgServInfo->ip_addr1;
		server_value["ip_addr2"] = pMsgServInfo->ip_addr2;
		server_value["hostname"] = pMsgServInfo->hostname;
		server_value["online_count"] = pMsgServInfo->cur_conn_cnt;
		server_arr_value[index] = server_value;
		index ++;
	}
	value["msg_servers"] = server_arr_value;
	string strContent = value.toStyledString();
	char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
	uint32_t nLen = strContent.length();
	snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, nLen, strContent.c_str());
	Send((void*)szContent, strlen(szContent));
	delete [] szContent;
	return;
}

// Add By Lanhu 2014-12-19 通过登陆IP来优选电信还是联通IP
void WorkHttpConn::_HandleMsgServRequest(string& url, string& post_data)
{
	msg_serv_info_t* pMsgServInfo;
	uint32_t min_user_cnt = (uint32_t)-1;
	map<uint32_t, msg_serv_info_t*>::iterator it_min_conn = g_msg_serv_info.end();
	map<uint32_t, msg_serv_info_t*>::iterator it;
	if(g_msg_serv_info.size() <= 0)
	{
		Json::Value value;
		value["code"] = 1;
		value["msg"] = "没有msg_server";
		string strContent = value.toStyledString();
		char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
		snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, strContent.length(), strContent.c_str());
		Send((void*)szContent, strlen(szContent));
		delete [] szContent;
		return ;
	}

	for (it = g_msg_serv_info.begin() ; it != g_msg_serv_info.end(); it++) {
		pMsgServInfo = it->second;
		if ( (pMsgServInfo->cur_conn_cnt < pMsgServInfo->max_conn_cnt) &&
				(pMsgServInfo->cur_conn_cnt < min_user_cnt)) {
			it_min_conn = it;
			min_user_cnt = pMsgServInfo->cur_conn_cnt;
		}
	}

	if (it_min_conn == g_msg_serv_info.end()) {
		log("All TCP MsgServer are full ");
		Json::Value value;
		value["code"] = 2;
		value["msg"] = "负载过高";
		string strContent = value.toStyledString();
		char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
		snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, strContent.length(), strContent.c_str());
		Send((void*)szContent, strlen(szContent));
		delete [] szContent;
		return;
	} else {
		Json::Value value;
		value["code"] = 0;
		value["msg"] = "";
		if(pIpParser->isTelcome(GetPeerIP()))
		{
			value["priorIP"] = string(it_min_conn->second->ip_addr1);
			value["backupIP"] = string(it_min_conn->second->ip_addr2);
			value["msfsPrior"] = strMsfsUrl;
			value["msfsBackup"] = strMsfsUrl;
		}
		else
		{
			value["priorIP"] = string(it_min_conn->second->ip_addr2);
			value["backupIP"] = string(it_min_conn->second->ip_addr1);
			value["msfsPrior"] = strMsfsUrl;
			value["msfsBackup"] = strMsfsUrl;
		}
		value["discovery"] = strDiscovery;
		value["baseUrl"] = strBaseUrl;
		value["port"] = int2string(it_min_conn->second->port);
		string strContent = value.toStyledString();
		char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
		uint32_t nLen = strContent.length();
		snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, nLen, strContent.c_str());
		Send((void*)szContent, strlen(szContent));
		delete [] szContent;
		return;
	}
}


