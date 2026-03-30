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

#ifndef _BRepGraph_NodeId_HeaderFile
#define _BRepGraph_NodeId_HeaderFile

#include <Standard_Assert.hxx>
#include <Standard_HashUtils.hxx>

#include <cstddef>
#include <functional>

//! Lightweight typed index into a per-kind node vector inside BRepGraph.
//!
//! The pair (NodeKind, Index) forms a unique node identifier within one graph
//! instance.  Default-constructed NodeId has Index = -1 (invalid).
//!
//! NodeId is a value type: cheap to copy, compare, hash.  It carries no
//! pointer back to the owning graph; the caller is responsible for using
//! it with the correct BRepGraph instance.
struct BRepGraph_NodeId
{
  //! Enumeration of node kinds within a BRepGraph.
  //!
  //! Topology kinds 0-5 cover core hierarchy; Compound(6)/CompSolid(7)
  //! are container kinds.  Note: ordering does NOT match TopAbs_ShapeEnum.
  //! Geometry kinds start at 10, leaving room for future topology extensions.
  enum class Kind : int
  {
    Solid      = 0,
    Shell      = 1,
    Face       = 2,
    Wire       = 3,
    Edge       = 4,
    Vertex     = 5,
    Compound   = 6,  //!< TopoDS_Compound container
    CompSolid  = 7,  //!< TopoDS_CompSolid container
    CoEdge     = 8,  //!< Use of an edge on a face (owns PCurve data)
    // Value 9 reserved for future topology kind extension.
    Product    = 10, //!< Reusable shape definition (part or assembly)
    Occurrence = 11  //!< Placed instance of a product within a parent product
  };

  //! @brief Compile-time typed wrapper around BRepGraph_NodeId.
  //!
  //! Provides compile-time kind safety: a Typed<Kind::Face>
  //! cannot be accidentally used where a Typed<Kind::Edge> is expected.
  //! Implicitly converts to BRepGraph_NodeId for backward compatibility.
  //!
  //! @tparam TheKind the BRepGraph_NodeId::Kind this typed id represents
  template <Kind TheKind>
  struct Typed
  {
    int Index;

    //! Default: invalid (Index = -1).
    Typed()
        : Index(-1)
    {
    }

    //! Construct from index.
    explicit Typed(const int theIdx)
        : Index(theIdx)
    {
      Standard_ASSERT_VOID(theIdx >= -1, "index must be >= -1");
    }

    //! True if this id points to an allocated node slot.
    [[nodiscard]] bool IsValid() const { return Index >= 0; }

    //! True if this id points to an allocated slot within [0, theMaxCount).
    [[nodiscard]] bool IsValid(const int theMaxCount) const
    {
      Standard_ASSERT_RETURN(theMaxCount >= 0, "max count must be non-negative", false);
      return Index >= 0 && Index < theMaxCount;
    }

    //! Implicit conversion to untyped NodeId.
    operator BRepGraph_NodeId() const { return BRepGraph_NodeId(TheKind, Index); }

    //! Explicit conversion from untyped NodeId.
    //! Asserts that the Kind matches in debug builds.
    //! @param[in] theId untyped NodeId to convert
    static Typed FromNodeId(const BRepGraph_NodeId theId)
    {
      Standard_ASSERT_VOID(theId.NodeKind == TheKind, "NodeId kind mismatch");
      return Typed(theId.Index);
    }

    bool operator==(const Typed& theOther) const { return Index == theOther.Index; }

    bool operator!=(const Typed& theOther) const { return Index != theOther.Index; }

    bool operator<(const Typed& theOther) const { return Index < theOther.Index; }

    bool operator<=(const Typed& theOther) const { return Index <= theOther.Index; }

    bool operator>(const Typed& theOther) const { return Index > theOther.Index; }

    bool operator>=(const Typed& theOther) const { return Index >= theOther.Index; }

    //! Comparison with untyped NodeId (checks both Kind and Index).
    bool operator==(const BRepGraph_NodeId& theOther) const
    {
      return theOther.NodeKind == TheKind && theOther.Index == Index;
    }

