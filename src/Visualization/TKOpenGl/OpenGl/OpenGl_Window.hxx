// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_Window_HeaderFile
#define OpenGl_Window_HeaderFile

#include <Aspect_RenderingContext.hxx>
#include <Aspect_Window.hxx>
#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Caps.hxx>
#include <Standard_Transient.hxx>

#if defined(__APPLE__)
  #import <TargetConditionals.h>
#endif

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  #ifdef __OBJC__
@class UIView;
  #else
struct UIView;
  #endif
#endif

class OpenGl_Context;
class OpenGl_GraphicDriver;

//! This class represents low-level wrapper over window with GL context.
//! The window itself should be provided to constructor.
class OpenGl_Window : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_Window, Standard_Transient)
public:
  //! Empty constructor.
  Standard_EXPORT OpenGl_Window();

  //! Initialize the new window - prepare GL context for specified window.
  //! Throws exception in case of failure.
  Standard_EXPORT void Init(const occ::handle<OpenGl_GraphicDriver>& theDriver,
                            const occ::handle<Aspect_Window>&        thePlatformWindow,
                            const occ::handle<Aspect_Window>&        theSizeWindow,
                            Aspect_RenderingContext                  theGContext,
                            const occ::handle<OpenGl_Caps>&          theCaps,
                            const occ::handle<OpenGl_Context>&       theShareCtx);

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_Window();

  //! Resizes the window.
  Standard_EXPORT virtual void Resize();

  //! Return platform window.
  const occ::handle<Aspect_Window>& PlatformWindow() { return myPlatformWindow; }

  //! Return window object defining dimensions.
  const occ::handle<Aspect_Window>& SizeWindow() { return mySizeWindow; }

  int Width() const { return mySize.x(); }

  int Height() const { return mySize.y(); }

  //! Return OpenGL context.
  const occ::handle<OpenGl_Context>& GetGlContext() const { return myGlContext; }

  //! Makes GL context for this window active in current thread
  Standard_EXPORT virtual bool Activate();

  //! Sets swap interval for this window according to the context's settings.
  Standard_EXPORT void SetSwapInterval(bool theToForceNoSync);

protected:
  //! Activates GL context and setup viewport.
  Standard_EXPORT void init();

protected:
  occ::handle<OpenGl_Context> myGlContext;
  // clang-format off
  bool       myOwnGContext;    //!< set to TRUE if GL context was not created by this class
  // clang-format on
  occ::handle<Aspect_Window> myPlatformWindow; //!< software platform window wrapper
  occ::handle<Aspect_Window> mySizeWindow;     //!< window object defining dimensions
#if defined(__APPLE__)
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  UIView* myUIView;
  #endif
  NCollection_Vec2<int> mySizePt; //!< window width x height in logical units
#endif
  NCollection_Vec2<int> mySize; //!< window width x height in pixels

  int mySwapInterval; //!< last assigned swap interval (VSync) for this window
};

#endif //_OpenGl_Window_Header
