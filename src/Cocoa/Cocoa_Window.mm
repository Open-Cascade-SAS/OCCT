// Created on: 2012-11-12
// Created by: Kirill GAVRILOV
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

#import <Cocoa/Cocoa.h>

#include <Cocoa_Window.hxx>

#include <Cocoa_LocalPool.hxx>

#include <Image_AlienPixMap.hxx>
#include <Aspect_Convert.hxx>
#include <Aspect_GraphicDevice.hxx>
#include <Aspect_WindowDefinitionError.hxx>

IMPLEMENT_STANDARD_HANDLE (Cocoa_Window, Aspect_Window)
IMPLEMENT_STANDARD_RTTIEXT(Cocoa_Window, Aspect_Window)

//! Dummy device class implementation
class Cocoa_GraphicDevice : public Aspect_GraphicDevice
{

public:

  virtual Handle_Aspect_GraphicDriver GraphicDriver() const
  {
    return NULL;
  }

  DEFINE_STANDARD_RTTI(Cocoa_GraphicDevice)

};

DEFINE_STANDARD_HANDLE(Cocoa_GraphicDevice, Aspect_GraphicDevice)

IMPLEMENT_STANDARD_HANDLE (Cocoa_GraphicDevice, Aspect_GraphicDevice)
IMPLEMENT_STANDARD_RTTIEXT(Cocoa_GraphicDevice, Aspect_GraphicDevice)

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
: Aspect_Window (new Cocoa_GraphicDevice()),
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
: Aspect_Window (new Cocoa_GraphicDevice()),
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
// function : DoubleBuffer
// purpose  :
// =======================================================================
Standard_Boolean Cocoa_Window::DoubleBuffer() const
{
  return Standard_True;
}

// =======================================================================
// function : SetBackground
// purpose  :
// =======================================================================
void Cocoa_Window::SetBackground (const Aspect_Background& theBackground)
{
  SetBackground (theBackground.Color());
}

// =======================================================================
// function : SetBackground
// purpose  :
// =======================================================================
void Cocoa_Window::SetBackground (const Quantity_NameOfColor theBackColor)
{
  SetBackground (Quantity_Color (theBackColor));
}

// =======================================================================
// function : SetBackground
// purpose  :
// =======================================================================
void Cocoa_Window::SetBackground (const Aspect_Handle theBackPixmap)
{
  //
}

// =======================================================================
// function : SetBackground
// purpose  :
// =======================================================================
Standard_Boolean Cocoa_Window::SetBackground (const Standard_CString  theFileName,
                                              const Aspect_FillMethod theMethod)
{
  return Standard_False;
}

// =======================================================================
// function : SetBackground
// purpose  :
// =======================================================================
void Cocoa_Window::SetBackground (const Quantity_Color& theColor)
{
  //
}

// =======================================================================
// function : SetBackground
// purpose  :
// =======================================================================
void Cocoa_Window::SetBackground (const Aspect_GradientBackground& theGrBackground)
{
  Quantity_Color aColor1, aColor2;
  theGrBackground.Colors (aColor1, aColor2);
  SetBackground (aColor1, aColor2, theGrBackground.BgGradientFillMethod());
}

// =======================================================================
// function : SetBackground
// purpose  :
// =======================================================================
void Cocoa_Window::SetBackground (const Quantity_Color&           theColor1,
                                  const Quantity_Color&           theColor2,
                                  const Aspect_GradientFillMethod theMethod)
{
  //
}

// =======================================================================
// function : SetDoubleBuffer
// purpose  :
// =======================================================================
void Cocoa_Window::SetDoubleBuffer (const Standard_Boolean )
{
  //
}

