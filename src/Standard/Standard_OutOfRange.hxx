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

#ifndef _Standard_OutOfRange_HeaderFile
#define _Standard_OutOfRange_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_RangeError.hxx>

class Standard_OutOfRange;
DEFINE_STANDARD_HANDLE(Standard_OutOfRange, Standard_RangeError)

#if (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
  // suppress false-positive warnings produced by GCC optimizer
  #define Standard_OutOfRange_Always_Raise_if(CONDITION, MESSAGE) \
  _Pragma("GCC diagnostic push") \
  _Pragma("GCC diagnostic ignored \"-Wstrict-overflow\"") \
  if (CONDITION) throw Standard_OutOfRange(MESSAGE); \
  _Pragma("GCC diagnostic pop")
#else
  #define Standard_OutOfRange_Always_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw Standard_OutOfRange(MESSAGE);
#endif

#if !defined No_Exception && !defined No_Standard_OutOfRange
  #define Standard_OutOfRange_Raise_if(CONDITION, MESSAGE) Standard_OutOfRange_Always_Raise_if(CONDITION, MESSAGE)
#else
  #define Standard_OutOfRange_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Standard_OutOfRange, Standard_RangeError)

#endif // _Standard_OutOfRange_HeaderFile
