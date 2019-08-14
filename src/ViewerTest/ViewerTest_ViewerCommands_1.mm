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

#if defined(__APPLE__) && !defined(MACOSX_USE_GLX)

#import <Cocoa/Cocoa.h>

#include <Cocoa_Window.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DoubleMap.hxx>

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
                          Standard_Boolean theToUpdate = Standard_True);

extern NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)> ViewerTest_myViews;

// =======================================================================
// function : GetCocoaScreenResolution
// purpose  :
// =======================================================================
void GetCocoaScreenResolution (Standard_Integer& theWidth, Standard_Integer& theHeight)
{
  NSRect aRect = [[NSScreen mainScreen] visibleFrame];
  theWidth = (Standard_Integer )aRect.size.width;
  theHeight = (Standard_Integer )aRect.size.height;
}

// =======================================================================
// function : FindViewId
// purpose  :
// =======================================================================
TCollection_AsciiString FindViewId (const NSWindow* theWindow)
{
  TCollection_AsciiString aViewId = "";
  NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>::Iterator anIter(ViewerTest_myViews);
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

// =======================================================================
// function : ViewerMainLoop
// purpose  :
// =======================================================================
int ViewerMainLoop (Standard_Integer, const char** )
{
  // unused
  return 0;
}

// =======================================================================
// function : ViewerTest_SetCocoaEventManagerView
// purpose  :
// =======================================================================
void ViewerTest_SetCocoaEventManagerView (const Handle(Cocoa_Window)& theWindow)
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
static Graphic3d_Vec2i getMouseCoords (NSView*  theView,
                                       NSEvent* theEvent)
{
  NSPoint aMouseLoc = [theView convertPoint: [theEvent locationInWindow] fromView: nil];
  NSRect  aBounds   = [theView bounds];
  return Graphic3d_Vec2i (Standard_Integer(aMouseLoc.x),
                          Standard_Integer(aBounds.size.height - aMouseLoc.y));
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

// =======================================================================
// function : setFrameSize
// purpose  :
// =======================================================================
- (void )setFrameSize: (NSSize )theNewSize
{
  [super setFrameSize: theNewSize];
  ViewerTest::CurrentEventManager()->ProcessConfigure();
}

// =======================================================================
// function : drawRect
// purpose  :
// =======================================================================
- (void )drawRect: (NSRect )theDirtyRect
{
  (void )theDirtyRect;
  if (!ViewerTest::CurrentEventManager().IsNull())
  {
    ViewerTest::CurrentEventManager()->ProcessExpose();
  }
}

// =======================================================================
// function : mouseMoved
// purpose  :
// =======================================================================
- (void )mouseMoved: (NSEvent* )theEvent
{
  const Graphic3d_Vec2i  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  const Aspect_VKeyMouse aButtons = ViewerTest::CurrentEventManager()->PressedMouseButtons();
  ViewerTest::CurrentEventManager()->UpdateMousePosition (aPos, aButtons, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

// =======================================================================
// function : acceptsFirstResponder
// purpose  :
// =======================================================================
- (BOOL )acceptsFirstResponder
{
  return YES;
}

// =======================================================================
// function : mouseDown
// purpose  :
// =======================================================================
- (void )mouseDown: (NSEvent* )theEvent
{
  const Graphic3d_Vec2i  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  ViewerTest::CurrentEventManager()->PressMouseButton (aPos, Aspect_VKeyMouse_LeftButton, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

// =======================================================================
// function : mouseUp
// purpose  :
// =======================================================================
- (void )mouseUp: (NSEvent* )theEvent
{
  const Graphic3d_Vec2i  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  ViewerTest::CurrentEventManager()->ReleaseMouseButton (aPos, Aspect_VKeyMouse_LeftButton, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

// =======================================================================
// function : mouseDragged
// purpose  :
// =======================================================================
- (void )mouseDragged: (NSEvent* )theEvent
{
  const Graphic3d_Vec2i  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  const Aspect_VKeyMouse aButtons = ViewerTest::CurrentEventManager()->PressedMouseButtons();
  ViewerTest::CurrentEventManager()->UpdateMousePosition (aPos, aButtons, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

// =======================================================================
// function : rightMouseDown
// purpose  :
// =======================================================================
- (void )rightMouseDown: (NSEvent* )theEvent
{
  const Graphic3d_Vec2i  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  ViewerTest::CurrentEventManager()->PressMouseButton (aPos, Aspect_VKeyMouse_RightButton, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

// =======================================================================
// function : rightMouseUp
// purpose  :
// =======================================================================
- (void )rightMouseUp: (NSEvent* )theEvent
{
  const Graphic3d_Vec2i  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  ViewerTest::CurrentEventManager()->ReleaseMouseButton (aPos, Aspect_VKeyMouse_RightButton, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

// =======================================================================
// function : rightMouseDragged
// purpose  :
// =======================================================================
- (void )rightMouseDragged: (NSEvent* )theEvent
{
  const Graphic3d_Vec2i  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);
  const Aspect_VKeyMouse aButtons = ViewerTest::CurrentEventManager()->PressedMouseButtons();
  ViewerTest::CurrentEventManager()->UpdateMousePosition (aPos, aButtons, aFlags, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

// =======================================================================
// function : scrollWheel
// purpose  :
// =======================================================================
- (void )scrollWheel: (NSEvent* )theEvent
{
  const Graphic3d_Vec2i  aPos   = getMouseCoords (self, theEvent);
  const Aspect_VKeyFlags aFlags = getMouseKeyFlags (theEvent);

  const Standard_Real aDelta = [theEvent deltaY];
  if (Abs (aDelta) < 0.001)
  {
    // a lot of values near zero can be generated by touchpad
    return;
  }

  ViewerTest::CurrentEventManager()->UpdateMouseScroll (Aspect_ScrollDelta (aPos, aDelta, aFlags));
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
}

// =======================================================================
// function : keyDown
// purpose  :
// =======================================================================
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
  //  const Standard_CString aString = [aStringNs UTF8String];
  //}
}

// =======================================================================
// function : keyUp
// purpose  :
// =======================================================================
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
