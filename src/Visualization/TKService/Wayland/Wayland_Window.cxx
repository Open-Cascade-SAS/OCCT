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

#include <Wayland_Window.hxx>

#include <Aspect_ScrollDelta.hxx>
#include <Aspect_WindowDefinitionError.hxx>
#include <Aspect_WindowInputListener.hxx>
#include <Message.hxx>

#if defined(HAVE_WAYLAND)
  #include <wayland-client-core.h>
  #include <wayland-client.h>
  #include <wayland-server.h>
  #include <wayland-client-protocol.h>
  #include <wayland-egl.h> // should be included before EGL headers

  // generated from xdg-shell.xml
  #include "xdg-shell-client-protocol.pxx"
#endif

#include <Wayland_DisplayConnection.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Wayland_Window, Aspect_Window)

#if defined(HAVE_WAYLAND)
static void xdg_surface_configure (void* theData,
                                   struct xdg_surface* theXdgSurf,
                                   uint32_t theSerial)
{
  (void )theData;
  // just confirm existence to the compositor
  xdg_surface_ack_configure (theXdgSurf, theSerial);
}

static void xdg_toplevel_handle_configure (void* theData,
                                           struct xdg_toplevel* theXdgTop,
                                           int32_t theWidth,
                                           int32_t theHeight,
                                           struct wl_array* theStates)
{
  if (theWidth == 0 && theHeight == 0)
  {
    return;
  }

  /*if (myWidth != theWidth && myHeight != theHeight)
  {
    myWidth = theWidth;
    myHeight = theHeight;
    wl_egl_window_resize (myEglWindow, theWidth, theHeight, 0, 0);
    wl_surface_commit (myWlSurface);
  }*/
}

static void xdg_toplevel_handle_close (void* theData,
                                       struct xdg_toplevel* theXdgTop)
{
  // window is closed
}
#endif

// =======================================================================
// function : Wayland_Window
// purpose  :
// =======================================================================
Wayland_Window::Wayland_Window (const Handle(Wayland_DisplayConnection)& theDisplay,
                                const Standard_CString theTitle,
                                const Standard_Integer thePxLeft,
                                const Standard_Integer thePxTop,
                                const Standard_Integer thePxWidth,
                                const Standard_Integer thePxHeight)
: myXLeft    (thePxLeft),
  myYTop     (thePxTop),
  myXRight   (thePxLeft + thePxWidth),
  myYBottom  (thePxTop + thePxHeight),
  myIsOwnWin (Standard_True)
{
  myDisplay = theDisplay;
  if (thePxWidth <= 0 || thePxHeight <= 0)
  {
    throw Aspect_WindowDefinitionError ("Wayland_Window, Coordinate(s) out of range");
  }
  else if (theDisplay.IsNull())
  {
    throw Aspect_WindowDefinitionError ("Wayland_Window, Display connection is undefined");
  }

#if defined(HAVE_WAYLAND)

  myWlSurface = wl_compositor_create_surface (myDisplay->GetWlCompositor());
  if (myWlSurface == nullptr)
  {
    throw Aspect_WindowDefinitionError ("Wayland_Window, cannot create compositor surface");
  }

  myXdgSurf = xdg_wm_base_get_xdg_surface (myDisplay->GetXDGWMBase(), myWlSurface);

  static const struct xdg_surface_listener anXdgSurfList = { .configure = &xdg_surface_configure, };
  xdg_surface_add_listener (myXdgSurf, &anXdgSurfList, this);

  myXdgTop = xdg_surface_get_toplevel (myXdgSurf);
  xdg_toplevel_set_title (myXdgTop, theTitle);

  static const struct xdg_toplevel_listener anXdgTopList =
  {
    .configure = &xdg_toplevel_handle_configure,
    .close     = &xdg_toplevel_handle_close,
  };
  xdg_toplevel_add_listener (myXdgTop, &anXdgTopList, this);

  wl_surface_commit (myWlSurface);

  myWlRegion = wl_compositor_create_region (myDisplay->GetWlCompositor());

  wl_region_add (myWlRegion, myXLeft, myYTop, thePxWidth, thePxHeight);
  wl_surface_set_opaque_region (myWlSurface, myWlRegion);

  myEglWindow = wl_egl_window_create (myWlSurface, thePxWidth, thePxHeight);
  if (myEglWindow == nullptr)
  {
    throw Aspect_WindowDefinitionError ("Wayland_Window, cannot create EGL window");
  }

wl_display_dispatch_pending (myDisplay->GetWlDisplay()); /// TODO

Message::SendInfo() << "Wayland window " << thePxWidth << "x" << thePxHeight << " created"; ///
#else
  (void )theTitle;
  if (myXWindow == 0)
  {
    throw Aspect_WindowDefinitionError ("Wayland_Window, Unable to create window - not implemented");
  }
#endif
}

