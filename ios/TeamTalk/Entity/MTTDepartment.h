//
//  MTTDepartment.h
//  IOSDuoduo
//
//  Created by Michael Scofield on 2014-08-06.
//  Copyright (c) 2014 dujia. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MTTDepartment : NSObject

@property(nonatomic ,copy)NSString *ID;
@property(nonatomic ,copy)NSString *parentID;
@property(nonatomic ,copy)NSString *departName;
@property(nonatomic ,assign)NSInteger priority;
@property(nonatomic ,assign)NSInteger status;
@property(nonatomic ,assign)NSInteger created;
@property(nonatomic ,assign)NSInteger updated;

+(id)departmentFromDic:(NSDictionary *)dic;
@end
