// Created on: 1993-07-07
// Created by: Jean Claude VAUTHIER
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

#ifndef _GeomLib_HeaderFile
#define _GeomLib_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>

class Geom_Curve;
class gp_Ax2;
class Geom2d_Curve;
class gp_GTrsf2d;
class Adaptor3d_CurveOnSurface;
class Geom_BoundedCurve;
class gp_Pnt;
class gp_Vec;
class Geom_BoundedSurface;
class gp_Dir;
class Adaptor3d_Curve;
class Geom_BSplineSurface;
class Geom_BezierSurface;
class Geom_Surface;

typedef class Adaptor2d_Curve2d Adaptor2d_Curve2d;

//! Geom Library. This package provides an
//! implementation of functions for basic computation
//! on geometric entity from packages Geom and Geom2d.
class GeomLib
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the curve 3d from package Geom
  //! corresponding to curve 2d from package Geom2d, on
  //! the plan defined with the local coordinate system
  //! Position.
  Standard_EXPORT static occ::handle<Geom_Curve> To3d(const gp_Ax2&               Position,
                                                 const occ::handle<Geom2d_Curve>& Curve2d);

  //! Computes the curve 3d from package Geom
  //! corresponding to the curve 3d from package Geom,
  //! transformed with the transformation <GTrsf>
  //! WARNING : this method may return a null Handle if
  //! it's impossible to compute the transformation of
  //! a curve. It's not implemented when :
  //! 1) the curve is an infinite parabola or hyperbola
  //! 2) the curve is an offsetcurve
  Standard_EXPORT static occ::handle<Geom2d_Curve> GTransform(const occ::handle<Geom2d_Curve>& Curve,
                                                         const gp_GTrsf2d&           GTrsf);

  //! Make the curve Curve2dPtr have the imposed
  //! range First to List the most economic way,
  //! that is if it can change the range without
  //! changing the nature of the curve it will try
  //! to do that. Otherwise it will produce a Bspline
  //! curve that has the required range
  Standard_EXPORT static void SameRange(const double         Tolerance,
                                        const occ::handle<Geom2d_Curve>& Curve2dPtr,
                                        const double         First,
                                        const double         Last,
                                        const double         RequestedFirst,
                                        const double         RequestedLast,
                                        occ::handle<Geom2d_Curve>&       NewCurve2dPtr);

  Standard_EXPORT static void BuildCurve3d(const double       Tolerance,
                                           Adaptor3d_CurveOnSurface& CurvePtr,
                                           const double       FirstParameter,
                                           const double       LastParameter,
                                           occ::handle<Geom_Curve>&       NewCurvePtr,
                                           double&            MaxDeviation,
                                           double&            AverageDeviation,
                                           const GeomAbs_Shape       Continuity = GeomAbs_C1,
                                           const int    MaxDegree  = 14,
                                           const int    MaxSegment = 30);

  Standard_EXPORT static void AdjustExtremity(occ::handle<Geom_BoundedCurve>& Curve,
                                              const gp_Pnt&              P1,
                                              const gp_Pnt&              P2,
                                              const gp_Vec&              T1,
                                              const gp_Vec&              T2);

  //! Extends the bounded curve Curve to the point Point.
  //! The extension is built:
  //! -      at the end of the curve if After equals true, or
  //! -      at the beginning of the curve if After equals false.
  //! The extension is performed according to a degree of
  //! continuity equal to Cont, which in its turn must be equal to 1, 2 or 3.
  //! This function converts the bounded curve Curve into a BSpline curve.
  //! Warning
  //! -   Nothing is done, and Curve is not modified if Cont is
  //! not equal to 1, 2 or 3.
  //! -   It is recommended that the extension should not be
  //! too large with respect to the size of the bounded
  //! curve Curve: Point must not be located too far from
  //! one of the extremities of Curve.
  Standard_EXPORT static void ExtendCurveToPoint(occ::handle<Geom_BoundedCurve>& Curve,
                                                 const gp_Pnt&              Point,
                                                 const int     Cont,
                                                 const bool     After);

  //! Extends the bounded surface Surf along one of its
  //! boundaries. The chord length of the extension is equal to Length.
  //! The direction of the extension is given as:
  //! -   the u parametric direction of Surf, if InU equals true, or
  //! -   the v parametric direction of Surf, if InU equals false.
  //! In this parametric direction, the extension is built on the side of:
  //! -   the last parameter of Surf, if After equals true, or
  //! -   the first parameter of Surf, if After equals false.
  //! The extension is performed according to a degree of
  //! continuity equal to Cont, which in its turn must be equal to 1, 2 or 3.
  //! This function converts the bounded surface Surf into a BSpline surface.
  //! Warning
  //! -   Nothing is done, and Surf is not modified if Cont is
  //! not equal to 1, 2 or 3.
  //! -   It is recommended that Length, the size of the
  //! extension should not be too large with respect to the
  //! size of the bounded surface Surf.
  //! -   Surf must not be a periodic BSpline surface in the
  //! parametric direction corresponding to the direction of extension.
  Standard_EXPORT static void ExtendSurfByLength(occ::handle<Geom_BoundedSurface>& Surf,
                                                 const double          Length,
                                                 const int       Cont,
                                                 const bool       InU,
                                                 const bool       After);

  //! Compute axes of inertia, of some points
  //! <Axe>.Location() is the BaryCentre
  //! <Axe>.XDirection is the axe of upper inertia
  //! <Axe>.Direction is the Normal to the average plane
  //! IsSingular is True if points are on line
  //! Tol is used to determine singular cases.
  Standard_EXPORT static void AxeOfInertia(const NCollection_Array1<gp_Pnt>& Points,
                                           gp_Ax2&                   Axe,
                                           bool&         IsSingular,
                                           const double       Tol = 1.0e-7);

  //! Compute principale axes of inertia, and dispersion
  //! value of some points.
  Standard_EXPORT static void Inertia(const NCollection_Array1<gp_Pnt>& Points,
                                      gp_Pnt&                   Bary,
                                      gp_Dir&                   XDir,
                                      gp_Dir&                   YDir,
                                      double&            Xgap,
                                      double&            YGap,
                                      double&            ZGap);

  //! Warning! This assume that the InParameter is an increasing sequence
  //! of real number and it will not check for that : Unpredictable
  //! result can happen if this is not satisfied. It is the caller
  //! responsibility to check for that property.
  //!
  //! This method makes uniform NumPoints segments S1,...SNumPoints out
  //! of the segment defined by the first parameter and the
  //! last parameter of the InParameter ; keeps only one
  //! point of the InParameters set of parameter in each of
  //! the uniform segments taking care of the first and the
  //! last parameters. For the ith segment the element of
  //! the InParameter is the one that is the first to exceed
  //! the midpoint of the segment and to fall before the
  //! midpoint of the next segment
  //! There will be at the end at most NumPoints + 1
  //! if NumPoints > 2 in the OutParameters Array
  Standard_EXPORT static void RemovePointsFromArray(const int         NumPoints,
                                                    const NCollection_Array1<double>&    InParameters,
                                                    occ::handle<NCollection_HArray1<double>>& OutParameters);

  //! this makes sure that there is at least MinNumPoints
  //! in OutParameters taking into account the parameters in
  //! the InParameters array provided those are in order,
  //! that is the sequence of real in the InParameter is strictly
  //! non decreasing
  Standard_EXPORT static void DensifyArray1OfReal(const int         MinNumPoints,
                                                  const NCollection_Array1<double>&    InParameters,
                                                  occ::handle<NCollection_HArray1<double>>& OutParameters);

  //! This method fuse intervals Interval1 and Interval2 with specified Confusion
  //! @param[in] Interval1  first interval to fuse
  //! @param[in] Interval2  second interval to fuse
  //! @param[in] Confision  tolerance to compare intervals
  //! @param[in] IsAdjustToFirstInterval  flag to set method of fusion, if intervals are close
  //!                               if false, intervals are fusing by half-division method
  //!                               if true, intervals are fusing by selecting value from Interval1
  //! @param[out] Fusion  output interval
  Standard_EXPORT static void FuseIntervals(
    const NCollection_Array1<double>& Interval1,
    const NCollection_Array1<double>& Interval2,
    NCollection_Sequence<double>&     Fusion,
    const double         Confusion               = 1.0e-9,
    const bool      IsAdjustToFirstInterval = false);

  //! this will compute the maximum distance at the
  //! parameters given in the Parameters array by
  //! evaluating each parameter the two curves and taking
  //! the maximum of the evaluated distance
  Standard_EXPORT static void EvalMaxParametricDistance(const Adaptor3d_Curve&      Curve,
                                                        const Adaptor3d_Curve&      AReferenceCurve,
                                                        const double         Tolerance,
                                                        const NCollection_Array1<double>& Parameters,
                                                        double&              MaxDistance);

  //! this will compute the maximum distance at the parameters
  //! given in the Parameters array by projecting from the Curve
  //! to the reference curve and taking the minimum distance
  //! Than the maximum will be taken on those minimas.
  Standard_EXPORT static void EvalMaxDistanceAlongParameter(const Adaptor3d_Curve& Curve,
                                                            const Adaptor3d_Curve& AReferenceCurve,
                                                            const double    Tolerance,
                                                            const NCollection_Array1<double>& Parameters,
                                                            double& MaxDistance);

  //! Cancel,on the boundaries,the denominator first derivative
  //! in the directions wished by the user and set its value to 1.
  Standard_EXPORT static void CancelDenominatorDerivative(occ::handle<Geom_BSplineSurface>& BSurf,
                                                          const bool       UDirection,
                                                          const bool       VDirection);

  //! Estimate surface normal at the given (U, V) point.
  //! @param[in]  theSurf input surface
  //! @param[in]  theUV   (U, V) point coordinates on the surface
  //! @param[in]  theTol  estimation tolerance
  //! @param[out] theNorm computed normal
  //! @return 0 if normal estimated from D1,
  //!         1 if estimated from D2 (quasysingular),
  //!       >=2 in case of failure (undefined or infinite solutions)
  Standard_EXPORT static int NormEstim(const occ::handle<Geom_Surface>& theSurf,
                                                    const gp_Pnt2d&             theUV,
                                                    const double         theTol,
                                                    gp_Dir&                     theNorm);

  //! This method defines if opposite boundaries of surface
  //! coincide with given tolerance
  Standard_EXPORT static void IsClosed(const occ::handle<Geom_Surface>& S,
                                       const double         Tol,
                                       bool&           isUClosed,
                                       bool&           isVClosed);

  //! Returns true if the poles of U1 isoline and the poles of
  //! U2 isoline of surface are identical according to tolerance criterion.
  //! For rational surfaces Weights(i)*Poles(i) are checked.
  Standard_EXPORT static bool IsBSplUClosed(const occ::handle<Geom_BSplineSurface>& S,
                                                        const double                U1,
                                                        const double                U2,
                                                        const double                Tol);

  //! Returns true if the poles of V1 isoline and the poles of
  //! V2 isoline of surface are identical according to tolerance criterion.
  //! For rational surfaces Weights(i)*Poles(i) are checked.
  Standard_EXPORT static bool IsBSplVClosed(const occ::handle<Geom_BSplineSurface>& S,
                                                        const double                V1,
                                                        const double                V2,
                                                        const double                Tol);

  //! Returns true if the poles of U1 isoline and the poles of
  //! U2 isoline of surface are identical according to tolerance criterion.
  Standard_EXPORT static bool IsBzUClosed(const occ::handle<Geom_BezierSurface>& S,
                                                      const double               U1,
                                                      const double               U2,
                                                      const double               Tol);

  //! Returns true if the poles of V1 isoline and the poles of
  //! V2 isoline of surface are identical according to tolerance criterion.
  Standard_EXPORT static bool IsBzVClosed(const occ::handle<Geom_BezierSurface>& S,
                                                      const double               V1,
                                                      const double               V2,
                                                      const double               Tol);

  //! Checks whether the 2d curve is a isoline. It can be represented by b-spline, bezier,
  //! or geometric line. This line should have natural parameterization.
  //! @param theC2D       Trimmed curve to be checked.
  //! @param theIsU       Flag indicating that line is u const.
  //! @param theParam     Line parameter.
  //! @param theIsForward Flag indicating forward parameterization on a isoline.
  //! @return true when 2d curve is a line and false otherwise.
  Standard_EXPORT static bool isIsoLine(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                                                    bool&                theIsU,
                                                    double&                   theParam,
                                                    bool&                theIsForward);

  //! Builds 3D curve for a isoline. This method takes corresponding isoline from
  //! the input surface.
  //! @param theC2D   Trimmed curve to be approximated.
  //! @param theIsU   Flag indicating that line is u const.
  //! @param theParam Line parameter.
  //! @param theIsForward Flag indicating forward parameterization on a isoline.
  //! @return true when 3d curve is built and false otherwise.
  Standard_EXPORT static occ::handle<Geom_Curve> buildC3dOnIsoLine(
    const occ::handle<Adaptor2d_Curve2d>& theC2D,
    const occ::handle<Adaptor3d_Surface>& theSurf,
    const double              theFirst,
    const double              theLast,
    const double              theTolerance,
    const bool           theIsU,
    const double              theParam,
    const bool           theIsForward);
};

#endif // _GeomLib_HeaderFile
