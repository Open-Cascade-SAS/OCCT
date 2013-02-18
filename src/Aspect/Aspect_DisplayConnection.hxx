// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _Aspect_DisplayConnection_H__
#define _Aspect_DisplayConnection_H__

#include <Standard_Transient.hxx>
#include <Aspect_DisplayConnection_Handle.hxx>

#include <TCollection_AsciiString.hxx>

#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  #include <X11/Xlib.h>
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

#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  //! Constructor. Creates connection with display specified in theDisplayName.
  //! Display name should be in format "hostname:number" or "hostname:number.screen_number", where:
  //! hostname      - Specifies the name of the host machine on which the display is physically attached.
  //! number        - Specifies the number of the display server on that host machine.
  //! screen_number - Specifies the screen to be used on that server. Optional variable.
  Aspect_DisplayConnection (const TCollection_AsciiString& theDisplayName);

  //! @return pointer to Display structure that serves as the connection to the X server.
  Display* GetDisplay();

  //! @return display name for this connection.
  TCollection_AsciiString GetDisplayName();

private:

  //! Open connection with display specified in myDisplayName class field.
  void Init();

private:

  Display*                myDisplay;
  TCollection_AsciiString myDisplayName;
#endif

private:

  //! To protect the connection from closing copying allowed only through the handles.
  Aspect_DisplayConnection            (const Aspect_DisplayConnection& );
  Aspect_DisplayConnection& operator= (const Aspect_DisplayConnection& );

public:

  DEFINE_STANDARD_RTTI(Aspect_DisplayConnection) // Type definition

};

#endif // _Aspect_DisplayConnection_H__
