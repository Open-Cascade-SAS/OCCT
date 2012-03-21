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


#include <GC_MakeConicalSurface.ixx>
#include <gce_MakeCone.hxx>
#include <gp.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_NotImplemented.hxx>

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Ax2&       A2    ,
					       const Standard_Real Ang   ,
					       const Standard_Real Radius)
{
  if (Radius < 0.) { TheError = gce_NegativeRadius; }
  else if (Ang <= gp::Resolution() || Ang >= M_PI/2. - gp::Resolution()) {
    TheError = gce_BadAngle;
  }
  else {
    TheError = gce_Done;
    TheCone = new Geom_ConicalSurface(A2,Ang,Radius);
  }
}

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Cone& C)
{
  TheError = gce_Done;
  TheCone = new Geom_ConicalSurface(C);
}

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Cone& , //C,
					       const gp_Pnt&  ) //P )
{
  Standard_NotImplemented::Raise("GC_MakeConicalSurface");
}

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Cone& , //C,
					       const Standard_Real) // Dist )
{
  Standard_NotImplemented::Raise("GC_MakeConicalSurface");
}

//=========================================================================
//   Creation of a cone by four points.                                +
//   two first give the axis.                                     +
//   the third gives the base radius.                              +
//   the third and the fourth the half-angle.                          +
//=========================================================================

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Pnt& P1 ,
					       const gp_Pnt& P2 ,
					       const gp_Pnt& P3 ,
					       const gp_Pnt& P4 ) 
{
  gce_MakeCone C = gce_MakeCone(P1,P2,P3,P4);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCone = new Geom_ConicalSurface(C.Value());
  }
}


GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Ax1& , //Axis,
					       const gp_Pnt& , //P1  ,
					       const gp_Pnt& ) //P2   )
{
  Standard_NotImplemented::Raise("GC_MakeConicalSurface");
}


GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Lin& , //Axis,
					       const gp_Pnt& , //P1  ,
					       const gp_Pnt& ) //P2   )
{
  Standard_NotImplemented::Raise("GC_MakeConicalSurface");
}


//=========================================================================
//=========================================================================

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Pnt&       P1 ,
					       const gp_Pnt&       P2 ,
					       const Standard_Real R1 ,
					       const Standard_Real R2 ) 
{
  gce_MakeCone C = gce_MakeCone(P1,P2,R1,R2);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCone = new Geom_ConicalSurface(C);
  }
}

const Handle(Geom_ConicalSurface)& GC_MakeConicalSurface::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheCone;
}

const Handle(Geom_ConicalSurface)& GC_MakeConicalSurface::Operator() const 
{
  return Value();
}

GC_MakeConicalSurface::operator Handle(Geom_ConicalSurface) () const
{
  return Value();
}

