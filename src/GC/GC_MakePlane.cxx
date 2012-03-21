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


#include <GC_MakePlane.ixx>
#include <gce_MakePln.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_NotImplemented.hxx>


GC_MakePlane::GC_MakePlane(const gp_Ax2& ) //A2)
{
  Standard_NotImplemented::Raise("GC_MakePlane");
}

GC_MakePlane::GC_MakePlane(const gp_Pln& Pl)
{
  TheError = gce_Done;
  ThePlane = new Geom_Plane(Pl);
}

GC_MakePlane::GC_MakePlane(const gp_Pnt& P,
			     const gp_Dir& V)
{
  TheError = gce_Done;
  ThePlane = new Geom_Plane(P,V);
}

GC_MakePlane::GC_MakePlane(const Standard_Real A,
			     const Standard_Real B,
			     const Standard_Real C,
			     const Standard_Real D)
{
  if (Sqrt(A*A + B*B +C*C) <= gp::Resolution()) {
    TheError = gce_BadEquation; 
  }
  else {
    TheError = gce_Done;
    ThePlane = new Geom_Plane(gp_Pln(A,B,C,D));
  }
}

//=========================================================================
//   Creation d un Geom_Plane passant par trois points.                   +
//=========================================================================

GC_MakePlane::GC_MakePlane(const gp_Pnt& P1    ,
			     const gp_Pnt& P2    ,
			     const gp_Pnt& P3    ) {
  gce_MakePln Pl(P1,P2,P3);
  TheError = Pl.Status();
  if (TheError == gce_Done) {
    ThePlane = new Geom_Plane(Pl.Value());
  }
}

//=========================================================================
//   Creation d un Geom_Plane parallele a un pln a une distance donnee.   +
//=========================================================================

GC_MakePlane::GC_MakePlane(const gp_Pln& Pl   ,
			     const Standard_Real    Dist ) {
  gp_Pln Pln = gce_MakePln(Pl,Dist);
  TheError = gce_Done;
  ThePlane = new Geom_Plane(Pln);
}

//=========================================================================
//   Creation d un Geom_Plane parallele a un pln passant par un point     +
//   <Point1>.                                                            +
//=========================================================================

GC_MakePlane::GC_MakePlane(const gp_Pln& Pl    ,
			     const gp_Pnt& Point ) {
  gp_Pln Pln= gce_MakePln(Pl,Point);
  TheError = gce_Done;
  ThePlane = new Geom_Plane(Pln);
}

//=========================================================================
//  Creation d un Geom_Plane a partir d un Ax1 (Point + Normale).         +
//=========================================================================

GC_MakePlane::GC_MakePlane(const gp_Ax1& Axis ) {
  gp_Pln Pln = gce_MakePln(Axis);
  TheError = gce_Done;
  ThePlane = new Geom_Plane(Pln);
}
     
//=========================================================================
//  Creation d un Geom_Plane par un tableau de points.                    +
//=========================================================================

/*GC_MakePlane::GC_MakePlane(const TColgp_Array1OfPnt&    Pts     ,
	 		           Standard_Real            ErrMax  ,
			           Standard_Real            ErrMean ) {
  GC_MakePln Pln(Pts,ErrMax,ErrMean);
  TheError = Pln.Status();
  if (TheError == GC_Done) {
    ThePlane = new Geom_Plane(Pln.Value());
  }
}
*/

const Handle(Geom_Plane)& GC_MakePlane::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return ThePlane;
}

const Handle(Geom_Plane)& GC_MakePlane::Operator() const 
{
  return Value();
}

GC_MakePlane::operator Handle(Geom_Plane) () const
{
  return Value();
}

