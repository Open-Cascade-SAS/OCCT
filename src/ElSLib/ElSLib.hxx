// Created on: 1991-09-09
// Created by: Michel Chauvat
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _ElSLib_HeaderFile
#define _ElSLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
class gp_Pnt;
class gp_Pln;
class gp_Cone;
class gp_Cylinder;
class gp_Sphere;
class gp_Torus;
class gp_Vec;
class gp_Ax3;
class gp_Lin;
class gp_Circ;


//! Provides functions for basic geometric computation on
//! elementary surfaces.
//! This includes:
//! -   calculation of a point or derived vector on a surface
//! where the surface is provided by the gp package, or
//! defined in canonical form (as in the gp package), and
//! the point is defined with a parameter,
//! -   evaluation of the parameters corresponding to a
//! point on an elementary surface from gp,
//! -   calculation of isoparametric curves on an elementary
//! surface defined in canonical form (as in the gp package).
//! Notes:
//! -   ElSLib stands for Elementary Surfaces Library.
//! -   If the surfaces provided by the gp package are not
//! explicitly parameterized, they still have an implicit
//! parameterization, similar to that which they infer on
//! the equivalent Geom surfaces.
//! Note: ElSLib stands for Elementary Surfaces Library.
class ElSLib 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! For elementary surfaces from the gp package (planes,
  //! cones, cylinders, spheres and tori), computes the point
  //! of parameters (U, V).
    static gp_Pnt Value (const Standard_Real U, const Standard_Real V, const gp_Pln& Pl);
  
    static gp_Pnt Value (const Standard_Real U, const Standard_Real V, const gp_Cone& C);
  
    static gp_Pnt Value (const Standard_Real U, const Standard_Real V, const gp_Cylinder& C);
  
    static gp_Pnt Value (const Standard_Real U, const Standard_Real V, const gp_Sphere& S);
  
    static gp_Pnt Value (const Standard_Real U, const Standard_Real V, const gp_Torus& T);
  
  //! For elementary surfaces from the gp package (planes,
  //! cones, cylinders, spheres and tori), computes the
  //! derivative vector of order Nu and Nv in the u and v
  //! parametric directions respectively, at the point of
  //! parameters (U, V).
    static gp_Vec DN (const Standard_Real U, const Standard_Real V, const gp_Pln& Pl, const Standard_Integer Nu, const Standard_Integer Nv);
  
    static gp_Vec DN (const Standard_Real U, const Standard_Real V, const gp_Cone& C, const Standard_Integer Nu, const Standard_Integer Nv);
  
    static gp_Vec DN (const Standard_Real U, const Standard_Real V, const gp_Cylinder& C, const Standard_Integer Nu, const Standard_Integer Nv);
  
    static gp_Vec DN (const Standard_Real U, const Standard_Real V, const gp_Sphere& S, const Standard_Integer Nu, const Standard_Integer Nv);
  
    static gp_Vec DN (const Standard_Real U, const Standard_Real V, const gp_Torus& T, const Standard_Integer Nu, const Standard_Integer Nv);
  
  //! For elementary surfaces from the gp package (planes,
  //! cones, cylinders, spheres and tori), computes the point P
  //! of parameters (U, V).inline
    static void D0 (const Standard_Real U, const Standard_Real V, const gp_Pln& Pl, gp_Pnt& P);
  
    static void D0 (const Standard_Real U, const Standard_Real V, const gp_Cone& C, gp_Pnt& P);
  
    static void D0 (const Standard_Real U, const Standard_Real V, const gp_Cylinder& C, gp_Pnt& P);
  
    static void D0 (const Standard_Real U, const Standard_Real V, const gp_Sphere& S, gp_Pnt& P);
  
    static void D0 (const Standard_Real U, const Standard_Real V, const gp_Torus& T, gp_Pnt& P);
  

  //! For elementary surfaces from the gp package (planes,
  //! cones, cylinders, spheres and tori), computes:
  //! -   the point P of parameters (U, V), and
  //! -   the first derivative vectors Vu and Vv at this point in
  //! the u and v parametric directions respectively.
    static void D1 (const Standard_Real U, const Standard_Real V, const gp_Pln& Pl, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv);
  
    static void D1 (const Standard_Real U, const Standard_Real V, const gp_Cone& C, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv);
  
    static void D1 (const Standard_Real U, const Standard_Real V, const gp_Cylinder& C, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv);
  
    static void D1 (const Standard_Real U, const Standard_Real V, const gp_Sphere& S, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv);
  
    static void D1 (const Standard_Real U, const Standard_Real V, const gp_Torus& T, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv);
  

  //! For elementary surfaces from the gp package (cones,
  //! cylinders, spheres and tori), computes:
  //! -   the point P of parameters (U, V), and
  //! -   the first derivative vectors Vu and Vv at this point in
  //! the u and v parametric directions respectively, and
  //! -   the second derivative vectors Vuu, Vvv and Vuv at this point.
    static void D2 (const Standard_Real U, const Standard_Real V, const gp_Cone& C, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv);
  
    static void D2 (const Standard_Real U, const Standard_Real V, const gp_Cylinder& C, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv);
  
    static void D2 (const Standard_Real U, const Standard_Real V, const gp_Sphere& S, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv);
  
    static void D2 (const Standard_Real U, const Standard_Real V, const gp_Torus& T, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv);
  

  //! For elementary surfaces from the gp package (cones,
  //! cylinders, spheres and tori), computes:
  //! -   the point P of parameters (U,V), and
  //! -   the first derivative vectors Vu and Vv at this point in
  //! the u and v parametric directions respectively, and
  //! -   the second derivative vectors Vuu, Vvv and Vuv at
  //! this point, and
  //! -   the third derivative vectors Vuuu, Vvvv, Vuuv and
  //! Vuvv at this point.
    static void D3 (const Standard_Real U, const Standard_Real V, const gp_Cone& C, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv, gp_Vec& Vuuu, gp_Vec& Vvvv, gp_Vec& Vuuv, gp_Vec& Vuvv);
  
    static void D3 (const Standard_Real U, const Standard_Real V, const gp_Cylinder& C, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv, gp_Vec& Vuuu, gp_Vec& Vvvv, gp_Vec& Vuuv, gp_Vec& Vuvv);
  
    static void D3 (const Standard_Real U, const Standard_Real V, const gp_Sphere& S, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv, gp_Vec& Vuuu, gp_Vec& Vvvv, gp_Vec& Vuuv, gp_Vec& Vuvv);
  
  //! Surface evaluation
  //! The following functions compute the point and the
  //! derivatives on elementary surfaces defined with their
  //! geometric characteristics.
  //! You don't need to create the surface to use these functions.
  //! These functions are called by the previous  ones.
  //! Example :
  //! A cylinder is defined with its position and its radius.
    static void D3 (const Standard_Real U, const Standard_Real V, const gp_Torus& T, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv, gp_Vec& Vuuu, gp_Vec& Vvvv, gp_Vec& Vuuv, gp_Vec& Vuvv);
  
  Standard_EXPORT static gp_Pnt PlaneValue (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos);
  
  Standard_EXPORT static gp_Pnt CylinderValue (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius);
  
  Standard_EXPORT static gp_Pnt ConeValue (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real SAngle);
  
  Standard_EXPORT static gp_Pnt SphereValue (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius);
  
  Standard_EXPORT static gp_Pnt TorusValue (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  Standard_EXPORT static gp_Vec PlaneDN (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Integer Nu, const Standard_Integer Nv);
  
  Standard_EXPORT static gp_Vec CylinderDN (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Integer Nu, const Standard_Integer Nv);
  
  Standard_EXPORT static gp_Vec ConeDN (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real SAngle, const Standard_Integer Nu, const Standard_Integer Nv);
  
  Standard_EXPORT static gp_Vec SphereDN (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Integer Nu, const Standard_Integer Nv);
  
  Standard_EXPORT static gp_Vec TorusDN (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Integer Nu, const Standard_Integer Nv);
  
  Standard_EXPORT static void PlaneD0 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, gp_Pnt& P);
  
  Standard_EXPORT static void ConeD0 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real SAngle, gp_Pnt& P);
  
  Standard_EXPORT static void CylinderD0 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, gp_Pnt& P);
  
  Standard_EXPORT static void SphereD0 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, gp_Pnt& P);
  
  Standard_EXPORT static void TorusD0 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt& P);
  
  Standard_EXPORT static void PlaneD1 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv);
  
  Standard_EXPORT static void ConeD1 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real SAngle, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv);
  
  Standard_EXPORT static void CylinderD1 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv);
  
  Standard_EXPORT static void SphereD1 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv);
  
  Standard_EXPORT static void TorusD1 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv);
  
  Standard_EXPORT static void ConeD2 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real SAngle, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv);
  
  Standard_EXPORT static void CylinderD2 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv);
  
  Standard_EXPORT static void SphereD2 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv);
  
  Standard_EXPORT static void TorusD2 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv);
  
  Standard_EXPORT static void ConeD3 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real SAngle, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv, gp_Vec& Vuuu, gp_Vec& Vvvv, gp_Vec& Vuuv, gp_Vec& Vuvv);
  
  Standard_EXPORT static void CylinderD3 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv, gp_Vec& Vuuu, gp_Vec& Vvvv, gp_Vec& Vuuv, gp_Vec& Vuvv);
  
  Standard_EXPORT static void SphereD3 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real Radius, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv, gp_Vec& Vuuu, gp_Vec& Vvvv, gp_Vec& Vuuv, gp_Vec& Vuvv);
  

  //! The following functions compute the parametric values
  //! corresponding to a given point on a elementary surface.
  //! The point should be on the surface.
  Standard_EXPORT static void TorusD3 (const Standard_Real U, const Standard_Real V, const gp_Ax3& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt& P, gp_Vec& Vu, gp_Vec& Vv, gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv, gp_Vec& Vuuu, gp_Vec& Vvvv, gp_Vec& Vuuv, gp_Vec& Vuvv);
  
  //! parametrization
  //! P (U, V) =
  //! Pl.Location() + U * Pl.XDirection() + V * Pl.YDirection()
    static void Parameters (const gp_Pln& Pl, const gp_Pnt& P, Standard_Real& U, Standard_Real& V);
  
  //! parametrization
  //! P (U, V) = Location + V * ZDirection +
  //! Radius * (Cos(U) * XDirection + Sin (U) * YDirection)
    static void Parameters (const gp_Cylinder& C, const gp_Pnt& P, Standard_Real& U, Standard_Real& V);
  
  //! parametrization
  //! P (U, V) =  Location + V * ZDirection +
  //! (Radius + V * Tan (SemiAngle)) *
  //! (Cos(U) * XDirection + Sin(U) * YDirection)
    static void Parameters (const gp_Cone& C, const gp_Pnt& P, Standard_Real& U, Standard_Real& V);
  
  //! parametrization
  //! P (U, V) = Location +
  //! Radius * Cos (V) * (Cos (U) * XDirection + Sin (U) * YDirection) +
  //! Radius * Sin (V) * ZDirection
    static void Parameters (const gp_Sphere& S, const gp_Pnt& P, Standard_Real& U, Standard_Real& V);
  
  //! parametrization
  //! P (U, V) = Location +
  //! (MajorRadius + MinorRadius * Cos(U)) *
  //! (Cos(V) * XDirection - Sin(V) * YDirection) +
  //! MinorRadius * Sin(U) * ZDirection
    static void Parameters (const gp_Torus& T, const gp_Pnt& P, Standard_Real& U, Standard_Real& V);
  
  //! parametrization
  //! P (U, V) =
  //! Pl.Location() + U * Pl.XDirection() + V * Pl.YDirection()
  Standard_EXPORT static void PlaneParameters (const gp_Ax3& Pos, const gp_Pnt& P, Standard_Real& U, Standard_Real& V);
  
  //! parametrization
  //! P (U, V) = Location + V * ZDirection +
  //! Radius * (Cos(U) * XDirection + Sin (U) * YDirection)
  Standard_EXPORT static void CylinderParameters (const gp_Ax3& Pos, const Standard_Real Radius, const gp_Pnt& P, Standard_Real& U, Standard_Real& V);
  
  //! parametrization
  //! P (U, V) =  Location + V * ZDirection +
  //! (Radius + V * Tan (SemiAngle)) *
  //! (Cos(U) * XDirection + Sin(U) * YDirection)
  Standard_EXPORT static void ConeParameters (const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real SAngle, const gp_Pnt& P, Standard_Real& U, Standard_Real& V);
  
  //! parametrization
  //! P (U, V) = Location +
  //! Radius * Cos (V) * (Cos (U) * XDirection + Sin (U) * YDirection) +
  //! Radius * Sin (V) * ZDirection
  Standard_EXPORT static void SphereParameters (const gp_Ax3& Pos, const Standard_Real Radius, const gp_Pnt& P, Standard_Real& U, Standard_Real& V);
  
  //! parametrization
  //! P (U, V) = Location +
  //! (MajorRadius + MinorRadius * Cos(U)) *
  //! (Cos(V) * XDirection - Sin(V) * YDirection) +
  //! MinorRadius * Sin(U) * ZDirection
  Standard_EXPORT static void TorusParameters (const gp_Ax3& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const gp_Pnt& P, Standard_Real& U, Standard_Real& V);
  
  //! compute the U Isoparametric gp_Lin of the plane.
  Standard_EXPORT static gp_Lin PlaneUIso (const gp_Ax3& Pos, const Standard_Real U);
  
  //! compute the U Isoparametric gp_Lin of the cylinder.
  Standard_EXPORT static gp_Lin CylinderUIso (const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real U);
  
  //! compute the U Isoparametric gp_Lin of the cone.
  Standard_EXPORT static gp_Lin ConeUIso (const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real SAngle, const Standard_Real U);
  
  //! compute the U Isoparametric gp_Circ of the sphere,
  //! (the meridian is not trimmed).
  Standard_EXPORT static gp_Circ SphereUIso (const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real U);
  
  //! compute the U Isoparametric gp_Circ of the torus.
  Standard_EXPORT static gp_Circ TorusUIso (const gp_Ax3& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Real U);
  
  //! compute the V Isoparametric gp_Lin of the plane.
  Standard_EXPORT static gp_Lin PlaneVIso (const gp_Ax3& Pos, const Standard_Real V);
  
  //! compute the V Isoparametric gp_Circ of the cylinder.
  Standard_EXPORT static gp_Circ CylinderVIso (const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real V);
  
  //! compute the V Isoparametric gp_Circ of the cone.
  Standard_EXPORT static gp_Circ ConeVIso (const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real SAngle, const Standard_Real V);
  
  //! compute the V Isoparametric gp_Circ of the sphere,
  //! (the meridian is not trimmed).
  Standard_EXPORT static gp_Circ SphereVIso (const gp_Ax3& Pos, const Standard_Real Radius, const Standard_Real V);
  
  //! compute the V Isoparametric gp_Circ of the torus.
  Standard_EXPORT static gp_Circ TorusVIso (const gp_Ax3& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Real V);




protected:





private:





};


#include <ElSLib.lxx>





#endif // _ElSLib_HeaderFile
