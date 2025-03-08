// Created on: 1991-04-11
// Created by: Laurent PAINNOT
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

#ifndef _AppParCurves_HeaderFile
#define _AppParCurves_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_Vector.hxx>
#include <math_IntegerVector.hxx>
class math_Matrix;

//! Parallel Approximation in n curves.
//! This package gives all the algorithms used to approximate a MultiLine
//! described by the tool MLineTool.
//! The result of the approximation will be a MultiCurve.
class AppParCurves
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void BernsteinMatrix(const Standard_Integer NbPoles,
                                              const math_Vector&     U,
                                              math_Matrix&           A);

  Standard_EXPORT static void Bernstein(const Standard_Integer NbPoles,
                                        const math_Vector&     U,
                                        math_Matrix&           A,
                                        math_Matrix&           DA);

  Standard_EXPORT static void SecondDerivativeBernstein(const Standard_Real U, math_Vector& DDA);

  Standard_EXPORT static void SplineFunction(const Standard_Integer NbPoles,
                                             const Standard_Integer Degree,
                                             const math_Vector&     Parameters,
                                             const math_Vector&     FlatKnots,
                                             math_Matrix&           A,
                                             math_Matrix&           DA,
                                             math_IntegerVector&    Index);
};

#endif // _AppParCurves_HeaderFile
