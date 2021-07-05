/*
 * @Author: xiaominfc
 * @Date: 2019-08-29 11:30:07
 * @Description: 
 */

#ifndef BASE_IM_CONN_UTIL_H_
#define BASE_IM_CONN_UTIL_H_

#include "ostype.h"

namespace google { namespace protobuf {
    class MessageLite;
}}

class CImPduConn;

int SendMessageLite(CImPduConn* conn, uint16_t sid, uint16_t cid, const ::google::protobuf::MessageLite* message);
int SendMessageLite(CImPduConn* conn, uint16_t sid, uint16_t cid, uint16_t seq_num, const ::google::protobuf::MessageLite* message);
int SendMessageLite(CImPduConn* conn, uint16_t sid, uint16_t cid, uint16_t seq_num, uint16_t error, const ::google::protobuf::MessageLite* message);

#endif /* defined(BASE_IM_CONN_UTIL_H_) */
