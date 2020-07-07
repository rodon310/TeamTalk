/*
 * HttpPdu.h
 *
 * Created on: 2013-10-1
 * Author: ziteng@mogujie.com
 */

#ifndef HTTPPDU_H_
#define HTTPPDU_H_

#include "util.h"
#include "ImPduBase.h"
#include <list>
#include "IM.BaseDefine.pb.h"

#define HTTP_JSON_QUEYR_HEADER "HTTP/1.1 200 OK\r\n"\
		"Cache-Control:no-cache\r\n"\
		"Connection:close\r\n"\
		"Content-Length:%d\r\n"\
		"Content-Type:text/html;charset=utf-8\r\n\r\n%s"


// jsonp parameter parser
class CPostDataParser
{
public:
	CPostDataParser() {}
	virtual ~CPostDataParser() {}

	bool Parse(const char* content);

	char* GetValue(const char* key);
private:
	std::map<std::string, std::string> m_post_map;
};

char* PackSendResult(uint32_t error_code, const char* error_msg = "");
uint32_t PackSendResult(uint32_t error_code, const char* error_msg, uint32_t msg_id , char * out_data, uint32_t size);
char* PackSendCreateGroupResult(uint32_t error_code, const char* error_msg, uint32_t group_id);
char* PackGetUserIdByNickNameResult(uint32_t result, std::list<IM::BaseDefine::UserInfo> user_list);
#endif /* HTTPPDU_H_ */
