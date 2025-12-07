// Created on: 1998-07-14
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeConstruct_ProjectCurveOnSurface_HeaderFile
#define _ShapeConstruct_ProjectCurveOnSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <ShapeExtend_Status.hxx>
#include <Standard_Transient.hxx>

#include <utility>

class Geom2d_Curve;
class Geom_Curve;
class Geom_Surface;
class ShapeAnalysis_Surface;

// Resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeConstruct_ProjectCurveOnSurface;
DEFINE_STANDARD_HANDLE(ShapeConstruct_ProjectCurveOnSurface, Standard_Transient)

//! This tool provides a method for computing pcurve by projecting
//! 3d curve onto a surface.
//! Projection is done by 23 or more points (this number is changed
//! for B-Splines according to the following rule:
//! the total number of the points is not less than number of spans *
//! (degree + 1);
//! it is increased recursively starting with 23 and is added with 22
//! until the condition is fulfilled).
//! Isoparametric cases (if curve corresponds to U=const or V=const on
//! the surface) are recognized with the given precision.
class ShapeConstruct_ProjectCurveOnSurface : public Standard_Transient
{
public:
  // Type aliases for internal containers (1-based indexing for geometry arrays)
  using ArrayOfPnt   = NCollection_Array1<gp_Pnt>;
  using ArrayOfPnt2d = NCollection_Array1<gp_Pnt2d>;
  using ArrayOfReal  = NCollection_Array1<Standard_Real>;
  // Cache uses 0-based indexing (simple 2-element array)
  using CachePoint = std::pair<gp_Pnt, gp_Pnt2d>;
  using CacheArray = NCollection_Array1<CachePoint>;

public:
  //! Empty constructor.
  Standard_EXPORT ShapeConstruct_ProjectCurveOnSurface();

  //! Initializes the object with all necessary parameters,
  //! i.e. surface and precision
  //! @param[in] theSurf the surface to project on
  //! @param[in] thePreci the precision for projection
  Standard_EXPORT virtual void Init(const Handle(Geom_Surface)& theSurf,
                                    const Standard_Real         thePreci);

  //! Initializes the object with all necessary parameters,
  //! i.e. surface and precision
  //! @param[in] theSurf the surface to project on (ShapeAnalysis_Surface)
  //! @param[in] thePreci the precision for projection
  Standard_EXPORT virtual void Init(const Handle(ShapeAnalysis_Surface)& theSurf,
                                    const Standard_Real                  thePreci);

  //! Loads a surface (in the form of Geom_Surface) to project on
  //! @param[in] theSurf the surface to project on
  Standard_EXPORT void SetSurface(const Handle(Geom_Surface)& theSurf);

  //! Loads a surface (in the form of ShapeAnalysis_Surface) to project on
  //! @param[in] theSurf the surface to project on
  Standard_EXPORT void SetSurface(const Handle(ShapeAnalysis_Surface)& theSurf);

  //! Sets value for current precision
  //! @param[in] thePreci the precision value
  Standard_EXPORT void SetPrecision(const Standard_Real thePreci);

  //! Returns (modifiable) the flag specifying to which side of
  //! parametrical space adjust part of pcurve which lies on seam.
  //! This is required in very rare case when 3d curve which is
  //! to be projected goes partly along the seam on the closed
  //! surface with singularity (e.g. sphere), goes through the
  //! degenerated point and partly lies on internal area of surface.
  //!
  //! If this flag is True, the seam part of such curve will be
  //! adjusted to the left side of parametric space (on sphere U=0),
  //! else to the right side (on sphere U=2*PI)
  //! Default value is True
  //! @return modifiable reference to the adjustment flag
  Standard_EXPORT Standard_Integer& AdjustOverDegenMode();

  //! Returns the status of last Perform
  //! @param[in] theStatus the status to query
  //! @return true if the specified status is set
  Standard_EXPORT Standard_Boolean Status(const ShapeExtend_Status theStatus) const;

  //! Computes the projection of 3d curve onto a surface using the
  //! specialized algorithm. Returns False if projector fails,
  //! otherwise, if pcurve computed successfully, returns True.
  //! The output curve 2D is guaranteed to be same-parameter
  //! with input curve 3D on the interval [theFirst, theLast]. If the output curve
  //! lies on a direct line the infinite line is returned, in the case
  //! same-parameter condition is satisfied.
  //! @param[in] theC3D the 3D curve to project
  //! @param[in] theFirst the first parameter of the curve
  //! @param[in] theLast the last parameter of the curve
  //! @param[out] theC2D the resulting 2D curve
  //! @param[in] theTolFirst the tolerance at the first point (default: Precision::Confusion())
  //! @param[in] theTolLast the tolerance at the last point (default: Precision::Confusion())
  //! @return true if projection succeeded
  Standard_EXPORT virtual Standard_Boolean Perform(
    const Handle(Geom_Curve)& theC3D,
    const Standard_Real       theFirst,
    const Standard_Real       theLast,
    Handle(Geom2d_Curve)&     theC2D,
    const Standard_Real       theTolFirst = Precision::Confusion(),
    const Standard_Real       theTolLast  = Precision::Confusion());

