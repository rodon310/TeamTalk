#include "HttpParserWrapper.h"
#include "atomic.h"
#include "HttpTaskConn.h"


CLock CHttpTaskConn::s_list_lock;
list<Response_t*> CHttpTaskConn::s_response_pdu_list;


void http_conn_loop_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	(void)callback_data;
	(void)msg;
	(void)handle;
	(void)pParam;
	CHttpTaskConn::SendResponsePduList();
}



void init_http_task_conn()
{
	//netlib_register_timer(http_conn_timer_callback, NULL, 1000);
	init_http_conn();
	netlib_add_loop(http_conn_loop_callback, NULL);
}

//////////////////////////

CHttpTask::CHttpTask(Request_t request)
{
	m_ConnHandle = request.conn_handle;
	m_nMethod = request.method;
	m_strUrl = request.strUrl;
	m_strContentType = request.strContentType;
	m_pContent = request.pContent;
	m_nContentLen = request.nContentLen;
	m_strAccessHost = request.strAccessHost;
}

CHttpTask::~CHttpTask()
{
}

void CHttpTask::run()
{

	if(HTTP_GET == m_nMethod)
	{
		OnDownload();
	}
	else if(HTTP_POST == m_nMethod)
	{
	   OnUpload();
	}
	else
	{
		char* pContent = new char[strlen(HTTP_RESPONSE_403)];
		snprintf(pContent, strlen(HTTP_RESPONSE_403), HTTP_RESPONSE_403);
		CHttpTaskConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
	}
	if(m_pContent != NULL)
	{
		delete [] m_pContent;
		m_pContent = NULL;
	}
}

