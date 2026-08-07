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

#include <NCollection_BaseIndexedMap.hxx>

#include <cstring>
#include <utility>

//=================================================================================================

NCollection_BaseIndexedMap::NCollection_BaseIndexedMap(
  const size_t                                  theNbBuckets,
  const occ::handle<NCollection_BaseAllocator>& theAllocator)
    : NCollection_BaseMap(theNbBuckets, theAllocator)
{
}

//=================================================================================================

NCollection_BaseIndexedMap::~NCollection_BaseIndexedMap()
{
  Standard::Free(myData2);
}

//=================================================================================================

void NCollection_BaseIndexedMap::resizeIndexTable(const size_t theNewBuckets)
{
  const size_t aNewCount = theNewBuckets + 1;
  const size_t anOldCount = myData1 == nullptr || myData2 == nullptr ? 0 : NbBuckets() + 1;
  const size_t aNewBytes  = aNewCount * sizeof(NCollection_ListNode*);
  myData2 = static_cast<NCollection_ListNode**>(Standard::Reallocate(myData2, aNewBytes));
  if (anOldCount == 0)
  {
    std::memset(myData2, 0, aNewBytes);
  }
  else if (aNewCount > anOldCount)
  {
    std::memset(myData2 + anOldCount,
                0,
                (aNewCount - anOldCount) * sizeof(NCollection_ListNode*));
  }
}

//=================================================================================================

void NCollection_BaseIndexedMap::clearIndexTable(const bool theReleaseMemory) noexcept
{
  if (myData2 == nullptr)
  {
    return;
  }

  std::memset(myData2, 0, (NbBuckets() + 1) * sizeof(NCollection_ListNode*));
  if (theReleaseMemory)
  {
    Standard::Free(myData2);
    myData2 = nullptr;
  }
}

//=================================================================================================

void NCollection_BaseIndexedMap::exchangeIndexedMapsData(
  NCollection_BaseIndexedMap& theOther) noexcept
{
  exchangeMapsData(theOther);
  std::swap(myData2, theOther.myData2);
}
