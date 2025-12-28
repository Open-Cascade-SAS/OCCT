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

#ifndef _Extrema_ExtCC_HeaderFile
#define _Extrema_ExtCC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Extrema_ECC.hxx>
#include <Extrema_POnCurv.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt.hxx>

class Adaptor3d_Curve;
class Extrema_POnCurv;
class Extrema_ExtElC;

//! It calculates all the distance between two curves.
//! These distances can be maximum or minimum.
class Extrema_ExtCC
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Extrema_ExtCC(const double TolC1 = 1.0e-10, const double TolC2 = 1.0e-10);

  //! It calculates all the distances.
  Standard_EXPORT Extrema_ExtCC(const Adaptor3d_Curve& C1,
                                const Adaptor3d_Curve& C2,
                                const double           TolC1 = 1.0e-10,
                                const double           TolC2 = 1.0e-10);

  //! It calculates all the distances.
  Standard_EXPORT Extrema_ExtCC(const Adaptor3d_Curve& C1,
                                const Adaptor3d_Curve& C2,
                                const double           U1,
                                const double           U2,
                                const double           V1,
                                const double           V2,
                                const double           TolC1 = 1.0e-10,
                                const double           TolC2 = 1.0e-10);

  //! Initializes but does not perform algorithm.
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& C1,
                                  const Adaptor3d_Curve& C2,
                                  const double           TolC1 = 1.0e-10,
                                  const double           TolC2 = 1.0e-10);

  //! Initializes but does not perform algorithm.
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& C1,
                                  const Adaptor3d_Curve& C2,
                                  const double           U1,
                                  const double           U2,
                                  const double           V1,
                                  const double           V2,
                                  const double           TolC1 = 1.0e-10,
                                  const double           TolC2 = 1.0e-10);

  Standard_EXPORT void SetCurve(const int theRank, const Adaptor3d_Curve& C);

  Standard_EXPORT void SetCurve(const int              theRank,
                                const Adaptor3d_Curve& C,
                                const double           Uinf,
                                const double           Usup);

  Standard_EXPORT void SetRange(const int theRank, const double Uinf, const double Usup);

  Standard_EXPORT void SetTolerance(const int theRank, const double Tol);

  Standard_EXPORT void Perform();

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
  Standard_EXPORT void Points(const int N, Extrema_POnCurv& P1, Extrema_POnCurv& P2) const;

  //! if the curve is a trimmed curve,
  //! dist11 is a square distance between the point on C1
  //! of parameter FirstParameter and the point of
  //! parameter FirstParameter on C2.
  Standard_EXPORT void TrimmedSquareDistances(double& dist11,
                                              double& distP12,
                                              double& distP21,
                                              double& distP22,
                                              gp_Pnt& P11,
                                              gp_Pnt& P12,
                                              gp_Pnt& P21,
                                              gp_Pnt& P22) const;

  //! Set flag for single extrema computation. Works on parametric solver only.
  Standard_EXPORT void SetSingleSolutionFlag(const bool theSingleSolutionFlag);

  //! Get flag for single extrema computation. Works on parametric solver only.
  Standard_EXPORT bool GetSingleSolutionFlag() const;

protected:
  //! Prepares the extrema result(s) for analytical cases (line, circle, ellipsis etc.)
  Standard_EXPORT void PrepareResults(const Extrema_ExtElC& AlgExt,
                                      const bool            theIsInverse,
                                      const double          Ut11,
                                      const double          Ut12,
                                      const double          Ut21,
                                      const double          Ut22);

  //! Prepares the extrema result(s) for general cases (e.g. with B-spline curves).
  Standard_EXPORT void PrepareResults(const Extrema_ECC& AlgExt,
                                      const double       Ut11,
                                      const double       Ut12,
                                      const double       Ut21,
                                      const double       Ut22);

  //! Prepares the extrema result(s) in case when the given curves are parallel.
  Standard_EXPORT void PrepareParallelResult(const double theUt11,
                                             const double theUt12,
                                             const double theUt21,
                                             const double theUt22,
                                             const double theSqDist);

  // Clears all found extremas.
  // This method does not change any flags (e.g. Done or IsParallel)
  void ClearSolutions()
  {
    mySqDist.Clear();
    mypoints.Clear();
  }

private:
  // disallow copies
  Extrema_ExtCC(Extrema_ExtCC&)            = delete;
  Extrema_ExtCC& operator=(Extrema_ExtCC&) = delete;

private:
  bool                                  myIsFindSingleSolution; // Default value is false.
  Extrema_ECC                           myECC;
  bool                                  myDone;
  bool                                  myIsParallel;
  NCollection_Sequence<Extrema_POnCurv> mypoints;
  NCollection_Sequence<double>          mySqDist;
  const Adaptor3d_Curve*                myC[2];
  double                                myInf[2];
  double                                mySup[2];
  double                                myTol[2];
  gp_Pnt                                myP1f;
  gp_Pnt                                myP1l;
  gp_Pnt                                myP2f;
  gp_Pnt                                myP2l;
  double                                mydist11;
  double                                mydist12;
  double                                mydist21;
  double                                mydist22;
};

#endif // _Extrema_ExtCC_HeaderFile
