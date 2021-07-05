/*
 * HttpConn.h
 *
 *  Created on: 2013-9-29
 *	  Author: ziteng
 */

#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

#include "netlib.h"
#include "util.h"
#include "HttpParserWrapper.h"
#include "imconn.h"

#define HTTP_CONN_TIMEOUT			60000

#define READ_BUF_SIZE	2048

enum {
	CONN_STATE_IDLE,
	CONN_STATE_CONNECTED,
	CONN_STATE_OPEN,
	CONN_STATE_CLOSED,
};

class CHttpConn : public CImConn
{
public:
	CHttpConn();
	virtual ~CHttpConn();

	uint32_t GetConnHandle() { return m_conn_handle; }
	char* GetPeerIP() { return (char*)m_peer_ip.c_str(); }
	virtual void HandleData();
	virtual void OnConnect(CBaseSocket* socket);
	virtual void Close();
	virtual void OnTimer(uint64_t curr_tick);
	virtual void HandleWork();
	virtual void OnWriteCompelete();

protected:
	uint32_t		m_conn_handle;
	uint32_t		m_state;
	CHttpParserWrapper m_HttpParser;
};

typedef hash_map<uint32_t, CHttpConn*> HttpConnMap_t;

CHttpConn* FindHttpConnByHandle(uint32_t handle);
void init_http_conn();

#endif /* IMCONN_H_ */
