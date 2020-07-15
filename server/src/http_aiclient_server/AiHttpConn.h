/*
 * AiHttpConn.h
 * Copyright (C) 2020 xiaominfc(武汉鸣鸾信息科技有限公司)
 * Email: xiaominfc@gmail.com
 * Distributed under terms of the MIT license.
 */

#ifndef AIHTTPCONN_H
#define AIHTTPCONN_H

#include "HttpConn.h"

class AiHttpConn : public CHttpConn{
public:
	AiHttpConn();
	virtual ~AiHttpConn();
	virtual void HandleWork();

};




#endif /* !AIHTTPCONN_H */
