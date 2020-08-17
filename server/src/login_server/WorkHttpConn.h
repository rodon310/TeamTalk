/*
 * WorkHttpConn.h
 * Copyright (C) 2020 xiaominfc(武汉鸣鸾信息科技有限公司)
 * Email: xiaominfc@gmail.com
 * Distributed under terms of the MIT license.
 */

#ifndef WORKHTTPCONN_H
#define WORKHTTPCONN_H

#include "HttpConn.h"


#define READ_BUF_SIZE		2048
#define HTTP_RESPONSE_HTML	"HTTP/1.1 200 OK\r\n"\
							"Connection:close\r\n"\
							"Content-Length:%d\r\n"\
							"Content-Type:text/html;charset=utf-8\r\n\r\n%s"


#define HTTP_RESPONSE_JSON	"HTTP/1.1 200 OK\r\n"\
							"Connection:close\r\n"\
							"Content-Length:%d\r\n"\
							"Content-Type:application/json;charset=utf-8\r\n\r\n%s"

#define HTTP_RESPONSE_HTML_MAX      4096


class WorkHttpConn:public CHttpConn{
public:
	WorkHttpConn();
	~WorkHttpConn();
	virtual void HandleWork();

private:
	void _HandleMsgServRequest(string& url, string& post_data);
	void _HandleStatusReques(string& url, string& post_data);
};



#endif /* !WORKHTTPCONN_H */
