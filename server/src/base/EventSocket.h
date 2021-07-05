/*
 * EventSocket.h
 * Copyright (C) 2021 xiaominfc
 * Email: xiaominfc@126.com
 * Distributed under terms of the MIT license.
 */

#ifndef EVENTSOCKET_H
#define EVENTSOCKET_H

#include "BaseSocket.h"
#include "EventFactoryInterface.h"
#include "imconn.h"

//T must base CImConn
template <class T> class IMConnEventDefaultFactory: public EventFactoryInterface{
	public:
		CEventInterface* createEvent(CBaseSocket *socket){
			T* t = new T();
			t->OnConnect(socket);
			return t;
		}
};


class IMConnEventConnFactory: public EventFactoryInterface{
	public:
		IMConnEventConnFactory(CImConn* t){m_instance = t;};
		CEventInterface* createEvent(CBaseSocket *socket){
			if(m_instance != NULL){
				m_instance->OnConnect(socket);
			}
			return m_instance;
		}
	private:
		CImConn* m_instance;
};


class EventSocket:public CBaseSocket{
	public:
		EventSocket(EventFactoryInterface *_factory):factory(_factory){};
		~EventSocket(){};

		virtual void BindEvent(CBaseSocket *socket,uint8_t socket_event);
	private:
		EventFactoryInterface *factory;
};
int tcp_server_listen(
	const char*	server_ip, 
	uint16_t	port,
	EventFactoryInterface* factory);


int tcp_server_unix_listen(
	const char*	unix_path, 
	EventFactoryInterface* factory);

int tcp_client_conn(
	const char*	server_ip, 
	uint16_t	port,
	CImConn* conn);

int tcp_client_unix_conn(
	const char*	unix_path, 
	CImConn* conn);

#endif /* !EVENTSOCKET_H */
