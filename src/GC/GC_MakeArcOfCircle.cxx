// Created on: 1992-10-02
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
#include <Extrema_ExtElC.hxx>
#include <Extrema_POnCurv.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <gce_MakeCirc.hxx>
#include <gce_MakeLin.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GC_MakeArcOfCircle
//purpose  : 
//=======================================================================
GC_MakeArcOfCircle::GC_MakeArcOfCircle(const gp_Pnt&  P1 ,
				       const gp_Pnt&  P2 ,
				       const gp_Pnt&  P3  ) 
{
  Standard_Boolean sense;
  //
  gce_MakeCirc Cir(P1, P2, P3);
  TheError = Cir.Status();
  if (TheError == gce_Done) {
    Standard_Real Alpha1, Alpha3;//,Alpha2
    gp_Circ C(Cir.Value());
    //modified by NIZNHY-PKV Thu Mar  3 10:53:02 2005f
    //Alpha1 is always =0.
    //Alpha1 = ElCLib::Parameter(C,P1);
    //Alpha2 = ElCLib::Parameter(C,P2);
    //Alpha3 = ElCLib::Parameter(C,P3);
    //
    //if (Alpha2 >= Alpha1 && Alpha2 <= Alpha3) sense = Standard_True;
    //else if (Alpha1 <= Alpha3 && Alpha2 >= Alpha3 ) sense = Standard_True;
    //else sense = Standard_False;
    // 
    Alpha1=0.;
    Alpha3 = ElCLib::Parameter(C, P3);
    sense=Standard_True;
    //modified by NIZNHY-PKV Thu Mar  3 10:53:04 2005t
    
    Handle(Geom_Circle) Circ = new Geom_Circle(C);
    TheArc= new Geom_TrimmedCurve(Circ, Alpha1, Alpha3, sense);
  }
}

//=======================================================================
//function : GC_MakeArcOfCircle
//purpose  : 
//=======================================================================
GC_MakeArcOfCircle::GC_MakeArcOfCircle(const gp_Pnt& P1 ,
				       const gp_Vec& V  ,
				       const gp_Pnt& P2 )
{
  gp_Circ cir;
  gce_MakeLin Corde(P1,P2);
  TheError = Corde.Status();
  if (TheError == gce_Done) {
    gp_Lin corde(Corde.Value());
    gp_Dir dir(corde.Direction());
    gp_Dir dbid(V);
    gp_Dir Daxe(dbid^dir);
    gp_Dir Dir1(Daxe^dir);
    gp_Lin bis(gp_Pnt((P1.X()+P2.X())/2.,(P1.Y()+P2.Y())/2.,
		      (P1.Z()+P2.Z())/2.),Dir1);
    gp_Dir d(dbid^Daxe);
    gp_Lin norm(P1,d);
    Standard_Real Tol = 0.000000001;
    Extrema_ExtElC distmin(bis,norm,Tol);
    if (!distmin.IsDone()) { TheError = gce_IntersectionError; }
    else {
      Standard_Integer nbext = distmin.NbExt();
      if (nbext == 0) { TheError = gce_IntersectionError; }
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
	Standard_Real Rad = (pInt.Distance(P1)+pInt.Distance(P2))/2.;
	cir = gp_Circ(gp_Ax2(pInt,Daxe,d),Rad);
	Standard_Real Alpha1 = ElCLib::Parameter(cir,P1);
	Standard_Real Alpha3 = ElCLib::Parameter(cir,P2);
	Handle(Geom_Circle) Circ = new Geom_Circle(cir);
	TheArc= new Geom_TrimmedCurve(Circ,Alpha1,Alpha3, Standard_True);
      }
    }
  }
}
//=======================================================================
//function : GC_MakeArcOfCircle
//purpose  : 
//=======================================================================
GC_MakeArcOfCircle::GC_MakeArcOfCircle(const gp_Circ& Circ   ,
				       const gp_Pnt&  P1     ,
				       const gp_Pnt&  P2     ,
				       const Standard_Boolean  Sense  ) 
{
  Standard_Real Alpha1 = ElCLib::Parameter(Circ,P1);
  Standard_Real Alpha2 = ElCLib::Parameter(Circ,P2);
  Handle(Geom_Circle) C = new Geom_Circle(Circ);
  TheArc= new Geom_TrimmedCurve(C,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}
//=======================================================================
//function : GC_MakeArcOfCircle
//purpose  : 
//=======================================================================
GC_MakeArcOfCircle::GC_MakeArcOfCircle(const gp_Circ& Circ   ,
				       const gp_Pnt&  P      ,
				       const Standard_Real     Alpha  ,
				       const Standard_Boolean  Sense  ) 
{
  Standard_Real Alphafirst = ElCLib::Parameter(Circ,P);
  Handle(Geom_Circle) C = new Geom_Circle(Circ);
  TheArc= new Geom_TrimmedCurve(C,Alphafirst,Alpha,Sense);
  TheError = gce_Done;
}
//=======================================================================
//function : GC_MakeArcOfCircle
//purpose  : 
//=======================================================================
GC_MakeArcOfCircle::GC_MakeArcOfCircle(const gp_Circ& Circ   ,
					 const Standard_Real     Alpha1 ,
					 const Standard_Real     Alpha2 ,
					 const Standard_Boolean  Sense  ) 
{
  Handle(Geom_Circle) C = new Geom_Circle(Circ);
  TheArc= new Geom_TrimmedCurve(C,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
const Handle(Geom_TrimmedCurve)& GC_MakeArcOfCircle::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GC_MakeArcOfCircle::Value() - no result");
  return TheArc;
}
