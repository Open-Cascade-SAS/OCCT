// Created on: 2000-05-22
// Created by: Peter KURNEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <IntTools_Compare.ixx>
//=======================================================================
//function :IntTools_Compare::IntTools_Compare
//purpose  : 
//=======================================================================
IntTools_Compare::IntTools_Compare() :myTol(1.e-12) {}

//=======================================================================
//function :IntTools_Compare::IntTools_Compare
//purpose  : 
//=======================================================================
  IntTools_Compare::IntTools_Compare(const Standard_Real aTol)
{
  myTol=aTol;
}

//=======================================================================
//function :IsLower
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Compare::IsLower(const IntTools_Root& aLeft,
					     const IntTools_Root& aRight)const
{
  return aLeft.Root()<aRight.Root();
}

//=======================================================================
//function :IsGreater
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Compare::IsGreater(const IntTools_Root& aLeft,
					       const IntTools_Root& aRight)const
{
  return !IsLower(aLeft,aRight);
}

//=======================================================================
//function :IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Compare::IsEqual(const IntTools_Root& aLeft,
					     const IntTools_Root& aRight)const
{
  Standard_Real a, b;
  a=aLeft.Root();
  b=aRight.Root();
  return fabs(a-b) < myTol;
}






