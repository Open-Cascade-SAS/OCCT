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

#include <Wayland_DisplayConnection.hxx>

#include <Aspect_DisplayConnectionDefinitionError.hxx>
#include <Message.hxx>

#if defined(HAVE_WAYLAND)
  #include <wayland-client-core.h>
  #include <wayland-client.h>
  #include <wayland-server.h>
  #include <wayland-client-protocol.h>
  #include <wayland-egl.h> // should be included before EGL headers

  // generated from xdg-shell.xml
  // > wayland-scanner client-header stable/xdg-shell/xdg-shell.xml xdg-shell-client-protocol.pxx
  // > wayland-scanner private-code  stable/xdg-shell/xdg-shell.xml xdg-shell-protocol.c
  #include "xdg-shell-client-protocol.pxx"
#endif

IMPLEMENT_STANDARD_RTTIEXT(Wayland_DisplayConnection,Aspect_DisplayConnection)

// =======================================================================
// function : Wayland_DisplayConnection
// purpose  :
// =======================================================================
Wayland_DisplayConnection::Wayland_DisplayConnection()
{
  Init (nullptr);
}

// =======================================================================
// function : ~Wayland_DisplayConnection
// purpose  :
// =======================================================================
Wayland_DisplayConnection::~Wayland_DisplayConnection()
{
  Close();
}

// =======================================================================
// function : Close
// purpose  :
// =======================================================================
void Wayland_DisplayConnection::Close()
{
#if defined(HAVE_WAYLAND)
  if (myDisplay != nullptr
   && myIsOwnDisplay)
  {
    wl_display_disconnect (myDisplay);
  }
  myDisplay = nullptr;
  myCompositor = nullptr;
  myXDGWMBase = nullptr;
  myIsOwnDisplay = false;
#endif
}

// =======================================================================
// function : Wayland_DisplayConnection
// purpose  :
// =======================================================================
Wayland_DisplayConnection::Wayland_DisplayConnection (const TCollection_AsciiString& theDisplayName)
{
  Init (nullptr, theDisplayName);
}

// =======================================================================
// function : Wayland_DisplayConnection
// purpose  :
// =======================================================================
Wayland_DisplayConnection::Wayland_DisplayConnection (Aspect_XDisplay* theDisplay)
{
  Init (theDisplay);
}

// =======================================================================
// function : FileDescriptor
// purpose  :
// =======================================================================
int Wayland_DisplayConnection::FileDescriptor() const
{
#if defined(HAVE_WAYLAND)
  return myDisplay != nullptr ? wl_display_get_fd (myDisplay) : 0;
#else
  return 0;
#endif
}

#if defined(HAVE_WAYLAND)
//! XDG ping callback.
static void xdg_wm_base_ping (void* theData,
                              struct xdg_wm_base* theXdgWmBase,
                              uint32_t theSerial)
{
  (void )theData;
  xdg_wm_base_pong (theXdgWmBase, theSerial);
}

// =======================================================================
// function : registryHandlerGlobal
// purpose  :
// =======================================================================
void Wayland_DisplayConnection::registryHandlerGlobal (struct wl_registry* theRegistry,
                                                       uint32_t theId,
                                                       const char* theInterface,
                                                       uint32_t theVersion)
{
  (void )theVersion;
  if (::strcmp (theInterface, "wl_compositor") == 0)
  {
    myCompositor = (struct wl_compositor* )wl_registry_bind (theRegistry, theId, &wl_compositor_interface, 1);
  }
  else if (::strcmp (theInterface, xdg_wm_base_interface.name) == 0)
  {
    static const struct xdg_wm_base_listener anXdgBaseList = { .ping = &xdg_wm_base_ping, };
    myXDGWMBase = (struct xdg_wm_base* )wl_registry_bind (theRegistry, theId, &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener (myXDGWMBase, &anXdgBaseList, this);
  }
}

// =======================================================================
// function : registryHandlerGlobalRemove
// purpose  :
// =======================================================================
void Wayland_DisplayConnection::registryHandlerGlobalRemove (struct wl_registry* ,
                                                             uint32_t )
{
  //
}
#endif

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void Wayland_DisplayConnection::Init (Aspect_XDisplay* theDisplay,
                                      const TCollection_AsciiString& theDisplayName)
{
  Close();
#if defined(HAVE_WAYLAND)
  myDisplay = theDisplay != nullptr
            ? (struct wl_display* )theDisplay
            : wl_display_connect (!theDisplayName.IsEmpty() ? theDisplayName.ToCString() : nullptr);
  if (myDisplay == nullptr)
  {
    TCollection_AsciiString aMessage;
    aMessage += "Cannot connect to the Wayland server \"";
    aMessage += theDisplayName + "\"";
    throw Aspect_DisplayConnectionDefinitionError (aMessage.ToCString());
  }
  myIsOwnDisplay = (theDisplay == nullptr);

  static const struct wl_registry_listener aListener =
  {
    .global        = &Wayland_DisplayConnection::registryHandlerGlobal,
    .global_remove = &Wayland_DisplayConnection::registryHandlerGlobalRemove
  };

  struct wl_registry* wl_registry = wl_display_get_registry (myDisplay);
  wl_registry_add_listener (wl_registry, &aListener, this);
  // call attached listener
  wl_display_dispatch (myDisplay);
  wl_display_roundtrip (myDisplay);
  if (myCompositor == nullptr)
  {
    throw Aspect_DisplayConnectionDefinitionError ("Wayland server has no compositor");
  }
  if (myXDGWMBase == nullptr)
  {
    throw Aspect_DisplayConnectionDefinitionError ("Wayland server has no XDG shell");
  }
#else
  (void)theDisplayName;
  myDisplay = (struct wl_display* )theDisplay;
  myIsOwnDisplay = (theDisplay == nullptr);
#endif
}

// =======================================================================
// function : Dispatch
// purpose  :
// =======================================================================
void Wayland_DisplayConnection::Dispatch()
{
  if (myDisplay == nullptr)
  {
    return;
  }
#if defined(HAVE_WAYLAND)
  wl_display_dispatch (myDisplay);
#endif
}

// =======================================================================
// function : DispatchPending
// purpose  :
// =======================================================================
void Wayland_DisplayConnection::DispatchPending()
{
  if (myDisplay == nullptr)
  {
    return;
  }
#if defined(HAVE_WAYLAND)
  wl_display_dispatch_pending (myDisplay);
#endif
}
