// Created on: 1993-06-23
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



#include <TopOpeBRepDS_CurvePointInterference.ixx>

//=======================================================================
//function : TopOpeBRepDS_CurvePointInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_CurvePointInterference::TopOpeBRepDS_CurvePointInterference
  (const TopOpeBRepDS_Transition& T, 
   const TopOpeBRepDS_Kind ST,
   const Standard_Integer S, 
   const TopOpeBRepDS_Kind GT,
   const Standard_Integer G, 
   const Standard_Real P) :
  TopOpeBRepDS_Interference(T,ST,S,GT,G),
  myParam(P)
{
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real  TopOpeBRepDS_CurvePointInterference::Parameter()const 
{
  return myParam;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_CurvePointInterference::Parameter(const Standard_Real P)
{
  myParam = P;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_CurvePointInterference::Dump
  (Standard_OStream& OS) const
{
#ifdef DEB
  TopOpeBRepDS_Kind supporttype = SupportType();
  if      (supporttype == TopOpeBRepDS_EDGE)  OS<<"EPI";
  else if (supporttype == TopOpeBRepDS_CURVE) OS<<"CPI";
  else if (supporttype == TopOpeBRepDS_FACE)  OS<<"ECPI";
  else                                        OS<<"???";
  OS<<" "; TopOpeBRepDS_Interference::Dump(OS);
  OS<<" "<<myParam; 
#endif

  return OS;
}
