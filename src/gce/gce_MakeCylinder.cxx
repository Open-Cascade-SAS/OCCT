// Created on: 1992-09-02
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


#include <gce_MakeCylinder.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

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
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "gce_MakeCylinder::Value() - no result");
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





