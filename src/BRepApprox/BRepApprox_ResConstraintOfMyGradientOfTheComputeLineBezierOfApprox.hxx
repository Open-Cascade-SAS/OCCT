// Created on: 1995-06-06
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox_HeaderFile
#define _BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
class Standard_OutOfRange;
class BRepApprox_TheMultiLineOfApprox;
class BRepApprox_TheMultiLineToolOfApprox;
class AppParCurves_MultiCurve;
class math_Matrix;



class BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox(const BRepApprox_TheMultiLineOfApprox& SSP, AppParCurves_MultiCurve& SCurv, const Standard_Integer FirstPoint, const Standard_Integer LastPoint, const Handle(AppParCurves_HArray1OfConstraintCouple)& Constraints, const math_Matrix& Bern, const math_Matrix& DerivativeBern, const Standard_Real Tolerance = 1.0e-10);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Real Error() const;
  
  Standard_EXPORT const math_Matrix& ConstraintMatrix() const;
  
  Standard_EXPORT const math_Vector& Duale() const;
  
  Standard_EXPORT const math_Matrix& ConstraintDerivative (const BRepApprox_TheMultiLineOfApprox& SSP, const math_Vector& Parameters, const Standard_Integer Deg, const math_Matrix& DA);
  
  Standard_EXPORT const math_Matrix& InverseMatrix() const;




protected:

  
  Standard_EXPORT Standard_Integer NbConstraints (const BRepApprox_TheMultiLineOfApprox& SSP, const Standard_Integer FirstPoint, const Standard_Integer LastPoint, const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints) const;
  
  Standard_EXPORT Standard_Integer NbColumns (const BRepApprox_TheMultiLineOfApprox& SSP, const Standard_Integer Deg) const;




private:



  Standard_Boolean Done;
  Standard_Real Err;
  math_Matrix Cont;
  math_Matrix DeCont;
  math_Vector Secont;
  math_Matrix CTCinv;
  math_Vector Vardua;
  Standard_Integer IncPass;
  Standard_Integer IncTan;
  Standard_Integer IncCurv;
  TColStd_Array1OfInteger IPas;
  TColStd_Array1OfInteger ITan;
  TColStd_Array1OfInteger ICurv;


};







#endif // _BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox_HeaderFile
