// Created on: 1993-09-07
// Created by: Bruno DUMORTIER
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

#ifndef _ProjLib_ComputeApprox_HeaderFile
#define _ProjLib_ComputeApprox_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <AppParCurves_Constraint.hxx>

class Geom2d_BSplineCurve;
class Geom2d_BezierCurve;

//! Approximate the  projection of  a 3d curve   on an
//! analytic surface and stores the result in Approx.
//! The result is a 2d curve.
//! For approximation some parameters are used, including
//! required tolerance of approximation.
//! Tolerance is maximal possible value of 3d deviation of 3d projection of projected curve from
//! "exact" 3d projection. Since algorithm searches 2d curve on surface, required 2d tolerance is
//! computed from 3d tolerance with help of U,V resolutions of surface. 3d and 2d tolerances have
//! sense only for curves on surface, it defines precision of projecting and approximation and have
//! nothing to do with distance between the projected curve and the surface.
class ProjLib_ComputeApprox
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor, it only sets some initial values for class fields.
  Standard_EXPORT ProjLib_ComputeApprox();

  //! <Tol>    is   the   tolerance   with  which    the
  //! approximation is performed.
  //! Other parameters for approximation have default values.
  Standard_EXPORT ProjLib_ComputeApprox(const Handle(Adaptor3d_Curve)&   C,
                                        const Handle(Adaptor3d_Surface)& S,
                                        const Standard_Real              Tol);

  //! Performs projecting.
  //! In case of approximation current values of parameters are used:
  //! default values or set by corresponding methods Set...
  Standard_EXPORT void Perform(const Handle(Adaptor3d_Curve)&   C,
                               const Handle(Adaptor3d_Surface)& S);

  //! Set tolerance of approximation.
  //! Default value is Precision::Confusion().
  Standard_EXPORT void SetTolerance(const Standard_Real theTolerance);

  //! Set min and max possible degree of result BSpline curve2d, which is got by approximation.
  //! If theDegMin/Max < 0, algorithm uses values that are chosen depending of types curve 3d
  //! and surface.
  Standard_EXPORT void SetDegree(const Standard_Integer theDegMin,
                                 const Standard_Integer theDegMax);

  //! Set the parameter, which defines maximal value of parametric intervals the projected
  //! curve can be cut for approximation. If theMaxSegments < 0, algorithm uses default
  //! value = 1000.
  Standard_EXPORT void SetMaxSegments(const Standard_Integer theMaxSegments);

  //! Set the parameter, which defines type of boundary condition between segments during
  //! approximation. It can be AppParCurves_PassPoint or AppParCurves_TangencyPoint. Default value
  //! is AppParCurves_TangencyPoint;
  Standard_EXPORT void SetBndPnt(const AppParCurves_Constraint theBndPnt);

  Standard_EXPORT Handle(Geom2d_BSplineCurve) BSpline() const;

  Standard_EXPORT Handle(Geom2d_BezierCurve) Bezier() const;

  //! returns the reached Tolerance.
  Standard_EXPORT Standard_Real Tolerance() const;

protected:
private:
  Standard_Real               myTolerance;
  Handle(Geom2d_BSplineCurve) myBSpline;
  Handle(Geom2d_BezierCurve)  myBezier;
  Standard_Integer            myDegMin;
  Standard_Integer            myDegMax;
  Standard_Integer            myMaxSegments;
  AppParCurves_Constraint     myBndPnt;
};

#endif // _ProjLib_ComputeApprox_HeaderFile
