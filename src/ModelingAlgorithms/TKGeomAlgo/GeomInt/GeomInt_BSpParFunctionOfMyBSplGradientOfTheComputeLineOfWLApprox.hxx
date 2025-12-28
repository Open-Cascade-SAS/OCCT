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
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <AppParCurves_ConstraintCouple.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
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
    const int                                FirstPoint,
    const int                                LastPoint,
    const occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>>& TheConstraints,
    const math_Vector&                                    Parameters,
    const NCollection_Array1<double>&                           Knots,
    const NCollection_Array1<int>&                        Mults,
    const int                                NbPol);

  //! returns the number of variables of the function. It
  //! corresponds to the number of MultiPoints.
  Standard_EXPORT int NbVariables() const;

  //! this method computes the new approximation of the
  //! MultiLine
  //! SSP and calculates F = sum (||Pui - Bi*Pi||2) for each
  //! point of the MultiLine.
  Standard_EXPORT bool Value(const math_Vector& X, double& F);

  //! returns the gradient G of the sum above for the
  //! parameters Xi.
  Standard_EXPORT bool Gradient(const math_Vector& X, math_Vector& G);

  //! returns the value F=sum(||Pui - Bi*Pi||)2.
  //! returns the value G = grad(F) for the parameters Xi.
  Standard_EXPORT bool Values(const math_Vector& X, double& F, math_Vector& G);

  //! returns the new parameters of the MultiLine.
  Standard_EXPORT const math_Vector& NewParameters() const;

  //! returns the MultiBSpCurve approximating the set after
  //! computing the value F or Grad(F).
  Standard_EXPORT AppParCurves_MultiBSpCurve CurveValue();

  //! returns the distance between the MultiPoint of range
  //! IPoint and the curve CurveIndex.
  Standard_EXPORT double Error(const int IPoint,
                                      const int CurveIndex);

  //! returns the maximum distance between the points
  //! and the MultiBSpCurve.
  Standard_EXPORT double MaxError3d() const;

  //! returns the maximum distance between the points
  //! and the MultiBSpCurve.
  Standard_EXPORT double MaxError2d() const;

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
    FirstConstraint(const occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>>& TheConstraints,
                    const int                                FirstPoint) const;

  Standard_EXPORT AppParCurves_Constraint
    LastConstraint(const occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>>& TheConstraints,
                   const int                                LastPoint) const;

  Standard_EXPORT void SetFirstLambda(const double l1);

  Standard_EXPORT void SetLastLambda(const double l2);

protected:
  //! this method is used each time Value or Gradient is
  //! needed.
  Standard_EXPORT void Perform(const math_Vector& X);

private:
  bool                                                    Done;
  GeomInt_TheMultiLineOfWLApprox                                      MyMultiLine;
  AppParCurves_MultiBSpCurve                                          MyMultiBSpCurve;
  int                                                    nbpoles;
  math_Vector                                                         myParameters;
  double                                                       FVal;
  math_Vector                                                         ValGrad_F;
  math_Matrix                                                         MyF;
  math_Matrix                                                         PTLX;
  math_Matrix                                                         PTLY;
  math_Matrix                                                         PTLZ;
  math_Matrix                                                         A;
  math_Matrix                                                         DA;
  GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox MyLeastSquare;
  bool                                                    Contraintes;
  int                                                    NbP;
  int                                                    NbCu;
  int                                                    Adeb;
  int                                                    Afin;
  occ::handle<NCollection_HArray1<int>>                                    tabdim;
  double                                                       ERR3d;
  double                                                       ERR2d;
  int                                                    FirstP;
  int                                                    LastP;
  occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>>                      myConstraints;
  double                                                       mylambda1;
  double                                                       mylambda2;
};

#endif // _GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox_HeaderFile
