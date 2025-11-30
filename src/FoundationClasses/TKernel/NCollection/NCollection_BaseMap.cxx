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

#include <NCollection_Array1.hxx>

//=================================================================================================

bool NCollection_BaseMap::BeginResize(const size_t            theNbBuckets,
                                      size_t&                 theNewBuckets,
                                      NCollection_ListNode**& theData1,
                                      NCollection_ListNode**& theData2) const
{
  // get next size for the buckets array
  theNewBuckets = NextPrimeForMap(theNbBuckets);
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
  theData1 =
    static_cast<NCollection_ListNode**>(Standard::Allocate(theNewBuckets * sizeof(NCollection_ListNode*)));
  if (myIsDouble)
  {
    theData2 =
      static_cast<NCollection_ListNode**>(Standard::Allocate(theNewBuckets * sizeof(NCollection_ListNode*)));
  }
  else
  {
    theData2 = nullptr;
  }
  return true;
}

//=================================================================================================

void NCollection_BaseMap::EndResize(const size_t           theNewBuckets,
                                    NCollection_ListNode** theData1,
                                    NCollection_ListNode** theData2) noexcept
{
  if (myData1)
  {
    Standard::Free(myData1);
  }
  if (myData2 && myIsDouble)
  {
    Standard::Free(myData2);
  }
  myNbBuckets = theNewBuckets;
  myData1     = theData1;
  myData2     = theData2;
}

//=================================================================================================

void NCollection_BaseMap::Destroy(NCollection_DelMapNode fDel, bool doReleaseMemory)
{
  if (!IsEmpty())
  {
    const size_t aNbBuckets = NbBuckets();
    for (size_t anInd = 0; anInd < aNbBuckets; ++anInd)
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
    if (myData2)
    {
      memset(myData2, 0, aNbBuckets * sizeof(NCollection_ListNode*));
    }
    mySize = 0;
  }
  if (doReleaseMemory)
  {
    if (myData1)
    {
      Standard::Free(myData1);
    }
    if (myData2)
    {
      Standard::Free(myData2);
    }
    myData1 = myData2 = nullptr;
  }
}

//=================================================================================================

void NCollection_BaseMap::Statistics(Standard_OStream& S) const
{
  S << "\nMap Statistics\n---------------\n\n";
  S << "This Map has " << myNbBuckets << " Buckets and " << mySize << " Keys\n\n";

  if (mySize == 0)
  {
    return;
  }

  // compute statistics on bucket chain lengths
  NCollection_Array1<size_t> aSizes(0, static_cast<Standard_Integer>(mySize));
  aSizes.Init(0);

  S << "\nStatistics for the first Key\n";
  size_t aNbNonEmpty = 0;
  for (size_t i = 0; i < myNbBuckets; ++i)
  {
    size_t aLen = 0;
    for (NCollection_ListNode* p = myData1[i]; p != nullptr; p = p->Next())
    {
      ++aLen;
    }
    if (aLen > 0)
    {
      ++aNbNonEmpty;
    }
    ++aSizes[static_cast<Standard_Integer>(aLen)];
  }

  // display results
  size_t aTotalLen = 0;
  for (Standard_Integer i = 0; i <= static_cast<Standard_Integer>(mySize); ++i)
  {
    if (aSizes[i] > 0)
    {
      aTotalLen += aSizes[i] * i;
      S << std::setw(5) << aSizes[i] << " buckets of size " << i << "\n";
    }
  }

  const double aMean = aNbNonEmpty > 0 ? static_cast<double>(aTotalLen) / static_cast<double>(aNbNonEmpty) : 0.0;
  S << "\n\nMean of length : " << aMean << "\n";
}

//=================================================================================================

size_t NCollection_BaseMap::NextPrimeForMap(const size_t theN) const noexcept
{
  // Use power of 2 for bucket count to enable fast modulo via bitwise AND.
  // Minimum bucket count is 128 to ensure reasonable initial capacity.
  constexpr size_t THE_MIN_BUCKETS = 128;
  if (theN < THE_MIN_BUCKETS)
  {
    return THE_MIN_BUCKETS;
  }
  // Round up to next power of 2.
  // If theN is already a power of 2, return theN; otherwise return the next power of 2.
  size_t aValue = theN;
  // Check if already power of 2
  if ((aValue & (aValue - 1)) == 0)
  {
    return aValue;
  }
  // Find next power of 2 by filling all bits below the highest set bit, then adding 1
  aValue |= aValue >> 1;
  aValue |= aValue >> 2;
  aValue |= aValue >> 4;
  aValue |= aValue >> 8;
  aValue |= aValue >> 16;
  if constexpr (sizeof(size_t) > 4)
  {
    aValue |= aValue >> 32;
  }
  return aValue + 1;
}
