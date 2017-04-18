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


#include <gce_MakeCone.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Cone.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//  Construction d un cone par son axe , le rayon de sa base et le demi   +
//  angle d ouverture.                                                    +
//=========================================================================
gce_MakeCone::gce_MakeCone(const gp_Ax2&       A2    ,
			   const Standard_Real Ang   ,
			   const Standard_Real Radius)
{
  if (Radius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    if (Ang <= gp::Resolution() || M_PI/2-Ang <= gp::Resolution()) {
      TheError = gce_BadAngle;
    }
    else {
      TheError = gce_Done;
      TheCone = gp_Cone(A2,Ang,Radius);
    }
  }
}

//=========================================================================
//  Constructions d un cone de gp par quatre points P1, P2, P3 et P4.     +
//  P1 et P2 donnent l axe du cone, la distance de P3 a l axe donne       +
//  le rayon de la base du cone et la distance de P4 a l axe donne le     +
//  rayon du cone pour la section passant par P4.                         +
//=========================================================================

gce_MakeCone::gce_MakeCone(const gp_Pnt& P1 ,
			   const gp_Pnt& P2 ,
			   const gp_Pnt& P3 ,
			   const gp_Pnt& P4 ) 
{
  if (P1.Distance(P2)<RealEpsilon() || P3.Distance(P4)<RealEpsilon()) { TheError = gce_ConfusedPoints; return;  }

  gp_Dir D1(P2.XYZ()-P1.XYZ());
  Standard_Real cos = D1.Dot(gp_Dir(P4.XYZ()-P1.XYZ()));
  Standard_Real dist = P1.Distance(P4);
  gp_Pnt PP4(P1.XYZ()+cos*dist*D1.XYZ());
  cos = D1.Dot(gp_Dir(P3.XYZ()-P1.XYZ()));
  dist = P1.Distance(P3);
  gp_Pnt PP3(P1.XYZ()+cos*dist*D1.XYZ());
  
  Standard_Real Dist13 = PP3.Distance(P1);
  Standard_Real Dist14 = PP4.Distance(P1);
  if(Abs(Dist13-Dist14)<RealEpsilon()) { TheError = gce_NullAngle;  return; }
  gp_Lin L1(P1,D1);
  Standard_Real Dist3 = L1.Distance(P3);
  Standard_Real Dist4 = L1.Distance(P4);
  Standard_Real DifRad = Dist3-Dist4;
  Standard_Real angle = Abs(ATan(DifRad/(Dist13-Dist14)));
  if(Abs(M_PI/2.-angle) < RealEpsilon() || Abs(angle) < RealEpsilon()) { TheError = gce_NullRadius; return; }
  Standard_Real R1 = PP3.Distance(P3);
  Standard_Real R2 = PP4.Distance(P4);
  if (R1 < 0.0 || R2 < 0.0) { TheError = gce_NegativeRadius; return; }
  gp_Dir DD1(PP4.XYZ()-PP3.XYZ());
  gp_Dir D2;
  Standard_Real x = DD1.X();
  Standard_Real y = DD1.Y();
  Standard_Real z = DD1.Z();
  if (Abs(x) > gp::Resolution()) { D2 = gp_Dir(-y,x,0.0); }
  else if (Abs(y) > gp::Resolution()) { D2 = gp_Dir(-y,x,0.0); }
  else if (Abs(z) > gp::Resolution()) { D2 = gp_Dir(0.0,-z,y); }
  if (R1 > R2) { angle *= -1; }
  TheCone = gp_Cone(gp_Ax2(PP3,DD1,D2),angle,R1);
  TheError = gce_Done;
}
 
 

//=========================================================================
//  Constructions d un cone de gp par son axe et deux points P1, P2.      +
//  La distance de P1 a l axe donne le rayon de la base du cone et la     +
//  distance de P2 a l axe donne le rayon du cone pour la section passant +
//  par P2.                                                               +
//=========================================================================

