// Created on: 1991-03-27
// Created by: Michel CHAUVAT
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

#ifndef _LProp_NotDefined_HeaderFile
#define _LProp_NotDefined_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_Failure.hxx>

class LProp_NotDefined;
DEFINE_STANDARD_HANDLE(LProp_NotDefined, Standard_Failure)

#if !defined No_Exception && !defined No_LProp_NotDefined
  #define LProp_NotDefined_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw LProp_NotDefined(MESSAGE);
#else
  #define LProp_NotDefined_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(LProp_NotDefined, Standard_Failure)

#endif // _LProp_NotDefined_HeaderFile
