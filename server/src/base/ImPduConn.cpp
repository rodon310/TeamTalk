/*
 * ImPduConn.cpp
 * Copyright (C) 2021 xiaominfc <xiaominfc@126.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "ImPduConn.h"


void CImPduConn::HandleData(){
	CImPdu* pPdu = NULL;
	try
	{
		while((pPdu = CImPdu::ReadPdu(m_in_buf.GetBuffer(), m_in_buf.GetWriteOffset())))
		{
			uint32_t pdu_len = pPdu->GetLength();
			HandlePdu(pPdu);
			delete pPdu;
			pPdu = NULL;
			m_in_buf.Read(NULL, pdu_len);
		}
	} catch (CPduException& ex) {
		log("!!!catch exception, sid=%u, cid=%u, err_code=%u, err_msg=%s, close the connection ",ex.GetServiceId(), ex.GetCommandId(), ex.GetErrorCode(), ex.GetErrorMsg());
		if (pPdu) {
			delete pPdu;
			pPdu = NULL;
		}
		OnClose();
	}
}


PduTask::PduTask(CImPduConn * conn, CImPdu* pPdu){
	m_conn = conn;
	m_conn->AddRef();
	m_pPdu = pPdu;
}

PduTask::~PduTask(){
	delete m_pPdu;
	m_conn->ReleaseRef();
}

void PduTask::run(){
	if(m_conn) {
		m_conn->HandlePdu(m_pPdu);
	}
}

