// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Aspect_Window_HeaderFile
#define _Aspect_Window_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Aspect_Background.hxx>
#include <Aspect_GradientBackground.hxx>
#include <Aspect_FBConfig.hxx>
#include <Aspect_FillMethod.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <Quantity_Color.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_TypeOfResize.hxx>
#include <Standard_Integer.hxx>
#include <Aspect_Drawable.hxx>
class Aspect_WindowDefinitionError;
class Aspect_WindowError;
class Aspect_Background;
class Aspect_GradientBackground;

class Aspect_Window;
DEFINE_STANDARD_HANDLE(Aspect_Window, Standard_Transient)

//! Defines a window.
class Aspect_Window : public Standard_Transient
{

public:
  
  //! Modifies the window background.
  Standard_EXPORT void SetBackground (const Aspect_Background& ABack);

  //! Modifies the window background.
  Standard_EXPORT void SetBackground (const Quantity_Color& color);
  
  //! Modifies the window gradient background.
  Standard_EXPORT void SetBackground (const Aspect_GradientBackground& ABackground);
  
  //! Modifies the window gradient background.
  Standard_EXPORT void SetBackground (const Quantity_Color& theFirstColor, const Quantity_Color& theSecondColor, const Aspect_GradientFillMethod theFillMethod);
  
  //! Opens the window <me>.
  Standard_EXPORT virtual void Map() const = 0;
  
  //! Closes the window <me>.
  Standard_EXPORT virtual void Unmap() const = 0;
  
  //! Apply the resizing to the window <me>.
  Standard_EXPORT virtual Aspect_TypeOfResize DoResize() const = 0;
  
  //! Apply the mapping change to the window <me>.
  //! and returns TRUE if the window is mapped at screen.
  Standard_EXPORT virtual Standard_Boolean DoMapping() const = 0;
  
  //! Returns the window background.
  Standard_EXPORT Aspect_Background Background() const;
  
  //! Returns the current image background fill mode.
  Standard_EXPORT Aspect_FillMethod BackgroundFillMethod() const;
  
  //! Returns the window gradient background.
  Standard_EXPORT Aspect_GradientBackground GradientBackground() const;
  
  //! Returns True if the window <me> is opened
  //! and False if the window is closed.
  Standard_EXPORT virtual Standard_Boolean IsMapped() const = 0;
  
  //! Returns True if the window <me> is virtual
  Standard_EXPORT Standard_Boolean IsVirtual() const;
  
  //! Setup the virtual state
  Standard_EXPORT void SetVirtual (const Standard_Boolean theVirtual);
  
  //! Returns The Window RATIO equal to the physical
  //! WIDTH/HEIGHT dimensions
  Standard_EXPORT virtual Standard_Real Ratio() const = 0;
  
  //! Returns The Window POSITION in PIXEL
  Standard_EXPORT virtual void Position (Standard_Integer& X1, Standard_Integer& Y1, Standard_Integer& X2, Standard_Integer& Y2) const = 0;
  
  //! Returns The Window SIZE in PIXEL
  Standard_EXPORT virtual void Size (Standard_Integer& Width, Standard_Integer& Height) const = 0;
  
  //! Returns native Window handle (HWND on Windows, Window with Xlib, and so on)
  Standard_EXPORT virtual Aspect_Drawable NativeHandle() const = 0;
  
  //! Returns parent of native Window handle (HWND on Windows, Window with Xlib, and so on)
  Standard_EXPORT virtual Aspect_Drawable NativeParentHandle() const = 0;

  //! Returns native Window FB config (GLXFBConfig on Xlib)
  Standard_EXPORT virtual Aspect_FBConfig NativeFBConfig() const = 0;

  DEFINE_STANDARD_RTTIEXT(Aspect_Window,Standard_Transient)

protected:

  //! Initializes the datas of a Window.
  Standard_EXPORT Aspect_Window();

protected:

  Aspect_Background MyBackground;
  Aspect_GradientBackground MyGradientBackground;
  Aspect_FillMethod MyBackgroundFillMethod;
  Standard_Boolean MyIsVirtual;

};

#endif // _Aspect_Window_HeaderFile
