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

#ifndef _BRepGraphSupInc_GeometryStore_HeaderFile
#define _BRepGraphSupInc_GeometryStore_HeaderFile

#include <BRepGraphSupInc_GeometryDefinition.hxx>
#include <BRepGraphSupInc_Store.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_LinearVector.hxx>

#include <type_traits>

//! Dense standalone store for reusable curves and surfaces.
//! Dense geometry IDs are transient positions that may change after compaction; ItemUID identifies
//! active definitions during the process.
class BRepGraphSupInc_GeometryStore : public BRepGraphSupInc_Store
{
public:
  DEFINE_STANDARD_RTTIEXT(BRepGraphSupInc_GeometryStore, BRepGraphSupInc_Store)

  //! Geometry definition kind used in runtime UIDs.
  enum class Kind : uint32_t
  {
    //! Reusable 3D curve definition.
    Curve3d = 1,
    //! Reusable 2D curve definition.
    Curve2d = 2,
    //! Reusable surface definition.
    Surface = 3
  };

  //! Return the stable GUID of this store type.
  //! @return store-type GUID used for registry lookup
  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();

  //! Return this store's stable GUID.
  //! @return store-type GUID
  [[nodiscard]] const Standard_GUID& ID() const override { return GetID(); }

  //! Return this store's display name.
  //! @return display name
  [[nodiscard]] Standard_EXPORT const TCollection_AsciiString& Name() const override;
  //! Find an active geometry definition by runtime UID.
  //! @param[in] theUID runtime item identifier to query
  //! @return matching dense definition ID, or an invalid ID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_DefinitionId
    FindDefinitionByUID(const BRepGraphSupInc_ItemUID& theUID) const override;
  //! Test whether no allocated geometry records remain.
  //! @return true when all geometry vectors are empty
  [[nodiscard]] Standard_EXPORT bool IsEmpty() const override;

protected:
  //! Soft-remove an active geometry definition by runtime UID.
  //! @param[in] theUID runtime item identifier to remove
  //! @return true when an active definition was removed
  Standard_EXPORT bool Remove(const BRepGraphSupInc_ItemUID& theUID) override;
  //! Clear all geometry records while retaining runtime UID counter allocation history.
  Standard_EXPORT void Clear() noexcept override;
  //! Discard removed records while preserving active runtime UIDs and rebuilding dense IDs.
  Standard_EXPORT void Compact() override;

  [[nodiscard]] bool NeedsCompaction() const override
  {
    return myCurves3d.Size() != myActive3d || myCurves2d.Size() != myActive2d
           || mySurfaces.Size() != myActiveSurface;
  }

  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraphSupInc_Store> CloneForCompaction(
    const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap) const override;
  //! Test whether a target store has the same type and schema.
  //! @param[in] theTarget target store to validate
  //! @return true when the target is compatible
  [[nodiscard]] Standard_EXPORT bool IsCompatible(
    const BRepGraphSupInc_Store& theTarget) const override;
  //! Create an empty store of this type and schema.
  //! @return new empty geometry store
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraphSupInc_Store> NewEmpty() const override;
  //! Copy active definitions to the context target and map their runtime UIDs.
  //! @param[in,out] theContext source-to-target copy context
  //! @return true when every active definition is copied and mapped
  [[nodiscard]] Standard_EXPORT bool CopyTo(BRepGraphSupInc_CopyContext& theContext) const override;

public:
  //! Add a non-null 3D curve definition.
  //! @param[in] theCurve reusable curve
  //! @return typed dense identifier of the existing or added definition
  Standard_EXPORT BRepGraphSupInc_Curve3dId Add(const occ::handle<Geom_Curve>& theCurve);

  //! Add a non-null 2D curve definition.
  //! @param[in] theCurve reusable parametric curve
  //! @return typed dense identifier of the existing or added definition
  Standard_EXPORT BRepGraphSupInc_Curve2dId Add(const occ::handle<Geom2d_Curve>& theCurve);

  //! Add a non-null surface definition.
  //! @param[in] theSurface reusable surface
  //! @return typed dense identifier of the existing or added definition
  Standard_EXPORT BRepGraphSupInc_SurfaceId Add(const occ::handle<Geom_Surface>& theSurface);

  //! Find an active 3D curve definition by dense ID.
  //! @param[in] theID transient dense 3D curve ID
  //! @return active definition or null when the ID is invalid or removed
  [[nodiscard]] Standard_EXPORT const BRepGraphSupInc::Curve3dDef* Find(
    const BRepGraphSupInc_Curve3dId theID) const;
  //! Find an active 2D curve definition by dense ID.
  //! @param[in] theID transient dense 2D curve ID
  //! @return active definition or null when the ID is invalid or removed
  [[nodiscard]] Standard_EXPORT const BRepGraphSupInc::Curve2dDef* Find(
    const BRepGraphSupInc_Curve2dId theID) const;
  //! Find an active surface definition by dense ID.
  //! @param[in] theID transient dense surface ID
  //! @return active definition or null when the ID is invalid or removed
  [[nodiscard]] Standard_EXPORT const BRepGraphSupInc::SurfaceDef* Find(
    const BRepGraphSupInc_SurfaceId theID) const;

