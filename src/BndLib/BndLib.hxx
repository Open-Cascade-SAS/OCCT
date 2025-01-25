// Created on: 1993-07-08
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BndLib_HeaderFile
#define _BndLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class gp_Lin;
class Bnd_Box;
class gp_Lin2d;
class Bnd_Box2d;
class gp_Circ;
class gp_Circ2d;
class gp_Elips;
class gp_Elips2d;
class gp_Parab;
class gp_Parab2d;
class gp_Hypr;
class gp_Hypr2d;
class gp_Cylinder;
class gp_Cone;
class gp_Sphere;
class gp_Torus;

//! The BndLib package provides functions to add a geometric primitive to a bounding box.
//! Note: these functions work with gp objects, optionally
//! limited by parameter values. If the curves and surfaces
//! provided by the gp package are not explicitly
//! parameterized, they still have an implicit parameterization,
//! similar to that which they infer for the equivalent Geom or Geom2d objects.
//! Add : Package to compute the bounding boxes for elementary
//! objects from gp in 2d and 3d .
//!
//! AddCurve2d : A class to compute the bounding box for a curve
//! in 2d dimensions ;the curve is defined by a tool
//!
//! AddCurve : A class to compute the bounding box for a curve
//! in 3d dimensions ;the curve is defined by a tool
//!
//! AddSurface : A class to compute the bounding box for a surface.
//! The surface is defined by a tool for the geometry and another
//! tool for the topology (only the edges in 2d dimensions)
class BndLib
{
public:
  DEFINE_STANDARD_ALLOC

  //! Bounding box for a surface trimmed or not
  //! Adds the segment of the line L limited by the two
  //! parameter values P1 and P2, to the bounding box B, and
  //! then enlarges B by the tolerance value Tol.
  //! Tol is the tolerance value to enlarge the minimum and maximum dimension
  //! P1 and P2 may represent infinite values.
  //! Exceptions
  //! Standard_Failure if P1 and P2 are either two negative
  //! infinite real numbers, or two positive infinite real numbers.
  Standard_EXPORT static void Add(const gp_Lin&       L,
                                  const Standard_Real P1,
                                  const Standard_Real P2,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);

  Standard_EXPORT static void Add(const gp_Lin2d&     L,
                                  const Standard_Real P1,
                                  const Standard_Real P2,
                                  const Standard_Real Tol,
                                  Bnd_Box2d&          B);

  Standard_EXPORT static void Add(const gp_Circ& C, const Standard_Real Tol, Bnd_Box& B);

  //! P2-P1 can be in [0,2*pi]
  Standard_EXPORT static void Add(const gp_Circ&      C,
                                  const Standard_Real P1,
                                  const Standard_Real P2,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);

  Standard_EXPORT static void Add(const gp_Circ2d& C, const Standard_Real Tol, Bnd_Box2d& B);

  //! Adds the circle C, or the arc of the circle C
  //! limited by the two parameter values P1 and P2,
  //! to the bounding box B, and then enlarges B by the tolerance value Tol.
  //! P2-P1 can be in [0,2*pi]
  Standard_EXPORT static void Add(const gp_Circ2d&    C,
                                  const Standard_Real P1,
                                  const Standard_Real P2,
                                  const Standard_Real Tol,
                                  Bnd_Box2d&          B);

  Standard_EXPORT static void Add(const gp_Elips& C, const Standard_Real Tol, Bnd_Box& B);

  //! P2-P1 can be in [0,2*pi]
  Standard_EXPORT static void Add(const gp_Elips&     C,
                                  const Standard_Real P1,
                                  const Standard_Real P2,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);

  Standard_EXPORT static void Add(const gp_Elips2d& C, const Standard_Real Tol, Bnd_Box2d& B);

  //! Adds the ellipse E, or the arc of the ellipse E
  //! limited by the two parameter values P1 and P2,
  //! to the bounding box B, and then enlarges B by the tolerance value Tol.
  //! P2-P1 can be in [0,2*pi]
  Standard_EXPORT static void Add(const gp_Elips2d&   C,
                                  const Standard_Real P1,
                                  const Standard_Real P2,
                                  const Standard_Real Tol,
                                  Bnd_Box2d&          B);

  Standard_EXPORT static void Add(const gp_Parab&     P,
                                  const Standard_Real P1,
                                  const Standard_Real P2,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);

