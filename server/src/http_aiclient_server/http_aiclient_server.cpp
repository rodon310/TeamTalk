//
//  http_msg_server.cpp
//  http_msg_server
//
//  Created by jianqing.du on 13-9-29.
//  Copyright (c) 2013年 ziteng. All rights reserved.
//

#include "netlib.h"
#include "ConfigFileReader.h"
#include "version.h"
#include "ServInfo.h"
#include "HttpConn.h"
#include "HttpQuery.h"
#include "util.h"
#include "EncDec.h"
#include "AiClient.h"

CAes* pAes;

#define DEFAULT_CONCURRENT_DB_CONN_CNT  2

// for client connect in
void http_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if (msg == NETLIB_MSG_CONNECT)
	{
		CHttpConn* pConn = new CHttpConn();
		pConn->OnConnect(handle);
	}
	else
	{
		log("!!!error msg: %d ", msg);
	}
}


int main(int argc, char* argv[])
{
	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		printf("Server Version: HttpAiClientServer/%s\n", VERSION);
		printf("Server Build: %s %s\n", __DATE__, __TIME__);
		return 0;
	}
	
	signal(SIGPIPE, SIG_IGN);
	srand(time(NULL));
	
	log("HttpAiClientServer max files can open: %d ", getdtablesize());
	
	CConfigFileReader config_file("httpaiclientserver.conf");
	
	char* listen_ip = config_file.GetConfigName("ListenIP");
	char* str_listen_port = config_file.GetConfigName("ListenPort");
	



	char* str_aes_key = config_file.GetConfigName("aesKey");
	if (!str_aes_key || strlen(str_aes_key)!=32) {
		log("aes key is invalied");
		return -1;
	}
	pAes = new CAes(str_aes_key);

	if (!listen_ip || !str_listen_port) {
		log("config file miss, exit... ");
		return -1;
	}

	uint16_t listen_port = atoi(str_listen_port);
	
	int ret = netlib_init();
	
	if (ret == NETLIB_ERROR)
		return ret;
	
	CStrExplode listen_ip_list(listen_ip, ';');
	for (uint32_t i = 0; i < listen_ip_list.GetItemCnt(); i++) {
		ret = netlib_listen(listen_ip_list.GetItem(i), listen_port, http_callback, NULL);
		if (ret == NETLIB_ERROR)
			return ret;
	}

	string filepath("last_users.txt");

	InitLastLoginData(filepath);

	printf("server start listen on: %s:%d\n", listen_ip, listen_port);
	
	init_http_conn();

	printf("now enter the event loop...\n");
	
	writePid();

	netlib_eventloop();
	
	return 0;
}
