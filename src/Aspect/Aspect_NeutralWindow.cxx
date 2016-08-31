// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Aspect_NeutralWindow.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Aspect_NeutralWindow, Aspect_Window)

// =======================================================================
// function : Aspect_NeutralWindow
// purpose  :
// =======================================================================
Aspect_NeutralWindow::Aspect_NeutralWindow()
: myHandle (0),
  myParentHandle (0),
  myFBConfig (0),
  myPosX (0),
  myPosY (0),
  myWidth (0),
  myHeight (0),
  myIsMapped (Standard_True) {}

// =======================================================================
// function : SetNativeHandles
// purpose  :
// =======================================================================
Standard_Boolean Aspect_NeutralWindow::SetNativeHandles (Aspect_Drawable theWindow,
                                                         Aspect_Drawable theParentWindow,
                                                         Aspect_FBConfig theFbConfig)
{
  if (myHandle       == theWindow
   && myParentHandle == theParentWindow
   && myFBConfig     == theFbConfig)
  {
    return Standard_False;
  }

  myHandle       = theWindow;
  myParentHandle = theParentWindow;
  myFBConfig     = theFbConfig;
  return Standard_True;
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
Standard_Boolean Aspect_NeutralWindow::SetPosition (Standard_Integer theX1,
                                                    Standard_Integer theY1)
{
  if (myPosX == theX1
   && myPosY == theY1)
  {
    return Standard_False;
  }

  myPosX = theX1;
  myPosY = theY1;
  return Standard_True;
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
Standard_Boolean Aspect_NeutralWindow::SetPosition (Standard_Integer theX1, Standard_Integer theY1,
                                                    Standard_Integer theX2, Standard_Integer theY2)
{
  Standard_Integer aWidthNew  = theX2 - theX1;
  Standard_Integer aHeightNew = theY2 - theY1;
  if (myPosX == theX1
   && myPosY == theY1
   && myWidth  == aWidthNew
   && myHeight == aHeightNew)
  {
    return Standard_False;
  }

  myPosX   = theX1;
  myWidth  = aWidthNew;
  myPosY   = theY1;
  myHeight = aHeightNew;
  return Standard_True;
}

// =======================================================================
// function : SetSize
// purpose  :
// =======================================================================
Standard_Boolean Aspect_NeutralWindow::SetSize (const Standard_Integer theWidth,
                                                const Standard_Integer theHeight)
{
  if (myWidth  == theWidth
   && myHeight == theHeight)
  {
    return Standard_False;
  }

  myWidth  = theWidth;
  myHeight = theHeight;
  return Standard_True;
}
