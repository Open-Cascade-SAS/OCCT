// Created on: 1994-10-07
// Created by: Bruno DUMORTIER
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

#ifndef _ProjLib_ComputeApproxOnPolarSurface_HeaderFile
#define _ProjLib_ComputeApproxOnPolarSurface_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <AppParCurves_Constraint.hxx>

class Geom2d_BSplineCurve;
class Geom2d_Curve;

//! Approximate the projection of a 3d curve on an
//! polar surface and stores the result in Approx.
//! The result is a 2d curve. The evaluation of the
//! current point of the 2d curve is done with the
//! evaluation of the extrema P3d - Surface.
//! For approximation some parameters are used, including
//! required tolerance of approximation.
//! Tolerance is maximal possible value of 3d deviation of 3d projection of projected curve from
//! "exact" 3d projection. Since algorithm searches 2d curve on surface, required 2d tolerance is
//! computed from 3d tolerance with help of U,V resolutions of surface. 3d and 2d tolerances have
//! sense only for curves on surface, it defines precision of projecting and approximation and have
//! nothing to do with distance between the projected curve and the surface.
class ProjLib_ComputeApproxOnPolarSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor, it only sets some initial values for class fields.
  Standard_EXPORT ProjLib_ComputeApproxOnPolarSurface();

  //! Constructor, which performs projecting.
  Standard_EXPORT ProjLib_ComputeApproxOnPolarSurface(const occ::handle<Adaptor3d_Curve>&   C,
                                                      const occ::handle<Adaptor3d_Surface>& S,
                                                      const double Tol = 1.0e-4);

  //! Constructor, which performs projecting, using initial curve 2d InitCurve2d, which is any rough
  //! approximation of result curve. Parameter Tol is 3d tolerance of approximation.
  Standard_EXPORT ProjLib_ComputeApproxOnPolarSurface(const occ::handle<Adaptor2d_Curve2d>& InitCurve2d,
                                                      const occ::handle<Adaptor3d_Curve>&   C,
                                                      const occ::handle<Adaptor3d_Surface>& S,
                                                      const double              Tol);

  //! Constructor, which performs projecting, using two initial curves 2d: InitCurve2d and
  //! InitCurve2dBis that are any rough approximations of result curves. This constructor is used to
  //! get two pcurves for seem edge. Parameter Tol is 3d tolerance of approximation.
  Standard_EXPORT ProjLib_ComputeApproxOnPolarSurface(
    const occ::handle<Adaptor2d_Curve2d>& InitCurve2d,
    const occ::handle<Adaptor2d_Curve2d>& InitCurve2dBis,
    const occ::handle<Adaptor3d_Curve>&   C,
    const occ::handle<Adaptor3d_Surface>& S,
    const double              Tol);

  //! Set min and max possible degree of result BSpline curve2d, which is got by approximation.
  //! If theDegMin/Max < 0, algorithm uses values min = 2, max = 8.
  Standard_EXPORT void SetDegree(const int theDegMin,
                                 const int theDegMax);

  //! Set the parameter, which defines maximal value of parametric intervals the projected
  //! curve can be cut for approximation. If theMaxSegments < 0, algorithm uses default
  //! value = 1000.
  Standard_EXPORT void SetMaxSegments(const int theMaxSegments);

  //! Set the parameter, which defines type of boundary condition between segments during
  //! approximation. It can be AppParCurves_PassPoint or AppParCurves_TangencyPoint. Default value
  //! is AppParCurves_TangencyPoint.
  Standard_EXPORT void SetBndPnt(const AppParCurves_Constraint theBndPnt);

  //! Set the parameter, which defines maximal possible distance between projected curve and
  //! surface. It is used only for projecting on not analytical surfaces. If theMaxDist < 0,
  //! algorithm uses default value 100.*Tolerance. If real distance between curve and surface more
  //! then theMaxDist, algorithm stops working.
  Standard_EXPORT void SetMaxDist(const double theMaxDist);

  //! Set the tolerance used to project
  //! the curve on the surface.
  //! Default value is Precision::Approximation().
  Standard_EXPORT void SetTolerance(const double theTolerance);

  //! Method, which performs projecting, using default values of parameters or
  //! they must be set by corresponding methods before using.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Curve>&   C,
                               const occ::handle<Adaptor3d_Surface>& S);

  //! Method, which performs projecting, using default values of parameters or
  //! they must be set by corresponding methods before using.
  //! Parameter InitCurve2d is any rough estimation of 2d result curve.
  Standard_EXPORT occ::handle<Geom2d_BSplineCurve> Perform(const occ::handle<Adaptor2d_Curve2d>& InitCurve2d,
                                                      const occ::handle<Adaptor3d_Curve>&   C,
                                                      const occ::handle<Adaptor3d_Surface>& S);

  //! Builds initial 2d curve as BSpline with degree = 1 using Extrema algorithm.
  //! Method is used in method Perform(...).
  Standard_EXPORT occ::handle<Adaptor2d_Curve2d> BuildInitialCurve2d(
    const occ::handle<Adaptor3d_Curve>&   Curve,
    const occ::handle<Adaptor3d_Surface>& S);

  //! Method, which performs projecting.
  //! Method is used in method Perform(...).
  Standard_EXPORT occ::handle<Geom2d_BSplineCurve> ProjectUsingInitialCurve2d(
    const occ::handle<Adaptor3d_Curve>&   Curve,
    const occ::handle<Adaptor3d_Surface>& S,
    const occ::handle<Adaptor2d_Curve2d>& InitCurve2d);

  //! Returns result curve 2d.
  Standard_EXPORT occ::handle<Geom2d_BSplineCurve> BSpline() const;
  //! Returns second 2d curve.
  Standard_EXPORT occ::handle<Geom2d_Curve> Curve2d() const;

  Standard_EXPORT bool IsDone() const;

  //! returns the reached Tolerance.
  Standard_EXPORT double Tolerance() const;

private:
  bool            myProjIsDone;
  double               myTolerance;
  occ::handle<Geom2d_BSplineCurve> myBSpline;
  occ::handle<Geom2d_Curve>        my2ndCurve;
  double               myTolReached;
  int            myDegMin;
  int            myDegMax;
  int            myMaxSegments;
  double               myMaxDist;
  AppParCurves_Constraint     myBndPnt;
  double               myDist;
};

#endif // _ProjLib_ComputeApproxOnPolarSurface_HeaderFile
