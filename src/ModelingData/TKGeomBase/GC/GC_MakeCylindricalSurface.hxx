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

//! This class implements the following algorithms used
//! to create a CylindricalSurface from Geom.
//! * Create a CylindricalSurface parallel to another and
//! passing through a point.
//! * Create a CylindricalSurface parallel to another at a
//! distance
//! <Dist>.
//! * Create a CylindricalSurface passing through 3 points.
//! * Create a CylindricalSurface by its axis and radius.
//! * Create a cylindricalSurface by its circular base.
//! The local coordinate system of the CylindricalSurface is defined
//! with an axis placement (see class ElementarySurface).
//!
//! The "ZAxis" is the symmetry axis of the CylindricalSurface,
//! it gives the direction of increasing parametric value V.
//!
//! The parametrization range is :
//! U [0, 2*PI],  V ]- infinite, + infinite[
//!
//! The "XAxis" and the "YAxis" define the placement plane of the
//! surface (Z = 0, and parametric value V = 0)  perpendicular to
//! the symmetry axis. The "XAxis" defines the origin of the
//! parameter U = 0.  The trigonometric sense gives the positive
//! orientation for the parameter U.
class GC_MakeCylindricalSurface : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! A2 defines the local coordinate system of the cylindrical surface.
  //! The "ZDirection" of A2 defines the direction of the surface's
  //! axis of symmetry.
  //! At the creation the parametrization of the surface is defined
  //! such that the normal Vector (N = D1U ^ D1V) is oriented towards
  //! the "outside region" of the surface.
  //! Warnings :
  //! It is not forbidden to create a cylindrical surface with
  //! Radius = 0.0
  //! Status is "NegativeRadius" if Radius < 0.0
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Ax2& A2, const Standard_Real Radius);

  //! Creates a CylindricalSurface from a non persistent Cylinder
  //! from package gp.
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Cylinder& C);

  //! Make a CylindricalSurface from Geom <TheCylinder>
  //! parallel to another
  //! CylindricalSurface <Cylinder> and passing through a
  //! Pnt <Point>.
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Cylinder& Cyl, const gp_Pnt& Point);

  //! Make a CylindricalSurface from Geom <TheCylinder>
  //! parallel to another
  //! CylindricalSurface <Cylinder> at the distance <Dist>
  //! which can be greater or lower than zero.
  //! The radius of the result is the absolute value of the
  //! radius of <Cyl> plus <Dist>
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Cylinder& Cyl, const Standard_Real Dist);

  //! Make a CylindricalSurface from Geom <TheCylinder>
  //! passing through 3 Pnt <P1>,<P2>,<P3>.
  //! Its axis is <P1P2> and its radius is the distance
  //! between <P3> and <P1P2>
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3);

  //! Make a CylindricalSurface by its axis <Axis> and radius
  //! <Radius>.
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Ax1& Axis, const Standard_Real Radius);

  //! Make a CylindricalSurface by its circular base.
  Standard_EXPORT GC_MakeCylindricalSurface(const gp_Circ& Circ);

  //! Returns the constructed cylinder.
  //! Exceptions StdFail_NotDone if no cylinder is constructed.
  Standard_EXPORT const Handle(Geom_CylindricalSurface)& Value() const;

  operator const Handle(Geom_CylindricalSurface) & () const { return Value(); }

private:
  Handle(Geom_CylindricalSurface) TheCylinder;
};

#endif // _GC_MakeCylindricalSurface_HeaderFile
