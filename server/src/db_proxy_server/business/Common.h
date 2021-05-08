/*
 * Common.h
 *
 *  Created on: 2014年11月12日
 *      Author: yumo
 */

#ifndef __DB_PROXY_COMMON_H__
#define __DB_PROXY_COMMON_H__

#define     GROUP_TOTAL_MSG_COUNTER_REDIS_KEY_SUFFIX    "_im_group_msg"
#define     GROUP_USER_MSG_COUNTER_REDIS_KEY_SUFFIX     "_im_user_group"
#define     GROUP_COUNTER_SUBKEY_COUNTER_FIELD          "count"


#define PARSE_PB(req,pPdu,_block) \
	if(req.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength())) \
	_block \
	else{log("parse pb failed");}


//default use db_conn for _block
#define DBCONN(name,_conn,_block) \
	CDBConn* _conn = CDBManager::getInstance()->GetDBConn(name);\
	if(_conn){\
	_block\
	CDBManager::getInstance()->RelDBConn(_conn);\
	}else{\
		log("no db connection for %s",name);\
	}

//for db write
#define DBCONN_MASTER(_conn,_block) DBCONN("teamtalk_master",_conn,_block)

//for db read
#define DBCONN_SLAVE(_conn,_block) DBCONN("teamtalk_slave",_conn,_block)

#endif