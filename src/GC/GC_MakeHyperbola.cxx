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


#include <GC_MakeHyperbola.ixx>
#include <gce_MakeHypr.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeHyperbola::GC_MakeHyperbola(const gp_Hypr& H)
{
  TheError = gce_Done;
  TheHyperbola = new Geom_Hyperbola(H);
}

GC_MakeHyperbola::GC_MakeHyperbola(const gp_Ax2&       A2         ,
				     const Standard_Real MajorRadius,
				     const Standard_Real MinorRadius)
{
  if (MajorRadius < 0. || MinorRadius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheError = gce_Done;
    TheHyperbola = new Geom_Hyperbola(gp_Hypr(A2,MajorRadius,MinorRadius));
  }
}

GC_MakeHyperbola::GC_MakeHyperbola(const gp_Pnt& S1     ,
				     const gp_Pnt& S2     ,
				     const gp_Pnt& Center ) {
  gce_MakeHypr H = gce_MakeHypr(S1,S2,Center);
  TheError = H.Status();
  if (TheError == gce_Done) {
    TheHyperbola = new Geom_Hyperbola(H.Value());
  }
}

const Handle(Geom_Hyperbola)& GC_MakeHyperbola::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheHyperbola;
}

const Handle(Geom_Hyperbola)& GC_MakeHyperbola::Operator() const 
{
  return Value();
}

GC_MakeHyperbola::operator Handle(Geom_Hyperbola) () const
{
  return Value();
}

