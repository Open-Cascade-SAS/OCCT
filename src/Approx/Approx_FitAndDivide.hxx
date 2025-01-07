// Created on: 1993-01-26
// Created by: Laurent PAINNOT
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Approx_FitAndDivide_HeaderFile
#define _Approx_FitAndDivide_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <AppCont_Function.hxx>
#include <AppParCurves_Constraint.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_SequenceOfMultiCurve.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfReal.hxx>
class AppParCurves_MultiCurve;

class Approx_FitAndDivide
{
public:
  DEFINE_STANDARD_ALLOC

  //! The MultiLine <Line> will be approximated until tolerances
  //! will be reached.
  //! The approximation will be done from degreemin to degreemax
  //! with a cutting if the corresponding boolean is True.
  Standard_EXPORT Approx_FitAndDivide(
    const AppCont_Function&       Line,
    const Standard_Integer        degreemin   = 3,
    const Standard_Integer        degreemax   = 8,
    const Standard_Real           Tolerance3d = 1.0e-5,
    const Standard_Real           Tolerance2d = 1.0e-5,
    const Standard_Boolean        cutting     = Standard_False,
    const AppParCurves_Constraint FirstC      = AppParCurves_TangencyPoint,
    const AppParCurves_Constraint LastC       = AppParCurves_TangencyPoint);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT Approx_FitAndDivide(
    const Standard_Integer        degreemin   = 3,
    const Standard_Integer        degreemax   = 8,
    const Standard_Real           Tolerance3d = 1.0e-05,
    const Standard_Real           Tolerance2d = 1.0e-05,
    const Standard_Boolean        cutting     = Standard_False,
    const AppParCurves_Constraint FirstC      = AppParCurves_TangencyPoint,
    const AppParCurves_Constraint LastC       = AppParCurves_TangencyPoint);

  //! runs the algorithm after having initialized the fields.
  Standard_EXPORT void Perform(const AppCont_Function& Line);

  //! changes the degrees of the approximation.
  Standard_EXPORT void SetDegrees(const Standard_Integer degreemin,
                                  const Standard_Integer degreemax);

  //! Changes the tolerances of the approximation.
  Standard_EXPORT void SetTolerances(const Standard_Real Tolerance3d,
                                     const Standard_Real Tolerance2d);

  //! Changes the constraints of the approximation.
  Standard_EXPORT void SetConstraints(const AppParCurves_Constraint FirstC,
                                      const AppParCurves_Constraint LastC);

  //! Changes the max number of segments, which is allowed for cutting.
  Standard_EXPORT void SetMaxSegments(const Standard_Integer theMaxSegments);

  //! Set inverse order of degree selection:
  //! if theInvOrdr = true, current degree is chosen by inverse order -
  //! from maxdegree to mindegree.
  //! By default inverse order is used.
  Standard_EXPORT void SetInvOrder(const Standard_Boolean theInvOrder);

  //! Set value of hang checking flag
  //! if this flag = true, possible hang of algorithm is checked
  //! and algorithm is forced to stop.
  //! By default hang checking is used.
  Standard_EXPORT void SetHangChecking(const Standard_Boolean theHangChecking);

  //! returns False if at a moment of the approximation,
  //! the status NoApproximation has been sent by the user
  //! when more points were needed.
  Standard_EXPORT Standard_Boolean IsAllApproximated() const;

  //! returns False if the status NoPointsAdded has been sent.
  Standard_EXPORT Standard_Boolean IsToleranceReached() const;

  //! returns the tolerances 2d and 3d of the <Index> MultiCurve.
  Standard_EXPORT void Error(const Standard_Integer Index,
                             Standard_Real&         tol3d,
                             Standard_Real&         tol2d) const;

  //! Returns the number of MultiCurve doing the approximation
  //! of the MultiLine.
  Standard_EXPORT Standard_Integer NbMultiCurves() const;

  //! returns the approximation MultiCurve of range <Index>.
  Standard_EXPORT AppParCurves_MultiCurve Value(const Standard_Integer Index = 1) const;

  Standard_EXPORT void Parameters(const Standard_Integer Index,
                                  Standard_Real&         firstp,
                                  Standard_Real&         lastp) const;

protected:
private:
  //! is internally used by the algorithms.
  Standard_EXPORT Standard_Boolean Compute(const AppCont_Function& Line,
                                           const Standard_Real     Ufirst,
                                           const Standard_Real     Ulast,
                                           Standard_Real&          TheTol3d,
                                           Standard_Real&          TheTol2d);

  AppParCurves_SequenceOfMultiCurve myMultiCurves;
  TColStd_SequenceOfReal            myfirstparam;
  TColStd_SequenceOfReal            mylastparam;
  AppParCurves_MultiCurve           TheMultiCurve;
  Standard_Boolean                  alldone;
  Standard_Boolean                  tolreached;
  TColStd_SequenceOfReal            Tolers3d;
  TColStd_SequenceOfReal            Tolers2d;
  Standard_Integer                  mydegremin;
  Standard_Integer                  mydegremax;
  Standard_Real                     mytol3d;
  Standard_Real                     mytol2d;
  Standard_Real                     currenttol3d;
  Standard_Real                     currenttol2d;
  Standard_Boolean                  mycut;
  AppParCurves_Constraint           myfirstC;
  AppParCurves_Constraint           mylastC;
  Standard_Integer                  myMaxSegments;
  Standard_Boolean                  myInvOrder;
  Standard_Boolean                  myHangChecking;
};

#endif // _Approx_FitAndDivide_HeaderFile
