// Created on: 2012-11-12
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#import <TargetConditionals.h>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  #import <UIKit/UIKit.h>
#else
  #import <Cocoa/Cocoa.h>
#endif

#include <Cocoa_Window.hxx>

#include <Cocoa_LocalPool.hxx>

#include <Image_AlienPixMap.hxx>
#include <Aspect_Convert.hxx>
#include <Aspect_WindowDefinitionError.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Cocoa_Window,Aspect_Window)

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  //
#else

#if !defined(MAC_OS_X_VERSION_10_12) || (MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_12)
  // replacements for macOS versions before 10.12
  #define NSWindowStyleMaskResizable NSResizableWindowMask
  #define NSWindowStyleMaskClosable  NSClosableWindowMask
  #define NSWindowStyleMaskTitled    NSTitledWindowMask
#endif

static Standard_Integer getScreenBottom()
{
  Cocoa_LocalPool aLocalPool;
  NSArray* aScreens = [NSScreen screens];
  if (aScreens == NULL || [aScreens count] == 0)
  {
    return 0;
  }

  NSScreen* aScreen = (NSScreen* )[aScreens objectAtIndex: 0];
  NSDictionary* aDict = [aScreen deviceDescription];
  NSNumber* aNumber = [aDict objectForKey: @"NSScreenNumber"];
  if (aNumber == NULL
  || [aNumber isKindOfClass: [NSNumber class]] == NO)
  {
    return 0;
  }

  CGDirectDisplayID aDispId = [aNumber unsignedIntValue];
  CGRect aRect = CGDisplayBounds(aDispId);
  return Standard_Integer(aRect.origin.y + aRect.size.height);
}
#endif

//! Extension for Cocoa_Window::InvalidateContent().
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  @interface UIView (UIViewOcctAdditions)
  - (void )invalidateContentOcct: (id )theSender;
  @end
  @implementation UIView (UIViewOcctAdditions)
  - (void )invalidateContentOcct: (id )theSender
  {
    (void )theSender;
    [self setNeedsDisplay];
  }
  @end
#else
  @interface NSView (NSViewOcctAdditions)
  - (void )invalidateContentOcct: (id )theSender;
  @end
  @implementation NSView (NSViewOcctAdditions)
  - (void )invalidateContentOcct: (id )theSender
  {
    (void )theSender;
    [self setNeedsDisplay: YES];
  }
  @end
#endif

// =======================================================================
// function : Cocoa_Window
// purpose  :
// =======================================================================
Cocoa_Window::Cocoa_Window (const Standard_CString theTitle,
                            const Standard_Integer thePxLeft,
                            const Standard_Integer thePxTop,
                            const Standard_Integer thePxWidth,
                            const Standard_Integer thePxHeight)
: Aspect_Window (),
#if !(defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
  myHWindow (NULL),
#endif
  myHView   (NULL),
  myXLeft   (thePxLeft),
  myYTop    (thePxTop),
  myXRight  (thePxLeft + thePxWidth),
  myYBottom (thePxTop + thePxHeight)
{
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  //
#else
  if (thePxWidth <= 0 || thePxHeight <= 0)
  {
    throw Aspect_WindowDefinitionError("Coordinate(s) out of range");
  }
  else if (NSApp == NULL)
  {
    throw Aspect_WindowDefinitionError("Cocoa application should be instantiated before window");
    return;
  }

  // convert top-bottom coordinates to bottom-top (Cocoa)
  myYTop    = getScreenBottom() - myYBottom;
  myYBottom = myYTop + thePxHeight;

  Cocoa_LocalPool aLocalPool;
  NSUInteger aWinStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;
  NSRect aRectNs = NSMakeRect (float(myXLeft), float(myYTop), float(thePxWidth), float(thePxHeight));
  myHWindow = [[NSWindow alloc] initWithContentRect: aRectNs
                                          styleMask: aWinStyle
                                            backing: NSBackingStoreBuffered
                                              defer: NO];
  if (myHWindow == NULL)
  {
    throw Aspect_WindowDefinitionError("Unable to create window");
  }
  // for the moment, OpenGL renderer is expected to output sRGB colorspace
  [myHWindow setColorSpace: [NSColorSpace sRGBColorSpace]];
  myHView = [[myHWindow contentView] retain];

  NSString* aTitleNs = [[NSString alloc] initWithUTF8String: theTitle];
  [myHWindow setTitle: aTitleNs];
  [aTitleNs release];

  // do not destroy NSWindow on close - we didn't handle it!
  [myHWindow setReleasedWhenClosed: NO];
#endif
}

