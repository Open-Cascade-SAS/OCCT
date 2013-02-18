// Created by: NW,JPB,CAL,GG
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#define BUC60701	//GG 23/06/00 Enable to dump an exact image size
//			according to the window size
#define	RIC120302 	//GG Add a NEW XParentWindow methods which enable
//                      to retrieve the parent of the actual Xwindow ID.


//-Version

//-Design	Creation d'une fenetre X

//-Warning

//-References

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Xw_Window.ixx>
// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}
#include <Aspect_Convert.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Image_AlienPixMap.hxx>
#include <Standard_PCharacter.hxx>

#include <Xw_Extension.h>

//-Static data definitions

static XW_STATUS status ;

//============================================================================
//==== HashCode : Returns a HashCode CString
//============================================================================
inline Standard_Integer HashCode (const Standard_CString Value)
{
Standard_Integer  i,n,aHashCode = 0;
union {
  char             charPtr[80];
  int              intPtr[20];
} u;

  n = strlen(Value);

  if( n > 0 ) {
    if( n < 80 ) {
      n = (n+3)/4;
      u.intPtr[n-1] = 0;
      strcpy(u.charPtr,Value);
    } else {
      n = 20;
      strncpy(u.charPtr,Value,80);
    }

    for( i=0 ; i<n ; i++ ) {
      aHashCode = aHashCode ^ u.intPtr[i];
    }
  }

//printf(" HashCode of '%s' is %d\n",Value,aHashCode);

  return Abs(aHashCode) + 1;
}

void Xw_Window::PrintError() {
Standard_CString ErrorMessag ;
Standard_Integer ErrorNumber ;
Standard_Integer ErrorGravity ;

        status = XW_SUCCESS ;
        ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
        Xw_print_error() ;
}

//-Aliases

//-Global data definitions

//-Constructors

Xw_Window::Xw_Window (const Handle(Aspect_DisplayConnection)& theDisplayConnection)
: Aspect_Window(),
  myDisplayConnection (theDisplayConnection)
{
  Init();
}


//=======================================================================
//function : Xw_Window
//purpose  :
//=======================================================================
Xw_Window::Xw_Window (const Handle(Aspect_DisplayConnection)& theDisplayConnection,
                      const Standard_Integer            aPart1,
                      const Standard_Integer            aPart2,
                      const Quantity_NameOfColor        BackColor)
: Aspect_Window(),
  myDisplayConnection (theDisplayConnection)
{
  Init();
  Aspect_Handle aWindow = (aPart1 << 16) | (aPart2 & 0xFFFF);
  SetWindow (aWindow, BackColor);
}

//=======================================================================
//function : Xw_Window
//purpose  :
//=======================================================================
Xw_Window::Xw_Window (const Handle(Aspect_DisplayConnection)& theDisplayConnection,
                      const Aspect_Handle               aWindow,
                      const Quantity_NameOfColor        BackColor)
: Aspect_Window(),
  myDisplayConnection (theDisplayConnection)
{
  Init();
  SetWindow (aWindow, BackColor);
}

//=======================================================================
//function : Xw_Window
//purpose  :
//=======================================================================
Xw_Window::Xw_Window (const Handle(Aspect_DisplayConnection)& theDisplayConnection,
                      const Standard_CString          theTitle,
                      const Standard_Integer          thePxLeft,
                      const Standard_Integer          thePxTop,
                      const Standard_Integer          theWidth,
                      const Standard_Integer          theHeight,
                      const Quantity_NameOfColor      theBackColor,
                      const Aspect_Handle             theParent)
: Aspect_Window (),
  myDisplayConnection (theDisplayConnection)
{
  Init();

  Standard_Integer aParentSizeX = 1;
  Standard_Integer aParentSizeY = 1;

  Aspect_Handle aRoot, aColormap, *aDisplay;
  Xw_TypeOfVisual aVisualClass;
  Standard_Integer aVisualDepth;
  Xw_get_display_info (MyExtendedDisplay,
                       &aDisplay, &aRoot, &aColormap, &aVisualClass, &aVisualDepth);
  Xw_get_screen_size (MyExtendedDisplay, &aParentSizeX, &aParentSizeY);
  if (theParent)
  {
    XWindowAttributes anAttributes;
    if(XGetWindowAttributes ((Display* )aDisplay, theParent, &anAttributes))
    {
      aParentSizeX = anAttributes.width;
      aParentSizeY = anAttributes.height;
    }
  }
  Quantity_Parameter aQCenterX, aQCenterY, aQSizeX, aQSizeY;
  Aspect_Convert::ConvertCoordinates (aParentSizeX, aParentSizeY,
                                      thePxLeft, thePxTop, theWidth, theHeight,
                                      aQCenterX, aQCenterY, aQSizeX, aQSizeY);
  SetWindow (theTitle, aQCenterX, aQCenterY, aQSizeX, aQSizeY,
            theBackColor, theParent);
}

