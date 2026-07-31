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

#ifndef _BRepGraph_Cache_HeaderFile
#define _BRepGraph_Cache_HeaderFile

#include <BRepGraph_ItemId.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

#include <cstdint>

class BRepGraph;
class BRepGraph_CacheRegistry;
class BRepGraph_CopyRemap;

//! @brief Lightweight owner-bound base for transient graph cache services.
//!
//! A cache service stores typed, recomputable, graph-local data such as
//! bounding boxes, UV bounds, or display-resolution results. The registry owns
//! only service identity and lifetime binding; concrete caches own their own
//! typed storage and validate freshness lazily via graph generation counters.
class BRepGraph_Cache : public Standard_Transient
{
public:
  //! Cache service identity, unique within a graph registry.
  [[nodiscard]] virtual const Standard_GUID& ID() const = 0;

  //! Cache service display name.
  [[nodiscard]] virtual const TCollection_AsciiString& Name() const = 0;

  //! Clear all transient data owned by this cache.
  Standard_EXPORT virtual void Clear() noexcept;

  //! Copy fresh, remappable cache data into the target graph described by the remap.
  //! Default implementation copies nothing.
  Standard_EXPORT virtual void CopyFreshTo(const BRepGraph_CopyRemap& theCopy) const;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_Cache, Standard_Transient)

