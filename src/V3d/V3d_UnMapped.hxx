// Created on: 1992-11-13
// Created by: GG
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

#ifndef _V3d_UnMapped_HeaderFile
#define _V3d_UnMapped_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_DomainError.hxx>

class V3d_UnMapped;
DEFINE_STANDARD_HANDLE(V3d_UnMapped, Standard_DomainError)

#if !defined No_Exception && !defined No_V3d_UnMapped
  #define V3d_UnMapped_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw V3d_UnMapped(MESSAGE);
#else
  #define V3d_UnMapped_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(V3d_UnMapped, Standard_DomainError)

#endif // _V3d_UnMapped_HeaderFile
