// Created on: 2001-04-19
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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



#include <BOPTools_PointBetween.ixx>

//=======================================================================
// function:  BOPTools_PointBetween:: BOPTools_PointBetween
// purpose: 
//=======================================================================
BOPTools_PointBetween::BOPTools_PointBetween()
:
  myT(0.),
  myU(0.),
  myV(0.)
{}
  
//=======================================================================
// function:  SetParameter
// purpose: 
//=======================================================================
  void BOPTools_PointBetween::SetParameter(const Standard_Real aT)
{
  myT=aT;
}

//=======================================================================
// function:  Parameter
// purpose: 
//=======================================================================
  Standard_Real BOPTools_PointBetween::Parameter()const 
{
  return myT;
}

//=======================================================================
// function:  SetUV
// purpose: 
//=======================================================================
  void BOPTools_PointBetween::SetUV(const Standard_Real aU, 
				    const Standard_Real aV)
{
  myU=aU;
  myV=aV;
}
//=======================================================================
// function:  UV
// purpose: 
//=======================================================================
  void BOPTools_PointBetween::UV(Standard_Real& aU, 
				 Standard_Real& aV) const
{
  aU=myU;
  aV=myV;
}

//=======================================================================
// function:  SetPnt
// purpose: 
//=======================================================================
  void BOPTools_PointBetween::SetPnt (const gp_Pnt& aP) 
{
  myPnt=aP;
}

//=======================================================================
// function:  Pnt
// purpose: 
//=======================================================================
  const gp_Pnt& BOPTools_PointBetween::Pnt () const 
{
  return myPnt;
}
