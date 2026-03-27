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

#ifndef _BRepGraph_UserAttribute_HeaderFile
#define _BRepGraph_UserAttribute_HeaderFile

#include <NCollection_DataMap.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Transient.hxx>

#include <atomic>
#include <functional>
#include <mutex>
#include <shared_mutex>

// ============================================================
// BRepGraph_AttrRegistry
// ============================================================

//! Process-global registry mapping Standard_GUID <-> int
//! for user-defined graph attributes.
//!
//! Integer keys are faster for hash-map operations than 128-bit GUIDs,
//! so per-node attribute storage uses integers.  GUIDs provide stable,
//! cross-process identity for serialization and plugin interop.
//!
//! The registry is a singleton with thread-safe registration.
//! All methods are static.
class BRepGraph_AttrRegistry
{
public:
  //! Register a GUID and obtain its integer key.
  //! Idempotent: same GUID always yields same int.
  //! Thread-safe.
  //!
  //! @param theGUID  Attribute kind identifier.
  //! @return         The integer key to use in Set/Get/Invalidate operations.
  static int Register(const Standard_GUID& theGUID);

  //! Look up the integer key for a previously registered GUID.
  //!
  //! @param theGUID  Attribute kind identifier.
  //! @param theKey   [out] The integer key, if found.
  //! @return         True if the GUID is registered.
  static bool Find(const Standard_GUID& theGUID, int& theKey);

  //! Look up the GUID for a previously registered integer key.
  //!
  //! @param theKey   Integer key from Register().
  //! @param theGUID  [out] The GUID, if found.
  //! @return         True if the key is registered.
  static bool Find(const int theKey, Standard_GUID& theGUID);

  //! Check whether a GUID has been registered.
  static bool Contains(const Standard_GUID& theGUID);

  //! Check whether an integer key has been registered.
  static bool Contains(const int theKey);

  //! Total number of registered attribute kinds.
  static int NbRegistered();

  //! Allocate an anonymous integer key (no GUID association).
  //! Uses the same atomic counter as Register(), so anonymous keys
  //! and GUID-registered keys never collide.
  static int AllocateAnonymous() { return nextKey().fetch_add(1, std::memory_order_relaxed); }

private:
  BRepGraph_AttrRegistry() = delete;

  //! The bidirectional maps (lazily initialized on first use).
  static NCollection_DataMap<Standard_GUID, int>& guidToInt();
  static NCollection_DataMap<int, Standard_GUID>& intToGuid();

  //! Next integer key to allocate.
  static std::atomic<int>& nextKey();

  //! Reader-writer lock protecting both maps.
  static std::shared_mutex& mutex();
};

//! Inline implementation kept in the header (small registry, few TUs).

inline NCollection_DataMap<Standard_GUID, int>& BRepGraph_AttrRegistry::guidToInt()
{
  static NCollection_DataMap<Standard_GUID, int> aMap;
  return aMap;
}

inline NCollection_DataMap<int, Standard_GUID>& BRepGraph_AttrRegistry::intToGuid()
{
  static NCollection_DataMap<int, Standard_GUID> aMap;
  return aMap;
}

inline std::atomic<int>& BRepGraph_AttrRegistry::nextKey()
{
  static std::atomic<int> aCounter{0};
  return aCounter;
}

inline std::shared_mutex& BRepGraph_AttrRegistry::mutex()
{
  static std::shared_mutex aMutex;
  return aMutex;
}

inline int BRepGraph_AttrRegistry::Register(const Standard_GUID& theGUID)
{
  // Fast path: already registered?
  {
    std::shared_lock<std::shared_mutex> aReadLock(mutex());
    const int*                          aPtr = guidToInt().Seek(theGUID);
    if (aPtr != nullptr)
      return *aPtr;
  }
  // Slow path: allocate under exclusive lock.
  std::unique_lock<std::shared_mutex> aWriteLock(mutex());
  const int*                          aPtr = guidToInt().Seek(theGUID);
  if (aPtr != nullptr)
    return *aPtr;

  int aNewKey = nextKey().fetch_add(1, std::memory_order_relaxed);
  guidToInt().Bind(theGUID, aNewKey);
  intToGuid().Bind(aNewKey, theGUID);
  return aNewKey;
}

inline bool BRepGraph_AttrRegistry::Find(const Standard_GUID& theGUID, int& theKey)
{
  std::shared_lock<std::shared_mutex> aLock(mutex());
  const int*                          aPtr = guidToInt().Seek(theGUID);
  if (aPtr == nullptr)
    return false;
  theKey = *aPtr;
  return true;
}

