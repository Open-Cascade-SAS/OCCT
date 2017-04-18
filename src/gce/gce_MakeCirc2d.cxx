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


#include <ElCLib.hxx>
#include <gce_MakeCirc2d.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d un cercle 2d de gp passant par trois points.              +
//   Trois cas de figures :                                               +
//      1/ Les trois points sont confondus.                               +
//      -----------------------------------                               +
//      Le resultat est le cercle centre en Point1 de rayon zero.         +
//      2/ Deux des trois points sont confondus.                          +
//      ----------------------------------------                          +
//      On cree la mediatrice a deux points non confondus ainsi que la    +
//      droite passant par ces deux points.                               +
//      La solution a pour centre l intersection de ces deux droite et    +
//      pour rayon la distance entre ce centre et l un des trois points.  +
//      3/ Les trois points sont distinct.                                +
//      ----------------------------------                                +
//      On cree la mediatrice a P1P2 ainsi que la mediatrice a P1P3.      +
//      La solution a pour centre l intersection de ces deux droite et    +
//      pour rayon la distance entre ce centre et l un des trois points.  +
//=========================================================================
gce_MakeCirc2d::gce_MakeCirc2d(const gp_Pnt2d&  P1 ,
			       const gp_Pnt2d&  P2 ,
			       const gp_Pnt2d&  P3 )
{
  gp_Dir2d dirx(1.0,0.0);

//=========================================================================
//   Traitement.                                                          +
//=========================================================================

  Standard_Real dist1 = P1.Distance(P2);
  Standard_Real dist2 = P1.Distance(P3);
  Standard_Real dist3 = P2.Distance(P3);
  
  if ((dist1<gp::Resolution()) && (dist2<gp::Resolution()) && 
      (dist3<gp::Resolution())) {
    TheCirc2d = gp_Circ2d(gp_Ax2d(P1,dirx),0.0);
    TheError = gce_Done;
  }
  else {
    gp_Lin2d L1;
    gp_Lin2d L2;
    Standard_Real x1,y1,x2,y2,x3,y3;
    P1.Coord(x1,y1);
    P2.Coord(x2,y2);
    P3.Coord(x3,y3);
    if (dist1 >= RealEpsilon()) {
      L1 = gp_Lin2d(gp_Pnt2d((P1.XY()+P2.XY())/2.0),
		    gp_Dir2d(P1.Y()-P2.Y(),P2.X()-P1.X()));
    }
    if (dist2 >= RealEpsilon()) {
      L2 = gp_Lin2d(gp_Pnt2d((P1.XY()+P3.XY())/2.0),
		    gp_Dir2d(P1.Y()-P3.Y(),P3.X()-P1.X()));
    }
    if (dist2 <= RealEpsilon()) {
      L2 = gp_Lin2d(P1,gp_Dir2d(P1.Y()-P2.Y(),P2.X()-P1.X()));
    }
    else if (dist1 <= RealEpsilon()) {
      L1 = gp_Lin2d(P1,gp_Dir2d(P1.Y()-P3.Y(),P3.X()-P1.X()));
    }
    else if (dist3 <= RealEpsilon()) {
      L2 = gp_Lin2d(P1,gp_Dir2d(P1.Y()-P2.Y(),P2.X()-P1.X()));
    }
    IntAna2d_AnaIntersection Intp(L1,L2);
    if (Intp.IsDone()) {
      if (!Intp.IsEmpty()) {
	gp_Pnt2d pInt(Intp.Point(1).Value());
	dist1 = P1.Distance(pInt);
	dist2 = P2.Distance(pInt);
	dist3 = P3.Distance(pInt);
	Standard_Real xc,yc;
	pInt.Coord(xc,yc);
	gp_Dir2d d1(x1-xc,y1-yc);
	gp_Dir2d d2(xc-x3,yc-y3);
	TheCirc2d = gp_Circ2d(gp_Ax22d(pInt,d1,d2),(dist1+dist2+dist3)/3.);
	Standard_Real Alpha1 = ElCLib::Parameter(TheCirc2d,P1);
	Standard_Real Alpha2 = ElCLib::Parameter(TheCirc2d,P2);
	Standard_Real Alpha3 = ElCLib::Parameter(TheCirc2d,P3);
	if (!((Alpha1 <= Alpha2) && (Alpha2 <= Alpha3))) {
	  TheCirc2d.Reverse();
	}
	TheError = gce_Done;
      }
    }
    else {
      TheError = gce_IntersectionError;
    }
  }
}

