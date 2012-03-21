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


#include <gce_MakeParab2d.ixx>
#include <gp.hxx>
#include <StdFail_NotDone.hxx>

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax22d&     A     ,
				 const Standard_Real Focal ) 
{
  if (Focal < 0.0) { TheError = gce_NullFocusLength; }
  else {
    TheParab2d = gp_Parab2d(A,Focal);
    TheError = gce_Done;
  }
}

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax2d&         MirrorAxis ,
				 const Standard_Real    Focal      ,
				 const Standard_Boolean Sense      ) 
{
  if (Focal < 0.0) { TheError = gce_NullFocusLength; }
  else {
    TheParab2d = gp_Parab2d(MirrorAxis,Focal,Sense);
    TheError = gce_Done;
  }
}

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax2d&  D            ,
				 const gp_Pnt2d& F            ,
				 const Standard_Boolean Sense )
{
  TheParab2d = gp_Parab2d(D,F,Sense);
  TheError = gce_Done;
}

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax22d&  D ,
				 const gp_Pnt2d& F  )
{
  TheParab2d = gp_Parab2d(D,F);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une Parabole 2d de gp de centre <Center> et de sommet     +
//   <S1> .                                                               +
//   <CenterS1> donne le grand axe .                                      +
//   <S1> donne la focale.                                                +
//=========================================================================

gce_MakeParab2d::gce_MakeParab2d(const gp_Pnt2d&        S      ,
				 const gp_Pnt2d&        Center ,
				 const Standard_Boolean Sense  ) 
{
  if (S.Distance(Center) >= gp::Resolution()) {
    gp_Dir2d XAxis(gp_XY(S.XY()-Center.XY()));
    TheParab2d = gp_Parab2d(gp_Ax2d(Center,XAxis),S.Distance(Center),Sense);
    TheError = gce_Done;
  }
  else { TheError = gce_NullAxis; }
}

const gp_Parab2d& gce_MakeParab2d::Value () const
{
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheParab2d;
}

const gp_Parab2d& gce_MakeParab2d::Operator() const 
{
  return Value();
}

gce_MakeParab2d::operator gp_Parab2d() const
{
  return Value();
}

