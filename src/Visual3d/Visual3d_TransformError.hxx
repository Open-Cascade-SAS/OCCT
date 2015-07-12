// Created on: 1991-10-07
// Created by: NW,JPB,CAL
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

#ifndef _Visual3d_TransformError_HeaderFile
#define _Visual3d_TransformError_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_OutOfRange.hxx>

class Visual3d_TransformError;
DEFINE_STANDARD_HANDLE(Visual3d_TransformError, Standard_OutOfRange)

#if !defined No_Exception && !defined No_Visual3d_TransformError
  #define Visual3d_TransformError_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) Visual3d_TransformError::Raise(MESSAGE);
#else
  #define Visual3d_TransformError_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Visual3d_TransformError, Standard_OutOfRange)

#endif // _Visual3d_TransformError_HeaderFile
