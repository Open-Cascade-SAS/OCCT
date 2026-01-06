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

#ifndef _Standard_DefineException_HeaderFile
#define _Standard_DefineException_HeaderFile

#include <Standard_SStream.hxx>

#include <memory>

//! @brief Defines an exception class inheriting from Standard_Failure.
//!
//! This macro creates a complete exception class with:
//! - Constructors that forward to base class
//! - ExceptionType() override returning the class name
//!
//! Usage:
//! @code
//! DEFINE_STANDARD_EXCEPTION(Standard_OutOfRange, Standard_RangeError)
//! @endcode
//!
//! @param C1 Name of the exception class to define
//! @param C2 Name of the parent exception class (must be Standard_Failure or derived)
#define DEFINE_STANDARD_EXCEPTION(C1, C2)                                                          \
  class C1 : public C2                                                                             \
  {                                                                                                \
  public:                                                                                          \
    C1(const char* theMessage = "")                                                                \
        : C2(theMessage)                                                                           \
    {                                                                                              \
    }                                                                                              \
                                                                                                   \
    C1(const char* theMessage, const char* theStackTrace)                                          \
        : C2(theMessage, theStackTrace)                                                            \
    {                                                                                              \
    }                                                                                              \
                                                                                                   \
    const char* ExceptionType() const noexcept override { return #C1; }                            \
  };

#endif
