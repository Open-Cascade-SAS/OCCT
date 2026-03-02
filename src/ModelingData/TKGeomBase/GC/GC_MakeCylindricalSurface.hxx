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

#ifndef _GC_MakeCylindricalSurface_HeaderFile
#define _GC_MakeCylindricalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_CylindricalSurface.hxx>

class gp_Ax2;
class gp_Cylinder;
class gp_Pnt;
class gp_Ax1;
class gp_Circ;

//! Implements construction algorithms for cylindrical surfaces.
//! Supported constructions include:
//! - a cylindrical surface from axis placement and radius;
//! - conversion from `gp_Cylinder`;
//! - an offset/parallel cylindrical surface;
//! - a cylindrical surface through three points;
//! - a cylindrical surface from axis and radius;
//! - a cylindrical surface from circular base.
//! The local coordinate system of the CylindricalSurface is defined
//! with an axis placement (see class ElementarySurface).
//!
//! The "ZAxis" is the symmetry axis of the CylindricalSurface,
//! it gives the direction of increasing parametric value V.
//!
//! The parametrization range is :
//! U [0, 2*PI], V ]- infinite, + infinite[
//!
//! The "XAxis" and the "YAxis" define the placement plane of the
//! surface (Z = 0, and parametric value V = 0) perpendicular to
//! the symmetry axis. The "XAxis" defines the origin of the
//! parameter U = 0. The trigonometric sense gives the positive
//! orientation for the parameter U.
class GC_MakeCylindricalSurface : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a cylindrical surface from axis placement and radius.
  //! @param[in] theA2 local coordinate system
  //! @param[in] theRadius cylinder radius
  //! @note `theA2` defines the local coordinate system of the cylindrical surface.
  //! @note The "ZDirection" of `theA2` defines the direction of the surface axis
  //!       of symmetry.
  //! @note The created surface is parametrized such that the normal vector
  //!       (`N = D1U ^ D1V`) is oriented towards the "outside region".
  //! @note It is valid to create a cylindrical surface with `theRadius = 0.0`.
  //! @note Status is `gce_NegativeRadius` if `theRadius < 0.0`.
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Ax2& theA2, const double theRadius);

  //! Creates a cylindrical surface from a `gp_Cylinder`.
  //! @param[in] theC source cylinder
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Cylinder& theC);

  //! Creates a cylindrical surface parallel to the input cylinder and passing through the input point.
  //! @param[in] theCyl source cylinder
  //! @param[in] thePoint point on resulting surface
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Cylinder& theCyl, const gp_Pnt& thePoint);

  //! Creates a cylindrical surface parallel to the input cylinder at signed distance.
  //! @param[in] theCyl source cylinder
  //! @param[in] theDist signed offset distance
  //! @note The result radius is the absolute value of
  //!       (source radius + signed distance).
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Cylinder& theCyl, const double theDist);

  //! Creates a cylindrical surface passing through three points.
  //! @param[in] theP1 first axis point
  //! @param[in] theP2 second axis point
  //! @param[in] theP3 point defining radius
  //! @note The axis is defined by points `theP1` and `theP2`, and radius is the
  //!       distance between point `theP3` and that axis.
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Pnt& theP1, const gp_Pnt& theP2, const gp_Pnt& theP3);

  //! Creates a cylindrical surface from axis and radius.
  //! @param[in] theAxis cylinder axis
  //! @param[in] theRadius cylinder radius
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Ax1& theAxis, const double theRadius);

  //! Creates a cylindrical surface from its circular base.
  //! @param[in] theCirc base circle
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Circ& theCirc);

  //! Returns the constructed cylinder.
  //! Exceptions StdFail_NotDone if no cylinder is constructed.
  //! @return resulting cylindrical surface
  Standard_EXPORT const occ::handle<Geom_CylindricalSurface>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_CylindricalSurface>&() const { return Value(); }

private:
  occ::handle<Geom_CylindricalSurface> TheCylinder;
};

#endif // _GC_MakeCylindricalSurface_HeaderFile
