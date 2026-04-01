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

#ifndef _BRepGraph_RepId_HeaderFile
#define _BRepGraph_RepId_HeaderFile

#include <Standard_Assert.hxx>
#include <Standard_HashUtils.hxx>

#include <cstddef>
#include <functional>

//! Lightweight typed index into a per-kind representation vector inside BRepGraph.
//!
//! The pair (Kind, Index) forms a unique representation identifier within one
//! graph instance.  Default-constructed RepId has Index = -1 (invalid).
//!
//! Representations are NOT topology nodes - they hold geometry or mesh data
//! referenced by topology entities.  They do not participate in BFS traversal,
//! reverse index, or parent-child relationships.
//!
//! RepId is a value type: cheap to copy, compare, hash.
struct BRepGraph_RepId
{
  //! Categories of representation data.
  enum class Kind : int
  {
    // Geometry (exact mathematical definition)
    Surface = 0, //!< Geom_Surface for faces
    Curve3D = 1, //!< Geom_Curve for edges
    Curve2D = 2, //!< Geom2d_Curve for coedges (PCurve geometry)

    // Mesh (discrete approximation)
    Triangulation = 3, //!< Poly_Triangulation for faces
    Polygon3D     = 4, //!< Poly_Polygon3D for edges
    Polygon2D     = 5, //!< Poly_Polygon2D for coedges (polygon-on-surface)
    PolygonOnTri  = 6, //!< Poly_PolygonOnTriangulation for coedges

    // Reserved 7-19 for future built-in types
    // Custom plugin types start at 100+
  };

  //! True if the kind is a geometry kind (Surface, Curve3D, Curve2D).
  static bool IsGeometryKind(const Kind theKind)
  {
    return theKind == Kind::Surface || theKind == Kind::Curve3D || theKind == Kind::Curve2D;
  }

  //! True if the kind is a mesh kind (Triangulation, Polygon3D, Polygon2D, PolygonOnTri).
  static bool IsMeshKind(const Kind theKind)
  {
    return theKind == Kind::Triangulation || theKind == Kind::Polygon3D
           || theKind == Kind::Polygon2D || theKind == Kind::PolygonOnTri;
  }

  //! @brief Compile-time typed wrapper around BRepGraph_RepId.
  //!
  //! Provides compile-time kind safety: a Typed<Kind::Surface>
  //! cannot be accidentally used where a Typed<Kind::Curve3D> is expected.
  //! Implicitly converts to BRepGraph_RepId for backward compatibility.
  //!
  //! @tparam TheKind the BRepGraph_RepId::Kind this typed id represents
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

    //! True if this id points to an allocated representation slot.
    [[nodiscard]] bool IsValid() const { return Index >= 0; }

    //! True if this id points to an allocated slot within [0, theMaxCount).
    [[nodiscard]] bool IsValid(const int theMaxCount) const
    {
      Standard_ASSERT_RETURN(theMaxCount >= 0, "max count must be non-negative", false);
      return Index >= 0 && Index < theMaxCount;
    }

    //! Implicit conversion to untyped RepId.
    operator BRepGraph_RepId() const { return BRepGraph_RepId(TheKind, Index); }

    //! Explicit conversion from untyped RepId.
    //! Asserts that the Kind matches in debug builds.
    //! @param[in] theId untyped RepId to convert
    static Typed FromRepId(const BRepGraph_RepId theId)
    {
      Standard_ASSERT_VOID(theId.RepKind == TheKind, "RepId kind mismatch");
      return Typed(theId.Index);
    }

    bool operator==(const Typed& theOther) const { return Index == theOther.Index; }

    bool operator!=(const Typed& theOther) const { return Index != theOther.Index; }

    bool operator<(const Typed& theOther) const { return Index < theOther.Index; }

    bool operator<=(const Typed& theOther) const { return Index <= theOther.Index; }

    bool operator>(const Typed& theOther) const { return Index > theOther.Index; }

    bool operator>=(const Typed& theOther) const { return Index >= theOther.Index; }

    //! Pre-increment (++id).
    Typed& operator++()
    {
      Standard_ASSERT_VOID(Index >= 0, "pre-increment on invalid id");
      ++Index;
      return *this;
    }

    //! Post-increment (id++).
    Typed operator++(int)
    {
      Standard_ASSERT_VOID(Index >= 0, "post-increment on invalid id");
      Typed aPrev = *this;
      ++Index;
      return aPrev;
    }

    //! Advance by offset.
    [[nodiscard]] Typed operator+(const int theOffset) const { return Typed(Index + theOffset); }

    //! Retreat by offset.
    [[nodiscard]] Typed operator-(const int theOffset) const { return Typed(Index - theOffset); }

    //! Comparison with untyped RepId (checks both Kind and Index).
    bool operator==(const BRepGraph_RepId& theOther) const
    {
      return theOther.RepKind == TheKind && theOther.Index == Index;
    }

