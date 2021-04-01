/*
 * client_connect_impl.cpp
 * Copyright (C) 2020 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "client_connect_impl.h"

void msg_serv_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	(void)callback_data;
	(void)pParam;
	if (msg == NETLIB_MSG_CONNECT)
	{
		CMsgConn* pConn = new CMsgConn();
		if(pParam){
			pConn->OnConnect(handle,pParam);
		}else {
			pConn->OnConnect(handle);
		}
	}
	else
	{
		log("!!!error msg: %d ", msg);
	}
}


