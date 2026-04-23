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
#include <cstdint>
#include <functional>
#include <limits>
#include <utility>

//! Lightweight typed index into a per-kind representation vector inside BRepGraph.
//!
//! The pair (Kind, Index) forms a unique representation identifier within one
//! graph instance.  Default-constructed RepId has Index = UINT32_MAX (invalid).
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
    static constexpr uint32_t THE_START_INDEX   = 0u;
    static constexpr uint32_t THE_INVALID_INDEX = std::numeric_limits<uint32_t>::max();

    uint32_t Index;

    //! Default: invalid (Index = UINT32_MAX).
    Typed()
        : Index(THE_INVALID_INDEX)
    {
    }

    //! Construct from index.
    explicit Typed(const uint32_t theIdx)
        : Index(theIdx)
    {
    }

    //! Construct from an untyped representation id of the same kind.
    explicit Typed(const BRepGraph_RepId theId)
        : Typed(FromRepId(theId))
    {
    }

    template <Kind OtherKind, typename std::enable_if_t<OtherKind != TheKind, int> = 0>
    Typed(const Typed<OtherKind>&) = delete;

    //! First valid id in a dense per-kind sequence.
    [[nodiscard]] static Typed Start() { return Typed(THE_START_INDEX); }

    //! Invalid sentinel id.
    [[nodiscard]] static Typed Invalid() { return Typed(); }

    //! True if this id points to an allocated representation slot.
    [[nodiscard]] bool IsValid() const { return Index != THE_INVALID_INDEX; }

    //! True if this id points to an allocated slot within [0, theMaxCount).
    //! UINT32_MAX (invalid sentinel) always fails this check for any realistic count.
    [[nodiscard]] bool IsValid(const uint32_t theMaxCount) const { return Index < theMaxCount; }

    //! True if this id is within the dense range exposed by a provider with Nb().
    template <typename CountProviderT>
    [[nodiscard]] auto IsValidIn(const CountProviderT& theProvider) const
      -> decltype(theProvider.Nb(), bool())
    {
      return IsValid(theProvider.Nb());
    }

    //! True if this id is within the dense range exposed by a provider with Length().
    template <typename CountProviderT>
    [[nodiscard]] auto IsValidIn(const CountProviderT& theProvider) const
      -> decltype(theProvider.Size(), bool())
    {
      return IsValid(static_cast<uint32_t>(theProvider.Size()));
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
      Standard_ASSERT_VOID(Index != THE_INVALID_INDEX, "pre-increment on invalid id");
      ++Index;
      return *this;
    }

    //! Post-increment (id++).
    Typed operator++(int)
    {
      Standard_ASSERT_VOID(Index != THE_INVALID_INDEX, "post-increment on invalid id");
      Typed aPrev = *this;
      ++Index;
      return aPrev;
    }

    //! Advance by offset.
    [[nodiscard]] Typed operator+(const uint32_t theOffset) const
    {
      return Typed(Index + theOffset);
    }

    //! Retreat by offset.
    [[nodiscard]] Typed operator-(const uint32_t theOffset) const
    {
      Standard_ASSERT_VOID(Index != THE_INVALID_INDEX && Index >= theOffset,
                           "retreat underflows index");
      return Typed(Index - theOffset);
    }

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

  static constexpr uint32_t THE_START_INDEX   = 0u;
  static constexpr uint32_t THE_INVALID_INDEX = std::numeric_limits<uint32_t>::max();

  Kind     RepKind;
  uint32_t Index;

  //! Default: invalid RepId (Index = UINT32_MAX).
  //! RepKind is set to Kind::Surface but is meaningless when !IsValid().
  BRepGraph_RepId()
      : RepKind(Kind::Surface),
        Index(THE_INVALID_INDEX)
  {
  }

  BRepGraph_RepId(const Kind theKind, const uint32_t theIdx)
      : RepKind(theKind),
        Index(theIdx)
  {
  }

  //! First valid id in a dense sequence for the specified kind.
  [[nodiscard]] static BRepGraph_RepId Start(const Kind theKind)
  {
    return BRepGraph_RepId(theKind, THE_START_INDEX);
  }

  //! Invalid sentinel id for the specified kind.
  [[nodiscard]] static BRepGraph_RepId Invalid(const Kind theKind = Kind::Surface)
  {
    return BRepGraph_RepId(theKind, THE_INVALID_INDEX);
  }

  //! True if this id points to an allocated representation slot.
  [[nodiscard]] bool IsValid() const { return Index != THE_INVALID_INDEX; }

  //! True if this id points to an allocated slot within [0, theMaxCount).
  //! UINT32_MAX (invalid sentinel) always fails this check for any realistic count.
  [[nodiscard]] bool IsValid(const uint32_t theMaxCount) const { return Index < theMaxCount; }

  //! True if this id is within the dense range exposed by a provider with Nb().
  template <typename CountProviderT>
  [[nodiscard]] auto IsValidIn(const CountProviderT& theProvider) const
    -> decltype(theProvider.Nb(), bool())
  {
    return IsValid(theProvider.Nb());
  }

  //! True if this id is within the dense range exposed by a provider with Size().
  template <typename CountProviderT>
  [[nodiscard]] auto IsValidIn(const CountProviderT& theProvider) const
    -> decltype(theProvider.Size(), bool())
  {
    return IsValid(static_cast<uint32_t>(theProvider.Size()));
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
    Standard_ASSERT_VOID(Index != THE_INVALID_INDEX, "pre-increment on invalid id");
    ++Index;
    return *this;
  }

  //! Post-increment (id++).
  BRepGraph_RepId operator++(int)
  {
    Standard_ASSERT_VOID(Index != THE_INVALID_INDEX, "post-increment on invalid id");
    BRepGraph_RepId aPrev = *this;
    ++Index;
    return aPrev;
  }

  //! Advance by offset.
  [[nodiscard]] BRepGraph_RepId operator+(const uint32_t theOffset) const
  {
    return BRepGraph_RepId(RepKind, Index + theOffset);
  }

  //! Retreat by offset.
  [[nodiscard]] BRepGraph_RepId operator-(const uint32_t theOffset) const
  {
    Standard_ASSERT_VOID(Index != THE_INVALID_INDEX && Index >= theOffset,
                         "retreat underflows index");
    return BRepGraph_RepId(RepKind, Index - theOffset);
  }

  //! Dispatch a generic rep id to a callable taking the matching typed rep id.
  template <typename FuncT>
  static auto Visit(const BRepGraph_RepId theRepId, FuncT&& theFunc)
    -> decltype(std::forward<FuncT>(theFunc)(Typed<Kind::Surface>()))
  {
    switch (theRepId.RepKind)
    {
      case Kind::Surface:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Surface>::FromRepId(theRepId));
      case Kind::Curve3D:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Curve3D>::FromRepId(theRepId));
      case Kind::Curve2D:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Curve2D>::FromRepId(theRepId));
      case Kind::Triangulation:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Triangulation>::FromRepId(theRepId));
      case Kind::Polygon3D:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Polygon3D>::FromRepId(theRepId));
      case Kind::Polygon2D:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Polygon2D>::FromRepId(theRepId));
      case Kind::PolygonOnTri:
        return std::forward<FuncT>(theFunc)(Typed<Kind::PolygonOnTri>::FromRepId(theRepId));
    }

    Standard_ASSERT_VOID(false, "BRepGraph_RepId::Visit: unhandled Kind");
    return std::forward<FuncT>(theFunc)(Typed<Kind::Surface>());
  }
};

// Convenience type aliases for typed RepIds.
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
