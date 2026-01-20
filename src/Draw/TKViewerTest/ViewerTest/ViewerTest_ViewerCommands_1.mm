// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#if defined(__APPLE__) && !defined(HAVE_XLIB)

#import <Cocoa/Cocoa.h>

#include <Cocoa_Window.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DoubleMap.hxx>

#include <cmath>

#if !defined(MAC_OS_X_VERSION_10_12) || (MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_12)
  // replacements for macOS versions before 10.12
  #define NSEventModifierFlagControl  NSControlKeyMask
  #define NSEventModifierFlagShift    NSShiftKeyMask
  #define NSEventModifierFlagOption   NSAlternateKeyMask
  #define NSEventModifierFlagCommand  NSCommandKeyMask
  #define NSEventModifierFlagFunction NSFunctionKeyMask
#endif

//! Custom Cocoa view to handle events
@interface ViewerTest_CocoaEventManagerView : NSView
@end

//! Custom Cocoa window delegate to handle window events
@interface Cocoa_WindowController : NSObject <NSWindowDelegate>
@end

extern void ActivateView (const TCollection_AsciiString& theViewName,
                          bool theToUpdate = true);

extern NCollection_DoubleMap <TCollection_AsciiString, occ::handle<V3d_View>> ViewerTest_myViews;

//=================================================================================================

void GetCocoaScreenResolution (int& theWidth, int& theHeight)
{
  NSRect aRect = [[NSScreen mainScreen] visibleFrame];
  theWidth = (int )aRect.size.width;
  theHeight = (int )aRect.size.height;
}

//=================================================================================================

TCollection_AsciiString FindViewId (const NSWindow* theWindow)
{
  TCollection_AsciiString aViewId = "";
  NCollection_DoubleMap<TCollection_AsciiString, occ::handle<V3d_View>>::Iterator anIter(ViewerTest_myViews);
  for (;anIter.More();anIter.Next())
  {
    NSView* aView = Handle(Cocoa_Window)::DownCast
                   (anIter.Value()->Window())->HView();
    NSWindow* aWindow = [aView window];
    if (aWindow == theWindow)
    {
      aViewId = anIter.Key1();
      return aViewId;
    }
  }
  return aViewId;
}

@implementation Cocoa_WindowController

- (void )windowWillClose: (NSNotification* )theNotification
{
  (void )theNotification;
  TCollection_AsciiString aViewId = "";
  if (ViewerTest_myViews.IsBound2 (ViewerTest::CurrentView()))
  {
    aViewId = ViewerTest_myViews.Find2 (ViewerTest::CurrentView());
  }
  ViewerTest::RemoveView (aViewId);
}

- (void )windowDidBecomeKey: (NSNotification* )theNotification
{
  NSWindow *aWindow = [theNotification object];
  ActivateView (FindViewId (aWindow));
}

@end

//=================================================================================================

int ViewerMainLoop (int, const char** )
{
  // unused
  return 0;
}

//=================================================================================================

void ViewerTest_SetCocoaEventManagerView (const occ::handle<Cocoa_Window>& theWindow)
{
  if (theWindow.IsNull())
  {
    return;
  }

  NSWindow* aWin = [theWindow->HView() window];
  NSRect aBounds = [[aWin contentView] bounds];

  ViewerTest_CocoaEventManagerView* aView = [[ViewerTest_CocoaEventManagerView alloc] initWithFrame: aBounds];

  // replace content view in the window
  theWindow->SetHView (aView);

  // set delegate for window
  Cocoa_WindowController* aWindowController = [[[Cocoa_WindowController alloc] init] autorelease];
  [aWin setDelegate: aWindowController];
  
  // make view as first responder in winow to capture all useful events
  [aWin makeFirstResponder: aView];
  [aWin setAcceptsMouseMovedEvents: YES];

  // should be retained by parent NSWindow
  [aView release];
}

//! Retrieve cursor position
static NCollection_Vec2<int> getMouseCoords (NSView*  theView,
                                       NSEvent* theEvent)
{
  NSPoint aMouseLoc = [theView convertPoint: [theEvent locationInWindow] fromView: nil];
  NSRect  aBounds   = [theView bounds];
  return NCollection_Vec2<int> (int(aMouseLoc.x),
                          int(aBounds.size.height - aMouseLoc.y));
}

//! Convert key flags from mouse event.
static Aspect_VKeyFlags getMouseKeyFlags (NSEvent* theEvent)
{
  Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
  if (([theEvent modifierFlags] & NSEventModifierFlagShift) != 0)
  {
    aFlags |= Aspect_VKeyFlags_SHIFT;
  }
  if (([theEvent modifierFlags] & NSEventModifierFlagControl) != 0)
  {
    aFlags |= Aspect_VKeyFlags_CTRL;
  }
  if (([theEvent modifierFlags] & NSEventModifierFlagOption) != 0)
  {
    aFlags |= Aspect_VKeyFlags_ALT;
  }
  if (([theEvent modifierFlags] & NSEventModifierFlagFunction) != 0)
  {
    //aFlags |= Aspect_VKeyFlags_FUNC;
  }
  if (([theEvent modifierFlags] & NSEventModifierFlagCommand) != 0)
  {
    //aFlags |= Aspect_VKeyFlags_CMD;
  }
  return aFlags;
}

