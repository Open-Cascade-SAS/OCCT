// Created on: 1992-09-28
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _GC_MakeArcOfCircle_HeaderFile
#define _GC_MakeArcOfCircle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_TrimmedCurve.hxx>

class gp_Circ;
class gp_Pnt;
class gp_Vec;

//! Implements construction algorithms for an
//! arc of circle in 3D space. The result is a Geom_TrimmedCurve curve.
//! A MakeArcOfCircle object provides a framework for:
//! -   defining the construction of the arc of circle,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the
//! Value function returns the constructed arc of circle.
class GC_MakeArcOfCircle : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an arc of circle from angular bounds.
  //! @param[in] theCirc source circle
  //! @param[in] theAlpha1 first angle (radians)
  //! @param[in] theAlpha2 second angle (radians)
  //! @param[in] theSense orientation of resulting arc
  Standard_EXPORT GC_MakeArcOfCircle(const gp_Circ& theCirc,
                                     const double   theAlpha1,
                                     const double   theAlpha2,
                                     const bool     theSense);

  //! Creates an arc of circle from a point and an angular bound.
  //! @param[in] theCirc source circle
  //! @param[in] theP point on circle
  //! @param[in] theAlpha target angle (radians)
  //! @param[in] theSense orientation of resulting arc
  Standard_EXPORT GC_MakeArcOfCircle(const gp_Circ& theCirc,
                                     const gp_Pnt&  theP,
                                     const double   theAlpha,
                                     const bool     theSense);

  //! Creates an arc of circle from two points on the circle.
  //! @param[in] theCirc source circle
  //! @param[in] theP1 first point on circle
  //! @param[in] theP2 second point on circle
  //! @param[in] theSense orientation of resulting arc
  Standard_EXPORT GC_MakeArcOfCircle(const gp_Circ& theCirc,
                                     const gp_Pnt&  theP1,
                                     const gp_Pnt&  theP2,
                                     const bool     theSense);

  //! Creates an arc of circle passing through three points.
  //! @param[in] theP1 first point
  //! @param[in] theP2 second point
  //! @param[in] theP3 third point
  Standard_EXPORT GC_MakeArcOfCircle(const gp_Pnt& theP1, const gp_Pnt& theP2, const gp_Pnt& theP3);

  //! Creates an arc of circle from two points and a tangent at the first point.
  //! @param[in] theP1 start point
  //! @param[in] theV tangent vector at start point
  //! @param[in] theP2 end point
  //! @note The tangent direction is given by the input vector.
  //! The orientation of the arc is:
  //! -   the sense determined by the order of the three input points;
  //! -   the sense defined by the input vector; or
  //! -   for the other constructors:
  //! -   the sense of the source circle if the orientation flag is true, or
  //! -   the opposite sense if `theSense` is false.
  //! @note Angles are expressed in radians.
  //! @note If an error occurs (that is, when IsDone returns false),
  //!       Status() returns:
  //! -   `gce_ConfusedPoints` if:
  //! -   any two of the three input points are coincident, or
  //! -   the start and end points are coincident; or
  //! -   `gce_IntersectionError` if:
  //! -   the three points are collinear and not coincident, or
  //! -   the chord direction is collinear with the tangent vector.
  Standard_EXPORT GC_MakeArcOfCircle(const gp_Pnt& theP1, const gp_Vec& theV, const gp_Pnt& theP2);

  //! Returns the constructed arc of circle.
  //! Exceptions StdFail_NotDone if no arc of circle is constructed.
  //! @return resulting arc
  Standard_EXPORT const occ::handle<Geom_TrimmedCurve>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_TrimmedCurve>&() const { return Value(); }

private:
  occ::handle<Geom_TrimmedCurve> TheArc;
};

#endif // _GC_MakeArcOfCircle_HeaderFile
