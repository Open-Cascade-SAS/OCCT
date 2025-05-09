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

#ifndef Wayland_DisplayConnection_HeaderFile
#define Wayland_DisplayConnection_HeaderFile

#include <Aspect_DisplayConnection.hxx>

struct wl_compositor;
struct wl_display;
struct wl_registry;
struct xdg_wm_base;

//! This class creates and provides connection with Wayland server.
//! WARRNING: Do not close display connection manually!
class Wayland_DisplayConnection : public Aspect_DisplayConnection
{
  DEFINE_STANDARD_RTTIEXT(Wayland_DisplayConnection, Aspect_DisplayConnection)
public:

  //! Default constructor. Creates connection with default server.
  Standard_EXPORT Wayland_DisplayConnection();

  //! Constructor. Creates connection with specified display.
  Standard_EXPORT Wayland_DisplayConnection (const TCollection_AsciiString& theDisplayName);

  //! Constructor wrapping existing wl_display instance.
  //! WARNING! it is a responsibility of application to keep this pointer
  //! valid while Aspect_DisplayConnection is alive and to close wl_display when it is no more needed.
  Standard_EXPORT Wayland_DisplayConnection (Aspect_XDisplay* theDisplay);

  //! Destructor. Close opened connection.
  Standard_EXPORT virtual ~Wayland_DisplayConnection();

  //! @return pointer to Display structure that serves as the connection to the X server.
  virtual Aspect_XDisplay* GetDisplayAspect() Standard_OVERRIDE { return (Aspect_XDisplay* )myDisplay; }

  //! @return TRUE if X Display has been allocated by this class
  Standard_Boolean IsOwnDisplay() const { return myIsOwnDisplay; }

  //! Open connection with display or takes theDisplay parameter when it is not NULL.
  //! WARNING! When external Display is specified, it is a responsibility of application
  //! to keep this pointer valid while Aspect_DisplayConnection is alive
  //! and to close Display when it is no more needed.
  //! @param theDisplay external pointer to allocated wl_display, or NULL if new connection should be created
  Standard_EXPORT void Init (Aspect_XDisplay* theDisplay,
                             const TCollection_AsciiString& theDisplayName = TCollection_AsciiString());

  //! Close connection.
  Standard_EXPORT void Close();

  //! Return file description for this connection.
  Standard_EXPORT int FileDescriptor() const;

  //! wl_display_dispatch() wrapper.
  Standard_EXPORT void Dispatch();

  //! wl_display_dispatch_pending() wrapper.
  Standard_EXPORT void DispatchPending();

public:

  //! Return native display.
  struct wl_display* GetWlDisplay() { return myDisplay; }

  //! Return native compositor.
  struct wl_compositor* GetWlCompositor() { return myCompositor; }

  //! Return native XDG WM base.
  struct xdg_wm_base* GetXDGWMBase() { return myXDGWMBase; }

private:

  //! Wayland registry listener.
  void registryHandlerGlobal (struct wl_registry* theRegistry,
                              uint32_t theId,
                              const char* theInterface,
                              uint32_t theVersion);

  //! Wayland registry remove listener.
  void registryHandlerGlobalRemove (struct wl_registry* theRegistry,
                                    uint32_t theId);

  //! Wayland registry listener.
  static void registryHandlerGlobal (void* theData,
                                     struct wl_registry* theRegistry,
                                     uint32_t theId,
                                     const char* theInterface,
                                     uint32_t theVersion)
  {
    ((Wayland_DisplayConnection* )theData)->registryHandlerGlobal (theRegistry, theId, theInterface, theVersion);
  }

  //! Wayland registry remove listener.
  static void registryHandlerGlobalRemove (void* theData,
                                           struct wl_registry* theRegistry,
                                           uint32_t theId)
  {
    ((Wayland_DisplayConnection* )theData)->registryHandlerGlobalRemove (theRegistry, theId);
  }

private:

  struct wl_display*    myDisplay      = nullptr;
  struct wl_compositor* myCompositor   = nullptr;
  struct xdg_wm_base*   myXDGWMBase    = nullptr;
  Standard_Boolean      myIsOwnDisplay = false;

private:

  //! To protect the connection from closing copying allowed only through the handles.
  Wayland_DisplayConnection            (const Wayland_DisplayConnection& ) Standard_DELETE;
  Wayland_DisplayConnection& operator= (const Wayland_DisplayConnection& ) Standard_DELETE;

};

#endif // Wayland_DisplayConnection_HeaderFile
