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


#include <GC_MakeArcOfHyperbola.ixx>
#include <Geom_Hyperbola.hxx>
#include <StdFail_NotDone.hxx>
#include <ElCLib.hxx>

GC_MakeArcOfHyperbola::
  GC_MakeArcOfHyperbola(const gp_Hypr& Hypr   ,
			 const gp_Pnt&  P1     ,
			 const gp_Pnt&  P2     ,
			 const Standard_Boolean  Sense  ) 
{
  Standard_Real Alpha1 = ElCLib::Parameter(Hypr,P1);
  Standard_Real Alpha2 = ElCLib::Parameter(Hypr,P2);
  Handle(Geom_Hyperbola) H = new Geom_Hyperbola(Hypr);
  TheArc = new Geom_TrimmedCurve(H,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

GC_MakeArcOfHyperbola::GC_MakeArcOfHyperbola(const gp_Hypr& Hypr   ,
					       const gp_Pnt&  P     ,
					       const Standard_Real     Alpha ,
					       const Standard_Boolean  Sense ) 
{
  Standard_Real Alphafirst = ElCLib::Parameter(Hypr,P);
  Handle(Geom_Hyperbola) H = new Geom_Hyperbola(Hypr);
  TheArc = new Geom_TrimmedCurve(H,Alphafirst,Alpha,Sense);
  TheError = gce_Done;
}

GC_MakeArcOfHyperbola::GC_MakeArcOfHyperbola(const gp_Hypr&        Hypr   ,
					     const Standard_Real     Alpha1 ,
					     const Standard_Real     Alpha2 ,
					     const Standard_Boolean  Sense  ) 
{
  Handle(Geom_Hyperbola) H = new Geom_Hyperbola(Hypr);
  TheArc = new Geom_TrimmedCurve(H,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

const Handle(Geom_TrimmedCurve)& GC_MakeArcOfHyperbola::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheArc;
}

const Handle(Geom_TrimmedCurve)& GC_MakeArcOfHyperbola::Operator() const 
{
  return Value();
}

GC_MakeArcOfHyperbola::operator Handle(Geom_TrimmedCurve) () const
{
  return Value();
}

