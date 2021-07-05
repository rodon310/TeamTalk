/*
 * login_server.cpp
 *
 *  Created on: 2013-6-21
 *      Author: ziteng@mogujie.com
 */

#include "LoginConn.h"
#include "netlib.h"
#include "ConfigFileReader.h"
#include "version.h"
#include "WorkHttpConn.h"
#include "ipparser.h"
#include "EventSocket.h"

IpParser* pIpParser = NULL;
string strMsfsUrl;
string strDiscovery;//发现获取地址
string strBaseUrl;





class CONNTypeEventFactory: public EventFactoryInterface{
	public:
		CONNTypeEventFactory(int _type):conn_type(_type){};
		virtual CEventInterface* createEvent(CBaseSocket* socket){
			CLoginConn* conn = new CLoginConn();
			conn->OnConnect(socket,conn_type);
			return conn;
		}		
	private:
		int conn_type;

};


int main(int argc, char* argv[])
{
	PRINTSERVERVERSION()

	signal(SIGPIPE, SIG_IGN);

	CConfigFileReader config_file("loginserver.conf");

	char* client_listen_ip = config_file.GetConfigName("ClientListenIP");
	char* str_client_port = config_file.GetConfigName("ClientPort");
	char* http_listen_ip = config_file.GetConfigName("HttpListenIP");
	char* str_http_port = config_file.GetConfigName("HttpPort");
	char* msg_server_listen_ip = config_file.GetConfigName("MsgServerListenIP");
	char* str_msg_server_port = config_file.GetConfigName("MsgServerPort");
	char* str_msfs_url = config_file.GetConfigName("msfs");
	char* str_discovery = config_file.GetConfigName("discovery");
	char* str_api = config_file.GetConfigName("BaseUrl");
	if (!msg_server_listen_ip || !str_msg_server_port || !http_listen_ip
			|| !str_http_port || !str_msfs_url || !str_discovery) {
		log("config item missing, exit... ");
		return -1;
	}

	uint16_t client_port = atoi(str_client_port);
	uint16_t msg_server_port = atoi(str_msg_server_port);
	uint16_t http_port = atoi(str_http_port);
	strMsfsUrl = str_msfs_url;
	strDiscovery = str_discovery;
	strBaseUrl = str_api;

	pIpParser = new IpParser();

	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;
	CStrExplode client_listen_ip_list(client_listen_ip, ';');
	for (uint32_t i = 0; i < client_listen_ip_list.GetItemCnt(); i++) {
		ret = tcp_server_listen(client_listen_ip_list.GetItem(i), client_port, new CONNTypeEventFactory(LOGIN_CONN_TYPE_CLIENT));
		if (ret == NETLIB_ERROR)
			return ret;
	}

	CStrExplode msg_server_listen_ip_list(msg_server_listen_ip, ';');
	for (uint32_t i = 0; i < msg_server_listen_ip_list.GetItemCnt(); i++) {
		ret = tcp_server_listen(msg_server_listen_ip_list.GetItem(i), msg_server_port, new CONNTypeEventFactory(LOGIN_CONN_TYPE_MSG_SERV));
		if (ret == NETLIB_ERROR)
			return ret;
	}

	CStrExplode http_listen_ip_list(http_listen_ip, ';');
	for (uint32_t i = 0; i < http_listen_ip_list.GetItemCnt(); i++) {
		ret = tcp_server_listen(http_listen_ip_list.GetItem(i), http_port,new IMConnEventDefaultFactory<WorkHttpConn>());
		if (ret == NETLIB_ERROR)
			return ret;
	}


	printf("server start listen on:\nFor client %s:%d\nFor MsgServer: %s:%d\nFor http:%s:%d\n",
			client_listen_ip, client_port, msg_server_listen_ip, msg_server_port, http_listen_ip, http_port);
	init_login_conn();
	init_http_conn();

	printf("now enter the event loop...\n");

	writePid();

	netlib_eventloop();

	return 0;
}