  //! Find an active 3D curve by handle.
  //! @param[in] theCurve curve to find
  //! @return matching dense ID or an invalid ID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_Curve3dId
    Find(const occ::handle<Geom_Curve>& theCurve) const;
  //! Find an active 2D curve by handle.
  //! @param[in] theCurve curve to find
  //! @return matching dense ID or an invalid ID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_Curve2dId
    Find(const occ::handle<Geom2d_Curve>& theCurve) const;
  //! Find an active surface by handle.
  //! @param[in] theSurface surface to find
  //! @return matching dense ID or an invalid ID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_SurfaceId
    Find(const occ::handle<Geom_Surface>& theSurface) const;

  //! Find an active geometry definition by runtime UID with the requested dense ID type.
  //! Supported ID types are BRepGraphSupInc_Curve3dId, BRepGraphSupInc_Curve2dId, and
  //! BRepGraphSupInc_SurfaceId. A UID from another store or kind, an inactive UID, and a missing
  //! UID return an invalid ID.
  //! @param[in] theUID runtime item identifier to query
  //! @return matching dense ID of type IdT or an invalid ID
  template <typename IdT>
  [[nodiscard]] IdT FindByUID(const BRepGraphSupInc_ItemUID& theUID) const
  {
    if constexpr (std::is_same_v<IdT, BRepGraphSupInc_Curve3dId>)
    {
      return findCurve3dByUID(theUID);
    }
    else if constexpr (std::is_same_v<IdT, BRepGraphSupInc_Curve2dId>)
    {
      return findCurve2dByUID(theUID);
    }
    else if constexpr (std::is_same_v<IdT, BRepGraphSupInc_SurfaceId>)
    {
      return findSurfaceByUID(theUID);
    }
    else
    {
      static_assert(std::is_same_v<IdT, BRepGraphSupInc_Curve3dId>
                      || std::is_same_v<IdT, BRepGraphSupInc_Curve2dId>
                      || std::is_same_v<IdT, BRepGraphSupInc_SurfaceId>,
                    "BRepGraphSupInc_GeometryStore::FindByUID supports only geometry IDs");
      return IdT();
    }
  }

  //! Return the runtime UID of an active 3D curve definition.
  //! @param[in] theID transient dense 3D curve ID
  //! @return runtime UID or an invalid UID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_ItemUID
    ItemUID(const BRepGraphSupInc_Curve3dId theID) const;
  //! Return the runtime UID of an active 2D curve definition.
  //! @param[in] theID transient dense 2D curve ID
  //! @return runtime UID or an invalid UID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_ItemUID
    ItemUID(const BRepGraphSupInc_Curve2dId theID) const;
  //! Return the runtime UID of an active surface definition.
  //! @param[in] theID transient dense surface ID
  //! @return runtime UID or an invalid UID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_ItemUID
    ItemUID(const BRepGraphSupInc_SurfaceId theID) const;

  //! Replace one active 3D curve and advance its own generation.
  //! @param[in] theID transient dense 3D curve ID
  //! @param[in] theCurve non-null replacement curve
  //! @return false when the ID is inactive or the curve belongs to another definition
  Standard_EXPORT bool Change(const BRepGraphSupInc_Curve3dId theID,
                              const occ::handle<Geom_Curve>&  theCurve);
  //! Replace one active 2D curve and advance its own generation.
  //! @param[in] theID transient dense 2D curve ID
  //! @param[in] theCurve non-null replacement curve
  //! @return false when the ID is inactive or the curve belongs to another definition
  Standard_EXPORT bool Change(const BRepGraphSupInc_Curve2dId  theID,
                              const occ::handle<Geom2d_Curve>& theCurve);
  //! Replace one active surface and advance its own generation.
  //! @param[in] theID transient dense surface ID
  //! @param[in] theSurface non-null replacement surface
  //! @return false when the ID is inactive or the surface belongs to another definition
  Standard_EXPORT bool Change(const BRepGraphSupInc_SurfaceId  theID,
                              const occ::handle<Geom_Surface>& theSurface);

  //! Return the number of active definitions of the requested kind.
  //! @param[in] theKind geometry kind to count
  //! @return active definition count
  [[nodiscard]] Standard_EXPORT uint32_t ActiveCount(const Kind theKind) const;
  //! Return the number of allocated definitions of the requested kind, including removed records.
  //! @param[in] theKind geometry kind to count
  //! @return allocated definition count
  [[nodiscard]] Standard_EXPORT uint32_t Count(const Kind theKind) const;

  //! Find the first active 3D curve definition at or after a dense index.
  //! @param[in] theStartIndex first dense index to inspect
  //! @return matching active ID, or an invalid ID when no definition remains
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_Curve3dId
    NextCurve3d(const uint32_t theStartIndex) const;

