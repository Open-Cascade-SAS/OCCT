// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// include windows.h first to have all definitions available
#include <windows.h>

//                GG 07/03/00 Add MMSize() method

#include <WNT_Window.ixx>

#include <Image_AlienPixMap.hxx>
#include <Aspect_Convert.hxx>

#include <stdio.h>

//************************************************************************//
//***//
// callback function to manage window's background
extern LRESULT CALLBACK WNT_WndProc (
                         HWND, UINT, WPARAM, LPARAM
                        );

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
  myWClass (theClass)
{
  ZeroMemory (&myExtraData, sizeof (WNT_WindowData));
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

  myUsrData = (Standard_Address )SetWindowLongPtr ((HWND )myHWindow, GWLP_USERDATA, (LONG_PTR )&myExtraData);

  myExtraData.WNT_Window_Ptr = (void* )this;

  SetFlags (WDF_NOERASEBKGRND);
}

//***//
//************************* Constructor **********************************//
//***//
WNT_Window :: WNT_Window (
               const Aspect_Handle                aHandle,
               const Quantity_NameOfColor         aBackColor
              ) : Aspect_Window()
{
  doCreate (aHandle, aBackColor);

  /* Bug OCC20596 */
  SetFlags(WDF_NOERASEBKGRND);

}  // end constructor
//***//
//************************* Constructor **********************************//
//***//
WNT_Window :: WNT_Window (
               const Standard_Integer             aPart1,
               const Standard_Integer             aPart2,
               const Quantity_NameOfColor         aBackColor
              ) : Aspect_Window()
{
  Aspect_Handle aHandle = ( Aspect_Handle )(  ( aPart1 << 16 ) + aPart2  );

  doCreate (aHandle, aBackColor);

  /* Bug OCC20596 */
  SetFlags(WDF_NOERASEBKGRND);

}  // end constructor
//***//
//***************************** Destroy **********************************//
//***//
void WNT_Window :: Destroy ()
{
  if (myHWindow)
  {
    if (myUsrData != Standard_Address(-1))
    {
      SetWindowLongPtr ((HWND )myHWindow, GWLP_USERDATA, (LONG_PTR )myUsrData);
    }

    if (!( myExtraData.dwFlags & WDF_FOREIGN))
    {
      DestroyWindow ((HWND )myHWindow);
    }
  }  // end if
}  // end WNT_Window :: Destroy

//**************************** SetCursor *********************************//
//***//
void WNT_Window :: SetCursor ( const Aspect_Handle aCursor ) const {

#ifndef _WIN64
 SetClassLong (  ( HWND )myHWindow, GCL_HCURSOR, ( LONG )aCursor  );
#else
 SetClassLong (  ( HWND )myHWindow, GCLP_HCURSOR, ( LONG )aCursor  );
#endif

}  // end WNT_Window :: SetCursor

//***//
//***************************** IsMapped *********************************//
//***//
Standard_Boolean WNT_Window :: IsMapped () const {
  if (IsVirtual()) {
    return Standard_True;
  }

  WINDOWPLACEMENT wp;

  wp.length = sizeof ( WINDOWPLACEMENT );
  GetWindowPlacement (  ( HWND )myHWindow, &wp  );

  return !(  wp.showCmd == SW_HIDE || wp.showCmd == SW_MINIMIZE );
}  // WNT_Window :: IsMapped

//***//
//***************************** Map (1) **********************************//
//***//
void WNT_Window :: Map () const {
  if (IsVirtual()) {
    return;
  }
  Map ( SW_SHOW );
}  // end WNT_Window :: Map

//***//
//***************************** Map (2) **********************************//
//***//
void WNT_Window :: Map ( const Standard_Integer aMapMode ) const {
  if (IsVirtual()) {
    return;
  }
  ShowWindow (  ( HWND )myHWindow, aMapMode  );
  UpdateWindow (  ( HWND )myHWindow  );

}  // end WNT_Window :: Map

//***//
//**************************** Unmap *************************************//
//***//
void WNT_Window :: Unmap () const {

 Map ( SW_HIDE );

}  // end WNT_Window :: Unmap

//***//
//**************************** DoResize **********************************//
//***//
Aspect_TypeOfResize WNT_Window :: DoResize () const
{
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

}  // end WNT_Window :: DoResize

