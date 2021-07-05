/*
 * EventInterface.h
 * Copyright (C) 2021 xiaominfc
 * Email: xiaominfc@126.com
 * Distributed under terms of the MIT license.
 */

#ifndef EVENTINTERFACE_H
#define EVENTINTERFACE_H

#include "util.h"
class CEventInterface : public CRefObject{
	public:
		~CEventInterface(){};

		virtual void OnRead() = 0;
		virtual void OnWrite() = 0;
		virtual void OnClose() = 0;
};

#endif /* !EVENTINTERFACE_H */
