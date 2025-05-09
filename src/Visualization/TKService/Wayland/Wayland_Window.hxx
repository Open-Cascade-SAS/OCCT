// Copyright (c) 2024 Kirill Gavrilov
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of
// commercial license or contractual agreement.

#ifndef Wayland_Window_HeaderFile
#define Wayland_Window_HeaderFile

#include <Aspect_Window.hxx>

#include <Aspect_VKey.hxx>

class Wayland_DisplayConnection;
class Aspect_WindowInputListener;

struct wl_egl_window;
struct wl_region;
struct wl_surface;

/// TODO - put into namespace?
struct xdg_surface;
struct xdg_toplevel;

//! This class defines Wayland window intended for creation of OpenGL context.
class Wayland_Window : public Aspect_Window
{
  DEFINE_STANDARD_RTTIEXT(Wayland_Window, Aspect_Window)
public:

  //! Convert X11 virtual key (KeySym) into Aspect_VKey.
  Standard_EXPORT static Aspect_VKey VirtualKeyFromNative (unsigned long theKey);

public:

  //! Creates a XLib window defined by his position and size in pixels.
  //! Throws exception if window can not be created or Display do not support GLX extension.
  Standard_EXPORT Wayland_Window (const Handle(Wayland_DisplayConnection)& theDisplay,
                                  const Standard_CString theTitle,
                                  const Standard_Integer thePxLeft,
                                  const Standard_Integer thePxTop,
                                  const Standard_Integer thePxWidth,
                                  const Standard_Integer thePxHeight);

  //! Creates a wrapper over existing window handle
  Standard_EXPORT Wayland_Window (const Handle(Wayland_DisplayConnection)& theDisplay,
                                  const Aspect_Drawable theWlWin);

  //! Destroys the window and all resources attached to it
  Standard_EXPORT ~Wayland_Window();

  //! Returns connection to Display.
  Standard_EXPORT virtual const Handle(Aspect_DisplayConnection)& DisplayConnection() const Standard_OVERRIDE;

  //! Opens the window.
  Standard_EXPORT virtual void Map() const Standard_OVERRIDE;

  //! Closes the window.
  Standard_EXPORT virtual void Unmap() const Standard_OVERRIDE;

  //! Applies the resizing to the window.
  Standard_EXPORT virtual Aspect_TypeOfResize DoResize() Standard_OVERRIDE;

  //! Apply the mapping change to the window.
  virtual Standard_Boolean DoMapping() const Standard_OVERRIDE
  {
    return Standard_True; // IsMapped()
  }

  //! Returns True if the window is opened.
  Standard_EXPORT virtual Standard_Boolean IsMapped() const Standard_OVERRIDE;

  //! Returns The Window RATIO equal to the physical WIDTH/HEIGHT dimensions.
  Standard_EXPORT virtual Standard_Real Ratio() const Standard_OVERRIDE;

  //! Returns The Window POSITION in PIXEL
  Standard_EXPORT virtual void Position (Standard_Integer& theX1,
                                         Standard_Integer& theY1,
                                         Standard_Integer& theX2,
                                         Standard_Integer& theY2) const Standard_OVERRIDE;

  //! Returns The Window SIZE in PIXEL.
  Standard_EXPORT virtual void Size (Standard_Integer& theWidth,
                                     Standard_Integer& theHeight) const Standard_OVERRIDE;

  //! @return native window handle.
  Aspect_Drawable XWindow() const { return myXWindow; }

  //! @return native window handle
  virtual Aspect_Drawable NativeHandle() const Standard_OVERRIDE
  {
    return myXWindow;
  }

  //! @return parent of native window handle.
  virtual Aspect_Drawable NativeParentHandle() const Standard_OVERRIDE { return 0; }

  //! @return native Window FB config
  virtual Aspect_FBConfig NativeFBConfig() const Standard_OVERRIDE { return 0; }

  //! Sets window title.
  Standard_EXPORT virtual void SetTitle (const TCollection_AsciiString& theTitle) Standard_OVERRIDE;

  //! Invalidate entire window content through generation of Expose event.
  Standard_EXPORT virtual void InvalidateContent (const Handle(Aspect_DisplayConnection)& theDisp) Standard_OVERRIDE;

  //! Process a single window message.
  //! @param[in][out] theListener listener to redirect message
  //! @param[in][out] theMsg  message to process
  //! @return TRUE if message has been processed
  //Standard_EXPORT virtual bool ProcessMessage (Aspect_WindowInputListener& theListener,
  //                                             XEvent& theMsg);

protected:

  Handle(Wayland_DisplayConnection) myDisplay; //!< Display connection

  struct wl_surface*    myWlSurface = nullptr;
  struct wl_region*     myWlRegion  = nullptr;
  struct wl_egl_window* myEglWindow = nullptr;
  struct xdg_surface*   myXdgSurf   = nullptr;
  struct xdg_toplevel*  myXdgTop    = nullptr;

  Aspect_Drawable  myXWindow  = 0;       //!< window handle
  Standard_Integer myXLeft    = 0;       //!< left   position in pixels
  Standard_Integer myYTop     = 0;       //!< top    position in pixels
  Standard_Integer myXRight   = 512;     //!< right  position in pixels
  Standard_Integer myYBottom  = 512;     //!< bottom position in pixels
  Standard_Boolean myIsOwnWin = false;   //!< flag to indicate own window handle (to be deallocated on destruction)

};

#endif // Wayland_Window_HeaderFile
