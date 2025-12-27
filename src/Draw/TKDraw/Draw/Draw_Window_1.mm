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

#include <Draw_Window.hxx>
#include <Cocoa_LocalPool.hxx>

#if !defined(MAC_OS_X_VERSION_10_12) || (MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_12)
  // replacements for macOS versions before 10.12
  #define NSEventTypeLeftMouseDown    NSLeftMouseDown
  #define NSEventTypeRightMouseDown   NSRightMouseDown
  #define NSEventTypeLeftMouseDragged NSLeftMouseDragged
  #define NSEventTypeMouseMoved       NSMouseMoved

  #define NSEventMaskLeftMouseDragged NSLeftMouseDraggedMask
  #define NSEventMaskMouseMoved       NSMouseMovedMask
  #define NSEventMaskLeftMouseDown    NSLeftMouseDownMask
  #define NSEventMaskRightMouseDown   NSRightMouseDownMask

  #define NSWindowStyleMaskResizable  NSResizableWindowMask
  #define NSWindowStyleMaskClosable   NSClosableWindowMask
  #define NSWindowStyleMaskTitled     NSTitledWindowMask

  #define NSCompositingOperationSourceOver NSCompositeSourceOver
#endif
#if !defined(MAC_OS_X_VERSION_10_14) || (MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_14)
  #define NSBitmapImageFileTypePNG  NSPNGFileType
  #define NSBitmapImageFileTypeBMP  NSBMPFileType
  #define NSBitmapImageFileTypeJPEG NSJPEGFileType
  #define NSBitmapImageFileTypeGIF  NSGIFFileType
#endif

@interface Draw_CocoaView : NSView
{
  NSImage* myImage;
}

- (void )setImage: (NSImage* )theImage;
- (void )redraw;
@end

@implementation Draw_CocoaView

- (void )setImage: (NSImage* )theImage
{
  [theImage retain];
  [myImage release];
  myImage = theImage;
}

- (BOOL )isFlipped
{
  return YES; // for drawing image from left-top corner
}

- (void )redraw
{
  [self setNeedsDisplay: YES];
}

- (void )drawRect: (NSRect )theRect
{
  (void )theRect;
  NSRect aBounds = NSMakeRect (0.0, 0.0, myImage.size.width, myImage.size.height);

  [myImage drawInRect: aBounds
             fromRect: NSZeroRect
            operation: NSCompositingOperationSourceOver
             fraction: 1
       respectFlipped: YES
                hints: nil];
}

- (void )dealloc
{
  [myImage release];
  [super dealloc];
}
@end

static int getScreenBottom()
{
  NSRect aRect = [[[NSScreen screens] objectAtIndex:0] frame];
  int aScreenBottom = int(aRect.size.height + aRect.origin.y);
  return aScreenBottom;
}

extern bool Draw_VirtualWindows;
static bool Draw_IsInZoomingMode = false;

double Draw_RGBColorsArray[MAXCOLOR][3] = {{1.0,  1.0,  1.0},
                                                  {1.0,  0.0,  0.0},
                                                  {0.0,  1.0,  0.0},
                                                  {0.0,  0.0,  1.0},
                                                  {0.0,  1.0,  1.0},
                                                  {1.0,  0.84, 0.0},
                                                  {1.0,  0.0,  1.0},
                                                  {1.0,  0.2,  0.7},
                                                  {1.0,  0.65, 0.0},
                                                  {1.0,  0.89, 0.88},
                                                  {1.0,  0.63, 0.48},
                                                  {0.78, 0.08, 0.52},
                                                  {1.0,  1.0,  0.0},
                                                  {0.94, 0.9,  0.55},
                                                  {1.0,  0.5,  0.31}};

//=================================================================================================

Draw_Window::Draw_Window (const char* theTitle,
                          const NCollection_Vec2<int>& theXY,
                          const NCollection_Vec2<int>& theSize,
                          Aspect_Drawable theParent,
                          Aspect_Drawable theWindow)
