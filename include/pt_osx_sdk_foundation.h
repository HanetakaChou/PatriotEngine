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

#ifndef _PT_APPLE_SDK_POSIX_MACH_FOUNDATION_H_
#define _PT_APPLE_SDK_POSIX_MACH_FOUNDATION_H_ 1

#include "pt_apple_sdk_common.h"
#include "pt_osx_sdk_objc.h"

typedef struct _Class_NSThreadDetachTarget_T_ *Class_NSThreadDetachTarget;
typedef struct _NSThreadDetachTarget_T_ *NSThreadDetachTarget;
typedef struct _NSThreadDetachSelector__T_ *NSThreadDetachSelector_;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK Class_NSThreadDetachTarget NSThreadDetachTarget_allocateClass(char const *class_name, char const *selector_name, void (*_I_NSThreadDetachSelector_)(NSThreadDetachTarget, NSThreadDetachSelector_, void *argument));
    PT_ATTR_APPLE_SDK NSThreadDetachTarget NSThreadDetachTarget_alloc(Class_NSThreadDetachTarget class_ns_thread_detach_target);
    PT_ATTR_APPLE_SDK NSThreadDetachTarget NSThreadDetachTarget_init(NSThreadDetachTarget ns_thread_detach_target);
    PT_ATTR_APPLE_SDK void NSThreadDetachTarget_release(NSThreadDetachTarget ns_thread_detach_target);
    /** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Foundation.framework/Headers/NSThread.h */
    PT_ATTR_APPLE_SDK void NSThread_detachNewThreadSelector(char const *selector_name, NSThreadDetachTarget target, void *argument);
    PT_ATTR_APPLE_SDK bool NSThread_isMultiThreaded();
#ifdef __cplusplus
}
#endif

typedef struct _NSFileManager_T_ *NSFileManager;
typedef NSUInteger NSSearchPathDirectory;
enum : NSSearchPathDirectory
{
    NSApplicationDirectory = 1,
    NSDemoApplicationDirectory,
    NSDeveloperApplicationDirectory,
    NSAdminApplicationDirectory,
    NSLibraryDirectory,
    NSDeveloperDirectory,
    NSUserDirectory,
    NSDocumentationDirectory,
    NSDocumentDirectory,
    NSCoreServiceDirectory,
    NSAutosavedInformationDirectory __attribute__((availability(ios, introduced = 4.0), availability(macos, introduced = 10.6))) = 11,
    NSDesktopDirectory = 12,
    NSCachesDirectory = 13,
    NSApplicationSupportDirectory = 14,
    NSDownloadsDirectory __attribute__((availability(ios, introduced = 2.0), availability(macos, introduced = 10.5))) = 15,
    NSInputMethodsDirectory __attribute__((availability(ios, introduced = 4.0), availability(macos, introduced = 10.6))) = 16,
    NSMoviesDirectory __attribute__((availability(ios, introduced = 4.0), availability(macos, introduced = 10.6))) = 17,
    NSMusicDirectory __attribute__((availability(ios, introduced = 4.0), availability(macos, introduced = 10.6))) = 18,
    NSPicturesDirectory __attribute__((availability(ios, introduced = 4.0), availability(macos, introduced = 10.6))) = 19,
    NSPrinterDescriptionDirectory __attribute__((availability(ios, introduced = 4.0), availability(macos, introduced = 10.6))) = 20,
    NSSharedPublicDirectory __attribute__((availability(ios, introduced = 4.0), availability(macos, introduced = 10.6))) = 21,
    NSPreferencePanesDirectory __attribute__((availability(ios, introduced = 4.0), availability(macos, introduced = 10.6))) = 22,
    NSApplicationScriptsDirectory __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.8))) = 23,
    NSItemReplacementDirectory __attribute__((availability(ios, introduced = 4.0), availability(macos, introduced = 10.6))) = 99,
    NSAllApplicationsDirectory = 100,
    NSAllLibrariesDirectory = 101,
    NSTrashDirectory __attribute__((availability(ios, introduced = 11.0), availability(macos, introduced = 10.8))) = 102
};
typedef NSUInteger NSSearchPathDomainMask;
enum : NSSearchPathDomainMask
{
    NSUserDomainMask = 1,
    NSLocalDomainMask = 2,
    NSNetworkDomainMask = 4,
    NSSystemDomainMask = 8,
    NSAllDomainsMask = 0x0ffff
};
typedef struct _NSArray_NSURL_T_ *NSArray_NSURL;
typedef struct _NSURL_T_ *NSURL;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK NSFileManager NSFileManager_defaultManager();
    PT_ATTR_APPLE_SDK NSArray_NSURL NSFileManager_URLsForDirectory(NSFileManager ns_file_manager, NSSearchPathDirectory in_domains, NSSearchPathDomainMask domain_mask);
    PT_ATTR_APPLE_SDK NSURL NSArray_NSURL_objectAtIndexedSubscript(NSArray_NSURL ns_array_ns_url, NSUInteger idx);
    PT_ATTR_APPLE_SDK char const *NSURL_fileSystemRepresentation(NSURL ns_url);
#ifdef __cplusplus
}
#endif

typedef struct _NSBundle_T_ *NSBundle;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK NSBundle NSBundle_mainBundle();
    PT_ATTR_APPLE_SDK NSString NSBundle_resourcePath(NSBundle ns_bundle);
#ifdef __cplusplus
}
#endif

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/CoreGraphics.framework/Headers/CGBase.h */
#if defined(__LP64__) && __LP64__
#define CGFLOAT_TYPE double
#else
#define CGFLOAT_TYPE float
#endif
typedef CGFLOAT_TYPE CGFloat;

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Foundation.framework/Headers/CGGeometry.h */
typedef struct
{
    CGFloat x;
    CGFloat y;
} CGPoint;
typedef struct
{
    CGFloat width;
    CGFloat height;
} CGSize;
typedef struct
{
    CGPoint origin;
    CGSize size;
} CGRect;

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Foundation.framework/Headers/NSGeometry.h */
#if (defined(__LP64__) && __LP64__) || NS_BUILD_32_LIKE_64
typedef CGPoint NSPoint;
typedef CGSize NSSize;
typedef CGRect NSRect;
#endif

static inline NSSize NSMakeSize(CGFloat w, CGFloat h)
{
    NSSize s;
    s.width = w;
    s.height = h;
    return s;
}
static inline NSRect NSMakeRect(CGFloat x, CGFloat y, CGFloat w, CGFloat h)
{
    NSRect r;
    r.origin.x = x;
    r.origin.y = y;
    r.size.width = w;
    r.size.height = h;
    return r;
}

#endif
