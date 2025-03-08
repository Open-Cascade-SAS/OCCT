// Created on: 1992-08-06
// Created by: Laurent BUCHARD
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

#ifndef _IntAna_QuadQuadGeo_HeaderFile
#define _IntAna_QuadQuadGeo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <IntAna_ResultType.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
class gp_Pln;
class gp_Cylinder;
class gp_Sphere;
class gp_Cone;
class gp_Torus;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Parab;
class gp_Hypr;

//! Geometric intersections between two natural quadrics
//! (Sphere , Cylinder , Cone , Pln from gp).
//! The possible intersections are :
//! - 1 point
//! - 1 or 2 line(s)
//! - 1 Point and 1 Line
//! - 1 circle
//! - 1 ellipse
//! - 1 parabola
//! - 1 or 2 hyperbola(s).
//! - Empty : there is no intersection between the two quadrics.
//! - Same  : the quadrics are identical
//! - NoGeometricSolution : there may be an intersection, but it
//! is necessary to use an analytic algorithm to determine
//! it. See class IntQuadQuad from IntAna.
class IntAna_QuadQuadGeo
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT IntAna_QuadQuadGeo();

  //! Creates the intersection between two planes.
  //! TolAng is the angular tolerance used to determine
  //! if the planes are parallel.
  //! Tol is the tolerance used to determine if the planes
  //! are identical (only when they are parallel).
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Pln&       P1,
                                     const gp_Pln&       P2,
                                     const Standard_Real TolAng,
                                     const Standard_Real Tol);

  //! Intersects two planes.
  //! TolAng is the angular tolerance used to determine
  //! if the planes are parallel.
  //! Tol is the tolerance used to determine if the planes
  //! are identical (only when they are parallel).
  Standard_EXPORT void Perform(const gp_Pln&       P1,
                               const gp_Pln&       P2,
                               const Standard_Real TolAng,
                               const Standard_Real Tol);

  //! Creates the intersection between a plane and a cylinder.
  //! TolAng is the angular tolerance used to determine
  //! if the axis of the cylinder is parallel to the plane.
  //! Tol is the tolerance used to determine if the result
  //! is a circle or an ellipse. If the maximum distance between
  //! the ellipse solution and the circle centered at the ellipse
  //! center is less than Tol, the result will be the circle.
  //! H is the height of the cylinder <Cyl>. It is  used to check
  //! whether the plane and cylinder are parallel.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Pln&       P,
                                     const gp_Cylinder&  C,
                                     const Standard_Real Tolang,
                                     const Standard_Real Tol,
                                     const Standard_Real H = 0);

  //! Intersects a plane and a cylinder.
  //! TolAng is the angular tolerance used to determine
  //! if the axis of the cylinder is parallel to the plane.
  //! Tol is the tolerance used to determine if the result
  //! is a circle or an ellipse. If the maximum distance between
  //! the ellipse solution and the circle centered at the ellipse
  //! center is less than Tol, the result will be the circle.
  //! H is the height of the cylinder <Cyl>. It is  used to check
  //! whether the plane and cylinder are parallel.
  Standard_EXPORT void Perform(const gp_Pln&       P,
                               const gp_Cylinder&  C,
                               const Standard_Real Tolang,
                               const Standard_Real Tol,
                               const Standard_Real H = 0);

  //! Creates the intersection between a plane and a sphere.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Pln& P, const gp_Sphere& S);

  //! Intersects a plane and a sphere.
  Standard_EXPORT void Perform(const gp_Pln& P, const gp_Sphere& S);

  //! Creates the intersection between a plane and a cone.
  //! TolAng is the angular tolerance used to determine
  //! if the axis of the cone is parallel or perpendicular
  //! to the plane, and if the generating line of the cone
  //! is parallel to the plane.
  //! Tol is the tolerance used to determine if the apex
  //! of the cone is in the plane.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Pln&       P,
                                     const gp_Cone&      C,
                                     const Standard_Real Tolang,
                                     const Standard_Real Tol);

  //! Intersects a plane and a cone.
  //! TolAng is the angular tolerance used to determine
  //! if the axis of the cone is parallel or perpendicular
  //! to the plane, and if the generating line of the cone
  //! is parallel to the plane.
  //! Tol is the tolerance used to determine if the apex
  //! of the cone is in the plane.
  Standard_EXPORT void Perform(const gp_Pln&       P,
                               const gp_Cone&      C,
                               const Standard_Real Tolang,
                               const Standard_Real Tol);

  //! Creates the intersection between two cylinders.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Cylinder&  Cyl1,
                                     const gp_Cylinder&  Cyl2,
                                     const Standard_Real Tol);

  //! Intersects two cylinders
  Standard_EXPORT void Perform(const gp_Cylinder&  Cyl1,
                               const gp_Cylinder&  Cyl2,
                               const Standard_Real Tol);

  //! Creates the intersection between a Cylinder and a Sphere.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Cylinder&  Cyl,
                                     const gp_Sphere&    Sph,
                                     const Standard_Real Tol);

  //! Intersects a cylinder and a sphere.
  Standard_EXPORT void Perform(const gp_Cylinder&  Cyl,
                               const gp_Sphere&    Sph,
                               const Standard_Real Tol);

  //! Creates the intersection between a Cylinder and a Cone
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Cylinder&  Cyl,
                                     const gp_Cone&      Con,
                                     const Standard_Real Tol);

  //! Intersects a cylinder and a cone.
  Standard_EXPORT void Perform(const gp_Cylinder& Cyl, const gp_Cone& Con, const Standard_Real Tol);

  //! Creates the intersection between two Spheres.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Sphere&    Sph1,
                                     const gp_Sphere&    Sph2,
                                     const Standard_Real Tol);

  //! Intersects a two spheres.
  Standard_EXPORT void Perform(const gp_Sphere&    Sph1,
                               const gp_Sphere&    Sph2,
                               const Standard_Real Tol);

  //! Creates the intersection between a Sphere and a Cone.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Sphere&    Sph,
                                     const gp_Cone&      Con,
                                     const Standard_Real Tol);

  //! Intersects a sphere and a cone.
  Standard_EXPORT void Perform(const gp_Sphere& Sph, const gp_Cone& Con, const Standard_Real Tol);

  //! Creates the intersection between two cones.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Cone&      Con1,
                                     const gp_Cone&      Con2,
                                     const Standard_Real Tol);

  //! Intersects two cones.
  Standard_EXPORT void Perform(const gp_Cone& Con1, const gp_Cone& Con2, const Standard_Real Tol);

  //! Creates the intersection between plane and torus.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Pln&       Pln,
                                     const gp_Torus&     Tor,
                                     const Standard_Real Tol);

  //! Intersects plane and torus.
  Standard_EXPORT void Perform(const gp_Pln& Pln, const gp_Torus& Tor, const Standard_Real Tol);

  //! Creates the intersection between cylinder and torus.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Cylinder&  Cyl,
                                     const gp_Torus&     Tor,
                                     const Standard_Real Tol);

  //! Intersects cylinder and torus.
  Standard_EXPORT void Perform(const gp_Cylinder&  Cyl,
                               const gp_Torus&     Tor,
                               const Standard_Real Tol);

  //! Creates the intersection between cone and torus.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Cone&      Con,
                                     const gp_Torus&     Tor,
                                     const Standard_Real Tol);

  //! Intersects cone and torus.
  Standard_EXPORT void Perform(const gp_Cone& Con, const gp_Torus& Tor, const Standard_Real Tol);

  //! Creates the intersection between sphere and torus.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Sphere&    Sph,
                                     const gp_Torus&     Tor,
                                     const Standard_Real Tol);

  //! Intersects sphere and torus.
  Standard_EXPORT void Perform(const gp_Sphere& Sph, const gp_Torus& Tor, const Standard_Real Tol);

  //! Creates the intersection between two toruses.
  Standard_EXPORT IntAna_QuadQuadGeo(const gp_Torus&     Tor1,
                                     const gp_Torus&     Tor2,
                                     const Standard_Real Tol);

  //! Intersects two toruses.
  Standard_EXPORT void Perform(const gp_Torus& Tor1, const gp_Torus& Tor2, const Standard_Real Tol);

  //! Returns Standard_True if the computation was successful.
  Standard_Boolean IsDone() const;

  //! Returns the type of intersection.
  IntAna_ResultType TypeInter() const;

  //! Returns the number of intersections.
  //! The possible intersections are :
  //! - 1 point
  //! - 1 or 2 line(s)
  //! - 1 Point and 1 Line
  //! - 1 circle
  //! - 1 ellipse
  //! - 1 parabola
  //! - 1 or 2 hyperbola(s).
  Standard_Integer NbSolutions() const;

  //! Returns the point solution of range Num.
  Standard_EXPORT gp_Pnt Point(const Standard_Integer Num) const;

  //! Returns the line solution of range Num.
  Standard_EXPORT gp_Lin Line(const Standard_Integer Num) const;

  //! Returns the circle solution of range Num.
  Standard_EXPORT gp_Circ Circle(const Standard_Integer Num) const;

  //! Returns the ellipse solution of range Num.
  Standard_EXPORT gp_Elips Ellipse(const Standard_Integer Num) const;

  //! Returns the parabola solution of range Num.
  Standard_EXPORT gp_Parab Parabola(const Standard_Integer Num) const;

  //! Returns the hyperbola solution of range Num.
  Standard_EXPORT gp_Hypr Hyperbola(const Standard_Integer Num) const;

  Standard_EXPORT Standard_Boolean HasCommonGen() const;

  Standard_EXPORT const gp_Pnt& PChar() const;

