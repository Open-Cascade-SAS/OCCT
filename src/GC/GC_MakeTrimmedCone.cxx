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


#include <GC_MakeTrimmedCone.ixx>
#include <GC_MakeConicalSurface.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_ConicalSurface.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Extrema_ExtPElC.hxx>

//=========================================================================
//   Creation of a cone by four points.                                +
//   First two give the axis.                                     +
//   The third gives the base radius.                              +
//   the third and the fourth demi-angle.                          +
//=========================================================================

GC_MakeTrimmedCone::GC_MakeTrimmedCone(const gp_Pnt& P1 ,
					 const gp_Pnt& P2 ,
					 const gp_Pnt& P3 ,
					 const gp_Pnt& P4 ) 
{
  GC_MakeConicalSurface Cone(P1,P2,P3,P4);
  TheError = Cone.Status();
  if (TheError == gce_Done) {
    gp_Dir D1(P2.XYZ()-P1.XYZ());
    gp_Lin L1(P1,D1);
    Extrema_ExtPElC ext1(P3,L1,1.0e-7,-2.0e+100,+2.0e+100);
    Extrema_ExtPElC ext2(P4,L1,1.0e-7,-2.0e+100,+2.0e+100);
    gp_Pnt P5 = ext1.Point(1).Value();
    gp_Pnt P6 = ext2.Point(1).Value();
    Standard_Real D = P6.Distance(P5)/cos((Cone.Value())->SemiAngle());
    TheCone=new Geom_RectangularTrimmedSurface(Cone.Value(),0.,2.*M_PI,0.,D,Standard_True,Standard_True);
  }
}

//=========================================================================
//=========================================================================

GC_MakeTrimmedCone::GC_MakeTrimmedCone(const gp_Pnt&       P1 ,
					 const gp_Pnt&       P2 ,
					 const Standard_Real R1 ,
					 const Standard_Real R2 ) 
{
  GC_MakeConicalSurface Cone(P1,P2,R1,R2);
  TheError = Cone.Status();
  if (TheError == gce_Done) {
    Standard_Real D = (P2.Distance(P1))/cos((Cone.Value())->SemiAngle());
    TheCone=new Geom_RectangularTrimmedSurface(Cone.Value(),0.,2.*M_PI,0.,D,Standard_True,Standard_True);
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const Handle(Geom_RectangularTrimmedSurface)& GC_MakeTrimmedCone::
       Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheCone;
}

const Handle(Geom_RectangularTrimmedSurface)& GC_MakeTrimmedCone::
       Operator() const 
{
  return Value();
}

GC_MakeTrimmedCone::
  operator Handle(Geom_RectangularTrimmedSurface) () const
{
  return Value();
}

