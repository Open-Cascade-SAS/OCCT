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

#ifndef _GC_MakeSegment2d_HeaderFile
#define _GC_MakeSegment2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom2d_TrimmedCurve.hxx>

class gp_Pnt2d;
class gp_Dir2d;
class gp_Lin2d;

//! This class implements construction algorithms for line segments in the plane.
//! The result is a `Geom2d_TrimmedCurve`.
//! A `GC_MakeSegment2d` object provides a framework for:
//! - defining the construction parameters;
//! - running the construction algorithm;
//! - querying the construction status and the resulting segment via `Value()`.
class GC_MakeSegment2d : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a segment between two points.
  //! @param[in] theP1 first point
  //! @param[in] theP2 second point
  //! @note Construction fails with `gce_ConfusedPoints` if points are coincident.
  Standard_EXPORT GC_MakeSegment2d(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2);

  //! Creates a segment on a line defined by point and direction.
  //! The segment starts at `theP1` and ends at the orthogonal projection
  //! of `theP2` onto that line.
  //! @param[in] theP1 first point
  //! @param[in] theV direction vector
  //! @param[in] theP2 second point
  //! @note Construction fails with `gce_ConfusedPoints` if the projected
  //!       endpoint is coincident with `theP1` within resolution.
  Standard_EXPORT GC_MakeSegment2d(const gp_Pnt2d& theP1,
                                    const gp_Dir2d& theV,
                                    const gp_Pnt2d& theP2);

  //! Creates a segment on a line between two parameter values.
  //! @param[in] theLine source line
  //! @param[in] theU1 first parameter
  //! @param[in] theU2 second parameter
  Standard_EXPORT GC_MakeSegment2d(const gp_Lin2d& theLine, const double theU1, const double theU2);

  //! Creates a segment on a line between point parameter and target parameter.
  //! @param[in] theLine source line
  //! @param[in] thePoint first point on segment support line
  //! @param[in] theUlast last parameter
  Standard_EXPORT GC_MakeSegment2d(const gp_Lin2d& theLine,
                                    const gp_Pnt2d& thePoint,
                                    const double    theUlast);

  //! Creates a segment on a line between projections of two points.
  //! @param[in] theLine source line
  //! @param[in] theP1 first point
  //! @param[in] theP2 second point
  Standard_EXPORT GC_MakeSegment2d(const gp_Lin2d& theLine,
                                    const gp_Pnt2d& theP1,
                                    const gp_Pnt2d& theP2);

  //! Returns the constructed line segment.
  //! Exceptions StdFail_NotDone if no line segment is constructed.
  //! @return resulting trimmed curve
  Standard_EXPORT const occ::handle<Geom2d_TrimmedCurve>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting trimmed curve
  operator const occ::handle<Geom2d_TrimmedCurve>&() const { return Value(); }

private:
  occ::handle<Geom2d_TrimmedCurve> TheSegment;
};

#endif // _GC_MakeSegment2d_HeaderFile
