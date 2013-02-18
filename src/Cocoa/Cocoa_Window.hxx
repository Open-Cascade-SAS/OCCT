// Created on: 2012-11-12
// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 65 (the "License") You may not use the content of this file
// except in compliance with the License Please obtain a copy of the License
// at http://wwwopencascadeorg and read it completely before using this file
//
// The Initial Developer of the Original Code is Open CASCADE SAS, having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement Please see the License for the specific terms
// and conditions governing the rights and limitations under the License

#ifndef _Cocoa_Window_H__
#define _Cocoa_Window_H__

#ifdef __OBJC__
  @class NSView;
  @class NSWindow;
#else
  struct NSView;
  struct NSWindow;
#endif

#include <Aspect_Window.hxx>

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

#include <Aspect_FillMethod.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_TypeOfResize.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Quantity_Parameter.hxx>
#include <Quantity_Ratio.hxx>

class Aspect_WindowDefinitionError;
class Aspect_WindowError;
class Aspect_Background;
class Quantity_Color;
class Aspect_GradientBackground;

//! This class defines Cocoa window
class Cocoa_Window : public Aspect_Window
{

public:

  //! Creates a NSWindow and NSView defined by his position and size in pixels
  Standard_EXPORT Cocoa_Window (const Standard_CString theTitle,
                                const Standard_Integer thePxLeft,
                                const Standard_Integer thePxTop,
                                const Standard_Integer thePxWidth,
                                const Standard_Integer thePxHeight);

  //! Creates a wrapper over existing NSView handle
  Standard_EXPORT Cocoa_Window (NSView* theViewNS);

  //! Destroies the Window and all resourses attached to it
  Standard_EXPORT virtual  void Destroy();

  ~Cocoa_Window()
  {
    Destroy();
  }

  //! Opens the window <me>
  Standard_EXPORT virtual void Map() const;

  //! Closes the window <me>
  Standard_EXPORT virtual void Unmap() const;

  //! Applies the resizing to the window <me>
  Standard_EXPORT virtual Aspect_TypeOfResize DoResize() const;

  //! Apply the mapping change to the window <me>
  Standard_EXPORT virtual Standard_Boolean DoMapping() const;

  //! Returns True if the window <me> is opened
  Standard_EXPORT virtual Standard_Boolean IsMapped() const;

  //! Returns The Window RATIO equal to the physical WIDTH/HEIGHT dimensions
  Standard_EXPORT virtual Quantity_Ratio Ratio() const;

  //! Returns The Window POSITION in PIXEL
  Standard_EXPORT virtual void Position (Standard_Integer& X1,
                                         Standard_Integer& Y1,
                                         Standard_Integer& X2,
                                         Standard_Integer& Y2) const;

  //! Returns The Window SIZE in PIXEL
  Standard_EXPORT virtual void Size (Standard_Integer& theWidth,
                                     Standard_Integer& theHeight) const;

  //! @return associated NSView
  Standard_EXPORT NSView* HView() const;

  //! Setup new NSView.
  Standard_EXPORT void SetHView (NSView* theView);

protected:

  NSWindow*        myHWindow;
  NSView*          myHView;
  Standard_Integer myXLeft;
  Standard_Integer myYTop;
  Standard_Integer myXRight;
  Standard_Integer myYBottom;

public:

  DEFINE_STANDARD_RTTI(Cocoa_Window)

};

DEFINE_STANDARD_HANDLE(Cocoa_Window, Aspect_Window)

#endif // _Cocoa_Window_H__
