/*
 * QueryHttpConn.cpp
 * Copyright (C) 2020 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "QueryHttpConn.h"
#include "HttpParserWrapper.h"
#include "HttpQuery.h"

QueryHttpConn::QueryHttpConn():CHttpConn(){

}

QueryHttpConn::~QueryHttpConn(){

}

void QueryHttpConn::HandleWork(){
// 每次请求对应一个HTTP连接，所以读完数据后，不用在同一个连接里面准备读取下个请求
	//CHttpConn::HandleWork();
	char* in_buf = (char*)m_in_buf.GetBuffer();
	uint32_t buf_len = m_in_buf.GetWriteOffset();
	in_buf[buf_len] = '\0';

	//log("OnRead, buf_len=%u, conn_handle=%u\n", buf_len, m_conn_handle); // for debug

	m_HttpParser.ParseHttpContent(in_buf, buf_len);

	if (m_HttpParser.IsReadAll()) {
		string url =  m_HttpParser.GetUrl();
		if (strncmp(url.c_str(), "/query/", 7) == 0) {
			string content = m_HttpParser.GetBodyContent();
			CHttpQuery* pQueryInstance = CHttpQuery::GetInstance();
			pQueryInstance->DispatchQuery(url, content, this);
		} else {
			log("url unknown, url=%s ", url.c_str());
			Close();
		}
	}	
}