  //! Find the first active 2D curve definition at or after a dense index.
  //! @param[in] theStartIndex first dense index to inspect
  //! @return matching active ID, or an invalid ID when no definition remains
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_Curve2dId
    NextCurve2d(const uint32_t theStartIndex) const;

  //! Find the first active surface definition at or after a dense index.
  //! @param[in] theStartIndex first dense index to inspect
  //! @return matching active ID, or an invalid ID when no definition remains
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_SurfaceId
    NextSurface(const uint32_t theStartIndex) const;

private:
  friend class BRepGraphSupInc_Storage;

  //! Soft-remove an active 3D curve definition.
  //! @param[in] theID transient dense 3D curve ID
  //! @return true when an active definition was removed
  Standard_EXPORT bool Remove(const BRepGraphSupInc_Curve3dId theID);
  //! Soft-remove an active 2D curve definition.
  //! @param[in] theID transient dense 2D curve ID
  //! @return true when an active definition was removed
  Standard_EXPORT bool Remove(const BRepGraphSupInc_Curve2dId theID);
  //! Soft-remove an active surface definition.
  //! @param[in] theID transient dense surface ID
  //! @return true when an active definition was removed
  Standard_EXPORT bool Remove(const BRepGraphSupInc_SurfaceId theID);

  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_Curve3dId findCurve3dByUID(
    const BRepGraphSupInc_ItemUID& theUID) const;
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_Curve2dId findCurve2dByUID(
    const BRepGraphSupInc_ItemUID& theUID) const;
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_SurfaceId findSurfaceByUID(
    const BRepGraphSupInc_ItemUID& theUID) const;

  [[nodiscard]] uint32_t allocateCounter(const Kind theKind);
  void bindCurve3d(const occ::handle<Geom_Curve>& theCurve, const BRepGraphSupInc_Curve3dId theID);
  void bindCurve2d(const occ::handle<Geom2d_Curve>& theCurve,
                   const BRepGraphSupInc_Curve2dId  theID);
  void bindSurface(const occ::handle<Geom_Surface>& theSurface,
                   const BRepGraphSupInc_SurfaceId  theID);
  void unbindCurve3d(const occ::handle<Geom_Curve>&  theCurve,
                     const BRepGraphSupInc_Curve3dId theID);
  void unbindCurve2d(const occ::handle<Geom2d_Curve>& theCurve,
                     const BRepGraphSupInc_Curve2dId  theID);
  void unbindSurface(const occ::handle<Geom_Surface>& theSurface,
                     const BRepGraphSupInc_SurfaceId  theID);
  void rebuildIndices();
  [[nodiscard]] BRepGraphSupInc_Curve3dId append(const occ::handle<Geom_Curve>& theCurve,
                                                 const uint32_t                 theOwnGen,
                                                 const uint32_t                 theUID = 0);
  [[nodiscard]] BRepGraphSupInc_Curve2dId append(const occ::handle<Geom2d_Curve>& theCurve,
                                                 const uint32_t                   theOwnGen,
                                                 const uint32_t                   theUID = 0);
  [[nodiscard]] BRepGraphSupInc_SurfaceId append(const occ::handle<Geom_Surface>& theSurface,
                                                 const uint32_t                   theOwnGen,
                                                 const uint32_t                   theUID = 0);

  NCollection_LinearVector<BRepGraphSupInc::Curve3dDef> myCurves3d;
  NCollection_LinearVector<BRepGraphSupInc::Curve2dDef> myCurves2d;
  NCollection_LinearVector<BRepGraphSupInc::SurfaceDef> mySurfaces;
  NCollection_DataMap<const Geom_Curve*, NCollection_LinearVector<BRepGraphSupInc_Curve3dId>>
    myCurve3dToIDs;
  NCollection_DataMap<const Geom2d_Curve*, NCollection_LinearVector<BRepGraphSupInc_Curve2dId>>
    myCurve2dToIDs;
  NCollection_DataMap<const Geom_Surface*, NCollection_LinearVector<BRepGraphSupInc_SurfaceId>>
                                                               mySurfaceToIDs;
  NCollection_FlatDataMap<uint32_t, BRepGraphSupInc_Curve3dId> myUIDToCurve3d;
  NCollection_FlatDataMap<uint32_t, BRepGraphSupInc_Curve2dId> myUIDToCurve2d;
  NCollection_FlatDataMap<uint32_t, BRepGraphSupInc_SurfaceId> myUIDToSurface;
  uint32_t                                                     myActive3d      = 0;
  uint32_t                                                     myActive2d      = 0;
  uint32_t                                                     myActiveSurface = 0;
  uint32_t                                                     myNext3d        = 1;
  uint32_t                                                     myNext2d        = 1;
  uint32_t                                                     myNextSurface   = 1;
};

#endif // _BRepGraphSupInc_GeometryStore_HeaderFile
