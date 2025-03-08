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

#ifndef _Standard_NotImplemented_HeaderFile
#define _Standard_NotImplemented_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_ProgramError.hxx>

class Standard_NotImplemented;
DEFINE_STANDARD_HANDLE(Standard_NotImplemented, Standard_ProgramError)

#if !defined No_Exception && !defined No_Standard_NotImplemented
  #define Standard_NotImplemented_Raise_if(CONDITION, MESSAGE)                                     \
    if (CONDITION)                                                                                 \
      throw Standard_NotImplemented(MESSAGE);
#else
  #define Standard_NotImplemented_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Standard_NotImplemented, Standard_ProgramError)

#endif // _Standard_NotImplemented_HeaderFile
