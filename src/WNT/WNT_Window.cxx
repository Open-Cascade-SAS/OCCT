// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// include windows.h first to have all definitions available
#include <windows.h>


#include <Aspect_Convert.hxx>
#include <Aspect_WindowDefinitionError.hxx>
#include <Aspect_WindowError.hxx>
#include <Image_AlienPixMap.hxx>
#include <Standard_Type.hxx>
#include <WNT_WClass.hxx>
#include <WNT_Window.hxx>

#include <stdio.h>
// =======================================================================
// function : WNT_Window
// purpose  :
// =======================================================================
WNT_Window::WNT_Window (const Standard_CString           theTitle,
                        const Handle(WNT_WClass)&        theClass,
                        const WNT_Dword&                 theStyle,
                        const Standard_Integer           thePxLeft,
                        const Standard_Integer           thePxTop,
                        const Standard_Integer           thePxWidth,
                        const Standard_Integer           thePxHeight,
                        const Quantity_NameOfColor       theBackColor,
                        const Aspect_Handle              theParent,
                        const Aspect_Handle              theMenu,
                        const Standard_Address           theClientStruct)
: Aspect_Window(),
  aXLeft (thePxLeft),
  aYTop (thePxTop),
  aXRight (thePxLeft + thePxWidth),
  aYBottom (thePxTop + thePxHeight),
  myWClass (theClass),
  myIsForeign (Standard_False)
{
  DWORD dwStyle = theStyle;

  if (thePxWidth <= 0 || thePxHeight <= 0)
  {
    Aspect_WindowDefinitionError::Raise ("Coordinate(s) out of range");
  }

  if (theParent && !(theStyle & WS_CHILD))
  {
    dwStyle |= WS_CHILD | WS_CLIPSIBLINGS;
  }
  else if (!theParent && !(theStyle & WS_CLIPCHILDREN))
  {
    dwStyle |= WS_CLIPCHILDREN;
  }

  // include decorations in the window dimensions
  // to reproduce same behaviour of Xw_Window.
  RECT aRect;
  aRect.top    = aYTop;
  aRect.bottom = aYBottom;
  aRect.left   = aXLeft;
  aRect.right  = aXRight;
  AdjustWindowRect (&aRect, dwStyle, theMenu != NULL ? TRUE : FALSE);
  aXLeft   = aRect.left;
  aYTop    = aRect.top;
  aXRight  = aRect.right;
  aYBottom = aRect.bottom;

  myHWindow = CreateWindow (
              myWClass->Name(),                 // window's class name
              theTitle,                         // window's name
              dwStyle,                          // window's style
              aXLeft, aYTop,                    // window's coordinates
              (aXRight - aXLeft), (aYBottom - aYTop),
              (HWND )theParent,                 // window's parent
              (HMENU )theMenu,                  // window's menu
              (HINSTANCE )myWClass->Instance(), // application's instance
              theClientStruct);                 // pointer to CLIENTCREATESTRUCT
  if (!myHWindow)
  {
    Aspect_WindowDefinitionError::Raise ("Unable to create window");
  }

  myHParentWindow = theParent;
  SetBackground (theBackColor);
}

// =======================================================================
// function : WNT_Window
// purpose  :
// =======================================================================
WNT_Window::WNT_Window (const Aspect_Handle        theHandle,
                        const Quantity_NameOfColor theBackColor)
: myIsForeign (Standard_True)
{
  myHWindow        = theHandle;
  myHParentWindow  = GetParent ((HWND )theHandle);

  SetBackground (theBackColor);

  WINDOWPLACEMENT aPlace;
  aPlace.length = sizeof (WINDOWPLACEMENT);
  ::GetWindowPlacement ((HWND )myHWindow, &aPlace);

  aXLeft   = aPlace.rcNormalPosition.left;
  aYTop    = aPlace.rcNormalPosition.top;
  aXRight  = aPlace.rcNormalPosition.right;
  aYBottom = aPlace.rcNormalPosition.bottom;
}

// =======================================================================
// function : Destroy
// purpose  :
// =======================================================================
void WNT_Window::Destroy()
{
  if (myHWindow == NULL
   || myIsForeign)
  {
    return;
  }

  DestroyWindow ((HWND )myHWindow);
  myIsForeign = Standard_False;
}

// =======================================================================
// function : SetCursor
// purpose  :
// =======================================================================
void WNT_Window::SetCursor (const Aspect_Handle theCursor) const
{
  ::SetClassLongPtr ((HWND )myHWindow, GCLP_HCURSOR, (LONG_PTR )theCursor);
}

