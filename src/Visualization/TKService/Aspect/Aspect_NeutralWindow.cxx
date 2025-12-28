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

//=================================================================================================

Aspect_NeutralWindow::Aspect_NeutralWindow()
    : myHandle(0),
      myParentHandle(0),
      myFBConfig(0),
      myPosX(0),
      myPosY(0),
      myWidth(0),
      myHeight(0),
      myIsMapped(true)
{
}

//=================================================================================================

bool Aspect_NeutralWindow::SetNativeHandles(Aspect_Drawable theWindow,
                                            Aspect_Drawable theParentWindow,
                                            Aspect_FBConfig theFbConfig)
{
  if (myHandle == theWindow && myParentHandle == theParentWindow && myFBConfig == theFbConfig)
  {
    return false;
  }

  myHandle       = theWindow;
  myParentHandle = theParentWindow;
  myFBConfig     = theFbConfig;
  return true;
}

//=================================================================================================

bool Aspect_NeutralWindow::SetPosition(int theX1, int theY1)
{
  if (myPosX == theX1 && myPosY == theY1)
  {
    return false;
  }

  myPosX = theX1;
  myPosY = theY1;
  return true;
}

//=================================================================================================

bool Aspect_NeutralWindow::SetPosition(int theX1, int theY1, int theX2, int theY2)
{
  int aWidthNew  = theX2 - theX1;
  int aHeightNew = theY2 - theY1;
  if (myPosX == theX1 && myPosY == theY1 && myWidth == aWidthNew && myHeight == aHeightNew)
  {
    return false;
  }

  myPosX   = theX1;
  myWidth  = aWidthNew;
  myPosY   = theY1;
  myHeight = aHeightNew;
  return true;
}

//=================================================================================================

bool Aspect_NeutralWindow::SetSize(const int theWidth, const int theHeight)
{
  if (myWidth == theWidth && myHeight == theHeight)
  {
    return false;
  }

  myWidth  = theWidth;
  myHeight = theHeight;
  return true;
}
