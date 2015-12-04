// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <AndroidQt_Window.h>

IMPLEMENT_STANDARD_RTTIEXT(AndroidQt_Window, Aspect_Window)

// =======================================================================
// function : AndroidQt_Window
// purpose  :
// =======================================================================
AndroidQt_Window::AndroidQt_Window (const int theWidth, const int theHeight,
                                    const int theX1,    const int theX2,
                                    const int theY1,    const int theY2)
: myWidth (theWidth), myHeight(theHeight),
  myX1 (theX1), myX2 (theX2),
  myY1 (theY1), myY2 (theY2)
{
  if (myX1 == -1) myX1 = 0;
  if (myX2 == -1) myX2 = myWidth;

  if (myY1 == -1) myY1 = 0;
  if (myY2 == -1) myY2 = myHeight;
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void AndroidQt_Window::Position (Standard_Integer& theX1,
                                 Standard_Integer& theY1,
                                 Standard_Integer& theX2,
                                 Standard_Integer& theY2) const
{
  theX1 = myX1;
  theX2 = myX2;
  theY1 = myY1;
  theY2 = myY2;
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
void AndroidQt_Window::SetPosition (const Standard_Integer theX1,
                                    const Standard_Integer theY1,
                                    const Standard_Integer theX2,
                                    const Standard_Integer theY2)
{
  myX1 = theX1;
  myX2 = theX2;
  myY1 = theY1;
  myY2 = theY2;
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void AndroidQt_Window::Size (Standard_Integer& theWidth,
                             Standard_Integer& theHeight) const
{
  theWidth  = myWidth;
  theHeight = myHeight;
}

// =======================================================================
// function : SetSize
// purpose  :
// =======================================================================
void AndroidQt_Window::SetSize (const Standard_Integer theWidth,
                                const Standard_Integer theHeight)
{
  myWidth  = theWidth;
  myHeight = theHeight;
}
