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
#include <GCE2d_MakeLine.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

GCE2d_MakeSegment::GCE2d_MakeSegment(const gp_Pnt2d& P1 ,
				     const gp_Dir2d& V  ,
				     const gp_Pnt2d& P2 ) 
{
  gp_Lin2d Line(P1,V);
  Standard_Real Ulast = ElCLib::Parameter(Line,P2);
  if (Ulast != 0.0) {
    Handle(Geom2d_Line) L = new Geom2d_Line(Line);
    TheSegment = new Geom2d_TrimmedCurve(L,0.0,Ulast,Standard_True);
    TheError = gce_Done;
  }
  else { TheError = gce_ConfusedPoints; }
}

GCE2d_MakeSegment::GCE2d_MakeSegment(const gp_Pnt2d& P1 ,
				     const gp_Pnt2d& P2 ) 
{
  Standard_Real dist = P1.Distance(P2);
  if (dist != 0.0) {
    Handle(Geom2d_Line) L = GCE2d_MakeLine(P1,P2);
    TheSegment = new Geom2d_TrimmedCurve(L,0.,dist,Standard_True);
    TheError = gce_Done;
  }
  else { TheError = gce_ConfusedPoints; }
}
GCE2d_MakeSegment::GCE2d_MakeSegment(const gp_Lin2d&     Line  ,
				     const gp_Pnt2d&     Point ,
				     const Standard_Real U     ) 
{
  Standard_Real Ufirst = ElCLib::Parameter(Line,Point);
  Handle(Geom2d_Line) L = new Geom2d_Line(Line);
  TheSegment=new Geom2d_TrimmedCurve(L,Ufirst,U,Standard_True);
  TheError = gce_Done;
}

GCE2d_MakeSegment::GCE2d_MakeSegment(const gp_Lin2d& Line  ,
				     const gp_Pnt2d& P1    ,
				     const gp_Pnt2d& P2    ) 
{
  Standard_Real Ufirst = ElCLib::Parameter(Line,P1);
  Standard_Real Ulast = ElCLib::Parameter(Line,P2);
  Handle(Geom2d_Line) L = new Geom2d_Line(Line);
  TheSegment = new Geom2d_TrimmedCurve(L,Ufirst,Ulast,Standard_True);
  TheError = gce_Done;
}

GCE2d_MakeSegment::GCE2d_MakeSegment(const gp_Lin2d&     Line  ,
				     const Standard_Real U1    ,
				     const Standard_Real U2    ) 
{
  Handle(Geom2d_Line) L = new Geom2d_Line(Line);
  TheSegment = new Geom2d_TrimmedCurve(L,U1,U2,Standard_True);
  TheError = gce_Done;
}

const Handle(Geom2d_TrimmedCurve)& GCE2d_MakeSegment::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GCE2d_MakeSegment::Value() - no result");
  return TheSegment;
}
