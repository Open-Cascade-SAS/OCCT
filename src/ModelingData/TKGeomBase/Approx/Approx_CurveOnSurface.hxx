// Created on: 1997-09-30
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

#ifndef _Approx_CurveOnSurface_HeaderFile
#define _Approx_CurveOnSurface_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_Shape.hxx>

class Geom_BSplineCurve;
class Geom2d_BSplineCurve;

//! Approximation of curve on surface
class Approx_CurveOnSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! This constructor calls perform method. This constructor is deprecated.
  Standard_DEPRECATED(
    "This constructor is deprecated. Use other constructor and perform method instead.")
  Standard_EXPORT Approx_CurveOnSurface(const occ::handle<Adaptor2d_Curve2d>& C2D,
                                        const occ::handle<Adaptor3d_Surface>& Surf,
                                        const double                          First,
                                        const double                          Last,
                                        const double                          Tol,
                                        const GeomAbs_Shape                   Continuity,
                                        const int                             MaxDegree,
                                        const int                             MaxSegments,
                                        const bool                            Only3d = false,
                                        const bool                            Only2d = false);

  //! This constructor does not call perform method.
  //! @param theC2D   2D Curve to be approximated in 3D.
  //! @param theSurf  Surface where 2D curve is located.
  //! @param theFirst First parameter of resulting curve.
  //! @param theFirst Last parameter of resulting curve.
  //! @param theTol   Computation tolerance.
  Standard_EXPORT Approx_CurveOnSurface(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                                        const occ::handle<Adaptor3d_Surface>& theSurf,
                                        const double                          theFirst,
                                        const double                          theLast,
                                        const double                          theTol);

  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT bool HasResult() const;

  Standard_EXPORT occ::handle<Geom_BSplineCurve> Curve3d() const;

  Standard_EXPORT double MaxError3d() const;

  Standard_EXPORT occ::handle<Geom2d_BSplineCurve> Curve2d() const;

  Standard_EXPORT double MaxError2dU() const;

  //! returns the maximum errors relatively to the U component or the V component of the
  //! 2d Curve
  Standard_EXPORT double MaxError2dV() const;

  //! Constructs the 3d curve. Input parameters are ignored when the input curve is
  //! U-isoline or V-isoline.
  //! @param theMaxSegments Maximal number of segments in the resulting spline.
  //! @param theMaxDegree   Maximal degree of the result.
  //! @param theContinuity  Resulting continuity.
  //! @param theOnly3d      Determines building only 3D curve.
  //! @param theOnly2d      Determines building only 2D curve.
  Standard_EXPORT void Perform(const int           theMaxSegments,
                               const int           theMaxDegree,
                               const GeomAbs_Shape theContinuity,
                               const bool          theOnly3d = false,
                               const bool          theOnly2d = false);

protected:
  //! Checks whether the 2d curve is a isoline. It can be represented by b-spline, bezier,
  //! or geometric line. This line should have natural parameterization.
  //! @param theC2D       Trimmed curve to be checked.
  //! @param theIsU       Flag indicating that line is u const.
  //! @param theParam     Line parameter.
  //! @param theIsForward Flag indicating forward parameterization on a isoline.
  //! @return true when 2d curve is a line and false otherwise.
  bool isIsoLine(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                 bool&                                 theIsU,
                 double&                               theParam,
                 bool&                                 theIsForward) const;

  //! Builds 3D curve for a isoline. This method takes corresponding isoline from
  //! the input surface.
  //! @param theC2D   Trimmed curve to be approximated.
  //! @param theIsU   Flag indicating that line is u const.
  //! @param theParam Line parameter.
  //! @param theIsForward Flag indicating forward parameterization on a isoline.
  //! @return true when 3d curve is built and false otherwise.
  bool buildC3dOnIsoLine(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                         const bool                            theIsU,
                         const double                          theParam,
                         const bool                            theIsForward);

private:
  Approx_CurveOnSurface& operator=(const Approx_CurveOnSurface&) = delete;

private:
  //! Input curve.
  const occ::handle<Adaptor2d_Curve2d> myC2D;

  //! Input surface.
  const occ::handle<Adaptor3d_Surface> mySurf;

  //! First parameter of the result.
  const double myFirst;

  //! Last parameter of the result.
  const double myLast;

  //! Tolerance.
  double myTol;

  occ::handle<Geom2d_BSplineCurve> myCurve2d;
  occ::handle<Geom_BSplineCurve>   myCurve3d;
  bool                             myIsDone;
  bool                             myHasResult;
  double                           myError3d;
  double                           myError2dU;
  double                           myError2dV;
};

#endif // _Approx_CurveOnSurface_HeaderFile