: myWindow (NULL),
  myView (NULL),
  myImageBuffer (NULL),
  myCurrentColor (0),
  myUseBuffer (false)
{
  (void )theParent;
  if (theWindow != 0)
  {
    myWindow = [(NSWindow* )theWindow retain];
  }
  init (theXY, theSize);
  SetTitle (theTitle);
}

//=======================================================================
//function : ~Draw_Window
//purpose  :
//=======================================================================
Draw_Window::~Draw_Window()
{
  if (myWindow != NULL)
  { 
    [myWindow release];
    myWindow = NULL;
  }

  if (myView != NULL)
  {
    [myView release];
    myView = NULL;
  }

  if (myImageBuffer != NULL)
  {
    [myImageBuffer release];
    myImageBuffer = NULL;
  }
}

//=================================================================================================

void Draw_Window::init (const NCollection_Vec2<int>& theXY,
                        const NCollection_Vec2<int>& theSize)
{
  Cocoa_LocalPool aLocalPool;

  // converting left-bottom coordinate to left-top coordinate
  int anYTop = getScreenBottom() - theXY.y() - theSize.y();

  if (myWindow == NULL)
  {
    NSRect     aRectNs   = NSMakeRect (theXY.x(), anYTop, theSize.x(), theSize.y());
    NSUInteger aWinStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;

    myWindow = [[NSWindow alloc] initWithContentRect: aRectNs
                                           styleMask: aWinStyle
                                             backing: NSBackingStoreBuffered
                                               defer: NO];
  }

  if (myView == NULL)
  {
    NSRect aBounds = [[myWindow contentView] bounds];
    
    myView = [[Draw_CocoaView alloc] initWithFrame: aBounds];
    [myWindow setContentView: myView];
  }

  if (myImageBuffer == NULL)
  {
    NSRect aRectNs = [myView bounds];
    myImageBuffer  = [[NSImage alloc] initWithSize: aRectNs.size];
  }

  [myView setImage: myImageBuffer];

  myUseBuffer = Draw_VirtualWindows;

  myCurrentColor = 3;

  [myWindow setBackgroundColor: NSColor.blackColor];
  [myWindow setReleasedWhenClosed: NO];
}

//=================================================================================================

void Draw_Window::InitBuffer()
{
  //
}

//=================================================================================================

void Draw_Window::SetPosition (int theNewXpos,
                               int theNewYpos)
{
  NSPoint aNewPosition = NSMakePoint (theNewXpos, theNewYpos);
  [myWindow setFrameTopLeftPoint: aNewPosition];
}

//=================================================================================================

void Draw_Window::SetDimension (int theNewWidth,
                                int theNewHeight)
{
  NSRect aWindowRect = [myWindow frame];
  int aNewY = aWindowRect.origin.y + aWindowRect.size.height - theNewHeight;
  NSRect aNewContentRect = NSMakeRect (aWindowRect.origin.x, aNewY,
                                       theNewWidth, theNewHeight);
  [myWindow setFrame: aNewContentRect display: YES];
}

//=================================================================================================

void Draw_Window::GetPosition (int &thePosX,
                               int &thePosY)
{
  NSRect aWindowRect = [myWindow frame];
  thePosX = aWindowRect.origin.x;
  thePosY = getScreenBottom() - aWindowRect.origin.y - aWindowRect.size.height;
}

//=================================================================================================

int Draw_Window::HeightWin() const
{
  NSRect aViewBounds = [myView bounds];
  return aViewBounds.size.height;
}

//=================================================================================================

int Draw_Window::WidthWin() const
{
  NSRect aViewBounds = [myView bounds];
  return aViewBounds.size.width;
}

//=================================================================================================

void Draw_Window::SetTitle (const TCollection_AsciiString& theTitle)
{
  NSString* aTitleNs = [[NSString alloc] initWithUTF8String: theTitle.ToCString()];
  [myWindow setTitle: aTitleNs];
  [aTitleNs release];
}

//=================================================================================================

TCollection_AsciiString Draw_Window::GetTitle() const
{
  const char* aTitle = [[myWindow title] UTF8String];
  return TCollection_AsciiString (aTitle);
}

