// Created on: 2001-04-09
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


#include <BOP_EdgeInfo.ixx>

//=======================================================================
// function: BOP_EdgeInfo::BOP_EdgeInfo
// purpose: 
//=======================================================================
BOP_EdgeInfo::BOP_EdgeInfo() 
:
  myPassed(Standard_False),
  myInFlag(Standard_False),
  myAngle (-1.)
{}

//=======================================================================
// function: SetEdge
// purpose: 
//=======================================================================
  void BOP_EdgeInfo::SetEdge(const TopoDS_Edge& anEdge)
{
  myEdge=anEdge;
}

//=======================================================================
// function: SetPassed
// purpose: 
//=======================================================================
  void BOP_EdgeInfo::SetPassed(const Standard_Boolean aFlag)
{
  myPassed=aFlag;
}
//=======================================================================
// function: SetInFlag
// purpose: 
//=======================================================================
  void BOP_EdgeInfo::SetInFlag(const Standard_Boolean aFlag)
{
  myInFlag=aFlag;
}

//=======================================================================
// function: SetAngle
// purpose: 
//=======================================================================
  void BOP_EdgeInfo::SetAngle(const Standard_Real anAngle)
{
  myAngle=anAngle;
}

//=======================================================================
// function: Edge
// purpose: 
//=======================================================================
  const TopoDS_Edge& BOP_EdgeInfo::Edge()const
{
  return myEdge;
}

//=======================================================================
// function: Passed
// purpose: 
//=======================================================================
  Standard_Boolean BOP_EdgeInfo::Passed()const 
{
  return myPassed;
}
//=======================================================================
// function: IsIn
// purpose: 
//=======================================================================
  Standard_Boolean BOP_EdgeInfo::IsIn()const 
{
  return myInFlag;
}

//=======================================================================
// function: Angle
// purpose: 
//=======================================================================
  Standard_Real BOP_EdgeInfo::Angle()const 
{
  return myAngle;
}
