/*
 * HttpConn.cpp
 *
 *  Created on: 2013-9-29
 *	  Author: ziteng@mogujie.com
 */

#include "HttpConn.h"
#include "HttpParserWrapper.h"
#include "HttpQuery.h"

static HttpConnMap_t g_http_conn_map;

// conn_handle 从0开始递增，可以防止因socket handle重用引起的一些冲突
static uint32_t g_conn_handle_generator = 0;

CHttpConn* FindHttpConnByHandle(uint32_t conn_handle)
{
	CHttpConn* pConn = NULL;
	HttpConnMap_t::iterator it = g_http_conn_map.find(conn_handle);
	if (it != g_http_conn_map.end()) {
		pConn = it->second;
	}

	return pConn;
}


void http_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	(void)callback_data;
	(void)msg;
	(void)handle;
	(void)pParam;
	CHttpConn* pConn = NULL;
	HttpConnMap_t::iterator it, it_old;
	uint64_t cur_time = get_tick_count();

	for (it = g_http_conn_map.begin(); it != g_http_conn_map.end(); ) {
		it_old = it;
		it++;

		pConn = it_old->second;
		pConn->OnTimer(cur_time);
	}
}

void init_http_conn()
{
	netlib_register_timer(http_conn_timer_callback, NULL, 1000);
}

//////////////////////////
CHttpConn::CHttpConn()
{
	m_state = CONN_STATE_IDLE;
	m_conn_handle = ++g_conn_handle_generator;
	if (m_conn_handle == 0) {
		m_conn_handle = ++g_conn_handle_generator;
	}
}

CHttpConn::~CHttpConn()
{
	//log("~CHttpConn, handle=%u ", m_conn_handle);
}

void CHttpConn::Close()
{
	CImConn::Close();
	if (m_state != CONN_STATE_CLOSED) {
		m_state = CONN_STATE_CLOSED;
		g_http_conn_map.erase(m_conn_handle);
		ReleaseRef();
	}
}

void CHttpConn::OnConnect(CBaseSocket* socket)
{
	CImConn::OnConnect(socket);
	m_state = CONN_STATE_CONNECTED;
	g_http_conn_map.insert(make_pair(m_conn_handle, this));
}



void CHttpConn::HandleData(){
	printf("HandeData\n");
	HandleWork();
}

void CHttpConn::HandleWork(){
	// 每次请求对应一个HTTP连接，所以读完数据后，不用在同一个连接里面准备读取下个请求
	char* in_buf = (char*)m_in_buf.GetBuffer();
	uint32_t buf_len = m_in_buf.GetWriteOffset();
	in_buf[buf_len] = '\0';
	log("OnRead, buf_len=%u, conn_handle=%u\n", buf_len, m_conn_handle); // for debug
}



void CHttpConn::OnTimer(uint64_t curr_tick)
{
	if (curr_tick > m_last_recv_tick + HTTP_CONN_TIMEOUT) {
		log("HttpConn timeout, handle=%d ", m_conn_handle);
		Close();
	}
}

void CHttpConn::OnWriteCompelete()
{
	Close();
}

