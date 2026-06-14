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

#ifndef _BRepGraph_LayerRegistry_HeaderFile
#define _BRepGraph_LayerRegistry_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_RefId.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_GUID.hxx>

#include <functional>
#include <mutex>
#include <shared_mutex>
#include <atomic>

//! @brief Dense GUID-keyed runtime registry of graph layers.
//!
//! Stores registered layers in a compact vector for O(1) slot access and a
//! GUID-to-slot map for O(1) lookup by stable public identity.
class BRepGraph_LayerRegistry
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepGraph_LayerRegistry();

  BRepGraph_LayerRegistry(const BRepGraph_LayerRegistry&)            = delete;
  BRepGraph_LayerRegistry& operator=(const BRepGraph_LayerRegistry&) = delete;

  Standard_EXPORT BRepGraph_LayerRegistry(BRepGraph_LayerRegistry&& theOther) noexcept;
  Standard_EXPORT BRepGraph_LayerRegistry& operator=(BRepGraph_LayerRegistry&& theOther) noexcept;

  //! Register a layer. Replaces an existing layer with the same GUID.
  //! @return slot index in the internal dense vector.
  Standard_EXPORT uint32_t RegisterLayer(const occ::handle<BRepGraph_Layer>& theLayer);

  //! Remove a layer by GUID.
  Standard_EXPORT void UnregisterLayer(const Standard_GUID& theGUID);

  //! Find a layer by GUID. Returns null handle if not found.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Layer> FindLayer(
    const Standard_GUID& theGUID) const;

  //! Typed convenience lookup by layer GUID.
  template <typename T>
  [[nodiscard]] occ::handle<T> FindLayer() const
  {
    return Find<T>();
  }

  //! Typed lookup by layer GUID.
  template <typename T>
  [[nodiscard]] occ::handle<T> Find() const
  {
    return occ::down_cast<T>(FindLayer(T::GetID()));
  }

  //! Return an existing layer or create and register a default one.
  //! Template convenience wrapper: extracts GUID and calls ensureLayer.
  template <typename T>
  [[nodiscard]] occ::handle<T> Ensure()
  {
    return occ::down_cast<T>(
      ensureLayer(T::GetID(), []() -> occ::handle<BRepGraph_Layer> { return new T(); }));
  }

  //! Return current slot for a GUID.
  [[nodiscard]] Standard_EXPORT bool FindSlot(const Standard_GUID& theGUID,
                                              uint32_t&            theSlot) const;

  //! Return layer by slot index, or null handle if the slot is out of range.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Layer> Layer(uint32_t theSlot) const;

  //! Number of registered layers.
  [[nodiscard]] uint32_t NbLayers() const
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    return static_cast<uint32_t>(myLayers.Size());
  }

  //! True if any registered layer subscribes to node modification events.
  [[nodiscard]] bool HasModificationSubscribers() const
  {
    return mySubscribedKindsMask.load(std::memory_order_acquire) != 0;
  }

  //! Bitwise OR of all registered layer node subscription masks.
  [[nodiscard]] int SubscribedKindsMask() const
  {
    return static_cast<int>(mySubscribedKindsMask.load(std::memory_order_acquire));
  }

  //! Dispatch OnNodeRemoved to all registered layers.
  Standard_EXPORT void DispatchOnNodeRemoved(const BRepGraph_NodeId theNode) noexcept;

  //! Dispatch generic item removal to all registered layers.
  Standard_EXPORT void DispatchOnItemRemoved(const BRepGraph_ItemId theItem) noexcept;

  //! Dispatch OnNodeReplaced to all registered layers.
  Standard_EXPORT void DispatchOnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                              const BRepGraph_NodeId theNewNode) noexcept;

  //! Dispatch OnNodeModified to subscribed layers.
  Standard_EXPORT void DispatchNodeModified(const BRepGraph_NodeId theNode) noexcept;

  //! Dispatch generic item modification through the matching typed subscription path.
  Standard_EXPORT void DispatchItemModified(const BRepGraph_ItemId theItem) noexcept;

  //! Dispatch OnNodesModified to subscribed layers.
  Standard_EXPORT void DispatchNodesModified(
    const NCollection_Array1<BRepGraph_NodeId>& theModifiedNodes,
    const int                                   theModifiedKindsMask) noexcept;

  //! Ask every registered source layer to copy itself into the target graph.
  //! For Mode::Compact, layers are unregistered first and CopyTo creates fresh instances.
  //! @param[in] theTargetGraph target graph to receive layer data
  //! @param[in] theItemRemap   source -> target item id mapping
  //! @param[in] theMode        Copy or Compact semantics
  Standard_EXPORT void CopyLayersTo(
    BRepGraph&                                                         theTargetGraph,
    const NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>& theItemRemap,
    const BRepGraph_CopyRemap::Mode                                    theMode) const;

  //! Ask every registered source layer to copy itself using identity mapping.
  //! Source item ids are the same as target item ids (full identity copy).
  //! @param[in] theTargetGraph target graph to receive layer data
  //! @param[in] theMappingKind identity or explicit mapping
  //! @param[in] theMode        Copy or Compact semantics
  Standard_EXPORT void CopyLayersTo(BRepGraph&                       theTargetGraph,
                                    BRepGraph_CopyRemap::MappingKind theMappingKind,
                                    BRepGraph_CopyRemap::Mode        theMode) const;

  //! True if any registered layer subscribes to reference modification events.
  [[nodiscard]] bool HasRefModificationSubscribers() const
  {
    return mySubscribedRefKindsMask.load(std::memory_order_acquire) != 0;
  }

  //! Bitwise OR of all registered layer reference subscription masks.
  [[nodiscard]] int SubscribedRefKindsMask() const
  {
    return static_cast<int>(mySubscribedRefKindsMask.load(std::memory_order_acquire));
  }

  //! Dispatch OnRefRemoved to all registered layers (unconditional - not filtered).
  Standard_EXPORT void DispatchOnRefRemoved(const BRepGraph_RefId theRef) noexcept;

  //! Dispatch OnRefModified to subscribed layers (immediate mode).
  Standard_EXPORT void DispatchRefModified(const BRepGraph_RefId theRef) noexcept;

  //! Dispatch OnRefsModified to subscribed layers (deferred/batch mode).
  Standard_EXPORT void DispatchRefsModified(
    const NCollection_Array1<BRepGraph_RefId>& theModifiedRefs,
    const int                                  theModifiedRefKindsMask) noexcept;

  //! Clear all registered layer data without unregistering services.
  Standard_EXPORT void ClearAll() noexcept;

  //! Invalidate all registered layer data.
  Standard_EXPORT void InvalidateAll() noexcept;

