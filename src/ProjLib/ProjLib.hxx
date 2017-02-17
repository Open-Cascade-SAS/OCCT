// Created on: 1993-08-11
// Created by: Bruno DUMORTIER
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

#ifndef _ProjLib_HeaderFile
#define _ProjLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Geom2d_Curve.hxx>

class gp_Pnt2d;
class gp_Pln;
class gp_Pnt;
class gp_Lin2d;
class gp_Lin;
class gp_Circ2d;
class gp_Circ;
class gp_Elips2d;
class gp_Elips;
class gp_Parab2d;
class gp_Parab;
class gp_Hypr2d;
class gp_Hypr;
class gp_Cylinder;
class gp_Cone;
class gp_Sphere;
class gp_Torus;
class ProjLib_ProjectOnPlane;
class ProjLib_ProjectOnSurface;
class ProjLib_ComputeApprox;
class ProjLib_ComputeApproxOnPolarSurface;
class ProjLib_ProjectedCurve;
class ProjLib_HProjectedCurve;
class ProjLib_CompProjectedCurve;
class ProjLib_HCompProjectedCurve;
class ProjLib_PrjResolve;
class ProjLib_PrjFunc;
class ProjLib_Projector;
class ProjLib_Plane;
class ProjLib_Cylinder;
class ProjLib_Cone;
class ProjLib_Sphere;
class ProjLib_Torus;


//! The  projLib package  first provides projection of
//! curves on a   plane along a  given Direction.  The
//! result will be a 3D curve.
//! The ProjLib package  provides projection of curves
//! on surfaces to compute the curve in the parametric
//! space.
//!
//! It is assumed that the curve is on the surface.
//!
//! It provides :
//!
//! * Package methods to handle the easiest cases :
//!
//! - Line, Circle, Ellipse, Parabola, Hyperbola on plane.
//!
//! - Line, Circle on cylinder.
//!
//! - Line, Circle on cone.
//!
//! * Classes to handle the general cases :
//!
//! - Plane.
//!
//! - Cylinder.
//!
//! - Cone.
//!
//! - Sphere.
//!
//! - Torus.
//!
//! * A generic  class to handle  a Curve from Adaptor3d
//! on a Surface from Adaptor3d.
class ProjLib 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static gp_Pnt2d Project (const gp_Pln& Pl, const gp_Pnt& P);
  
  Standard_EXPORT static gp_Lin2d Project (const gp_Pln& Pl, const gp_Lin& L);
  
  Standard_EXPORT static gp_Circ2d Project (const gp_Pln& Pl, const gp_Circ& C);
  
  Standard_EXPORT static gp_Elips2d Project (const gp_Pln& Pl, const gp_Elips& E);
  
  Standard_EXPORT static gp_Parab2d Project (const gp_Pln& Pl, const gp_Parab& P);
  
  Standard_EXPORT static gp_Hypr2d Project (const gp_Pln& Pl, const gp_Hypr& H);
  
  Standard_EXPORT static gp_Pnt2d Project (const gp_Cylinder& Cy, const gp_Pnt& P);
  
  Standard_EXPORT static gp_Lin2d Project (const gp_Cylinder& Cy, const gp_Lin& L);
  
  Standard_EXPORT static gp_Lin2d Project (const gp_Cylinder& Cy, const gp_Circ& Ci);
  
  Standard_EXPORT static gp_Pnt2d Project (const gp_Cone& Co, const gp_Pnt& P);
  
  Standard_EXPORT static gp_Lin2d Project (const gp_Cone& Co, const gp_Lin& L);
  
  Standard_EXPORT static gp_Lin2d Project (const gp_Cone& Co, const gp_Circ& Ci);
  
  Standard_EXPORT static gp_Pnt2d Project (const gp_Sphere& Sp, const gp_Pnt& P);
  
  Standard_EXPORT static gp_Lin2d Project (const gp_Sphere& Sp, const gp_Circ& Ci);
  
  Standard_EXPORT static gp_Pnt2d Project (const gp_Torus& To, const gp_Pnt& P);
  
  Standard_EXPORT static gp_Lin2d Project (const gp_Torus& To, const gp_Circ& Ci);

  //! Make empty  P-Curve <aC> of relevant to <PC> type
  Standard_EXPORT static void MakePCurveOfType (const ProjLib_ProjectedCurve& PC,
                                                Handle(Geom2d_Curve)& aC);



protected:





private:




friend class ProjLib_ProjectOnPlane;
friend class ProjLib_ProjectOnSurface;
friend class ProjLib_ComputeApprox;
friend class ProjLib_ComputeApproxOnPolarSurface;
friend class ProjLib_ProjectedCurve;
friend class ProjLib_HProjectedCurve;
friend class ProjLib_CompProjectedCurve;
friend class ProjLib_HCompProjectedCurve;
friend class ProjLib_PrjResolve;
friend class ProjLib_PrjFunc;
friend class ProjLib_Projector;
friend class ProjLib_Plane;
friend class ProjLib_Cylinder;
friend class ProjLib_Cone;
friend class ProjLib_Sphere;
friend class ProjLib_Torus;

};







#endif // _ProjLib_HeaderFile
