// Created on: 1994-03-03
// Created by: Joelle CHAUVET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepFill_ComputeCLine_HeaderFile
#define _BRepFill_ComputeCLine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <AppParCurves_MultiCurve.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <AppParCurves_Constraint.hxx>
#include <BRepFill_MultiLine.hxx>
class AppParCurves_MultiCurve;

class BRepFill_ComputeCLine
{
public:
  DEFINE_STANDARD_ALLOC

  //! The MultiLine <Line> will be approximated until tolerances
  //! will be reached.
  //! The approximation will be done from degreemin to degreemax
  //! with a cutting if the corresponding boolean is True.
  Standard_EXPORT BRepFill_ComputeCLine(
    const BRepFill_MultiLine&     Line,
    const int                     degreemin   = 3,
    const int                     degreemax   = 8,
    const double                  Tolerance3d = 1.0e-5,
    const double                  Tolerance2d = 1.0e-5,
    const bool                    cutting     = false,
    const AppParCurves_Constraint FirstC      = AppParCurves_TangencyPoint,
    const AppParCurves_Constraint LastC       = AppParCurves_TangencyPoint);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT BRepFill_ComputeCLine(
    const int                     degreemin   = 3,
    const int                     degreemax   = 8,
    const double                  Tolerance3d = 1.0e-05,
    const double                  Tolerance2d = 1.0e-05,
    const bool                    cutting     = false,
    const AppParCurves_Constraint FirstC      = AppParCurves_TangencyPoint,
    const AppParCurves_Constraint LastC       = AppParCurves_TangencyPoint);

  //! runs the algorithm after having initialized the fields.
  Standard_EXPORT void Perform(const BRepFill_MultiLine& Line);

  //! changes the degrees of the approximation.
  Standard_EXPORT void SetDegrees(const int degreemin, const int degreemax);

  //! Changes the tolerances of the approximation.
  Standard_EXPORT void SetTolerances(const double Tolerance3d, const double Tolerance2d);

  //! Changes the constraints of the approximation.
  Standard_EXPORT void SetConstraints(const AppParCurves_Constraint FirstC,
                                      const AppParCurves_Constraint LastC);

  //! Changes the max number of segments, which is allowed for cutting.
  Standard_EXPORT void SetMaxSegments(const int theMaxSegments);

  //! Set inverse order of degree selection:
  //! if theInvOrdr = true, current degree is chosen by inverse order -
  //! from maxdegree to mindegree.
  //! By default inverse order is used.
  Standard_EXPORT void SetInvOrder(const bool theInvOrder);

  //! Set value of hang checking flag
  //! if this flag = true, possible hang of algorithm is checked
  //! and algorithm is forced to stop.
  //! By default hang checking is used.
  Standard_EXPORT void SetHangChecking(const bool theHangChecking);

  //! returns False if at a moment of the approximation,
  //! the status NoApproximation has been sent by the user
  //! when more points were needed.
  Standard_EXPORT bool IsAllApproximated() const;

  //! returns False if the status NoPointsAdded has been sent.
  Standard_EXPORT bool IsToleranceReached() const;

  //! returns the tolerances 2d and 3d of the <Index> MultiCurve.
  Standard_EXPORT void Error(const int Index, double& tol3d, double& tol2d) const;

  //! Returns the number of MultiCurve doing the approximation
  //! of the MultiLine.
  Standard_EXPORT int NbMultiCurves() const;

  //! returns the approximation MultiCurve of range <Index>.
  Standard_EXPORT AppParCurves_MultiCurve Value(const int Index = 1) const;

  Standard_EXPORT void Parameters(const int Index, double& firstp, double& lastp) const;

private:
  //! is internally used by the algorithms.
  Standard_EXPORT bool Compute(const BRepFill_MultiLine& Line,
                               const double              Ufirst,
                               const double              Ulast,
                               double&                   TheTol3d,
                               double&                   TheTol2d);

  NCollection_Sequence<AppParCurves_MultiCurve> myMultiCurves;
  NCollection_Sequence<double>                  myfirstparam;
  NCollection_Sequence<double>                  mylastparam;
  AppParCurves_MultiCurve                       TheMultiCurve;
  bool                                          alldone;
  bool                                          tolreached;
  NCollection_Sequence<double>                  Tolers3d;
  NCollection_Sequence<double>                  Tolers2d;
  int                                           mydegremin;
  int                                           mydegremax;
  double                                        mytol3d;
  double                                        mytol2d;
  double                                        currenttol3d;
  double                                        currenttol2d;
  bool                                          mycut;
  AppParCurves_Constraint                       myfirstC;
  AppParCurves_Constraint                       mylastC;
  int                                           myMaxSegments;
  bool                                          myInvOrder;
  bool                                          myHangChecking;
};

#endif // _BRepFill_ComputeCLine_HeaderFile
