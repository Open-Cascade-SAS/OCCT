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

#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Transient.hxx>
#include <ShapeExtend_Status.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_DynamicArray.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColStd_SequenceOfReal.hxx>

class ShapeAnalysis_Surface;
class GeomAdaptor_Curve;
class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;

// resolve name collisions with X11 headers
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
protected:
  using SequenceOfPnt = NCollection_DynamicArray<gp_Pnt>;
  using SequenceOfPnt2d = NCollection_DynamicArray<gp_Pnt2d>;
  using SequenceOfReal = NCollection_DynamicArray<Standard_Real>;

public:
  //! Empty constructor.
  Standard_EXPORT ShapeConstruct_ProjectCurveOnSurface();

  //! Initializes the object with all necessary parameters,
  //! i.e. surface and precision
  Standard_EXPORT virtual void Init(const Handle(Geom_Surface)& surf, const Standard_Real preci);

  //! Initializes the object with all necessary parameters,
  //! i.e. surface and precision
  Standard_EXPORT virtual void Init(const Handle(ShapeAnalysis_Surface)& surf,
                                    const Standard_Real                  preci);

  //! Loads a surface (in the form of Geom_Surface) to project on
  Standard_EXPORT void SetSurface(const Handle(Geom_Surface)& surf);

  //! Loads a surface (in the form of ShapeAnalysis_Surface) to project on
  Standard_EXPORT void SetSurface(const Handle(ShapeAnalysis_Surface)& surf);

  //! Sets value for current precision
  Standard_EXPORT void SetPrecision(const Standard_Real preci);

  //! Returns (modifiable) the flag specifying to which side of
  //! parametrical space adjust part of pcurve which lies on seam.
  //! This is required in very rare case when 3d curve which is
  //! to be projected goes partly along the seam on the closed
  //! surface with singularity (e.g. sphere), goes through the
  //! degenerated point and paerly lies on internal area of surface.
  //!
  //! If this flag is True, the seam part of such curve will be
  //! adjusted to the left side of parametric space (on sphere U=0),
  //! else to the right side (on sphere U=2*PI)
  //! Default value is True
  Standard_EXPORT Standard_Integer& AdjustOverDegenMode();

  //! Returns the status of last Perform
  Standard_EXPORT Standard_Boolean Status(const ShapeExtend_Status theStatus) const;

  //! Computes the projection of 3d curve onto a surface using the
  //! specialized algorithm. Returns False if projector fails,
  //! otherwise, if pcurve computed successfully, returns True.
  //! The output curve 2D is guaranteed to be same-parameter
  //! with input curve 3D on the interval [First, Last]. If the output curve
  //! lies on a direct line the infinite line is returned, in the case
  //! same-parameter condition is satisfied.
  //! TolFirst and TolLast are the tolerances at the ends of input curve 3D.
  Standard_EXPORT virtual Standard_Boolean Perform(
    const Handle(Geom_Curve)& theC3d,
    const Standard_Real       theFirst,
    const Standard_Real       theLast,
    Handle(Geom2d_Curve)&     theC2d,
    const Standard_Real       theTolFirst = Precision::Confusion(),
    const Standard_Real       theTolLast  = Precision::Confusion());

  DEFINE_STANDARD_RTTIEXT(ShapeConstruct_ProjectCurveOnSurface, Standard_Transient)

protected:
  //! Try to approximate 3D curve by Geom2d_Line
  //! or Geom2d_BsplineCurve with degree 1 with specified tolerance.
  //! points - points obtained from 3d curve.
  //! params - parameters corresponding points on 3d curves
  //! points2d - 2d points lies on line in parametric space
  //! theTol - tolerance used for compare initial points 3d and
  //! 3d points obtained from line lying in parameric space of surface
  Standard_EXPORT Handle(Geom2d_Curve) getLine(const SequenceOfPnt&   points,
                                               const SequenceOfReal& params,
                                               SequenceOfPnt2d&       points2d,
                                               const Standard_Real           theTol,
                                               Standard_Boolean&             IsRecompute,
                                               Standard_Boolean&             isFromCache) const;

  //! Computes the projection of 3d curve onto a surface using the
  //! standard algorithm from ProjLib. Returns False if standard
  //! projector fails or raises an exception or cuts the curve by
  //! parametrical bounds of the surface. Else, if pcurve computed
  //! successfully, returns True.
  Standard_EXPORT Standard_Boolean PerformByProjLib(const Handle(GeomAdaptor_Curve)& theC3d,
                                                    Handle(Geom2d_Curve)&            theC2d);

  Handle(ShapeAnalysis_Surface) mySurf;
  Standard_Real                 myPreci;
  Standard_Integer              myStatus;
  Standard_Integer              myAdjustOverDegen;
  Standard_Integer              myNbCache;
  gp_Pnt                        myCache3d[2];
  gp_Pnt2d                      myCache2d[2];

