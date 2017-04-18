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


#include <Extrema_ExtElC.hxx>
#include <Extrema_POnCurv.hxx>
#include <gce_MakeCirc.hxx>
#include <gce_MakeDir.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : gce_MakeCirc
//purpose  : 
//   Creation d un cercle 3d de gp passant par trois points.              +
//   Trois cas de figures :                                               +
//      1/ Les trois points sont confondus.                               +
//      -----------------------------------                               +
//      Le resultat est le cercle centre en Point1 de rayon zero.         +
//      2/ Deux des trois points sont confondus.                          +
//      ----------------------------------------                          +
//      Pas de solution (Erreur : Points confondus).                      +
//      3/ Les trois points sont distinct.                                +
//      ----------------------------------                                +
//      On cree la mediatrice a P1P2 ainsi que la mediatrice a P1P3.      +
//      La solution a pour centre l intersection de ces deux droite et    +
//      pour rayon la distance entre ce centre et l un des trois points.  +
//=========================================================================
gce_MakeCirc::gce_MakeCirc(const gp_Pnt&  P1 ,
			   const gp_Pnt&  P2 ,
			   const gp_Pnt&  P3) {
//=========================================================================
//   Traitement.                                                          +
//=========================================================================
  Standard_Real dist1, dist2, dist3, aResolution;
  //
  aResolution=gp::Resolution();
  //
  dist1 = P1.Distance(P2);
  dist2 = P1.Distance(P3);
  dist3 = P2.Distance(P3);
  //
  if ((dist1<aResolution) && (dist2<aResolution) && (dist3<aResolution)) {
    gp_Dir Dirx(1.,0.,0.);
    gp_Dir Dirz(0.,0.,1.);
    TheCirc = gp_Circ(gp_Ax2(P1,Dirx,Dirz), 0.);
    return;
  }
  if (!(dist1 >= aResolution && dist2 >= aResolution)) {
    TheError = gce_ConfusedPoints;
    return;
  }
  //
  Standard_Real x1,y1,z1,x2,y2,z2,x3,y3,z3;
  //
  P1.Coord(x1,y1,z1);
  P2.Coord(x2,y2,z2);
  P3.Coord(x3,y3,z3);
  gp_Dir Dir1(x2-x1,y2-y1,z2-z1);
  gp_Dir Dir2(x3-x2,y3-y2,z3-z2);
  //
  gp_Ax1 anAx1(P1, Dir1);
  gp_Lin aL12 (anAx1);
  if (aL12.Distance(P3)<aResolution) {
    TheError = gce_ColinearPoints;
    return;
  }
  //
  gp_Dir Dir3 = Dir1.Crossed(Dir2);
  //
  gp_Dir dir = Dir1.Crossed(Dir3);
  gp_Lin L1(gp_Pnt((P1.XYZ()+P2.XYZ())/2.),dir);
  dir = Dir2.Crossed(Dir3);
  gp_Lin L2(gp_Pnt((P3.XYZ()+P2.XYZ())/2.),dir);

  Standard_Real Tol = 0.000000001;
  Extrema_ExtElC distmin(L1,L2,Tol);
  
  if (!distmin.IsDone()) { 
    TheError = gce_IntersectionError; 
  }
  else {
    Standard_Integer nbext;
    //
    //
    if (distmin.IsParallel()) {
      TheError = gce_IntersectionError;
      return;
    }
    nbext = distmin.NbExt();
    //
    //
    if (nbext == 0) {
      TheError = gce_IntersectionError; 
    }
    else {
      Standard_Real TheDist = RealLast();
      gp_Pnt pInt,pon1,pon2;
      Standard_Integer i = 1;
      Extrema_POnCurv Pon1,Pon2;
      while (i<=nbext) {
	if (distmin.SquareDistance(i)<TheDist) {
	  TheDist = distmin.SquareDistance(i);
	  distmin.Points(i,Pon1,Pon2);
	  pon1 = Pon1.Value();
	  pon2 = Pon2.Value();
	  pInt = gp_Pnt((pon1.XYZ()+pon2.XYZ())/2.);
	}
	i++;
      }
      //modified by NIZNHY-PKV Thu Mar  3 11:30:34 2005f
      //Dir2.Cross(Dir1);
      //modified by NIZNHY-PKV Thu Mar  3 11:30:37 2005t
      dist1 = P1.Distance(pInt);
      dist2 = P2.Distance(pInt);
      dist3 = P3.Distance(pInt);
      pInt.Coord(x3,y3,z3);
      Dir1 = gp_Dir(x1-x3,y1-y3,z1-z3);
      //modified by NIZNHY-PKV Thu Mar  3 11:31:11 2005f
      //Dir2 = gp_Dir(x2-x3,y2-y3,z2-z3);
      //modified by NIZNHY-PKV Thu Mar  3 11:31:13 2005t
      //
      TheCirc = gp_Circ(gp_Ax2(pInt, Dir3, Dir1),(dist1+dist2+dist3)/3.);
      TheError = gce_Done;
    }
  }
}
//=======================================================================
//function : gce_MakeCirc
//purpose  : 
//=======================================================================
gce_MakeCirc::gce_MakeCirc(const gp_Ax2&       A2      ,
			   const Standard_Real Radius  ) {
  if (Radius < 0.) { 
    TheError = gce_NegativeRadius;
  }
  else {
    TheError = gce_Done;
    TheCirc = gp_Circ(A2,Radius);
  }
}
//=========================================================================
//   Creation d un gp_Circ par son centre <Center>, son plan <Plane> et   +
//   son rayon <Radius>.                                                  +
//=========================================================================
gce_MakeCirc::gce_MakeCirc(const gp_Pnt&       Center  ,
			   const gp_Pln&       Plane   ,
			   const Standard_Real Radius  ) {
  gce_MakeCirc C = gce_MakeCirc(Center,Plane.Position().Direction(),Radius);
  TheCirc = C.Value();
  TheError = C.Status();
}
//=======================================================================
//function : gce_MakeCirc
//purpose  : Creation d un gp_Circ par son centre <Center>, 
//sa normale <Norm> et son rayon <Radius>.  
//=======================================================================
gce_MakeCirc::gce_MakeCirc(const gp_Pnt&       Center  ,
			   const gp_Dir&       Norm    ,
			   const Standard_Real Radius  ) {
   if (Radius < 0.) { 
     TheError = gce_NegativeRadius;
   }
   else {
     Standard_Real A = Norm.X();
     Standard_Real B = Norm.Y();
     Standard_Real C = Norm.Z();
     Standard_Real Aabs = Abs(A);
     Standard_Real Babs = Abs(B);
     Standard_Real Cabs = Abs(C);
     gp_Ax2 Pos;

//=========================================================================
//  pour determiner l'axe X :                                             +
//  on dit que le produit scalaire Vx.Norm = 0.                           +
//  et on recherche le max(A,B,C) pour faire la division.                 +
//  l'une des coordonnees du vecteur est nulle.                           +
//=========================================================================

     if( Babs <= Aabs && Babs <= Cabs) {
       if(Aabs > Cabs) { Pos = gp_Ax2(Center,Norm,gp_Dir(-C,0.,A)); }
       else { Pos = gp_Ax2(Center,Norm,gp_Dir(C,0.,-A)); }
     }
     else if( Aabs <= Babs && Aabs <= Cabs) {
       if(Babs > Cabs) { Pos = gp_Ax2(Center,Norm,gp_Dir(0.,-C,B)); }
       else { Pos = gp_Ax2(Center,Norm,gp_Dir(0.,C,-B)); }
     }
     else {
       if(Aabs > Babs) { Pos = gp_Ax2(Center,Norm,gp_Dir(-B,A,0.)); }
       else { Pos = gp_Ax2(Center,Norm,gp_Dir(B,-A,0.)); }
     }
     TheCirc = gp_Circ(Pos,Radius);
     TheError = gce_Done;
   }
 }
