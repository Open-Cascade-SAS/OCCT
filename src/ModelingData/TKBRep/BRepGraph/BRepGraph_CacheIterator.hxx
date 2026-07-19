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

#ifndef _BRepGraph_CacheIterator_HeaderFile
#define _BRepGraph_CacheIterator_HeaderFile

#include <BRepGraph_CacheRegistry.hxx>
#include <NCollection_ForwardRange.hxx>

//! @brief Iterator over registered cache families in a BRepGraph_CacheRegistry.
//!
//! Supports OCCT More()/Next()/Value() pattern and STL range-for via begin()/end().
class BRepGraph_CacheIterator
{
public:
  //! Construct an iterator over all cache families in the registry.
  explicit BRepGraph_CacheIterator(const BRepGraph_CacheRegistry& theRegistry)
      : myRegistry(&theRegistry),
        myCount(theRegistry.NbCaches())
  {
  }

  //! True if the iterator has a current element.
  [[nodiscard]] bool More() const { return myCurrent < myCount; }

  //! Advance to the next cache family.
  void Next() { ++myCurrent; }

  //! Return the current cache family descriptor.
  [[nodiscard]] occ::handle<BRepGraph_Cache> Value() const { return myRegistry->Cache(myCurrent); }

  //! Return the current slot index in the registry.
  [[nodiscard]] uint32_t Slot() const { return myCurrent; }

  //! Number of cache families in the registry.
  [[nodiscard]] uint32_t NbCaches() const { return myRegistry->NbCaches(); }

  //! STL range-for support.
  NCollection_ForwardRangeIterator<BRepGraph_CacheIterator> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_CacheIterator>(this);
  }

  //! Sentinel marking end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  const BRepGraph_CacheRegistry* myRegistry;
  uint32_t                       myCount;
  uint32_t                       myCurrent = 0;
};

#endif // _BRepGraph_CacheIterator_HeaderFile
