// Created on: 1992-09-02
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


#include <gce_MakeHypr.ixx>
#include <StdFail_NotDone.hxx>
#include <gp_Lin.hxx>

//=========================================================================
//   Creation d une Hyperbole 3d de gp de centre <Center> et de sommets   +
//   <S1> et <S2>.                                                        +
//   <CenterS1> donne le grand axe .                                      +
//   <S1> donne le grand rayon et <S2> le petit rayon.                    +
//=========================================================================

gce_MakeHypr::gce_MakeHypr(const gp_Pnt&   S1     ,
			   const gp_Pnt&   S2     ,
			   const gp_Pnt&   Center ) 
{
  gp_Dir XAxis(gp_XYZ(S1.XYZ()-Center.XYZ()));
  gp_Lin L(Center,XAxis);
  Standard_Real D = S1.Distance(Center);
  Standard_Real d = L.Distance(S2);
  if (d > D) { TheError = gce_InvertAxis; }
  else {
    gp_Dir Norm(XAxis.Crossed(gp_Dir(gp_XYZ(S2.XYZ()-Center.XYZ()))));
    TheHypr = gp_Hypr(gp_Ax2(Center,Norm,XAxis),D,d);
    TheError = gce_Done;
  }
}

gce_MakeHypr::gce_MakeHypr(const gp_Ax2&       A2          ,
			   const Standard_Real MajorRadius ,
			   const Standard_Real MinorRadius ) 
{
  if (MajorRadius < MinorRadius) { TheError = gce_InvertRadius; }
  else if (MajorRadius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheHypr = gp_Hypr(A2,MajorRadius,MinorRadius);
    TheError = gce_Done;
  }
}

const gp_Hypr& gce_MakeHypr::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheHypr;
}

const gp_Hypr& gce_MakeHypr::Operator() const 
{
  return Value();
}

gce_MakeHypr::operator gp_Hypr() const
{
  return Value();
}

