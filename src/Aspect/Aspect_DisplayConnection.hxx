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

#ifndef _Aspect_DisplayConnection_H__
#define _Aspect_DisplayConnection_H__

#include <Standard_Transient.hxx>
#include <Aspect_XAtom.hxx>

#include <TCollection_AsciiString.hxx>
#include <NCollection_DataMap.hxx>

#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && !defined(__ANDROID__) && !defined(__QNX__)
  #include <InterfaceGraphic.hxx>
#endif

//! This class creates and provides connection with X server.
//! Raises exception if can not connect to X server.
//! On Windows and Mac OS X (in case when Cocoa used) platforms this class do nothing.
//! WARRNING: Do not close display connection manualy!

class Aspect_DisplayConnection : public Standard_Transient
{
public:

  //! Default constructor. Creates connection with display name taken from "DISPLAY" environment variable
  Standard_EXPORT Aspect_DisplayConnection();

  //! Destructor. Close opened connection.
  Standard_EXPORT ~Aspect_DisplayConnection();

#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && !defined(__ANDROID__) && !defined(__QNX__)
  //! Constructor. Creates connection with display specified in theDisplayName.
  //! Display name should be in format "hostname:number" or "hostname:number.screen_number", where:
  //! hostname      - Specifies the name of the host machine on which the display is physically attached.
  //! number        - Specifies the number of the display server on that host machine.
  //! screen_number - Specifies the screen to be used on that server. Optional variable.
  Aspect_DisplayConnection (const TCollection_AsciiString& theDisplayName);

  //! @return pointer to Display structure that serves as the connection to the X server.
  Display* GetDisplay();
  
  //! @return identifier(atom) for custom named property associated with windows that use current connection to X server.
  Atom GetAtom (const Aspect_XAtom theAtom) const;

  //! @return display name for this connection.
  TCollection_AsciiString GetDisplayName();

private:

  //! Open connection with display specified in myDisplayName class field.
  void Init();

private:

  Display*                 myDisplay;
  NCollection_DataMap<Aspect_XAtom, Atom> myAtoms;
  TCollection_AsciiString  myDisplayName;
#endif

private:

  //! To protect the connection from closing copying allowed only through the handles.
  Aspect_DisplayConnection            (const Aspect_DisplayConnection& );
  Aspect_DisplayConnection& operator= (const Aspect_DisplayConnection& );

public:

  DEFINE_STANDARD_RTTIEXT(Aspect_DisplayConnection,Standard_Transient) // Type definition

};

DEFINE_STANDARD_HANDLE (Aspect_DisplayConnection, Standard_Transient)

#endif // _Aspect_DisplayConnection_H__
