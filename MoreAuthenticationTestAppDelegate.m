//
//  MoreAuthenticationTestAppDelegate.m
//  MoreAuthenticationTest
//
//  Created by Reza Jelveh on 9/9/09.
//  Copyright 2009 Flying Seagull. All rights reserved.
//

#import "MoreAuthenticationTestAppDelegate.h"

@implementation MoreAuthenticationTestAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application 
}

- (void)testOpen:(id)sender {
    NSString *rootPath = [[NSBundle mainBundle] resourcePath];
    NSString *filePath = [rootPath stringByAppendingPathComponent:@"MoreAuthHelper"];
    NSTask *helper = [[NSTask alloc] init];

    [helper setLaunchPath:filePath];
    [helper setArguments:[NSArray arrayWithObjects:@"world", @"test", nil]];
    [helper launch];
    [helper waitUntilExit];
    [helper release];

    NSLog(@"hello world");
}

@end