//=======================================================================
//function : gce_MakeCirc
//purpose  :  Creation d un gp_Circ par son centre <Center>, 
// sa normale <Ptaxis> et  son rayon <Radius>
//=======================================================================
gce_MakeCirc::gce_MakeCirc(const gp_Pnt&       Center  ,
			   const gp_Pnt&       Ptaxis  ,
			   const Standard_Real Radius  ) {
   if (Radius < 0.) { 
     TheError = gce_NegativeRadius;
   }
   else {
     if (Center.Distance(Ptaxis) <= gp::Resolution()) {
       TheError = gce_NullAxis;
     }
     else {
       Standard_Real A = Ptaxis.X()-Center.X();
       Standard_Real B = Ptaxis.Y()-Center.Y();
       Standard_Real C = Ptaxis.Z()-Center.Z();
       Standard_Real Aabs = Abs(A);
       Standard_Real Babs = Abs(B);
       Standard_Real Cabs = Abs(C);
       gp_Ax2 Pos;

//=========================================================================
//  pour determiner l'axe X :                                             +
//  on dit que le produit scalaire Vx.Norm = 0.                           +
//  et on recherche le max(A,B,C) pour faire la division.                 +
//  l'une des coordonnees du vecteur est nulle.                           +
//=========================================================================

       gp_Dir Norm = gce_MakeDir(Center,Ptaxis);
       if( Babs <= Aabs && Babs <= Cabs) {
	 if(Aabs > Cabs) { Pos = gp_Ax2(Center,Norm,gp_Dir(-C,0.,A)); }
	 else { Pos = gp_Ax2(Center,Norm,gp_Dir(C,0.,-A)); }
       }
       else if( Aabs <= Babs && Aabs <= Cabs) {
	 if(Babs > Cabs) { Pos = gp_Ax2(Center,Norm,gp_Dir(0.,-C,B)); }
	 else { Pos = gp_Ax2(Center,Norm,gp_Dir(0.,C,-B)); }
       }
       else {
	 if(Aabs > Babs) { Pos = gp_Ax2(Center,Norm,gp_Dir(-B,A,0.)); }
	 else { Pos = gp_Ax2(Center,Norm,gp_Dir(B,-A,0.)); }
       }
       TheCirc = gp_Circ(Pos,Radius);
       TheError = gce_Done;
     }
   }
 }
