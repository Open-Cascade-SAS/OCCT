// Created on: 2001-12-13
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


#include <IntTools_PntOn2Faces.ixx>

//=======================================================================
//function : IntTools_PntOn2Faces::IntTools_PntOn2Faces
//purpose  : 
//=======================================================================
IntTools_PntOn2Faces::IntTools_PntOn2Faces()
:
  myIsValid(Standard_False)
{}
//=======================================================================
//function : IntTools_PntOn2Faces::IntTools_PntOn2Faces
//purpose  : 
//=======================================================================
  IntTools_PntOn2Faces::IntTools_PntOn2Faces(const IntTools_PntOnFace& aP1,
					     const IntTools_PntOnFace& aP2)
:
  myIsValid(Standard_False)
{
  myPnt1=aP1;
  myPnt2=aP2;
}
//=======================================================================
//function : SetP1
//purpose  : 
//=======================================================================
  void IntTools_PntOn2Faces::SetP1(const IntTools_PntOnFace& aP)
{
  myPnt1=aP;
}
//=======================================================================
//function : SetP2
//purpose  : 
//=======================================================================
  void IntTools_PntOn2Faces::SetP2(const IntTools_PntOnFace& aP)
{
  myPnt2=aP;
}
//=======================================================================
//function : P1
//purpose  : 
//=======================================================================
  const IntTools_PntOnFace& IntTools_PntOn2Faces::P1()const
{
  return myPnt1;
}
//=======================================================================
//function : P2
//purpose  : 
//=======================================================================
  const IntTools_PntOnFace& IntTools_PntOn2Faces::P2()const 
{
  return myPnt2;
}

//=======================================================================
//function : SetValid
//purpose  : 
//=======================================================================
  void IntTools_PntOn2Faces::SetValid(const Standard_Boolean bF)
{
  myIsValid=bF;
}
//=======================================================================
//function : IsValid
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_PntOn2Faces::IsValid()const
{
  return myIsValid;
}

