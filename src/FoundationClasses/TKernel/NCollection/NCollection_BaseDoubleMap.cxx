// Created on: 2026-08-07
// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <NCollection_BaseDoubleMap.hxx>

#include <cstring>
#include <utility>

namespace
{
NCollection_ListNode** allocateDoubleBuckets(const size_t            theNbBuckets,
                                             NCollection_ListNode**& theData2)
{
  const size_t aCount = theNbBuckets + 1;
  const size_t aBytes = aCount * sizeof(NCollection_ListNode*) * 2;
  NCollection_ListNode** aData1 =
    static_cast<NCollection_ListNode**>(Standard::Allocate(aBytes));
  std::memset(aData1, 0, aBytes);
  theData2 = aData1 + aCount;
  return aData1;
}
} // namespace

//=================================================================================================

bool NCollection_BaseDoubleMap::beginResize(const size_t            theExtent,
                                            size_t&                 theNewBuckets,
                                            NCollection_ListNode**& theData1,
                                            NCollection_ListNode**& theData2) const
{
  theNewBuckets = nextPrimeForMap(theExtent);
  if (theNewBuckets <= myNbBuckets)
  {
    if (!myData1)
    {
      theNewBuckets = myNbBuckets;
    }
    else
    {
      return false;
    }
  }

  theData1 = allocateDoubleBuckets(theNewBuckets, theData2);
  return true;
}

//=================================================================================================

void NCollection_BaseDoubleMap::endResize(const size_t           theExtent,
                                          const size_t           theNewBuckets,
                                          NCollection_ListNode** theData1,
                                          NCollection_ListNode** theData2) noexcept
{
  (void)theExtent;
  Standard::Free(myData1);
  myNbBuckets = theNewBuckets;
  myData1     = theData1;
  myData2     = theData2;
}

//=================================================================================================

void NCollection_BaseDoubleMap::destroy(NCollection_DelMapNode fDel,
                                        const bool               theReleaseMemory)
{
  if (!IsEmpty())
  {
    const size_t aNbBuckets = NbBuckets();
    for (size_t anIndex = 0; anIndex <= aNbBuckets; ++anIndex)
    {
      if (myData1[anIndex] != nullptr)
      {
        NCollection_ListNode* aCurrent = myData1[anIndex];
        while (aCurrent != nullptr)
        {
          NCollection_ListNode* aNext = aCurrent->Next();
          fDel(aCurrent, myAllocator);
          aCurrent = aNext;
        }
        myData1[anIndex] = nullptr;
      }
    }
    mySize = 0;
  }

  if (myData2 != nullptr)
  {
    std::memset(myData2, 0, (NbBuckets() + 1) * sizeof(NCollection_ListNode*));
  }

  if (theReleaseMemory)
  {
    Standard::Free(myData1);
    myData1 = nullptr;
    myData2 = nullptr;
  }
}

//=================================================================================================

void NCollection_BaseDoubleMap::exchangeDoubleMapsData(
  NCollection_BaseDoubleMap& theOther) noexcept
{
  exchangeMapsData(theOther);
  std::swap(myData2, theOther.myData2);
}
