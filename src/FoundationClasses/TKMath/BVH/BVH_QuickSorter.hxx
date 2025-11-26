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
#include <NCollection_Allocator.hxx>

#include <algorithm>
#include <vector>

//! Performs centroid-based sorting of abstract set along
//! the given axis (X - 0, Y - 1, Z - 2) using std::sort.
//! Uses introsort algorithm which guarantees O(n log n) complexity.
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
    const Standard_Integer aSize = theFinal - theStart + 1;
    if (aSize <= 1)
    {
      return;
    }

    // Create index array for sorting with OCCT allocator
    std::vector<Standard_Integer, NCollection_Allocator<Standard_Integer>> anIndices(aSize);
    for (Standard_Integer i = 0; i < aSize; ++i)
    {
      anIndices[i] = i;
    }

    // Sort indices by center value using std::sort (introsort - O(n log n) guaranteed)
    const Standard_Integer anAxis = myAxis;
    std::sort(anIndices.begin(),
              anIndices.end(),
              [theSet, theStart, anAxis](Standard_Integer a, Standard_Integer b) {
                return theSet->Center(theStart + a, anAxis) < theSet->Center(theStart + b, anAxis);
              });

    // Compute inverse permutation: invPerm[i] = where element i should go
    std::vector<Standard_Integer, NCollection_Allocator<Standard_Integer>> anInvPerm(aSize);
    for (Standard_Integer i = 0; i < aSize; ++i)
    {
      anInvPerm[anIndices[i]] = i;
    }

    // Apply permutation using cycle-based algorithm - O(n) swaps total
    for (Standard_Integer i = 0; i < aSize; ++i)
    {
      // Follow the cycle starting at position i
      while (anInvPerm[i] != i)
      {
        Standard_Integer j = anInvPerm[i];
        theSet->Swap(theStart + i, theStart + j);
        std::swap(anInvPerm[i], anInvPerm[j]);
      }
    }
  }

protected:
  //! Axis used to arrange the primitives (X - 0, Y - 1, Z - 2).
  Standard_Integer myAxis;
};

#endif // _BVH_QuickSorter_Header
