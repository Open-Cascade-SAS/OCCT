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


#include <GC_MakeTrimmedCylinder.ixx>
#include <GC_MakeCylindricalSurface.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_NotImplemented.hxx>

//=========================================================================
//   Creation of a cylinder limited by three points <P1>, <P2> and <P3>.         +
//   the height og the resulting cylinder is the distance from <P1> to <P2>.     +
//   The radius is the distance from <P3> to axis <P1P2>.                 +
//=========================================================================

GC_MakeTrimmedCylinder::GC_MakeTrimmedCylinder(const gp_Pnt& P1 ,
						 const gp_Pnt& P2 ,
						 const gp_Pnt& P3 ) 
{
  GC_MakeCylindricalSurface Cyl(P1,P2,P3);
  TheError = Cyl.Status();
  if (TheError == gce_Done) {
    TheCyl = new Geom_RectangularTrimmedSurface(Cyl.Value(),0.,2.*M_PI,0.,
				  P2.Distance(P1),Standard_True,Standard_True);
  }
}

//=========================================================================
//   Creation of a cylinder limited by a circle and height.          +
//=========================================================================

GC_MakeTrimmedCylinder::GC_MakeTrimmedCylinder(const gp_Circ&      Circ   ,
						 const Standard_Real Height ) {
  GC_MakeCylindricalSurface Cyl(Circ);
  TheError = Cyl.Status();
  if (TheError == gce_Done) {
    TheCyl = new Geom_RectangularTrimmedSurface(Cyl.Value(),0.,2.*M_PI,0.,
					   Height,Standard_True,Standard_True);
  }
}
     
//=========================================================================
//=========================================================================

GC_MakeTrimmedCylinder::GC_MakeTrimmedCylinder(const gp_Ax1&       A1     ,
						 const Standard_Real Radius ,
						 const Standard_Real Height ) {
  GC_MakeCylindricalSurface Cyl(A1,Radius);
  TheError = Cyl.Status();
  if (TheError == gce_Done) {
    TheCyl = new Geom_RectangularTrimmedSurface(Cyl.Value(),0.,2.*M_PI,0.,
					Height,Standard_True,Standard_True);
  }
}

GC_MakeTrimmedCylinder::GC_MakeTrimmedCylinder(const gp_Cylinder& , // Cyl,
						 const gp_Pnt&      , // P,
						 const Standard_Real )//Height)
{
  Standard_NotImplemented::Raise("GC_MakeTrimmedCylinder");
}

GC_MakeTrimmedCylinder::GC_MakeTrimmedCylinder(const gp_Cylinder& , // Cyl,
						 const gp_Pnt&      , // P1,
						 const gp_Pnt&      ) // P2)
{
  Standard_NotImplemented::Raise("GC_MakeTrimmedCylinder");
}

const Handle(Geom_RectangularTrimmedSurface)& GC_MakeTrimmedCylinder::
       Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheCyl;
}

const Handle(Geom_RectangularTrimmedSurface)& GC_MakeTrimmedCylinder::
       Operator() const 
{
  return Value();
}

GC_MakeTrimmedCylinder::
  operator Handle(Geom_RectangularTrimmedSurface) () const
{
  return Value();
}

