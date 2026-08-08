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

#ifndef NCollection_BaseDoubleMap_HeaderFile
#define NCollection_BaseDoubleMap_HeaderFile

#include <NCollection_BaseMap.hxx>

//! Non-template storage base for maps indexed by two independent hash tables.
class NCollection_BaseDoubleMap : public NCollection_BaseMap
{
protected:
  NCollection_BaseDoubleMap(const size_t                                  theNbBuckets,
                            const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : NCollection_BaseMap(theNbBuckets, theAllocator)
  {
  }

  ~NCollection_BaseDoubleMap() = default;

  //! Allocate a new pair of bucket tables for a resize operation.
  Standard_EXPORT bool beginResize(const size_t            theExtent,
                                   size_t&                 theNewBuckets,
                                   NCollection_ListNode**& theData1,
                                   NCollection_ListNode**& theData2) const;

  //! Commit a pair of bucket tables after nodes have been relinked.
  Standard_EXPORT void endResize(const size_t           theExtent,
                                 const size_t           theNewBuckets,
                                 NCollection_ListNode** theData1,
                                 NCollection_ListNode** theData2) noexcept;

  //! Destroy nodes and optionally release both bucket tables.
  Standard_EXPORT void destroy(NCollection_DelMapNode fDel, bool theReleaseMemory = true);

  //! Exchange both bucket tables and common map state.
  Standard_EXPORT void exchangeDoubleMapsData(NCollection_BaseDoubleMap& theOther) noexcept;

protected:
  NCollection_ListNode** myData2 = nullptr;
};

#endif // NCollection_BaseDoubleMap_HeaderFile
