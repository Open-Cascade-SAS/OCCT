// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
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

#ifndef _GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox_HeaderFile
#define _GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox_HeaderFile

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
class GeomInt_TheMultiLineOfWLApprox;
class GeomInt_TheMultiLineToolOfWLApprox;
class AppParCurves_MultiCurve;
class math_Matrix;



class GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Given a MultiLine SSP with constraints points, this
  //! algorithm finds the best curve solution to approximate it.
  //! The poles from SCurv issued for example from the least
  //! squares are used as a guess solution for the uzawa
  //! algorithm. The tolerance used in the Uzawa algorithms
  //! is Tolerance.
  //! A is the Bernstein matrix associated to the MultiLine
  //! and DA is the derivative bernstein matrix.(They can come
  //! from an approximation with ParLeastSquare.)
  //! The MultiCurve is modified. New MultiPoles are given.
  Standard_EXPORT GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox(const GeomInt_TheMultiLineOfWLApprox& SSP, AppParCurves_MultiCurve& SCurv, const Standard_Integer FirstPoint, const Standard_Integer LastPoint, const Handle(AppParCurves_HArray1OfConstraintCouple)& Constraints, const math_Matrix& Bern, const math_Matrix& DerivativeBern, const Standard_Real Tolerance = 1.0e-10);
  
  //! returns True if all has been correctly done.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! returns the maximum difference value between the curve
  //! and the given points.
  Standard_EXPORT Standard_Real Error() const;
  
  Standard_EXPORT const math_Matrix& ConstraintMatrix() const;
  
  //! returns the duale variables of the system.
  Standard_EXPORT const math_Vector& Duale() const;
  
  //! Returns the derivative of the constraint matrix.
  Standard_EXPORT const math_Matrix& ConstraintDerivative (const GeomInt_TheMultiLineOfWLApprox& SSP, const math_Vector& Parameters, const Standard_Integer Deg, const math_Matrix& DA);
  
  //! returns the Inverse of Cont*Transposed(Cont), where
  //! Cont is the constraint matrix for the algorithm.
  Standard_EXPORT const math_Matrix& InverseMatrix() const;




protected:

  
  //! is used internally to create the fields.
  Standard_EXPORT Standard_Integer NbConstraints (const GeomInt_TheMultiLineOfWLApprox& SSP, const Standard_Integer FirstPoint, const Standard_Integer LastPoint, const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints) const;
  
  //! is internally used for the fields creation.
  Standard_EXPORT Standard_Integer NbColumns (const GeomInt_TheMultiLineOfWLApprox& SSP, const Standard_Integer Deg) const;




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







#endif // _GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox_HeaderFile
