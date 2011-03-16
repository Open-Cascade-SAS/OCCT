// File:	TopOpeBRepDS_ListOfShapeOn1State.cxx
// Created:	Tue Aug 31 15:40:14 1993
// Author:	Jean Yves LEBEY
//		<jyl@phobox>

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