protected:
  //! @brief Value base for node-derived cache entries.
  //!
  //! Concrete cache services inherit this from their private entry structs and
  //! store entries by value in their own typed storage. The base records the
  //! node identity plus the generation counter used to validate freshness.
  class NodeEntry
  {
  public:
    //! Reset to an unbound stale state.
    Standard_EXPORT void Reset() noexcept;

    //! Bind this entry to the current OwnGen of a node.
    //! @return false when the cache is detached or the node is inactive
    [[nodiscard]] Standard_EXPORT bool BindOwnGen(const BRepGraph_Cache& theCache,
                                                  const BRepGraph_NodeId theNode) noexcept;

    //! Bind this entry to the current SubtreeGen of a node.
    //! @return false when the cache is detached or the node is inactive
    [[nodiscard]] Standard_EXPORT bool BindSubtreeGen(const BRepGraph_Cache& theCache,
                                                      const BRepGraph_NodeId theNode) noexcept;

    //! Validate against the current OwnGen of the same node.
    [[nodiscard]] Standard_EXPORT bool IsFreshOwn(const BRepGraph_Cache& theCache,
                                                  const BRepGraph_NodeId theNode) const noexcept;

    //! Validate against the current SubtreeGen of the same node.
    [[nodiscard]] Standard_EXPORT bool IsFreshSubtree(
      const BRepGraph_Cache& theCache,
      const BRepGraph_NodeId theNode) const noexcept;

    //! Validate against the generation kind captured at bind time.
    [[nodiscard]] Standard_EXPORT bool IsFresh(const BRepGraph_Cache& theCache) const noexcept;

    //! True if the entry was successfully bound to a node generation.
    [[nodiscard]] bool IsBound() const noexcept { return myKind != GenKind::None; }

    //! Node identity captured at bind time.
    [[nodiscard]] BRepGraph_NodeId Node() const noexcept { return myNode; }

  private:
    enum class GenKind : uint8_t
    {
      None,
      Own,
      Subtree
    };

    [[nodiscard]] Standard_EXPORT bool bind(const BRepGraph_Cache& theCache,
                                            const BRepGraph_NodeId theNode,
                                            const GenKind          theKind) noexcept;

    [[nodiscard]] Standard_EXPORT bool isFresh(const BRepGraph_Cache& theCache,
                                               const BRepGraph_NodeId theNode,
                                               const GenKind          theKind) const noexcept;

    BRepGraph_NodeId myNode;
    uint32_t         myGeneration = 0;
    GenKind          myKind       = GenKind::None;
  };

  //! @brief Value base for reference-derived cache entries.
  class RefEntry
  {
  public:
    //! Reset to an unbound stale state.
    Standard_EXPORT void Reset() noexcept;

    //! Bind this entry to the current OwnGen of a reference.
    //! @return false when the cache is detached or the reference is inactive
    [[nodiscard]] Standard_EXPORT bool BindOwnGen(const BRepGraph_Cache& theCache,
                                                  const BRepGraph_RefId  theRef) noexcept;

    //! Validate against the current OwnGen of the same reference.
    [[nodiscard]] Standard_EXPORT bool IsFreshOwn(const BRepGraph_Cache& theCache,
                                                  const BRepGraph_RefId  theRef) const noexcept;

    //! Validate against the reference captured at bind time.
    [[nodiscard]] Standard_EXPORT bool IsFresh(const BRepGraph_Cache& theCache) const noexcept;

    //! True if the entry was successfully bound to a reference generation.
    [[nodiscard]] bool IsBound() const noexcept { return myIsBound; }

    //! Reference identity captured at bind time.
    [[nodiscard]] BRepGraph_RefId Ref() const noexcept { return myRef; }

  private:
    BRepGraph_RefId myRef;
    uint32_t        myGeneration = 0;
    bool            myIsBound    = false;
  };

  //! @brief Value base for item-derived cache entries.
  class ItemEntry
  {
  public:
    //! Reset to an unbound stale state.
    Standard_EXPORT void Reset() noexcept;

    //! Bind this entry to the current OwnGen of a graph item.
    //! @return false when the cache is detached or the item is inactive
    [[nodiscard]] Standard_EXPORT bool BindOwnGen(const BRepGraph_Cache& theCache,
                                                  const BRepGraph_ItemId theItem) noexcept;

    //! Validate against the current OwnGen of the same item.
    [[nodiscard]] Standard_EXPORT bool IsFreshOwn(const BRepGraph_Cache& theCache,
                                                  const BRepGraph_ItemId theItem) const noexcept;

    //! Validate against the item captured at bind time.
    [[nodiscard]] Standard_EXPORT bool IsFresh(const BRepGraph_Cache& theCache) const noexcept;

    //! True if the entry was successfully bound to an item generation.
    [[nodiscard]] bool IsBound() const noexcept { return myIsBound; }

    //! Item identity captured at bind time.
    [[nodiscard]] BRepGraph_ItemId Item() const noexcept { return myItem; }

  private:
    BRepGraph_ItemId myItem;
    uint32_t         myGeneration = 0;
    bool             myIsBound    = false;
  };

  Standard_EXPORT BRepGraph_Cache();

  //! True while this cache is registered in a live graph registry.
  [[nodiscard]] bool IsAttached() const noexcept { return myGraph != nullptr; }

  //! Attached graph for read-only cache services. Raises Standard_ProgramError if detached.
  [[nodiscard]] Standard_EXPORT const BRepGraph& Graph() const;

  //! Attached mutable graph for graph-owned cache services. Returns null if detached.
  [[nodiscard]] BRepGraph* AttachedGraph() const noexcept { return myGraph; }

  //! Called after the cache is attached to a graph registry.
  Standard_EXPORT virtual void OnAttached() noexcept;

  //! Called before the cache is detached from a graph registry.
  Standard_EXPORT virtual void OnDetached() noexcept;

  //! Return current SubtreeGen for an active node.
  [[nodiscard]] Standard_EXPORT bool NodeSubtreeGen(const BRepGraph_NodeId theNode,
                                                    uint32_t&              theGen) const noexcept;

  //! Return current OwnGen for an active node.
  [[nodiscard]] Standard_EXPORT bool NodeOwnGen(const BRepGraph_NodeId theNode,
                                                uint32_t&              theGen) const noexcept;

  //! Return current OwnGen for an active reference.
  [[nodiscard]] Standard_EXPORT bool RefOwnGen(const BRepGraph_RefId theRef,
                                               uint32_t&             theGen) const noexcept;

  //! Return current OwnGen for an active graph item.
  [[nodiscard]] Standard_EXPORT bool ItemOwnGen(const BRepGraph_ItemId theItem,
                                                uint32_t&              theGen) const noexcept;

  //! Resolve an active reference to its active child node.
  [[nodiscard]] Standard_EXPORT bool ResolveActiveRefChild(
    const BRepGraph_RefId theRef,
    BRepGraph_NodeId&     theNode) const noexcept;

  //! Resolve an active face reference to its active face node.
  [[nodiscard]] Standard_EXPORT bool ResolveActiveFaceRef(const BRepGraph_FaceRefId theRef,
                                                          BRepGraph_FaceId& theFace) const noexcept;

private:
  friend class ::BRepGraph_CacheRegistry;

  Standard_EXPORT void attachGraph(BRepGraph* theGraph) noexcept;
  Standard_EXPORT void rebindGraph(BRepGraph* theGraph) noexcept;
  Standard_EXPORT void detachGraph() noexcept;

  BRepGraph* myGraph = nullptr;
};

#endif // _BRepGraph_Cache_HeaderFile
