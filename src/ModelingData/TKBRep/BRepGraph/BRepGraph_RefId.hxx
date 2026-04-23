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
#include <cstdint>
#include <functional>
#include <limits>
#include <utility>

//! Lightweight typed index into a per-kind reference vector inside BRepGraph.
//!
//! The pair (Kind, Index) forms a unique reference identifier within one graph
//! instance. Default-constructed RefId has Index = UINT32_MAX (invalid).
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
    static constexpr uint32_t THE_START_INDEX   = 0u;
    static constexpr uint32_t THE_INVALID_INDEX = std::numeric_limits<uint32_t>::max();

    uint32_t Index;

    Typed()
        : Index(THE_INVALID_INDEX)
    {
    }

    explicit Typed(const uint32_t theIdx)
        : Index(theIdx)
    {
    }

    //! Construct from an untyped reference id of the same kind.
    explicit Typed(const BRepGraph_RefId theRefId)
        : Typed(FromRefId(theRefId))
    {
    }

    template <Kind OtherKind, typename std::enable_if_t<OtherKind != TheKind, int> = 0>
    Typed(const Typed<OtherKind>&) = delete;

    //! First valid id in a dense per-kind sequence.
    [[nodiscard]] static Typed Start() { return Typed(THE_START_INDEX); }

    //! Invalid sentinel id.
    [[nodiscard]] static Typed Invalid() { return Typed(); }

    [[nodiscard]] bool IsValid() const { return Index != THE_INVALID_INDEX; }

    //! True if this id points to an allocated slot within [0, theMaxCount).
    //! UINT32_MAX (invalid sentinel) always fails this check for any realistic count.
    [[nodiscard]] bool IsValid(const uint32_t theMaxCount) const { return Index < theMaxCount; }

    template <typename CountProviderT>
    [[nodiscard]] auto IsValidIn(const CountProviderT& theProvider) const
      -> decltype(theProvider.Nb(), bool())
    {
      return IsValid(theProvider.Nb());
    }

    template <typename CountProviderT>
    [[nodiscard]] auto IsValidIn(const CountProviderT& theProvider) const
      -> decltype(theProvider.Size(), bool())
    {
      return IsValid(static_cast<uint32_t>(theProvider.Size()));
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

  static constexpr uint32_t THE_START_INDEX   = 0u;
  static constexpr uint32_t THE_INVALID_INDEX = std::numeric_limits<uint32_t>::max();

  Kind     RefKind;
  uint32_t Index;

  BRepGraph_RefId()
      : RefKind(Kind::Shell),
        Index(THE_INVALID_INDEX)
  {
  }

  BRepGraph_RefId(const Kind theKind, const uint32_t theIdx)
      : RefKind(theKind),
        Index(theIdx)
  {
  }

  //! First valid id in a dense sequence for the specified kind.
  [[nodiscard]] static BRepGraph_RefId Start(const Kind theKind)
  {
    return BRepGraph_RefId(theKind, THE_START_INDEX);
  }

  //! Invalid sentinel id for the specified kind.
  [[nodiscard]] static BRepGraph_RefId Invalid(const Kind theKind = Kind::Shell)
  {
    return BRepGraph_RefId(theKind, THE_INVALID_INDEX);
  }

  [[nodiscard]] bool IsValid() const { return Index != THE_INVALID_INDEX; }

  //! True if this id points to an allocated slot within [0, theMaxCount).
  //! UINT32_MAX (invalid sentinel) always fails this check for any realistic count.
  [[nodiscard]] bool IsValid(const uint32_t theMaxCount) const { return Index < theMaxCount; }

  template <typename CountProviderT>
  [[nodiscard]] auto IsValidIn(const CountProviderT& theProvider) const
    -> decltype(theProvider.Nb(), bool())
  {
    return IsValid(theProvider.Nb());
  }

  template <typename CountProviderT>
  [[nodiscard]] auto IsValidIn(const CountProviderT& theProvider) const
    -> decltype(theProvider.Size(), bool())
  {
    return IsValid(static_cast<uint32_t>(theProvider.Size()));
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

  //! Pre-increment (++id).
  BRepGraph_RefId& operator++()
  {
    Standard_ASSERT_VOID(Index != THE_INVALID_INDEX, "pre-increment on invalid id");
    ++Index;
    return *this;
  }

  //! Post-increment (id++).
  BRepGraph_RefId operator++(int)
  {
    Standard_ASSERT_VOID(Index != THE_INVALID_INDEX, "post-increment on invalid id");
    BRepGraph_RefId aPrev = *this;
    ++Index;
    return aPrev;
  }

  //! Advance by offset.
  [[nodiscard]] BRepGraph_RefId operator+(const uint32_t theOffset) const
  {
    return BRepGraph_RefId(RefKind, Index + theOffset);
  }

  //! Retreat by offset.
  [[nodiscard]] BRepGraph_RefId operator-(const uint32_t theOffset) const
  {
    Standard_ASSERT_VOID(Index != THE_INVALID_INDEX && Index >= theOffset,
                         "retreat underflows index");
    return BRepGraph_RefId(RefKind, Index - theOffset);
  }

  //! Dispatch a generic ref id to a callable taking the matching typed ref id.
  template <typename FuncT>
  static auto Visit(const BRepGraph_RefId theRefId, FuncT&& theFunc)
    -> decltype(std::forward<FuncT>(theFunc)(Typed<Kind::Shell>()))
  {
    switch (theRefId.RefKind)
    {
      case Kind::Shell:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Shell>::FromRefId(theRefId));
      case Kind::Face:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Face>::FromRefId(theRefId));
      case Kind::Wire:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Wire>::FromRefId(theRefId));
      case Kind::CoEdge:
        return std::forward<FuncT>(theFunc)(Typed<Kind::CoEdge>::FromRefId(theRefId));
      case Kind::Vertex:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Vertex>::FromRefId(theRefId));
      case Kind::Solid:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Solid>::FromRefId(theRefId));
      case Kind::Child:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Child>::FromRefId(theRefId));
      case Kind::Occurrence:
        return std::forward<FuncT>(theFunc)(Typed<Kind::Occurrence>::FromRefId(theRefId));
    }

    Standard_ASSERT_VOID(false, "BRepGraph_RefId::Visit: unhandled Kind");
    return std::forward<FuncT>(theFunc)(Typed<Kind::Shell>());
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
