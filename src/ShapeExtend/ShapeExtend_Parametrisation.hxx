// Created on: 1998-07-21
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeExtend_Parametrisation_HeaderFile
#define _ShapeExtend_Parametrisation_HeaderFile

//! Defines kind of global parametrisation on the composite surface
//! each patch of the 1st row and column adds its range, Ui+1 = Ui + URange(i,1), etc.
//! each patch gives range 1.: Ui = i-1, Vj = j-1
//! uniform parametrisation with global range [0,1]
enum ShapeExtend_Parametrisation
{
ShapeExtend_Natural,
ShapeExtend_Uniform,
ShapeExtend_Unitary
};

#endif // _ShapeExtend_Parametrisation_HeaderFile
