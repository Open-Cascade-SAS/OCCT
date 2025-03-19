// Created on: 1990-12-11
// Created by: Remi Lequette
// Copyright (c) 1990-1999 Matra Datavision
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

#ifndef _TopoDS_LockedShape_HeaderFile
#define _TopoDS_LockedShape_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_DomainError.hxx>

//! An attempt was made to modify a geometry of Shape already
//! shared or protected.

class TopoDS_LockedShape;
DEFINE_STANDARD_HANDLE(TopoDS_LockedShape, Standard_DomainError)

#if !defined No_Exception && !defined No_TopoDS_LockedShape
  #define TopoDS_LockedShape_Raise_if(CONDITION, MESSAGE)                                          \
    if (CONDITION)                                                                                 \
      throw TopoDS_LockedShape(MESSAGE);
#else
  #define TopoDS_LockedShape_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(TopoDS_LockedShape, Standard_DomainError)

#endif // _TopoDS_LockedShape_HeaderFile
