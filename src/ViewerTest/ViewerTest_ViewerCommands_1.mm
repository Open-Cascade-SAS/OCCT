// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#if defined(__APPLE__) && !defined(MACOSX_USE_GLX)

#import <Cocoa/Cocoa.h>

#include <Cocoa_Window.hxx>
#include <ViewerTest.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <NIS_View.hxx>

//! Custom Cocoa view to handle events
@interface ViewerTest_CocoaEventManagerView : NSView
@end

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

extern int X_Motion; // Current cursor position
extern int Y_Motion;
extern int X_ButtonPress; // Last ButtonPress position
extern int Y_ButtonPress;
extern Standard_Boolean IsDragged;

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
