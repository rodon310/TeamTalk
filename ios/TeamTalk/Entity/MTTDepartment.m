//
//  MTTDepartment.m
//  IOSDuoduo
//
//  Created by Michael Scofield on 2014-08-06.
//  Copyright (c) 2014 dujia. All rights reserved.
//

#import "MTTDepartment.h"

@implementation MTTDepartment
- (instancetype)init
{
    self = [super init];
    if (self) {
        self.ID =@"";
        self.parentID=@"";
        self.departName=@"";
        self.priority=0;
        self.status=0;
        self.updated=0;
        self.created=0;

    }
    return self;
}

+(id)departmentFromDic:(NSDictionary *)dic
{
    MTTDepartment *department = [MTTDepartment new];
    department.ID = [dic objectForKey:@"departID"];
    department.departName = [dic objectForKey:@"departName"];
    department.parentID = [dic objectForKey:@"parentID"];
    department.status = [[dic objectForKey:@"status"] integerValue];
    department.priority = [[dic objectForKey:@"priority"] integerValue];
    department.updated = [[dic objectForKey:@"updated"] integerValue];
    department.created = [[dic objectForKey:@"created"] integerValue];
    return department;
}
@end
