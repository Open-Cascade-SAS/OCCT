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

#import <Cocoa/Cocoa.h>

#include <Cocoa_Window.hxx>

#include <Cocoa_LocalPool.hxx>

#include <Image_AlienPixMap.hxx>
#include <Aspect_Convert.hxx>
#include <Aspect_WindowDefinitionError.hxx>

IMPLEMENT_STANDARD_HANDLE (Cocoa_Window, Aspect_Window)
IMPLEMENT_STANDARD_RTTIEXT(Cocoa_Window, Aspect_Window)

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
  myHWindow (NULL),
  myHView   (NULL),
  myXLeft   (thePxLeft),
  myYTop    (thePxTop),
  myXRight  (thePxLeft + thePxWidth),
  myYBottom (thePxTop + thePxHeight)
{
  if (thePxWidth <= 0 || thePxHeight <= 0)
  {
    Aspect_WindowDefinitionError::Raise ("Coordinate(s) out of range");
  }
  else if (NSApp == NULL)
  {
    Aspect_WindowDefinitionError::Raise ("Cocoa application should be instantiated before window");
    return;
  }

  // convert top-bottom coordinates to bottom-top (Cocoa)
  myYTop    = getScreenBottom() - myYBottom;
  myYBottom = myYTop + thePxHeight;

  Cocoa_LocalPool aLocalPool;
  NSUInteger aWinStyle = NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask;
  NSRect aRectNs = NSMakeRect (float(myXLeft), float(myYTop), float(thePxWidth), float(thePxHeight));
  myHWindow = [[NSWindow alloc] initWithContentRect: aRectNs
                                          styleMask: aWinStyle
                                            backing: NSBackingStoreBuffered
                                              defer: NO];
  if (myHWindow == NULL)
  {
    Aspect_WindowDefinitionError::Raise ("Unable to create window");
  }
  myHView = [[myHWindow contentView] retain];

  NSString* aTitleNs = [[NSString alloc] initWithUTF8String: theTitle];
  [myHWindow setTitle: aTitleNs];
  [aTitleNs release];

  // do not destroy NSWindow on close - we didn't handle it!
  [myHWindow setReleasedWhenClosed: NO];
}

// =======================================================================
// function : Cocoa_Window
// purpose  :
// =======================================================================
Cocoa_Window::Cocoa_Window (NSView* theViewNS)
: Aspect_Window (),
  myHWindow (NULL),
  myHView   ([theViewNS retain]),
  myXLeft   (0),
  myYTop    (0),
  myXRight  (512),
  myYBottom (512)
{
  DoResize();
}

// =======================================================================
// function : Destroy
// purpose  :
// =======================================================================
void Cocoa_Window::Destroy()
{
  Cocoa_LocalPool aLocalPool;
  if (myHWindow != NULL)
  {
    //[myHWindow close];
    [myHWindow release];
    myHWindow = NULL;
  }
  if (myHView != NULL)
  {
    [myHView release];
    myHView = NULL;
  }
}

// =======================================================================
// function : HView
// purpose  :
// =======================================================================
NSView* Cocoa_Window::HView() const
{
  return myHView;
}

// =======================================================================
// function : SetHView
// purpose  :
// =======================================================================
void Cocoa_Window::SetHView (NSView* theView)
{
  if (myHWindow != NULL)
  {
    [myHWindow setContentView: theView];
  }
  if (myHView != NULL)
  {
    [myHView release];
    myHView = NULL;
  }
  myHView = [theView retain];
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

  return (myHView != NULL) &&  [[myHView window] isVisible];
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
    [[myHView window] orderFront: NULL];
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
    [[myHView window] orderOut: NULL];
  }
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize Cocoa_Window::DoResize() const
{
  if (myHView == NULL)
  {
    return Aspect_TOR_UNKNOWN;
  }

  NSRect aBounds = [myHView bounds];
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

  *((Standard_Integer* )&myXLeft   ) = (Standard_Integer )aBounds.origin.x;
  *((Standard_Integer* )&myXRight  ) = (Standard_Integer )(aBounds.origin.x + aBounds.size.width);
  *((Standard_Integer* )&myYTop    ) = (Standard_Integer )aBounds.origin.y;
  *((Standard_Integer* )&myYBottom ) = (Standard_Integer )(aBounds.origin.y + aBounds.size.height);
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
Quantity_Ratio Cocoa_Window::Ratio() const
{
  if (myHView == NULL)
  {
    return 1.0;
  }

  NSRect aBounds = [myHView bounds];
  return Quantity_Ratio (aBounds.size.width / aBounds.size.height);
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void Cocoa_Window::Position (Standard_Integer& X1, Standard_Integer& Y1,
                             Standard_Integer& X2, Standard_Integer& Y2) const
{
  NSWindow* aWindow = [myHView window];
  NSRect aWindowRect = [aWindow frame];
  X1 = (Standard_Integer) aWindowRect.origin.x;
  Y1 = getScreenBottom() - (Standard_Integer) aWindowRect.origin.y - (Standard_Integer) aWindowRect.size.height;
  X2 = X1 + (Standard_Integer) aWindowRect.size.width;
  Y2 = Y1 + (Standard_Integer) aWindowRect.size.height;
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

  NSRect aBounds = [myHView bounds];
  theWidth  = (Standard_Integer )aBounds.size.width;
  theHeight = (Standard_Integer )aBounds.size.height;
}
