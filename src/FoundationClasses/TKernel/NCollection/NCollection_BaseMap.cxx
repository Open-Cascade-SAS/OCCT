// Created on: 2002-04-18
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Purpose:     Implementation of the BaseMap class

#include <NCollection_BaseMap.hxx>
#include <NCollection_Primes.hxx>

#include <cstring>

namespace
{
NCollection_ListNode** allocateBuckets(const size_t theNbBuckets)
{
  const size_t aCount = theNbBuckets + 1;
  const size_t aBytes = aCount * sizeof(NCollection_ListNode*);
  NCollection_ListNode** aBuckets =
    static_cast<NCollection_ListNode**>(Standard::Allocate(aBytes));
  std::memset(aBuckets, 0, aBytes);
  return aBuckets;
}
} // namespace

//=================================================================================================

bool NCollection_BaseMap::beginResize(const size_t            theExtent,
                                      size_t&                 theNewBuckets,
                                      NCollection_ListNode**& data1) const
{
  // get next size for the buckets array
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
  data1 = allocateBuckets(theNewBuckets);
  return true;
}

//=================================================================================================

void NCollection_BaseMap::endResize(const size_t           theExtent,
                                    const size_t           theNewBuckets,
                                    NCollection_ListNode** data1) noexcept
{
  (void)theExtent; // obsolete parameter
  Standard::Free(myData1);
  myNbBuckets = theNewBuckets;
  myData1     = data1;
}

//=================================================================================================

void NCollection_BaseMap::destroy(NCollection_DelMapNode fDel, bool doReleaseMemory)
{
  if (!IsEmpty())
  {
    const size_t aNbBuckets = NbBuckets();
    for (size_t anInd = 0; anInd <= aNbBuckets; ++anInd)
    {
      if (myData1[anInd])
      {
        NCollection_ListNode* aCur = myData1[anInd];
        while (aCur)
        {
          NCollection_ListNode* aNext = aCur->Next();
          fDel(aCur, myAllocator);
          aCur = aNext;
        }
        myData1[anInd] = nullptr;
      }
    }
    mySize = 0;
  }
  if (doReleaseMemory)
  {
    Standard::Free(myData1);
    myData1 = nullptr;
  }
}

//=================================================================================================

size_t NCollection_BaseMap::nextPrimeForMap(const size_t N) const noexcept
{
  return NCollection_Primes::NextPrimeForMap(N);
}