// =======================================================================
// function : Wayland_Window
// purpose  :
// =======================================================================
Wayland_Window::Wayland_Window (const Handle(Wayland_DisplayConnection)& theDisplay,
                                const Aspect_Drawable theWlWin)
{
  myDisplay = theDisplay;
  if (theWlWin == 0)
  {
    throw Aspect_WindowDefinitionError("Wayland_Window, given invalid window handle");
  }
  else if (theDisplay.IsNull())
  {
    throw Aspect_WindowDefinitionError("Wayland_Window, Display connection is undefined");
  }

#if defined(HAVE_WAYLAND)
  ///TODO
  DoResize();
#else
  //throw Standard_NotImplemented("Wayland_Window, not implemented");
#endif
}

// =======================================================================
// function : ~Wayland_Window
// purpose  :
// =======================================================================
Wayland_Window::~Wayland_Window()
{
  if (!myIsOwnWin)
  {
    return;
  }

#if defined(HAVE_WAYLAND)
  if (myEglWindow != nullptr)
  {
    wl_egl_window_destroy (myEglWindow);
    myEglWindow = nullptr;
  }
  if (myXdgTop != nullptr)
  {
    xdg_toplevel_destroy (myXdgTop);
    myXdgTop = nullptr;
  }
  if (myXdgSurf != nullptr)
  {
    xdg_surface_destroy (myXdgSurf);
    myXdgSurf = nullptr;
  }
  if (myWlSurface != nullptr)
  {
    wl_surface_destroy (myWlSurface);
  }
#endif
}

// =======================================================================
// function : DisplayConnection
// purpose  :
// =======================================================================
const Handle(Aspect_DisplayConnection)& Wayland_Window::DisplayConnection() const
{
  return myDisplay;
}

