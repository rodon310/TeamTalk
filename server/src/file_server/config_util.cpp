/*
 * @File: 
 * @Author: xiaominfc
 * @Date: 2019-08-29 11:30:07
 * @Description: 
 */
//
//  config_util.cpp
//  im-server-mac-new
//
//  Created by wubenqi on 15/7/16.
//  Copyright (c) 2015年 benqi. All rights reserved.
//

#include "config_util.h"

void ConfigUtil::AddAddress(const char* ip, uint16_t port) {
    IM::BaseDefine::IpAddr addr;
    addr.set_ip(ip);
    addr.set_port(port);
    addrs_.push_back(addr);
}
