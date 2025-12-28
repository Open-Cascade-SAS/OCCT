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

#ifndef _GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox_HeaderFile
#define _GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_MultiBSpCurve.hxx>
#include <math_Vector.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <AppParCurves_ConstraintCouple.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
class Standard_OutOfRange;
class StdFail_NotDone;
class GeomInt_TheMultiLineOfWLApprox;
class GeomInt_TheMultiLineToolOfWLApprox;
class GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox;
class GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox;
class GeomInt_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfWLApprox;
class AppParCurves_MultiBSpCurve;

class GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox
{
public:
  DEFINE_STANDARD_ALLOC

  //! Tries to minimize the sum (square(||Qui - Bi*Pi||))
  //! where Pui describe the approximating BSpline curves'Poles
  //! and Qi the MultiLine points with a parameter ui.
  //! In this algorithm, the parameters ui are the unknowns.
  //! The tolerance required on this sum is given by Tol.
  //! The desired degree of the resulting curve is Deg.
  Standard_EXPORT GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox(
    const GeomInt_TheMultiLineOfWLApprox&                 SSP,
    const int                                FirstPoint,
    const int                                LastPoint,
    const occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>>& TheConstraints,
    math_Vector&                                          Parameters,
    const NCollection_Array1<double>&                           Knots,
    const NCollection_Array1<int>&                        Mults,
    const int                                Deg,
    const double                                   Tol3d,
    const double                                   Tol2d,
    const int                                NbIterations = 1);

  //! Tries to minimize the sum (square(||Qui - Bi*Pi||))
  //! where Pui describe the approximating BSpline curves'Poles
  //! and Qi the MultiLine points with a parameter ui.
  //! In this algorithm, the parameters ui are the unknowns.
  //! The tolerance required on this sum is given by Tol.
  //! The desired degree of the resulting curve is Deg.
  Standard_EXPORT GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox(
    const GeomInt_TheMultiLineOfWLApprox&                 SSP,
    const int                                FirstPoint,
    const int                                LastPoint,
    const occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>>& TheConstraints,
    math_Vector&                                          Parameters,
    const NCollection_Array1<double>&                           Knots,
    const NCollection_Array1<int>&                        Mults,
    const int                                Deg,
    const double                                   Tol3d,
    const double                                   Tol2d,
    const int                                NbIterations,
    const double                                   lambda1,
    const double                                   lambda2);

  //! returns True if all has been correctly done.
  Standard_EXPORT bool IsDone() const;

  //! returns all the BSpline curves approximating the
  //! MultiLine SSP after minimization of the parameter.
  Standard_EXPORT AppParCurves_MultiBSpCurve Value() const;

  //! returns the difference between the old and the new
  //! approximation.
  //! An exception is raised if NotDone.
  //! An exception is raised if Index<1 or Index>NbParameters.
  Standard_EXPORT double Error(const int Index) const;

  //! returns the maximum difference between the old and the
  //! new approximation.
  Standard_EXPORT double MaxError3d() const;

  //! returns the maximum difference between the old and the
  //! new approximation.
  Standard_EXPORT double MaxError2d() const;

  //! returns the average error between the old and the
  //! new approximation.
  Standard_EXPORT double AverageError() const;

protected:
  Standard_EXPORT void Perform(const GeomInt_TheMultiLineOfWLApprox&                 SSP,
                               const int                                FirstPoint,
                               const int                                LastPoint,
                               const occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>>& TheConstraints,
                               math_Vector&                                          Parameters,
                               const NCollection_Array1<double>&                           Knots,
                               const NCollection_Array1<int>&                        Mults,
                               const int                                Deg,
                               const double                                   Tol3d,
                               const double                                   Tol2d,
                               const int NbIterations = 200);

private:
  AppParCurves_MultiBSpCurve SCU;
  math_Vector                ParError;
  double              AvError;
  double              MError3d;
  double              MError2d;
  double              mylambda1;
  double              mylambda2;
  bool           myIsLambdaDefined;
  bool           Done;
};

#endif // _GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox_HeaderFile