protected:
  //! Initialize the values of inner tolerances.
  Standard_EXPORT void InitTolerances();

  Standard_Boolean  done;
  Standard_Integer  nbint;
  IntAna_ResultType typeres;
  gp_Pnt            pt1;
  gp_Pnt            pt2;
  gp_Pnt            pt3;
  gp_Pnt            pt4;
  gp_Dir            dir1;
  gp_Dir            dir2;
  gp_Dir            dir3;
  gp_Dir            dir4;
  Standard_Real     param1;
  Standard_Real     param2;
  Standard_Real     param3;
  Standard_Real     param4;
  Standard_Real     param1bis;
  Standard_Real     param2bis;
  Standard_Real     myEPSILON_DISTANCE;
  Standard_Real     myEPSILON_ANGLE_CONE;
  Standard_Real     myEPSILON_MINI_CIRCLE_RADIUS;
  Standard_Real     myEPSILON_CYLINDER_DELTA_RADIUS;
  Standard_Real     myEPSILON_CYLINDER_DELTA_DISTANCE;
  Standard_Real     myEPSILON_AXES_PARA;
  Standard_Boolean  myCommonGen;
  gp_Pnt            myPChar;

private:
};

#include <IntAna_QuadQuadGeo.lxx>

#endif // _IntAna_QuadQuadGeo_HeaderFile
