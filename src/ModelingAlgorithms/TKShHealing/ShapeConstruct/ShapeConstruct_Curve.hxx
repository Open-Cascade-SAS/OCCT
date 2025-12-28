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

#ifndef _ShapeConstruct_Curve_HeaderFile
#define _ShapeConstruct_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class Geom_Curve;
class gp_Pnt;
class Geom2d_Curve;
class gp_Pnt2d;
class Geom_BSplineCurve;
class Geom2d_BSplineCurve;

//! Adjusts curve to have start and end points at the given
//! points (currently works on lines and B-Splines only)
class ShapeConstruct_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Modifies a curve in order to make its bounds confused with
  //! given points.
  //! Works only on lines and B-Splines, returns True in this case,
  //! else returns False.
  //! For line considers both bounding points, for B-Splines only
  //! specified.
  //!
  //! Warning : Does not check if curve should be reversed
  Standard_EXPORT bool AdjustCurve(const occ::handle<Geom_Curve>& C3D,
                                   const gp_Pnt&                  P1,
                                   const gp_Pnt&                  P2,
                                   const bool                     take1 = true,
                                   const bool                     take2 = true) const;

  //! Modifies a curve in order to make its bounds confused with
  //! given points.
  //! Works only on lines and B-Splines.
  //!
  //! For lines works as previous method, B-Splines are segmented
  //! at the given values and then are adjusted to the points.
  Standard_EXPORT bool AdjustCurveSegment(const occ::handle<Geom_Curve>& C3D,
                                          const gp_Pnt&                  P1,
                                          const gp_Pnt&                  P2,
                                          const double                   U1,
                                          const double                   U2) const;

  //! Modifies a curve in order to make its bounds confused with
  //! given points.
  //! Works only on lines and B-Splines, returns True in this case,
  //! else returns False.
  //!
  //! For line considers both bounding points, for B-Splines only
  //! specified.
  //!
  //! Warning : Does not check if curve should be reversed
  Standard_EXPORT bool AdjustCurve2d(const occ::handle<Geom2d_Curve>& C2D,
                                     const gp_Pnt2d&                  P1,
                                     const gp_Pnt2d&                  P2,
                                     const bool                       take1 = true,
                                     const bool                       take2 = true) const;

  //! Converts a curve of any type (only part from first to last)
  //! to bspline. The method of conversion depends on the type
  //! of original curve:
  //! BSpline -> C.Segment(first,last)
  //! Bezier and Line -> GeomConvert::CurveToBSplineCurve(C).Segment(first,last)
  //! Conic and Other -> Approx_Curve3d(C[first,last],prec,C1,9,1000)
  Standard_EXPORT occ::handle<Geom_BSplineCurve> ConvertToBSpline(const occ::handle<Geom_Curve>& C,
                                                                  const double first,
                                                                  const double last,
                                                                  const double prec) const;

  //! Converts a curve of any type (only part from first to last)
  //! to bspline. The method of conversion depends on the type
  //! of original curve:
  //! BSpline -> C.Segment(first,last)
  //! Bezier and Line -> GeomConvert::CurveToBSplineCurve(C).Segment(first,last)
  //! Conic and Other -> Approx_Curve2d(C[first,last],prec,C1,9,1000)
  Standard_EXPORT occ::handle<Geom2d_BSplineCurve> ConvertToBSpline(
    const occ::handle<Geom2d_Curve>& C,
    const double                     first,
    const double                     last,
    const double                     prec) const;

  Standard_EXPORT static bool FixKnots(occ::handle<NCollection_HArray1<double>>& knots);

  //! Fix bspline knots to ensure that there is enough
  //! gap between neighbouring values
  //! Returns True if something fixed (by shifting knot)
  Standard_EXPORT static bool FixKnots(NCollection_Array1<double>& knots);
};

#endif // _ShapeConstruct_Curve_HeaderFile
