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

#ifndef _BRepGraph_LayerSupplement_HeaderFile
#define _BRepGraph_LayerSupplement_HeaderFile

#include <BRepGraph_LayerSupplementBase.hxx>
#include <BRepGraphSupInc_Endpoint.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_LinearVector.hxx>

//! Sparse generic associations between core and supplemental graph items.
class BRepGraph_LayerSupplement : public BRepGraph_LayerSupplementBase
{
public:
  struct Reference
  {
    BRepGraphSupInc_Endpoint   Source;
    BRepGraphSupInc_Endpoint   Target;
    Standard_GUID              Role;
  };

  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;
  [[nodiscard]] Standard_EXPORT const TCollection_AsciiString& Name() const override;

  //! Add one validated directional association.
  //! @param[in] theSource source endpoint
  //! @param[in] theTarget target endpoint
  //! @param[in] theRole stable semantic relation role
  //! @return non-zero layer-local reference UID, or zero on rejection
  Standard_EXPORT uint32_t Add(const BRepGraphSupInc_Endpoint& theSource,
                               const BRepGraphSupInc_Endpoint& theTarget,
                               const Standard_GUID&            theRole);

  //! Find an association by its layer-local active-process counter.
  //! @param[in] theUID layer-local reference UID
  //! @return reference, or null when it is absent
  [[nodiscard]] Standard_EXPORT const Reference* Find(const uint32_t theUID) const;

  //! Return insertion-ordered association UIDs containing an endpoint.
  //! @param[in] theEndpoint endpoint to query
  //! @return ordered reference UIDs
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<uint32_t>& Related(
    const BRepGraphSupInc_Endpoint& theEndpoint) const;

  //! Remove one relation without changing either endpoint definition.
  //! @param[in] theUID layer-local reference UID
  //! @return true when a reference was removed
  Standard_EXPORT bool Remove(const uint32_t theUID);
  [[nodiscard]] uint32_t Count() const { return static_cast<uint32_t>(myReferences.Size()); }

  Standard_EXPORT void OnItemRemoved(const BRepGraph_ItemId theItem) noexcept override;
  Standard_EXPORT void OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                      const BRepGraph_NodeId theNewNode) noexcept override;
  Standard_EXPORT void OnSupplementRemoved(const BRepGraphSupInc_ItemUID& theUID) noexcept override;
  Standard_EXPORT void CopySupplementalTo(const BRepGraph_CopyRemap&          theCopy,
                                          const BRepGraphSupInc_CopyContext& theSupplementCopy) const override;
  Standard_EXPORT void InvalidateAll() noexcept override;
  Standard_EXPORT void Clear() noexcept override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerSupplement, BRepGraph_LayerSupplementBase)

private:
  [[nodiscard]] bool isValidEndpoint(const BRepGraphSupInc_Endpoint& theEndpoint) const;
  void addIndex(const BRepGraphSupInc_Endpoint& theEndpoint, const uint32_t theUID);
  void removeIndex(const BRepGraphSupInc_Endpoint& theEndpoint, const uint32_t theUID) noexcept;
  void removeByEndpoint(const BRepGraphSupInc_Endpoint& theEndpoint) noexcept;
  [[nodiscard]] static BRepGraphSupInc_Endpoint remapEndpoint(
    const BRepGraphSupInc_Endpoint& theEndpoint,
    const BRepGraph_CopyRemap&      theCopy,
    const BRepGraphSupInc_CopyContext& theSupplementCopy);

private:
  //! Each reference is identified by its key in this map.
  NCollection_FlatDataMap<uint32_t, Reference> myReferences;
  NCollection_DataMap<BRepGraphSupInc_Endpoint,
                      NCollection_LinearVector<uint32_t>>
    myEndpointToUIDs;
  NCollection_LinearVector<uint32_t> myEmptyUIDs;
  uint32_t                           myNextUID = 1;
};

#endif // _BRepGraph_LayerSupplement_HeaderFile