inline bool BRepGraph_AttrRegistry::Find(const int theKey, Standard_GUID& theGUID)
{
  std::shared_lock<std::shared_mutex> aLock(mutex());
  const Standard_GUID*                aPtr = intToGuid().Seek(theKey);
  if (aPtr == nullptr)
    return false;
  theGUID = *aPtr;
  return true;
}

inline bool BRepGraph_AttrRegistry::Contains(const Standard_GUID& theGUID)
{
  std::shared_lock<std::shared_mutex> aLock(mutex());
  return guidToInt().IsBound(theGUID);
}

inline bool BRepGraph_AttrRegistry::Contains(const int theKey)
{
  std::shared_lock<std::shared_mutex> aLock(mutex());
  return intToGuid().IsBound(theKey);
}

inline int BRepGraph_AttrRegistry::NbRegistered()
{
  std::shared_lock<std::shared_mutex> aLock(mutex());
  return guidToInt().Extent();
}

// ============================================================
// BRepGraph_UserAttribute
// ============================================================

//! Abstract base for user-defined per-node cached attributes.
//!
//! Inherits from Standard_Transient, stored via occ::handle<BRepGraph_UserAttribute>.
//! This uses OCCT's embedded refcount (zero extra allocation) and is consistent
//! with the Handle pattern used throughout the codebase.
//!
//! Each attribute kind is identified by an int key for fast runtime lookups.
//! Keys can be obtained via GUID-based registration (recommended for public
//! attributes) or anonymous allocation (for private/ephemeral caches).
//!
//! @code
//!   // GUID-based (recommended):
//!   static const Standard_GUID MY_GUID("a3f1c4e2-7b09-4d5a-8e6f-1a2b3c4d5e6f");
//!   int KEY = BRepGraph_UserAttribute::AllocateKey(MY_GUID);
//!
//!   // Anonymous:
//!   static const int KEY = BRepGraph_UserAttribute::AllocateKey();
//! @endcode
class BRepGraph_UserAttribute : public Standard_Transient
{
public:
  //! Allocate an anonymous integer key (no GUID association).
  //! Thread-safe (shared atomic counter with BRepGraph_AttrRegistry).
  static int AllocateKey() { return BRepGraph_AttrRegistry::AllocateAnonymous(); }

  //! Allocate (or retrieve) an integer key associated with a GUID.
  //! Convenience wrapper around BRepGraph_AttrRegistry::Register().
  //! Idempotent: calling with the same GUID always returns the same integer.
  static int AllocateKey(const Standard_GUID& theGUID)
  {
    return BRepGraph_AttrRegistry::Register(theGUID);
  }

  //! Mark the attribute as needing recomputation.  Lock-free.
  void Invalidate() { myDirty.store(true, std::memory_order_release); }

  //! True if the attribute needs recomputation.
  bool IsDirty() const { return myDirty.load(std::memory_order_acquire); }

  DEFINE_STANDARD_RTTI_INLINE(BRepGraph_UserAttribute, Standard_Transient)

protected:
  BRepGraph_UserAttribute()
      : myDirty(true)
  {
  }

  //! Subclass calls after successful computation to clear the dirty flag.
  void MarkClean() const { myDirty.store(false, std::memory_order_release); }

  //! Mutex for thread-safe Get() in subclasses.
  mutable std::shared_mutex myMutex;

private:
  mutable std::atomic<bool> myDirty;
};

// ============================================================
// BRepGraph_TypedAttribute<T>
// ============================================================

//! Concrete typed wrapper for a lazily-computed per-node value.
//!
//! @tparam T  Cached value type (e.g. double for curvature).
//!            Must be default-constructible and copy-assignable.
template <typename T>
class BRepGraph_TypedAttribute : public BRepGraph_UserAttribute
{
public:
  BRepGraph_TypedAttribute() = default;

  //! Construct with an initial value (marked clean).
  explicit BRepGraph_TypedAttribute(const T& theInitial)
      : myValue(theInitial)
  {
    MarkClean();
  }

  //! Get the cached value, computing via theComputer if dirty.
  //! Thread-safe: uses the base class shared_mutex.
  T Get(const std::function<T()>& theComputer) const
  {
    // Fast path
    if (!IsDirty())
    {
      std::shared_lock<std::shared_mutex> aLock(myMutex);
      if (!IsDirty())
        return myValue;
    }
    // Slow path
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    if (IsDirty())
    {
      myValue = theComputer();
      MarkClean();
    }
    return myValue;
  }

  //! Direct write - stores the value and marks clean.
  void Set(const T& theValue)
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    myValue = theValue;
    MarkClean();
  }

  //! Direct read.  Caller must guarantee freshness.
  const T& UncheckedValue() const { return myValue; }

private:
  mutable T myValue{};
};

#endif // _BRepGraph_UserAttribute_HeaderFile
