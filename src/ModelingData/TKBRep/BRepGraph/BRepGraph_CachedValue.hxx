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

#ifndef _BRepGraph_CachedValue_HeaderFile
#define _BRepGraph_CachedValue_HeaderFile

#include <shared_mutex>
#include <atomic>

//! Thread-safe lazy cache for a single computed value of type T.
//!
//! Read path (fast):  atomic dirty check -> shared lock -> return value.
//! Compute path:      exclusive lock -> double-check -> compute -> store -> clear dirty.
//!
//! @tparam T  Must be default-constructible and copy-assignable.
template <typename T>
class BRepGraph_CachedValue
{
public:
  BRepGraph_CachedValue()
      : myDirty(true)
  {
  }

  //! Mark cache as stale.  Lock-free (atomic store).
  void Invalidate() { myDirty.store(true, std::memory_order_release); }

  //! True if the cache needs recomputation.
  bool IsDirty() const { return myDirty.load(std::memory_order_acquire); }

  //! Get the cached value, computing it via theComputer if dirty.
  //! Thread-safe: concurrent readers trigger at most one computation.
  //!
  //! @tparam Func  Callable returning T; invoked under exclusive lock.
  template <typename Func>
  T Get(const Func& theComputer) const
  {
    // Fast path
    if (!myDirty.load(std::memory_order_acquire))
    {
      std::shared_lock<std::shared_mutex> aReadLock(myMutex);
      if (!myDirty.load(std::memory_order_acquire))
        return myValue;
    }
    // Slow path -- compute under exclusive lock
    std::unique_lock<std::shared_mutex> aWriteLock(myMutex);
    if (myDirty.load(std::memory_order_acquire))
    {
      myValue = theComputer();
      myDirty.store(false, std::memory_order_release);
    }
    return myValue;
  }

  //! Direct read.  Caller must guarantee the cache is fresh (not dirty).
  const T& UncheckedValue() const { return myValue; }

private:
  mutable T                 myValue;
  mutable std::atomic<bool> myDirty;
  mutable std::shared_mutex myMutex;
};

#endif // _BRepGraph_CachedValue_HeaderFile
