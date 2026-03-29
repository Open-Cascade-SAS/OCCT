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

#ifndef _BRepGraph_RefUID_HeaderFile
#define _BRepGraph_RefUID_HeaderFile

#include <BRepGraph_RefId.hxx>

#include <cstddef>
#include <cstdint>
#include <functional>

//! Unique reference identifier within a BRepGraph.
//!
//! Identity = (RefKind, Counter). Generation is excluded from equality/hash.
//! Counter 0 is an invalid sentinel.
//!
//! ## Serialization Contract
//!
//! Entity UIDs (BRepGraph_UID) and reference UIDs (BRepGraph_RefUID) share
//! a single monotonic counter (BRepGraph_Data::myNextUIDCounter).
//! To persist a BRepGraph across sessions:
//! 1. Write: for each reference entry, serialize (RefKind, Counter, OwnGen).
//! 2. Read: reconstruct reference entries, populate RefUID vectors with
//!    deserialized (RefKind, Counter) values, set myNextUIDCounter to
//!    max(all_entity_counters, all_ref_counters) + 1.
//! 3. myGeneration resets to 0 on load (session-scoped).
//! 4. VersionStamps from a previous session will correctly detect staleness
//!    via Generation mismatch.
struct BRepGraph_RefUID
{
  BRepGraph_RefUID()
      : myCounter(0),
        myKind(BRepGraph_RefId::Kind::Shell),
        myGeneration(0)
  {
  }

  BRepGraph_RefUID(const BRepGraph_RefId::Kind theKind,
                   const size_t                theCounter,
                   const uint32_t              theGeneration)
      : myCounter(theCounter),
        myKind(theKind),
        myGeneration(theGeneration)
  {
    Standard_ASSERT_VOID(theCounter > 0, "BRepGraph_RefUID: counter must be > 0 for valid UIDs");
  }

  static BRepGraph_RefUID Invalid() { return BRepGraph_RefUID(); }

  [[nodiscard]] bool IsValid() const { return myCounter > 0; }

  [[nodiscard]] BRepGraph_RefId::Kind Kind() const { return myKind; }

  [[nodiscard]] size_t Counter() const { return myCounter; }

  [[nodiscard]] uint32_t Generation() const { return myGeneration; }

  bool operator==(const BRepGraph_RefUID& theOther) const
  {
    if (myCounter == 0 || theOther.myCounter == 0)
      return (myCounter == 0) == (theOther.myCounter == 0);
    return myKind == theOther.myKind && myCounter == theOther.myCounter;
  }

  bool operator!=(const BRepGraph_RefUID& theOther) const { return !(*this == theOther); }

  bool operator<(const BRepGraph_RefUID& theOther) const
  {
    if (myKind != theOther.myKind)
      return static_cast<int>(myKind) < static_cast<int>(theOther.myKind);
    return myCounter < theOther.myCounter;
  }

  [[nodiscard]] size_t HashValue() const
  {
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(myKind));
    aCombination[1] = opencascade::hash(myCounter);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }

private:
  size_t                myCounter;
  BRepGraph_RefId::Kind myKind;
  uint32_t              myGeneration;
};

template <>
struct std::hash<BRepGraph_RefUID>
{
  size_t operator()(const BRepGraph_RefUID& theUID) const noexcept { return theUID.HashValue(); }
};

#endif // _BRepGraph_RefUID_HeaderFile
