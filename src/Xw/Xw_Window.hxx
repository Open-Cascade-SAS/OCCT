// Created on: 2013-04-06
// Created by: Kirill Gavrilov
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

#ifndef _Xw_Window_H__
#define _Xw_Window_H__

#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && !defined(__ANDROID__) && !defined(__QNX__)

#include <Aspect_Window.hxx>

#include <Aspect_DisplayConnection.hxx>
#include <Aspect_FillMethod.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_TypeOfResize.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Quantity_NameOfColor.hxx>

class Aspect_WindowDefinitionError;
class Aspect_WindowError;
class Aspect_Background;
class Quantity_Color;
class Aspect_GradientBackground;

//! This class defines XLib window intended for creation of OpenGL context.
class Xw_Window : public Aspect_Window
{

public:

  //! Creates a XLib window defined by his position and size in pixels.
  //! Throws exception if window can not be created or Display do not support GLX extension.
  Standard_EXPORT Xw_Window (const Handle(Aspect_DisplayConnection)& theXDisplay,
                             const Standard_CString theTitle,
                             const Standard_Integer thePxLeft,
                             const Standard_Integer thePxTop,
                             const Standard_Integer thePxWidth,
                             const Standard_Integer thePxHeight,
                             const Aspect_FBConfig  theFBConfig = NULL);

  //! Creates a wrapper over existing Window handle
  Standard_EXPORT Xw_Window (const Handle(Aspect_DisplayConnection)& theXDisplay,
                             const Window theXWin,
                             const Aspect_FBConfig theFBConfig = NULL);

  //! Destroys the Window and all resourses attached to it
  Standard_EXPORT ~Xw_Window();

  //! Opens the window <me>
  Standard_EXPORT virtual void Map() const Standard_OVERRIDE;

  //! Closes the window <me>
  Standard_EXPORT virtual void Unmap() const Standard_OVERRIDE;

  //! Applies the resizing to the window <me>
  Standard_EXPORT virtual Aspect_TypeOfResize DoResize() const Standard_OVERRIDE;

  //! Apply the mapping change to the window <me>
  Standard_EXPORT virtual Standard_Boolean DoMapping() const Standard_OVERRIDE;

  //! Returns True if the window <me> is opened
  Standard_EXPORT virtual Standard_Boolean IsMapped() const Standard_OVERRIDE;

  //! Returns The Window RATIO equal to the physical WIDTH/HEIGHT dimensions
  Standard_EXPORT virtual Standard_Real Ratio() const Standard_OVERRIDE;

  //! Returns The Window POSITION in PIXEL
  Standard_EXPORT virtual void Position (Standard_Integer& X1,
                                         Standard_Integer& Y1,
                                         Standard_Integer& X2,
                                         Standard_Integer& Y2) const Standard_OVERRIDE;

  //! Returns The Window SIZE in PIXEL
  Standard_EXPORT virtual void Size (Standard_Integer& theWidth,
                                     Standard_Integer& theHeight) const Standard_OVERRIDE;

  //! @return native Window handle
  Standard_EXPORT Window XWindow() const;

  //! @return connection to X Display
  Standard_EXPORT const Handle(Aspect_DisplayConnection)& DisplayConnection() const;

  //! @return native Window handle
  virtual Aspect_Drawable NativeHandle() const Standard_OVERRIDE
  {
    return (Aspect_Drawable )XWindow();
  }

  //! @return parent of native Window handle
  virtual Aspect_Drawable NativeParentHandle() const Standard_OVERRIDE
  {
    return 0;
  }

  //! @return native Window FB config (GLXFBConfig on Xlib)
  virtual Aspect_FBConfig NativeFBConfig() const Standard_OVERRIDE
  {
    return myFBConfig;
  }

protected:

  Handle(Aspect_DisplayConnection) myDisplay; //!< X Display connection
  Window           myXWindow;  //!< XLib window handle
  Aspect_FBConfig  myFBConfig; //!< GLXFBConfig
  Standard_Integer myXLeft;    //!< left   position in pixels
  Standard_Integer myYTop;     //!< top    position in pixels
  Standard_Integer myXRight;   //!< right  position in pixels
  Standard_Integer myYBottom;  //!< bottom position in pixels
  Standard_Boolean myIsOwnWin; //!< flag to indicate own window handle (to be deallocated on destruction)

public:

  DEFINE_STANDARD_RTTIEXT(Xw_Window,Aspect_Window)

};

DEFINE_STANDARD_HANDLE(Xw_Window, Aspect_Window)

#endif //  Win32 or Mac OS X
#endif // _Xw_Window_H__
