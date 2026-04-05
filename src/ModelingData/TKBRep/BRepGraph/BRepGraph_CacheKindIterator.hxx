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

#ifndef _BRepGraph_CacheKindIterator_HeaderFile
#define _BRepGraph_CacheKindIterator_HeaderFile

#include <BRepGraph_TransientCache.hxx>
#include <NCollection_ForwardRange.hxx>

//! @brief Zero-allocation iterator over populated cache kinds on a node or reference.
//!
//! Template parameter TKeyId is either BRepGraph_NodeId or BRepGraph_RefId.
//! Supports OCCT More()/Next()/Value() pattern and STL range-for via begin()/end().
//!
//! Constructed by BRepGraph::CacheView::CacheKindIter(). Stores populated
//! kind slot indices in a fixed-size stack buffer (no heap allocation).
//!
//! @code
//!   // Range-for:
//!   for (const occ::handle<BRepGraph_CacheKind>& aKind : aGraph.Cache().CacheKindIter(aNode))
//!     doSomething(aKind);
//!
//!   // Traditional:
//!   for (auto anIt = aGraph.Cache().CacheKindIter(aNode); anIt.More(); anIt.Next())
//!     doSomething(anIt.Value());
//! @endcode
template <typename TKeyId>
class BRepGraph_CacheKindIterator
{
public:
  //! True if the iterator has a current element.
  [[nodiscard]] bool More() const { return myCurrent < myCount; }

  //! Advance to the next populated cache kind.
  void Next() { ++myCurrent; }

  //! Return the current cache kind descriptor.
  [[nodiscard]] occ::handle<BRepGraph_CacheKind> Value() const
  {
    return BRepGraph_CacheKindRegistry::FindKind(mySlots[myCurrent]);
  }

  //! Return the current cache-kind slot index (for fast slot-based access).
  [[nodiscard]] int KindSlot() const { return mySlots[myCurrent]; }

  //! Number of populated cache kinds found.
  [[nodiscard]] int NbKinds() const { return myCount; }

  //! STL range-for support.
  NCollection_ForwardRangeIterator<BRepGraph_CacheKindIterator> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_CacheKindIterator>(this);
  }

  //! Sentinel marking end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  friend class BRepGraph::CacheView;
  static constexpr int THE_MAX_SLOTS = BRepGraph_TransientCache::THE_DEFAULT_RESERVED_KIND_COUNT;
  int                  mySlots[THE_MAX_SLOTS];
  int                  myCount   = 0;
  int                  myCurrent = 0;
};

#endif // _BRepGraph_CacheKindIterator_HeaderFile
