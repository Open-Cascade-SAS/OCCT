// Created on: 1993-08-31
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


#include <TopOpeBRepDS_ListOfShapeOn1State.ixx>

#include <Standard_ProgramError.hxx>

//=======================================================================
//function : TopOpeBRepDS_ListOfShapeOn1State
//purpose  : 
//=======================================================================

TopOpeBRepDS_ListOfShapeOn1State::TopOpeBRepDS_ListOfShapeOn1State() 
: mySplits(0)
{
}

//=======================================================================
//function : ListOnState
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& TopOpeBRepDS_ListOfShapeOn1State::ListOnState
() const
{
  return myList;
}

//=======================================================================
//function : ChangeListOnState
//purpose  : 
//=======================================================================

TopTools_ListOfShape& TopOpeBRepDS_ListOfShapeOn1State::ChangeListOnState
()
{
  return myList;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void TopOpeBRepDS_ListOfShapeOn1State::Clear()
{
  myList.Clear();
  mySplits = 0;
}

//=======================================================================
//function : IsSplit
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_ListOfShapeOn1State::IsSplit
() const
{
  Standard_Boolean res = Standard_False;
  if (mySplits & 1) res = Standard_True;
  return res;
}

//=======================================================================
//function : Split
//purpose  : 
//=======================================================================

void TopOpeBRepDS_ListOfShapeOn1State::Split
(const Standard_Boolean B)
{
  Standard_Integer mask = 1;
  if (B) mySplits |= mask;
  else   mySplits &= ~mask;
}
