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
{
  myLargestKey = -1;
  myCustomTolerance = -1;
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
  if (myTolerances.IsBound (theTolerance))
  {
    Standard_Integer& aFreq = myTolerances.ChangeFind (theTolerance);
    aFreq++;

    if (aFreq == 1 && theTolerance != myLargestKey)
      myLargestKey = Max (theTolerance, myLargestKey);
  }
  else
  {
    if (myTolerances.IsEmpty())
    {
      myTolerances.Bind (theTolerance, 1);
      myLargestKey = theTolerance;
      return;
    }

    myTolerances.Bind (theTolerance, 1);
    myLargestKey = Max (theTolerance, myLargestKey);
  }
}

//=======================================================================
// function: Decrement
// purpose :
//=======================================================================
void SelectMgr_ToleranceMap::Decrement (const Standard_Integer& theTolerance)
{
  if (myTolerances.IsBound (theTolerance))
  {
    Standard_Integer& aFreq = myTolerances.ChangeFind (theTolerance);
    aFreq--;

    if (Abs (theTolerance - myLargestKey) < Precision::Confusion() && aFreq == 0)
    {
      myLargestKey = 0;
      for (NCollection_DataMap<Standard_Integer, Standard_Integer>::Iterator anIter (myTolerances); anIter.More(); anIter.Next())
      {
        if (anIter.Value() != 0)
          myLargestKey = Max (myLargestKey, anIter.Key());
      }
    }
  }
}
