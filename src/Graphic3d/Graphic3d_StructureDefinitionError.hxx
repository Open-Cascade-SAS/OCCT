// Created on: 1993-03-31
// Created by: NW,JPB,CAL
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Graphic3d_StructureDefinitionError_HeaderFile
#define _Graphic3d_StructureDefinitionError_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_OutOfRange.hxx>

class Graphic3d_StructureDefinitionError;
DEFINE_STANDARD_HANDLE(Graphic3d_StructureDefinitionError, Standard_OutOfRange)

#if !defined No_Exception && !defined No_Graphic3d_StructureDefinitionError
  #define Graphic3d_StructureDefinitionError_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw Graphic3d_StructureDefinitionError(MESSAGE);
#else
  #define Graphic3d_StructureDefinitionError_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Graphic3d_StructureDefinitionError, Standard_OutOfRange)

#endif // _Graphic3d_StructureDefinitionError_HeaderFile