void Xw_Window::Init()
{
  MyXWindow = 0 ;
  MyXParentWindow = 0 ;
#if defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  MyExtendedDisplay = NULL;
#else
  MyExtendedDisplay = Xw_set_display (myDisplayConnection->GetDisplay());
#endif
  MyExtendedWindow = NULL ;
  MyVisualClass = Xw_TOV_TRUECOLOR;
}

//=======================================================================
//function : SetWindow
//purpose  :
//=======================================================================

void Xw_Window::SetWindow (const Aspect_Handle          aWindow,
                           const Quantity_NameOfColor   BackColor)
{
  Aspect_Handle window,root,colormap,pixmap ;
  Xw_TypeOfVisual visualclass ;
  int visualdepth,visualid ;

  MyXWindow = aWindow ;
  MyXParentWindow = aWindow ;

  if( !MyXWindow ) {
    PrintError() ;
  }

  MyExtendedWindow =
    Xw_def_window (MyExtendedDisplay,MyXWindow,Standard_False);

  status = Xw_get_window_info(MyExtendedWindow,&window,&pixmap,
                              &root,&colormap,&visualclass,
                              &visualdepth,&visualid);
  if( !status ) {
    PrintError() ;
  }

  SetBackground(BackColor) ;
}

//=======================================================================
//function : SetWindow
//purpose  :
//=======================================================================

void Xw_Window::SetWindow (const Standard_CString       Title,
                           const Quantity_Parameter     Xc,
                           const Quantity_Parameter     Yc,
                           const Quantity_Parameter     Width,
                           const Quantity_Parameter     Height,
                           const Quantity_NameOfColor   BackColor,
                           const Aspect_Handle          Parent)
{
  Standard_Integer istransparent = Standard_False ;

  MyXParentWindow = Parent ;
  MyXWindow = 0 ;

  MyXWindow = Xw_open_window(MyExtendedDisplay,MyVisualClass,MyXParentWindow,
                             (float)Xc,(float)Yc,(float)Width,(float)Height,
                             (Standard_PCharacter)Title,istransparent) ;
  if( !MyXWindow ) {
    PrintError() ;
  }

  MyExtendedWindow =
    Xw_def_window (MyExtendedDisplay,MyXWindow,istransparent);

  SetBackground(BackColor) ;

  if( MyXParentWindow && (MyXWindow != MyXParentWindow) ) Map() ;

}

void Xw_Window::Map () const {
#ifdef RIC120302
	if( MyXWindow == MyXParentWindow ) return;
#endif
  if (IsVirtual()) return;
	status = Xw_set_window_state (MyExtendedWindow, XW_MAP);
	if( !status ) {
	    PrintError() ;
	}
}

void Xw_Window::Unmap () const {
#ifdef RIC120302
	if( MyXWindow == MyXParentWindow ) return;
#endif
	status = Xw_set_window_state (MyExtendedWindow, XW_ICONIFY);
	if( !status ) {
	    PrintError() ;
	}
}

Aspect_TypeOfResize Xw_Window::DoResize () const {
XW_RESIZETYPE state ;

	state = Xw_resize_window (MyExtendedWindow);
//	if( state == Aspect_TOR_UNKNOWN ) {
	if( state == XW_TOR_UNKNOWN ) {
	    PrintError() ;
	}

	return (Aspect_TypeOfResize(state)) ;
}

Standard_Boolean Xw_Window::DoMapping () const {
int pxc,pyc,width,height;
XW_WINDOWSTATE state;

      state = Xw_get_window_position (MyExtendedWindow,&pxc,&pyc,&width,&height);

      return IsMapped();
}

void Xw_Window::Destroy () {
Standard_Boolean destroy =
	( MyXWindow == MyXParentWindow ) ? Standard_False : Standard_True;

	status = Xw_close_window (MyExtendedWindow,destroy);
	if( !status ) {
	    PrintError() ;
	}

	MyXWindow = 0 ;
	MyExtendedWindow = NULL ;
}

