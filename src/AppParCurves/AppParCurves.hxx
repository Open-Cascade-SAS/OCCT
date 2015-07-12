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

#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
#include <Standard_Real.hxx>
#include <math_IntegerVector.hxx>
class math_Matrix;
class AppParCurves_MultiPoint;
class AppParCurves_MultiCurve;
class AppParCurves_MultiBSpCurve;
class AppParCurves_ConstraintCouple;
class AppParCurves_LeastSquare;
class AppParCurves_ResolConstraint;
class AppParCurves_Function;
class AppParCurves_BSpFunction;
class AppParCurves_Gradient;
class AppParCurves_Gradient_BFGS;
class AppParCurves_ParLeastSquare;
class AppParCurves_ResConstraint;
class AppParCurves_ParFunction;
class AppParCurves_BSpGradient;
class AppParCurves_BSpGradient_BFGS;
class AppParCurves_BSpParLeastSquare;
class AppParCurves_BSpParFunction;


//! Parallel Approximation in n curves.
//! This package gives all the algorithms used to approximate a MultiLine
//! described by the tool MLineTool.
//! The result of the approximation will be a MultiCurve.
class AppParCurves 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void BernsteinMatrix (const Standard_Integer NbPoles, const math_Vector& U, math_Matrix& A);
  
  Standard_EXPORT static void Bernstein (const Standard_Integer NbPoles, const math_Vector& U, math_Matrix& A, math_Matrix& DA);
  
  Standard_EXPORT static void SecondDerivativeBernstein (const Standard_Real U, math_Vector& DDA);
  
  Standard_EXPORT static void SplineFunction (const Standard_Integer NbPoles, const Standard_Integer Degree, const math_Vector& Parameters, const math_Vector& FlatKnots, math_Matrix& A, math_Matrix& DA, math_IntegerVector& Index);




protected:





private:




friend class AppParCurves_MultiPoint;
friend class AppParCurves_MultiCurve;
friend class AppParCurves_MultiBSpCurve;
friend class AppParCurves_ConstraintCouple;
friend class AppParCurves_LeastSquare;
friend class AppParCurves_ResolConstraint;
friend class AppParCurves_Function;
friend class AppParCurves_BSpFunction;
friend class AppParCurves_Gradient;
friend class AppParCurves_Gradient_BFGS;
friend class AppParCurves_ParLeastSquare;
friend class AppParCurves_ResConstraint;
friend class AppParCurves_ParFunction;
friend class AppParCurves_BSpGradient;
friend class AppParCurves_BSpGradient_BFGS;
friend class AppParCurves_BSpParLeastSquare;
friend class AppParCurves_BSpParFunction;

};







#endif // _AppParCurves_HeaderFile
