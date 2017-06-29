// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <windows.h>

#include <WNT_WClass.hxx>

#include <TCollection_ExtendedString.hxx>
#include <WNT_ClassDefinitionError.hxx>
#include <WNT_Window.hxx>

#if defined(_WIN32) && !defined(OCCT_UWP)

IMPLEMENT_STANDARD_RTTIEXT(WNT_WClass, Standard_Transient)

//=======================================================================
//function : WNT_WClass
//purpose  :
//=======================================================================
WNT_WClass::WNT_WClass (const TCollection_AsciiString& theClassName,
                        const Standard_Address theWndProc,
                        const unsigned int theStyle,
                        const Standard_Integer theClassExtra,
                        const Standard_Integer theWindowExtra,
                        const Aspect_Handle theCursor,
                        const Aspect_Handle theIcon,
                        const TCollection_AsciiString& theMenuName)
: myClassName (theClassName),
  myAppInstance (GetModuleHandleW (NULL)),
  myWndProc (NULL)
{
  const TCollection_ExtendedString aClassNameW (theClassName);
  const TCollection_ExtendedString aMenuNameW  (theMenuName);
  WNDCLASSW aWinClass;
  aWinClass.style         = (UINT)theStyle;
  aWinClass.lpfnWndProc   = theWndProc != NULL ? (WNDPROC )theWndProc : DefWindowProcW;
  aWinClass.cbClsExtra    = theClassExtra;
  aWinClass.cbWndExtra    = theWindowExtra;
  aWinClass.hInstance     = (HINSTANCE )myAppInstance;
  aWinClass.hIcon         = theIcon   != NULL ? (HICON   )theIcon   : LoadIconW   (NULL, IDI_APPLICATION);
  aWinClass.hCursor       = theCursor != NULL ? (HCURSOR )theCursor : LoadCursorW (NULL, IDC_NO);
  aWinClass.hbrBackground = 0;
  aWinClass.lpszMenuName  = !aMenuNameW.IsEmpty() ? aMenuNameW.ToWideString() : NULL;
  aWinClass.lpszClassName = aClassNameW.ToWideString();
  if (!RegisterClassW (&aWinClass))
  {
    myClassName.Clear();
    throw WNT_ClassDefinitionError("Unable to register window class");
  }
  myWndProc = (Standard_Address )aWinClass.lpfnWndProc;
}

//=======================================================================
//function : ~WNT_WClass
//purpose  :
//=======================================================================
WNT_WClass::~WNT_WClass()
{
  if (!myClassName.IsEmpty())
  {
    const TCollection_ExtendedString aClassNameW (myClassName);
    UnregisterClassW (aClassNameW.ToWideString(), (HINSTANCE )myAppInstance);
  }
}

#endif // _WIN32