gce_MakeCone::gce_MakeCone(const gp_Ax1&   Axis  ,
			   const gp_Pnt&   P1    ,
			   const gp_Pnt&   P2    ) 
{
  gp_Pnt P3(Axis.Location());
  gp_Pnt P4(P3.XYZ()+Axis.Direction().XYZ());
  gce_MakeCone Cone(P3,P4,P1,P2);
  if (Cone.IsDone()) {
    TheCone = Cone.Value();
    TheError = gce_Done;
  }
  else { 
    TheError = Cone.Status();
  }
}

//=========================================================================
//  Constructions d un cone parallele a un autre cone passant par un      +
//  donne.                                                                +
//=========================================================================

//gce_MakeCone::gce_MakeCone(const gp_Cone&  cone ,
//			   const gp_Pnt&   P    ) 
gce_MakeCone::gce_MakeCone(const gp_Cone&   ,
			   const gp_Pnt&       ) 
{
  TheError = gce_ConfusedPoints;
}

//=========================================================================
//  Constructions d un cone parallele a un autre cone a une distance      +
//  donnee.                                                               +
//=========================================================================

//gce_MakeCone::gce_MakeCone(const gp_Cone&      cone ,
//			   const Standard_Real Dist ) 
gce_MakeCone::gce_MakeCone(const gp_Cone&       ,
			   const Standard_Real  ) 
{
  TheError = gce_Done;
}

//=========================================================================
//  Constructions d un cone de gp par son axe et deux points P1, P2.      +
//  La distance de P1 a l axe donne le rayon de la base du cone et la     +
//  distance de P2 a l axe donne le rayon du cone pour la section passant +
//  par P2.                                                               +
//=========================================================================

gce_MakeCone::gce_MakeCone(const gp_Lin&   Axis  ,
			   const gp_Pnt&   P1    ,
			   const gp_Pnt&   P2    ) 
{
  gp_Pnt P3(Axis.Location());
  gp_Pnt P4(P3.XYZ()+Axis.Direction().XYZ());
  gce_MakeCone Cone(P3,P4,P1,P2);
  if (Cone.IsDone()) {
    TheCone = Cone.Value();
    TheError = gce_Done;
  }
  else { TheError = Cone.Status(); }
}

//=========================================================================
//  cone par deux points (axe du cone.) et deux rayons (rayon des         +
//  sections passant par chacun de ces points).                           +
//=========================================================================

gce_MakeCone::gce_MakeCone(const gp_Pnt&       P1   ,
			   const gp_Pnt&       P2   ,
			   const Standard_Real R1   ,
			   const Standard_Real R2   ) 
{
  Standard_Real dist = P1.Distance(P2);
  if (dist < RealEpsilon()) { TheError = gce_NullAxis; }
  else {
    if (R1 < 0.0 || R2 < 0.0) {
      TheError = gce_NegativeRadius;
    }
    else {
      Standard_Real Angle = Abs(atan((R1-R2)/dist));
      if (Abs(M_PI/2.-Angle)<RealEpsilon() || Abs(Angle)<RealEpsilon()) {
	TheError = gce_NullAngle;
      }
      else {
	gp_Dir D1(P2.XYZ()-P1.XYZ());
	gp_Dir D2;
	Standard_Real x = D1.X();
	Standard_Real y = D1.Y();
	Standard_Real z = D1.Z();
	if (Abs(x) > gp::Resolution()) { D2 = gp_Dir(-y,x,0.0); }
	else if (Abs(y) > gp::Resolution()) { D2 = gp_Dir(-y,x,0.0); }
	else if (Abs(z) > gp::Resolution()) { D2 = gp_Dir(0.0,-z,y); }
	if (R1 > R2) { Angle *= -1; }
	TheCone = gp_Cone(gp_Ax2(P1,D1,D2),Angle,R1);
	TheError = gce_Done;
      }
    }
  }
}

const gp_Cone& gce_MakeCone::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "gce_MakeCone::Value() - no result");
  return TheCone;
}

const gp_Cone& gce_MakeCone::Operator() const 
{
  return Value();
}

gce_MakeCone::operator gp_Cone() const
{
  return Value();
}

