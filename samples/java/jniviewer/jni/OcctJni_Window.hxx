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

#ifndef OcctJni_Window_H
#define OcctJni_Window_H

#include <Aspect_Window.hxx>

//! This class defines dummy window
class OcctJni_Window : public Aspect_Window
{

public:

  //! Creates a wrapper over existing Window handle
  OcctJni_Window (const int theWidth, const int theHeight)
  : myWidth (theWidth), myHeight(theHeight) {}

  //! Returns native Window handle
  virtual Aspect_Drawable NativeHandle() const Standard_OVERRIDE { return 0; }

  //! Returns parent of native Window handle
  virtual Aspect_Drawable NativeParentHandle() const Standard_OVERRIDE { return 0; }

  //! Returns nothing on Android
  virtual Aspect_FBConfig NativeFBConfig() const Standard_OVERRIDE { return 0; }

  virtual void Destroy() {}

  //! Opens the window <me>
  virtual void Map() const Standard_OVERRIDE {}

  //! Closes the window <me>
  virtual void Unmap() const Standard_OVERRIDE {}

  //! Applies the resizing to the window <me>
  virtual Aspect_TypeOfResize DoResize() const Standard_OVERRIDE { return Aspect_TOR_UNKNOWN; }

  //! Apply the mapping change to the window <me>
  virtual Standard_Boolean DoMapping() const Standard_OVERRIDE { return Standard_True; }

  //! Returns True if the window <me> is opened
  virtual Standard_Boolean IsMapped() const Standard_OVERRIDE { return Standard_True; }

  //! Returns The Window RATIO equal to the physical WIDTH/HEIGHT dimensions
  virtual Standard_Real Ratio() const Standard_OVERRIDE { return 1.0; }

  //! Returns The Window POSITION in PIXEL
  virtual void Position (Standard_Integer& theX1,
                         Standard_Integer& theY1,
                         Standard_Integer& theX2,
                         Standard_Integer& theY2) const Standard_OVERRIDE
  {
    theX1 = 0;
    theX2 = myWidth;
    theY1 = 0;
    theY2 = myHeight;
  }

  //! Set The Window POSITION in PIXEL
  void SetPosition (const Standard_Integer theX1,
                    const Standard_Integer theY1,
                    const Standard_Integer theX2,
                    const Standard_Integer theY2)
  {
    myWidth  = theX2 - theX1;
    myHeight = theY2 - theY1;
  }

  //! Returns The Window SIZE in PIXEL
  virtual void Size (Standard_Integer& theWidth,
                     Standard_Integer& theHeight) const Standard_OVERRIDE
  {
    theWidth  = myWidth;
    theHeight = myHeight;
  }

  //! Set The Window SIZE in PIXEL
  void SetSize (const Standard_Integer theWidth,
                const Standard_Integer theHeight)
  {
    myWidth  = theWidth;
    myHeight = theHeight;
  }

private:

  int myWidth;
  int myHeight;

public:

  DEFINE_STANDARD_RTTIEXT(OcctJni_Window,Aspect_Window)

};

DEFINE_STANDARD_HANDLE(OcctJni_Window, Aspect_Window)

#endif // OcctJni_Window_H
