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


#include <GCE2d_MakeSegment.ixx>
#include <GCE2d_MakeLine.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom2d_Line.hxx>
#include <ElCLib.hxx>

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
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheSegment;
}

const Handle(Geom2d_TrimmedCurve)& GCE2d_MakeSegment::Operator() const 
{
  return Value();
}

GCE2d_MakeSegment::operator Handle(Geom2d_TrimmedCurve) () const
{
  return Value();
}

