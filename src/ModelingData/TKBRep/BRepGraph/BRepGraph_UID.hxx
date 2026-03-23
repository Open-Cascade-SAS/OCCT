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
//! Generation is NOT part of identity; it indicates which Build() cycle
//! produced this UID (for stale-reference detection).
//!
//! 24 bytes on 64-bit (4 + 8 + 4 + 1 + 7 padding).  Trivially copyable, cheap to pass by value.
struct BRepGraph_UID
{
  //! Default: invalid UID.
  BRepGraph_UID()
    : myKind(BRepGraph_NodeId::Kind::Solid), myCounter(0),
      myGeneration(0), myValid(false) {}

  //! Construct a valid UID.  Called internally by BRepGraph::allocateUID().
  BRepGraph_UID(BRepGraph_NodeId::Kind theKind,
                size_t             theCounter,
                uint32_t           theGeneration)
    : myKind(theKind), myCounter(theCounter),
      myGeneration(theGeneration), myValid(true) {}

  //! Factory: returns an explicitly invalid UID.
  static BRepGraph_UID Invalid() { return BRepGraph_UID(); }

  bool               IsValid()    const { return myValid; }
  BRepGraph_NodeId::Kind Kind()       const { return myKind; }
  size_t             Counter()    const { return myCounter; }
  uint32_t           Generation() const { return myGeneration; }

  bool IsTopology() const { return static_cast<int>(myKind) <= 7; }

  //! Equality: Identity = (Kind, Counter).  Generation excluded.
  bool operator==(const BRepGraph_UID& theOther) const
  {
    if (!myValid || !theOther.myValid)
      return myValid == theOther.myValid;
    return myKind    == theOther.myKind
        && myCounter == theOther.myCounter;
  }

  bool operator!=(const BRepGraph_UID& theOther) const
  { return !(*this == theOther); }

  bool operator<(const BRepGraph_UID& theOther) const
  {
    if (myKind != theOther.myKind)
      return static_cast<int>(myKind) < static_cast<int>(theOther.myKind);
    return myCounter < theOther.myCounter;
  }

  //! Hash value: f(Kind, Counter).
  size_t HashValue() const
  {
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(myKind));
    aCombination[1] = opencascade::hash(myCounter);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }

private:
  BRepGraph_NodeId::Kind myKind;
  size_t             myCounter;
  uint32_t           myGeneration;
  bool               myValid;
};

//! std::hash specialization for NCollection_DefaultHasher support.
template<>
struct std::hash<BRepGraph_UID>
{
  size_t operator()(const BRepGraph_UID& theUID) const noexcept
  { return theUID.HashValue(); }
};

#endif // _BRepGraph_UID_HeaderFile