@implementation ViewerTest_CocoaEventManagerView

//=================================================================================================

- (void )setFrameSize: (NSSize )theNewSize
{
  [super setFrameSize: theNewSize];
  ViewerTest::CurrentEventManager()->ProcessConfigure();
}

//=================================================================================================

- (void )drawRect: (NSRect )theDirtyRect
{
  (void )theDirtyRect;
  if (!ViewerTest::CurrentEventManager().IsNull())
  {
    ViewerTest::CurrentEventManager()->ProcessExpose();
  }
}

//=================================================================================================

- (void )mouseMoved: (NSEvent* )theEvent
{
  const NCollection_Vec2<int>  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  const Aspect_VKeyMouse aButtons = ViewerTest::CurrentEventManager()->PressedMouseButtons();
  ViewerTest::CurrentEventManager()->UpdateMousePosition (aPos, aButtons, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

//=================================================================================================

- (BOOL )acceptsFirstResponder
{
  return YES;
}

//=================================================================================================

- (void )mouseDown: (NSEvent* )theEvent
{
  const NCollection_Vec2<int>  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  ViewerTest::CurrentEventManager()->PressMouseButton (aPos, Aspect_VKeyMouse_LeftButton, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

//=================================================================================================

- (void )mouseUp: (NSEvent* )theEvent
{
  const NCollection_Vec2<int>  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  ViewerTest::CurrentEventManager()->ReleaseMouseButton (aPos, Aspect_VKeyMouse_LeftButton, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

//=================================================================================================

- (void )mouseDragged: (NSEvent* )theEvent
{
  const NCollection_Vec2<int>  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  const Aspect_VKeyMouse aButtons = ViewerTest::CurrentEventManager()->PressedMouseButtons();
  ViewerTest::CurrentEventManager()->UpdateMousePosition (aPos, aButtons, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

//=================================================================================================

- (void )rightMouseDown: (NSEvent* )theEvent
{
  const NCollection_Vec2<int>  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  ViewerTest::CurrentEventManager()->PressMouseButton (aPos, Aspect_VKeyMouse_RightButton, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

//=================================================================================================

- (void )rightMouseUp: (NSEvent* )theEvent
{
  const NCollection_Vec2<int>  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  ViewerTest::CurrentEventManager()->ReleaseMouseButton (aPos, Aspect_VKeyMouse_RightButton, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

//=================================================================================================

- (void )rightMouseDragged: (NSEvent* )theEvent
{
  const NCollection_Vec2<int>  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  const Aspect_VKeyMouse aButtons = ViewerTest::CurrentEventManager()->PressedMouseButtons();
  ViewerTest::CurrentEventManager()->UpdateMousePosition (aPos, aButtons, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

//=================================================================================================

- (void )scrollWheel: (NSEvent* )theEvent
{
  const NCollection_Vec2<int>  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);

  const double aDelta = [theEvent deltaY];
  if (std::abs(aDelta) < 0.001)
  {
    // a lot of values near zero can be generated by touchpad
    return;
  }

  ViewerTest::CurrentEventManager()->UpdateMouseScroll (Aspect_ScrollDelta (aPos, aDelta, aFlags));
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

//=================================================================================================

- (void )keyDown: (NSEvent* )theEvent
{
  unsigned int aKeyCode = [theEvent keyCode];
  const Aspect_VKey aVKey = Cocoa_Window::VirtualKeyFromNative (aKeyCode);
  if (aVKey != Aspect_VKey_UNKNOWN)
  {
    const double aTimeStamp = [theEvent timestamp];
    ViewerTest::CurrentEventManager()->KeyDown (aVKey, aTimeStamp);
    ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
  }

  //NSString* aStringNs = [theEvent characters];
  //if (aStringNs != NULL && [aStringNs length] != 1)
  //{
  //  const char* const aString = [aStringNs UTF8String];
  //}
}

//=================================================================================================

- (void )keyUp: (NSEvent* )theEvent
{
  unsigned int aKeyCode = [theEvent keyCode];
  const Aspect_VKey aVKey = Cocoa_Window::VirtualKeyFromNative (aKeyCode);
  if (aVKey != Aspect_VKey_UNKNOWN)
  {
    const double aTimeStamp = [theEvent timestamp];
    ViewerTest::CurrentEventManager()->KeyUp (aVKey, aTimeStamp);
    ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
  }
}

@end

#endif
