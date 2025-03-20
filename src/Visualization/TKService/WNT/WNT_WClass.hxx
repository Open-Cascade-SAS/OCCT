// Created on: 1996-01-26
// Created by: PLOTNIKOV Eugeny
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

#ifndef _WNT_WClass_HeaderFile
#define _WNT_WClass_HeaderFile

#include <Standard.hxx>

#if defined(_WIN32) && !defined(OCCT_UWP)

  #include <Aspect_Handle.hxx>
  #include <Standard_Integer.hxx>
  #include <Standard_Transient.hxx>
  #include <NCollection_DefineAlloc.hxx>
  #include <TCollection_AsciiString.hxx>

//! This class defines a Windows NT window class.
//! A window in Windows NT is always created based on a
//! window class. The window class identifies the window
//! procedure that processes messages to the window. Each
//! window class has unique name ( character string ). More
//! than one window can be created based on a single window
//! class. For example, all button windows in Windows NT
//! are created based on the same window class. The window
//! class defines the window procedure and some other
//! characteristics ( background, mouse cursor shape etc. )
//! of the windows that are created based on that class.
//! When we create a window, we define additional
//! characteristics of the window that are unique to that
//! window. So, we have to create and register window
//! class before creation of any window. Of course, it's possible
//! to create a new window class for each window inside
//! the Window class and do not use the WClass at all.
//! We implemented this class for sake of flexibility of
//! event processing.
class WNT_WClass : public Standard_Transient
{
  friend class WNT_Window;
  DEFINE_STANDARD_RTTIEXT(WNT_WClass, Standard_Transient)
public:
  //! Creates a Windows NT window class and registers it.
  Standard_EXPORT WNT_WClass(
    const TCollection_AsciiString& theClassName,
    const Standard_Address         theWndProc,
    const unsigned int             theStyle,
    const Standard_Integer         theClassExtra  = 0,
    const Standard_Integer         theWindowExtra = 0,
    const Aspect_Handle            theCursor      = NULL,
    const Aspect_Handle            theIcon        = NULL,
    const TCollection_AsciiString& theMenuName    = TCollection_AsciiString());

  //! Destroys all resources attached to the class
  Standard_EXPORT ~WNT_WClass();

  //! Returns address of window procedure.
  Standard_Address WndProc() const { return myWndProc; }

  //! Returns a class name.
  const TCollection_AsciiString& Name() const { return myClassName; }

  //! Returns a program instance handle.
  Aspect_Handle Instance() const { return myAppInstance; }

protected:
  TCollection_AsciiString myClassName;
  Aspect_Handle           myAppInstance;
  Standard_Address        myWndProc;
};

DEFINE_STANDARD_HANDLE(WNT_WClass, Standard_Transient)

#endif // _WIN32
#endif // _WNT_WClass_HeaderFile
