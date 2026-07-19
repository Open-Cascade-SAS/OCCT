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

#ifndef _BRepGraphInc_RepId_HeaderFile
#define _BRepGraphInc_RepId_HeaderFile

#include <Standard_Assert.hxx>
#include <Standard_HashUtils.hxx>

#include <cstdint>
#include <functional>
#include <limits>

class BRepGraph;

//! Lightweight typed index into a per-kind use-record vector inside BRepGraph.
//!
//! The pair (Kind, Index) forms a unique use-record identifier within one graph
//! instance. Default-constructed RepId has Index = UINT32_MAX (invalid).
//!
//! Use records are session-local representation slots with no public stable UID,
//! graph-level lock state, independent mutation generation, or layer callbacks.
//! They do have a soft-removed state so an owner can clear and later reuse its slot.
struct BRepGraph_RepId
{
  //! Enumeration of use-record kinds.
  enum class Kind : int
  {
    EdgeCurve3D        = 0, //!< Geom_Curve use for edges
    EdgePolygon3D      = 1, //!< Poly_Polygon3D use for edges
    CoEdgeCurve2D      = 2, //!< Geom2d_Curve use for coedges
    CoEdgePolygon2D    = 3, //!< Poly_Polygon2D use for coedges
    CoEdgePolygonOnTri = 4, //!< Poly_PolygonOnTriangulation use for coedges
    FaceSurface        = 5, //!< Geom_Surface use for faces
    FaceTriangulation  = 6  //!< Poly_Triangulation use for faces
  };

  //! True if the kind value is one of the supported use-record kinds.
  static bool IsValidKind(const Kind theKind)
  {
    switch (theKind)
    {
      case Kind::EdgeCurve3D:
      case Kind::EdgePolygon3D:
      case Kind::CoEdgeCurve2D:
      case Kind::CoEdgePolygon2D:
      case Kind::CoEdgePolygonOnTri:
      case Kind::FaceSurface:
      case Kind::FaceTriangulation:
        return true;
    }
    return false;
  }

  //! Compile-time typed wrapper around BRepGraph_RepId.
  template <Kind TheKind>
  struct Typed
  {
    static constexpr uint32_t THE_START_INDEX   = 0u;
    static constexpr uint32_t THE_INVALID_INDEX = std::numeric_limits<uint32_t>::max();

    uint32_t Index;

    //! Default: invalid.
    Typed()
        : Index(THE_INVALID_INDEX)
    {
    }

    //! Construct from index.
    explicit Typed(const uint32_t theIdx)
        : Index(theIdx)
    {
    }

    //! First valid id in a dense sequence.
    [[nodiscard]] static Typed Start() { return Typed(THE_START_INDEX); }

    //! Invalid sentinel id.
    [[nodiscard]] static Typed Invalid() { return Typed(); }

    //! True if this id points to an allocated slot.
    [[nodiscard]] bool IsValid() const { return Index != THE_INVALID_INDEX; }

    //! True if this id is within [0, theMaxCount).
    [[nodiscard]] bool IsValid(const uint32_t theMaxCount) const
    {
      return IsValid() && Index < theMaxCount;
    }

    //! Implicit conversion to untyped RepId.
    operator BRepGraph_RepId() const { return BRepGraph_RepId(TheKind, Index); }

    //! Return true if this use entry has been soft-removed in the given graph.
    [[nodiscard]] bool IsRemoved(const BRepGraph& theGraph) const
    {
      return BRepGraph_RepId(*this).IsRemoved(theGraph);
    }

    //! Pre-increment.
    Typed& operator++()
    {
      Standard_ASSERT_VOID(Index != THE_INVALID_INDEX, "pre-increment on invalid use id");
      ++Index;
      return *this;
    }

    //! Post-increment.
    Typed operator++(int)
    {
      Standard_ASSERT_VOID(Index != THE_INVALID_INDEX, "post-increment on invalid use id");
      Typed aPrev = *this;
      ++Index;
      return aPrev;
    }

    bool operator==(const Typed& theOther) const { return Index == theOther.Index; }

    bool operator!=(const Typed& theOther) const { return Index != theOther.Index; }

    bool operator<(const Typed& theOther) const { return Index < theOther.Index; }

    bool operator<=(const Typed& theOther) const { return Index <= theOther.Index; }

    bool operator>(const Typed& theOther) const { return Index > theOther.Index; }

    bool operator>=(const Typed& theOther) const { return Index >= theOther.Index; }
  };

  static constexpr uint32_t THE_START_INDEX   = 0u;
  static constexpr uint32_t THE_INVALID_INDEX = std::numeric_limits<uint32_t>::max();

  Kind     RepKind;
  uint32_t Index;

  //! Default: invalid RepId.
  BRepGraph_RepId()
      : RepKind(Kind::EdgeCurve3D),
        Index(THE_INVALID_INDEX)
  {
  }

  BRepGraph_RepId(const Kind theKind, const uint32_t theIdx)
      : RepKind(theKind),
        Index(theIdx)
  {
  }

  //! True if this id points to an allocated slot.
  [[nodiscard]] bool IsValid() const { return IsValidKind(RepKind) && Index != THE_INVALID_INDEX; }

  //! True if this id is within [0, theMaxCount).
  [[nodiscard]] bool IsValid(const uint32_t theMaxCount) const
  {
    return IsValid() && Index < theMaxCount;
  }

  bool operator==(const BRepGraph_RepId& theOther) const
  {
    return RepKind == theOther.RepKind && Index == theOther.Index;
  }

  bool operator!=(const BRepGraph_RepId& theOther) const { return !(*this == theOther); }

  bool operator<(const BRepGraph_RepId& theOther) const
  {
    if (RepKind != theOther.RepKind)
    {
      return static_cast<int>(RepKind) < static_cast<int>(theOther.RepKind);
    }
    return Index < theOther.Index;
  }

  //! Return true if this use entry has been soft-removed in the given graph.
  [[nodiscard]] Standard_EXPORT bool IsRemoved(const BRepGraph& theGraph) const;
};

// Convenience type aliases for typed RepIds.
using BRepGraph_EdgeCurve3DRepId   = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::EdgeCurve3D>;
using BRepGraph_EdgePolygon3DRepId = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::EdgePolygon3D>;
using BRepGraph_CoEdgeCurve2DRepId = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::CoEdgeCurve2D>;
using BRepGraph_CoEdgePolygon2DRepId =
  BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::CoEdgePolygon2D>;
using BRepGraph_CoEdgePolygonOnTriRepId =
  BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::CoEdgePolygonOnTri>;
using BRepGraph_FaceSurfaceRepId = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::FaceSurface>;
using BRepGraph_FaceTriangulationRepId =
  BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::FaceTriangulation>;

template <>
struct std::hash<BRepGraph_RepId>
{
  size_t operator()(const BRepGraph_RepId& theId) const noexcept
  {
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(theId.RepKind));
    aCombination[1] = opencascade::hash(theId.Index);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

template <BRepGraph_RepId::Kind TheKind>
struct std::hash<BRepGraph_RepId::Typed<TheKind>>
{
  size_t operator()(const BRepGraph_RepId::Typed<TheKind>& theId) const noexcept
  {
    return std::hash<uint32_t>{}(theId.Index);
  }
};

#endif // _BRepGraphInc_RepId_HeaderFile
