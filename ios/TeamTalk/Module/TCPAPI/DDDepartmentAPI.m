//
//  DMTTDepartment.m
//  IOSDuoduo
//
//  Created by Michael Scofield on 2014-08-05.
//  Copyright (c) 2014 dujia. All rights reserved.
//

#import "DDDepartmentAPI.h"
#import "ImBuddy.pbobjc.h"
#import "ImBaseDefine.pbobjc.h"

@implementation DMTTDepartmentAPI

/**
 *  请求超时时间
 *
 *  @return 超时时间
 */
- (int)requestTimeOutTimeInterval
{
    return TimeOutTimeInterval;
}

/**
 *  请求的serviceID
 *
 *  @return 对应的serviceID
 */
- (int)requestServiceID
{
    return SID_BUDDY_LIST;
}

/**
 *  请求返回的serviceID
 *
 *  @return 对应的serviceID
 */
- (int)responseServiceID
{
    return SID_BUDDY_LIST;
}

/**
 *  请求的commendID
 *
 *  @return 对应的commendID
 */
- (int)requestCommendID
{
    return IM_ALL_DEPARTMENT_REQ;
    
}

/**
 *  请求返回的commendID
 *
 *  @return 对应的commendID
 */
- (int)responseCommendID
{
    return IM_ALL_DEPARTMENT_RES;
}

/**
 *  解析数据的block
 *
 *  @return 解析数据的block
 */
- (Analysis)analysisReturnData
{
    
    Analysis analysis = (id)^(NSData* data)
    {
        IMDepartmentRsp* rsp = [IMDepartmentRsp parseFromData:data error:nil];
        NSInteger departCount = [rsp deptListArray_Count];
        NSMutableArray *array = [NSMutableArray new];
        NSInteger recordTime = [[NSDate date] timeIntervalSince1970];
        for (int i = 0 ; i<departCount; i++) {
            DepartInfo* info = [rsp deptListArray][i];
            NSDictionary *result = @{@"departID":[@(info.deptId) stringValue],
                                     @"departName":info.deptName,
                                     @"parentID":[@(info.parentDeptId) stringValue],
                                     @"priority":@(info.priority),
                                     @"status":@(info.deptStatus),
                                     @"created":@(recordTime),
                                     @"updated":@(recordTime),
                                     };
            [array addObject:result];
        }

        return array;
    };
    return analysis;
}

/**
 *  打包数据的block
 *
 *  @return 打包数据的block
 */
- (Package)packageRequestObject
{
    Package package = (id)^(id object,uint32_t seqNo)
    {
        IMDepartmentReq *reqBuilder = [IMDepartmentReq new];
        NSInteger lastTime = [object[0] intValue];
        [reqBuilder setUserId:0];
        [reqBuilder setLatestUpdateTime:lastTime];

        DDDataOutputStream *dataout = [[DDDataOutputStream alloc] init];
        [dataout writeInt:0];
        [dataout writeTcpProtocolHeader:SID_BUDDY_LIST
                                    cId:IM_ALL_DEPARTMENT_REQ
                                  seqNo:seqNo];
        [dataout directWriteBytes:[reqBuilder data]];
        [dataout writeDataCount];
        return [dataout toByteArray];
    };
    return package;
}

@end
