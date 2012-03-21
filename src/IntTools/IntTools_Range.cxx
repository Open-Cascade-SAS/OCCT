// Created on: 2000-05-18
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



#include <IntTools_Range.ixx>
//=======================================================================
//function : IntTools_Range::IntTools_Range
//purpose  : 
//=======================================================================
IntTools_Range::IntTools_Range():myFirst(0.), myLast(0.) {}

//=======================================================================
//function : IntTools_Range::IntTools_Range
//purpose  : 
//=======================================================================
  IntTools_Range::IntTools_Range(const Standard_Real aFirst,const Standard_Real aLast)
{
  myFirst=aFirst;
  myLast=aLast;
}
//=======================================================================
//function : SetFirst
//purpose  : 
//=======================================================================
  void IntTools_Range::SetFirst(const Standard_Real aFirst) 
{
  myFirst=aFirst;
}
//=======================================================================
//function : SetLast
//purpose  : 
//=======================================================================
  void IntTools_Range::SetLast(const Standard_Real aLast) 
{
  myLast=aLast;
}
//=======================================================================
//function : First
//purpose  : 
//=======================================================================
  Standard_Real IntTools_Range::First() const
{
  return myFirst;
}
//=======================================================================
//function : Last
//purpose  : 
//=======================================================================
  Standard_Real IntTools_Range::Last() const
{
  return myLast;
}
//=======================================================================
//function : Range
//purpose  : 
//=======================================================================
  void IntTools_Range::Range(Standard_Real& aFirst,Standard_Real& aLast) const
{
  aFirst=myFirst;
  aLast =myLast;
}
