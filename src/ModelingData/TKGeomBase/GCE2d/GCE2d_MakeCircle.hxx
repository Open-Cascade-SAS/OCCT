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

#ifndef _GCE2d_MakeCircle_HeaderFile
#define _GCE2d_MakeCircle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_Circle.hxx>

class gp_Circ2d;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;

//! This class implements construction algorithms for circles in the plane.
//! The result is a `Geom2d_Circle`.
//! A `GCE2d_MakeCircle` object provides a framework for:
//! - defining the construction parameters;
//! - running the construction algorithm;
//! - querying the construction status and the resulting circle via `Value()`.
//! @note A circle is parameterized in the range [0, 2*PI], and the X axis
//!       of its local coordinate system defines the parameter origin.
class GCE2d_MakeCircle : public GCE2d_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a circle from a non-persistent one from package gp.
  //! @param[in] theCircle source circle
  Standard_EXPORT GCE2d_MakeCircle(const gp_Circ2d& theCircle);

  //! Creates a circle from an axis placement and radius.
  //! @param[in] theAxis axis placement
  //! @param[in] theRadius radius value
  //! @param[in] theSense orientation flag
  //! @note Construction fails with `gce_NegativeRadius` if `theRadius` is negative.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Ax2d& theAxis,
                                   const double   theRadius,
                                   const bool     theSense = true);

  //! Creates a circle from a local coordinate system and radius.
  //! @param[in] theAxis local coordinate system
  //! @param[in] theRadius radius value
  //! @note Construction fails with `gce_NegativeRadius` if `theRadius` is negative.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Ax22d& theAxis, const double theRadius);

  //! Creates a circle parallel to another one at signed distance.
  //! @param[in] theCircle source circle
  //! @param[in] theDist signed distance
  //! @note If `theDist` is positive, the resulting circle encloses `theCircle`.
  //! @note If `theDist` is negative, the resulting circle is enclosed by `theCircle`.
  //! @note Error status is provided by the underlying `gce_MakeCirc2d`.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Circ2d& theCircle, const double theDist);

  //! Creates a circle parallel to another one and passing through a point.
  //! @param[in] theCircle source circle
  //! @param[in] thePoint point on resulting circle
  //! @note Error status is provided by the underlying `gce_MakeCirc2d`.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Circ2d& theCircle, const gp_Pnt2d& thePoint);

  //! Creates a circle passing through three points.
  //! @param[in] theP1 first point
  //! @param[in] theP2 second point
  //! @param[in] theP3 third point
  //! @note Error status is provided by the underlying `gce_MakeCirc2d`.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Pnt2d& theP1,
                                   const gp_Pnt2d& theP2,
                                   const gp_Pnt2d& theP3);

  //! Creates a circle from center point and radius.
  //! @param[in] theCenter center point
  //! @param[in] theRadius radius value
  //! @param[in] theSense orientation flag
  //! @note Error status is provided by the underlying `gce_MakeCirc2d`.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Pnt2d& theCenter,
                                   const double    theRadius,
                                   const bool      theSense = true);

  //! Creates a circle from center point and one point on the circle.
  //! @param[in] theCenter center point
  //! @param[in] thePoint point on resulting circle
  //! @param[in] theSense orientation flag
  //! @note Error status is provided by the underlying `gce_MakeCirc2d`.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Pnt2d& theCenter,
                                   const gp_Pnt2d& thePoint,
                                   const bool      theSense = true);

  //! Returns the constructed circle.
  //! Exceptions StdFail_NotDone if no circle is constructed.
  //! @return resulting circle
  Standard_EXPORT const occ::handle<Geom2d_Circle>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting circle
  operator const occ::handle<Geom2d_Circle>&() const { return Value(); }

private:
  occ::handle<Geom2d_Circle> TheCircle;
};

#endif // _GCE2d_MakeCircle_HeaderFile
