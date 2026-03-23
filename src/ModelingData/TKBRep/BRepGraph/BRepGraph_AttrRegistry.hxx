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

#ifndef _BRepGraph_AttrRegistry_HeaderFile
#define _BRepGraph_AttrRegistry_HeaderFile

#include <Standard_GUID.hxx>
#include <NCollection_DataMap.hxx>

#include <shared_mutex>
#include <atomic>

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
  static bool Find(int theKey, Standard_GUID& theGUID);

  //! Check whether a GUID has been registered.
  static bool Contains(const Standard_GUID& theGUID);

  //! Check whether an integer key has been registered.
  static bool Contains(int theKey);

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

inline bool BRepGraph_AttrRegistry::Find(int theKey, Standard_GUID& theGUID)
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

inline bool BRepGraph_AttrRegistry::Contains(int theKey)
{
  std::shared_lock<std::shared_mutex> aLock(mutex());
  return intToGuid().IsBound(theKey);
}

inline int BRepGraph_AttrRegistry::NbRegistered()
{
  std::shared_lock<std::shared_mutex> aLock(mutex());
  return guidToInt().Extent();
}

#endif // _BRepGraph_AttrRegistry_HeaderFile
