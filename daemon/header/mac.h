#ifndef MAC_H
#define MAC_H

#include <CoreGraphics/CGEvent.h>
#include <CoreGraphics/CGEventTypes.h>
#include <CoreGraphics/CGEventSource.h>
#include <CoreFoundation/CFRunLoop.h>
#include <CoreFoundation/CoreFoundation.h>

#define _kCGEventTapOptionDefault 0 // for Mac OS X v10.4 support
#define EVENT_MASK ( \
    CGEventMaskBit(kCGEventKeyDown) | \
    CGEventMaskBit(kCGEventKeyUp) | \
    CGEventMaskBit(kCGEventFlagsChanged) \
)

#endif // MAC_H
