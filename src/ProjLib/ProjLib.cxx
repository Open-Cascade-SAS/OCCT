// Created on: 1993-08-24
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


#include <ElSLib.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <ProjLib.hxx>
#include <ProjLib_Cone.hxx>
#include <ProjLib_Cylinder.hxx>
#include <ProjLib_Plane.hxx>
#include <ProjLib_Sphere.hxx>
#include <ProjLib_Torus.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Standard_NotImplemented.hxx>

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================
gp_Pnt2d  ProjLib::Project(const gp_Pln& Pl, const gp_Pnt& P)
{
  Standard_Real U, V;
  ElSLib::Parameters(Pl, P, U, V);
  return gp_Pnt2d(U,V);
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Lin2d  ProjLib::Project(const gp_Pln& Pl, const gp_Lin& L)
{
  ProjLib_Plane Proj( Pl, L);
  return Proj.Line();
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Circ2d  ProjLib::Project(const gp_Pln& Pl, const gp_Circ& C)
{
  ProjLib_Plane Proj( Pl, C);
  return Proj.Circle();
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Elips2d  ProjLib::Project(const gp_Pln& Pl, const gp_Elips& E)
{
  ProjLib_Plane Proj( Pl, E);
  return Proj.Ellipse();
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Parab2d  ProjLib::Project(const gp_Pln& Pl, const gp_Parab& P)
{
  ProjLib_Plane Proj( Pl, P);
  return Proj.Parabola();
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Hypr2d  ProjLib::Project(const gp_Pln& Pl, const gp_Hypr& H)
{
  ProjLib_Plane Proj( Pl, H);
  return Proj.Hyperbola();
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Pnt2d  ProjLib::Project(const gp_Cylinder& Cy, const gp_Pnt& P)
{
  Standard_Real U, V;
  ElSLib::Parameters(Cy, P, U, V);
  return gp_Pnt2d(U,V);
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Lin2d  ProjLib::Project(const gp_Cylinder& Cy, const gp_Lin& L)
{
  ProjLib_Cylinder Proj( Cy, L);
  return Proj.Line();
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Lin2d  ProjLib::Project(const gp_Cylinder& Cy, const gp_Circ& Ci)
{
  ProjLib_Cylinder Proj( Cy, Ci);
  return Proj.Line();
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Pnt2d  ProjLib::Project(const gp_Cone& Co, const gp_Pnt& P)
{
  Standard_Real U, V;
  ElSLib::Parameters(Co, P, U, V);
  return gp_Pnt2d(U,V);
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Lin2d  ProjLib::Project(const gp_Cone& Co, const gp_Lin& L)
{
  ProjLib_Cone Proj( Co, L);
  return Proj.Line();
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Lin2d  ProjLib::Project(const gp_Cone& Co, const gp_Circ& Ci)
{
  ProjLib_Cone Proj( Co, Ci);
  return Proj.Line();
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Pnt2d  ProjLib::Project(const gp_Sphere& Sp, const gp_Pnt& P)
{
  Standard_Real U, V;
  ElSLib::Parameters(Sp, P, U, V);
  return gp_Pnt2d(U,V);
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Lin2d  ProjLib::Project(const gp_Sphere& Sp, const gp_Circ& Ci)
{
  ProjLib_Sphere Proj( Sp, Ci);
  return Proj.Line();
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Pnt2d  ProjLib::Project(const gp_Torus& To, const gp_Pnt& P)
{
  Standard_Real U, V;
  ElSLib::Parameters(To, P, U, V);
  return gp_Pnt2d(U,V);
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

gp_Lin2d  ProjLib::Project(const gp_Torus& To, const gp_Circ& Ci)
{
  ProjLib_Torus Proj( To, Ci);
  return Proj.Line();
}

//=======================================================================
//function : MakePCurveOfType
//purpose  : 
//=======================================================================
void  ProjLib::MakePCurveOfType
  (const ProjLib_ProjectedCurve& PC, 
   Handle(Geom2d_Curve)& C2D)
{
  
  switch (PC.GetType()) {

  case GeomAbs_Line : 
    C2D = new Geom2d_Line(PC.Line()); 
    break;
  case GeomAbs_Circle : 
    C2D = new Geom2d_Circle(PC.Circle());
    break;
  case GeomAbs_Ellipse :
    C2D = new Geom2d_Ellipse(PC.Ellipse());
    break;
  case GeomAbs_Parabola : 
    C2D = new Geom2d_Parabola(PC.Parabola()); 
    break;
  case GeomAbs_Hyperbola : 
    C2D = new Geom2d_Hyperbola(PC.Hyperbola()); 
    break;
  case GeomAbs_BSplineCurve :
    C2D = PC.BSpline(); 
    break;
  case GeomAbs_BezierCurve : 
  case GeomAbs_OtherCurve : 
    default :
    Standard_NotImplemented::Raise
      ("ProjLib::MakePCurveOfType");
    break;
  }
}
