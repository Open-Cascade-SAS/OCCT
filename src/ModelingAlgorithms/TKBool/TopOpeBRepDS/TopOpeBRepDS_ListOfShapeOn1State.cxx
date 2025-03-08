// Created on: 1993-08-31
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>

//=================================================================================================

TopOpeBRepDS_ListOfShapeOn1State::TopOpeBRepDS_ListOfShapeOn1State()
    : mySplits(0)
{
}

//=================================================================================================

const TopTools_ListOfShape& TopOpeBRepDS_ListOfShapeOn1State::ListOnState() const
{
  return myList;
}

//=================================================================================================

TopTools_ListOfShape& TopOpeBRepDS_ListOfShapeOn1State::ChangeListOnState()
{
  return myList;
}

//=================================================================================================

void TopOpeBRepDS_ListOfShapeOn1State::Clear()
{
  myList.Clear();
  mySplits = 0;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_ListOfShapeOn1State::IsSplit() const
{
  Standard_Boolean res = Standard_False;
  if (mySplits & 1)
    res = Standard_True;
  return res;
}

//=================================================================================================

void TopOpeBRepDS_ListOfShapeOn1State::Split(const Standard_Boolean B)
{
  Standard_Integer mask = 1;
  if (B)
    mySplits |= mask;
  else
    mySplits &= ~mask;
}
