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

#include <BRepGraph_Layer.hxx>
#include <BRepGraph_RefId.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_DynamicArray.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_GUID.hxx>

//! @brief Dense GUID-keyed runtime registry of graph layers.
//!
//! Stores registered layers in a compact vector for O(1) slot access and a
//! GUID-to-slot map for O(1) lookup by stable public identity.
class BRepGraph_LayerRegistry
{
public:
  DEFINE_STANDARD_ALLOC

  BRepGraph_LayerRegistry() = default;

  BRepGraph_LayerRegistry(const BRepGraph_LayerRegistry&)            = delete;
  BRepGraph_LayerRegistry& operator=(const BRepGraph_LayerRegistry&) = delete;

  BRepGraph_LayerRegistry(BRepGraph_LayerRegistry&&) noexcept            = default;
  BRepGraph_LayerRegistry& operator=(BRepGraph_LayerRegistry&&) noexcept = default;

  //! Bind the owning graph. Propagates to every registered layer.
  Standard_EXPORT void SetOwningGraph(BRepGraph* theGraph) noexcept;

  //! Owning graph bound via SetOwningGraph(), or nullptr.
  [[nodiscard]] BRepGraph* OwningGraph() const noexcept { return myOwningGraph; }

  //! Register a layer. Replaces an existing layer with the same GUID.
  //! @return slot index in the internal dense vector, or -1 for null input.
  Standard_EXPORT int RegisterLayer(const occ::handle<BRepGraph_Layer>& theLayer);

  //! Remove a layer by GUID.
  Standard_EXPORT void UnregisterLayer(const Standard_GUID& theGUID);

  //! Find a layer by GUID. Returns null handle if not found.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Layer> FindLayer(
    const Standard_GUID& theGUID) const;

  //! Typed convenience lookup by layer GUID.
  template <typename T>
  [[nodiscard]] occ::handle<T> FindLayer() const
  {
    return occ::down_cast<T>(FindLayer(T::GetID()));
  }

  //! Return current slot for a GUID, or -1 if not registered.
  [[nodiscard]] Standard_EXPORT int FindSlot(const Standard_GUID& theGUID) const;

  //! Return layer by slot index.
  [[nodiscard]] Standard_EXPORT const occ::handle<BRepGraph_Layer>& Layer(const int theSlot) const;

  //! Number of registered layers.
  [[nodiscard]] int NbLayers() const { return myLayers.Length(); }

  //! True if any registered layer subscribes to node modification events.
  [[nodiscard]] bool HasModificationSubscribers() const { return mySubscribedKindsMask != 0; }

  //! Bitwise OR of all registered layer node subscription masks.
  [[nodiscard]] int SubscribedKindsMask() const { return mySubscribedKindsMask; }

  //! Dispatch OnNodeRemoved to all registered layers.
  Standard_EXPORT void DispatchOnNodeRemoved(const BRepGraph_NodeId theNode,
                                             const BRepGraph_NodeId theReplacement) noexcept;

  //! Dispatch OnNodeModified to subscribed layers.
  Standard_EXPORT void DispatchNodeModified(const BRepGraph_NodeId theNode) noexcept;

  //! Dispatch OnNodesModified to subscribed layers.
  Standard_EXPORT void DispatchNodesModified(
    const NCollection_DynamicArray<BRepGraph_NodeId>& theModifiedNodes,
    const int                                   theModifiedKindsMask) noexcept;

  //! Dispatch OnCompact to all registered layers.
  Standard_EXPORT void DispatchOnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept;

  // --- Reference dispatch ---

  //! True if any registered layer subscribes to reference modification events.
  [[nodiscard]] bool HasRefModificationSubscribers() const { return mySubscribedRefKindsMask != 0; }

  //! Bitwise OR of all registered layer reference subscription masks.
  [[nodiscard]] int SubscribedRefKindsMask() const { return mySubscribedRefKindsMask; }

  //! Dispatch OnRefRemoved to all registered layers (unconditional - not filtered).
  Standard_EXPORT void DispatchOnRefRemoved(const BRepGraph_RefId theRef) noexcept;

  //! Dispatch OnRefModified to subscribed layers (immediate mode).
  Standard_EXPORT void DispatchRefModified(const BRepGraph_RefId theRef) noexcept;

  //! Dispatch OnRefsModified to subscribed layers (deferred/batch mode).
  Standard_EXPORT void DispatchRefsModified(
    const NCollection_DynamicArray<BRepGraph_RefId>& theModifiedRefs,
    const int                                  theModifiedRefKindsMask) noexcept;

  //! Clear all registered layer payloads without unregistering them.
  Standard_EXPORT void ClearAll() noexcept;

  //! Invalidate all registered layer payloads.
  Standard_EXPORT void InvalidateAll() noexcept;

private:
  Standard_EXPORT void recomputeSubscribedKindsMask();

private:
  NCollection_DynamicArray<occ::handle<BRepGraph_Layer>> myLayers;
  NCollection_DataMap<Standard_GUID, uint32_t>     myGuidToSlot;
  uint32_t                                         mySubscribedKindsMask    = 0;
  uint32_t                                         mySubscribedRefKindsMask = 0;
  BRepGraph*                                       myOwningGraph            = nullptr;
};

#endif // _BRepGraph_LayerRegistry_HeaderFile
