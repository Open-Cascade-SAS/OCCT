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



#include <BOP_FaceInfo.ixx>

//=======================================================================
// function: BOP_FaceInfo::BOP_FaceInfo
// purpose: 
//=======================================================================
BOP_FaceInfo::BOP_FaceInfo() 
:
  myPassed(Standard_False),
  myAngle(0.)
{}

//=======================================================================
// function: SetFace
// purpose: 
//=======================================================================
  void BOP_FaceInfo::SetFace(const TopoDS_Face& aF)
{
  myFace=aF;
}
//=======================================================================
// function: Face
// purpose: 
//=======================================================================
  const TopoDS_Face& BOP_FaceInfo::Face()const
{
  return myFace;
}

//=======================================================================
// function: SetPassed
// purpose: 
//=======================================================================
  void BOP_FaceInfo::SetPassed(const Standard_Boolean aFlag)
{
  myPassed=aFlag;
}

//=======================================================================
// function: SetPOnEdge
// purpose: 
//=======================================================================
  void BOP_FaceInfo::SetPOnEdge(const gp_Pnt& aP)
{
  myPOnEdge=aP;
}

//=======================================================================
// function: SetAngle
// purpose: 
//=======================================================================
  void BOP_FaceInfo::SetAngle(const Standard_Real A)
{
  myAngle=A;
}
//=======================================================================
// function: Angle
// purpose: 
//=======================================================================
  Standard_Real BOP_FaceInfo::Angle()const
{
  return myAngle;
}

//=======================================================================
// function: POnEdge
// purpose: 
//=======================================================================
  const gp_Pnt& BOP_FaceInfo::POnEdge()const
{
  return myPOnEdge;
}
//=======================================================================
// function: SetPInFace
// purpose: 
//=======================================================================
  void BOP_FaceInfo::SetPInFace(const gp_Pnt& aP)
{
  myPInFace=aP;
}
//=======================================================================
// function: PInFace
// purpose: 
//=======================================================================
  const gp_Pnt& BOP_FaceInfo::PInFace()const
{
  return myPInFace;
}
//=======================================================================
// function: SetPInFace2D
// purpose: 
//=======================================================================
  void BOP_FaceInfo::SetPInFace2D(const gp_Pnt2d& aP)
{
  myPInFace2D=aP;
}
//=======================================================================
// function: PInFace2D
// purpose: 
//=======================================================================
  const gp_Pnt2d& BOP_FaceInfo::PInFace2D()const
{
  return myPInFace2D;
}

//=======================================================================
// function: SetNormal
// purpose: 
//=======================================================================
  void BOP_FaceInfo::SetNormal(const gp_Dir& aD)
{
  myNormal=aD;
}
//=======================================================================
// function: Normal
// purpose: 
//=======================================================================
  const gp_Dir& BOP_FaceInfo::Normal()const
{
  return myNormal;
}

//=======================================================================
// function: IsPassed
// purpose: 
//=======================================================================
  Standard_Boolean BOP_FaceInfo::IsPassed()const
{
  return myPassed;
}