//==========================================================================
//   Creation d un gp_Circ2d par son Axe <XAxis> et son rayon  <Radius>.   +
//==========================================================================

gce_MakeCirc2d::gce_MakeCirc2d(const gp_Ax2d&         XAxis   ,
			       const Standard_Real    Radius  ,
			       const Standard_Boolean Sense   )
{
  if (Radius >= 0.) {
    TheCirc2d = gp_Circ2d(XAxis,Radius,Sense);
    TheError = gce_Done;
  }
  else { 
    TheError = gce_NegativeRadius;
  }
}

//==========================================================================
//   Creation d un gp_Circ2d par son Repere <Axis> et son rayon  <Radius>. +
//==========================================================================

gce_MakeCirc2d::gce_MakeCirc2d(const gp_Ax22d&     Axis   ,
			       const Standard_Real Radius  )
{
  if (Radius >= 0.) {
    TheCirc2d = gp_Circ2d(Axis,Radius);
    TheError = gce_Done;
  }
  else { 
    TheError = gce_NegativeRadius;
  }
}

//==========================================================================
//   Creation d un gp_Circ2d par son centre <Center> et son rayon          +
//   <Radius>.                                                             +
//==========================================================================

gce_MakeCirc2d::gce_MakeCirc2d(const gp_Pnt2d&        Center  ,
			       const Standard_Real    Radius  ,
			       const Standard_Boolean Sense   ) 
{
  if (Radius >= 0.) {
    TheCirc2d = gp_Circ2d(gp_Ax2d(Center,gp_Dir2d(1.0,0.0)),Radius,Sense);
    TheError = gce_Done;
  }
  else { 
    TheError = gce_NegativeRadius;
  }
}

//==========================================================================
//   Creation d un gp_Circ2d par son centre <Center> et un point de sa     +
//   circonference <Point>.                                                +
//==========================================================================

gce_MakeCirc2d::gce_MakeCirc2d(const gp_Pnt2d&        Center ,
			       const gp_Pnt2d&        Point  , 
			       const Standard_Boolean Sense  ) 
{
  TheCirc2d = gp_Circ2d(gp_Ax2d(Center,gp_Dir2d(1.0,0.0)),
			Point.Distance(Center),Sense);
  TheError = gce_Done;
}

//==========================================================================
//   Creation d un cercle <TheCirc2d> concentrique a <Circ> passant par le +
//   point <Point1>.                                                       +
//==========================================================================

gce_MakeCirc2d::gce_MakeCirc2d(const gp_Circ2d& Circ  ,
			       const gp_Pnt2d&  Point ) 
{
  TheCirc2d = gp_Circ2d(Circ.Axis(),Point.Distance(Circ.Location()));
  TheError = gce_Done;
}

//==========================================================================
//   Creation d un cercle <TheCirc2d> concentrique a <Circ> a une distance +
//   <Dist1>.                                                              +
//==========================================================================

gce_MakeCirc2d::gce_MakeCirc2d(const gp_Circ2d&    Circ  ,
			       const Standard_Real Dist1 ) 
{
  TheCirc2d = gp_Circ2d(Circ.Axis(),Abs(Circ.Radius()+Dist1));
  TheError = gce_Done;
}

const gp_Circ2d& gce_MakeCirc2d::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "gce_MakeCirc2d::Value() - no result");
  return TheCirc2d;
}

const gp_Circ2d& gce_MakeCirc2d::Operator() const 
{
  return Value();
}

gce_MakeCirc2d::operator gp_Circ2d() const
{
  return Value();
}


