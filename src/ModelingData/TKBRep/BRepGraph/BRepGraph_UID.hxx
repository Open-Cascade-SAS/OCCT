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

#ifndef _BRepGraph_UID_HeaderFile
#define _BRepGraph_UID_HeaderFile

#include <BRepGraph_NodeId.hxx>

#include <cstddef>
#include <cstdint>
#include <functional>

//! Unique node identifier within a BRepGraph.
//!
//! Identity = (Kind, Counter).  Two nodes of different kinds may share a
//! counter value but their UIDs are distinct.  Within one kind, counter
//! values never repeat (monotonic, never resets).
//!
//! Generation is NOT part of identity; it indicates which BRepGraph_Builder::Add() cycle
//! produced this UID (for stale-reference detection).
//!
//! Trivially copyable, cheap to pass by value.
//!
//! ## Serialization Contract
//!
//! Entity UIDs (BRepGraph_UID) and reference UIDs (BRepGraph_RefUID) share
//! a single monotonic counter (BRepGraph_Data::myNextUIDCounter).
//! To persist a BRepGraph across sessions:
//! 1. Write: for each entity, serialize (Kind, Counter, OwnGen).
//! 2. Read: reconstruct entities, populate UID vectors with deserialized
//!    (Kind, Counter) values, set myNextUIDCounter to
//!    max(all_entity_counters, all_ref_counters) + 1.
//! 3. myGeneration resets to 0 on load (session-scoped).
//! 4. VersionStamps from a previous session will correctly detect staleness
//!    via Generation mismatch.
struct BRepGraph_UID
{
  //! Default: invalid UID (counter = 0 is the invalid sentinel).
  BRepGraph_UID()
      : myCounter(0),
        myKind(BRepGraph_NodeId::Kind::Solid),
        myGeneration(0)
  {
  }

  //! Construct a valid UID.  Called internally by BRepGraph::allocateUID().
  //! @pre theCounter > 0 (counter = 0 is reserved as the invalid sentinel)
  BRepGraph_UID(const BRepGraph_NodeId::Kind theKind,
                const size_t                 theCounter,
                const uint32_t               theGeneration)
      : myCounter(theCounter),
        myKind(theKind),
        myGeneration(theGeneration)
  {
    Standard_ASSERT_VOID(theCounter > 0, "BRepGraph_UID: counter must be > 0 for valid UIDs");
  }

  //! Factory: returns an explicitly invalid UID.
  static BRepGraph_UID Invalid() { return BRepGraph_UID(); }

  [[nodiscard]] bool IsValid() const { return myCounter > 0; }

  [[nodiscard]] BRepGraph_NodeId::Kind Kind() const { return myKind; }

  [[nodiscard]] size_t Counter() const { return myCounter; }

  [[nodiscard]] uint32_t Generation() const { return myGeneration; }

  [[nodiscard]] bool IsTopology() const { return BRepGraph_NodeId::IsTopologyKind(myKind); }

  [[nodiscard]] bool IsAssembly() const { return BRepGraph_NodeId::IsAssemblyKind(myKind); }

  //! Equality: Identity = (Kind, Counter).  Generation excluded.
  //! Two invalid UIDs are equal.
  bool operator==(const BRepGraph_UID& theOther) const
  {
    if (myCounter == 0 || theOther.myCounter == 0)
      return (myCounter == 0) == (theOther.myCounter == 0);
    return myKind == theOther.myKind && myCounter == theOther.myCounter;
  }

  bool operator!=(const BRepGraph_UID& theOther) const { return !(*this == theOther); }

  bool operator<(const BRepGraph_UID& theOther) const
  {
    if (myKind != theOther.myKind)
      return static_cast<int>(myKind) < static_cast<int>(theOther.myKind);
    return myCounter < theOther.myCounter;
  }

  //! Hash value: f(Kind, Counter).
  [[nodiscard]] size_t HashValue() const
  {
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(myKind));
    aCombination[1] = opencascade::hash(myCounter);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }

private:
  size_t                 myCounter; //!< 0 = invalid sentinel; valid counters start at 1.
  BRepGraph_NodeId::Kind myKind;    //!< Node kind.
  uint32_t myGeneration;            //!< BRepGraph_Builder::Add() cycle that produced this UID.
};

//! std::hash specialization for NCollection_DefaultHasher support.
template <>
struct std::hash<BRepGraph_UID>
{
  size_t operator()(const BRepGraph_UID& theUID) const noexcept { return theUID.HashValue(); }
};

#endif // _BRepGraph_UID_HeaderFile
