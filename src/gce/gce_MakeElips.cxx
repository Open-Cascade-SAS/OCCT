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


#include <gce_MakeElips.ixx>
#include <gp.hxx>
#include <gp_Lin.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une Ellipse 3d de gp a partir de son Ax2 et de son        +
//   grand rayon <MajorRadius> et son petit rayon <MinorRadius>.          +
//=========================================================================

gce_MakeElips::gce_MakeElips(const gp_Ax2&       A2          ,
			     const Standard_Real MajorRadius ,
			     const Standard_Real MinorRadius ) 
{
  if (MajorRadius < MinorRadius ) { TheError = gce_InvertRadius;}
  else if (MinorRadius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheElips = gp_Elips(A2,MajorRadius,MinorRadius);
    TheError = gce_Done;
  }

}

//=========================================================================
//   Creation d une Ellipse 3d de gp de centre <Center> et de sommets     +
//   <S1> et <S2>.                                                        +
//   <S1> donne le grand rayon et <S2> le petit rayon.                    +
//=========================================================================

gce_MakeElips::gce_MakeElips(const gp_Pnt&   S1     ,
			     const gp_Pnt&   S2     ,
			     const gp_Pnt&   Center ) 
{
  Standard_Real D1 = S1.Distance(Center);
  if (D1 < gp::Resolution()) { TheError = gce_NullAxis; }
  else {
    gp_Dir XAxis(gp_XYZ(S1.XYZ()-Center.XYZ()));
    Standard_Real D2 = gp_Lin(Center,XAxis).Distance(S2);
    if (D1 < D2 || D2 < gp::Resolution()) { TheError = gce_InvertAxis; }
    else {
      gp_Dir Norm(XAxis.Crossed(gp_Dir(gp_XYZ(S2.XYZ()-Center.XYZ()))));
      TheElips = gp_Elips(gp_Ax2(Center,Norm,XAxis),D1,D2);
      TheError = gce_Done;
    }
  }
}

const gp_Elips& gce_MakeElips::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheElips;
}

const gp_Elips& gce_MakeElips::Operator() const 
{
  return Value();
}

gce_MakeElips::operator gp_Elips() const
{
  return Value();
}
