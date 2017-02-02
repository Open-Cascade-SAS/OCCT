// Created on: 1992-08-26
// Created by: Jean Louis FRENKEL
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

#ifndef _Prs3d_InvalidAngle_HeaderFile
#define _Prs3d_InvalidAngle_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_RangeError.hxx>

class Prs3d_InvalidAngle;
DEFINE_STANDARD_HANDLE(Prs3d_InvalidAngle, Standard_RangeError)

#if !defined No_Exception && !defined No_Prs3d_InvalidAngle
  #define Prs3d_InvalidAngle_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw Prs3d_InvalidAngle(MESSAGE);
#else
  #define Prs3d_InvalidAngle_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Prs3d_InvalidAngle, Standard_RangeError)

#endif // _Prs3d_InvalidAngle_HeaderFile
