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
#include <Standard_Assert.hxx>
#include <Standard_HashUtils.hxx>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>

//! Unique definition-node identifier within a BRepGraph.
//!
//! Identity = (Kind, Counter). Two nodes of different kinds may share a
//! counter value but their UIDs are distinct.  Within one kind, counter
//! values never repeat (monotonic, never resets).
//!
//! Trivially copyable, cheap to pass by value.
struct BRepGraph_UID
{
  BRepGraph_NodeId::Kind Kind    = BRepGraph_NodeId::Kind::Solid;
  uint32_t               Counter = 0;

  //! Default: invalid UID (counter = 0 is the invalid sentinel).
  BRepGraph_UID() = default;

  //! Construct a valid UID.  Called internally by BRepGraphInc_Storage::AllocateNodeUID().
  //! @pre theCounter > 0 (counter = 0 is reserved as the invalid sentinel)
  BRepGraph_UID(const BRepGraph_NodeId::Kind theKind, const uint32_t theCounter)
      : Kind(theKind),
        Counter(theCounter)
  {
  }

  //! Factory: returns an explicitly invalid UID.
  static BRepGraph_UID Invalid() { return BRepGraph_UID(); }

  //! True if this UID has a valid kind and a non-zero counter.
  [[nodiscard]] bool IsValid() const { return Counter > 0 && BRepGraph_NodeId::IsValidKind(Kind); }

  [[nodiscard]] bool IsTopology() const
  {
    return IsValid() && BRepGraph_NodeId::IsTopologyKind(Kind);
  }

  [[nodiscard]] bool IsAssembly() const
  {
    return IsValid() && BRepGraph_NodeId::IsAssemblyKind(Kind);
  }

  //! Equality: Identity = (Kind, Counter). Two invalid UIDs are equal.
  friend bool operator==(const BRepGraph_UID& theLeft, const BRepGraph_UID& theRight) noexcept
  {
    if (theLeft.Counter == 0 || theRight.Counter == 0)
    {
      return (theLeft.Counter == 0) == (theRight.Counter == 0);
    }
    return theLeft.Kind == theRight.Kind && theLeft.Counter == theRight.Counter;
  }

  friend bool operator!=(const BRepGraph_UID& theLeft, const BRepGraph_UID& theRight) noexcept
  {
    return !(theLeft == theRight);
  }

  friend bool operator<(const BRepGraph_UID& theLeft, const BRepGraph_UID& theRight) noexcept
  {
    if (theLeft.Kind != theRight.Kind)
    {
      return static_cast<int>(theLeft.Kind) < static_cast<int>(theRight.Kind);
    }
    return theLeft.Counter < theRight.Counter;
  }

  //! Hash value compatible with operator==.
  [[nodiscard]] size_t HashValue() const noexcept
  {
    if (Counter == 0)
    {
      return opencascade::hash(0);
    }
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(Kind));
    aCombination[1] = opencascade::hash(Counter);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

static_assert(sizeof(BRepGraph_UID) <= 8, "BRepGraph_UID must stay compact");

//! std::hash specialization for NCollection_DefaultHasher support.
template <>
struct std::hash<BRepGraph_UID>
{
  size_t operator()(const BRepGraph_UID& theUID) const noexcept { return theUID.HashValue(); }
};

#endif // _BRepGraph_UID_HeaderFile
