/*================================================================
*	 Copyright (c) 2014年 lanhu. All rights reserved.
*   
*   文件名称：CacheManager.h
*   创 建 者：Zhang Yuanhao
*   邮	箱：bluefoxah@gmail.com
*   创建日期：2014年12月02日
*   描	述：
*
#pragma once
================================================================*/
#ifndef __CACHEMANAGER_H__
#define __CACHEMANAGER_H__

#include <list>
#include <map>

#include "ostype.h"
#include "Lock.h"
#include "Condition.h"
#include "ImPduBase.h"
#include "public_define.h"
#include "IM.BaseDefine.pb.h"

class CSyncCenter
{
public:
	static CSyncCenter* getInstance();
	
	uint32_t getLastUpdate() {
		CAutoLock auto_lock(&last_update_lock_);
		return m_nLastUpdate;
	}
	uint32_t getLastUpdateGroup() {
		CAutoLock auto_lock(&last_update_lock_);
		return m_nLastUpdateGroup;
	}
	string getDeptName(uint32_t nDeptId);
	void startSync();
	void stopSync();
	void init();
	void updateTotalUpdate(uint32_t nUpdated);

private:
	void updateLastUpdateGroup(uint32_t nUpdated);
	
	CSyncCenter();
	~CSyncCenter();
	static void* doSyncGroupChat(void* arg);

private:
	static CSyncCenter* m_pInstance;

private:
	void getDept(uint32_t nDeptId, DBDeptInfo_t** pDept);
#ifdef _WIN32
	DWORD		m_nGroupChatThreadId;
#else
	pthread_t	m_nGroupChatThreadId;
#endif
	DBDeptMap_t* m_pDeptInfo;
	uint32_t	m_nLastUpdateGroup;
	uint32_t	m_nLastUpdate;

	CCondition* m_pCondGroupChat;
	bool m_bSyncGroupChatWaitting;
	CLock*	  m_pLockGroupChat;
	static bool		m_bSyncGroupChatRuning;
	

	CLock last_update_lock_;
};


#endif /*defined(__CACHEMANAGER_H__) */