//=================================================================================================

bool Draw_Window::DefineColor (const int , const char* )
{
  return true; // unused
}

//=================================================================================================

bool Draw_Window::IsMapped() const
{
  if (Draw_VirtualWindows
   || myWindow == NULL)
  {
    return false;
  }

  return [myWindow isVisible];
}

//=================================================================================================

void Draw_Window::DisplayWindow()
{
  if (Draw_VirtualWindows)
  {
    return;
  }

  if (myWindow != NULL)
  {
    [myWindow orderFront: NULL];
  }
}

//=================================================================================================

void Draw_Window::Hide()
{
  if (myWindow != NULL)
  {
    [myWindow orderOut: NULL];
  }
}

//=================================================================================================

void Draw_Window::Destroy()
{  
  if (myWindow != NULL)
  { 
    [myWindow release];
    myWindow = NULL;
  }

  if (myView != NULL)
  {
    [myView release];
    myView = NULL;
  }

  if (myImageBuffer != NULL)
  {
    [myImageBuffer release];
    myImageBuffer = NULL;
  }
}

//=================================================================================================

void Draw_Window::Clear()
{
  [myImageBuffer lockFocus];
  [[NSColor blackColor] set];
  NSRect anImageBounds = NSMakeRect (0.0, 0.0, myImageBuffer.size.width, myImageBuffer.size.height);
  NSRectFill (anImageBounds);
  [myImageBuffer unlockFocus];

  if (!myUseBuffer)
  {
    [myView redraw];
  }
}

//=================================================================================================

void Draw_Window::Flush()
{
  //
}

//=================================================================================================

void Draw_Window::DrawString (int theXLeft, int theYTop,
                              const char* theText)
{
  Cocoa_LocalPool aLocalPool;

  NSString* aTextNs = [[[NSString alloc] initWithUTF8String: theText] autorelease];
  NSColor*  aColor  = [NSColor colorWithDeviceRed: Draw_RGBColorsArray[myCurrentColor][0]
                                            green: Draw_RGBColorsArray[myCurrentColor][1]
                                             blue: Draw_RGBColorsArray[myCurrentColor][2]
                                            alpha: 1.0f];
  NSDictionary* anAttributes = [[[NSDictionary alloc] initWithObjectsAndKeys: aColor, NSForegroundColorAttributeName, nil] autorelease];

  [myImageBuffer lockFocus];
  [aTextNs drawAtPoint: NSMakePoint (theXLeft, myImageBuffer.size.height - theYTop) withAttributes: anAttributes];
  [myImageBuffer unlockFocus];

  if (!myUseBuffer)
  {
    [myView redraw];
  }
}

//=================================================================================================

void Draw_Window::DrawSegments (const Draw_XSegment* theSegments,
                                int theNumberOfElements)
{
  Cocoa_LocalPool aLocalPool;

  NSBezierPath* aPath = [[[NSBezierPath alloc] init] autorelease];

  NSImage* anImage;
  int anIter = 0;
  
  if (Draw_IsInZoomingMode)
  {
    // workaround for rectangle drawing when zooming
    anImage = [[myImageBuffer copy] autorelease];
    anIter  = 4;
  }
  else
  {
    anImage = myImageBuffer;
  }


  for (; anIter < theNumberOfElements; ++anIter)
  {
    const Draw_XSegment& aSeg = theSegments[anIter];
    NSPoint aPoint = NSMakePoint (aSeg[0].x(), myImageBuffer.size.height - aSeg[0].y());
    [aPath moveToPoint: aPoint];
    aPoint = NSMakePoint (aSeg[1].x(), myImageBuffer.size.height - aSeg[1].y());
    [aPath lineToPoint: aPoint];
  }

  [anImage lockFocus];
  NSColor* aColor = [NSColor colorWithDeviceRed: Draw_RGBColorsArray[myCurrentColor][0]
                                          green: Draw_RGBColorsArray[myCurrentColor][1]
                                           blue: Draw_RGBColorsArray[myCurrentColor][2]
                                          alpha: 1.0f];
  [aColor set];
  [aPath stroke];
  [anImage unlockFocus];

  if (!myUseBuffer)
  {
    [myView setImage: anImage];
    [myView redraw];
  }
  
  Draw_IsInZoomingMode = false;
}

