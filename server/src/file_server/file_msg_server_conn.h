/*
 * @File : file_conn.h
 * @Author: xiaominfc
 * @Date: 2019-08-29 11:30:07
 * @Description: 
 */


#ifndef FILE_SERVER_FILE_MSG_SERVER_CONN_H_
#define FILE_SERVER_FILE_MSG_SERVER_CONN_H_

#include "ImPduConn.h"
#include "file_server_util.h"

typedef map<std::string, transfer_task_t*> TaskMap_t; // on client connect

class FileMsgServerConn : public CImPduConn {
public:
    FileMsgServerConn();
    virtual ~FileMsgServerConn();
    
    virtual void Close();
    
    virtual void OnConnect(CBaseSocket* socket);
    
    virtual void OnClose();
    virtual void OnTimer(uint64_t curr_tick);
    
    virtual void OnWrite();
    virtual void HandlePdu(CImPdu* pdu);
    
private:
    void _HandleHeartBeat(CImPdu* pdu);
    void _HandleMsgFileTransferReq(CImPdu* pdu);
    void _HandleGetServerAddressReq(CImPdu* pdu);
    
    bool connected_;
};


void InitializeFileMsgServerConn();



#endif /* defined(FILE_SERVER_FILE_MSG_SERVER_CONN_H_) */