void CHttpTask::OnUpload()
{

	//get the file original filename
	char *pContent = NULL;
	int nTmpLen = 0;
	int error_code = 1;
	const char* pPos = memfind(m_pContent, m_nContentLen, CONTENT_DISPOSITION, strlen(CONTENT_DISPOSITION));
	if (pPos != NULL)
	{
		nTmpLen = pPos - m_pContent;
		const char* pPos2 = memfind(pPos, m_nContentLen - nTmpLen, "filename=", strlen("filename="));
		if (pPos2 != NULL)
		{
			pPos = pPos2 + strlen("filename=") + 1;
			const char * pPosQuotes = memfind(pPos, m_nContentLen - nTmpLen, "\"", strlen("\""));
			int nFileNameLen = pPosQuotes - pPos;

			char szFileName[256];
			if(nFileNameLen <= 255)
			{
				memcpy(szFileName,  pPos, nFileNameLen);
				szFileName[nFileNameLen] = 0;				   
				const char* pPosType = memfind(szFileName, nFileNameLen, ".", 1, false);
				if(pPosType != NULL)
				{
					char szType[16];
					int nTypeLen = nFileNameLen - (pPosType + 1 - szFileName);
					if(nTypeLen <=15)
					{
						memcpy(szType, pPosType + 1, nTypeLen);
						szType[nTypeLen] = 0;
						log("upload file, file name:%s", szFileName);
						char szExtend[16];
						const char* pPosExtend = memfind(szFileName, nFileNameLen, "_", 1, false);
						if(pPosExtend != NULL)
						{
							const char* pPosTmp = memfind(pPosExtend, nFileNameLen - (pPosExtend + 1 - szFileName), "x", 1);
							if(pPosTmp != NULL)
							{
								int nWidthLen = pPosTmp - pPosExtend - 1;
								int nHeightLen = pPosType - pPosTmp - 1;
								if(nWidthLen >= 0 && nHeightLen >= 0)
								{
									int nWidth = 0;
									int nHeight = 0;
									char szWidth[5], szHeight[5];
									if(nWidthLen <=4 && nHeightLen <=4)
									{
										memcpy(szWidth, pPosExtend + 1, nWidthLen);
										szWidth[nWidthLen] = 0;
										memcpy(szHeight, pPosTmp + 1, nHeightLen );
										szHeight[nHeightLen] = 0;
										nWidth = atoi(szWidth);
										nHeight = atoi(szHeight);
										snprintf(szExtend, sizeof(szExtend), "%dx%d.%s", nWidth, nHeight, szType);
									}else
									{
										szExtend[0] = 0;
									}
								}
								else
								{
									szExtend[0] = 0;
								}
							}
							else{
								szExtend[0] = 0;
							}
						}
						else
						{
							szExtend[0] = 0;
						}

						//get the file content
						size_t nPos = m_strContentType.find(BOUNDARY_MARK);
						if (nPos != m_strContentType.npos)
						{
							const  char* pBoundary = m_strContentType.c_str() + nPos + strlen(BOUNDARY_MARK);
							int nBoundaryLen = m_strContentType.length() - nPos - strlen(BOUNDARY_MARK);

							pPos = memfind(m_pContent, m_nContentLen, pBoundary, nBoundaryLen);
							if (NULL != pPos)
							{
								nTmpLen = pPos - m_pContent;
								pPos = memfind(m_pContent + nTmpLen, m_nContentLen - nTmpLen, CONTENT_TYPE, strlen(CONTENT_TYPE));
								if (NULL != pPos)
								{
									nTmpLen = pPos - m_pContent;
									pPos = memfind(m_pContent + nTmpLen, m_nContentLen - nTmpLen, HTTP_END_MARK, strlen(HTTP_END_MARK));
									if (NULL != pPos)
									{
										nTmpLen = pPos - m_pContent;
										const char* pFileStart = pPos + strlen(HTTP_END_MARK);
										pPos2 = memfind(m_pContent + nTmpLen, m_nContentLen - nTmpLen, pBoundary, nBoundaryLen);
										if (NULL != pPos2)
										{
											int64_t nFileSize = pPos2 - strlen(HTTP_END_MARK) - pFileStart;
											if (nFileSize <= HTTP_UPLOAD_MAX)
											{
												char filePath[512] ={ 0 };

												int result = 0;
												if(strlen(szExtend) != 0)
												{
													result = g_fileManager->uploadFile(szType, pFileStart, nFileSize, filePath, szExtend);
												}
												else{
													result = g_fileManager->uploadFile(szType, pFileStart, nFileSize, filePath);
												}
												if(result==0) {
													char url[1024];
													snprintf(url, sizeof(url), "{\"error_code\":0,\"error_msg\": \"成功\",\"path\":\"%s\",\"url\":\"http://%s/%s\"}", filePath,m_strAccessHost.c_str(), filePath);
													uint32_t content_length = strlen(url);
													pContent = new char[HTTP_RESPONSE_HTML_MAX];
													snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
													CHttpTaskConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
													return;
												}
											}
											char url[128];
											snprintf(url, sizeof(url), "{\"error_code\":12,\"error_msg\": \"文件过大\",\"path\":\"\",\"url\":\"\"}");
											log("%s",url);
											uint32_t content_length = strlen(url);
											pContent = new char[HTTP_RESPONSE_HTML_MAX];
											snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
											CHttpTaskConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
											return;

										}
										else
										{
											error_code = 8;
										}
									}
									else
									{
										error_code = 7;
									}

								}
								else
								{
									error_code = 6;
								}
							}
							else
							{
								error_code = 5;
							}
						}
						else
						{
							error_code = 4;
						}
					}
					else{
						error_code = 9;
					}
				}
				else{
					error_code = 10;
				}
			}else
			{
				error_code = 11;
			}
		}
		else
		{
			error_code = 3;
		}
	}
	else
	{
		error_code = 2;
	}

	char out_result[128];
	snprintf(out_result, sizeof(out_result), "{\"error_code\":%d,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}",error_code);
	log("%s", out_result);
	uint32_t content_length = strlen(out_result);
	pContent = new char[HTTP_RESPONSE_HTML_MAX];
	snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,out_result);
	CHttpTaskConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));

}

void  CHttpTask::OnDownload()
{
		uint32_t  nFileSize = 0;
		int32_t nTmpSize = 0;
		string strPath;
		if(g_fileManager->getAbsPathByUrl(m_strUrl, strPath ) == 0)
		{
			nTmpSize = File::getFileSize((char*)strPath.c_str());
			if(nTmpSize != -1)
			{
				char szResponseHeader[1024];
				size_t nPos = strPath.find_last_of(".");
				string strType = strPath.substr(nPos + 1, strPath.length() - nPos);
				if(strType == "jpg" || strType == "JPG" || strType == "jpeg" || strType == "JPEG" || strType == "png" || strType == "PNG" || strType == "gif" || strType == "GIF")
				{
					snprintf(szResponseHeader, sizeof(szResponseHeader), HTTP_RESPONSE_IMAGE, nTmpSize, strType.c_str());
				}
				else
				{
					snprintf(szResponseHeader,sizeof(szResponseHeader), HTTP_RESPONSE_EXTEND, nTmpSize);
				}
				int nLen = strlen(szResponseHeader);
				char* pContent = new char[nLen + nTmpSize];
				memcpy(pContent, szResponseHeader, nLen);
				g_fileManager->downloadFileByUrl((char*)m_strUrl.c_str(), pContent + nLen, &nFileSize);
				int nTotalLen = nLen + nFileSize;
				CHttpTaskConn::AddResponsePdu(m_ConnHandle, pContent, nTotalLen);
			}
			else
			{
				int nTotalLen = strlen(HTTP_RESPONSE_404);
				char* pContent = new char[nTotalLen];
				snprintf(pContent, nTotalLen, HTTP_RESPONSE_404);
				CHttpTaskConn::AddResponsePdu(m_ConnHandle, pContent, nTotalLen);
				log("File size is invalied\n");
				
			}
		}
		else
		{
			int nTotalLen = strlen(HTTP_RESPONSE_500);
			char* pContent = new char[nTotalLen];
			snprintf(pContent, nTotalLen, HTTP_RESPONSE_500);
			CHttpTaskConn::AddResponsePdu(m_ConnHandle, pContent, nTotalLen);
		}
}


