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


#include <GC_MakeCircle.hxx>
#include <gce_MakeCirc.hxx>
#include <Geom_Circle.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeCircle::GC_MakeCircle(const gp_Circ& C)
{
  TheError = gce_Done;
  TheCircle = new Geom_Circle(C);
}

GC_MakeCircle::GC_MakeCircle(const gp_Ax2&       A2    ,
			       const Standard_Real Radius)
{
  if (Radius < 0.) { TheError = gce_NegativeRadius; }
  else {
    TheError = gce_Done;
    TheCircle = new Geom_Circle(gp_Circ(A2,Radius));
  }
}

GC_MakeCircle::GC_MakeCircle(const gp_Circ& Circ  ,
			       const gp_Pnt&  Point ) 
{
  gp_Circ C = gce_MakeCirc(Circ,Point);
  TheCircle = new Geom_Circle(C);
  TheError = gce_Done;
}

GC_MakeCircle::GC_MakeCircle(const gp_Circ& Circ ,
			       const Standard_Real     Dist ) 
{
  gce_MakeCirc C = gce_MakeCirc(Circ,Dist);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom_Circle(C.Value());
  }
}

GC_MakeCircle::GC_MakeCircle(const gp_Pnt& P1 ,
			       const gp_Pnt& P2 ,
			       const gp_Pnt& P3 ) 
{
  gce_MakeCirc C = gce_MakeCirc(P1,P2,P3);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom_Circle(C.Value());
  }
}

GC_MakeCircle::GC_MakeCircle(const gp_Pnt& Point  ,
			       const gp_Dir& Norm   ,
			       const Standard_Real    Radius ) 
{
  gce_MakeCirc C = gce_MakeCirc(Point,Norm,Radius);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom_Circle(C.Value());
  }
}

GC_MakeCircle::GC_MakeCircle(const gp_Pnt&        Point  ,
			       const gp_Pnt&        PtAxis ,
			       const Standard_Real  Radius ) 
{
  gce_MakeCirc C = gce_MakeCirc(Point,PtAxis,Radius);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom_Circle(C.Value());
  }
}

GC_MakeCircle::GC_MakeCircle(const gp_Ax1& Axis   ,
			       const Standard_Real    Radius ) 
{
  gce_MakeCirc C = gce_MakeCirc(Axis,Radius);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom_Circle(C.Value());
  }
}

const Handle(Geom_Circle)& GC_MakeCircle::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GC_MakeCircle::Value() - no result");
  return TheCircle;
}