private:
  friend class ::BRepGraph;
  friend struct ::BRepGraph_Data;

  //! Attach this registry to graph owner. Propagates context to registered layers.
  Standard_EXPORT void Attach(BRepGraph* theGraph) noexcept;

  //! Clear the graph data binding.
  Standard_EXPORT void Detach() noexcept;

  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Layer> findLayerLocked(
    const Standard_GUID& theGUID) const;

  //! Return an existing layer or create and register a default one.
  //! Uses double-checked locking: shared lock for fast path (layer exists),
  //! exclusive lock only for creation (rare, first-call only).
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Layer> ensureLayer(
    const Standard_GUID&                                 theGUID,
    const std::function<occ::handle<BRepGraph_Layer>()>& theFactory);

  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Layer> layerAt(uint32_t theSlot) const;

  Standard_EXPORT uint32_t registerLayerLocked(const occ::handle<BRepGraph_Layer>& theLayer);

  Standard_EXPORT void recomputeSubscribedKindsMask();
  Standard_EXPORT void detachAllLocked() noexcept;

private:
  NCollection_LinearVector<occ::handle<BRepGraph_Layer>> myLayers;
  NCollection_DataMap<Standard_GUID, uint32_t>           myGuidToSlot;
  std::atomic<uint32_t>                                  mySubscribedKindsMask{0};
  std::atomic<uint32_t>                                  mySubscribedRefKindsMask{0};
  BRepGraph*                                             myGraph = nullptr;
  mutable std::shared_mutex                              myMutex;
};

#endif // _BRepGraph_LayerRegistry_HeaderFile
