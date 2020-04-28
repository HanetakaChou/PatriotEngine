#ifndef _FOUNDATION_CXX_H_
#define _FOUNDATION_CXX_H_ 1

#include "NSRuntime_CXX.h"

struct NSObject *NSArrayNSObject_objectAtIndexedSubscript(struct NSArrayNSObject *self, NSUInteger idx);

struct NSFileManager *NSFileManager_defaultManager();

typedef NSUInteger NSSearchPathDirectory;
enum
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
enum
{
    NSUserDomainMask = 1,
    NSLocalDomainMask = 2,
    NSNetworkDomainMask = 4,
    NSSystemDomainMask = 8,
    NSAllDomainsMask = 0x0ffff
};

struct NSArrayNSURL *NSFileManager_URLsForDirectory(struct NSFileManager *self, NSSearchPathDirectory inDomains, NSSearchPathDomainMask domainMask);

struct NSURL *NSArrayNSURL_objectAtIndexedSubscript(struct NSArrayNSURL *self, NSUInteger idx);

char const *NSURL_fileSystemRepresentation(struct NSURL *self);

struct NSThreadDetachTarget_Class *NSThreadDetachTarget_allocClass(
    char const *classname,
    char const *selectorname,
    void (*_I_NSThreadDetachSelector_)(struct NSThreadDetachTarget *, struct NSThreadDetachSelector_ *, void *argument));

struct NSThreadDetachTarget *NSThreadDetachTarget_alloc(struct NSThreadDetachTarget_Class *);

struct NSThreadDetachTarget *NSThreadDetachTarget_init(struct NSThreadDetachTarget *self);

void NSThreadDetachTarget_release(struct NSThreadDetachTarget *self);

void NSThread_detachNewThreadSelector(char const *selectorname, struct NSThreadDetachTarget *toTarget, void *argument);

bool NSThread_isMultiThreaded();

#endif