// =======================================================================
// function : IsMapped
// purpose  :
// =======================================================================
Standard_Boolean WNT_Window::IsMapped() const
{
  if (IsVirtual())
  {
    return Standard_True;
  }

  WINDOWPLACEMENT aPlace;
  aPlace.length = sizeof (WINDOWPLACEMENT);
  ::GetWindowPlacement ((HWND )myHWindow, &aPlace);
  return !(aPlace.showCmd == SW_HIDE
        || aPlace.showCmd == SW_MINIMIZE);
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void WNT_Window::Map() const
{
  if (!IsVirtual())
  {
    Map (SW_SHOW);
  }
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void WNT_Window::Map (const Standard_Integer theMapMode) const
{
  if (IsVirtual())
  {
    return;
  }

  ::ShowWindow   ((HWND )myHWindow, theMapMode);
  ::UpdateWindow ((HWND )myHWindow);
}

// =======================================================================
// function : Unmap
// purpose  :
// =======================================================================
void WNT_Window::Unmap() const
{
  Map (SW_HIDE);
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize WNT_Window::DoResize() const
{
  if (IsVirtual())
  {
    return Aspect_TOR_UNKNOWN;
  }

  int                 mask = 0;
  Aspect_TypeOfResize mode = Aspect_TOR_UNKNOWN;
  WINDOWPLACEMENT     wp;

  wp.length = sizeof ( WINDOWPLACEMENT );
  GetWindowPlacement (  ( HWND )myHWindow, &wp  );

  if (wp.showCmd != SW_SHOWMINIMIZED)
  {
    if (Abs (wp.rcNormalPosition.left   - aXLeft  ) > 2) mask |= 1;
    if (Abs (wp.rcNormalPosition.right  - aXRight ) > 2) mask |= 2;
    if (Abs (wp.rcNormalPosition.top    - aYTop   ) > 2) mask |= 4;
    if (Abs (wp.rcNormalPosition.bottom - aYBottom) > 2) mask |= 8;

    switch (mask)
    {
      case 0:
        mode = Aspect_TOR_NO_BORDER;
        break;
      case 1:
        mode = Aspect_TOR_LEFT_BORDER;
        break;
      case 2:
        mode = Aspect_TOR_RIGHT_BORDER;
        break;
      case 4:
        mode = Aspect_TOR_TOP_BORDER;
        break;
      case 5:
        mode = Aspect_TOR_LEFT_AND_TOP_BORDER;
        break;
      case 6:
        mode = Aspect_TOR_TOP_AND_RIGHT_BORDER;
        break;
      case 8:
        mode = Aspect_TOR_BOTTOM_BORDER;
        break;
      case 9:
        mode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;
        break;
      case 10:
        mode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER;
        break;
      default:
        break;
    }  // end switch

    *((Standard_Integer* )&aXLeft  ) = wp.rcNormalPosition.left;
    *((Standard_Integer* )&aXRight ) = wp.rcNormalPosition.right;
    *((Standard_Integer* )&aYTop   ) = wp.rcNormalPosition.top;
    *((Standard_Integer* )&aYBottom) = wp.rcNormalPosition.bottom;
  }

  return mode;
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Quantity_Ratio WNT_Window::Ratio() const
{
  if (IsVirtual())
  {
    return Quantity_Ratio(aXRight - aXLeft)/ Quantity_Ratio(aYBottom - aYTop);
  }

  RECT aRect;
  GetClientRect ((HWND )myHWindow, &aRect);
  return Quantity_Ratio(aRect.right - aRect.left) / Quantity_Ratio(aRect.bottom - aRect.top);
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void WNT_Window::Position (Standard_Integer& theX1, Standard_Integer& theY1,
                           Standard_Integer& theX2, Standard_Integer& theY2) const
{
  if (IsVirtual())
  {
    theX1  = aXLeft;
    theX2  = aXRight;
    theY1  = aYTop;
    theY2  = aYBottom;
    return;
  }

  RECT  aRect;
  ::GetClientRect ((HWND )myHWindow, &aRect);

  POINT aPntLeft, aPntRight;
  aPntLeft.x = aPntLeft.y = 0;
  ::ClientToScreen ((HWND )myHWindow, &aPntLeft);
  aPntRight.x = aRect.right;
  aPntRight.y = aRect.bottom;
  ::ClientToScreen ((HWND )myHWindow, &aPntRight);

  if (myHParentWindow != NULL)
  {
    ::ScreenToClient ((HWND )myHParentWindow, &aPntLeft);
    ::ScreenToClient ((HWND )myHParentWindow, &aPntRight);
  }

  theX1 = aPntLeft.x;
  theX2 = aPntRight.x;
  theY1 = aPntLeft.y;
  theY2 = aPntRight.y;
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void WNT_Window::Size (Standard_Integer& theWidth,
                       Standard_Integer& theHeight) const
{
  if (IsVirtual())
  {
    theWidth  = aXRight - aXLeft;
    theHeight = aYBottom - aYTop;
    return;
  }

  RECT aRect;
  ::GetClientRect ((HWND )myHWindow, &aRect);
  theWidth  = aRect.right;
  theHeight = aRect.bottom;
}

// =======================================================================
// function : SetPos
// purpose  :
// =======================================================================
void WNT_Window::SetPos (const Standard_Integer theX,  const Standard_Integer theY,
                         const Standard_Integer theX1, const Standard_Integer theY1)
{
  aXLeft   = theX;
  aYTop    = theY;
  aXRight  = theX1;
  aYBottom = theY1;
}
