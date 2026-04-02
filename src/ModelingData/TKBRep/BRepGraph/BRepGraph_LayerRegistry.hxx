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

#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
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

  //! True if any registered layer subscribes to modification events.
  [[nodiscard]] bool HasModificationSubscribers() const { return mySubscribedKindsMask != 0; }

  //! Bitwise OR of all registered layer subscription masks.
  [[nodiscard]] int SubscribedKindsMask() const { return mySubscribedKindsMask; }

  //! Dispatch OnNodeRemoved to all registered layers.
  Standard_EXPORT void DispatchOnNodeRemoved(const BRepGraph_NodeId theNode,
                                             const BRepGraph_NodeId theReplacement) noexcept;

  //! Dispatch OnNodeModified to subscribed layers.
  Standard_EXPORT void DispatchNodeModified(const BRepGraph_NodeId theNode) noexcept;

  //! Dispatch OnNodesModified to subscribed layers.
  Standard_EXPORT void DispatchNodesModified(
    const NCollection_Vector<BRepGraph_NodeId>& theModifiedNodes,
    const int                                   theModifiedKindsMask) noexcept;

  //! Dispatch OnCompact to all registered layers.
  Standard_EXPORT void DispatchOnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept;

  //! Clear all registered layer payloads without unregistering them.
  Standard_EXPORT void ClearAll() noexcept;

  //! Invalidate all registered layer payloads.
  Standard_EXPORT void InvalidateAll() noexcept;

private:
  Standard_EXPORT void recomputeSubscribedKindsMask();

private:
  NCollection_Vector<occ::handle<BRepGraph_Layer>> myLayers;
  NCollection_DataMap<Standard_GUID, int>          myGuidToSlot;
  int                                              mySubscribedKindsMask = 0;
};

#endif // _BRepGraph_LayerRegistry_HeaderFile
