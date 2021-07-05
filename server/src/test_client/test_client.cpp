/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：test_client.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮	箱：bluefoxah@gmail.com
 *   创建日期：2014年12月30日
 *   描	述：
 *
 ================================================================*/

#include <vector>
#include <iostream>
#include <string>
#include "ClientConn.h"
#include "netlib.h"
#include "TokenValidator.h"
#include "Thread.h"
#include "IM.BaseDefine.pb.h"
#include "IM.Buddy.pb.h"
#include "playsound.h"
#include "Common.h"
#include "Client.h"
#include "EncDec.h"
using namespace std;

#define MAX_LINE_LEN	1024
string g_login_domain = "http://xiaominfc.com:8080";
string g_cmd_string[10];
int g_cmd_num;
CClient* g_pClient = NULL;
CAes* pAes;

void split_cmd(char* buf)
{
	int len = strlen(buf);
	string element;

	g_cmd_num = 0;
	for (int i = 0; i < len; i++) {
		if (buf[i] == ' ' || buf[i] == '\t') {
			if (!element.empty()) {
				g_cmd_string[g_cmd_num++] = element;
				element.clear();
			}
		} else {
			element += buf[i];
		}
	}

	// put the last one
	if (!element.empty()) {
		g_cmd_string[g_cmd_num++] = element;
	}
}

void print_help()
{
	printf("Usage:\n");
	printf("login user_name user_pass\n");
	/*
	printf("connect serv_ip serv_port user_name user_pass\n");
	printf("getuserinfo\n");
	printf("send toId msg\n");
	printf("unreadcnt\n");
	 */
	printf("close\n");
	printf("quit\n");
}


void sendTextMsg(const string& id, const string& msg)
{
	if(g_pClient){
		g_pClient->sendMsg(atoi(id.c_str()),IM::BaseDefine::MSG_TYPE_SINGLE_TEXT,msg);
	}
}

void doLogin(const string& strName, const string& strPass)
{
	try
	{
		g_pClient = new CClient(strName, strPass, g_login_domain);
	}
	catch(...)
	{
		printf("get error while alloc memory\n");
		PROMPTION;
		return;
	}
	g_pClient->connect();
}

void getOfflineFile() {

	printf("get offlinefile\n");
	if(g_pClient) {
		g_pClient->getOfflineFile();
	}
}


void exec_cmd()
{
	if (g_cmd_num == 0) {
		return;
	}

	printf("cmd:%s\n",g_cmd_string[0].c_str());
	
	if(g_cmd_string[0] == "login")
	{
		if(g_cmd_num == 3)
		{
			doLogin(g_cmd_string[1], g_cmd_string[2]);
		}
		else
		{
			print_help();
		}
	}else if(strcmp(g_cmd_string[0].c_str(), "send") == 0){
		if(g_cmd_num == 3) {
			sendTextMsg(g_cmd_string[1], g_cmd_string[2]);
		}else {
			print_help();
		}
	}else if(strcmp(g_cmd_string[0].c_str(), "offlinefile") == 0) {
		getOfflineFile();
	}
	else if (strcmp(g_cmd_string[0].c_str(), "close") == 0) {
		g_pClient->close();
	}
	else if (strcmp(g_cmd_string[0].c_str(), "quit") == 0) {
		exit(0);

	}
	else if(strcmp(g_cmd_string[0].c_str(), "list") == 0)
	{
		printf("+---------------------+\n");
		printf("|		用户名		|\n");
		printf("+---------------------+\n");
		CMapNick2User_t mapUser = g_pClient->getNick2UserMap();
		auto it = mapUser.begin();
		for(;it!=mapUser.end();++it)
		{
			uint32_t nLen = 21 - it->first.length();
			printf("|");
			for(uint32_t i=0; i<nLen/2; ++i)
			{
				printf(" ");
			}
			printf("%s:%d", it->first.c_str(),((IM::BaseDefine::UserInfo*)it->second)->user_id());
			for(uint32_t i=0; i<nLen/2; ++i)
			{
				printf(" ");
			}
			printf("|\n");
			printf("+---------------------+\n");
		}
	}
	else {
		print_help();
	}
}


class CmdThread : public CThread
{
public:
	void OnThreadRun()
	{
		while (true)
		{
			fprintf(stderr, "%s", PROMPT);	// print to error will not buffer the printed message

			if (fgets(m_buf, MAX_LINE_LEN - 1, stdin) == NULL)
			{
				if(errno == 0)  {
					exit(0);
					break;
				}
				fprintf(stderr, "fgets failed: %d\n", errno);
				continue;
			}

			m_buf[strlen(m_buf) - 1] = '\0';	// remove newline character

			split_cmd(m_buf);

			exec_cmd();
		}
	}
private:
	char	m_buf[MAX_LINE_LEN];
};

CmdThread g_cmd_thread;

int main(int argc, char* argv[])
{
//	play("message.wav");
	pAes = new CAes("12345678901234567890123456789012");
	
	if(argc == 3){
		doLogin(argv[1],argv[2]);
	}

	const char* inData = "123";
	char* outEncData = NULL;
	uint32_t outEncDataLen = 0;
	if(pAes->Encrypt(inData,strlen(inData),&outEncData,outEncDataLen) == 0){
		printf("outEnc:%s\n",outEncData);
		pAes->Free(outEncData);
	}

	char* msg_out = NULL;
	uint32_t msg_out_len = 0;
	//string msg_data = "ykhWS0fWSlcv7EBgpJE07/RrSAJbif6MfLUMWkqJlUdU1nK0y0jePD85L5oBbkUiNwF6cNwJXAl26E1XqrkcrDzvni++OnahvUFfWYnXX2ilrjy1blP47yg7X7uvZ0dqS/pma5WeOuumQVVSE8hspMPUhojz1XSjsAEw7fUXfGoVZlBbKx44XmJcEMxCmzJ+IF18OFLeiHVpJyLxkr6PF10lH4MpZUFRCv64RrkXHnhr2hZx2LZkwG97k/ESZuI08wdWu7RMG8LuOBupMpg3puWYq2LJT/irLD1rjQmiSY5saBnutrmlt+XDM0NhgzTb0eBfwFrGnYcXCu/PDRljCf/OzHM0P24SQTWOQmKqJ6xyC22RTK0xEUjLLF5U0Xo/";
	//string msg_data = "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";
	//string msg_data = "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";
	//string msg_data = "EDoPoYllRGFjw9bej/T/2MS+JHXTq4s2m8HH53NG2RHo0dWGPhl0TDP9EKTgRKH/ygibZV6g5DI4er7zKXNaSA==";
	string msg_data = "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";
	int result = pAes->Decrypt(msg_data.c_str(), msg_data.length(), &msg_out, msg_out_len);
	if (result == 0)
	{
		msg_data = string(msg_out, msg_out_len);
		pAes->Free(msg_out);
		printf("%s\n",msg_data.c_str());
	}else {
		printf("decode failed:%d\n",result);
	}

	
	g_cmd_thread.StartThread();
	signal(SIGPIPE, SIG_IGN);
	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;
	
	netlib_eventloop();

	return 0;
}