void Xw_Window::SetCursor (const Standard_Integer anId, const Quantity_NameOfColor aColor) const {
Quantity_Color Color(aColor) ;
Standard_Real r,g,b ;

	Color.Values(r,g,b,Quantity_TOC_RGB) ;

    	status = Xw_set_hard_cursor (MyExtendedWindow,(int)anId,0,
							(float)r,
							(float)g,
							(float)b) ;

        if( !status ) {
            PrintError() ;
        }
}

Standard_Boolean Xw_Window::IsMapped () const {
  if (IsVirtual()) {
    return Standard_True;
  }
XW_WINDOWSTATE state;
      state = Xw_get_window_state (MyExtendedWindow);
      switch (state) {
        case XW_WS_UNKNOWN:
	  return Standard_False;
        case XW_ICONIFY:
	  return Standard_False;
        case XW_PUSH:
	  return Standard_True;
        case XW_MAP:
	  return Standard_True;
#ifndef DEB
	default:
	  return Standard_False;
#endif
      }
  return Standard_False;
}

Standard_Real Xw_Window::Ratio () const {
int width, height;

	status = Xw_get_window_size (MyExtendedWindow,&width,&height);
	if( !status ) {
	    Xw_print_error() ;
	}

	return ((Standard_Real)width/height) ;
}

void Xw_Window::Size (Standard_Integer &Width, Standard_Integer &Height) const {
int width, height;
	status = Xw_get_window_size (MyExtendedWindow,&width,&height);
	if( !status ) {
	    Xw_print_error() ;
	}

	Width = width ;
	Height = height ;
}

void Xw_Window::Position (Standard_Integer &X1, Standard_Integer &Y1, Standard_Integer &X2, Standard_Integer &Y2) const {
XW_WINDOWSTATE state = XW_WS_UNKNOWN ;
int pxc, pyc;
int width, height;

	state = Xw_get_window_position (MyExtendedWindow,
						&pxc,&pyc,&width,&height);
	if( state == XW_WS_UNKNOWN ) {
	    Xw_print_error() ;
	}

	X1 = pxc - width/2 ; Y1 = pyc - height/2 ;
	X2 = X1 + width - 1 ; Y2 = Y1 + height - 1 ;
}

Aspect_Handle Xw_Window::XWindow () const {

	return (MyXWindow);

}

void Xw_Window::XWindow (Standard_Integer& aPart1, Standard_Integer& aPart2) const {

  aPart1 = (Standard_Integer)((MyXWindow >> 16 ) & 0xffff);
  aPart2 = (Standard_Integer)(MyXWindow & 0xffff);
}

// RIC120302
Aspect_Handle Xw_Window::XParentWindow () const {

	return (MyXParentWindow);

}

void Xw_Window::XParentWindow (Standard_Integer& aPart1, Standard_Integer& aPart2) const {

  aPart1 = (Standard_Integer)((MyXParentWindow >> 16 ) & 0xffff);
  aPart2 = (Standard_Integer)(MyXParentWindow & 0xffff);
}
// RIC120302

Aspect_Handle Xw_Window::XPixmap () const {
Aspect_Handle window,pixmap,root,colormap ;
Xw_TypeOfVisual visualclass ;
int visualdepth,visualid ;

	status = Xw_get_window_info(MyExtendedWindow,&window,&pixmap,
			&root,&colormap,&visualclass,&visualdepth,&visualid) ;

	return (pixmap);
}

Standard_Boolean Xw_Window::PointerPosition (Standard_Integer& X, Standard_Integer& Y) const {
Standard_Boolean cstatus ;
int x,y ;

	cstatus = Xw_get_cursor_position(MyExtendedWindow,&x,&y) ;
	X = x ;
	Y = y ;

	return (cstatus);
}

Xw_TypeOfVisual Xw_Window::VisualClass () const {

	return (MyVisualClass);

}

Standard_Boolean Xw_Window::BackgroundPixel ( Standard_Integer &aPixel ) const {
unsigned long pixel ;

        status = Xw_get_background_pixel(MyExtendedWindow,&pixel) ;
        if( !status ) PrintError() ;

	aPixel = Standard_Integer(pixel);

	return status;
}


Standard_Address Xw_Window::ExtendedWindow () const {

	return (MyExtendedWindow);

}