// =======================================================================
// function : Cocoa_Window
// purpose  :
// =======================================================================
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
Cocoa_Window::Cocoa_Window (UIView* theViewNS)
: Aspect_Window(),
#else
Cocoa_Window::Cocoa_Window (NSView* theViewNS)
: Aspect_Window(),
  myHWindow (NULL),
#endif
  myHView   (NULL),
  myXLeft   (0),
  myYTop    (0),
  myXRight  (512),
  myYBottom (512)
{
#if defined(HAVE_OBJC_ARC)
  myHView = theViewNS;
#else
  myHView = [theViewNS retain];
#endif
  DoResize();
}

// =======================================================================
// function : ~Cocoa_Window
// purpose  :
// =======================================================================
Cocoa_Window::~Cocoa_Window()
{
#if !defined(HAVE_OBJC_ARC)
  Cocoa_LocalPool aLocalPool;
#endif
#if !(defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
  if (myHWindow != NULL)
  {
  #if !defined(HAVE_OBJC_ARC)
    //[myHWindow close];
    [myHWindow release];
  #endif
    myHWindow = NULL;
  }
#endif
  if (myHView != NULL)
  {
  #if !defined(HAVE_OBJC_ARC)
    [myHView release];
  #endif
    myHView = NULL;
  }
}

