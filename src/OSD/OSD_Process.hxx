// Created on: 2018-03-15
// Created by: Stephan GARNAUD (ARM)
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _OSD_Process_HeaderFile
#define _OSD_Process_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <OSD_Error.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class OSD_OSDError;
class TCollection_AsciiString;
class Quantity_Date;
class OSD_Path;

// undefine SetCurrentDirectory that can be #defined by previous inclusion of windows.h
#ifdef SetCurrentDirectory
# undef SetCurrentDirectory
#endif

//! A set of system process tools
class OSD_Process 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes the object and prepare for a possible dump
  Standard_EXPORT OSD_Process();
  
  //! Returns the terminal used (vt100, vt200 ,sun-cmd ...)
  Standard_EXPORT void TerminalType (TCollection_AsciiString& Name);
  
  //! Gets system date.
  Standard_EXPORT Quantity_Date SystemDate();
  
  //! Returns the user name.
  Standard_EXPORT TCollection_AsciiString UserName();
  
  //! Returns True if the process user is the super-user.
  Standard_EXPORT Standard_Boolean IsSuperUser();
  
  //! Returns the 'Process Id'
  Standard_EXPORT Standard_Integer ProcessId();
  
  //! Returns the current path where the process is.
  Standard_EXPORT OSD_Path CurrentDirectory();
  
  //! Changes the current process directory.
  Standard_EXPORT void SetCurrentDirectory (const OSD_Path& where);
  
  //! Returns TRUE if an error occurs
  Standard_EXPORT Standard_Boolean Failed() const;
  
  //! Resets error counter to zero
  Standard_EXPORT void Reset();
  
  //! Raises OSD_Error
  Standard_EXPORT void Perror();
  
  //! Returns error number if 'Failed' is TRUE.
  Standard_EXPORT Standard_Integer Error() const;

private:
  OSD_Error myError;
};

#endif // _OSD_Process_HeaderFile
