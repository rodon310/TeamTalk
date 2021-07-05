/*
 * ConnFactoryInterface.h
 * Copyright (C) 2021 xiaominfc
 * Email: xiaominfc@126.com
 * Distributed under terms of the MIT license.
 */

#ifndef EVENTFACTORYINTERFACE_H
#define EVENTFACTORYINTERFACE_H

#include "BaseSocket.h"

class EventFactoryInterface{
	public:
		virtual ~EventFactoryInterface(){};
		virtual CEventInterface* createEvent(CBaseSocket* socket) = 0;
};

#endif /* !EVENTFACTORYINTERFACE_H */
