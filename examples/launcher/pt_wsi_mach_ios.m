//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <dispatch/dispatch.h>
#include <CoreVideo/CoreVideo.h>
#include <QuartzCore/QuartzCore.h>

@interface pt_wsi_mach_osx_nsthread_detach_target : NSObject
- (void)pt_wsi_mach_osx_nsmain:(void *)__here_ns_thread_detach_target_has_finished;
@end

@implementation pt_wsi_mach_osx_nsthread_detach_target
- (void)pt_wsi_mach_osx_nsmain:(void *)__here_ns_thread_detach_target_has_finished
{
    (*((bool volatile *)__here_ns_thread_detach_target_has_finished)) = true;
}
@end

void cocoa_set_multithreaded(void)
{
    @autoreleasepool
    {
        bool volatile __here_ns_thread_detach_target_has_finished = false;
        id ns_thread_detach_target = [[pt_wsi_mach_osx_nsthread_detach_target alloc] init];
        [NSThread detachNewThreadSelector:@selector(pt_wsi_mach_osx_nsmain:)
                                 toTarget:ns_thread_detach_target
                               withObject:((__bridge id)((void *)&__here_ns_thread_detach_target_has_finished))];
        while (!__here_ns_thread_detach_target_has_finished)
        {
            pthread_yield_np();
        }
    }
}

bool cocoa_is_multithreaded(void)
{
    @autoreleasepool
    {
        return [NSThread isMultiThreaded];
    }
}
