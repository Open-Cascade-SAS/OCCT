// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <SelectMgr_ToleranceMap.hxx>

//=======================================================================
// function: SelectMgr_ToleranceMap
// purpose :
//=======================================================================
SelectMgr_ToleranceMap::SelectMgr_ToleranceMap()
: myLargestKey (-1),
  myCustomTolerance (-1)
{
  //
}

//=======================================================================
// function: ~SelectMgr_ToleranceMap
// purpose :
//=======================================================================
SelectMgr_ToleranceMap::~SelectMgr_ToleranceMap()
{
  myTolerances.Clear();
}

//=======================================================================
// function: Add
// purpose :
//=======================================================================
void SelectMgr_ToleranceMap::Add (const Standard_Integer& theTolerance)
{
  if (Standard_Integer* aFreq = myTolerances.ChangeSeek (theTolerance))
  {
    ++(*aFreq);
    if (*aFreq == 1 && theTolerance != myLargestKey)
    {
      myLargestKey = Max (theTolerance, myLargestKey);
    }
    return;
  }

  myTolerances.Bind (theTolerance, 1);
  if (myTolerances.Extent() == 1)
  {
    myLargestKey = theTolerance;
  }
  else
  {
    myLargestKey = Max (theTolerance, myLargestKey);
  }
}

//=======================================================================
// function: Decrement
// purpose :
//=======================================================================
void SelectMgr_ToleranceMap::Decrement (const Standard_Integer& theTolerance)
{
  Standard_Integer* aFreq = myTolerances.ChangeSeek (theTolerance);
  if (aFreq == NULL)
  {
    return;
  }

  --(*aFreq);
  if (theTolerance == myLargestKey
  && *aFreq == 0)
  {
    myLargestKey = 0;
    for (NCollection_DataMap<Standard_Integer, Standard_Integer>::Iterator anIter (myTolerances); anIter.More(); anIter.Next())
    {
      if (anIter.Value() != 0)
      {
        myLargestKey = Max (myLargestKey, anIter.Key());
      }
    }
  }
}
