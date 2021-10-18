// Created on: 2012-11-12
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef Cocoa_Window_HeaderFile
#define Cocoa_Window_HeaderFile

#if defined(__APPLE__)
  #import <TargetConditionals.h>
#endif

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  #ifdef __OBJC__
    @class UIView;
    @class UIWindow;
  #else
    struct UIView;
    struct UIWindow;
  #endif
#else
  #ifdef __OBJC__
    @class NSView;
    @class NSWindow;
  #else
    struct NSView;
    struct NSWindow;
  #endif
#endif

#include <Aspect_Window.hxx>

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Aspect_FillMethod.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_TypeOfResize.hxx>
#include <Aspect_VKey.hxx>
#include <Quantity_NameOfColor.hxx>

class Aspect_WindowDefinitionError;
class Aspect_WindowError;
class Aspect_Background;
class Quantity_Color;
class Aspect_GradientBackground;

//! This class defines Cocoa window
class Cocoa_Window : public Aspect_Window
{
public:

  //! Convert Carbon virtual key into Aspect_VKey.
  Standard_EXPORT static Aspect_VKey VirtualKeyFromNative (Standard_Integer theKey);

public:

  //! Creates a NSWindow and NSView defined by his position and size in pixels
  Standard_EXPORT Cocoa_Window (const Standard_CString theTitle,
                                const Standard_Integer thePxLeft,
                                const Standard_Integer thePxTop,
                                const Standard_Integer thePxWidth,
                                const Standard_Integer thePxHeight);

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  //! Creates a wrapper over existing UIView handle
  Standard_EXPORT Cocoa_Window (UIView* theViewUI);
#else
  //! Creates a wrapper over existing NSView handle
  Standard_EXPORT Cocoa_Window (NSView* theViewNS);
#endif

  //! Destroys the Window and all resourses attached to it
  Standard_EXPORT ~Cocoa_Window();

  //! Opens the window <me>
  Standard_EXPORT virtual void Map() const Standard_OVERRIDE;

  //! Closes the window <me>
  Standard_EXPORT virtual void Unmap() const Standard_OVERRIDE;

  //! Applies the resizing to the window <me>
  Standard_EXPORT virtual Aspect_TypeOfResize DoResize() Standard_OVERRIDE;

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

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  //! @return associated UIView
  UIView* HView() const { return myHView; }

  //! Setup new UIView.
  Standard_EXPORT void SetHView (UIView* theView);
#else
  //! @return associated NSView
  NSView* HView() const { return myHView; }

  //! Setup new NSView.
  Standard_EXPORT void SetHView (NSView* theView);
#endif

  //! @return native Window handle
  virtual Aspect_Drawable NativeHandle() const Standard_OVERRIDE
  {
    return (Aspect_Drawable )HView();
  }

  //! @return parent of native Window handle
  virtual Aspect_Drawable NativeParentHandle() const Standard_OVERRIDE
  {
    return 0;
  }

  //! Returns nothing on OS X
  virtual Aspect_FBConfig NativeFBConfig() const Standard_OVERRIDE { return NULL; }

  //! Sets window title.
  Standard_EXPORT virtual void SetTitle (const TCollection_AsciiString& theTitle) Standard_OVERRIDE;

  //! Invalidate entire window content by setting NSView::setNeedsDisplay property.
  //! Call will be implicitly redirected to the main thread when called from non-GUI thread.
  Standard_EXPORT virtual void InvalidateContent (const Handle(Aspect_DisplayConnection)& theDisp = NULL) Standard_OVERRIDE;

protected:

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  UIView*          myHView;
#else
  NSWindow*        myHWindow;
  NSView*          myHView;
#endif
  Standard_Integer myXLeft;
  Standard_Integer myYTop;
  Standard_Integer myXRight;
  Standard_Integer myYBottom;

public:

  DEFINE_STANDARD_RTTIEXT(Cocoa_Window,Aspect_Window)

};

DEFINE_STANDARD_HANDLE(Cocoa_Window, Aspect_Window)

#endif // _Cocoa_Window_H__
