// Created on: 1993-11-10
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <TopOpeBRep_WPointInter.hxx>

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
