// Created on: 1994-02-08
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Quantity_DateDefinitionError_HeaderFile
#define _Quantity_DateDefinitionError_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_SStream.hxx>
#include <Standard_DomainError.hxx>

class Quantity_DateDefinitionError;
DEFINE_STANDARD_HANDLE(Quantity_DateDefinitionError, Standard_DomainError)

#if !defined No_Exception && !defined No_Quantity_DateDefinitionError
  #define Quantity_DateDefinitionError_Raise_if(CONDITION, MESSAGE)                                \
    if (CONDITION)                                                                                 \
      throw Quantity_DateDefinitionError(MESSAGE);
#else
  #define Quantity_DateDefinitionError_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Quantity_DateDefinitionError, Standard_DomainError)

#endif // _Quantity_DateDefinitionError_HeaderFile