// =======================================================================
// function : SetHView
// purpose  :
// =======================================================================
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
void Cocoa_Window::SetHView (UIView* theView)
{
#else
void Cocoa_Window::SetHView (NSView* theView)
{
  if (myHWindow != NULL)
  {
    [myHWindow setContentView: theView];
  }
#endif

#if defined(HAVE_OBJC_ARC)
  myHView = theView;
#else
  if (myHView != NULL)
  {
    [myHView release];
    myHView = NULL;
  }
  myHView = [theView retain];
#endif
}

// =======================================================================
// function : IsMapped
// purpose  :
// =======================================================================
Standard_Boolean Cocoa_Window::IsMapped() const
{
  if (IsVirtual())
  {
    return Standard_True;
  }

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  return myHView != NULL;
#else
  return myHView != NULL
   &&  [[myHView window] isVisible];
#endif
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void Cocoa_Window::Map() const
{
  if (IsVirtual())
  {
    return;
  }

  if (myHView != NULL)
  {
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    //
  #else
    [[myHView window] orderFront: NULL];
  #endif
  }
}

// =======================================================================
// function : Unmap
// purpose  :
// =======================================================================
void Cocoa_Window::Unmap() const
{
  if (myHView != NULL)
  {
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    //
  #else
    [[myHView window] orderOut: NULL];
  #endif
  }
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize Cocoa_Window::DoResize()
{
  if (myHView == NULL)
  {
    return Aspect_TOR_UNKNOWN;
  }

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  CGRect aBounds = [myHView bounds];
#else
  NSRect aBounds = [myHView bounds];
#endif
  Standard_Integer aMask = 0;
  Aspect_TypeOfResize aMode = Aspect_TOR_UNKNOWN;

  if (Abs ((Standard_Integer )aBounds.origin.x                         - myXLeft  ) > 2) aMask |= 1;
  if (Abs ((Standard_Integer )(aBounds.origin.x + aBounds.size.width)  - myXRight ) > 2) aMask |= 2;
  if (Abs ((Standard_Integer )aBounds.origin.y                         - myYTop   ) > 2) aMask |= 4;
  if (Abs ((Standard_Integer )(aBounds.origin.y + aBounds.size.height) - myYBottom) > 2) aMask |= 8;
  switch (aMask)
  {
    case 0:  aMode = Aspect_TOR_NO_BORDER;               break;
    case 1:  aMode = Aspect_TOR_LEFT_BORDER;             break;
    case 2:  aMode = Aspect_TOR_RIGHT_BORDER;            break;
    case 4:  aMode = Aspect_TOR_TOP_BORDER;              break;
    case 5:  aMode = Aspect_TOR_LEFT_AND_TOP_BORDER;     break;
    case 6:  aMode = Aspect_TOR_TOP_AND_RIGHT_BORDER;    break;
    case 8:  aMode = Aspect_TOR_BOTTOM_BORDER;           break;
    case 9:  aMode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;  break;
    case 10: aMode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER; break;
    default: break;
  }

  myXLeft   = (Standard_Integer )aBounds.origin.x;
  myXRight  = (Standard_Integer )(aBounds.origin.x + aBounds.size.width);
  myYTop    = (Standard_Integer )aBounds.origin.y;
  myYBottom = (Standard_Integer )(aBounds.origin.y + aBounds.size.height);
  return aMode;
}

// =======================================================================
// function : DoMapping
// purpose  :
// =======================================================================
Standard_Boolean Cocoa_Window::DoMapping() const
{
  return Standard_True;
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Standard_Real Cocoa_Window::Ratio() const
{
  if (myHView == NULL)
  {
    return 1.0;
  }

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  CGRect aBounds = [myHView bounds];
#else
  NSRect aBounds = [myHView bounds];
#endif
  return Standard_Real (aBounds.size.width / aBounds.size.height);
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void Cocoa_Window::Position (Standard_Integer& X1, Standard_Integer& Y1,
                             Standard_Integer& X2, Standard_Integer& Y2) const
{
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  CGRect aBounds = [myHView bounds];
  X1 = 0;
  Y1 = 0;
  X2 = (Standard_Integer )aBounds.size.width;
  Y2 = (Standard_Integer )aBounds.size.height;
#else
  NSWindow* aWindow = [myHView window];
  NSRect aWindowRect = [aWindow frame];
  X1 = (Standard_Integer) aWindowRect.origin.x;
  Y1 = getScreenBottom() - (Standard_Integer) aWindowRect.origin.y - (Standard_Integer) aWindowRect.size.height;
  X2 = X1 + (Standard_Integer) aWindowRect.size.width;
  Y2 = Y1 + (Standard_Integer) aWindowRect.size.height;
#endif
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void Cocoa_Window::Size (Standard_Integer& theWidth,
                         Standard_Integer& theHeight) const
{
  if (myHView == NULL)
  {
    return;
  }

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  CGRect aBounds = [myHView bounds];
#else
  NSRect aBounds = [myHView bounds];
#endif
  theWidth  = (Standard_Integer )aBounds.size.width;
  theHeight = (Standard_Integer )aBounds.size.height;
}

// =======================================================================
// function : SetTitle
// purpose  :
// =======================================================================
void Cocoa_Window::SetTitle (const TCollection_AsciiString& theTitle)
{
  if (myHView == NULL)
  {
    return;
  }

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  (void )theTitle;
#else
  NSWindow* aWindow  = [myHView window];
  NSString* aTitleNS = [[NSString alloc] initWithUTF8String: theTitle.ToCString()];
  [aWindow setTitle: aTitleNS];
  [aTitleNS release];
#endif
}

// =======================================================================
// function : InvalidateContent
// purpose  :
// =======================================================================
void Cocoa_Window::InvalidateContent (const Handle(Aspect_DisplayConnection)& )
{
  if (myHView == NULL)
  {
    return;
  }

  if ([NSThread isMainThread])
  {
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    [myHView setNeedsDisplay];
  #else
    [myHView setNeedsDisplay: YES];
  #endif
  }
  else
  {
    [myHView performSelectorOnMainThread: @selector(invalidateContentOcct:)
                              withObject: NULL
                           waitUntilDone: NO];
  }
}

// =======================================================================
// function : VirtualKeyFromNative
// purpose  :
// =======================================================================
Aspect_VKey Cocoa_Window::VirtualKeyFromNative (Standard_Integer theKey)
{
  switch (theKey)
  {
    case 0x00: return Aspect_VKey_A;
    case 0x01: return Aspect_VKey_S;
    case 0x02: return Aspect_VKey_D;
    case 0x03: return Aspect_VKey_F;
    case 0x04: return Aspect_VKey_H;
    case 0x05: return Aspect_VKey_G;
    case 0x06: return Aspect_VKey_Z;
    case 0x07: return Aspect_VKey_X;
    case 0x08: return Aspect_VKey_C;
    case 0x09: return Aspect_VKey_V;
    case 0x0A: return Aspect_VKey_UNKNOWN;
    case 0x0B: return Aspect_VKey_B;
    case 0x0C: return Aspect_VKey_Q;
    case 0x0D: return Aspect_VKey_W;
    case 0x0E: return Aspect_VKey_E;
    case 0x0F: return Aspect_VKey_R;
    case 0x10: return Aspect_VKey_Y;
    case 0x11: return Aspect_VKey_T;
    case 0x12: return Aspect_VKey_1;
    case 0x13: return Aspect_VKey_2;
    case 0x14: return Aspect_VKey_3;
    case 0x15: return Aspect_VKey_4;
    case 0x16: return Aspect_VKey_6;
    case 0x17: return Aspect_VKey_5;
    case 0x18: return Aspect_VKey_Plus;
    case 0x19: return Aspect_VKey_9;
    case 0x1A: return Aspect_VKey_7;
    case 0x1B: return Aspect_VKey_Minus;
    case 0x1C: return Aspect_VKey_8;
    case 0x1D: return Aspect_VKey_0;
    case 0x1E: return Aspect_VKey_BracketRight;
    case 0x1F: return Aspect_VKey_O;
    case 0x20: return Aspect_VKey_U;
    case 0x21: return Aspect_VKey_BracketLeft;
    case 0x22: return Aspect_VKey_I;
    case 0x23: return Aspect_VKey_P;
    case 0x24: return Aspect_VKey_Enter;
    case 0x25: return Aspect_VKey_L;
    case 0x26: return Aspect_VKey_J;
    case 0x27: return Aspect_VKey_Apostrophe;
    case 0x28: return Aspect_VKey_K;
    case 0x29: return Aspect_VKey_Semicolon;
    case 0x2A: return Aspect_VKey_Backslash;
    case 0x2B: return Aspect_VKey_Comma; // 43, ',<'
    case 0x2C: return Aspect_VKey_Slash; //ST_VK_OEM_2, // 44, '?/'
    case 0x2D: return Aspect_VKey_N;
    case 0x2E: return Aspect_VKey_M;
    case 0x2F: return Aspect_VKey_Period; // 47, '.>'
    case 0x30: return Aspect_VKey_Tab;
    case 0x31: return Aspect_VKey_Space;
    case 0x32: return Aspect_VKey_Tilde;  // '~`'
    case 0x33: return Aspect_VKey_Backspace;
    case 0x34: return Aspect_VKey_UNKNOWN;
    case 0x35: return Aspect_VKey_Escape;
    case 0x36: return Aspect_VKey_UNKNOWN; // Aspect_VKey_Cmd, right Command
    case 0x37: return Aspect_VKey_UNKNOWN; // Aspect_VKey_Cmd, left  Command
    case 0x38: return Aspect_VKey_Shift;   // left shift
    case 0x39: return Aspect_VKey_UNKNOWN;
    case 0x3A: return Aspect_VKey_Alt;     // left alt/option
    case 0x3B: return Aspect_VKey_Control;
    case 0x3C: return Aspect_VKey_Shift;   // right shift
    case 0x3D: return Aspect_VKey_Alt;     // right alt/option
    case 0x3E: return Aspect_VKey_UNKNOWN;
    case 0x3F: return Aspect_VKey_UNKNOWN; // Aspect_VKey_Func, fn
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4A:
    case 0x4B: return Aspect_VKey_UNKNOWN;
    case 0x4C: return Aspect_VKey_Enter;   // fn + return
    case 0x4D:
    case 0x4E:
    case 0x4F:
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
    case 0x58:
    case 0x59:
    case 0x5A:
    case 0x5B:
    case 0x5C:
    case 0x5D:
    case 0x5E:
    case 0x5F: return Aspect_VKey_UNKNOWN;
    case 0x60: return Aspect_VKey_F5;
    case 0x61: return Aspect_VKey_F6;
    case 0x62: return Aspect_VKey_F7;
    case 0x63: return Aspect_VKey_F3;
    case 0x64: return Aspect_VKey_F8;
    case 0x65: return Aspect_VKey_F9;
    //case 0x66: return Aspect_VKey_UNKNOWN;
    case 0x67: return Aspect_VKey_F11;
    //case 0x68: return Aspect_VKey_UNKNOWN;
    //case 0x69: return Aspect_VKey_UNKNOWN;
    //case 0x6A: return Aspect_VKey_UNKNOWN;
    //case 0x6B: return Aspect_VKey_UNKNOWN;
    //case 0x6C: return Aspect_VKey_UNKNOWN;
    case 0x6D: return Aspect_VKey_F10;
    //case 0x6E: return Aspect_VKey_UNKNOWN;
    case 0x6F: return Aspect_VKey_F12;
    //case 0x70: return Aspect_VKey_UNKNOWN;
    //case 0x71: return Aspect_VKey_UNKNOWN;
    //case 0x72: return Aspect_VKey_UNKNOWN;
    case 0x73: return Aspect_VKey_Home;
    case 0x74: return Aspect_VKey_PageUp;
    case 0x75: return Aspect_VKey_Delete;
    case 0x76: return Aspect_VKey_F4;
    case 0x77: return Aspect_VKey_End;
    case 0x78: return Aspect_VKey_F2;
    case 0x79: return Aspect_VKey_PageDown;
    case 0x7A: return Aspect_VKey_F1;
    case 0x7B: return Aspect_VKey_Left;
    case 0x7C: return Aspect_VKey_Right;
    case 0x7D: return Aspect_VKey_Down;
    case 0x7E: return Aspect_VKey_Up;
    case 0x7F: return Aspect_VKey_UNKNOWN;
  }
  return Aspect_VKey_UNKNOWN;
}
