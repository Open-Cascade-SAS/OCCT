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

#ifndef NCollection_BaseIndexedMap_HeaderFile
#define NCollection_BaseIndexedMap_HeaderFile

#include <NCollection_BaseMap.hxx>

//! Non-template storage base for maps with an index-to-node table.
class NCollection_BaseIndexedMap : public NCollection_BaseMap
{
protected:
  Standard_EXPORT NCollection_BaseIndexedMap(
    const size_t                                  theNbBuckets,
    const occ::handle<NCollection_BaseAllocator>& theAllocator);

  Standard_EXPORT ~NCollection_BaseIndexedMap();

  //! Resize the index-to-node table independently from hash buckets.
  Standard_EXPORT void resizeIndexTable(const size_t theNewBuckets);

  //! Clear the index-to-node table and optionally release its storage.
  Standard_EXPORT void clearIndexTable(const bool theReleaseMemory) noexcept;

  //! Exchange hash buckets and index-to-node table.
  Standard_EXPORT void exchangeIndexedMapsData(NCollection_BaseIndexedMap& theOther) noexcept;

protected:
  NCollection_ListNode** myData2 = nullptr;
};

#endif // NCollection_BaseIndexedMap_HeaderFile
