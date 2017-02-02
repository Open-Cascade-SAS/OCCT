// Created on: 1992-05-07
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _StdFail_Undefined_HeaderFile
#define _StdFail_Undefined_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_Failure.hxx>

class StdFail_Undefined;
DEFINE_STANDARD_HANDLE(StdFail_Undefined, Standard_Failure)

#if !defined No_Exception && !defined No_StdFail_Undefined
  #define StdFail_Undefined_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw StdFail_Undefined(MESSAGE);
#else
  #define StdFail_Undefined_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(StdFail_Undefined, Standard_Failure)

#endif // _StdFail_Undefined_HeaderFile
