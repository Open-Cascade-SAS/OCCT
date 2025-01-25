// Created on: 1993-04-13
// Created by: JCV
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

#ifndef _gp_VectorWithNullMagnitude_HeaderFile
#define _gp_VectorWithNullMagnitude_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_DomainError.hxx>

class gp_VectorWithNullMagnitude;
DEFINE_STANDARD_HANDLE(gp_VectorWithNullMagnitude, Standard_DomainError)

#if !defined No_Exception && !defined No_gp_VectorWithNullMagnitude
  #define gp_VectorWithNullMagnitude_Raise_if(CONDITION, MESSAGE)                                  \
    if (CONDITION)                                                                                 \
      throw gp_VectorWithNullMagnitude(MESSAGE);
#else
  #define gp_VectorWithNullMagnitude_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(gp_VectorWithNullMagnitude, Standard_DomainError)

#endif // _gp_VectorWithNullMagnitude_HeaderFile
