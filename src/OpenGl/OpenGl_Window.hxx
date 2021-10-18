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

class OpenGl_Window;
DEFINE_STANDARD_HANDLE(OpenGl_Window,Standard_Transient)

//! This class represents low-level wrapper over window with GL context.
//! The window itself should be provided to constructor.
class OpenGl_Window : public Standard_Transient
{
public:

  //! Main constructor - prepare GL context for specified window.
  Standard_EXPORT OpenGl_Window (const Handle(OpenGl_GraphicDriver)& theDriver,
                                 const Handle(Aspect_Window)&  thePlatformWindow,
                                 Aspect_RenderingContext       theGContext,
                                 const Handle(OpenGl_Caps)&    theCaps,
                                 const Handle(OpenGl_Context)& theShareCtx);

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_Window();

  //! Resizes the window.
  Standard_EXPORT virtual void Resize();

  Handle(Aspect_Window) PlatformWindow() { return myPlatformWindow; }

  Standard_Integer Width()  const { return myWidth; }
  Standard_Integer Height() const { return myHeight; }

  const Handle(OpenGl_Context)& GetGlContext() const { return myGlContext; }

  //! Activates GL context and setup viewport.
  Standard_EXPORT void Init();

  //! Makes GL context for this window active in current thread
  Standard_EXPORT virtual Standard_Boolean Activate();

  //! Sets swap interval for this window according to the context's settings.
  Standard_EXPORT void SetSwapInterval (Standard_Boolean theToForceNoSync);

protected:

  Handle(OpenGl_Context) myGlContext;
  Standard_Boolean       myOwnGContext; //!< set to TRUE if GL context was not created by this class
  Handle(Aspect_Window)  myPlatformWindow; //!< software platform window wrapper
#if defined(__APPLE__)
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  UIView*                myUIView;
#endif
  Standard_Integer       myWidthPt;     //!< window width  in logical units
  Standard_Integer       myHeightPt;    //!< window height in logical units
#endif
  Standard_Integer       myWidth;       //!< window width  in pixels
  Standard_Integer       myHeight;      //!< window height in pixels

  Standard_Integer       mySwapInterval;//!< last assigned swap interval (VSync) for this window

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_Window,Standard_Transient) // Type definition
  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_Window_Header
