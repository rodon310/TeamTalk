/*
 * imconn.cpp
 *
 * Created on: 2013-6-5
 * Author: ziteng@mogujie.com
 */

#include "imconn.h"

//static uint64_t g_send_pkt_cnt = 0;		// 发送数据包总数
//static uint64_t g_recv_pkt_cnt = 0;		// 接收数据包总数

static CImConn* FindImConn(ConnMap_t* imconn_map, net_handle_t handle)
{
    CImConn* pConn = NULL;
    ConnMap_t::iterator iter = imconn_map->find(handle);
    if (iter != imconn_map->end())
    {
        pConn = iter->second;
        pConn->AddRef();
    }

    return pConn;
}

void imconn_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	NOTUSED_ARG(handle);
	NOTUSED_ARG(pParam);

	if (!callback_data)
		return;

	ConnMap_t* conn_map = (ConnMap_t*)callback_data;
	CImConn* pConn = FindImConn(conn_map, handle);
	if (!pConn)
		return;

    //log("msg=%d, handle=%d ", msg, handle);

	switch (msg)
	{
		case NETLIB_MSG_CONFIRM:
			pConn->OnConfirm();
			break;
		case NETLIB_MSG_READ:
			pConn->OnRead();
			break;
		case NETLIB_MSG_WRITE:
			pConn->OnWrite();
			break;
		case NETLIB_MSG_CLOSE:
			pConn->OnClose();
			break;
		default:
			log("!!!imconn_callback error msg: %d ", msg);
			break;
	}
	pConn->ReleaseRef();
}

//////////////////////////
CImConn::CImConn()
{
	m_socket = NULL;
	m_busy = false;
	m_handle = NETLIB_INVALID_HANDLE;
	m_recv_bytes = 0;
	m_last_send_tick = m_last_recv_tick = get_tick_count();
}

CImConn::~CImConn()
{
	//log("CImConn::~CImConn, handle=%d ", m_handle);
}

int CImConn::Send(void* data, int len)
{
	m_last_send_tick = get_tick_count();
	//	++g_send_pkt_cnt;
	if (m_busy)
	{
		m_out_buf.Write(data, len);
		return len;
	}

	int offset = 0;
	int remain = len;
	while (remain > 0) {
		int send_size = remain;
		if (send_size > NETLIB_MAX_SOCKET_BUF_SIZE) {
			send_size = NETLIB_MAX_SOCKET_BUF_SIZE;
		}
		int ret = m_socket->Recv((char*)data + offset , send_size);
		if (ret <= 0) {
			ret = 0;
			break;
		}

		offset += ret;
		remain -= ret;
    }

	if (remain > 0)
	{
		m_out_buf.Write((char*)data + offset, remain);
		m_busy = true;
		log("send busy, remain=%d ", m_out_buf.GetWriteOffset());
	}
	else
	{
		OnWriteCompelete();
    }
	return len;
}

void CImConn::OnRead()
{
	if(m_socket == NULL || (!m_socket->Readable())){
		OnClose();
		return;
	}

	ReadData();
	HandleData();
}


void CImConn::ReadData(){
	for (;;)
	{
		uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
		if (free_buf_len < READ_BUF_SIZE)
			m_in_buf.Extend(READ_BUF_SIZE);

		int ret = m_socket->Recv(m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(), READ_BUF_SIZE);
		if (ret <= 0)
			break;
		m_recv_bytes += ret;
		m_in_buf.IncWriteOffset(ret);
		m_last_recv_tick = get_tick_count();
	}
}



void CImConn::OnWrite()
{
	if(m_socket !=NULL){
		int state = m_socket->CheckWriteState();
		switch (state)
		{
		case NETLIB_MSG_CLOSE:
			OnClose();
			break;
		case NETLIB_MSG_CONFIRM:
			OnConfirm();
			break;
		case NETLIB_MSG_WRITE:
			WriteData();
			break;	
		default:
			break;
		}
	}
}

void CImConn::WriteData(){
	if (!m_busy)
		return;

	while (m_out_buf.GetWriteOffset() > 0) {
		int send_size = m_out_buf.GetWriteOffset();
		if (send_size > NETLIB_MAX_SOCKET_BUF_SIZE) {
			send_size = NETLIB_MAX_SOCKET_BUF_SIZE;
		}

		int ret = m_socket->Send(m_out_buf.GetBuffer(), send_size);
		if (ret <= 0) {
			ret = 0;
			break;
		}
		m_out_buf.Read(NULL, ret);
	}

	if (m_out_buf.GetWriteOffset() == 0) {
		m_busy = false;
	}
	log("onWrite, remain=%d ", m_out_buf.GetWriteOffset());
}

void CImConn::Close(){
	if(m_socket != NULL){
		m_socket->Close();
		m_socket->ReleaseRef();
		m_socket = NULL;
	}
	m_handle = NETLIB_INVALID_HANDLE;
}