private:
  Standard_EXPORT Handle(Geom2d_Curve) ProjectAnalytic(const Handle(Geom_Curve)& c3d) const;

  Standard_EXPORT Standard_Boolean ApproxPCurve(const Standard_Integer           nbrPnt,
                                                const Handle(GeomAdaptor_Curve)& c3d,
                                                const Standard_Real               TolFirst,
                                                const Standard_Real               TolLast,
                                                SequenceOfPnt&                    points,
                                                SequenceOfReal&                   params,
                                                SequenceOfPnt2d&                  points2d,
                                                Handle(Geom2d_Curve)&            c2d);

  Standard_EXPORT void CorrectExtremity(const Handle(GeomAdaptor_Curve)& theC3d,
                                        const SequenceOfReal&            theParams,
                                        SequenceOfPnt2d&                  thePnt2d,
                                        const Standard_Boolean           theIsFirstPoint,
                                        const gp_Pnt2d&                  thePointOnIsoLine,
                                        const Standard_Boolean           theIsUiso);

  Standard_EXPORT void InsertAdditionalPointOrAdjust(Standard_Boolean&                ToAdjust,
                                                     const Standard_Integer           theIndCoord,
                                                     const Standard_Real              Period,
                                                     const Standard_Real              TolOnPeriod,
                                                     Standard_Real&                   CurCoord,
                                                     const Standard_Real              prevCoord,
                                                     const Handle(GeomAdaptor_Curve)& c3d,
                                                     Standard_Integer&                theIndex,
                                                     SequenceOfPnt&                   points,
                                                     SequenceOfReal&                  params,
                                                     SequenceOfPnt2d&                 pnt2d);

  Standard_EXPORT Handle(Geom2d_Curve) InterpolatePCurve(
    const Standard_Integer         nbrPnt,
    Handle(TColgp_HArray1OfPnt2d)& points2d,
    Handle(TColStd_HArray1OfReal)& params) const;

  Standard_EXPORT Handle(Geom2d_Curve) ApproximatePCurve(Handle(TColgp_HArray1OfPnt2d)& points2d,
                                                         Handle(TColStd_HArray1OfReal)& params) const;

  Standard_EXPORT Handle(Geom_Curve) InterpolateCurve3d(
    const Standard_Integer         nbrPnt,
    Handle(TColgp_HArray1OfPnt)&   points,
    Handle(TColStd_HArray1OfReal)& params) const;

  Standard_EXPORT void CheckPoints(Handle(TColgp_HArray1OfPnt)&   points,
                                   Handle(TColStd_HArray1OfReal)& params,
                                   Standard_Real&                 preci) const;

  Standard_EXPORT void CheckPoints2d(Handle(TColgp_HArray1OfPnt2d)& points,
                                     Handle(TColStd_HArray1OfReal)& params,
                                     Standard_Real&                 preci) const;

  Standard_EXPORT Standard_Boolean IsAnIsoparametric(const Standard_Integer        nbrPnt,
                                                     const SequenceOfPnt&           points,
                                                     const SequenceOfReal&         params,
                                                     Standard_Boolean&             isoTypeU,
                                                     Standard_Boolean&             p1OnIso,
                                                     gp_Pnt2d&                     valueP1,
                                                     Standard_Boolean&             p2OnIso,
                                                     gp_Pnt2d&                     valueP2,
                                                     Standard_Boolean&             isoPar2d3d,
                                                     Handle(Geom_Curve)&           cIso,
                                                     Standard_Real&                t1,
                                                     Standard_Real&                t2,
                                                     TColStd_Array1OfReal&         pout) const;
};

#endif // _ShapeConstruct_ProjectCurveOnSurface_HeaderFile
