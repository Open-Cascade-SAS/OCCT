// Created on: 1991-09-05
// Created by: J.P. TIRAUlt
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Standard_OutOfMemory_HeaderFile
#define _Standard_OutOfMemory_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_ProgramError.hxx>

class Standard_OutOfMemory;
DEFINE_STANDARD_HANDLE(Standard_OutOfMemory, Standard_ProgramError)

#if !defined No_Exception && !defined No_Standard_OutOfMemory
  #define Standard_OutOfMemory_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw Standard_OutOfMemory(MESSAGE);
#else
  #define Standard_OutOfMemory_Raise_if(CONDITION, MESSAGE)
#endif

//! Standard_OutOfMemory exception is defined explicitly and not by
//! macro DEFINE_STANDARD_EXCEPTION, to avoid necessity of dynamic
//! memory allocations during throwing and stack unwinding:
//! 
//! - method NewInstance() returns static instance (singleton)
//! - method Raise() raises copy of that singleton, resetting 
//!   its message string
//! - message string is stored as field, not allocated dynamically
//!   (storable message length is limited by buffer size)
//!
//! The reason is that in out-of-memory condition any memory allocation can 
//! fail, thus use of operator new for allocation of new exception instance 
//! is dangerous (can cause recursion until stack overflow, see #24836).

class Standard_OutOfMemory : public Standard_ProgramError
{
  Standard_EXPORT void Throw () const Standard_OVERRIDE;

public:

  //! Constructor is kept public for backward compatibility
  Standard_EXPORT Standard_OutOfMemory(const Standard_CString theMessage = 0);

  //! Returns error message
  Standard_EXPORT Standard_CString GetMessageString() const Standard_OVERRIDE;
  
  //! Sets error message
  Standard_EXPORT void SetMessageString (const Standard_CString aMessage) Standard_OVERRIDE;

  //! Raises exception with specified message string
  Standard_EXPORT static void Raise(const Standard_CString theMessage = "");

  //! Raises exception with specified message string
  Standard_EXPORT static void Raise(Standard_SStream& theMessage);

  //! Returns global instance of exception
  Standard_EXPORT static Handle(Standard_OutOfMemory) NewInstance(const Standard_CString theMessage = "");

  DEFINE_STANDARD_RTTIEXT(Standard_OutOfMemory,Standard_ProgramError)

protected:
  char myBuffer[1024];
};

#endif // _Standard_OutOfMemory_HeaderFile
