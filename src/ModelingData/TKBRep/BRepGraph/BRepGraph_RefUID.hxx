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
#include <Standard_Assert.hxx>
#include <Standard_HashUtils.hxx>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>

//! Unique reference-entry identifier within a BRepGraph.
//!
//! Identity = (RefKind, Counter). Counter 0 is an invalid sentinel.
struct BRepGraph_RefUID
{
  BRepGraph_RefId::Kind Kind    = BRepGraph_RefId::Kind::Shell;
  uint32_t              Counter = 0;

  BRepGraph_RefUID() = default;

  BRepGraph_RefUID(const BRepGraph_RefId::Kind theKind, const uint32_t theCounter)
      : Kind(theKind),
        Counter(theCounter)
  {
  }

  static BRepGraph_RefUID Invalid() { return BRepGraph_RefUID(); }

  //! True if this UID has a valid kind and a non-zero counter.
  [[nodiscard]] bool IsValid() const { return Counter > 0 && BRepGraph_RefId::IsValidKind(Kind); }

  friend bool operator==(const BRepGraph_RefUID& theLeft, const BRepGraph_RefUID& theRight) noexcept
  {
    if (theLeft.Counter == 0 || theRight.Counter == 0)
    {
      return (theLeft.Counter == 0) == (theRight.Counter == 0);
    }
    return theLeft.Kind == theRight.Kind && theLeft.Counter == theRight.Counter;
  }

  friend bool operator!=(const BRepGraph_RefUID& theLeft, const BRepGraph_RefUID& theRight) noexcept
  {
    return !(theLeft == theRight);
  }

  friend bool operator<(const BRepGraph_RefUID& theLeft, const BRepGraph_RefUID& theRight) noexcept
  {
    if (theLeft.Kind != theRight.Kind)
    {
      return static_cast<int>(theLeft.Kind) < static_cast<int>(theRight.Kind);
    }
    return theLeft.Counter < theRight.Counter;
  }

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

static_assert(sizeof(BRepGraph_RefUID) <= 8, "BRepGraph_RefUID must stay compact");

template <>
struct std::hash<BRepGraph_RefUID>
{
  size_t operator()(const BRepGraph_RefUID& theUID) const noexcept { return theUID.HashValue(); }
};

#endif // _BRepGraph_RefUID_HeaderFile
