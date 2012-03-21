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


#include <GCE2d_MakeLine.ixx>
#include <gce_MakeLin2d.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Constructions of 2d geometrical elements from Geom2d.
//=========================================================================

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Ax2d& A)
{
  TheError = gce_Done;
  TheLine = new Geom2d_Line(A);
}

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Lin2d& L)
{
  TheError = gce_Done;
  TheLine = new Geom2d_Line(L);
}

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Pnt2d& P,
			       const gp_Dir2d& V)
{
  TheError = gce_Done;
  TheLine = new Geom2d_Line(P,V);
}

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Pnt2d& P1 ,
			       const gp_Pnt2d& P2 ) 
{
  gce_MakeLin2d L(P1,P2);
  TheError = L.Status();
  if (TheError == gce_Done) {
    TheLine = new Geom2d_Line(L.Value());
  }
}

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Lin2d& Lin   ,
			       const gp_Pnt2d& Point ) 
{
  gce_MakeLin2d L(Lin,Point);
  TheError = L.Status();
  if (TheError == gce_Done) {
    TheLine = new Geom2d_Line(L.Value());
  }
}

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Lin2d&     Lin  ,
			       const Standard_Real Dist ) 
{
  gce_MakeLin2d L(Lin,Dist);
  TheError = L.Status();
  if (TheError == gce_Done) {
    TheLine = new Geom2d_Line(L.Value());
  }
}

const Handle(Geom2d_Line)& GCE2d_MakeLine::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheLine;
}

const Handle(Geom2d_Line) & GCE2d_MakeLine::Operator() const 
{
  return Value();
}

GCE2d_MakeLine::operator Handle(Geom2d_Line) () const
{
  return Value();
}

