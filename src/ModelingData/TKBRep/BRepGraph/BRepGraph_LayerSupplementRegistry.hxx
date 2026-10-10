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

#ifndef _BRepGraph_LayerSupplementRegistry_HeaderFile
#define _BRepGraph_LayerSupplementRegistry_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <BRepGraph_LayerSupplementBase.hxx>
#include <BRepGraphSupInc_ItemUID.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_GUID.hxx>

#include <cstdint>
#include <functional>
#include <shared_mutex>

struct BRepGraph_Data;
class BRepGraphSupInc_CopyContext;

//! @brief Dense GUID-keyed registry for supplemental-aware graph layers.
//!
//! This registry is deliberately independent from BRepGraph_LayerRegistry so
//! that its layers are copied only after supplemental stores have migrated.
class BRepGraph_LayerSupplementRegistry
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepGraph_LayerSupplementRegistry();

  BRepGraph_LayerSupplementRegistry(const BRepGraph_LayerSupplementRegistry&)            = delete;
  BRepGraph_LayerSupplementRegistry& operator=(const BRepGraph_LayerSupplementRegistry&) = delete;

  Standard_EXPORT BRepGraph_LayerSupplementRegistry(
    BRepGraph_LayerSupplementRegistry&& theOther) noexcept;
  Standard_EXPORT BRepGraph_LayerSupplementRegistry& operator=(
    BRepGraph_LayerSupplementRegistry&& theOther) noexcept;

  //! Register a supplemental layer, replacing an existing layer with the same GUID.
  //! @return slot index in the internal dense vector
  Standard_EXPORT uint32_t
    RegisterLayer(const occ::handle<BRepGraph_LayerSupplementBase>& theLayer);

  //! Remove a supplemental layer by GUID.
  Standard_EXPORT void UnregisterLayer(const Standard_GUID& theGUID);

  //! Find a supplemental layer by GUID, or return a null handle.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_LayerSupplementBase> FindLayer(
    const Standard_GUID& theGUID) const;

  //! Typed supplemental layer lookup by fixed layer GUID.
  template <typename T>
  [[nodiscard]] occ::handle<T> FindLayer() const
  {
    return Find<T>();
  }

  //! Typed supplemental layer lookup by fixed layer GUID.
  template <typename T>
  [[nodiscard]] occ::handle<T> Find() const
  {
    return occ::down_cast<T>(FindLayer(T::GetID()));
  }

  //! Return an existing supplemental layer or create and register a default one.
  template <typename T>
  [[nodiscard]] occ::handle<T> Ensure()
  {
    return occ::down_cast<T>(
      ensureLayer(T::GetID(),
                  []() -> occ::handle<BRepGraph_LayerSupplementBase> { return new T(); }));
  }

  //! Return the current dense slot for a layer GUID.
  [[nodiscard]] Standard_EXPORT bool FindSlot(const Standard_GUID& theGUID,
                                              uint32_t&            theSlot) const;

  //! Return a layer by dense slot, or a null handle when the slot is invalid.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_LayerSupplementBase> Layer(
    const uint32_t theSlot) const;

  //! Number of registered supplemental layers.
  [[nodiscard]] uint32_t NbLayers() const
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    return static_cast<uint32_t>(myLayers.Size());
  }

  //! Dispatch generic core item removal to all supplemental layers.
  Standard_EXPORT void DispatchOnItemRemoved(const BRepGraph_ItemId theItem) noexcept;

  //! Dispatch a node replacement to all supplemental layers.
  Standard_EXPORT void DispatchOnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                              const BRepGraph_NodeId theNewNode) noexcept;

  //! Dispatch supplemental store item removal to all supplemental layers.
  Standard_EXPORT void DispatchOnSupplementRemoved(const BRepGraphSupInc_ItemUID& theUID) noexcept;

  //! Clear all supplemental layer data while retaining their registrations.
  Standard_EXPORT void ClearAll() noexcept;

  //! Copy supplemental layer data after supplemental store migration.
  //! The source registrations remain unchanged. Copy callbacks run from a stable
  //! handle list after the registry lock is released.
  Standard_EXPORT void CopyLayersTo(
    BRepGraph&                                                         theTargetGraph,
    const NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>& theItemRemap,
    const BRepGraph_CopyRemap::Mode                                    theMode,
    const BRepGraphSupInc_CopyContext&                                 theSupplementCopy) const;

  //! Copy supplemental layer data using identity core item mapping.
  Standard_EXPORT void CopyLayersTo(BRepGraph&                             theTargetGraph,
                                    const BRepGraph_CopyRemap::MappingKind theMappingKind,
                                    const BRepGraph_CopyRemap::Mode        theMode,
                                    const BRepGraphSupInc_CopyContext&     theSupplementCopy) const;

private:
  friend class ::BRepGraph;
  friend struct ::BRepGraph_Data;

  //! Bind this registry to its graph owner for copy remap construction.
  Standard_EXPORT void Attach(BRepGraph* theGraph) noexcept;

  //! Remove this registry's graph owner binding.
  Standard_EXPORT void Detach() noexcept;

  //! Update graph owner after wrapper relocation without lifecycle callbacks.
  Standard_EXPORT void Relocate(BRepGraph* theGraph) noexcept;

  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_LayerSupplementBase> findLayerLocked(
    const Standard_GUID& theGUID) const;
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_LayerSupplementBase> ensureLayer(
    const Standard_GUID&                                               theGUID,
    const std::function<occ::handle<BRepGraph_LayerSupplementBase>()>& theFactory);
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_LayerSupplementBase> layerAt(
    const uint32_t theSlot) const;
  Standard_EXPORT uint32_t
       registerLayerLocked(const occ::handle<BRepGraph_LayerSupplementBase>& theLayer);
  void clearLayersLocked() noexcept;

private:
  NCollection_LinearVector<occ::handle<BRepGraph_LayerSupplementBase>> myLayers;
  NCollection_FlatDataMap<Standard_GUID, uint32_t>                     myGuidToSlot;
  BRepGraph*                                                           myGraph = nullptr;
  mutable std::shared_mutex                                            myMutex;
};

#endif // _BRepGraph_LayerSupplementRegistry_HeaderFile
