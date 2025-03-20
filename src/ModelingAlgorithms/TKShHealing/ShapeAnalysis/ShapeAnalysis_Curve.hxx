// Created on: 1998-06-03
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

#ifndef _ShapeAnalysis_Curve_HeaderFile
#define _ShapeAnalysis_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColgp_SequenceOfPnt.hxx>
class Geom_Curve;
class gp_Pnt;
class Adaptor3d_Curve;
class Geom2d_Curve;
class Bnd_Box2d;
class gp_XYZ;

//! Analyzing tool for 2d or 3d curve.
//! Computes parameters of projected point onto a curve.
class ShapeAnalysis_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Projects a Point on a Curve.
  //! Computes the projected point and its parameter on the curve.
  //! <preci> is used as 3d precision (hence, 0 will produce
  //! reject unless exact confusion).
  //! The number of iterations is limited.
  //! If AdjustToEnds is True, point will be adjusted to the end
  //! of the curve if distance is less than <preci>
  //!
  //! Returned value is the distance between the given point and
  //! computed one.
  Standard_EXPORT Standard_Real Project(const Handle(Geom_Curve)& C3D,
                                        const gp_Pnt&             P3D,
                                        const Standard_Real       preci,
                                        gp_Pnt&                   proj,
                                        Standard_Real&            param,
                                        const Standard_Boolean AdjustToEnds = Standard_True) const;

  //! Projects a Point on a Curve.
  //! Computes the projected point and its parameter on the curve.
  //! <preci> is used as 3d precision (hence, 0 will produce
  //! reject unless exact confusion).
  //! The number of iterations is limited.
  //!
  //! Returned value is the distance between the given point and
  //! computed one.
  Standard_EXPORT Standard_Real Project(const Adaptor3d_Curve& C3D,
                                        const gp_Pnt&          P3D,
                                        const Standard_Real    preci,
                                        gp_Pnt&                proj,
                                        Standard_Real&         param,
                                        const Standard_Boolean AdjustToEnds = Standard_True) const;

  //! Projects a Point on a Curve, but parameters are limited
  //! between <cf> and <cl>.
  //! The range [cf, cl] is extended with help of Adaptor3d on the
  //! basis of 3d precision <preci>.
  //! If AdjustToEnds is True, point will be adjusted to the end
  //! of the curve if distance is less than <preci>
  Standard_EXPORT Standard_Real Project(const Handle(Geom_Curve)& C3D,
                                        const gp_Pnt&             P3D,
                                        const Standard_Real       preci,
                                        gp_Pnt&                   proj,
                                        Standard_Real&            param,
                                        const Standard_Real       cf,
                                        const Standard_Real       cl,
                                        const Standard_Boolean AdjustToEnds = Standard_True) const;

  Standard_EXPORT Standard_Real ProjectAct(const Adaptor3d_Curve& C3D,
                                           const gp_Pnt&          P3D,
                                           const Standard_Real    preci,
                                           gp_Pnt&                proj,
                                           Standard_Real&         param) const;

  //! Projects a Point on a Curve using Newton method.
  //! <paramPrev> is taken as the first approximation of solution.
  //! If Newton algorithm fails the method Project() is used.
  //! If AdjustToEnds is True, point will be adjusted to the end
  //! of the curve if distance is less than <preci>
  Standard_EXPORT Standard_Real
    NextProject(const Standard_Real       paramPrev,
                const Handle(Geom_Curve)& C3D,
                const gp_Pnt&             P3D,
                const Standard_Real       preci,
                gp_Pnt&                   proj,
                Standard_Real&            param,
                const Standard_Real       cf,
                const Standard_Real       cl,
                const Standard_Boolean    AdjustToEnds = Standard_True) const;

  //! Projects a Point on a Curve using Newton method.
  //! <paramPrev> is taken as the first approximation of solution.
  //! If Newton algorithm fails the method Project() is used.
  Standard_EXPORT Standard_Real NextProject(const Standard_Real    paramPrev,
                                            const Adaptor3d_Curve& C3D,
                                            const gp_Pnt&          P3D,
                                            const Standard_Real    preci,
                                            gp_Pnt&                proj,
                                            Standard_Real&         param) const;

  //! Validate parameters First and Last for the given curve
  //! in order to make them valid for creation of edge.
  //! This includes:
  //! - limiting range [First,Last] by range of curve
  //! - adjusting range [First,Last] for periodic (or closed)
  //! curve if Last < First
  //! Returns True if parameters are OK or are successfully
  //! corrected, or False if parameters cannot be corrected.
  //! In the latter case, parameters are reset to range of curve.
  Standard_EXPORT Standard_Boolean ValidateRange(const Handle(Geom_Curve)& Crv,
                                                 Standard_Real&            First,
                                                 Standard_Real&            Last,
                                                 const Standard_Real       prec) const;

  //! Computes a boundary box on segment of curve C2d from First
  //! to Last. This is done by taking NPoints points from the
  //! curve and, if Exact is True, by searching for exact
  //! extrema. All these points are added to Box.
  Standard_EXPORT void FillBndBox(const Handle(Geom2d_Curve)& C2d,
                                  const Standard_Real         First,
                                  const Standard_Real         Last,
                                  const Standard_Integer      NPoints,
                                  const Standard_Boolean      Exact,
                                  Bnd_Box2d&                  Box) const;

  //! Defines which pcurve (C1 or C2) should be chosen for FORWARD
  //! seam edge.
  Standard_EXPORT Standard_Integer SelectForwardSeam(const Handle(Geom2d_Curve)& C1,
                                                     const Handle(Geom2d_Curve)& C2) const;

  //! Checks if points are planar with given preci. If Normal has not zero
  //! modulus, checks with given normal
  Standard_EXPORT static Standard_Boolean IsPlanar(const TColgp_Array1OfPnt& pnts,
                                                   gp_XYZ&                   Normal,
                                                   const Standard_Real       preci = 0);

  //! Checks if curve is planar with given preci. If Normal has not zero
  //! modulus, checks with given normal
  Standard_EXPORT static Standard_Boolean IsPlanar(const Handle(Geom_Curve)& curve,
                                                   gp_XYZ&                   Normal,
                                                   const Standard_Real       preci = 0);

  //! Returns sample points which will serve as linearisation
  //! of the2d curve in range (first, last)
  //! The distribution of sample points is consystent with
  //! what is used by BRepTopAdaptor_FClass2d
  Standard_EXPORT static Standard_Boolean GetSamplePoints(const Handle(Geom2d_Curve)& curve,
                                                          const Standard_Real         first,
                                                          const Standard_Real         last,
                                                          TColgp_SequenceOfPnt2d&     seq);

  //! Returns sample points which will serve as linearisation
  //! of the curve in range (first, last)
  Standard_EXPORT static Standard_Boolean GetSamplePoints(const Handle(Geom_Curve)& curve,
                                                          const Standard_Real       first,
                                                          const Standard_Real       last,
                                                          TColgp_SequenceOfPnt&     seq);

  //! Tells if the Curve is closed with given precision.
  //! If <preci> < 0 then Precision::Confusion is used.
  Standard_EXPORT static Standard_Boolean IsClosed(const Handle(Geom_Curve)& curve,
                                                   const Standard_Real       preci = -1);

  //! This method was implemented as fix for changes in trimmed curve
  //! behaviour. For the moment trimmed curve returns false anyway.
  //! So it is necessary to adapt all Data exchange tools for this behaviour.
  //! Current implementation takes into account that curve may be offset.
  Standard_EXPORT static Standard_Boolean IsPeriodic(const Handle(Geom_Curve)& curve);

  //! The same as for Curve3d.
  Standard_EXPORT static Standard_Boolean IsPeriodic(const Handle(Geom2d_Curve)& curve);

protected:
private:
};

#endif // _ShapeAnalysis_Curve_HeaderFile
