// Created on: 1997-09-22
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _ProjLib_CompProjectedCurve_HeaderFile
#define _ProjLib_CompProjectedCurve_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <ProjLib_Projector.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <GeomAbs_CurveType.hxx>

class gp_Pnt2d;
class gp_Vec2d;

class ProjLib_CompProjectedCurve : public Adaptor2d_Curve2d
{
  DEFINE_STANDARD_RTTIEXT(ProjLib_CompProjectedCurve, Adaptor2d_Curve2d)
public:
  Standard_EXPORT ProjLib_CompProjectedCurve();

  //! try to find all solutions
  Standard_EXPORT ProjLib_CompProjectedCurve(const occ::handle<Adaptor3d_Surface>& S,
                                             const occ::handle<Adaptor3d_Curve>&   C,
                                             const double              TolU,
                                             const double              TolV);

  //! this constructor tries to optimize the search using the
  //! assumption that maximum distance between surface and curve less or
  //! equal then MaxDist.
  //! if MaxDist < 0 then algorithm works as above.
  Standard_EXPORT ProjLib_CompProjectedCurve(const occ::handle<Adaptor3d_Surface>& S,
                                             const occ::handle<Adaptor3d_Curve>&   C,
                                             const double              TolU,
                                             const double              TolV,
                                             const double              MaxDist);

  //! this constructor tries to optimize the search using the
  //! assumption that maximum distance between surface and curve less or
  //! equal then MaxDist.
  //! if MaxDist < 0 then algorithm try to find all solutions
  //! Tolerances of parameters are calculated automatically.
  Standard_EXPORT ProjLib_CompProjectedCurve(const double              Tol3d,
                                             const occ::handle<Adaptor3d_Surface>& S,
                                             const occ::handle<Adaptor3d_Curve>&   C,
                                             const double              MaxDist = -1.0);

  //! Shallow copy of adaptor
  Standard_EXPORT virtual occ::handle<Adaptor2d_Curve2d> ShallowCopy() const override;

  //! computes a set of projected point and determine the
  //! continuous parts of the projected curves. The points
  //! corresponding to a projection on the bounds of the surface are
  //! included in this set of points.
  Standard_EXPORT void Init();

  //! Performs projecting for given curve.
  //! If projecting uses approximation,
  //! approximation parameters can be set before by corresponding methods
  //! SetTol3d(...), SeContinuity(...), SetMaxDegree(...), SetMaxSeg(...)
  Standard_EXPORT void Perform();

  //! Set the parameter, which defines 3d tolerance of approximation.
  Standard_EXPORT void SetTol3d(const double theTol3d);

  //! Set the parameter, which defines curve continuity.
  //! Default value is GeomAbs_C2;
  Standard_EXPORT void SetContinuity(const GeomAbs_Shape theContinuity);

  //! Set max possible degree of result BSpline curve2d, which is got by approximation.
  //! If MaxDegree < 0, algorithm uses values that are chosen depending of types curve 3d
  //! and surface.
  Standard_EXPORT void SetMaxDegree(const int theMaxDegree);

  //! Set the parameter, which defines maximal value of parametric intervals the projected
  //! curve can be cut for approximation. If MaxSeg < 0, algorithm uses default
  //! value = 16.
  Standard_EXPORT void SetMaxSeg(const int theMaxSeg);

  //! Set the parameter, which defines necessity of 2d results.
  Standard_EXPORT void SetProj2d(const bool theProj2d);

  //! Set the parameter, which defines necessity of 3d results.
  Standard_EXPORT void SetProj3d(const bool theProj3d);

  //! Changes the surface.
  Standard_EXPORT void Load(const occ::handle<Adaptor3d_Surface>& S);

  //! Changes the curve.
  Standard_EXPORT void Load(const occ::handle<Adaptor3d_Curve>& C);

  Standard_EXPORT const occ::handle<Adaptor3d_Surface>& GetSurface() const;

  Standard_EXPORT const occ::handle<Adaptor3d_Curve>& GetCurve() const;

  Standard_EXPORT void GetTolerance(double& TolU, double& TolV) const;

  //! returns the number of continuous part of the projected curve
  Standard_EXPORT int NbCurves() const;

  //! returns the bounds of the continuous part corresponding to Index
  Standard_EXPORT void Bounds(const int Index,
                              double&         Udeb,
                              double&         Ufin) const;

  //! returns True if part of projection with number Index is a single point and writes
  //! its coordinates in P
  Standard_EXPORT bool IsSinglePnt(const int Index, gp_Pnt2d& P) const;

  //! returns True if part of projection with number Index is an u-isoparametric curve of
  //! input surface
  Standard_EXPORT bool IsUIso(const int Index, double& U) const;

  //! returns True if part of projection with number Index is an v-isoparametric curve of
  //! input surface
  Standard_EXPORT bool IsVIso(const int Index, double& V) const;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT gp_Pnt2d Value(const double U) const override;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT void D0(const double U, gp_Pnt2d& P) const override;

  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1(const double U, gp_Pnt2d& P, gp_Vec2d& V) const override;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2(const double U,
                          gp_Pnt2d&           P,
                          gp_Vec2d&           V1,
                          gp_Vec2d&           V2) const override;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if N < 1.
  //! Raised if N > 2.
  Standard_EXPORT gp_Vec2d DN(const double    U,
                              const int N) const override;

