// File:	TopOpeBRep_WPointInter.cxx
// Created:	Wed Nov 10 18:55:48 1993
// Author:	Jean Yves LEBEY
//		<jyl@phobox>

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