  DEFINE_STANDARD_RTTIEXT(ShapeConstruct_ProjectCurveOnSurface, Standard_Transient)

protected:
  //! Try to approximate 3D curve by Geom2d_Line
  //! or Geom2d_BSplineCurve with degree 1 with specified tolerance.
  //! @param[in] thePoints points obtained from 3d curve
  //! @param[in] theParams parameters corresponding points on 3d curve
  //! @param[out] thePoints2d 2d points lies on line in parametric space
  //! @param[in] theTol tolerance used for compare initial points 3d and
  //!            3d points obtained from line lying in parametric space of surface
  //! @param[out] theIsRecompute flag indicating if recomputation is needed
  //! @param[out] theIsFromCache flag indicating if result is from cache
  //! @return the resulting 2D curve or null if line fitting failed
  Standard_EXPORT Handle(Geom2d_Curve) getLine(const ArrayOfPnt&   thePoints,
                                               const ArrayOfReal&  theParams,
                                               ArrayOfPnt2d&       thePoints2d,
                                               const Standard_Real theTol,
                                               Standard_Boolean&   theIsRecompute,
                                               Standard_Boolean&   theIsFromCache) const;

  //! Computes the projection of 3d curve onto a surface using the
  //! standard algorithm from ProjLib. Returns False if standard
  //! projector fails or raises an exception or cuts the curve by
  //! parametrical bounds of the surface. Else, if pcurve computed
  //! successfully, returns True.
  //! @param[in] theC3D the 3D curve to project
  //! @param[in] theFirst the first parameter of the curve
  //! @param[in] theLast the last parameter of the curve
  //! @param[out] theC2D the resulting 2D curve
  //! @return true if projection succeeded
  Standard_EXPORT Standard_Boolean PerformByProjLib(const Handle(Geom_Curve)& theC3D,
                                                    const Standard_Real       theFirst,
                                                    const Standard_Real       theLast,
                                                    Handle(Geom2d_Curve)&     theC2D);

  //! Performs analytical projection for special cases (plane surfaces)
  //! @param[in] theC3D the 3D curve to project
  //! @return the resulting 2D curve or null if analytical projection not applicable
  Standard_EXPORT Handle(Geom2d_Curve) projectAnalytic(const Handle(Geom_Curve)& theC3D) const;

  //! Main approximation routine for pcurve computation
  //! @param[in] theNbPnt number of points
  //! @param[in] theC3D the 3D curve
  //! @param[in] theTolFirst tolerance at first point
  //! @param[in] theTolLast tolerance at last point
  //! @param[in,out] thePoints array of 3D points
  //! @param[in,out] theParams array of parameters
  //! @param[out] thePoints2d array of 2D points
  //! @param[out] theC2D resulting 2D curve
  //! @return true if approximation succeeded
  Standard_EXPORT Standard_Boolean approxPCurve(const Standard_Integer    theNbPnt,
                                                const Handle(Geom_Curve)& theC3D,
                                                const Standard_Real       theTolFirst,
                                                const Standard_Real       theTolLast,
                                                ArrayOfPnt&               thePoints,
                                                ArrayOfReal&              theParams,
                                                ArrayOfPnt2d&             thePoints2d,
                                                Handle(Geom2d_Curve)&     theC2D);

  //! Corrects extremity point near singularity
  //! @param[in] theC3D the 3D curve
  //! @param[in] theParams array of parameters
  //! @param[in,out] thePoints2d array of 2D points
  //! @param[in] theIsFirstPoint true if correcting first point, false for last
  //! @param[in] thePointOnIsoLine point on isoline
  //! @param[in] theIsUIso true if U-isoline, false if V-isoline
  Standard_EXPORT void correctExtremity(const Handle(Geom_Curve)& theC3D,
                                        const ArrayOfReal&        theParams,
                                        ArrayOfPnt2d&             thePoints2d,
                                        const Standard_Boolean    theIsFirstPoint,
                                        const gp_Pnt2d&           thePointOnIsoLine,
                                        const Standard_Boolean    theIsUIso);

