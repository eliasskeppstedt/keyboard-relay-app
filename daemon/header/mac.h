#ifndef MAC_H
#define MAC_H

#include <Carbon/Carbon.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>

#define _kCGEventTapOptionDefault 0 // for Mac OS X v10.4 support
#define EVENT_MASK ( \
    CGEventMaskBit(kCGEventKeyDown) | \
    CGEventMaskBit(kCGEventKeyUp) | \
    CGEventMaskBit(kCGEventFlagsChanged) \
)

#endif // MAC_H
