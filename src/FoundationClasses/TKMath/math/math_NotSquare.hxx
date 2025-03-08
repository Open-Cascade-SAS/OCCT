// Created on: 1991-01-21
// Created by: Isabelle GRIGNON
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

#ifndef _math_NotSquare_HeaderFile
#define _math_NotSquare_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_DimensionError.hxx>

class math_NotSquare;
DEFINE_STANDARD_HANDLE(math_NotSquare, Standard_DimensionError)

#if !defined No_Exception && !defined No_math_NotSquare
  #define math_NotSquare_Raise_if(CONDITION, MESSAGE)                                              \
    if (CONDITION)                                                                                 \
      throw math_NotSquare(MESSAGE);
#else
  #define math_NotSquare_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(math_NotSquare, Standard_DimensionError)

#endif // _math_NotSquare_HeaderFile
