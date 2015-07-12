// Created on: 1992-04-13
// Created by: Jacques GOUSSARD
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

#ifndef _IntSurf_Quadric_HeaderFile
#define _IntSurf_Quadric_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>
#include <Standard_Integer.hxx>
class gp_Pln;
class gp_Cylinder;
class gp_Sphere;
class gp_Cone;
class gp_Torus;
class gp_Pnt;
class gp_Vec;



class IntSurf_Quadric 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntSurf_Quadric();
  
  Standard_EXPORT IntSurf_Quadric(const gp_Pln& P);
  
  Standard_EXPORT IntSurf_Quadric(const gp_Cylinder& C);
  
  Standard_EXPORT IntSurf_Quadric(const gp_Sphere& S);
  
  Standard_EXPORT IntSurf_Quadric(const gp_Cone& C);
  
  Standard_EXPORT IntSurf_Quadric(const gp_Torus& T);
  
  Standard_EXPORT void SetValue (const gp_Pln& P);
  
  Standard_EXPORT void SetValue (const gp_Cylinder& C);
  
  Standard_EXPORT void SetValue (const gp_Sphere& S);
  
  Standard_EXPORT void SetValue (const gp_Cone& C);
  
  Standard_EXPORT void SetValue (const gp_Torus& T);
  
  Standard_EXPORT Standard_Real Distance (const gp_Pnt& P) const;
  
  Standard_EXPORT gp_Vec Gradient (const gp_Pnt& P) const;
  
  Standard_EXPORT void ValAndGrad (const gp_Pnt& P, Standard_Real& Dist, gp_Vec& Grad) const;
  
    GeomAbs_SurfaceType TypeQuadric() const;
  
    gp_Pln Plane() const;
  
    gp_Sphere Sphere() const;
  
    gp_Cylinder Cylinder() const;
  
    gp_Cone Cone() const;
  
    gp_Torus Torus() const;
  
  Standard_EXPORT gp_Pnt Value (const Standard_Real U, const Standard_Real V) const;
  
  Standard_EXPORT void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const;
  
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const;
  
  Standard_EXPORT gp_Vec Normale (const Standard_Real U, const Standard_Real V) const;
  
  Standard_EXPORT void Parameters (const gp_Pnt& P, Standard_Real& U, Standard_Real& V) const;
  
  Standard_EXPORT gp_Vec Normale (const gp_Pnt& P) const;




protected:





private:



  gp_Ax3 ax3;
  gp_Lin lin;
  GeomAbs_SurfaceType typ;
  Standard_Real prm1;
  Standard_Real prm2;
  Standard_Real prm3;
  Standard_Real prm4;
  Standard_Boolean ax3direc;


};


#include <IntSurf_Quadric.lxx>





#endif // _IntSurf_Quadric_HeaderFile
