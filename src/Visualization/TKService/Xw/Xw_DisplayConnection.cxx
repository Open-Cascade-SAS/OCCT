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

#include <Xw_DisplayConnection.hxx>

#include <Aspect_DisplayConnectionDefinitionError.hxx>
#include <OSD_Environment.hxx>

#if defined(HAVE_XLIB)
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Xw_DisplayConnection,Aspect_DisplayConnection)

// =======================================================================
// function : Xw_DisplayConnection
// purpose  :
// =======================================================================
Xw_DisplayConnection::Xw_DisplayConnection()
{
#if defined(HAVE_XLIB)
  myDisplay = NULL;
  myDefVisualInfo = NULL;
  myDefFBConfig = NULL;
  myIsOwnDisplay = false;
  OSD_Environment anEnv ("DISPLAY");
  myDisplayName = anEnv.Value();
  Init (NULL);
#endif
}

// =======================================================================
// function : ~Xw_DisplayConnection
// purpose  :
// =======================================================================
Xw_DisplayConnection::~Xw_DisplayConnection()
{
#if defined(HAVE_XLIB)
  if (myDefVisualInfo != NULL)
  {
    XFree (myDefVisualInfo);
  }
  if (myDisplay != NULL
   && myIsOwnDisplay)
  {
    XCloseDisplay ((Display* )myDisplay);
  }
#endif
}

// =======================================================================
// function : Xw_DisplayConnection
// purpose  :
// =======================================================================
Xw_DisplayConnection::Xw_DisplayConnection (const TCollection_AsciiString& theDisplayName)
: myDisplay (NULL),
  myDefVisualInfo (NULL),
  myDefFBConfig (NULL),
  myIsOwnDisplay (false)
{
  myDisplayName = theDisplayName;
  Init (NULL);
}

// =======================================================================
// function : Xw_DisplayConnection
// purpose  :
// =======================================================================
Xw_DisplayConnection::Xw_DisplayConnection (Aspect_XDisplay* theDisplay)
: myDisplay (NULL),
  myDefVisualInfo (NULL),
  myDefFBConfig (NULL),
  myIsOwnDisplay (false)
{
  Init (theDisplay);
}

// =======================================================================
// function : FileDescriptor
// purpose  :
// =======================================================================
int Xw_DisplayConnection::FileDescriptor() const
{
#if defined(HAVE_XLIB)
  return myDisplay != nullptr ? XConnectionNumber((Display* )myDisplay) : 0;
#else
  return 0;
#endif
}

// =======================================================================
// function : SetDefaultVisualInfo
// purpose  :
// =======================================================================
void Xw_DisplayConnection::SetDefaultVisualInfo (Aspect_XVisualInfo* theVisual,
                                                 Aspect_FBConfig theFBConfig)
{
  if (myDefVisualInfo != NULL)
  {
  #if defined(HAVE_XLIB)
    XFree (myDefVisualInfo);
  #endif
  }
  myDefVisualInfo = theVisual;
  myDefFBConfig = theFBConfig;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void Xw_DisplayConnection::Init (Aspect_XDisplay* theDisplay)
{
#if defined(HAVE_XLIB)
  if (myDisplay != NULL
   && myIsOwnDisplay)
  {
    XCloseDisplay ((Display* )myDisplay);
  }
  myIsOwnDisplay = false;
  myAtoms.Clear();

  myDisplay = theDisplay != NULL ? theDisplay : (Aspect_XDisplay* )XOpenDisplay (myDisplayName.ToCString());
  if (myDisplay == NULL)
  {
    TCollection_AsciiString aMessage;
    aMessage += "Can not connect to the X11 server \"";
    aMessage += myDisplayName + "\"";
    throw Aspect_DisplayConnectionDefinitionError(aMessage.ToCString());
  }
  else
  {
    myIsOwnDisplay = theDisplay == NULL;
    myAtoms.Bind (Aspect_XA_DELETE_WINDOW, (uint64_t )XInternAtom((Display* )myDisplay, "WM_DELETE_WINDOW", False));
  }
#else
  myDisplay = theDisplay;
  myIsOwnDisplay = theDisplay == NULL;
#endif
}
