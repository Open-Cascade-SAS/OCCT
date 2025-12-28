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

#include <Aspect_Background.hxx>
#include <Aspect_GradientBackground.hxx>
#include <Aspect_FBConfig.hxx>
#include <Aspect_FillMethod.hxx>
#include <Quantity_Color.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_TypeOfResize.hxx>
#include <Aspect_Drawable.hxx>
#include <NCollection_Vec2.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

class Aspect_DisplayConnection;

//! Defines a window.
class Aspect_Window : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Aspect_Window, Standard_Transient)
public:
  //! Returns True if the window <me> is virtual
  Standard_EXPORT bool IsVirtual() const;

  //! Setup the virtual state
  Standard_EXPORT void SetVirtual(const bool theVirtual);

  //! Returns window top-left corner.
  NCollection_Vec2<int> TopLeft() const
  {
    NCollection_Vec2<int> aTopLeft, aBotRight;
    Position(aTopLeft.x(), aTopLeft.y(), aBotRight.x(), aBotRight.y());
    return aTopLeft;
  }

  //! Returns window dimensions.
  NCollection_Vec2<int> Dimensions() const
  {
    NCollection_Vec2<int> aSize;
    Size(aSize.x(), aSize.y());
    return aSize;
  }

  //! Returns connection to Display or NULL.
  const occ::handle<Aspect_DisplayConnection>& DisplayConnection() const { return myDisplay; }

  //! Returns the window background.
  Standard_EXPORT Aspect_Background Background() const;

  //! Returns the current image background fill mode.
  Standard_EXPORT Aspect_FillMethod BackgroundFillMethod() const;

  //! Returns the window gradient background.
  Standard_EXPORT Aspect_GradientBackground GradientBackground() const;

  //! Modifies the window background.
  Standard_EXPORT void SetBackground(const Aspect_Background& theBack);

  //! Modifies the window background.
  Standard_EXPORT void SetBackground(const Quantity_Color& theColor);

  //! Modifies the window gradient background.
  Standard_EXPORT void SetBackground(const Aspect_GradientBackground& theBackground);

  //! Modifies the window gradient background.
  Standard_EXPORT void SetBackground(const Quantity_Color&           theFirstColor,
                                     const Quantity_Color&           theSecondColor,
                                     const Aspect_GradientFillMethod theFillMethod);

public:
  //! Returns True if the window <me> is opened
  //! and False if the window is closed.
  Standard_EXPORT virtual bool IsMapped() const = 0;

  //! Opens the window <me>.
  Standard_EXPORT virtual void Map() const = 0;

  //! Closes the window <me>.
  Standard_EXPORT virtual void Unmap() const = 0;

  //! Apply the resizing to the window <me>.
  Standard_EXPORT virtual Aspect_TypeOfResize DoResize() = 0;

  //! Apply the mapping change to the window <me>.
  //! and returns TRUE if the window is mapped at screen.
  Standard_EXPORT virtual bool DoMapping() const = 0;

  //! Returns The Window RATIO equal to the physical
  //! WIDTH/HEIGHT dimensions
  Standard_EXPORT virtual double Ratio() const = 0;

  //! Returns The Window POSITION in PIXEL
  Standard_EXPORT virtual void Position(int& X1,
                                        int& Y1,
                                        int& X2,
                                        int& Y2) const = 0;

  //! Returns The Window SIZE in PIXEL
  Standard_EXPORT virtual void Size(int& Width, int& Height) const = 0;

  //! Returns native Window handle (HWND on Windows, Window with Xlib, and so on)
  Standard_EXPORT virtual Aspect_Drawable NativeHandle() const = 0;

  //! Returns parent of native Window handle (HWND on Windows, Window with Xlib, and so on)
  Standard_EXPORT virtual Aspect_Drawable NativeParentHandle() const = 0;

  //! Returns native Window FB config (GLXFBConfig on Xlib)
  Standard_EXPORT virtual Aspect_FBConfig NativeFBConfig() const = 0;

  //! Sets window title.
  virtual void SetTitle(const TCollection_AsciiString& theTitle) { (void)theTitle; }

  //! Invalidate entire window content.
  //!
  //! Implementation is expected to allow calling this method from non-GUI thread,
  //! e.g. by queuing exposure event into window message queue or in other thread-safe manner.
  //!
  //! Optional display argument should be passed when called from non-GUI thread
  //! on platforms implementing thread-unsafe connections to display.
  //! NULL can be passed instead otherwise.
  virtual void InvalidateContent(const occ::handle<Aspect_DisplayConnection>& theDisp) { (void)theDisp; }

public:
  //! Return device pixel ratio (logical to backing store scale factor).
  virtual double DevicePixelRatio() const { return 1.0; }

  //! Convert point from logical units into backing store units.
  virtual NCollection_Vec2<double> ConvertPointToBacking(const NCollection_Vec2<double>& thePnt) const
  {
    return thePnt * DevicePixelRatio();
  }

  //! Convert point from backing store units to logical units.
  virtual NCollection_Vec2<double> ConvertPointFromBacking(const NCollection_Vec2<double>& thePnt) const
  {
    return thePnt / DevicePixelRatio();
  }

public:
  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

protected:
  //! Initializes the data of a Window.
  Standard_EXPORT Aspect_Window();

protected:
  occ::handle<Aspect_DisplayConnection> myDisplay; //!< Display connection
  Aspect_Background                MyBackground;
  Aspect_GradientBackground        MyGradientBackground;
  Aspect_FillMethod                MyBackgroundFillMethod;
  bool                 MyIsVirtual;
};

#endif // _Aspect_Window_HeaderFile
