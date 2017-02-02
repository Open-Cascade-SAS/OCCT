// Created on: 1992-02-03
// Created by: Christian CAILLET
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

#ifndef _Interface_CheckFailure_HeaderFile
#define _Interface_CheckFailure_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Interface_InterfaceError.hxx>

class Interface_CheckFailure;
DEFINE_STANDARD_HANDLE(Interface_CheckFailure, Interface_InterfaceError)

#if !defined No_Exception && !defined No_Interface_CheckFailure
  #define Interface_CheckFailure_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw Interface_CheckFailure(MESSAGE);
#else
  #define Interface_CheckFailure_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Interface_CheckFailure, Interface_InterfaceError)

#endif // _Interface_CheckFailure_HeaderFile