//=================================================================================================

void Draw_Window::Redraw()
{
  if (myUseBuffer)
  {
    [myView redraw];
  }
}

//=================================================================================================

void Draw_Window::SetColor (int theColor)
{
  myCurrentColor = theColor;
}

//=================================================================================================

void Draw_Window::SetMode (int theMode)
{
  // unsupported
  (void )theMode;
}

//=================================================================================================

bool Draw_Window::Save (const char* theFileName) const
{
  Cocoa_LocalPool aLocalPool;

  NSString* aFileName = [[[NSString alloc] initWithUTF8String: theFileName] autorelease];
  NSString* aFileExtension = [[aFileName pathExtension] lowercaseString];

  NSDictionary* aFileTypeDict = [NSDictionary dictionaryWithObjectsAndKeys:
                                  [NSNumber numberWithInt: NSBitmapImageFileTypePNG],  @"png",
                                  [NSNumber numberWithInt: NSBitmapImageFileTypeBMP],  @"bmp",
                                  [NSNumber numberWithInt: NSBitmapImageFileTypeJPEG], @"jpg",
                                  [NSNumber numberWithInt: NSBitmapImageFileTypeGIF],  @"gif",
                                  nil];
  if ([aFileTypeDict valueForKey: aFileExtension] == NULL)
  {
    return false; // unsupported image extension
  }

  NSBitmapImageFileType aFileType = (NSBitmapImageFileType )[[aFileTypeDict valueForKey: aFileExtension] intValue];
  NSBitmapImageRep* anImageRep = [NSBitmapImageRep imageRepWithData: [myImageBuffer TIFFRepresentation]];

  NSDictionary* anImgProps = [NSDictionary dictionaryWithObject: [NSNumber numberWithFloat: 0.8]
                                                         forKey: NSImageCompressionFactor];

  NSData* aData = [anImageRep representationUsingType: aFileType 
                                           properties: anImgProps];

  bool isSuccess = [aData writeToFile: aFileName
                                       atomically: NO];

  return isSuccess;
}

bool Draw_Window::IsEqualWindows (const long theWindowNumber)
{
  return ([myWindow windowNumber] == theWindowNumber);
}

void Draw_Window::GetNextEvent (bool  theWait,
                                long&             theWindowNumber,
                                int& theX,
                                int& theY,
                                int& theButton)
{
  Cocoa_LocalPool aLocalPool;

  unsigned int anEventMatchMask = NSEventMaskLeftMouseDown | NSEventMaskRightMouseDown;

  if (!theWait)
  {
    anEventMatchMask = anEventMatchMask | NSEventMaskMouseMoved | NSEventMaskLeftMouseDragged;
    Draw_IsInZoomingMode = true;
  }

  NSEvent* anEvent = [NSApp nextEventMatchingMask: anEventMatchMask
                                        untilDate: [NSDate distantFuture]
                                           inMode: NSEventTrackingRunLoopMode
                                          dequeue: YES];

  NSWindow* aWindow = [anEvent window];
  NSView*   aView   = [aWindow contentView];
  theWindowNumber   = [aWindow windowNumber];

  NSPoint aMouseLoc = [aView convertPoint: [anEvent locationInWindow] fromView: nil];

  theX = int (aMouseLoc.x);
  theY = int (aMouseLoc.y);

  NSEventType anEventType = [anEvent type];

  if (anEventType == NSEventTypeLeftMouseDown)
  {
    theButton = 1;
  }
  else if (anEventType == NSEventTypeRightMouseDown)
  {
    theButton = 3;
  }
  else if ((anEventType == NSEventTypeMouseMoved || anEventType == NSEventTypeLeftMouseDragged) && !theWait)
  {
    theButton = 0;
  }
}

#endif // __APPLE__
