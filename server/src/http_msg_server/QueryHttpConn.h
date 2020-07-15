/*
 * QueryHttpConn.h
 * Copyright (C) 2020 xiaominfc(武汉鸣鸾信息科技有限公司)
 * Email: xiaominfc@gmail.com
 * Distributed under terms of the MIT license.
 */

#ifndef QUERYHTTPCONN_H
#define QUERYHTTPCONN_H

#include "HttpConn.h"


class QueryHttpConn:public CHttpConn{
public:
	QueryHttpConn();
	~QueryHttpConn();
	void HandleWork();
};


#endif /* !QUERYHTTPCONN_H */
