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


#include <gce_MakeHypr2d.ixx>
#include <gp_Lin2d.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une Hyperbola 2d de gp de centre <Center> et de sommets   +
//   <S1> et <S2>.                                                        +
//   <CenterS1> donne le grand axe .                                      +
//   <S1> donne le grand rayon et <S2> le petit rayon.                    +
//=========================================================================

gce_MakeHypr2d::gce_MakeHypr2d(const gp_Pnt2d&   S1     ,
			       const gp_Pnt2d&   S2     ,
			       const gp_Pnt2d&   Center )
{
  gp_Dir2d XAxis(gp_XY(S1.XY()-Center.XY()));
  gp_Dir2d YAxis(gp_XY(S2.XY()-Center.XY()));
  gp_Ax22d Axis(Center,XAxis,YAxis);
  gp_Lin2d L(Center,XAxis);
  Standard_Real D1 = S1.Distance(Center);
  Standard_Real D2 = L.Distance(S2);
  if (D1 >= D2) {
    TheHypr2d = gp_Hypr2d(Axis,D1,D2);
    TheError = gce_Done;
  }
  else { TheError = gce_InvertAxis; }
}

gce_MakeHypr2d::gce_MakeHypr2d(const gp_Ax2d&         MajorAxis   ,
			       const Standard_Real    MajorRadius ,
			       const Standard_Real    MinorRadius ,
			       const Standard_Boolean Sense       )
{
  if (MajorRadius < 0.0 || MinorRadius < 0.0) { TheError = gce_NegativeRadius;}
  else {
    TheHypr2d = gp_Hypr2d(MajorAxis,MajorRadius,MinorRadius,Sense);
    TheError = gce_Done;
  }
}

gce_MakeHypr2d::gce_MakeHypr2d(const gp_Ax22d&     A           ,
			       const Standard_Real MajorRadius ,
			       const Standard_Real MinorRadius )
{
  if (MajorRadius < 0.0 || MinorRadius < 0.0) { TheError = gce_NegativeRadius;}
  else {
    TheHypr2d = gp_Hypr2d(A,MajorRadius,MinorRadius);
    TheError = gce_Done;
  }
}

const gp_Hypr2d& gce_MakeHypr2d::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheHypr2d;
}

const gp_Hypr2d& gce_MakeHypr2d::Operator() const 
{
  return Value();
}

gce_MakeHypr2d::operator gp_Hypr2d() const
{
  return Value();
}

