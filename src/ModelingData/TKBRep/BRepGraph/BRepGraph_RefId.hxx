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

#ifndef _BRepGraph_RefId_HeaderFile
#define _BRepGraph_RefId_HeaderFile

#include <Standard_Assert.hxx>
#include <Standard_HashUtils.hxx>

#include <cstddef>
#include <functional>

//! Lightweight typed index into a per-kind reference vector inside BRepGraph.
//!
//! The pair (Kind, Index) forms a unique reference identifier within one graph
//! instance. Default-constructed RefId has Index = -1 (invalid).
struct BRepGraph_RefId
{
  //! Enumeration of supported topology reference kinds.
  enum class Kind : int
  {
    Shell      = 0, //!< Shell reference entries (usage of shell definitions)
    Face       = 1, //!< Face reference entries (usage of face definitions)
    Wire       = 2, //!< Wire reference entries (usage of wire definitions)
    CoEdge     = 3, //!< CoEdge reference entries (usage of coedge definitions)
    Vertex     = 4, //!< Vertex reference entries (usage of vertex definitions)
    Solid      = 5, //!< Solid reference entries (usage of solid definitions)
    Child      = 6, //!< Generic child references (usage of mixed node definitions)
    Occurrence = 7  //!< Occurrence references (usage of occurrence definitions)
  };

  //! @brief Compile-time typed wrapper around BRepGraph_RefId.
  //!
  //! Provides compile-time kind safety similarly to BRepGraph_NodeId::Typed.
  template <Kind TheKind>
  struct Typed
  {
    int Index;

    Typed()
        : Index(-1)
    {
    }

    explicit Typed(const int theIdx)
        : Index(theIdx)
    {
      Standard_ASSERT_VOID(theIdx >= -1, "index must be >= -1");
    }

    [[nodiscard]] bool IsValid() const { return Index >= 0; }

    [[nodiscard]] bool IsValid(const int theMaxCount) const
    {
      Standard_ASSERT_RETURN(theMaxCount >= 0, "max count must be non-negative", false);
      return Index >= 0 && Index < theMaxCount;
    }

    operator BRepGraph_RefId() const { return BRepGraph_RefId(TheKind, Index); }

    static Typed FromRefId(const BRepGraph_RefId theRefId)
    {
      Standard_ASSERT_VOID(theRefId.RefKind == TheKind, "RefId kind mismatch");
      return Typed(theRefId.Index);
    }

    bool operator==(const Typed& theOther) const { return Index == theOther.Index; }

    bool operator!=(const Typed& theOther) const { return Index != theOther.Index; }

    bool operator<(const Typed& theOther) const { return Index < theOther.Index; }

    bool operator<=(const Typed& theOther) const { return Index <= theOther.Index; }

    bool operator>(const Typed& theOther) const { return Index > theOther.Index; }

    bool operator>=(const Typed& theOther) const { return Index >= theOther.Index; }

    bool operator==(const BRepGraph_RefId& theOther) const
    {
      return theOther.RefKind == TheKind && theOther.Index == Index;
    }

    bool operator!=(const BRepGraph_RefId& theOther) const { return !(*this == theOther); }

    friend bool operator==(const BRepGraph_RefId& theLhs, const Typed& theRhs)
    {
      return theRhs == theLhs;
    }

    friend bool operator!=(const BRepGraph_RefId& theLhs, const Typed& theRhs)
    {
      return theRhs != theLhs;
    }
  };

  static bool IsTopologyRefKind(const Kind theKind)
  {
    return static_cast<int>(theKind) >= static_cast<int>(Kind::Shell)
           && static_cast<int>(theKind) <= static_cast<int>(Kind::Child);
  }

  Kind RefKind;
  int  Index;

  BRepGraph_RefId()
      : RefKind(Kind::Shell),
        Index(-1)
  {
  }

  BRepGraph_RefId(const Kind theKind, const int theIdx)
      : RefKind(theKind),
        Index(theIdx)
  {
    Standard_ASSERT_VOID(theIdx >= -1, "BRepGraph_RefId: index must be >= -1");
  }

  [[nodiscard]] bool IsValid() const { return Index >= 0; }

  [[nodiscard]] bool IsValid(const int theMaxCount) const
  {
    Standard_ASSERT_RETURN(theMaxCount >= 0, "max count must be non-negative", false);
    return Index >= 0 && Index < theMaxCount;
  }

  bool operator==(const BRepGraph_RefId& theOther) const
  {
    return RefKind == theOther.RefKind && Index == theOther.Index;
  }

  bool operator!=(const BRepGraph_RefId& theOther) const { return !(*this == theOther); }

  bool operator<(const BRepGraph_RefId& theOther) const
  {
    if (RefKind != theOther.RefKind)
      return static_cast<int>(RefKind) < static_cast<int>(theOther.RefKind);
    return Index < theOther.Index;
  }
};

using BRepGraph_ShellRefId      = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Shell>;
using BRepGraph_FaceRefId       = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Face>;
using BRepGraph_WireRefId       = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Wire>;
using BRepGraph_CoEdgeRefId     = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::CoEdge>;
using BRepGraph_VertexRefId     = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Vertex>;
using BRepGraph_SolidRefId      = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Solid>;
using BRepGraph_ChildRefId      = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Child>;
using BRepGraph_OccurrenceRefId = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Occurrence>;

template <>
struct std::hash<BRepGraph_RefId>
{
  size_t operator()(const BRepGraph_RefId& theId) const noexcept
  {
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(theId.RefKind));
    aCombination[1] = opencascade::hash(theId.Index);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

template <BRepGraph_RefId::Kind TheKind>
struct std::hash<BRepGraph_RefId::Typed<TheKind>>
{
  size_t operator()(const BRepGraph_RefId::Typed<TheKind>& theId) const noexcept
  {
    return std::hash<BRepGraph_RefId>{}(static_cast<BRepGraph_RefId>(theId));
  }
};

#endif // _BRepGraph_RefId_HeaderFile
