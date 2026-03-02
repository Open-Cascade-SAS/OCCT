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

#ifndef _GC_MakeConicalSurface_HeaderFile
#define _GC_MakeConicalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_ConicalSurface.hxx>

class gp_Ax2;
class gp_Cone;
class gp_Pnt;

//! Implements construction algorithms for conical surfaces.
//! Supported constructions include:
//! - a conical surface from axis placement and angle/radius;
//! - conversion from `gp_Cone`;
//! - a conical surface through four points;
//! - a conical surface from two points and two radii.
//! The local coordinate system of the ConicalSurface is defined
//! with an axis placement (see class ElementarySurface).
//!
//! The "ZAxis" is the symmetry axis of the ConicalSurface,
//! it gives the direction of increasing parametric value V.
//! The apex of the surface is on the negative side of this axis.
//!
//! The parametrization range is:
//! U [0, 2*PI], V ]-infinite, + infinite[
//!
//! The "XAxis" and the "YAxis" define the placement plane of the
//! surface (Z = 0, and parametric value V = 0) perpendicular to
//! the symmetry axis. The "XAxis" defines the origin of the
//! parameter U = 0. The trigonometric sense gives the positive
//! orientation for the parameter U.
//!
//! When you create a ConicalSurface the U and V directions of
//! parametrization are such that at each point of the surface the
//! normal is oriented towards the "outside region".
class GC_MakeConicalSurface : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a conical surface from local frame, semi-angle and radius.
  //! @param[in] theA2 local coordinate system
  //! @param[in] theAng semi-angle
  //! @param[in] theRadius reference radius in placement plane
  //! @note `theA2` defines the local coordinate system of the conical surface.
  //! @note `theAng` is the conical surface semi-angle ]0, PI/2[.
  //! @note `theRadius` is the radius of the circle Viso in the placement plane
  //!       of the conical surface defined with "XAxis" and "YAxis".
  //! @note The "ZDirection" of `theA2` defines the direction of the surface axis
  //!       of symmetry.
  //! @note If the location point of `theA2` is the apex of the surface,
  //!       `theRadius` is zero.
  //! @note The created surface is parametrized such that the normal vector
  //!       (`N = D1U ^ D1V`) is oriented towards the "outside region".
  //! @note Status is `gce_NegativeRadius` if `theRadius < 0.0`, or
  //!       `gce_BadAngle` if `theAng` is outside valid range.
  Standard_EXPORT GC_MakeConicalSurface(const gp_Ax2& theA2, const double theAng, const double theRadius);

  //! Creates a conical surface from a `gp_Cone`.
  //! @param[in] theC source cone
  Standard_EXPORT GC_MakeConicalSurface(const gp_Cone& theC);

  //! Creates a conical surface from four points.
  //! @param[in] theP1 first point defining axis
  //! @param[in] theP2 second point defining axis
  //! @param[in] theP3 point defining first section radius
  //! @param[in] theP4 point defining second section radius
  //! @note Axis is defined by points `theP1` and `theP2`, and base radius is
  //!       the distance between point `theP3` and that axis.
  //! @note The distance between point `theP4` and that axis is the radius of
  //!       the section passing through P4.
  //! @note Construction fails if points `theP1`, `theP2`, `theP3` and `theP4` are
  //!       collinear, or if vector (`theP3`,`theP4`) is perpendicular/collinear
  //!       to vector (`theP1`,`theP2`).
  Standard_EXPORT GC_MakeConicalSurface(const gp_Pnt& theP1, const gp_Pnt& theP2, const gp_Pnt& theP3, const gp_Pnt& theP4);

  //! Creates a conical surface with two points and two radii.
  //! @param[in] theP1 first axis point
  //! @param[in] theP2 second axis point
  //! @param[in] theR1 radius at P1
  //! @param[in] theR2 radius at P2
  //! @note The axis of the solution is the line passing through `theP1` and `theP2`.
  //! @note `theR1` and `theR2` are radii of sections passing through
  //!       `theP1` and `theP2`.
  Standard_EXPORT GC_MakeConicalSurface(const gp_Pnt& theP1, const gp_Pnt& theP2, const double  theR1, const double  theR2);

  //! Returns the constructed cone.
  //! Exceptions
  //! StdFail_NotDone if no cone is constructed.
  //! @return resulting conical surface
  Standard_EXPORT const occ::handle<Geom_ConicalSurface>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_ConicalSurface>&() const { return Value(); }

private:
  occ::handle<Geom_ConicalSurface> TheCone;
};

#endif // _GC_MakeConicalSurface_HeaderFile