//=======================================================================
//function : gce_MakeCirc
//purpose  : Creation d un gp_Circ par son axe <Axis> et son rayon <Radius>.  
//=======================================================================
gce_MakeCirc::gce_MakeCirc(const gp_Ax1&       Axis    ,
			   const Standard_Real Radius  ) 
{
  if (Radius < 0.) { 
    TheError = gce_NegativeRadius;
  }
  else {
    gp_Dir Norm(Axis.Direction());
    gp_Pnt Center(Axis.Location());
    Standard_Real A = Norm.X();
    Standard_Real B = Norm.Y();
    Standard_Real C = Norm.Z();
    Standard_Real Aabs = Abs(A);
    Standard_Real Babs = Abs(B);
    Standard_Real Cabs = Abs(C);
    gp_Ax2 Pos;

//=========================================================================
//  pour determiner l'axe X :                                             +
//  on dit que le produit scalaire Vx.Norm = 0.                           +
//  et on recherche le max(A,B,C) pour faire la division.                 +
//  l'une des coordonnees du vecteur est nulle.                           +
//=========================================================================

    if( Babs <= Aabs && Babs <= Cabs) {
      if(Aabs > Cabs) { Pos = gp_Ax2(Center,Norm,gp_Dir(-C,0.,A)); }
      else { Pos = gp_Ax2(Center,Norm,gp_Dir(C,0.,-A)); }
    }
    else if( Aabs <= Babs && Aabs <= Cabs) {
      if(Babs > Cabs) { Pos = gp_Ax2(Center,Norm,gp_Dir(0.,-C,B)); }
      else { Pos = gp_Ax2(Center,Norm,gp_Dir(0.,C,-B)); }
    }
    else {
      if(Aabs > Babs) { Pos = gp_Ax2(Center,Norm,gp_Dir(-B,A,0.)); }
      else { Pos = gp_Ax2(Center,Norm,gp_Dir(B,-A,0.)); }
    }
    TheCirc = gp_Circ(Pos,Radius);
    TheError = gce_Done;
  }
}
//=======================================================================
//function : gce_MakeCirc
//purpose  : Creation d un gp_Circ concentrique a un autre gp_circ a une distance +
//   donnee.                
//=======================================================================
gce_MakeCirc::gce_MakeCirc(const gp_Circ&      Circ    ,
			   const Standard_Real Dist    )
{
  Standard_Real Rad = Circ.Radius()+Dist;
  if (Rad < 0.) { 
    TheError = gce_NegativeRadius;
  }
  else {
    TheCirc = gp_Circ(Circ.Position(),Rad);
    TheError = gce_Done;
  }
}
//=======================================================================
//function : gce_MakeCirc
//purpose  : Creation d un gp_Circ concentrique a un autre gp_circ dont le rayon 
//   est egal a la distance de <Point> a l axe de <Circ>. 
//=======================================================================
gce_MakeCirc::gce_MakeCirc(const gp_Circ& Circ ,
			   const gp_Pnt&  P    ) 
{
  Standard_Real Rad = gp_Lin(Circ.Axis()).Distance(P);
  TheCirc = gp_Circ(Circ.Position(),Rad);
  TheError = gce_Done;
}
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
const gp_Circ& gce_MakeCirc::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "gce_MakeCirc::Value() - no result");
  return TheCirc;
}
//=======================================================================
//function : Operator
//purpose  : 
//=======================================================================
const gp_Circ& gce_MakeCirc::Operator() const 
{
  return Value();
}
//=======================================================================
//function : operator
//purpose  : 
//=======================================================================
gce_MakeCirc::operator gp_Circ() const
{
  return Value();
}

