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


#include <GCE2d_MakeCircle.ixx>
#include <gce_MakeCirc2d.hxx>
#include <Geom2d_Circle.hxx>
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
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheCircle;
}

const Handle(Geom2d_Circle)& GCE2d_MakeCircle::Operator() const 
{
  return Value();
}

GCE2d_MakeCircle::operator Handle(Geom2d_Circle) () const
{
  return Value();
}

