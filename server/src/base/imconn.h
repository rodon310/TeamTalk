/*
 * @Author: xiaominfc
 * @Date: 2020-08-24 09:23:35
 * @Description: 
 */

#ifndef IMCONN_H_
#define IMCONN_H_

#include "netlib.h"
#include "util.h"
#include "Task.h"
#include "EventInterface.h"
#include "BaseSocket.h"

#define SERVER_HEARTBEAT_INTERVAL	5000
#define SERVER_TIMEOUT				30000
#define CLIENT_HEARTBEAT_INTERVAL	30000
#define CLIENT_TIMEOUT				120000
#define MOBILE_CLIENT_TIMEOUT       60000 * 5
#define READ_BUF_SIZE	2048

class CImConn : public CEventInterface
{
public:
	CImConn();
	virtual ~CImConn();

	bool IsBusy() { return m_busy; }
	
	virtual  int Send(void* data, int len);
	virtual void OnConnect(CBaseSocket *socket) {m_handle = socket->GetSocket();socket->AddRef(); m_socket = socket;}
	virtual void OnConfirm() {}
	virtual void OnRead();
	virtual void OnWrite();
	virtual void OnClose() {Close();};
	virtual void Close();
	virtual void OnTimer(uint64_t curr_tick){(void)curr_tick;};
	virtual void OnWriteCompelete() {};
	
	virtual void WriteData();
	virtual void ReadData();	// read all readable data
	virtual void HandleData(){};	// handle data for work
	

protected:
	CBaseSocket*	m_socket;
	net_handle_t	m_handle;
	bool			m_busy;
	string			m_peer_ip;
	uint16_t		m_peer_port;
	CSimpleBuffer	m_in_buf;
	CSimpleBuffer	m_out_buf;

	uint32_t		m_recv_bytes;
	uint64_t		m_last_send_tick;
	uint64_t		m_last_recv_tick;
};


typedef hash_map<net_handle_t, CImConn*> ConnMap_t;
typedef hash_map<uint32_t, CImConn*> UserMap_t;

void imconn_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam);
void ReadPolicyFile();

#endif /* IMCONN_H_ */
