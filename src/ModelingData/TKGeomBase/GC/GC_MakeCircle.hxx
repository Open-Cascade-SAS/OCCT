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

#ifndef _GC_MakeCircle_HeaderFile
#define _GC_MakeCircle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_Circle.hxx>

class gp_Circ;
class gp_Ax2;
class gp_Pnt;
class gp_Dir;
class gp_Ax1;

//! Implements construction algorithms for circles in 3D space.
//!
//! * Create a circle parallel to another and passing
//! through a point.
//! * Create a Circle parallel to another at the distance
//! Dist.
//! * Create a Circle passing through 3 points.
//! * Create a Circle with its center and the normal of its
//! plane and its radius.
//! * Create a Circle with its axis and radius.
//! The circle parameter is the angle in radians.
//! The parametrization range is [0,2*PI].
//! The circle is a closed and periodic curve.
//! The center of the circle is the Location point of its axis
//! placement. The XDirection of the axis placement defines the
//! origin of the parametrization.
class GC_MakeCircle : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a circle from a `gp_Circ`.
  //! @param[in] theC source circle
  Standard_EXPORT GC_MakeCircle(const gp_Circ& theC);

  //! Creates a circle from axis placement and radius.
  //! @param[in] theA2 local coordinate system of the circle
  //! @param[in] theRadius circle radius
  //! @note Radius equal to `0.0` is allowed.
  //! @note The status is `gce_NegativeRadius` if `theRadius < 0.0`.
  Standard_EXPORT GC_MakeCircle(const gp_Ax2& theA2, const double theRadius);

  //! Creates a circle concentric to the input circle with an offset radius.
  //! @param[in] theCirc reference circle
  //! @param[in] theDist radius offset
  Standard_EXPORT GC_MakeCircle(const gp_Circ& theCirc, const double theDist);

  //! Creates a circle concentric to the input circle and passing through the input point.
  //! @param[in] theCirc source circle
  //! @param[in] thePoint point on resulting circle
  Standard_EXPORT GC_MakeCircle(const gp_Circ& theCirc, const gp_Pnt& thePoint);

  //! Creates a circle passing through three points.
  //! @param[in] theP1 first point
  //! @param[in] theP2 second point
  //! @param[in] theP3 third point
  Standard_EXPORT GC_MakeCircle(const gp_Pnt& theP1, const gp_Pnt& theP2, const gp_Pnt& theP3);

  //! Creates a circle from center point, normal and radius.
  //! @param[in] theCenter circle center
  //! @param[in] theNorm normal direction of circle plane
  //! @param[in] theRadius circle radius
  Standard_EXPORT GC_MakeCircle(const gp_Pnt& theCenter, const gp_Dir& theNorm, const double theRadius);

  //! Creates a circle from center point, axis point and radius.
  //! @param[in] theCenter circle center
  //! @param[in] thePtAxis point defining normal direction
  //! @param[in] theRadius circle radius
  //! @note The direction is defined by vector (`theCenter`,`thePtAxis`).
  Standard_EXPORT GC_MakeCircle(const gp_Pnt& theCenter, const gp_Pnt& thePtAxis, const double theRadius);

  //! Creates a circle from axis and radius.
  //! @param[in] theAxis circle axis
  //! @param[in] theRadius circle radius
  Standard_EXPORT GC_MakeCircle(const gp_Ax1& theAxis, const double theRadius);

  //! Returns the constructed circle.
  //! Exceptions
  //! StdFail_NotDone if no circle is constructed.
  //! @return resulting circle
  Standard_EXPORT const occ::handle<Geom_Circle>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_Circle>&() const { return Value(); }

private:
  occ::handle<Geom_Circle> TheCircle;
};

#endif // _GC_MakeCircle_HeaderFile
