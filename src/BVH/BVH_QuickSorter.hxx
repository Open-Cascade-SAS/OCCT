// Created on: 2016-04-13
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2016 OPEN CASCADE SAS
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

#ifndef _BVH_QuickSorter_Header
#define _BVH_QuickSorter_Header

#include <BVH_Sorter.hxx>

//! Performs centroid-based sorting of abstract set along
//! the given axis (X - 0, Y - 1, Z - 2) using quick sort.
template <class T, int N>
class BVH_QuickSorter : public BVH_Sorter<T, N>
{
public:
  //! Creates new BVH quick sorter for the given axis.
  BVH_QuickSorter(const Standard_Integer theAxis = 0)
      : myAxis(theAxis)
  {
  }

  //! Sorts the set.
  virtual void Perform(BVH_Set<T, N>* theSet) Standard_OVERRIDE
  {
    Perform(theSet, 0, theSet->Size() - 1);
  }

  //! Sorts the given (inclusive) range in the set.
  virtual void Perform(BVH_Set<T, N>*         theSet,
                       const Standard_Integer theStart,
                       const Standard_Integer theFinal) Standard_OVERRIDE
  {
    Standard_Integer aLft = theStart;
    Standard_Integer aRgh = theFinal;

    T aPivot = theSet->Center((aRgh + aLft) / 2, myAxis);
    while (aLft < aRgh)
    {
      while (theSet->Center(aLft, myAxis) < aPivot && aLft < theFinal)
      {
        ++aLft;
      }

      while (theSet->Center(aRgh, myAxis) > aPivot && aRgh > theStart)
      {
        --aRgh;
      }

      if (aLft <= aRgh)
      {
        if (aLft != aRgh)
        {
          theSet->Swap(aLft, aRgh);
        }
        ++aLft;
        --aRgh;
      }
    }

    if (aRgh > theStart)
    {
      Perform(theSet, theStart, aRgh);
    }

    if (aLft < theFinal)
    {
      Perform(theSet, aLft, theFinal);
    }
  }

protected:
  //! Axis used to arrange the primitives (X - 0, Y - 1, Z - 2).
  Standard_Integer myAxis;
};

#endif // _BVH_QuickSorter_Header