    bool operator!=(const BRepGraph_NodeId& theOther) const { return !(*this == theOther); }

    //! Allow reversed comparison: NodeId == Typed.
    friend bool operator==(const BRepGraph_NodeId& theLhs, const Typed& theRhs)
    {
      return theRhs == theLhs;
    }

    friend bool operator!=(const BRepGraph_NodeId& theLhs, const Typed& theRhs)
    {
      return theRhs != theLhs;
    }
  };

  //! True if the kind is a core topology kind (Solid..CoEdge).
  static bool IsTopologyKind(const Kind theKind) { return static_cast<int>(theKind) <= 8; }

  //! True if the kind is an assembly kind (Product or Occurrence).
  static bool IsAssemblyKind(const Kind theKind)
  {
    return theKind == Kind::Product || theKind == Kind::Occurrence;
  }

  Kind NodeKind;
  int  Index;

  //! Default: invalid NodeId (Index = -1).
  //! NodeKind is set to Kind::Solid but is meaningless when !IsValid().
  BRepGraph_NodeId()
      : NodeKind(Kind::Solid),
        Index(-1)
  {
  }

  BRepGraph_NodeId(const Kind theKind, const int theIdx)
      : NodeKind(theKind),
        Index(theIdx)
  {
    Standard_ASSERT_VOID(theIdx >= -1, "BRepGraph_NodeId: index must be >= -1");
  }

  //! True if this id points to an allocated node slot.
  [[nodiscard]] bool IsValid() const { return Index >= 0; }

  //! True if this id points to an allocated slot within [0, theMaxCount).
  [[nodiscard]] bool IsValid(const int theMaxCount) const
  {
    Standard_ASSERT_RETURN(theMaxCount >= 0, "max count must be non-negative", false);
    return Index >= 0 && Index < theMaxCount;
  }

  bool operator==(const BRepGraph_NodeId& theOther) const
  {
    return NodeKind == theOther.NodeKind && Index == theOther.Index;
  }

  bool operator!=(const BRepGraph_NodeId& theOther) const { return !(*this == theOther); }

  bool operator<(const BRepGraph_NodeId& theOther) const
  {
    if (NodeKind != theOther.NodeKind)
      return static_cast<int>(NodeKind) < static_cast<int>(theOther.NodeKind);
    return Index < theOther.Index;
  }
};

//! @name Convenience type aliases for typed NodeIds.
using BRepGraph_SolidId      = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::Solid>;
using BRepGraph_ShellId      = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::Shell>;
using BRepGraph_FaceId       = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::Face>;
using BRepGraph_WireId       = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::Wire>;
using BRepGraph_EdgeId       = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::Edge>;
using BRepGraph_VertexId     = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::Vertex>;
using BRepGraph_CompoundId   = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::Compound>;
using BRepGraph_CompSolidId  = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::CompSolid>;
using BRepGraph_CoEdgeId     = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::CoEdge>;
using BRepGraph_ProductId    = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::Product>;
using BRepGraph_OccurrenceId = BRepGraph_NodeId::Typed<BRepGraph_NodeId::Kind::Occurrence>;

//! std::hash specialization for BRepGraph_NodeId.
template <>
struct std::hash<BRepGraph_NodeId>
{
  size_t operator()(const BRepGraph_NodeId& theId) const noexcept
  {
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(theId.NodeKind));
    aCombination[1] = opencascade::hash(theId.Index);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

//! std::hash specialization for BRepGraph_NodeId::Typed.
template <BRepGraph_NodeId::Kind TheKind>
struct std::hash<BRepGraph_NodeId::Typed<TheKind>>
{
  size_t operator()(const BRepGraph_NodeId::Typed<TheKind>& theId) const noexcept
  {
    return std::hash<BRepGraph_NodeId>{}(static_cast<BRepGraph_NodeId>(theId));
  }
};

#endif // _BRepGraph_NodeId_HeaderFile
