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

#ifndef ANDROIDQT_WINDOW_H
#define ANDROIDQT_WINDOW_H

#include <Aspect_Window.hxx>

//! This class defines dummy window.
//! The main functionality is viewport dimensions.
class AndroidQt_Window : public Aspect_Window
{

public:

  //! Creates a wrapper over existing Window handle
  AndroidQt_Window(const int theWidth,   const int theHeight,
                   const int theX1 = -1, const int theX2 = -1,
                   const int theY1 = -1, const int theY2 = -1);

  //! Returns native Window handle
  virtual Aspect_Drawable NativeHandle() const { return 0; }

  //! Returns parent of native Window handle.
  virtual Aspect_Drawable NativeParentHandle() const { return 0; }

  virtual void Destroy() {}

  //! Opens the window <me>
  virtual void Map() const {}

  //! Closes the window <me>
  virtual void Unmap() const {}

  //! Applies the resizing to the window <me>
  virtual Aspect_TypeOfResize DoResize() const { return Aspect_TOR_UNKNOWN; }

  //! Apply the mapping change to the window <me>
  virtual Standard_Boolean DoMapping() const { return Standard_True; }

  //! Returns True if the window <me> is opened
  virtual Standard_Boolean IsMapped() const { return Standard_True; }

  //! Returns The Window RATIO equal to the physical WIDTH/HEIGHT dimensions
  virtual Standard_Real Ratio() const { return 1.0; }

  //! Returns The Window POSITION in PIXEL
  virtual void Position (Standard_Integer& theX1,
                         Standard_Integer& theY1,
                         Standard_Integer& theX2,
                         Standard_Integer& theY2) const;

  //! Set The Window POSITION in PIXEL
  virtual void SetPosition (const Standard_Integer theX1,
                            const Standard_Integer theY1,
                            const Standard_Integer theX2,
                            const Standard_Integer theY2);

  //! Returns The Window SIZE in PIXEL
  virtual void Size (Standard_Integer& theWidth,
                     Standard_Integer& theHeight) const;

  //! Set The Window SIZE in PIXEL
  virtual void SetSize (const Standard_Integer theWidth,
                        const Standard_Integer theHeight);

private:

  int myWidth;
  int myHeight;

  int myX1;
  int myX2;
  int myY1;
  int myY2;

public:

  DEFINE_STANDARD_RTTIEXT(AndroidQt_Window,Aspect_Window)

};

DEFINE_STANDARD_HANDLE(AndroidQt_Window, Aspect_Window)

#endif // ANDROIDQT_WINDOW_H
