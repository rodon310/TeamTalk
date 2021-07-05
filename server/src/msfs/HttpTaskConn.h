#ifndef __HTTP_TASK_CONN_H__
#define __HTTP_TASK_CONN_H__

#include "util.h"
#if (MSFS_LINUX)
#include <sys/sendfile.h>
#elif (MSFS_BSD)
#include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/uio.h>
#endif
#include <pthread.h>
#include "netlib.h"
#include "FileManager.h"
#include "ConfigFileReader.h"
#include "ThreadPool.h"
#include "HttpParserWrapper.h"
#include "HttpConn.h"


#define HTTP_CONN_TIMEOUT			30000
#define HTTP_UPLOAD_MAX				 0xA00000	 //10M
#define BOUNDARY_MARK					"boundary="
#define HTTP_END_MARK						"\r\n\r\n"
#define CONTENT_TYPE							"Content-Type:"
#define CONTENT_DISPOSITION		 "Content-Disposition:"
#define READ_BUF_SIZE	0x100000 //1M
#define HTTP_RESPONSE_HEADER	"HTTP/1.1 200 OK\r\n"\
"Connection:close\r\n"\
"Content-Length:%d\r\n"\
"Content-Type:multipart/form-data\r\n\r\n"
#define HTTP_RESPONSE_EXTEND		"HTTP/1.1 200 OK\r\n"\
									"Connection:close\r\n"\
									"Content-Length:%d\r\n"\
									"Content-Type:multipart/form-data\r\n\r\n"
#define HTTP_RESPONSE_IMAGE		 "HTTP/1.1 200 OK\r\n"\
									"Connection:close\r\n"\
									"Content-Length:%d\r\n"\
									"Content-Type:image/%s\r\n\r\n"
#define HTTP_RESPONSE_HTML		  "HTTP/1.1 200 OK\r\n"\
									"Connection:close\r\n"\
									"Content-Length:%d\r\n"\
									"Content-Type:text/html;charset=utf-8\r\n\r\n%s"
#define HTTP_RESPONSE_HTML_MAX	  1024
#define HTTP_RESPONSE_403		   "HTTP/1.1 403 Access Forbidden\r\n"\
									"Content-Length: 0\r\n"\
									"Connection: close\r\n"\
									"Content-Type: text/html;charset=utf-8\r\n\r\n"
#define HTTP_RESPONSE_403_LEN	   strlen(HTTP_RESPONSE_403)
#define HTTP_RESPONSE_404		   "HTTP/1.1 404 Not Found\r\n"\
									"Content-Length: 0\r\n"\
									"Connection: close\r\n"\
									"Content-Type: text/html;charset=utf-8\r\n\r\n"
#define HTTP_RESPONSE_404_LEN	   strlen(HTTP_RESPONSE_404)
#define HTTP_RESPONSE_500		   "HTTP/1.1 500 Internal Server Error\r\n"\
									"Connection:close\r\n"\
									"Content-Length:0\r\n"\
									"Content-Type:text/html;charset=utf-8\r\n\r\n"
#define HTTP_RESPONSE_500_LEN	   strlen(HTTP_RESPONSE_500)

using namespace msfs;

extern FileManager * g_fileManager;
extern CConfigFileReader config_file;
extern CThreadPool g_PostThreadPool;
extern CThreadPool g_GetThreadPool;

typedef struct {
	uint32_t conn_handle;
	int method;
	int nContentLen;
	string strAccessHost;
	char* pContent;
	string strUrl;
	string strContentType;
}Request_t;

typedef struct {
	uint32_t	conn_handle;
	char*	 pContent;
	uint32_t content_len;
} Response_t;

class CHttpTaskConn;
class CHttpTask: public CTask
{
public:
	CHttpTask(Request_t request);
	virtual ~CHttpTask();
	void run();
	void OnUpload();
	void OnDownload();
private:
	uint32_t m_ConnHandle;
	int m_nMethod;
	string m_strUrl;
	string m_strContentType;
	char* m_pContent;
	int m_nContentLen;
	string m_strAccessHost;
};

class CHttpTaskConn: public CHttpConn
{
public:
	CHttpTaskConn(){};
	virtual ~CHttpTaskConn();
	virtual void HandleWork();
	static void AddResponsePdu(uint32_t conn_handle, char* pContent, int nLen);   // 工作线程调用
	static void SendResponsePduList();  // 主线程调用
protected:
	char* m_access_host;
	static CLock		  s_list_lock;
	static list<Response_t*> s_response_pdu_list;	// 主线程发送回复消息
};

void init_http_task_conn();

#endif
