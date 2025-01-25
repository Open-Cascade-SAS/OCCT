// Created on: 1991-12-02
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

#ifndef _AppDef_MyBSplGradientOfBSplineCompute_HeaderFile
#define _AppDef_MyBSplGradientOfBSplineCompute_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_MultiBSpCurve.hxx>
#include <math_Vector.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
class Standard_OutOfRange;
class StdFail_NotDone;
class AppDef_MultiLine;
class AppDef_MyLineTool;
class AppDef_BSpParLeastSquareOfMyBSplGradientOfBSplineCompute;
class AppDef_BSpParFunctionOfMyBSplGradientOfBSplineCompute;
class AppDef_BSpGradient_BFGSOfMyBSplGradientOfBSplineCompute;
class AppParCurves_MultiBSpCurve;

class AppDef_MyBSplGradientOfBSplineCompute
{
public:
  DEFINE_STANDARD_ALLOC

  //! Tries to minimize the sum (square(||Qui - Bi*Pi||))
  //! where Pui describe the approximating BSpline curves'Poles
  //! and Qi the MultiLine points with a parameter ui.
  //! In this algorithm, the parameters ui are the unknowns.
  //! The tolerance required on this sum is given by Tol.
  //! The desired degree of the resulting curve is Deg.
  Standard_EXPORT AppDef_MyBSplGradientOfBSplineCompute(
    const AppDef_MultiLine&                               SSP,
    const Standard_Integer                                FirstPoint,
    const Standard_Integer                                LastPoint,
    const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints,
    math_Vector&                                          Parameters,
    const TColStd_Array1OfReal&                           Knots,
    const TColStd_Array1OfInteger&                        Mults,
    const Standard_Integer                                Deg,
    const Standard_Real                                   Tol3d,
    const Standard_Real                                   Tol2d,
    const Standard_Integer                                NbIterations = 1);

  //! Tries to minimize the sum (square(||Qui - Bi*Pi||))
  //! where Pui describe the approximating BSpline curves'Poles
  //! and Qi the MultiLine points with a parameter ui.
  //! In this algorithm, the parameters ui are the unknowns.
  //! The tolerance required on this sum is given by Tol.
  //! The desired degree of the resulting curve is Deg.
  Standard_EXPORT AppDef_MyBSplGradientOfBSplineCompute(
    const AppDef_MultiLine&                               SSP,
    const Standard_Integer                                FirstPoint,
    const Standard_Integer                                LastPoint,
    const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints,
    math_Vector&                                          Parameters,
    const TColStd_Array1OfReal&                           Knots,
    const TColStd_Array1OfInteger&                        Mults,
    const Standard_Integer                                Deg,
    const Standard_Real                                   Tol3d,
    const Standard_Real                                   Tol2d,
    const Standard_Integer                                NbIterations,
    const Standard_Real                                   lambda1,
    const Standard_Real                                   lambda2);

  //! returns True if all has been correctly done.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! returns all the BSpline curves approximating the
  //! MultiLine SSP after minimization of the parameter.
  Standard_EXPORT AppParCurves_MultiBSpCurve Value() const;

  //! returns the difference between the old and the new
  //! approximation.
  //! An exception is raised if NotDone.
  //! An exception is raised if Index<1 or Index>NbParameters.
  Standard_EXPORT Standard_Real Error(const Standard_Integer Index) const;

  //! returns the maximum difference between the old and the
  //! new approximation.
  Standard_EXPORT Standard_Real MaxError3d() const;

  //! returns the maximum difference between the old and the
  //! new approximation.
  Standard_EXPORT Standard_Real MaxError2d() const;

  //! returns the average error between the old and the
  //! new approximation.
  Standard_EXPORT Standard_Real AverageError() const;

protected:
  Standard_EXPORT void Perform(const AppDef_MultiLine&                               SSP,
                               const Standard_Integer                                FirstPoint,
                               const Standard_Integer                                LastPoint,
                               const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints,
                               math_Vector&                                          Parameters,
                               const TColStd_Array1OfReal&                           Knots,
                               const TColStd_Array1OfInteger&                        Mults,
                               const Standard_Integer                                Deg,
                               const Standard_Real                                   Tol3d,
                               const Standard_Real                                   Tol2d,
                               const Standard_Integer NbIterations = 200);

private:
  AppParCurves_MultiBSpCurve SCU;
  math_Vector                ParError;
  Standard_Real              AvError;
  Standard_Real              MError3d;
  Standard_Real              MError2d;
  Standard_Real              mylambda1;
  Standard_Real              mylambda2;
  Standard_Boolean           myIsLambdaDefined;
  Standard_Boolean           Done;
};

#endif // _AppDef_MyBSplGradientOfBSplineCompute_HeaderFile
