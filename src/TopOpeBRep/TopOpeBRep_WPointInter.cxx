// Created on: 1993-11-10
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <TopOpeBRep_WPointInter.ixx>

//=======================================================================
//function : WPointInter
//purpose  : 
//=======================================================================

TopOpeBRep_WPointInter::TopOpeBRep_WPointInter()
{}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TopOpeBRep_WPointInter::Set(const IntSurf_PntOn2S& P)
{
  myPP2S = (IntSurf_PntOn2S*)&P;
}

//=======================================================================
//function : ParametersOnS1
//purpose  : 
//=======================================================================

void TopOpeBRep_WPointInter::ParametersOnS1
  (Standard_Real& U1, Standard_Real& V1) const
{
  myPP2S->ParametersOnS1(U1,V1);
}

//=======================================================================
//function : ParametersOnS2
//purpose  : 
//=======================================================================

void  TopOpeBRep_WPointInter::ParametersOnS2
  (Standard_Real& U2, Standard_Real& V2) const
{
  myPP2S->ParametersOnS2(U2,V2);
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

void  TopOpeBRep_WPointInter::Parameters
  (Standard_Real& U1, Standard_Real& V1,
   Standard_Real& U2, Standard_Real& V2) const
{
  myPP2S->Parameters(U1,V1,U2,V2);
}

//=======================================================================
//function : ValueOnS1
//purpose  : 
//=======================================================================

gp_Pnt2d TopOpeBRep_WPointInter::ValueOnS1() const
{
  Standard_Real u,v;
  myPP2S->ParametersOnS1(u,v);
  return gp_Pnt2d(u,v);
}

//=======================================================================
//function : ValueOnS2
//purpose  : 
//=======================================================================

gp_Pnt2d TopOpeBRep_WPointInter::ValueOnS2() const
{
  Standard_Real u,v;
  myPP2S->ParametersOnS2(u,v);
  return gp_Pnt2d(u,v);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const gp_Pnt& TopOpeBRep_WPointInter::Value() const
{
  return myPP2S->Value();
}

TopOpeBRep_PPntOn2S TopOpeBRep_WPointInter::PPntOn2SDummy() const { return myPP2S; } 
