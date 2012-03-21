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


#include <GC_MakeLine.ixx>
#include <gce_MakeLin.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Constructions of 3d geometrical elements from Geom.
//=========================================================================

GC_MakeLine::GC_MakeLine(const gp_Pnt& P ,
			   const gp_Dir& V )
{
  TheError = gce_Done;
  TheLine = new Geom_Line(P,V);
}

GC_MakeLine::GC_MakeLine(const gp_Ax1& A1 )
{
  TheError = gce_Done;
  TheLine = new Geom_Line(A1);
}

GC_MakeLine::GC_MakeLine(const gp_Lin& L )
{
  TheError = gce_Done;
  TheLine = new Geom_Line(L);
}

GC_MakeLine::GC_MakeLine(const gp_Pnt& P1 ,
			   const gp_Pnt& P2 ) 
{
  gce_MakeLin L(P1,P2);
  TheError = L.Status();
  if (TheError == gce_Done) {
    TheLine = new Geom_Line(L.Value());
  }
}

GC_MakeLine::GC_MakeLine(const gp_Lin& Lin   ,
			   const gp_Pnt& Point ) 
{
  gce_MakeLin L(Lin,Point);
  TheError = L.Status();
  if (TheError == gce_Done) {
    TheLine = new Geom_Line(L.Value());
  }
}

const Handle(Geom_Line)& GC_MakeLine::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheLine;
}

const Handle(Geom_Line)& GC_MakeLine::Operator() const 
{
  return Value();
}

GC_MakeLine::operator Handle(Geom_Line) () const
{
  return Value();
}

