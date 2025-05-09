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

#ifndef Xw_DisplayConnection_HeaderFile
#define Xw_DisplayConnection_HeaderFile

#include <Aspect_DisplayConnection.hxx>

//! This class creates and provides connection with X server.
//! Raises exception if can not connect to X server.
//! On Windows and Mac OS X (in case when Cocoa used) platforms this class does nothing.
//! WARRNING: Do not close display connection manually!
class Xw_DisplayConnection : public Aspect_DisplayConnection
{
  DEFINE_STANDARD_RTTIEXT(Xw_DisplayConnection, Aspect_DisplayConnection)
public:

  //! Default constructor. Creates connection with display name taken from "DISPLAY" environment variable
  Standard_EXPORT Xw_DisplayConnection();

  //! Destructor. Close opened connection.
  Standard_EXPORT virtual ~Xw_DisplayConnection();

  //! Constructor. Creates connection with display specified in theDisplayName.
  //! Display name should be in format "hostname:number" or "hostname:number.screen_number", where:
  //! hostname      - Specifies the name of the host machine on which the display is physically attached.
  //! number        - Specifies the number of the display server on that host machine.
  //! screen_number - Specifies the screen to be used on that server. Optional variable.
  Standard_EXPORT Xw_DisplayConnection (const TCollection_AsciiString& theDisplayName);

  //! Constructor wrapping existing Display instance.
  //! WARNING! it is a responsibility of application to keep this pointer
  //! valid while Aspect_DisplayConnection is alive and to close Display when it is no more needed.
  Standard_EXPORT Xw_DisplayConnection (Aspect_XDisplay* theDisplay);

  //! @return pointer to Display structure that serves as the connection to the X server.
  virtual Aspect_XDisplay* GetDisplayAspect() Standard_OVERRIDE { return myDisplay; }

  //! @return TRUE if X Display has been allocated by this class
  Standard_Boolean IsOwnDisplay() const { return myIsOwnDisplay; }

  //! @return identifier(atom) for custom named property associated with windows that use current connection to X server.
  virtual uint64_t GetAtom (const Aspect_XAtom theAtom) const Standard_OVERRIDE
  {
    return myAtoms.Find (theAtom);
  }

  //! @return display name for this connection.
  const TCollection_AsciiString& GetDisplayName() const { return myDisplayName; }

  //! Open connection with display specified in myDisplayName class field
  //! or takes theDisplay parameter when it is not NULL.
  //! WARNING! When external Display is specified, it is a responsibility of application
  //! to keep this pointer valid while Aspect_DisplayConnection is alive
  //! and to close Display when it is no more needed.
  //! @param theDisplay external pointer to allocated Display, or NULL if new connection should be created
  Standard_EXPORT void Init (Aspect_XDisplay* theDisplay);

  //! Return default window visual or NULL when undefined.
  Aspect_XVisualInfo* GetDefaultVisualInfo() const { return myDefVisualInfo; }

  //! @return native Window FB config (GLXFBConfig on Xlib)
  Aspect_FBConfig GetDefaultFBConfig() const { return myDefFBConfig; }

  //! Set default window visual; the visual will be deallocated using XFree().
  Standard_EXPORT virtual void SetDefaultVisualInfo (Aspect_XVisualInfo* theVisual,
                                                     Aspect_FBConfig theFBConfig) Standard_OVERRIDE;

  //! Return file description for this connection.
  Standard_EXPORT int FileDescriptor() const;

#ifdef X_PROTOCOL
  //! Constructor wrapping existing Display instance.
  //! WARNING! it is a responsibility of application to keep this pointer
  //! valid while Aspect_DisplayConnection is alive and to close Display when it is no more needed.
  Xw_DisplayConnection (Display* theDisplay)
  : Xw_DisplayConnection ((Aspect_XDisplay* )theDisplay) {}

  //! @return pointer to Display structure that serves as the connection to the X server.
  Display* GetDisplay() { return (Display* )myDisplay; }

  //! Return default window visual or NULL when undefined.
  XVisualInfo* GetDefaultVisualInfoX() const { return (XVisualInfo* )myDefVisualInfo; }

  //! Set default window visual; the visual will be deallocated using XFree().
  void SetDefaultVisualInfo (XVisualInfo* theVisual,
                             Aspect_FBConfig theFBConfig)
  {
    SetDefaultVisualInfo ((Aspect_XVisualInfo* )theVisual, theFBConfig);
  }

  //! @return identifier(atom) for custom named property associated with windows that use current connection to X server.
  Atom GetAtomX (const Aspect_XAtom theAtom) const
  {
    return (Atom )GetAtom (theAtom);
  }

  //! Open connection with display specified in myDisplayName class field
  //! or takes theDisplay parameter when it is not NULL.
  void Init (Display* theDisplay)
  {
    Init ((Aspect_XDisplay* )theDisplay);
  }
#endif

private:

  Aspect_XDisplay*         myDisplay;
  Aspect_XVisualInfo*      myDefVisualInfo;
  Aspect_FBConfig          myDefFBConfig;
  NCollection_DataMap<Aspect_XAtom, uint64_t> myAtoms;
  TCollection_AsciiString  myDisplayName;
  Standard_Boolean         myIsOwnDisplay;

private:

  //! To protect the connection from closing copying allowed only through the handles.
  Xw_DisplayConnection            (const Xw_DisplayConnection& ) Standard_DELETE;
  Xw_DisplayConnection& operator= (const Xw_DisplayConnection& ) Standard_DELETE;

};

DEFINE_STANDARD_HANDLE (Xw_DisplayConnection, Aspect_DisplayConnection)

#endif // _Xw_DisplayConnection_H__