    bool operator!=(const BRepGraph_RepId& theOther) const { return !(*this == theOther); }

    //! Allow reversed comparison: RepId == Typed.
    friend bool operator==(const BRepGraph_RepId& theLhs, const Typed& theRhs)
    {
      return theRhs == theLhs;
    }

    friend bool operator!=(const BRepGraph_RepId& theLhs, const Typed& theRhs)
    {
      return theRhs != theLhs;
    }
  };

  Kind RepKind;
  int  Index;

  //! Default: invalid RepId (Index = -1).
  //! RepKind is set to Kind::Surface but is meaningless when !IsValid().
  BRepGraph_RepId()
      : RepKind(Kind::Surface),
        Index(-1)
  {
  }

  BRepGraph_RepId(const Kind theKind, const int theIdx)
      : RepKind(theKind),
        Index(theIdx)
  {
  }

  //! True if this id points to an allocated representation slot.
  [[nodiscard]] bool IsValid() const { return Index >= 0; }

  //! True if this id points to an allocated slot within [0, theMaxCount).
  [[nodiscard]] bool IsValid(const int theMaxCount) const
  {
    Standard_ASSERT_RETURN(theMaxCount >= 0, "max count must be non-negative", false);
    return Index >= 0 && Index < theMaxCount;
  }

  //! @name Static factory methods returning typed RepIds.
  static Typed<Kind::Surface> Surface(const int theIdx) { return Typed<Kind::Surface>(theIdx); }

  static Typed<Kind::Curve3D> Curve3D(const int theIdx) { return Typed<Kind::Curve3D>(theIdx); }

  static Typed<Kind::Curve2D> Curve2D(const int theIdx) { return Typed<Kind::Curve2D>(theIdx); }

  static Typed<Kind::Triangulation> Triangulation(const int theIdx)
  {
    return Typed<Kind::Triangulation>(theIdx);
  }

  static Typed<Kind::Polygon3D> Polygon3D(const int theIdx)
  {
    return Typed<Kind::Polygon3D>(theIdx);
  }

  static Typed<Kind::Polygon2D> Polygon2D(const int theIdx)
  {
    return Typed<Kind::Polygon2D>(theIdx);
  }

  static Typed<Kind::PolygonOnTri> PolygonOnTri(const int theIdx)
  {
    return Typed<Kind::PolygonOnTri>(theIdx);
  }

  bool operator==(const BRepGraph_RepId& theOther) const
  {
    return RepKind == theOther.RepKind && Index == theOther.Index;
  }

  bool operator!=(const BRepGraph_RepId& theOther) const { return !(*this == theOther); }

  bool operator<(const BRepGraph_RepId& theOther) const
  {
    if (RepKind != theOther.RepKind)
      return static_cast<int>(RepKind) < static_cast<int>(theOther.RepKind);
    return Index < theOther.Index;
  }

  //! Pre-increment (++id).
  BRepGraph_RepId& operator++()
  {
    Standard_ASSERT_VOID(Index >= 0, "pre-increment on invalid id");
    ++Index;
    return *this;
  }

  //! Post-increment (id++).
  BRepGraph_RepId operator++(int)
  {
    Standard_ASSERT_VOID(Index >= 0, "post-increment on invalid id");
    BRepGraph_RepId aPrev = *this;
    ++Index;
    return aPrev;
  }

  //! Advance by offset.
  [[nodiscard]] BRepGraph_RepId operator+(const int theOffset) const
  {
    return BRepGraph_RepId(RepKind, Index + theOffset);
  }

  //! Retreat by offset.
  [[nodiscard]] BRepGraph_RepId operator-(const int theOffset) const
  {
    return BRepGraph_RepId(RepKind, Index - theOffset);
  }
};

//! @name Convenience type aliases for typed RepIds.
using BRepGraph_SurfaceRepId       = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::Surface>;
using BRepGraph_Curve3DRepId       = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::Curve3D>;
using BRepGraph_Curve2DRepId       = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::Curve2D>;
using BRepGraph_TriangulationRepId = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::Triangulation>;
using BRepGraph_Polygon3DRepId     = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::Polygon3D>;
using BRepGraph_Polygon2DRepId     = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::Polygon2D>;
using BRepGraph_PolygonOnTriRepId  = BRepGraph_RepId::Typed<BRepGraph_RepId::Kind::PolygonOnTri>;

//! std::hash specialization for NCollection_DefaultHasher support.
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

//! std::hash specialization for BRepGraph_RepId::Typed.
template <BRepGraph_RepId::Kind TheKind>
struct std::hash<BRepGraph_RepId::Typed<TheKind>>
{
  size_t operator()(const BRepGraph_RepId::Typed<TheKind>& theId) const noexcept
  {
    return std::hash<BRepGraph_RepId>{}(static_cast<BRepGraph_RepId>(theId));
  }
};

#endif // _BRepGraph_RepId_HeaderFile
