// Created on: 1993-08-24
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <ProjLib.ixx>
#include <ProjLib_Plane.hxx>
#include <ProjLib_Cylinder.hxx>
#include <ProjLib_Cone.hxx>
#include <ProjLib_Sphere.hxx>
#include <ProjLib_Torus.hxx>

#include <ElSLib.hxx>

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
