// Created on: 1992-09-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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


#include <gce_MakeCylinder.ixx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>
#include <gp_Lin.hxx>

//=========================================================================
//  Constructions d un cylindre de gp par son Ax2 A2 et son rayon         +
//  Radius.                                                               +
//=========================================================================

gce_MakeCylinder::gce_MakeCylinder(const gp_Ax2&       A2     ,
				   const Standard_Real Radius ) 
{
  if (Radius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheCylinder = gp_Cylinder(A2,Radius);
    TheError = gce_Done;
  }
}

//=========================================================================
//  Constructions d un cylindre de gp par son axe Axis et son rayon       +
//  Radius.                                                               +
//=========================================================================

gce_MakeCylinder::gce_MakeCylinder(const gp_Ax1&       Axis   ,
				   const Standard_Real Radius ) 
{
  if (Radius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    gp_Dir D(Axis.Direction());
    gp_Dir Direc;
    Standard_Real x = D.X();
    Standard_Real y = D.Y();
    Standard_Real z = D.Z();
    if (Abs(x) > gp::Resolution()) { Direc = gp_Dir(-y,x,0.0); }
    else if (Abs(y) > gp::Resolution()) { Direc = gp_Dir(-y,x,0.0); }
    else if (Abs(z) > gp::Resolution()) { Direc = gp_Dir(0.0,-z,y); }
    TheCylinder = gp_Cylinder(gp_Ax2(Axis.Location(),D,Direc),Radius);
    TheError = gce_Done;
  }
}

//=========================================================================
//  Constructions d un cylindre de gp par un cercle.                      +
//=========================================================================

gce_MakeCylinder::gce_MakeCylinder(const gp_Circ& Circ ) 
{
  TheCylinder = gp_Cylinder(Circ.Position(),Circ.Radius());
  TheError = gce_Done;
}

//=========================================================================
//  Constructions d un cylindre de gp par trois points P1, P2, P3.        +
//  P1 et P2 donnent l axe du cylindre, la distance de P3 a l axe donne   +
//  le rayon du cylindre.                                                 +
//=========================================================================

gce_MakeCylinder::gce_MakeCylinder(const gp_Pnt& P1 ,
				   const gp_Pnt& P2 ,
				   const gp_Pnt& P3 ) 
{
  if (P1.Distance(P2) < gp::Resolution()) { TheError = gce_ConfusedPoints; }
  else {
    gp_Dir D1(P2.XYZ()-P1.XYZ());
    gp_Dir D2;
    Standard_Real x = D1.X();
    Standard_Real y = D1.Y();
    Standard_Real z = D1.Z();
    if (Abs(x) > gp::Resolution()) { D2 = gp_Dir(-y,x,0.0); }
    else if (Abs(y) > gp::Resolution()) { D2 = gp_Dir(-y,x,0.0); }
    else if (Abs(z) > gp::Resolution()) { D2 = gp_Dir(0.0,-z,y); }
    TheCylinder = gp_Cylinder(gp_Ax2(P1,D1,D2 ),gp_Lin(P1,D1).Distance(P3));
    TheError = gce_Done;
  }
}

//=========================================================================
//  Constructions d un cylindre de gp concentrique a un autre cylindre de +
//  gp a une distance Dist.                                               +
//=========================================================================

gce_MakeCylinder::gce_MakeCylinder(const gp_Cylinder&  Cyl  ,
				   const Standard_Real Dist ) 
{
  Standard_Real Rad = Cyl.Radius()+Dist;
  if (Rad < 0.) { TheError = gce_NegativeRadius; }
  else {
    TheCylinder = gp_Cylinder(Cyl);
    TheCylinder.SetRadius(Rad);
    TheError = gce_Done;
  }
}

//=========================================================================
//  Constructions d un cylindre de gp concentrique a un autre cylindre de +
//  gp passant par le point P.                                            +
//=========================================================================

gce_MakeCylinder::gce_MakeCylinder(const gp_Cylinder& Cyl ,
				   const gp_Pnt&      P   ) 
{
  gp_Lin L(Cyl.Axis());
  Standard_Real Rad = L.Distance(P);
  TheCylinder = gp_Cylinder(Cyl);
  TheCylinder.SetRadius(Rad);
  TheError = gce_Done;
}

const gp_Cylinder& gce_MakeCylinder::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheCylinder;
}

const gp_Cylinder& gce_MakeCylinder::Operator() const 
{
  return Value();
}

gce_MakeCylinder::operator gp_Cylinder() const
{
  return Value();
}





