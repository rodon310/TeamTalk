/*
 * ImPduConn.h
 * Copyright (C) 2021 xiaominfc
 * Email: xiaominfc@126.com
 * Distributed under terms of the MIT license.
 */

#ifndef IMPDUCONN_H
#define IMPDUCONN_H

#include "imconn.h"
#include "ImPduBase.h"

class CImPduConn: public CImConn{
public:
	virtual ~CImPduConn(){};
	virtual void HandlePdu(CImPdu* pPdu){(void)pPdu;};
	virtual int SendPdu(CImPdu* pPdu) { return Send(pPdu->GetBuffer(), pPdu->GetLength());}
	virtual void HandleData();
};


class PduTask:public CTask {
public:
	PduTask(CImPduConn * conn, CImPdu* pPdu);
	virtual ~PduTask();
	virtual void run();
private:
	CImPduConn *m_conn;
	CImPdu *m_pPdu;
};

#endif /* !IMPDUCONN_H */
