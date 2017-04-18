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
#include <GCE2d_MakeArcOfCircle.hxx>
#include <gce_MakeCirc2d.hxx>
#include <gce_MakeLin2d.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <StdFail_NotDone.hxx>

GCE2d_MakeArcOfCircle::GCE2d_MakeArcOfCircle(const gp_Pnt2d&        P1     ,
					     const gp_Pnt2d&        P2     ,
					     const gp_Pnt2d&        P3     ) 
{
  gce_MakeCirc2d Cir = gce_MakeCirc2d(P1,P2,P3);
  TheError = Cir.Status();
  if (TheError == gce_Done) {
    gp_Circ2d C(Cir.Value());
    Standard_Real Alpha1 = ElCLib::Parameter(C,P1);
    Standard_Real Alpha2 = ElCLib::Parameter(C,P3);
    Handle(Geom2d_Circle) Circ = new Geom2d_Circle(C);
    TheArc= new Geom2d_TrimmedCurve(Circ,Alpha1,Alpha2,Standard_True);
  }
}

GCE2d_MakeArcOfCircle::GCE2d_MakeArcOfCircle(const gp_Pnt2d& P1 ,
					     const gp_Vec2d& V  ,
					     const gp_Pnt2d& P2 )
{
  Standard_Boolean Sense;
  gp_Circ2d cir;
  gp_Lin2d corde = gce_MakeLin2d(P1,P2);
  gp_Dir2d dir(corde.Direction());
  gp_Lin2d bis(gp_Pnt2d((P1.X()+P2.X())/2.,(P1.Y()+P2.Y())/2.),
	       gp_Dir2d(-dir.Y(),dir.X()));
  gp_Lin2d norm(P1,gp_Dir2d(-V.Y(),V.X()));
  TheError = gce_ConfusedPoints;

  IntAna2d_AnaIntersection Intp(bis,norm);

  if (Intp.IsDone())
    {
      if (!Intp.IsEmpty())
	{
	  gp_Pnt2d center(Intp.Point(1).Value());
	  Standard_Real rad = (center.Distance(P1)+center.Distance(P2))/2.;
	  cir = gce_MakeCirc2d(center,rad);
	  TheError = gce_Done;
	}
    }

  if (TheError == gce_Done) {
    Standard_Real Alpha1 = ElCLib::Parameter(cir,P1);
    Standard_Real Alpha2 = ElCLib::Parameter(cir,P2);
    Handle(Geom2d_Circle) Circ = new Geom2d_Circle(cir);
    gp_Vec2d vv(dir);
    Standard_Real cross = V^vv;
    Sense = cross > 0.;
    TheArc= new Geom2d_TrimmedCurve(Circ,Alpha1,Alpha2,Sense);
  }
}

GCE2d_MakeArcOfCircle::GCE2d_MakeArcOfCircle(const gp_Circ2d&        Circ   ,
					     const gp_Pnt2d&         P1     ,
					     const gp_Pnt2d&         P2     ,
					     const Standard_Boolean  Sense  ) 
{
  Standard_Real Alpha1 = ElCLib::Parameter(Circ,P1);
  Standard_Real Alpha2 = ElCLib::Parameter(Circ,P2);
  Handle(Geom2d_Circle) C = new Geom2d_Circle(Circ);
  TheArc= new Geom2d_TrimmedCurve(C,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

GCE2d_MakeArcOfCircle::GCE2d_MakeArcOfCircle(const gp_Circ2d&       Circ  ,
					     const gp_Pnt2d&        P     ,
					     const Standard_Real    Alpha ,
					     const Standard_Boolean Sense ) 
{
  Standard_Real Alphafirst = ElCLib::Parameter(Circ,P);
  Handle(Geom2d_Circle) C = new Geom2d_Circle(Circ);
  TheArc= new Geom2d_TrimmedCurve(C,Alphafirst,Alpha,Sense);
  TheError = gce_Done;
}

GCE2d_MakeArcOfCircle::GCE2d_MakeArcOfCircle(const gp_Circ2d&       Circ   ,
					     const Standard_Real    Alpha1 ,
					     const Standard_Real    Alpha2 ,
					     const Standard_Boolean Sense  ) 
{
  Handle(Geom2d_Circle) C = new Geom2d_Circle(Circ);
  TheArc= new Geom2d_TrimmedCurve(C,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

const Handle(Geom2d_TrimmedCurve)& GCE2d_MakeArcOfCircle::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GCE2d_MakeArcOfCircle::Value() - no result");
  return TheArc;
}
