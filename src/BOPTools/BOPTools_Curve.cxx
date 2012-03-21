// Created on: 2001-05-08
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



#include <BOPTools_Curve.ixx>

//=======================================================================
// function: BOPTools_Curve::BOPTools_Curve
// purpose: 
//=======================================================================
BOPTools_Curve::BOPTools_Curve()
{}
//=======================================================================
// function: BOPTools_Curve::BOPTools_Curve
// purpose: 
//=======================================================================
  BOPTools_Curve::BOPTools_Curve (const IntTools_Curve& aIC)
{
  myCurve=aIC;
}
//=======================================================================
// function: SetCurve
// purpose: 
//=======================================================================
  void BOPTools_Curve::SetCurve (const IntTools_Curve& aIC)
{
   myCurve=aIC;
}
//=======================================================================
// function: Curve
// purpose: 
//=======================================================================
  const IntTools_Curve& BOPTools_Curve::Curve () const 
{
   return myCurve;
}
//=======================================================================
// function: Set
// purpose: 
//=======================================================================
  BOPTools_PaveSet& BOPTools_Curve::Set()
{
  return myPaveSet;
}

//=======================================================================
//function :  AppendNewBlock
//purpose  : 
//=======================================================================
  void BOPTools_Curve::AppendNewBlock(const BOPTools_PaveBlock& aPB) 
{
  myNewPBs.Append(aPB);
}

//=======================================================================
//function :  NewPaveBlocks
//purpose  : 
//=======================================================================
  const BOPTools_ListOfPaveBlock& BOPTools_Curve::NewPaveBlocks() const
{
  return myNewPBs;
}
//=======================================================================
//function :  TechnoVertices
//purpose  : 
//=======================================================================
  TColStd_ListOfInteger& BOPTools_Curve::TechnoVertices() 
{
  return myTechnoVertices;
}
