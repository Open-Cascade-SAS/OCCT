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

#ifndef Xw_Window_HeaderFile
#define Xw_Window_HeaderFile

#include <Aspect_Window.hxx>

#include <Aspect_VKey.hxx>

class Aspect_DisplayConnection;
class Aspect_WindowInputListener;

typedef union _XEvent XEvent;

//! This class defines XLib window intended for creation of OpenGL context.
class Xw_Window : public Aspect_Window
{
  DEFINE_STANDARD_RTTIEXT(Xw_Window, Aspect_Window)
public:
  //! Convert X11 virtual key (KeySym) into Aspect_VKey.
  Standard_EXPORT static Aspect_VKey VirtualKeyFromNative(unsigned long theKey);

public:
  //! Creates a XLib window defined by his position and size in pixels.
  //! Throws exception if window can not be created or Display do not support GLX extension.
  Standard_EXPORT Xw_Window(const occ::handle<Aspect_DisplayConnection>& theXDisplay,
                            const char* const                            theTitle,
                            const int                                    thePxLeft,
                            const int                                    thePxTop,
                            const int                                    thePxWidth,
                            const int                                    thePxHeight);

  //! Creates a wrapper over existing Window handle
  Standard_EXPORT Xw_Window(const occ::handle<Aspect_DisplayConnection>& theXDisplay,
                            const Aspect_Drawable                        theXWin,
                            const Aspect_FBConfig                        theFBConfig = nullptr);

  //! Destroys the Window and all resources attached to it
  Standard_EXPORT ~Xw_Window() override;

  //! Opens the window <me>
  Standard_EXPORT void Map() const override;

  //! Closes the window <me>
  Standard_EXPORT void Unmap() const override;

  //! Applies the resizing to the window <me>
  Standard_EXPORT Aspect_TypeOfResize DoResize() override;

  //! Apply the mapping change to the window <me>
  bool DoMapping() const override
  {
    return true; // IsMapped()
  }

  //! Returns True if the window <me> is opened
  Standard_EXPORT bool IsMapped() const override;

  //! Returns The Window RATIO equal to the physical WIDTH/HEIGHT dimensions
  Standard_EXPORT double Ratio() const override;

  //! Returns The Window POSITION in PIXEL
  Standard_EXPORT void Position(int& X1, int& Y1, int& X2, int& Y2) const override;

  //! Returns The Window SIZE in PIXEL
  Standard_EXPORT void Size(int& theWidth, int& theHeight) const override;

  //! @return native Window handle
  Aspect_Drawable XWindow() const { return myXWindow; }

  //! @return native Window handle
  Aspect_Drawable NativeHandle() const override { return myXWindow; }

  //! @return parent of native Window handle
  Aspect_Drawable NativeParentHandle() const override { return 0; }

  //! @return native Window FB config (GLXFBConfig on Xlib)
  Aspect_FBConfig NativeFBConfig() const override { return myFBConfig; }

  //! Sets window title.
  Standard_EXPORT void SetTitle(const TCollection_AsciiString& theTitle) override;

  //! Invalidate entire window content through generation of Expose event.
  //! This method does not aggregate multiple calls into single event - dedicated event will be sent
  //! on each call. When NULL display connection is specified, the connection specified on window
  //! creation will be used. Sending exposure messages from non-window thread would require
  //! dedicated display connection opened specifically for this working thread to avoid race
  //! conditions, since Xlib display connection is not thread-safe by default.
  Standard_EXPORT void InvalidateContent(
    const occ::handle<Aspect_DisplayConnection>& theDisp) override;

  //! Process a single window message.
  //! @param[in][out] theListener  listener to redirect message
  //! @param[in][out] theMsg message to process
  //! @return TRUE if message has been processed
  Standard_EXPORT virtual bool ProcessMessage(Aspect_WindowInputListener& theListener,
                                              XEvent&                     theMsg);

protected:
  Aspect_Drawable myXWindow;  //!< XLib window handle
  Aspect_FBConfig myFBConfig; //!< GLXFBConfig
  int             myXLeft;    //!< left   position in pixels
  int             myYTop;     //!< top    position in pixels
  int             myXRight;   //!< right  position in pixels
  int             myYBottom;  //!< bottom position in pixels
  // clang-format off
  bool myIsOwnWin; //!< flag to indicate own window handle (to be deallocated on destruction)
  // clang-format on
};

#endif // _Xw_Window_H__