// =======================================================================
// function : IsMapped
// purpose  :
// =======================================================================
Standard_Boolean Wayland_Window::IsMapped() const
{
  if (myXWindow == 0)
  {
    return false;
  }
  else if (IsVirtual())
  {
    return Standard_True;
  }

/*#if defined(HAVE_WAYLAND)
  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  XGetWindowAttributes (myDisplay->GetDisplay(), (Window )myXWindow, &aWinAttr);
  return aWinAttr.map_state == IsUnviewable
      || aWinAttr.map_state == IsViewable;
#else*/
  return Standard_False;
//#endif
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void Wayland_Window::Map() const
{
  if (IsVirtual() || myXWindow == 0)
  {
    return;
  }

#if defined(HAVE_WAYLAND)
  //XMapWindow (myDisplay->GetDisplay(), (Window )myXWindow);
  //XFlush (myDisplay->GetDisplay());
#endif
}

// =======================================================================
// function : Unmap
// purpose  :
// =======================================================================
void Wayland_Window::Unmap() const
{
  if (IsVirtual() || myXWindow == 0)
  {
    return;
  }

#if defined(HAVE_WAYLAND)
  ///XIconifyWindow (myDisplay->GetDisplay(), (Window )myXWindow, DefaultScreen(myDisplay->GetDisplay()));
#endif
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize Wayland_Window::DoResize()
{
  if (IsVirtual() || myXWindow == 0)
  {
    return Aspect_TOR_UNKNOWN;
  }

/*#if defined(HAVE_WAYLAND)
  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  memset (&aWinAttr, 0, sizeof(aWinAttr));
  XGetWindowAttributes (myDisplay->GetDisplay(), (Window )myXWindow, &aWinAttr);
  if (aWinAttr.map_state == IsUnmapped)
  {
    return Aspect_TOR_UNKNOWN;
  }

  Standard_Integer aMask = 0;
  Aspect_TypeOfResize aMode = Aspect_TOR_UNKNOWN;

  if (Abs (aWinAttr.x                     - myXLeft  ) > 2) aMask |= 1;
  if (Abs ((aWinAttr.x + aWinAttr.width)  - myXRight ) > 2) aMask |= 2;
  if (Abs (aWinAttr.y                     - myYTop   ) > 2) aMask |= 4;
  if (Abs ((aWinAttr.y + aWinAttr.height) - myYBottom) > 2) aMask |= 8;
  switch (aMask)
  {
    case 0:  aMode = Aspect_TOR_NO_BORDER;               break;
    case 1:  aMode = Aspect_TOR_LEFT_BORDER;             break;
    case 2:  aMode = Aspect_TOR_RIGHT_BORDER;            break;
    case 4:  aMode = Aspect_TOR_TOP_BORDER;              break;
    case 5:  aMode = Aspect_TOR_LEFT_AND_TOP_BORDER;     break;
    case 6:  aMode = Aspect_TOR_TOP_AND_RIGHT_BORDER;    break;
    case 8:  aMode = Aspect_TOR_BOTTOM_BORDER;           break;
    case 9:  aMode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;  break;
    case 10: aMode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER; break;
    default: break;
  }

  myXLeft   = aWinAttr.x;
  myXRight  = aWinAttr.x + aWinAttr.width;
  myYTop    = aWinAttr.y;
  myYBottom = aWinAttr.y + aWinAttr.height;
  return aMode;
#else*/
  return Aspect_TOR_UNKNOWN;
//#endif
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Standard_Real Wayland_Window::Ratio() const
{
  if (IsVirtual() || myXWindow == 0)
  {
    return Standard_Real(myXRight - myXLeft) / Standard_Real(myYBottom - myYTop);
  }

/*#if defined(HAVE_WAYLAND)
  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  memset (&aWinAttr, 0, sizeof(aWinAttr));
  XGetWindowAttributes (myDisplay->GetDisplay(), (Window )myXWindow, &aWinAttr);
  return Standard_Real(aWinAttr.width) / Standard_Real(aWinAttr.height);
#else*/
  return 1.0;
//#endif
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void Wayland_Window::Position (Standard_Integer& theX1, Standard_Integer& theY1,
                               Standard_Integer& theX2, Standard_Integer& theY2) const
{
  if (IsVirtual() || myXWindow == 0)
  {
    theX1  = myXLeft;
    theX2  = myXRight;
    theY1  = myYTop;
    theY2  = myYBottom;
    return;
  }

#if defined(HAVE_WAYLAND)
  /*XFlush (myDisplay->GetDisplay());
  XWindowAttributes anAttributes;
  memset (&anAttributes, 0, sizeof(anAttributes));
  XGetWindowAttributes (myDisplay->GetDisplay(), (Window )myXWindow, &anAttributes);
  Window aChild;
  XTranslateCoordinates (myDisplay->GetDisplay(), anAttributes.root, (Window )myXWindow,
                         0, 0, &anAttributes.x, &anAttributes.y, &aChild);

  theX1 = -anAttributes.x;
  theX2 = theX1 + anAttributes.width;
  theY1 = -anAttributes.y;
  theY2 = theY1 + anAttributes.height;*/
#endif
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void Wayland_Window::Size (Standard_Integer& theWidth,
                           Standard_Integer& theHeight) const
{
  if (IsVirtual() || myXWindow == 0)
  {
    theWidth  = myXRight - myXLeft;
    theHeight = myYBottom - myYTop;
    return;
  }

#if defined(HAVE_WAYLAND)
  /*XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  memset (&aWinAttr, 0, sizeof(aWinAttr));
  XGetWindowAttributes (myDisplay->GetDisplay(), (Window )myXWindow, &aWinAttr);
  theWidth  = aWinAttr.width;
  theHeight = aWinAttr.height;*/
#endif
}

// =======================================================================
// function : SetTitle
// purpose  :
// =======================================================================
void Wayland_Window::SetTitle (const TCollection_AsciiString& theTitle)
{
  if (myXWindow != 0)
  {
  #if defined(HAVE_WAYLAND)
    //XStoreName (myDisplay->GetDisplay(), (Window )myXWindow, theTitle.ToCString());
  #else
    (void )theTitle;
  #endif
  }
}

// =======================================================================
// function : InvalidateContent
// purpose  :
// =======================================================================
void Wayland_Window::InvalidateContent (const Handle(Aspect_DisplayConnection)& theDisp)
{
  if (myXWindow == 0)
  {
    return;
  }

#if defined(HAVE_WAYLAND)
  Wayland_DisplayConnection* aDisp = !theDisp.IsNull() ? dynamic_cast<Wayland_DisplayConnection*>(theDisp.get()) : myDisplay.get();
  if (aDisp == nullptr)
  {
    throw Aspect_WindowDefinitionError("Wayland_Window::InvalidateContent(): Invalid X Display");
  }
  //
#else
  (void )theDisp;
#endif
}
