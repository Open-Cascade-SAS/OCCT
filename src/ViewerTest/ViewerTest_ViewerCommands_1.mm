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
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DoubleMap.hxx>

//! Custom Cocoa view to handle events
@interface ViewerTest_CocoaEventManagerView : NSView
@end

//! Custom Cocoa window delegate to handle window events
@interface Cocoa_WindowController : NSObject <NSWindowDelegate>
@end

extern void ActivateView (const TCollection_AsciiString& theViewName);
extern void VT_ProcessExpose();
extern void VT_ProcessConfigure();
extern void VT_ProcessKeyPress (const char* theBuffer);
extern void VT_ProcessMotion();
extern void VT_ProcessButton3Press();
extern void VT_ProcessButton3Release();
extern void VT_ProcessControlButton2Motion();
extern void VT_ProcessControlButton3Motion();
extern Standard_Boolean VT_ProcessButton1Press (Standard_Integer theArgsNb,
                                                const char**     theArgsVec,
                                                Standard_Boolean theToPick,
                                                Standard_Boolean theIsShift);
extern void VT_ProcessButton1Release(Standard_Boolean theIsShift);

extern NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)> ViewerTest_myViews;
extern int X_Motion; // Current cursor position
extern int Y_Motion;
extern int X_ButtonPress; // Last ButtonPress position
extern int Y_ButtonPress;
extern Standard_Boolean IsDragged;

// =======================================================================
// function : SetCocoaWindowTitle
// purpose  :
// =======================================================================
void SetCocoaWindowTitle (const Handle(Cocoa_Window)& theWindow, Standard_CString theTitle)
{
  NSView* aView = theWindow->HView();
  NSWindow* aWindow = [aView window];

  NSString* aTitleNS = [[NSString alloc] initWithUTF8String: theTitle];
  [aWindow setTitle: aTitleNS];
  [aTitleNS release];
  
}

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

// =======================================================================
// function : getMouseCoords
// purpose  : Retrieve cursor position
// =======================================================================
static void getMouseCoords (NSView*           theView,
                            NSEvent*          theEvent,
                            Standard_Integer& theX,
                            Standard_Integer& theY)
{
  NSPoint aMouseLoc = [theView convertPoint: [theEvent locationInWindow] fromView: nil];
  NSRect  aBounds   = [theView bounds];

  theX = Standard_Integer(aMouseLoc.x);
  theY = Standard_Integer(aBounds.size.height - aMouseLoc.y);
}

@implementation ViewerTest_CocoaEventManagerView

// =======================================================================
// function : setFrameSize
// purpose  :
// =======================================================================
- (void )setFrameSize: (NSSize )theNewSize
{
  [super setFrameSize: theNewSize];
  VT_ProcessConfigure();
}

// =======================================================================
// function : drawRect
// purpose  :
// =======================================================================
- (void )drawRect: (NSRect )theDirtyRect
{
  (void )theDirtyRect;
  VT_ProcessExpose();
}

// =======================================================================
// function : mouseMoved
// purpose  :
// =======================================================================
- (void )mouseMoved: (NSEvent* )theEvent
{
  getMouseCoords (self, theEvent, X_Motion, Y_Motion);
  VT_ProcessMotion();
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
  getMouseCoords (self, theEvent, X_ButtonPress, Y_ButtonPress);
  VT_ProcessButton1Press (0, NULL, Standard_False, [theEvent modifierFlags] & NSShiftKeyMask);
}

// =======================================================================
// function : mouseUp
// purpose  :
// =======================================================================
- (void )mouseUp: (NSEvent* )theEvent
{
  getMouseCoords (self, theEvent, X_Motion, Y_Motion);
  VT_ProcessButton1Release([theEvent modifierFlags] & NSShiftKeyMask);
}


// =======================================================================
// function : mouseDragged
// purpose  :
// =======================================================================
- (void )mouseDragged: (NSEvent* )theEvent
{
  IsDragged = Standard_True;
  if ([theEvent modifierFlags] & NSControlKeyMask)
  {
    getMouseCoords (self, theEvent, X_Motion, Y_Motion);
    VT_ProcessControlButton2Motion();
  }
}

// =======================================================================
// function : rightMouseDown
// purpose  :
// =======================================================================
- (void )rightMouseDown: (NSEvent* )theEvent
{
  getMouseCoords (self, theEvent, X_ButtonPress, Y_ButtonPress);
  VT_ProcessButton3Press(); // Start rotation
}

// =======================================================================
// function : rightMouseUp
// purpose  :
// =======================================================================
- (void )rightMouseUp: (NSEvent* )theEvent
{
  (void )theEvent;
  VT_ProcessButton3Release();
}

// =======================================================================
// function : rightMouseDragged
// purpose  :
// =======================================================================
- (void )rightMouseDragged: (NSEvent* )theEvent
{
  if ([theEvent modifierFlags] & NSControlKeyMask)
  {
    getMouseCoords (self, theEvent, X_Motion, Y_Motion);
    VT_ProcessControlButton3Motion();
  }
}

// =======================================================================
// function : scrollWheel
// purpose  :
// =======================================================================
- (void )scrollWheel: (NSEvent* )theEvent
{
  float aDelta = [theEvent deltaY];
  if (Abs (aDelta) < 0.001)
  {
    // a lot of values near zero can be generated by touchpad
    return;
  }

  ViewerTest::CurrentView()->Zoom (0, 0, aDelta, aDelta);
}

// =======================================================================
// function : keyDown
// purpose  :
// =======================================================================
- (void )keyDown: (NSEvent* )theEvent
{
  NSString* aStringNs = [theEvent characters];
  if (aStringNs == NULL || [aStringNs length] == 0)
  {
    return;
  }

  const Standard_CString aString = [aStringNs UTF8String];
  VT_ProcessKeyPress (aString);
}

@end

#endif
