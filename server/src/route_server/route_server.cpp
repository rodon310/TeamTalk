/*
 * @Author: xiaominfc
 * @Date: 2020-09-02 17:10:06
 * @Description: main run for routeserver
 */

#include "RouteConn.h"
#include "netlib.h"
#include "ConfigFileReader.h"
#include "version.h"
#include "EventSocket.h"


int main(int argc, char* argv[])
{
	PRINTSERVERVERSION()

	signal(SIGPIPE, SIG_IGN);
	srand(time(NULL));

	CConfigFileReader config_file("routeserver.conf");

	char* listen_ip = config_file.GetConfigName("ListenIP");
	char* str_listen_msg_port = config_file.GetConfigName("ListenMsgPort");

	if (!listen_ip || !str_listen_msg_port) {
		log("config item missing, exit... ");
		return -1;
	}

	uint16_t listen_msg_port = atoi(str_listen_msg_port);

	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;

	CStrExplode listen_ip_list(listen_ip, ';');
	for (uint32_t i = 0; i < listen_ip_list.GetItemCnt(); i++) {
		//ret = netlib_listen(listen_ip_list.GetItem(i), listen_msg_port, route_serv_callback, NULL);
		ret = tcp_server_listen(listen_ip_list.GetItem(i), listen_msg_port,new IMConnEventDefaultFactory<CRouteConn>());
		if (ret == NETLIB_ERROR)
			return ret;
	}

	printf("server start listen on: %s:%d\n", listen_ip,  listen_msg_port);

	init_routeconn_timer_callback();

	printf("now enter the event loop...\n");

    writePid();
	netlib_eventloop();

	return 0;
}