  //! Returns the first parameter of the curve C
  //! which has a projection on S.
  Standard_EXPORT double FirstParameter() const override;

  //! Returns the last parameter of the curve C
  //! which has a projection on S.
  Standard_EXPORT double LastParameter() const override;

  //! Returns the Continuity used in the approximation.
  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! Returns the number of intervals which define
  //! an S continuous part of the projected curve
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Returns a curve equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 2d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT occ::handle<Adaptor2d_Curve2d> Trim(const double FirstParam,
                                                 const double LastParam,
                                                 const double Tol) const override;

  //! Returns the parameters corresponding to
  //! S discontinuities.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape   S) const override;

  //! returns the maximum distance between
  //! curve to project and surface
  Standard_EXPORT double MaxDistance(const int Index) const;

  Standard_EXPORT const occ::handle<NCollection_HSequence<occ::handle<NCollection_HSequence<gp_Pnt>>>>& GetSequence() const;

  //! Returns the type of the curve in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  Standard_EXPORT GeomAbs_CurveType GetType() const override;

  //! Returns true if result of projecting of the curve interval
  //! with number Index is point.
  Standard_EXPORT bool ResultIsPoint(const int theIndex) const;

  //! Returns the error of approximation of U parameter 2d-curve as a result
  //! projecting of the curve interval with number Index.
  Standard_EXPORT double GetResult2dUApproxError(const int theIndex) const;

  //! Returns the error of approximation of V parameter 2d-curve as a result
  //! projecting of the curve interval with number Index.
  Standard_EXPORT double GetResult2dVApproxError(const int theIndex) const;

  //! Returns the error of approximation of 3d-curve as a result
  //! projecting of the curve interval with number Index.
  Standard_EXPORT double GetResult3dApproxError(const int theIndex) const;

  //! Returns the resulting 2d-curve of projecting
  //! of the curve interval with number Index.
  Standard_EXPORT occ::handle<Geom2d_Curve> GetResult2dC(const int theIndex) const;

  //! Returns the resulting 3d-curve of projecting
  //! of the curve interval with number Index.
  Standard_EXPORT occ::handle<Geom_Curve> GetResult3dC(const int theIndex) const;

  //! Returns the resulting 2d-point of projecting
  //! of the curve interval with number Index.
  Standard_EXPORT gp_Pnt2d GetResult2dP(const int theIndex) const;

  //! Returns the resulting 3d-point of projecting
  //! of the curve interval with number Index.
  Standard_EXPORT gp_Pnt GetResult3dP(const int theIndex) const;

  //! Returns the parameter, which defines necessity of only 2d results.
  bool GetProj2d() const { return myProj2d; }

  //! Returns the parameter, which defines necessity of only 3d results.
  bool GetProj3d() const { return myProj3d; }

private:
  //! This method performs check possibility of optimization traps and tries to go out from them.
  //@return thePoint - input / corrected point.
  Standard_EXPORT void UpdateTripleByTrapCriteria(gp_Pnt& thePoint) const;

  Standard_EXPORT void BuildIntervals(const GeomAbs_Shape S) const;

private:
  occ::handle<Adaptor3d_Surface>                 mySurface;
  occ::handle<Adaptor3d_Curve>                   myCurve;
  int                          myNbCurves;
  occ::handle<NCollection_HSequence<occ::handle<NCollection_HSequence<gp_Pnt>>>> mySequence;
  occ::handle<NCollection_HArray1<bool>>          myUIso;
  occ::handle<NCollection_HArray1<bool>>          myVIso;
  occ::handle<NCollection_HArray1<bool>>          mySnglPnts;
  occ::handle<NCollection_HArray1<double>>             myMaxDistance;
  occ::handle<NCollection_HArray1<double>>             myTabInt;
  double                             myTol3d;
  GeomAbs_Shape                             myContinuity;
  int                          myMaxDegree;
  int                          myMaxSeg;
  bool                          myProj2d;
  bool                          myProj3d;
  double                             myMaxDist;
  double                             myTolU;
  double                             myTolV;

  occ::handle<NCollection_HArray1<bool>>  myResultIsPoint;
  occ::handle<NCollection_HArray1<double>>     myResult2dUApproxError;
  occ::handle<NCollection_HArray1<double>>     myResult2dVApproxError;
  occ::handle<NCollection_HArray1<double>>     myResult3dApproxError;
  occ::handle<NCollection_HArray1<gp_Pnt>>       myResult3dPoint;
  occ::handle<NCollection_HArray1<gp_Pnt2d>>     myResult2dPoint;
  occ::handle<NCollection_HArray1<occ::handle<Geom_Curve>>>   myResult3dCurve;
  occ::handle<NCollection_HArray1<occ::handle<Geom2d_Curve>>> myResult2dCurve;
};

#endif // _ProjLib_CompProjectedCurve_HeaderFile
