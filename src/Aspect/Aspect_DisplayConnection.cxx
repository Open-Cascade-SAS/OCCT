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

#include <Aspect_DisplayConnection.hxx>

#include <Aspect_DisplayConnectionDefinitionError.hxx>
#include <OSD_Environment.hxx>

IMPLEMENT_STANDARD_HANDLE (Aspect_DisplayConnection, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Aspect_DisplayConnection, Standard_Transient)

// =======================================================================
// function : Aspect_DisplayConnection
// purpose  :
// =======================================================================
Aspect_DisplayConnection::Aspect_DisplayConnection()
{
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  OSD_Environment anEnv ("DISPLAY");
  myDisplayName = anEnv.Value();
  Init();
#endif
}

// =======================================================================
// function : ~Aspect_DisplayConnection
// purpose  :
// =======================================================================
Aspect_DisplayConnection::~Aspect_DisplayConnection()
{
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  if (myDisplay != NULL)
  {
    XCloseDisplay (myDisplay);
  }
#endif
}

#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
// =======================================================================
// function : Aspect_DisplayConnection
// purpose  :
// =======================================================================
Aspect_DisplayConnection::Aspect_DisplayConnection (const TCollection_AsciiString& theDisplayName)
{
  myDisplayName = theDisplayName;
  Init();
}

// =======================================================================
// function : GetDisplay
// purpose  :
// =======================================================================
Display* Aspect_DisplayConnection::GetDisplay()
{
  return myDisplay;
}

// =======================================================================
// function : GetDisplayName
// purpose  :
// =======================================================================
TCollection_AsciiString Aspect_DisplayConnection::GetDisplayName()
{
  return myDisplayName;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void Aspect_DisplayConnection::Init()
{
  myDisplay = XOpenDisplay (myDisplayName.ToCString());
  
  if (myDisplay == NULL)
  {
    TCollection_AsciiString aMessage;
    aMessage += "Can not connect to the server \"";
    aMessage += myDisplayName + "\"";
    Aspect_DisplayConnectionDefinitionError::Raise (aMessage.ToCString());
  }
}
#endif