  //! Inserts additional point or adjusts coordinate to handle period jumps
  //! @param[in,out] theToAdjust flag indicating if adjustment should be done
  //! @param[in] theIndCoord coordinate index (1=U, 2=V)
  //! @param[in] thePeriod the period value
  //! @param[in] theTolOnPeriod tolerance on period
  //! @param[in,out] theCurCoord current coordinate value
  //! @param[in] thePrevCoord previous coordinate value
  //! @param[in] theC3D the 3D curve
  //! @param[in,out] theIndex current point index
  //! @param[in,out] thePoints array of 3D points
  //! @param[in,out] theParams array of parameters
  //! @param[in,out] thePoints2d array of 2D points
  Standard_EXPORT void insertAdditionalPointOrAdjust(Standard_Boolean&         theToAdjust,
                                                     const Standard_Integer    theIndCoord,
                                                     const Standard_Real       thePeriod,
                                                     const Standard_Real       theTolOnPeriod,
                                                     Standard_Real&            theCurCoord,
                                                     const Standard_Real       thePrevCoord,
                                                     const Handle(Geom_Curve)& theC3D,
                                                     Standard_Integer&         theIndex,
                                                     ArrayOfPnt&               thePoints,
                                                     ArrayOfReal&              theParams,
                                                     ArrayOfPnt2d&             thePoints2d);

  //! Interpolates 2D curve from points
  //! @param[in] theNbPnt number of points
  //! @param[in] thePoints2d array of 2D points
  //! @param[in] theParams array of parameters
  //! @return the interpolated 2D curve or null on failure
  Standard_EXPORT Handle(Geom2d_Curve) interpolatePCurve(const Standard_Integer theNbPnt,
                                                         const ArrayOfPnt2d&    thePoints2d,
                                                         const ArrayOfReal&     theParams) const;

  //! Approximates 2D curve from points
  //! @param[in] thePoints2d array of 2D points
  //! @param[in] theParams array of parameters
  //! @return the approximated 2D curve or null on failure
  Standard_EXPORT Handle(Geom2d_Curve) approximatePCurve(const ArrayOfPnt2d& thePoints2d,
                                                         const ArrayOfReal&  theParams) const;

  //! Checks and removes coincident 3D points
  //! @param[in,out] thePoints array of 3D points
  //! @param[in,out] theParams array of parameters
  //! @param[in,out] thePreci precision (may be adjusted)
  Standard_EXPORT void checkPoints(ArrayOfPnt&    thePoints,
                                   ArrayOfReal&   theParams,
                                   Standard_Real& thePreci) const;

  //! Checks and removes coincident 2D points
  //! @param[in,out] thePoints2d array of 2D points
  //! @param[in,out] theParams array of parameters
  //! @param[in,out] thePreci precision (may be adjusted)
  Standard_EXPORT void checkPoints2d(ArrayOfPnt2d&  thePoints2d,
                                     ArrayOfReal&   theParams,
                                     Standard_Real& thePreci) const;

  //! Detects if curve is isoparametric (U=const or V=const)
  //! @param[in] theNbPnt number of points
  //! @param[in] thePoints array of 3D points
  //! @param[in] theParams array of parameters
  //! @param[out] theIsTypeU true if U-iso, false if V-iso
  //! @param[out] theP1OnIso true if first point is on iso
  //! @param[out] theValueP1 2D value of first point on iso
  //! @param[out] theP2OnIso true if last point is on iso
  //! @param[out] theValueP2 2D value of last point on iso
  //! @param[out] theIsoPar2d3d true if parametrization matches
  //! @param[out] theCIso the isoline curve
  //! @param[out] theT1 first iso parameter
  //! @param[out] theT2 last iso parameter
  //! @param[out] theParamsOut output parameters on isoline
  //! @return true if curve is isoparametric
  Standard_EXPORT Standard_Boolean isAnIsoparametric(const Standard_Integer theNbPnt,
                                                     const ArrayOfPnt&      thePoints,
                                                     const ArrayOfReal&     theParams,
                                                     Standard_Boolean&      theIsTypeU,
                                                     Standard_Boolean&      theP1OnIso,
                                                     gp_Pnt2d&              theValueP1,
                                                     Standard_Boolean&      theP2OnIso,
                                                     gp_Pnt2d&              theValueP2,
                                                     Standard_Boolean&      theIsoPar2d3d,
                                                     Handle(Geom_Curve)&    theCIso,
                                                     Standard_Real&         theT1,
                                                     Standard_Real&         theT2,
                                                     ArrayOfReal&           theParamsOut) const;

private:
  Handle(ShapeAnalysis_Surface) mySurf;            //!< Surface to project on
  Standard_Real                 myPreci;           //!< Current precision
  Standard_Integer              myStatus;          //!< Operation status
  Standard_Integer              myAdjustOverDegen; //!< Seam adjustment flag
  CacheArray                    myCache; //!< Cached 3D/2D point pairs for projection optimization
};

#endif // _ShapeConstruct_ProjectCurveOnSurface_HeaderFile
