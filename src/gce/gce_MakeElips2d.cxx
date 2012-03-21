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


#include <gce_MakeElips2d.ixx>
#include <gp.hxx>
#include <gp_Lin2d.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une Ellipse 2d de gp de centre <Center> et de sommets     +
//   <S1> et <S2>.                                                        +
//   <CenterS1> donne le grand axe .                                      +
//   <S1> donne le grand rayon et <S2> le petit rayon.                    +
//=========================================================================

gce_MakeElips2d::gce_MakeElips2d(const gp_Pnt2d&   S1     ,
				 const gp_Pnt2d&   S2     ,
				 const gp_Pnt2d&   Center ) 
{
  Standard_Real D1 = S1.Distance(Center);
  gp_Dir2d XAxis(gp_XY(S1.XY()-Center.XY()));
  gp_Dir2d YAxis(gp_XY(S2.XY()-Center.XY()));
  Standard_Real D2 = gp_Lin2d(Center,XAxis).Distance(S2);
  if (D1 < D2) { TheError = gce_InvertAxis; }
  else if (D2 < gp::Resolution()) { TheError = gce_NullAxis; }
  else {
    TheElips2d = gp_Elips2d(gp_Ax22d(Center,XAxis,YAxis),D1,D2);
    TheError = gce_Done;
  }
}

gce_MakeElips2d::gce_MakeElips2d(const gp_Ax2d&         MajorAxis   ,
				 const Standard_Real    MajorRadius ,
				 const Standard_Real    MinorRadius ,
				 const Standard_Boolean Sense       ) 
{
  if (MajorRadius < 0.0) { TheError = gce_NegativeRadius; }
  else if (MajorRadius < MinorRadius) { TheError = gce_InvertRadius; }
  else {
    TheElips2d = gp_Elips2d(MajorAxis,MajorRadius,MinorRadius,Sense);
    TheError = gce_Done;
  }
}

gce_MakeElips2d::gce_MakeElips2d(const gp_Ax22d&     A           ,
				 const Standard_Real MajorRadius ,
				 const Standard_Real MinorRadius ) 
{
  if (MajorRadius < 0.0) { TheError = gce_NegativeRadius; }
  else if (MajorRadius < MinorRadius) { TheError = gce_InvertRadius; }
  else {
    TheElips2d = gp_Elips2d(A,MajorRadius,MinorRadius);
    TheError = gce_Done;
  }
}

const gp_Elips2d& gce_MakeElips2d::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheElips2d;
}

const gp_Elips2d& gce_MakeElips2d::Operator() const 
{
  return Value();
}

gce_MakeElips2d::operator gp_Elips2d() const
{
  return Value();
}