//***//
//**************************** DoMapping **********************************//
//***//
Standard_Boolean WNT_Window :: DoMapping () const {
// DO nothing on WNT.
  return Standard_True;
}

//***//
//******************************* Ratio **********************************//
//***//
Quantity_Ratio WNT_Window :: Ratio () const {

 RECT r;

 GetClientRect (  ( HWND )myHWindow, &r  );

 return ( Quantity_Ratio )(  ( Quantity_Ratio )r.right / ( Quantity_Ratio )r.bottom  );

}  // end WNT_Window :: Ratio

//***//
//**************************** Position (2) ******************************//
//***//
void WNT_Window :: Position (
                    Standard_Integer& X1, Standard_Integer& Y1,
                    Standard_Integer& X2, Standard_Integer& Y2
                   ) const {

 POINT ptl, ptr;
 RECT  r;

 GetClientRect (  ( HWND )myHWindow, &r  );

 ptl.x = ptl.y = 0;
 ClientToScreen (  ( HWND )myHWindow, &ptl  );
 ptr.x = r.right;
 ptr.y = r.bottom;
 ClientToScreen (  ( HWND )myHWindow, &ptr  );

 if ( myHParentWindow ) {

  ScreenToClient (      ( HWND )myHParentWindow, &ptl  );
  ScreenToClient (  ( HWND )myHParentWindow, &ptr  );

 }  // end if

 X1 = ptl.x;
 X2 = ptr.x;
 Y1 = ptl.y;
 Y2 = ptr.y;

}  // end WNT_Window :: Position

//***//
//******************************* Size (2) *******************************//
//***//
void WNT_Window :: Size (
                    Standard_Integer& Width, Standard_Integer& Height
                   ) const {

 RECT r;

 GetClientRect (  ( HWND )myHWindow, &r  );

 Width  = r.right;
 Height = r.bottom;

}  // end WNT_Window :: Size

//***//
//******************************* SetPos *********************************//
//***//
void WNT_Window :: SetPos (
                    const Standard_Integer X,  const Standard_Integer Y,
                    const Standard_Integer X1, const Standard_Integer Y1
                   ) {

 aXLeft   = X;
 aYTop    = Y;
 aXRight  = X1;
 aYBottom = Y1;

}  // end WNT_Window :: SetPos

//***//
//**************************** SetFlags **********************************//
//***//
void WNT_Window :: SetFlags ( const Standard_Integer aFlags ) {

 myExtraData.dwFlags |= aFlags;

}  // end WNT_Window :: SetFlags

//***//
//*************************** ResetFlags *********************************//
//***//
void WNT_Window :: ResetFlags ( const Standard_Integer aFlags ) {

 myExtraData.dwFlags &= ~aFlags;

}  // end WNT_Window :: ResetFlags

//***//
//*************************** doCreate **********************************//
//***//
void WNT_Window :: doCreate (
                    const Aspect_Handle                aHandle,
                    const Quantity_NameOfColor         aBackColor
                   )
{
  ZeroMemory (&myExtraData, sizeof (WNT_WindowData));

  myHWindow        = aHandle;
  myHParentWindow  = GetParent ((HWND )aHandle);
  LONG_PTR uData   = GetWindowLongPtr ((HWND )aHandle, GWLP_USERDATA);
  myUsrData        = Standard_Address(-1);

  SetBackground (aBackColor);

  myExtraData.WNT_Window_Ptr = (void* )this;

  if (uData != (LONG_PTR )&myExtraData)
  {
    myUsrData = (Standard_Address )SetWindowLongPtr ((HWND )myHWindow, GWLP_USERDATA, (LONG_PTR )&myExtraData);
  }

  myExtraData.dwFlags = WDF_FOREIGN;

  WINDOWPLACEMENT wp;
  wp.length = sizeof (WINDOWPLACEMENT);
  GetWindowPlacement ((HWND )myHWindow, &wp);

  aXLeft   = wp.rcNormalPosition.left;
  aYTop    = wp.rcNormalPosition.top;
  aXRight  = wp.rcNormalPosition.right;
  aYBottom = wp.rcNormalPosition.bottom;
}  // end WNT_Window :: doCreate
