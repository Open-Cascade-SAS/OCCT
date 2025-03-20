// Created on: 1996-01-23
// Created by: s:       LAVNIKOV Alexey, PLOTNIKOV Eugeny & CHABROVSKY Dmitry
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

#ifndef _WNT_ClassDefinitionError_HeaderFile
#define _WNT_ClassDefinitionError_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_ConstructionError.hxx>

class WNT_ClassDefinitionError;
DEFINE_STANDARD_HANDLE(WNT_ClassDefinitionError, Standard_ConstructionError)

#if !defined No_Exception && !defined No_WNT_ClassDefinitionError
  #define WNT_ClassDefinitionError_Raise_if(CONDITION, MESSAGE)                                    \
    if (CONDITION)                                                                                 \
      throw WNT_ClassDefinitionError(MESSAGE);
#else
  #define WNT_ClassDefinitionError_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(WNT_ClassDefinitionError, Standard_ConstructionError)

#endif // _WNT_ClassDefinitionError_HeaderFile
