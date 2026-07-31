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

#ifndef _BRepGraph_ReverseIterator_HeaderFile
#define _BRepGraph_ReverseIterator_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <NCollection_ForwardRange.hxx>
#include <NCollection_LinearVector.hxx>
#include <type_traits>

//! @brief Single-level typed iterators over parent definitions via relation lists.
//!
//! These iterators wrap parent relation containers returned by TopoView accessors,
//! or derive parent definitions from const relation storage such as EdgeRelations::CoEdgeIds.
//! They provide a typed, skip-removed iteration pattern consistent with the
//! forward iterators in BRepGraph_DefsIterator and BRepGraph_RefsIterator.
//!
//! Usage:
//! @code
//!   // Traditional iteration:
//!   for (BRepGraph_FacesOfEdge anIt(aGraph, anEdgeId); anIt.More(); anIt.Next())
//!   {
//!     const BRepGraph_FaceId aFaceId = anIt.CurrentId();
//!   }
//!
//!   // Range-based for:
//!   for (const BRepGraph_FaceId aFaceId : BRepGraph_FacesOfEdge(aGraph, anEdgeId))
//!   {
//!     // ...
//!   }
//! @endcode
namespace BRepGraph_ReverseIterator
{

//! Compile-time traits mapping typed ID to its definition type and accessor.
template <typename TypedIdT>
struct DefTraits;

template <>
struct DefTraits<BRepGraph_SolidId>
{
  using DefType = BRepGraphInc::SolidDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_SolidId theId)
  {
    return theGraph.Topo().Solids().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_ShellId>
{
  using DefType = BRepGraphInc::ShellDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_ShellId theId)
  {
    return theGraph.Topo().Shells().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_FaceId>
{
  using DefType = BRepGraphInc::FaceDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_FaceId theId)
  {
    return theGraph.Topo().Faces().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_WireId>
{
  using DefType = BRepGraphInc::WireDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_WireId theId)
  {
    return theGraph.Topo().Wires().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_EdgeId>
{
  using DefType = BRepGraphInc::EdgeDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_EdgeId theId)
  {
    return theGraph.Topo().Edges().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_VertexId>
{
  using DefType = BRepGraphInc::VertexDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_VertexId theId)
  {
    return theGraph.Topo().Vertices().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_CoEdgeId>
{
  using DefType = BRepGraphInc::CoEdgeDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_CoEdgeId theId)
  {
    return theGraph.Topo().CoEdges().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_CompoundId>
{
  using DefType = BRepGraphInc::CompoundDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_CompoundId theId)
  {
    return theGraph.Topo().Compounds().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_CompSolidId>
{
  using DefType = BRepGraphInc::CompSolidDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_CompSolidId theId)
  {
    return theGraph.Topo().CompSolids().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_ProductId>
{
  using DefType = BRepGraphInc::ProductDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_ProductId theId)
  {
    return theGraph.Topo().Products().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_OccurrenceId>
{
  using DefType = BRepGraphInc::OccurrenceDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_OccurrenceId theId)
  {
    return theGraph.Topo().Occurrences().Definition(theId);
  }
};

//! Typed iterator over a relation vector of parent IDs.
//! Skips removed parent definitions automatically in sequential iteration.
//! Also provides indexed access (Length/Value) for callers that need
//! random access into the underlying vector (e.g. BRepGraph_ParentExplorer).
//! @tparam TypedIdT Typed ID such as BRepGraph_FaceId, BRepGraph_EdgeId, etc.
template <typename TypedIdT, typename ContainerT = NCollection_LinearVector<TypedIdT>>
class ParentsOf
{
public:
  ParentsOf(const BRepGraph& theGraph, const ContainerT& theParents)
      : myGraph(&theGraph),
        myParents(&theParents),
        myNbParents(theGraph.Topo().Gen().Nb(BRepGraph_NodeId(TypedIdT()).NodeKind))
  {
    skipRemoved();
  }

  //! Construct starting at a given vector index (for resumable iteration).
  //! Skips to the first non-removed entry at or after theStartIndex.
  ParentsOf(const BRepGraph& theGraph, const ContainerT& theParents, const uint32_t theStartIndex)
      : myGraph(&theGraph),
        myParents(&theParents),
        myNbParents(theGraph.Topo().Gen().Nb(BRepGraph_NodeId(TypedIdT()).NodeKind)),
        myIndex(theStartIndex)
  {
    skipRemoved();
  }

  [[nodiscard]] bool More() const { return myIndex < static_cast<uint32_t>(myParents->Size()); }

  void Next()
  {
    ++myIndex;
    skipRemoved();
  }

  [[nodiscard]] TypedIdT CurrentId() const
  {
    return myParents->Value(static_cast<size_t>(myIndex));
  }

  //! Alias for CurrentId(), enables range-for via NCollection_ForwardRange Current() priority.
  [[nodiscard]] TypedIdT Current() const { return CurrentId(); }

  //! Current parent definition (typed lookup via DefTraits).
  [[nodiscard]] const typename DefTraits<TypedIdT>::DefType& Definition() const
  {
    return DefTraits<TypedIdT>::Get(*myGraph, CurrentId());
  }

  [[nodiscard]] uint32_t Index() const { return myIndex; }

  [[nodiscard]] size_t Size() const { return myParents->Size(); }

  //! Returns the parent ID at the given bucket index (does NOT check removal status).
  [[nodiscard]] TypedIdT Value(const size_t theIndex) const { return myParents->Value(theIndex); }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<ParentsOf> begin()
  {
    return NCollection_ForwardRangeIterator<ParentsOf>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  void skipRemoved()
  {
    while (myIndex < static_cast<uint32_t>(myParents->Size()))
    {
      const TypedIdT aParentId = myParents->Value(static_cast<size_t>(myIndex));
      if (aParentId.IsValid(myNbParents) && !aParentId.IsRemoved(*myGraph))
      {
        return;
      }
      ++myIndex;
    }
  }

  const BRepGraph*  myGraph     = nullptr;
  const ContainerT* myParents   = nullptr;
  uint32_t          myNbParents = 0;
  uint32_t          myIndex     = 0;
};

template <typename TraitsT>
class EdgeParentsOf
{
public:
  using ParentId = typename TraitsT::ParentId;

  EdgeParentsOf(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
      : myGraph(&theGraph),
        myEdge(theEdge)
  {
    init();
    advance();
  }

  //! Construct starting at a given coedge relation index (for resumable iteration).
  EdgeParentsOf(const BRepGraph&       theGraph,
                const BRepGraph_EdgeId theEdge,
                const uint32_t         theStartIndex)
      : myGraph(&theGraph),
        myEdge(theEdge),
        myIndex(theStartIndex)
  {
    init();
    advance();
  }

  [[nodiscard]] bool More() const { return myHasCurrent; }

  void Next()
  {
    ++myIndex;
    advance();
  }

  [[nodiscard]] ParentId CurrentId() const { return myCurrent; }

  [[nodiscard]] ParentId Current() const { return CurrentId(); }

  [[nodiscard]] const typename DefTraits<ParentId>::DefType& Definition() const
  {
    return DefTraits<ParentId>::Get(*myGraph, CurrentId());
  }

  [[nodiscard]] uint32_t Index() const { return myIndex; }

  NCollection_ForwardRangeIterator<EdgeParentsOf> begin()
  {
    return NCollection_ForwardRangeIterator<EdgeParentsOf>(this);
  }

  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  void init()
  {
    if (!myEdge.IsValid(myGraph->Topo().Edges().Nb()) || myEdge.IsRemoved(*myGraph))
    {
      return;
    }
    myNbParents = TraitsT::NbParents(*myGraph);
    myCoEdges   = &myGraph->Topo().Edges().CoEdges(myEdge);
  }

  [[nodiscard]] ParentId parentAt(const uint32_t theIndex) const
  {
    if (myCoEdges == nullptr || theIndex >= static_cast<uint32_t>(myCoEdges->Size()))
    {
      return ParentId();
    }
    const BRepGraph_CoEdgeId aCoEdgeId = myCoEdges->Value(static_cast<size_t>(theIndex));
    if (!aCoEdgeId.IsValid(myGraph->Topo().CoEdges().Nb()) || aCoEdgeId.IsRemoved(*myGraph))
    {
      return ParentId();
    }

    const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph->Topo().CoEdges().Definition(aCoEdgeId);
    const ParentId                 aParent = TraitsT::ParentIdOf(aCoEdge);
    if (!aParent.IsValid(myNbParents) || aParent.IsRemoved(*myGraph))
    {
      return ParentId();
    }
    return aParent;
  }

  [[nodiscard]] bool isFirstOccurrence(const ParentId theParent, const uint32_t theIndex) const
  {
    for (uint32_t anIndex = 0; anIndex < theIndex; ++anIndex)
    {
      if (parentAt(anIndex) == theParent)
      {
        return false;
      }
    }
    return true;
  }

  void advance()
  {
    myHasCurrent = false;
    if (myCoEdges == nullptr)
    {
      return;
    }

    while (myIndex < static_cast<uint32_t>(myCoEdges->Size()))
    {
      const ParentId aParent = parentAt(myIndex);
      if (aParent.IsValid() && isFirstOccurrence(aParent, myIndex))
      {
        myCurrent    = aParent;
        myHasCurrent = true;
        return;
      }
      ++myIndex;
    }
  }

  const BRepGraph*                                    myGraph = nullptr;
  BRepGraph_EdgeId                                    myEdge;
  const NCollection_LinearVector<BRepGraph_CoEdgeId>* myCoEdges = nullptr;
  ParentId                                            myCurrent;
  uint32_t                                            myNbParents  = 0;
  uint32_t                                            myIndex      = 0;
  bool                                                myHasCurrent = false;
};

//! Result pair returned by parent-ref iterators: parent definition ID + the RefId
//! in that parent which references the child.
template <typename ParentIdT, typename RefIdT>
struct ParentRef
{
  ParentIdT ParentId;
  RefIdT    Ref;
};

//! Typed iterator over parent ID relation lists that also resolves the specific
//! RefId linking each parent to the child by lookup in the parent definition.
//! Used only where the reverse relation stores parent IDs but no ref IDs.
//! @tparam TraitsT Traits with: ParentId, ChildId, RefId types,
//!   FindRef(graph, parentId, childId) -> RefId (invalid if not found)
template <typename TraitsT>
class LookupParentRefsOf
{
public:
  using ParentIdType  = typename TraitsT::ParentId;
  using ChildIdType   = typename TraitsT::ChildId;
  using RefIdType     = typename TraitsT::RefId;
  using ResultType    = ParentRef<ParentIdType, RefIdType>;
  using ContainerType = typename TraitsT::ContainerType;

  LookupParentRefsOf(const BRepGraph&     theGraph,
                     const ContainerType& theParents,
                     const ChildIdType    theChild)
      : myGraph(&theGraph),
        myParents(&theParents),
        myChild(theChild),
        myNbParents(theGraph.Topo().Gen().Nb(BRepGraph_NodeId(ParentIdType()).NodeKind)),
        myNbRefs([&]() {
          if constexpr (std::is_convertible_v<RefIdType, BRepGraph_NodeId>)
          {
            return theGraph.Topo().Gen().Nb(BRepGraph_NodeId(RefIdType()).NodeKind);
          }
          else
          {
            return theGraph.Refs().Gen().Nb(BRepGraph_RefId(RefIdType()).RefKind);
          }
        }())
  {
    advance();
  }

  [[nodiscard]] bool More() const { return myHasCurrent; }

  void Next()
  {
    ++myIndex;
    advance();
  }

  [[nodiscard]] ResultType Current() const { return myCurrent; }

  [[nodiscard]] ParentIdType CurrentParentId() const { return myCurrent.ParentId; }

  [[nodiscard]] RefIdType CurrentRefId() const { return myCurrent.Ref; }

  [[nodiscard]] uint32_t Index() const { return myIndex; }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<LookupParentRefsOf> begin()
  {
    return NCollection_ForwardRangeIterator<LookupParentRefsOf>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  void advance()
  {
    myHasCurrent = false;
    while (myIndex < static_cast<uint32_t>(myParents->Size()))
    {
      const ParentIdType aParentId = myParents->Value(static_cast<size_t>(myIndex));
      if (aParentId.IsValid(myNbParents) && !aParentId.IsRemoved(*myGraph))
      {
        const RefIdType aRefId = TraitsT::FindRef(*myGraph, aParentId, myChild);
        if (aRefId.IsValid(myNbRefs) && !aRefId.IsRemoved(*myGraph))
        {
          myCurrent    = ResultType{aParentId, aRefId};
          myHasCurrent = true;
          return;
        }
      }
      ++myIndex;
    }
  }

  const BRepGraph*     myGraph   = nullptr;
  const ContainerType* myParents = nullptr;
  ChildIdType          myChild;
  ResultType           myCurrent;
  uint32_t             myNbParents  = 0;
  uint32_t             myNbRefs     = 0;
  uint32_t             myIndex      = 0;
  bool                 myHasCurrent = false;
};

template <typename TraitsT>
class IdsOfRefs
{
public:
  using IdType        = typename TraitsT::IdType;
  using RefIdType     = typename TraitsT::RefIdType;
  using ContainerType = typename TraitsT::ContainerType;

  IdsOfRefs(const BRepGraph& theGraph, const ContainerType& theRefs)
      : myGraph(&theGraph),
        myRefs(&theRefs),
        myNbRefs(theGraph.Refs().Gen().Nb(BRepGraph_RefId(RefIdType()).RefKind)),
        myNbIds(theGraph.Topo().Gen().Nb(BRepGraph_NodeId(IdType()).NodeKind))
  {
    advance();
  }

  IdsOfRefs(const BRepGraph& theGraph, const ContainerType& theRefs, const uint32_t theStartIndex)
      : myGraph(&theGraph),
        myRefs(&theRefs),
        myNbRefs(theGraph.Refs().Gen().Nb(BRepGraph_RefId(RefIdType()).RefKind)),
        myNbIds(theGraph.Topo().Gen().Nb(BRepGraph_NodeId(IdType()).NodeKind)),
        myIndex(theStartIndex)
  {
    advance();
  }

  [[nodiscard]] bool More() const { return myHasCurrent; }

  void Next()
  {
    ++myIndex;
    advance();
  }

  [[nodiscard]] IdType CurrentId() const { return myCurrent; }

  [[nodiscard]] IdType CurrentParentId() const { return CurrentId(); }

  [[nodiscard]] RefIdType CurrentRefId() const { return myCurrentRef; }

  [[nodiscard]] IdType Current() const { return CurrentId(); }

  [[nodiscard]] const typename DefTraits<IdType>::DefType& Definition() const
  {
    return DefTraits<IdType>::Get(*myGraph, CurrentId());
  }

  [[nodiscard]] uint32_t Index() const { return myIndex; }

  NCollection_ForwardRangeIterator<IdsOfRefs> begin()
  {
    return NCollection_ForwardRangeIterator<IdsOfRefs>(this);
  }

  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  void advance()
  {
    myHasCurrent = false;
    while (myIndex < static_cast<uint32_t>(myRefs->Size()))
    {
      const RefIdType aRefId = myRefs->Value(static_cast<size_t>(myIndex));
      if (aRefId.IsValid(myNbRefs) && !aRefId.IsRemoved(*myGraph))
      {
        const IdType anId = TraitsT::Id(*myGraph, aRefId);
        if (anId.IsValid(myNbIds) && !anId.IsRemoved(*myGraph))
        {
          myCurrent    = anId;
          myCurrentRef = aRefId;
          myHasCurrent = true;
          return;
        }
      }
      ++myIndex;
    }
  }

  const BRepGraph*     myGraph = nullptr;
  const ContainerType* myRefs  = nullptr;
  IdType               myCurrent;
  RefIdType            myCurrentRef;
  uint32_t             myNbRefs     = 0;
  uint32_t             myNbIds      = 0;
  uint32_t             myIndex      = 0;
  bool                 myHasCurrent = false;
};

struct WireOfCoEdgeUsageTraits
{
  using ParentId      = BRepGraph_WireId;
  using ChildId       = BRepGraph_CoEdgeId;
  using RefId         = BRepGraph_CoEdgeId;
  using ContainerType = NCollection_LinearVector<ParentId>;

  static RefId FindRef(const BRepGraph&         theGraph,
                       const BRepGraph_WireId   theParent,
                       const BRepGraph_CoEdgeId theChild)
  {
    for (BRepGraph_CoEdgesOfWire aIt(theGraph, theParent); aIt.More(); aIt.Next())
    {
      if (aIt.CurrentId() == theChild)
      {
        return aIt.CurrentId();
      }
    }
    return RefId();
  }
};

struct WireFromEdgeCoEdgeTraits
{
  using ParentId = BRepGraph_WireId;

  static ParentId ParentIdOf(const BRepGraphInc::CoEdgeDef& theCoEdge)
  {
    return theCoEdge.ParentWireId;
  }

  static uint32_t NbParents(const BRepGraph& theGraph) { return theGraph.Topo().Wires().Nb(); }
};

struct FaceFromEdgeCoEdgeTraits
{
  using ParentId = BRepGraph_FaceId;

  static ParentId ParentIdOf(const BRepGraphInc::CoEdgeDef& theCoEdge) { return theCoEdge.FaceId; }

  static uint32_t NbParents(const BRepGraph& theGraph) { return theGraph.Topo().Faces().Nb(); }
};

struct EdgeOfVertexRefTraits
{
  using ParentId      = BRepGraph_EdgeId;
  using ChildId       = BRepGraph_VertexId;
  using RefId         = BRepGraph_VertexRefId;
  using ContainerType = NCollection_LinearVector<ParentId>;

  static RefId FindRef(const BRepGraph&         theGraph,
                       const BRepGraph_EdgeId   theParent,
                       const BRepGraph_VertexId theChild)
  {
    for (BRepGraph_RefsVertexOfEdge aIt(theGraph, theParent); aIt.More(); aIt.Next())
    {
      if (theGraph.Refs().Gen().ChildNode(aIt.CurrentId()) == BRepGraph_NodeId(theChild))
      {
        return aIt.CurrentId();
      }
    }
    return RefId();
  }
};

struct FaceFromWireRefTraits
{
  using IdType        = BRepGraph_FaceId;
  using RefIdType     = BRepGraph_WireRefId;
  using ContainerType = NCollection_LinearVector<RefIdType>;

  static IdType Id(const BRepGraph& theGraph, const RefIdType theRefId)
  {
    return theGraph.Refs().Wires().Entry(theRefId).ParentFaceId;
  }
};

struct ShellFromFaceRefTraits
{
  using IdType        = BRepGraph_ShellId;
  using RefIdType     = BRepGraph_FaceRefId;
  using ContainerType = NCollection_LinearVector<RefIdType>;

  static IdType Id(const BRepGraph& theGraph, const RefIdType theRefId)
  {
    return theGraph.Refs().Faces().Entry(theRefId).ParentShellId;
  }
};

struct SolidFromShellRefTraits
{
  using IdType        = BRepGraph_SolidId;
  using RefIdType     = BRepGraph_ShellRefId;
  using ContainerType = NCollection_LinearVector<RefIdType>;

  static IdType Id(const BRepGraph& theGraph, const RefIdType theRefId)
  {
    return theGraph.Refs().Shells().Entry(theRefId).ParentSolidId;
  }
};

struct CompSolidFromSolidRefTraits
{
  using IdType        = BRepGraph_CompSolidId;
  using RefIdType     = BRepGraph_SolidRefId;
  using ContainerType = NCollection_LinearVector<RefIdType>;

  static IdType Id(const BRepGraph& theGraph, const RefIdType theRefId)
  {
    return theGraph.Refs().Solids().Entry(theRefId).ParentCompSolidId;
  }
};

struct CompoundFromChildRefTraits
{
  using IdType        = BRepGraph_CompoundId;
  using RefIdType     = BRepGraph_ChildRefId;
  using ContainerType = NCollection_LinearVector<RefIdType>;

  static IdType Id(const BRepGraph& theGraph, const RefIdType theRefId)
  {
    return theGraph.Refs().Children().Entry(theRefId).ParentCompoundId;
  }
};

struct OccurrenceFromOccurrenceRefTraits
{
  using IdType        = BRepGraph_OccurrenceId;
  using RefIdType     = BRepGraph_OccurrenceRefId;
  using ContainerType = NCollection_LinearVector<RefIdType>;

  static IdType Id(const BRepGraph& theGraph, const RefIdType theRefId)
  {
    return theGraph.Refs().Occurrences().Entry(theRefId).ChildOccurrenceId;
  }
};

struct ProductFromOccurrenceRefTraits
{
  using IdType        = BRepGraph_ProductId;
  using RefIdType     = BRepGraph_OccurrenceRefId;
  using ContainerType = NCollection_LinearVector<RefIdType>;

  static IdType Id(const BRepGraph& theGraph, const RefIdType theRefId)
  {
    return theGraph.Refs().Occurrences().Entry(theRefId).ParentProductId;
  }
};

} // namespace BRepGraph_ReverseIterator

// Vertex -> parent Edges
using BRepGraph_EdgesOfVertex =
  BRepGraph_ReverseIterator::ParentsOf<BRepGraph_EdgeId,
                                       NCollection_LinearVector<BRepGraph_EdgeId>>;
// Vertex -> parent Compounds
using BRepGraph_CompoundsOfVertex =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;

// Edge -> parent Wires
class BRepGraph_WiresOfEdge : public BRepGraph_ReverseIterator::EdgeParentsOf<
                                BRepGraph_ReverseIterator::WireFromEdgeCoEdgeTraits>
{
public:
  using BRepGraph_ReverseIterator::EdgeParentsOf<
    BRepGraph_ReverseIterator::WireFromEdgeCoEdgeTraits>::EdgeParentsOf;
};

// Edge -> parent CoEdges
using BRepGraph_CoEdgesOfEdge =
  BRepGraph_ReverseIterator::ParentsOf<BRepGraph_CoEdgeId,
                                       NCollection_LinearVector<BRepGraph_CoEdgeId>>;

// Edge -> parent Faces (derived from CoEdge.FaceId)
class BRepGraph_FacesOfEdge : public BRepGraph_ReverseIterator::EdgeParentsOf<
                                BRepGraph_ReverseIterator::FaceFromEdgeCoEdgeTraits>
{
public:
  using BRepGraph_ReverseIterator::EdgeParentsOf<
    BRepGraph_ReverseIterator::FaceFromEdgeCoEdgeTraits>::EdgeParentsOf;
};

// Edge -> parent Compounds
using BRepGraph_CompoundsOfEdge =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;
// CoEdge -> parent Compounds
using BRepGraph_CompoundsOfCoEdge =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;
// Wire -> parent Faces
using BRepGraph_FacesOfWire =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::FaceFromWireRefTraits>;
// Wire -> parent Compounds
using BRepGraph_CompoundsOfWire =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;
// Face -> parent Shells
using BRepGraph_ShellsOfFace =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::ShellFromFaceRefTraits>;
// Face -> parent Compounds
using BRepGraph_CompoundsOfFace =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;
// Shell -> parent Solids
using BRepGraph_SolidsOfShell =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::SolidFromShellRefTraits>;
// Shell -> parent Compounds
using BRepGraph_CompoundsOfShell =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;
// Solid -> parent CompSolids
using BRepGraph_CompSolidsOfSolid =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompSolidFromSolidRefTraits>;
// Solid -> parent Compounds
using BRepGraph_CompoundsOfSolid =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;
// CompSolid -> parent Compounds
using BRepGraph_CompoundsOfCompSolid =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;
// Compound -> parent Compounds
using BRepGraph_CompoundsOfCompound =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;
// Any child -> parent Compounds
using BRepGraph_CompoundsOfChild =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;
// Product -> Occurrences
using BRepGraph_OccurrencesOfProduct = BRepGraph_ReverseIterator::IdsOfRefs<
  BRepGraph_ReverseIterator::OccurrenceFromOccurrenceRefTraits>;
// Any occurrence child -> Occurrences
using BRepGraph_OccurrencesOfChild = BRepGraph_ReverseIterator::IdsOfRefs<
  BRepGraph_ReverseIterator::OccurrenceFromOccurrenceRefTraits>;
// Occurrence -> parent Products
using BRepGraph_ProductsOfOccurrence =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::ProductFromOccurrenceRefTraits>;

// Ref-based reverse iterators: yield (ParentId, RefId) pairs.
// These find the specific reference entry in each parent that links to the child.

// Wire -> parent Faces (with WireRefId)
using BRepGraph_RefsFacesOfWire =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::FaceFromWireRefTraits>;
// Face -> parent Shells (with FaceRefId)
using BRepGraph_RefsShellsOfFace =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::ShellFromFaceRefTraits>;
// Shell -> parent Solids (with ShellRefId)
using BRepGraph_RefsSolidsOfShell =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::SolidFromShellRefTraits>;
// CoEdge -> parent Wires (with direct CoEdgeId usage in each wire)
using BRepGraph_RefsWiresOfCoEdge =
  BRepGraph_ReverseIterator::LookupParentRefsOf<BRepGraph_ReverseIterator::WireOfCoEdgeUsageTraits>;
// Vertex -> parent Edges (with VertexRefId)
using BRepGraph_RefsEdgesOfVertex =
  BRepGraph_ReverseIterator::LookupParentRefsOf<BRepGraph_ReverseIterator::EdgeOfVertexRefTraits>;
// Solid -> parent CompSolids (with SolidRefId)
using BRepGraph_RefsCompSolidsOfSolid =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompSolidFromSolidRefTraits>;
// Any child -> parent Compounds (with ChildRefId)
using BRepGraph_RefsCompoundsOfChild =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>;
// Occurrence -> parent Products (with OccurrenceRefId)
using BRepGraph_RefsProductsOfOccurrence =
  BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::ProductFromOccurrenceRefTraits>;

#endif // _BRepGraph_ReverseIterator_HeaderFile
