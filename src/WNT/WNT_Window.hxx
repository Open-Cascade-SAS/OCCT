// Created on: 1996-01-26
// Created by: PLOTNIKOV Eugeny
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _WNT_Window_HeaderFile
#define _WNT_Window_HeaderFile

#include <Standard.hxx>

#if defined(_WIN32) && !defined(OCCT_UWP)

#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <Aspect_Handle.hxx>
#include <Standard_Boolean.hxx>
#include <Aspect_Window.hxx>
#include <Standard_CString.hxx>
#include <WNT_Dword.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Address.hxx>
#include <Aspect_TypeOfResize.hxx>
#include <Aspect_Drawable.hxx>
class WNT_WClass;
class Aspect_WindowDefinitionError;
class Aspect_WindowError;


class WNT_Window;
DEFINE_STANDARD_HANDLE(WNT_Window, Aspect_Window)

//! This class defines Windows NT window
class WNT_Window : public Aspect_Window
{

public:

  
  //! Creates a Window defined by his position and size
  //! in pixles from the Parent Window.
  //! Trigger: Raises WindowDefinitionError if the Position out of the
  //! Screen Space or the window creation failed.
  Standard_EXPORT WNT_Window(const Standard_CString theTitle, const Handle(WNT_WClass)& theClass, const WNT_Dword& theStyle, const Standard_Integer thePxLeft, const Standard_Integer thePxTop, const Standard_Integer thePxWidth, const Standard_Integer thePxHeight, const Quantity_NameOfColor theBackColor = Quantity_NOC_MATRAGRAY, const Aspect_Handle theParent = 0, const Aspect_Handle theMenu = 0, const Standard_Address theClientStruct = 0);
  
  //! Creates a Window based on the existing window handle.
  //! This handle equals ( aPart1 << 16 ) + aPart2.
  Standard_EXPORT WNT_Window(const Aspect_Handle aHandle, const Quantity_NameOfColor aBackColor = Quantity_NOC_MATRAGRAY);
  
  //! Destroys the Window and all resourses attached to it.
  Standard_EXPORT ~WNT_Window();
  
  //! Sets cursor <aCursor> for ENTIRE WINDOW CLASS to which
  //! the Window belongs.
  Standard_EXPORT void SetCursor (const Aspect_Handle aCursor) const;
  
  //! Opens the window <me>.
  Standard_EXPORT virtual void Map() const Standard_OVERRIDE;
  
  //! Opens a window <me> according to <aMapMode>.
  //! This method is specific to Windows NT.
  //! <aMapMode> can be one of SW_xxx constants defined
  //! in <windows.h>. See documentation.
  Standard_EXPORT void Map (const Standard_Integer aMapMode) const;
  
  //! Closes the window <me>.
  Standard_EXPORT virtual void Unmap() const Standard_OVERRIDE;
  
  //! Applies the resizing to the window <me>.
  Standard_EXPORT virtual Aspect_TypeOfResize DoResize() const Standard_OVERRIDE;
  
  //! Apply the mapping change to the window <me>
  //! and returns TRUE if the window is mapped at screen.
  virtual Standard_Boolean DoMapping() const Standard_OVERRIDE;
  
  //! Changes variables due to window position.
  Standard_EXPORT void SetPos (const Standard_Integer X, const Standard_Integer Y, const Standard_Integer X1, const Standard_Integer Y1);
  
  //! Returns True if the window <me> is opened
  //! and False if the window is closed.
  Standard_EXPORT virtual Standard_Boolean IsMapped() const Standard_OVERRIDE;
  
  //! Returns The Window RATIO equal to the physical
  //! WIDTH/HEIGHT dimensions.
  Standard_EXPORT virtual Standard_Real Ratio() const Standard_OVERRIDE;
  
  //! Returns The Window POSITION in PIXEL
  Standard_EXPORT virtual void Position (Standard_Integer& X1, Standard_Integer& Y1, Standard_Integer& X2, Standard_Integer& Y2) const Standard_OVERRIDE;
  
  //! Returns The Window SIZE in PIXEL
  Standard_EXPORT virtual void Size (Standard_Integer& Width, Standard_Integer& Height) const Standard_OVERRIDE;
  
  //! Returns the Windows NT handle of the created window <me>.
  Aspect_Handle HWindow() const;
  
  //! Returns the Windows NT handle parent of the created window <me>.
  Aspect_Handle HParentWindow() const;
  
  //! Returns native Window handle (HWND)
  virtual Aspect_Drawable NativeHandle() const Standard_OVERRIDE;
  
  //! Returns parent of native Window handle (HWND on Windows, Window with Xlib, and so on)
  virtual Aspect_Drawable NativeParentHandle() const Standard_OVERRIDE;

  //! Returns nothing on Windows
  virtual Aspect_FBConfig NativeFBConfig() const Standard_OVERRIDE { return NULL; }

  DEFINE_STANDARD_RTTIEXT(WNT_Window,Aspect_Window)

protected:

  Standard_Integer aXLeft;
  Standard_Integer aYTop;
  Standard_Integer aXRight;
  Standard_Integer aYBottom;
  Handle(WNT_WClass) myWClass;
  Aspect_Handle myHWindow;
  Aspect_Handle myHParentWindow;
  Standard_Boolean myIsForeign;

};

#include <WNT_Window.lxx>

#endif // _WIN32
#endif // _WNT_Window_HeaderFile
