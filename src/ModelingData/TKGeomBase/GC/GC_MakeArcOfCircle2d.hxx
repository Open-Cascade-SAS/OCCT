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

#ifndef _GC_MakeArcOfCircle2d_HeaderFile
#define _GC_MakeArcOfCircle2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom2d_TrimmedCurve.hxx>

class gp_Circ2d;
class gp_Pnt2d;
class gp_Vec2d;

//! This class implements construction algorithms for arcs of circles in the plane.
//! The result is a `Geom2d_TrimmedCurve`.
//! A `GC_MakeArcOfCircle2d` object provides a framework for:
//! - defining the construction parameters;
//! - running the construction algorithm;
//! - querying the construction status and the resulting arc via `Value()`.
//! @note Angular parameters are expressed in radians.
class GC_MakeArcOfCircle2d : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an arc from angular bounds on a circle.
  //! @param[in] theCircle source circle
  //! @param[in] theAlpha1 first angle (radians)
  //! @param[in] theAlpha2 second angle (radians)
  //! @param[in] theSense orientation of resulting arc
  Standard_EXPORT GC_MakeArcOfCircle2d(const gp_Circ2d& theCircle,
                                        const double     theAlpha1,
                                        const double     theAlpha2,
                                        const bool       theSense = true);

  //! Constructs an arc from a point and angular bound on a circle.
  //! @param[in] theCircle source circle
  //! @param[in] thePoint point on source circle
  //! @param[in] theAlpha angle value (radians)
  //! @param[in] theSense orientation of resulting arc
  Standard_EXPORT GC_MakeArcOfCircle2d(const gp_Circ2d& theCircle,
                                        const gp_Pnt2d&  thePoint,
                                        const double     theAlpha,
                                        const bool       theSense = true);

  //! Constructs an arc between two points on a circle.
  //! @param[in] theCircle source circle
  //! @param[in] theP1 first point on source circle
  //! @param[in] theP2 second point on source circle
  //! @param[in] theSense orientation of resulting arc
  Standard_EXPORT GC_MakeArcOfCircle2d(const gp_Circ2d& theCircle,
                                        const gp_Pnt2d&  theP1,
                                        const gp_Pnt2d&  theP2,
                                        const bool       theSense = true);

  //! Constructs an arc passing through three points.
  //! @param[in] theP1 first point
  //! @param[in] theP2 intermediate point
  //! @param[in] theP3 last point
  Standard_EXPORT GC_MakeArcOfCircle2d(const gp_Pnt2d& theP1,
                                        const gp_Pnt2d& theP2,
                                        const gp_Pnt2d& theP3);

  //! Constructs an arc from two points and tangent vector at start point.
  //! @param[in] theP1 start point
  //! @param[in] theV tangent vector at start point
  //! @param[in] theP2 end point
  Standard_EXPORT GC_MakeArcOfCircle2d(const gp_Pnt2d& theP1,
                                        const gp_Vec2d& theV,
                                        const gp_Pnt2d& theP2);

  //! Returns the constructed arc of circle.
  //! Exceptions StdFail_NotDone if no arc of circle is constructed.
  //! @return resulting trimmed curve
  Standard_EXPORT const occ::handle<Geom2d_TrimmedCurve>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting trimmed curve
  operator const occ::handle<Geom2d_TrimmedCurve>&() const { return Value(); }

private:
  occ::handle<Geom2d_TrimmedCurve> TheArc;
};

#endif // _GC_MakeArcOfCircle2d_HeaderFile