CHttpTaskConn::~CHttpTaskConn()
{
}



void CHttpTaskConn::HandleWork()
{

	// 每次请求对应一个HTTP连接，所以读完数据后，不用在同一个连接里面准备读取下个请求
	char* in_buf = (char*) m_in_buf.GetBuffer();
	uint32_t buf_len = m_in_buf.GetWriteOffset();
	in_buf[buf_len] = '\0';

	//log("OnRead, buf_len=%u, conn_handle=%u", buf_len, m_conn_handle); // for debug
	m_HttpParser.ParseHttpContent(in_buf, buf_len);

	if (m_HttpParser.IsReadAll())
	{
		string strUrl = m_HttpParser.GetUrl();
		log("IP:%s access:%s", m_peer_ip.c_str(), strUrl.c_str());
		if (strUrl.find("..") != strUrl.npos) {
			CHttpConn::Close();
			return;
		}
		m_access_host = m_HttpParser.GetHost();
		if (m_HttpParser.GetContentLen() > HTTP_UPLOAD_MAX)
		{
			// file is too big
			log("content  is too big");
			char url[128];
			snprintf(url, sizeof(url), "{\"error_code\":1,\"error_msg\": \"上传文件过大\",\"url\":\"\"}");
			log("%s",url);
			uint32_t content_length = strlen(url);
			char pContent[1024];
			snprintf(pContent, sizeof(pContent), HTTP_RESPONSE_HTML, content_length,url);
			Send(pContent, strlen(pContent));
			return;
		}

		int nContentLen = m_HttpParser.GetContentLen();
		char* pContent = NULL;
		if(nContentLen != 0)
		{
			try {
				pContent =new char[nContentLen];
				memcpy(pContent, m_HttpParser.GetBodyContent(), nContentLen);
			}
			catch(...)
			{
				log("not enough memory");
				char szResponse[HTTP_RESPONSE_500_LEN + 1];
				snprintf(szResponse, HTTP_RESPONSE_500_LEN, "%s", HTTP_RESPONSE_500);
				Send(szResponse, HTTP_RESPONSE_500_LEN);
				return;
			}
		}
		Request_t request;
		request.conn_handle = m_conn_handle;
		request.method = m_HttpParser.GetMethod();;
		request.nContentLen = nContentLen;
		request.pContent = pContent;
		request.strAccessHost = m_HttpParser.GetHost();
		request.strContentType = m_HttpParser.GetContentType();
		request.strUrl = m_HttpParser.GetUrl() + 1;
		CHttpTask* pTask = new CHttpTask(request);
		if(HTTP_GET == m_HttpParser.GetMethod())
		{
			g_GetThreadPool.AddTask(pTask);
		}
		else
		{
			g_PostThreadPool.AddTask(pTask);
		}
	}
}



void CHttpTaskConn::AddResponsePdu(uint32_t conn_handle, char* pContent, int nLen)
{
	Response_t* pResp = new Response_t;
	pResp->conn_handle = conn_handle;
	pResp->pContent = pContent;
	pResp->content_len = nLen;
	s_list_lock.lock();
	s_response_pdu_list.push_back(pResp);
	s_list_lock.unlock();
}

void CHttpTaskConn::SendResponsePduList()
{
	s_list_lock.lock();
	while (!s_response_pdu_list.empty()) {
		Response_t* pResp = s_response_pdu_list.front();
		s_response_pdu_list.pop_front();
		s_list_lock.unlock();

		CHttpConn* pConn = FindHttpConnByHandle(pResp->conn_handle);
		if (pConn) {
			pConn->Send(pResp->pContent, pResp->content_len);
		}
		if(pResp->pContent != NULL)
		{
			delete [] pResp->pContent;
			pResp->pContent = NULL;
		}
		if(pResp != NULL)
		{
			delete pResp;
			pResp = NULL;
		}

		s_list_lock.lock();
	}

	s_list_lock.unlock();
}

