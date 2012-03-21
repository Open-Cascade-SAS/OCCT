// Created on: 1992-10-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <GC_MakeSegment.ixx>
#include <GC_MakeLine.hxx>
#include <Geom_Line.hxx>
#include <StdFail_NotDone.hxx>
#include <ElCLib.hxx>

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
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheSegment;
}

const Handle(Geom_TrimmedCurve)& GC_MakeSegment::Operator() const 
{
  return Value();
}

GC_MakeSegment::operator Handle(Geom_TrimmedCurve) () const
{
  return Value();
}

