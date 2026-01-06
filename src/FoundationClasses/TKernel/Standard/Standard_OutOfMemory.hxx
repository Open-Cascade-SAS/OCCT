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

#include <Standard_ProgramError.hxx>

#include <memory>

#if !defined No_Exception && !defined No_Standard_OutOfMemory
  #define Standard_OutOfMemory_Raise_if(CONDITION, MESSAGE)                                        \
    if (CONDITION)                                                                                 \
      throw Standard_OutOfMemory(MESSAGE);
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
  Standard_EXPORT void Throw() const override;

public:
  //! Constructor is kept public for backward compatibility
  Standard_EXPORT Standard_OutOfMemory(const char* theMessage = nullptr);

  //! Returns error message
  Standard_EXPORT const char* GetMessageString() const override;

  //! Sets error message
  Standard_EXPORT void SetMessageString(const char* aMessage) override;

  //! Raises exception with specified message string
  Standard_EXPORT static void Raise(const char* theMessage = "");

  //! Raises exception with specified message string
  Standard_EXPORT static void Raise(Standard_SStream& theMessage);

  //! Returns global instance of exception
  Standard_EXPORT static std::shared_ptr<Standard_OutOfMemory> NewInstance(
    const char* theMessage = "");

  //! Returns global instance of exception
  Standard_EXPORT static std::shared_ptr<Standard_OutOfMemory> NewInstance(
    const char* theMessage,
    const char* theStackTrace);

  //! Returns the exception type name.
  const char* ExceptionType() const override { return "Standard_OutOfMemory"; }

protected:
  char myBuffer[1024];
};

#endif // _Standard_OutOfMemory_HeaderFile
