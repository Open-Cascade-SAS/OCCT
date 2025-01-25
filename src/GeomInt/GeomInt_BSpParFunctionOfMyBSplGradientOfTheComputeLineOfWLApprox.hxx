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

#ifndef _GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox_HeaderFile
#define _GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <math_IntegerVector.hxx>
#include <AppParCurves_Constraint.hxx>
class GeomInt_TheMultiLineOfWLApprox;
class GeomInt_TheMultiLineToolOfWLApprox;
class GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox;
class AppParCurves_MultiBSpCurve;
class math_Matrix;

class GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox
    : public math_MultipleVarFunctionWithGradient
{
public:
  DEFINE_STANDARD_ALLOC

  //! initializes the fields of the function. The approximating
  //! curve has <NbPol> control points.
  Standard_EXPORT GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox(
    const GeomInt_TheMultiLineOfWLApprox&                 SSP,
    const Standard_Integer                                FirstPoint,
    const Standard_Integer                                LastPoint,
    const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints,
    const math_Vector&                                    Parameters,
    const TColStd_Array1OfReal&                           Knots,
    const TColStd_Array1OfInteger&                        Mults,
    const Standard_Integer                                NbPol);

  //! returns the number of variables of the function. It
  //! corresponds to the number of MultiPoints.
  Standard_EXPORT Standard_Integer NbVariables() const;

  //! this method computes the new approximation of the
  //! MultiLine
  //! SSP and calculates F = sum (||Pui - Bi*Pi||2) for each
  //! point of the MultiLine.
  Standard_EXPORT Standard_Boolean Value(const math_Vector& X, Standard_Real& F);

  //! returns the gradient G of the sum above for the
  //! parameters Xi.
  Standard_EXPORT Standard_Boolean Gradient(const math_Vector& X, math_Vector& G);

  //! returns the value F=sum(||Pui - Bi*Pi||)2.
  //! returns the value G = grad(F) for the parameters Xi.
  Standard_EXPORT Standard_Boolean Values(const math_Vector& X, Standard_Real& F, math_Vector& G);

  //! returns the new parameters of the MultiLine.
  Standard_EXPORT const math_Vector& NewParameters() const;

  //! returns the MultiBSpCurve approximating the set after
  //! computing the value F or Grad(F).
  Standard_EXPORT AppParCurves_MultiBSpCurve CurveValue();

  //! returns the distance between the MultiPoint of range
  //! IPoint and the curve CurveIndex.
  Standard_EXPORT Standard_Real Error(const Standard_Integer IPoint,
                                      const Standard_Integer CurveIndex);

  //! returns the maximum distance between the points
  //! and the MultiBSpCurve.
  Standard_EXPORT Standard_Real MaxError3d() const;

  //! returns the maximum distance between the points
  //! and the MultiBSpCurve.
  Standard_EXPORT Standard_Real MaxError2d() const;

  //! returns the function matrix used to approximate the
  //! multiline.
  Standard_EXPORT const math_Matrix& FunctionMatrix() const;

  //! returns the derivative function matrix used to approximate the
  //! multiline.
  Standard_EXPORT const math_Matrix& DerivativeFunctionMatrix() const;

  //! Returns the indexes of the first non null values of
  //! A and DA.
  //! The values are non null from Index(ieme point) +1
  //! to Index(ieme point) + degree +1.
  Standard_EXPORT const math_IntegerVector& Index() const;

  Standard_EXPORT AppParCurves_Constraint
    FirstConstraint(const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints,
                    const Standard_Integer                                FirstPoint) const;

  Standard_EXPORT AppParCurves_Constraint
    LastConstraint(const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints,
                   const Standard_Integer                                LastPoint) const;

  Standard_EXPORT void SetFirstLambda(const Standard_Real l1);

  Standard_EXPORT void SetLastLambda(const Standard_Real l2);

protected:
  //! this method is used each time Value or Gradient is
  //! needed.
  Standard_EXPORT void Perform(const math_Vector& X);

private:
  Standard_Boolean                                                    Done;
  GeomInt_TheMultiLineOfWLApprox                                      MyMultiLine;
  AppParCurves_MultiBSpCurve                                          MyMultiBSpCurve;
  Standard_Integer                                                    nbpoles;
  math_Vector                                                         myParameters;
  Standard_Real                                                       FVal;
  math_Vector                                                         ValGrad_F;
  math_Matrix                                                         MyF;
  math_Matrix                                                         PTLX;
  math_Matrix                                                         PTLY;
  math_Matrix                                                         PTLZ;
  math_Matrix                                                         A;
  math_Matrix                                                         DA;
  GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox MyLeastSquare;
  Standard_Boolean                                                    Contraintes;
  Standard_Integer                                                    NbP;
  Standard_Integer                                                    NbCu;
  Standard_Integer                                                    Adeb;
  Standard_Integer                                                    Afin;
  Handle(TColStd_HArray1OfInteger)                                    tabdim;
  Standard_Real                                                       ERR3d;
  Standard_Real                                                       ERR2d;
  Standard_Integer                                                    FirstP;
  Standard_Integer                                                    LastP;
  Handle(AppParCurves_HArray1OfConstraintCouple)                      myConstraints;
  Standard_Real                                                       mylambda1;
  Standard_Real                                                       mylambda2;
};

#endif // _GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox_HeaderFile
