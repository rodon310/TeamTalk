/*
 * EventSocket.cpp
 * Copyright (C) 2021 xiaominfc <xiaominfc@126.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "EventSocket.h"
#include "EventDispatch.h"

int tcp_server_listen(
	const char*	server_ip, 
	uint16_t	port,
	EventFactoryInterface* factory){
	EventSocket *server = new EventSocket(factory);
	if (!server)
		return NETLIB_ERROR;
	int ret =  server->Listen(server_ip,port,NULL,NULL);
	if (ret == NETLIB_ERROR)
		delete server;
	return ret;
}


int tcp_server_unix_listen(
	const char*	unix_path, 
	EventFactoryInterface* factory){
	EventSocket *server = new EventSocket(factory);
	if (!server)
		return NETLIB_ERROR;
	int ret =  server->UnixListen(unix_path,NULL,NULL);
	if (ret == NETLIB_ERROR)
		delete server;
	return ret;
}

int tcp_client_conn(
	const char*	server_ip, 
	uint16_t	port,
	EventFactoryInterface* factory){
	EventSocket *conn = new EventSocket(factory);
	if (!conn)
		return NETLIB_INVALID_HANDLE;

	net_handle_t handle = conn->Connect(server_ip, port, NULL, NULL);
	if (handle == NETLIB_INVALID_HANDLE)
		delete conn;
	return handle;
}

int tcp_client_unix_conn(
	const char*	unix_path, 
	EventFactoryInterface* factory){
	EventSocket *conn = new EventSocket(factory);
	if (!conn)
		return NETLIB_INVALID_HANDLE;

	net_handle_t handle = conn->UnixConnect(unix_path, NULL, NULL);
	if (handle == NETLIB_INVALID_HANDLE)
		delete conn;
	return handle;		
}


void EventSocket::BindEvent(CBaseSocket *socket,uint8_t socket_event){
	if(factory!=NULL){
		CEventInterface *event = factory->createEvent(socket);
		if(event != NULL){
			int fd = socket->GetSocket();
#if (defined _WIN32)
			AddEvent(fd,event);
			CEventDispatch::Instance()->AddEvent(fd,socket_event);
#else
#if (defined __APPLE__)
			AddEvent(fd,event);
#endif
			CEventDispatch::Instance()->AddEventInterface(fd,socket_event,event);
#endif
		}
	}else {
		CBaseSocket::BindEvent(socket,socket_event);
	}
}
