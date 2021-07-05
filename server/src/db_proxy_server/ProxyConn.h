/*
 * @File ProxyConn.h
 * @Author: xiaominfc
 * @Date: 2019-08-29 11:30:07
 * @Description: listen for msgserver connection
 */


#ifndef PROXYCONN_H_
#define PROXYCONN_H_

#include <curl/curl.h>
#include "../base/util.h"
#include "ImPduConn.h"

typedef struct {
	uint32_t	conn_uuid;
	CImPdu*		pPdu;
} ResponsePdu_t;

class CProxyConn : public CImPduConn {
public:
	CProxyConn();
	virtual ~CProxyConn();
	virtual void Close();
	virtual void OnConnect(CBaseSocket* socket);
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	virtual void HandleData();
	virtual void HandlePdu(CImPdu* pPdu);

	static void AddResponsePdu(uint32_t conn_uuid, CImPdu* pPdu);	// 工作线程调用
	static void SendResponsePduList();	// 主线程调用
private:
	// 由于处理请求和发送回复在两个线程，socket的handle可能重用，所以需要用一个一直增加的uuid来表示一个连接
	static uint32_t	s_uuid_alloctor;
	uint32_t		m_uuid;

	static CLock			s_list_lock;
	static list<ResponsePdu_t*>	s_response_pdu_list;	// 主线程发送回复消息
};

int init_proxy_conn(uint32_t thread_num);
CProxyConn* get_proxy_conn_by_uuid(uint32_t uuid);

#endif /* PROXYCONN_H_ */
