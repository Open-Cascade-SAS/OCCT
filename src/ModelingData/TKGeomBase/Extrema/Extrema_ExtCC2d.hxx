// Created on: 1994-07-06
// Created by: Laurent PAINNOT
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

#ifndef _Extrema_ExtCC2d_HeaderFile
#define _Extrema_ExtCC2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Extrema_POnCurv2d.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt2d.hxx>

#include <Extrema_ECC2d.hxx>

class Adaptor2d_Curve2d;
class Extrema_POnCurv2d;
class Extrema_ExtElC2d;

//! It calculates all the distance between two curves.
//! These distances can be maximum or minimum.
class Extrema_ExtCC2d
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Extrema_ExtCC2d();

  //! It calculates all the distances.
  Standard_EXPORT Extrema_ExtCC2d(const Adaptor2d_Curve2d& C1,
                                  const Adaptor2d_Curve2d& C2,
                                  const double      TolC1 = 1.0e-10,
                                  const double      TolC2 = 1.0e-10);

  //! It calculates all the distances.
  Standard_EXPORT Extrema_ExtCC2d(const Adaptor2d_Curve2d& C1,
                                  const Adaptor2d_Curve2d& C2,
                                  const double      U1,
                                  const double      U2,
                                  const double      V1,
                                  const double      V2,
                                  const double      TolC1 = 1.0e-10,
                                  const double      TolC2 = 1.0e-10);

  //! initializes the fields.
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& C2,
                                  const double      V1,
                                  const double      V2,
                                  const double      TolC1 = 1.0e-10,
                                  const double      TolC2 = 1.0e-10);

  Standard_EXPORT void Perform(const Adaptor2d_Curve2d& C1,
                               const double      U1,
                               const double      U2);

  //! Returns True if the distances are found.
  Standard_EXPORT bool IsDone() const;

  //! Returns the number of extremum distances.
  Standard_EXPORT int NbExt() const;

  //! Returns True if the two curves are parallel.
  Standard_EXPORT bool IsParallel() const;

  //! Returns the value of the Nth extremum square distance.
  Standard_EXPORT double SquareDistance(const int N = 1) const;

  //! Returns the points of the Nth extremum distance.
  //! P1 is on the first curve, P2 on the second one.
  Standard_EXPORT void Points(const int N,
                              Extrema_POnCurv2d&     P1,
                              Extrema_POnCurv2d&     P2) const;

  //! if the curve is a trimmed curve,
  //! dist11 is a square distance between the point on C1
  //! of parameter FirstParameter and the point of
  //! parameter FirstParameter on C2.
  Standard_EXPORT void TrimmedSquareDistances(double& dist11,
                                              double& distP12,
                                              double& distP21,
                                              double& distP22,
                                              gp_Pnt2d&      P11,
                                              gp_Pnt2d&      P12,
                                              gp_Pnt2d&      P21,
                                              gp_Pnt2d&      P22) const;

  //! Set flag for single extrema computation. Works on parametric solver only.
  Standard_EXPORT void SetSingleSolutionFlag(const bool theSingleSolutionFlag);

  //! Get flag for single extrema computation. Works on parametric solver only.
  Standard_EXPORT bool GetSingleSolutionFlag() const;

protected:
  Standard_EXPORT void Results(const Extrema_ExtElC2d& AlgExt,
                               const double     Ut11,
                               const double     Ut12,
                               const double     Ut21,
                               const double     Ut22,
                               const double     Period1 = 0.0,
                               const double     Period2 = 0.0);

  Standard_EXPORT void Results(const Extrema_ECC2d& AlgExt,
                               const double  Ut11,
                               const double  Ut12,
                               const double  Ut21,
                               const double  Ut22,
                               const double  Period1 = 0.0,
                               const double  Period2 = 0.0);

private:
  bool            myIsFindSingleSolution; // Default value is false.
  bool            myDone;
  bool            myIsPar;
  NCollection_Sequence<Extrema_POnCurv2d> mypoints;
  NCollection_Sequence<double>      mySqDist;
  int            mynbext;
  bool            inverse;
  const Adaptor2d_Curve2d*    myC;
  double               myv1;
  double               myv2;
  double               mytolc1;
  double               mytolc2;
  gp_Pnt2d                    P1f;
  gp_Pnt2d                    P1l;
  gp_Pnt2d                    P2f;
  gp_Pnt2d                    P2l;
  double               mydist11;
  double               mydist12;
  double               mydist21;
  double               mydist22;
};

#endif // _Extrema_ExtCC2d_HeaderFile