  //! Adds the arc of the parabola P limited by the two
  //! parameter values P1 and P2, to the bounding box B, and
  //! then enlarges B by the tolerance value Tol.
  //! P1 and P2 may represent infinite values.
  //! Exceptions
  //! Standard_Failure if P1 and P2 are either two negative
  //! infinite real numbers, or two positive infinite real numbers.
  Standard_EXPORT static void Add(const gp_Parab2d&   P,
                                  const Standard_Real P1,
                                  const Standard_Real P2,
                                  const Standard_Real Tol,
                                  Bnd_Box2d&          B);

  Standard_EXPORT static void Add(const gp_Hypr&      H,
                                  const Standard_Real P1,
                                  const Standard_Real P2,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);

  //! Adds the arc of the branch of hyperbola H limited by the
  //! two parameter values P1 and P2, to the bounding box B,
  //! and then enlarges B by the tolerance value Tol.
  //! P1 and P2 may represent infinite values.
  //! Exceptions
  //! Standard_Failure if P1 and P2 are either two negative
  //! infinite real numbers, or two positive infinite real numbers.
  Standard_EXPORT static void Add(const gp_Hypr2d&    H,
                                  const Standard_Real P1,
                                  const Standard_Real P2,
                                  const Standard_Real Tol,
                                  Bnd_Box2d&          B);

  //! UMax -UMin can be in [0,2*pi]
  Standard_EXPORT static void Add(const gp_Cylinder&  S,
                                  const Standard_Real UMin,
                                  const Standard_Real UMax,
                                  const Standard_Real VMin,
                                  const Standard_Real VMax,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);

  //! Adds to the bounding box B, the patch of the cylinder S limited
  //! -   in the v parametric direction, by the two parameter
  //! values VMin and VMax
  //! -   and optionally in the u parametric direction, by the two
  //! parameter values UMin and UMax.
  //! B is then enlarged by the tolerance value Tol.
  //! VMin and VMax may represent infinite values.
  //! Exceptions
  //! Standard_Failure if VMin and VMax are either two
  //! negative infinite real numbers, or two positive infinite real numbers.
  Standard_EXPORT static void Add(const gp_Cylinder&  S,
                                  const Standard_Real VMin,
                                  const Standard_Real VMax,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);

  //! UMax-UMin can be in [0,2*pi]
  Standard_EXPORT static void Add(const gp_Cone&      S,
                                  const Standard_Real UMin,
                                  const Standard_Real UMax,
                                  const Standard_Real VMin,
                                  const Standard_Real VMax,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);

  //! Adds to the bounding box B, the patch of the cone S limited
  //! -   in the v parametric direction, by the two parameter
  //! values VMin and VMax
  //! -   and optionally in the u parametric direction, by the two
  //! parameter values UMin and UMax,
  //! B is then enlarged by the tolerance value Tol.
  //! VMin and VMax may represent infinite values.
  //! Exceptions
  //! Standard_Failure if VMin and VMax are either two
  //! negative infinite real numbers, or two positive infinite real numbers.
  Standard_EXPORT static void Add(const gp_Cone&      S,
                                  const Standard_Real VMin,
                                  const Standard_Real VMax,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);

  Standard_EXPORT static void Add(const gp_Sphere& S, const Standard_Real Tol, Bnd_Box& B);

  //! Adds to the bounding box B the sphere S, or
  //! -   the patch of the sphere S, limited in the u parametric
  //! direction, by the two parameter values UMin and UMax,
  //! and in the v parametric direction, by the two parameter
  //! values VMin and VMax.
  //! B is then enlarged by the tolerance value Tol.
  //! UMax-UMin can be in [0,2*pi]
  //! VMin,VMax can be [-pi/2,pi/2]
  Standard_EXPORT static void Add(const gp_Sphere&    S,
                                  const Standard_Real UMin,
                                  const Standard_Real UMax,
                                  const Standard_Real VMin,
                                  const Standard_Real VMax,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);

  Standard_EXPORT static void Add(const gp_Torus& P, const Standard_Real Tol, Bnd_Box& B);

  //! Adds to the bounding box B
  //! -   the torus S, or
  //! -   the patch of the torus S, limited in the u parametric
  //! direction, by the two parameter values UMin and UMax,
  //! and in the v parametric direction, by the two parameter
  //! values VMin and VMax.
  //! B is then enlarged by the tolerance value Tol.
  //! UMax-UMin can be in [0,2*pi],
  //! VMin,VMax can be [-pi/2,pi/2]
  Standard_EXPORT static void Add(const gp_Torus&     P,
                                  const Standard_Real UMin,
                                  const Standard_Real UMax,
                                  const Standard_Real VMin,
                                  const Standard_Real VMax,
                                  const Standard_Real Tol,
                                  Bnd_Box&            B);
};

#endif // _BndLib_HeaderFile
