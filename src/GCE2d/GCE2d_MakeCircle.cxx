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


#include <GCE2d_MakeCircle.hxx>
#include <gce_MakeCirc2d.hxx>
#include <Geom2d_Circle.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

GCE2d_MakeCircle::GCE2d_MakeCircle(const gp_Circ2d& C)
{
  TheError = gce_Done;
  TheCircle = new Geom2d_Circle(C);
}

GCE2d_MakeCircle::GCE2d_MakeCircle(const gp_Ax2d&         A     ,
				   const Standard_Real    Radius,
				   const Standard_Boolean Sense )
{
  if (Radius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheError = gce_Done;
    TheCircle = new Geom2d_Circle(A,Radius,Sense);
  }
}

GCE2d_MakeCircle::GCE2d_MakeCircle(const gp_Ax22d&     A     ,
				   const Standard_Real Radius)
{
  if (Radius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheError = gce_Done;
    TheCircle = new Geom2d_Circle(A,Radius);
  }
}

GCE2d_MakeCircle::GCE2d_MakeCircle(const gp_Circ2d& Circ  ,
				   const gp_Pnt2d&  Point ) 
{
  gp_Circ2d C = gce_MakeCirc2d(Circ,Point);
  TheCircle = new Geom2d_Circle(C);
  TheError = gce_Done;
}

GCE2d_MakeCircle::GCE2d_MakeCircle(const gp_Circ2d&    Circ ,
				   const Standard_Real Dist ) 
{
  gce_MakeCirc2d C = gce_MakeCirc2d(Circ,Dist);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom2d_Circle(C.Value());
  }
}

GCE2d_MakeCircle::GCE2d_MakeCircle(const gp_Pnt2d& P1 ,
				   const gp_Pnt2d& P2 ,
				   const gp_Pnt2d& P3 ) 
{
  gce_MakeCirc2d C = gce_MakeCirc2d(P1,P2,P3);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom2d_Circle(C.Value());
  }

}

GCE2d_MakeCircle::GCE2d_MakeCircle(const gp_Pnt2d&        Point  ,
				   const Standard_Real    Radius ,
				   const Standard_Boolean Sense  ) 
{
  gce_MakeCirc2d C = gce_MakeCirc2d(Point,Radius,Sense);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom2d_Circle(C.Value());
  }
}

GCE2d_MakeCircle::GCE2d_MakeCircle(const gp_Pnt2d&        Center ,
				   const gp_Pnt2d&        Point  ,
				   const Standard_Boolean Sense  ) 
{
  gce_MakeCirc2d C = gce_MakeCirc2d(Center,Point,Sense);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom2d_Circle(C.Value());
  }
}

const Handle(Geom2d_Circle)& GCE2d_MakeCircle::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GCE2d_MakeCircle::Value() - no result");
  return TheCircle;
}
