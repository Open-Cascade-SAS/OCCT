// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _Aspect_NeutralWindow_HeaderFile
#define _Aspect_NeutralWindow_HeaderFile

#include <Aspect_Window.hxx>

//! Defines a platform-neutral window.
//! This class is intended to be used in context when window management (including OpenGL context
//! creation) is performed on application side (e.g. using external framework).
//!
//! Window properties should be managed by application and assigned to this class as properties.
class Aspect_NeutralWindow : public Aspect_Window
{
  DEFINE_STANDARD_RTTIEXT(Aspect_NeutralWindow, Aspect_Window)
public:
  //! Empty constructor.
  //! Note that window is considered "mapped" by default.
  Standard_EXPORT Aspect_NeutralWindow();

  //! Return native handle of this drawable.
  virtual Aspect_Drawable NativeHandle() const override { return myHandle; }

  //! Return native handle of the parent drawable.
  virtual Aspect_Drawable NativeParentHandle() const override { return myParentHandle; }

  //! Return FBConfig.
  virtual Aspect_FBConfig NativeFBConfig() const override { return myFBConfig; }

  //! Set native handle.
  //! @return true if definition has been changed
  bool SetNativeHandle(Aspect_Drawable theWindow)
  {
    return SetNativeHandles(theWindow, 0, 0);
  }

  //! Set native handles.
  //! @return true if definition has been changed
  Standard_EXPORT bool SetNativeHandles(Aspect_Drawable theWindow,
                                                    Aspect_Drawable theParentWindow,
                                                    Aspect_FBConfig theFbConfig);

  //! Return true if window is not hidden.
  virtual bool IsMapped() const override { return myIsMapped; }

  //! Change window mapped flag to TRUE.
  virtual void Map() const override { myIsMapped = true; }

  //! Change window mapped flag to FALSE.
  virtual void Unmap() const override { myIsMapped = false; }

  //! Resize window - do nothing.
  virtual Aspect_TypeOfResize DoResize() override { return Aspect_TOR_UNKNOWN; }

  //! Map window - do nothing.
  virtual bool DoMapping() const override { return true; }

  //! Returns window ratio equal to the physical width/height dimensions.
  virtual double Ratio() const override
  {
    return (myWidth != 0 && myHeight != 0) ? double(myWidth) / double(myHeight) : 1.0;
  }

  //! Return the window position.
  virtual void Position(int& theX1,
                        int& theY1,
                        int& theX2,
                        int& theY2) const override
  {
    theX1 = myPosX;
    theX2 = myPosX + myWidth;
    theY1 = myPosY;
    theY2 = myPosY + myHeight;
  }

  //! Set the window position.
  //! @return true if position has been changed
  Standard_EXPORT bool SetPosition(int theX1, int theY1);

  //! Set the window position.
  //! @return true if position has been changed
  Standard_EXPORT bool SetPosition(int theX1,
                                               int theY1,
                                               int theX2,
                                               int theY2);

  //! Return the window size.
  virtual void Size(int& theWidth, int& theHeight) const override
  {
    theWidth  = myWidth;
    theHeight = myHeight;
  }

  //! Set the window size.
  //! @return true if size has been changed
  Standard_EXPORT bool SetSize(const int theWidth,
                                           const int theHeight);

protected:
  Aspect_Drawable          myHandle;
  Aspect_Drawable          myParentHandle;
  Aspect_FBConfig          myFBConfig;
  int         myPosX;
  int         myPosY;
  int         myWidth;
  int         myHeight;
  mutable bool myIsMapped;
};

#endif // _Aspect_NeutralWindow_HeaderFile