// =======================================================================
// function : Flush
// purpose  :
// =======================================================================
void Cocoa_Window::Flush() const
{
  Restore();
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
// function : Clear
// purpose  :
// =======================================================================
void Cocoa_Window::Clear() const
{
  //
}

// =======================================================================
// function : ClearArea
// purpose  :
// =======================================================================
void Cocoa_Window::ClearArea (const Standard_Integer Xc,
                              const Standard_Integer Yc,
                              const Standard_Integer Width,
                              const Standard_Integer Height) const
{
  //
}

// =======================================================================
// function : Restore
// purpose  :
// =======================================================================
void Cocoa_Window::Restore() const
{
  //
}

// =======================================================================
// function : RestoreArea
// purpose  :
// =======================================================================
void Cocoa_Window::RestoreArea (const Standard_Integer Xc,
                                const Standard_Integer Yc,
                                const Standard_Integer Width,
                                const Standard_Integer Height) const
{
  //
}

// =======================================================================
// function : Dump
// purpose  :
// =======================================================================
Standard_Boolean Cocoa_Window::Dump (const Standard_CString theFilename,
                                     const Standard_Real    theGammaValue) const
{
  /*Image_AlienPixMap anImg;
  if (!ToPixMap (anImg) || anImg.IsEmpty())
  {
    return Standard_False;
  }
  if (Abs (theGammaValue - 1.0) > 0.001)
  {
    anImg.AdjustGamma (theGammaValue);
  }
  return anImg.Save (theFilename);*/
  return Standard_False;
}

// =======================================================================
// function : DumpArea
// purpose  :
// =======================================================================
Standard_Boolean Cocoa_Window::DumpArea (const Standard_CString theFilename,
                                         const Standard_Integer theCenterX,
                                         const Standard_Integer theCenterY,
                                         const Standard_Integer theWidth,
                                         const Standard_Integer theHeight,
                                         const Standard_Real    theGammaValue) const
{
  return Standard_False;
}

// =======================================================================
// function : ToPixMap
// purpose  :
// =======================================================================
/*Standard_Boolean Cocoa_Window::ToPixMap (Image_PixMap& thePixMap) const
{
  return Standard_False;
}*/

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
Standard_Boolean Cocoa_Window::Load (const Standard_CString theFilename) const
{
  return Standard_False;
}

// =======================================================================
// function : LoadArea
// purpose  :
// =======================================================================
Standard_Boolean Cocoa_Window::LoadArea (const Standard_CString theFilename,
                                         const Standard_Integer theCenterX,
                                         const Standard_Integer theCenterY,
                                         const Standard_Integer theWidth,
                                         const Standard_Integer theHeight) const
{
  return Standard_False;
}

// =======================================================================
// function : BackingStore
// purpose  :
// =======================================================================
Standard_Boolean Cocoa_Window::BackingStore() const
{
  return Standard_False;
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
void Cocoa_Window::Position (Quantity_Parameter& X1, Quantity_Parameter& Y1,
                             Quantity_Parameter& X2, Quantity_Parameter& Y2) const
{
  //
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void Cocoa_Window::Position (Standard_Integer& X1, Standard_Integer& Y1,
                             Standard_Integer& X2, Standard_Integer& Y2) const
{
  //
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void Cocoa_Window::Size (Quantity_Parameter& theWidth,
                         Quantity_Parameter& theHeight) const
{
  //
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

// =======================================================================
// function : MMSize
// purpose  :
// =======================================================================
void Cocoa_Window::MMSize (Standard_Real& theWidth,
                           Standard_Real& theHeight) const
{
  //
}

// =======================================================================
// function : Convert
// purpose  :
// =======================================================================
Quantity_Parameter Cocoa_Window::Convert (const Standard_Integer PV) const
{
  return 0.0; ///
}

// =======================================================================
// function : Convert
// purpose  :
// =======================================================================
Standard_Integer Cocoa_Window::Convert (const Quantity_Parameter DV) const
{
  return 0; ////
}

// =======================================================================
// function : Convert
// purpose  :
// =======================================================================
void Cocoa_Window::Convert (const Standard_Integer PX,
                            const Standard_Integer PY,
                            Quantity_Parameter&    DX,
                            Quantity_Parameter&    DY) const
{
  //
}

// =======================================================================
// function : Convert
// purpose  :
// =======================================================================
void Cocoa_Window::Convert (const Quantity_Parameter DX,
                            const Quantity_Parameter DY,
                            Standard_Integer&        PX,
                            Standard_Integer&        PY) const
{
  //
}
