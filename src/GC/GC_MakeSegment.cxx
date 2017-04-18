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
#include <GC_MakeLine.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeSegment::GC_MakeSegment(const gp_Pnt& P1 ,
				 const gp_Pnt& P2 ) 
{
  Standard_Real dist = P1.Distance(P2);
  Handle(Geom_Line) L = GC_MakeLine(P1,P2);
  TheSegment = new Geom_TrimmedCurve(L,0.,dist,Standard_True);
  TheError = gce_Done;
}

GC_MakeSegment::GC_MakeSegment(const gp_Lin& Line    ,
				 const gp_Pnt& Point   ,
				 const Standard_Real U ) 
{
  Standard_Real Ufirst = ElCLib::Parameter(Line,Point);
  Handle(Geom_Line) L = new Geom_Line(Line);
  TheSegment=new Geom_TrimmedCurve(L,Ufirst,U,Standard_True);
  TheError = gce_Done;
}

GC_MakeSegment::GC_MakeSegment(const gp_Lin& Line  ,
				 const gp_Pnt& P1    ,
				 const gp_Pnt& P2    ) 
{
  Standard_Real Ufirst = ElCLib::Parameter(Line,P1);
  Standard_Real Ulast = ElCLib::Parameter(Line,P2);
  Handle(Geom_Line) L = new Geom_Line(Line);
  TheSegment = new Geom_TrimmedCurve(L,Ufirst,Ulast,Standard_True);
  TheError = gce_Done;
}

GC_MakeSegment::GC_MakeSegment(const gp_Lin& Line  ,
				 const Standard_Real    U1    ,
				 const Standard_Real    U2    ) 
{
  Handle(Geom_Line) L = new Geom_Line(Line);
  TheSegment = new Geom_TrimmedCurve(L,U1,U2,Standard_True);
  TheError = gce_Done;
}

const Handle(Geom_TrimmedCurve)& GC_MakeSegment::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GC_MakeSegment::Value() - no result");
  return TheSegment;
}
