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

#include <BRepGraphSupInc_GeometryStore.hxx>

#include <BRepGraphSupInc_CopyContext.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>

#include <limits>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraphSupInc_GeometryStore, BRepGraphSupInc_Store)

namespace
{
template <typename DefT>
void compactDefinitions(NCollection_LinearVector<DefT>& theDefinitions)
{
  NCollection_LinearVector<DefT> aCompactDefinitions;
  aCompactDefinitions.Reserve(theDefinitions.Size());
  for (const DefT& aDefinition : theDefinitions)
  {
    if (!aDefinition.IsRemoved)
    {
      aCompactDefinitions.Append(aDefinition);
    }
  }
  theDefinitions = std::move(aCompactDefinitions);
}

template <typename IdT>
void removeID(NCollection_LinearVector<IdT>& theIDs, const IdT theID)
{
  for (size_t anIndex = 0; anIndex < theIDs.Size(); ++anIndex)
  {
    if (theIDs.Value(anIndex) == theID)
    {
      theIDs.Erase(anIndex);
      return;
    }
  }
}
} // namespace

//=================================================================================================

const Standard_GUID& BRepGraphSupInc_GeometryStore::GetID()
{
  static const Standard_GUID THE_ID("8a9fa462-10fc-431f-9a81-8782b48f805b");
  return THE_ID;
}

//=================================================================================================

const TCollection_AsciiString& BRepGraphSupInc_GeometryStore::Name() const
{
  static const TCollection_AsciiString THE_NAME("BRepGraphSupInc_GeometryStore");
  return THE_NAME;
}

//=================================================================================================

BRepGraphSupInc_DefinitionId BRepGraphSupInc_GeometryStore::FindDefinitionByUID(
  const BRepGraphSupInc_ItemUID& theUID) const
{
  switch (static_cast<Kind>(theUID.Kind))
  {
    case Kind::Curve3d: {
      const BRepGraphSupInc_Curve3dId anID = findCurve3dByUID(theUID);
      return anID.IsValid()
               ? BRepGraphSupInc_DefinitionId{static_cast<uint32_t>(Kind::Curve3d), anID.Index}
               : BRepGraphSupInc_DefinitionId();
    }
    case Kind::Curve2d: {
      const BRepGraphSupInc_Curve2dId anID = findCurve2dByUID(theUID);
      return anID.IsValid()
               ? BRepGraphSupInc_DefinitionId{static_cast<uint32_t>(Kind::Curve2d), anID.Index}
               : BRepGraphSupInc_DefinitionId();
    }
    case Kind::Surface: {
      const BRepGraphSupInc_SurfaceId anID = findSurfaceByUID(theUID);
      return anID.IsValid()
               ? BRepGraphSupInc_DefinitionId{static_cast<uint32_t>(Kind::Surface), anID.Index}
               : BRepGraphSupInc_DefinitionId();
    }
  }
  return BRepGraphSupInc_DefinitionId();
}

//=================================================================================================

bool BRepGraphSupInc_GeometryStore::Remove(const BRepGraphSupInc_ItemUID& theUID)
{
  if (theUID.StoreId != StoreId() || theUID.Counter == 0)
  {
    return false;
  }
  switch (static_cast<Kind>(theUID.Kind))
  {
    case Kind::Curve3d: {
      const BRepGraphSupInc_Curve3dId* anID = myUIDToCurve3d.Seek(theUID.Counter);
      return anID != nullptr && Remove(*anID);
    }
    case Kind::Curve2d: {
      const BRepGraphSupInc_Curve2dId* anID = myUIDToCurve2d.Seek(theUID.Counter);
      return anID != nullptr && Remove(*anID);
    }
    case Kind::Surface: {
      const BRepGraphSupInc_SurfaceId* anID = myUIDToSurface.Seek(theUID.Counter);
      return anID != nullptr && Remove(*anID);
    }
  }
  return false;
}

//=================================================================================================

bool BRepGraphSupInc_GeometryStore::IsEmpty() const
{
  return myCurves3d.IsEmpty() && myCurves2d.IsEmpty() && mySurfaces.IsEmpty();
}

//=================================================================================================

void BRepGraphSupInc_GeometryStore::Clear() noexcept
{
  myCurves3d.Clear();
  myCurves2d.Clear();
  mySurfaces.Clear();
  myCurve3dToIDs.Clear();
  myCurve2dToIDs.Clear();
  mySurfaceToIDs.Clear();
  myUIDToCurve3d.Clear();
  myUIDToCurve2d.Clear();
  myUIDToSurface.Clear();
  myActive3d      = 0;
  myActive2d      = 0;
  myActiveSurface = 0;
}

//=================================================================================================

void BRepGraphSupInc_GeometryStore::Compact()
{
  compactDefinitions(myCurves3d);
  compactDefinitions(myCurves2d);
  compactDefinitions(mySurfaces);
  rebuildIndices();
}

//=================================================================================================

occ::handle<BRepGraphSupInc_Store> BRepGraphSupInc_GeometryStore::CloneForCompaction(
  const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>&) const
{
  occ::handle<BRepGraphSupInc_GeometryStore> aResult = new BRepGraphSupInc_GeometryStore();
  for (const BRepGraphSupInc::Curve3dDef& aDefinition : myCurves3d)
  {
    if (!aDefinition.IsRemoved)
    {
      (void)aResult->append(aDefinition.Curve, aDefinition.OwnGen, aDefinition.UID);
    }
  }
  for (const BRepGraphSupInc::Curve2dDef& aDefinition : myCurves2d)
  {
    if (!aDefinition.IsRemoved)
    {
      (void)aResult->append(aDefinition.Curve, aDefinition.OwnGen, aDefinition.UID);
    }
  }
  for (const BRepGraphSupInc::SurfaceDef& aDefinition : mySurfaces)
  {
    if (!aDefinition.IsRemoved)
    {
      (void)aResult->append(aDefinition.Surface, aDefinition.OwnGen, aDefinition.UID);
    }
  }
  aResult->myNext3d      = myNext3d;
  aResult->myNext2d      = myNext2d;
  aResult->myNextSurface = myNextSurface;
  return aResult;
}

//=================================================================================================

bool BRepGraphSupInc_GeometryStore::IsCompatible(const BRepGraphSupInc_Store& theTarget) const
{
  return theTarget.ID() == ID();
}

//=================================================================================================

occ::handle<BRepGraphSupInc_Store> BRepGraphSupInc_GeometryStore::NewEmpty() const
{
  return new BRepGraphSupInc_GeometryStore();
}

//=================================================================================================

bool BRepGraphSupInc_GeometryStore::CopyTo(BRepGraphSupInc_CopyContext& theContext) const
{
  const occ::handle<BRepGraphSupInc_Store> aTargetStore = theContext.TargetStore(ID());
  if (aTargetStore.IsNull() || !IsCompatible(*aTargetStore))
  {
    return false;
  }

  BRepGraphSupInc_GeometryStore& aTarget =
    static_cast<BRepGraphSupInc_GeometryStore&>(*aTargetStore);

  for (const BRepGraphSupInc::Curve3dDef& aDefinition : myCurves3d)
  {
    if (aDefinition.IsRemoved)
    {
      continue;
    }
    const BRepGraphSupInc_ItemUID aSourceUID =
      itemUID(static_cast<uint32_t>(Kind::Curve3d), aDefinition.UID);
    if (theContext.HasTargetUID(aSourceUID))
    {
      continue;
    }
    const BRepGraphSupInc_Curve3dId aTargetID =
      aTarget.append(aDefinition.Curve, aDefinition.OwnGen, 0);
    if (!theContext.AddMapping(aSourceUID, aTarget.ItemUID(aTargetID)))
    {
      return false;
    }
  }
  for (const BRepGraphSupInc::Curve2dDef& aDefinition : myCurves2d)
  {
    if (aDefinition.IsRemoved)
    {
      continue;
    }
    const BRepGraphSupInc_ItemUID aSourceUID =
      itemUID(static_cast<uint32_t>(Kind::Curve2d), aDefinition.UID);
    if (theContext.HasTargetUID(aSourceUID))
    {
      continue;
    }
    const BRepGraphSupInc_Curve2dId aTargetID =
      aTarget.append(aDefinition.Curve, aDefinition.OwnGen, 0);
    if (!theContext.AddMapping(aSourceUID, aTarget.ItemUID(aTargetID)))
    {
      return false;
    }
  }
  for (const BRepGraphSupInc::SurfaceDef& aDefinition : mySurfaces)
  {
    if (aDefinition.IsRemoved)
    {
      continue;
    }
    const BRepGraphSupInc_ItemUID aSourceUID =
      itemUID(static_cast<uint32_t>(Kind::Surface), aDefinition.UID);
    if (theContext.HasTargetUID(aSourceUID))
    {
      continue;
    }
    const BRepGraphSupInc_SurfaceId aTargetID =
      aTarget.append(aDefinition.Surface, aDefinition.OwnGen, 0);
    if (!theContext.AddMapping(aSourceUID, aTarget.ItemUID(aTargetID)))
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

BRepGraphSupInc_Curve3dId BRepGraphSupInc_GeometryStore::Add(
  const occ::handle<Geom_Curve>& theCurve)
{
  Standard_NullObject_Raise_if(theCurve.IsNull(),
                               "BRepGraphSupInc_GeometryStore::Add - null 3D curve");
  const BRepGraphSupInc_Curve3dId anExisting = Find(theCurve);
  return anExisting.IsValid() ? anExisting : append(theCurve, 0);
}

//=================================================================================================

BRepGraphSupInc_Curve2dId BRepGraphSupInc_GeometryStore::Add(
  const occ::handle<Geom2d_Curve>& theCurve)
{
  Standard_NullObject_Raise_if(theCurve.IsNull(),
                               "BRepGraphSupInc_GeometryStore::Add - null 2D curve");
  const BRepGraphSupInc_Curve2dId anExisting = Find(theCurve);
  return anExisting.IsValid() ? anExisting : append(theCurve, 0);
}

//=================================================================================================

BRepGraphSupInc_SurfaceId BRepGraphSupInc_GeometryStore::Add(
  const occ::handle<Geom_Surface>& theSurface)
{
  Standard_NullObject_Raise_if(theSurface.IsNull(),
                               "BRepGraphSupInc_GeometryStore::Add - null surface");
  const BRepGraphSupInc_SurfaceId anExisting = Find(theSurface);
  return anExisting.IsValid() ? anExisting : append(theSurface, 0);
}

//=================================================================================================

const BRepGraphSupInc::Curve3dDef* BRepGraphSupInc_GeometryStore::Find(
  const BRepGraphSupInc_Curve3dId theID) const
{
  if (!theID.IsValid() || static_cast<size_t>(theID.Index) >= myCurves3d.Size())
  {
    return nullptr;
  }
  const BRepGraphSupInc::Curve3dDef& aDefinition =
    myCurves3d.Value(static_cast<size_t>(theID.Index));
  return aDefinition.IsRemoved ? nullptr : &aDefinition;
}

//=================================================================================================

const BRepGraphSupInc::Curve2dDef* BRepGraphSupInc_GeometryStore::Find(
  const BRepGraphSupInc_Curve2dId theID) const
{
  if (!theID.IsValid() || static_cast<size_t>(theID.Index) >= myCurves2d.Size())
  {
    return nullptr;
  }
  const BRepGraphSupInc::Curve2dDef& aDefinition =
    myCurves2d.Value(static_cast<size_t>(theID.Index));
  return aDefinition.IsRemoved ? nullptr : &aDefinition;
}

//=================================================================================================

const BRepGraphSupInc::SurfaceDef* BRepGraphSupInc_GeometryStore::Find(
  const BRepGraphSupInc_SurfaceId theID) const
{
  if (!theID.IsValid() || static_cast<size_t>(theID.Index) >= mySurfaces.Size())
  {
    return nullptr;
  }
  const BRepGraphSupInc::SurfaceDef& aDefinition =
    mySurfaces.Value(static_cast<size_t>(theID.Index));
  return aDefinition.IsRemoved ? nullptr : &aDefinition;
}

//=================================================================================================

BRepGraphSupInc_Curve3dId BRepGraphSupInc_GeometryStore::Find(
  const occ::handle<Geom_Curve>& theCurve) const
{
  if (theCurve.IsNull())
  {
    return BRepGraphSupInc_Curve3dId();
  }
  const NCollection_LinearVector<BRepGraphSupInc_Curve3dId>* anIDs =
    myCurve3dToIDs.Seek(theCurve.get());
  return anIDs == nullptr || anIDs->IsEmpty() ? BRepGraphSupInc_Curve3dId() : anIDs->First();
}

//=================================================================================================

BRepGraphSupInc_Curve2dId BRepGraphSupInc_GeometryStore::Find(
  const occ::handle<Geom2d_Curve>& theCurve) const
{
  if (theCurve.IsNull())
  {
    return BRepGraphSupInc_Curve2dId();
  }
  const NCollection_LinearVector<BRepGraphSupInc_Curve2dId>* anIDs =
    myCurve2dToIDs.Seek(theCurve.get());
  return anIDs == nullptr || anIDs->IsEmpty() ? BRepGraphSupInc_Curve2dId() : anIDs->First();
}

//=================================================================================================

BRepGraphSupInc_SurfaceId BRepGraphSupInc_GeometryStore::Find(
  const occ::handle<Geom_Surface>& theSurface) const
{
  if (theSurface.IsNull())
  {
    return BRepGraphSupInc_SurfaceId();
  }
  const NCollection_LinearVector<BRepGraphSupInc_SurfaceId>* anIDs =
    mySurfaceToIDs.Seek(theSurface.get());
  return anIDs == nullptr || anIDs->IsEmpty() ? BRepGraphSupInc_SurfaceId() : anIDs->First();
}

//=================================================================================================

BRepGraphSupInc_Curve3dId BRepGraphSupInc_GeometryStore::findCurve3dByUID(
  const BRepGraphSupInc_ItemUID& theUID) const
{
  if (theUID.StoreId != StoreId() || theUID.Kind != static_cast<uint32_t>(Kind::Curve3d)
      || theUID.Counter == 0)
  {
    return BRepGraphSupInc_Curve3dId();
  }
  const BRepGraphSupInc_Curve3dId* anID = myUIDToCurve3d.Seek(theUID.Counter);
  return anID == nullptr ? BRepGraphSupInc_Curve3dId() : *anID;
}

//=================================================================================================

BRepGraphSupInc_Curve2dId BRepGraphSupInc_GeometryStore::findCurve2dByUID(
  const BRepGraphSupInc_ItemUID& theUID) const
{
  if (theUID.StoreId != StoreId() || theUID.Kind != static_cast<uint32_t>(Kind::Curve2d)
      || theUID.Counter == 0)
  {
    return BRepGraphSupInc_Curve2dId();
  }
  const BRepGraphSupInc_Curve2dId* anID = myUIDToCurve2d.Seek(theUID.Counter);
  return anID == nullptr ? BRepGraphSupInc_Curve2dId() : *anID;
}

//=================================================================================================

BRepGraphSupInc_SurfaceId BRepGraphSupInc_GeometryStore::findSurfaceByUID(
  const BRepGraphSupInc_ItemUID& theUID) const
{
  if (theUID.StoreId != StoreId() || theUID.Kind != static_cast<uint32_t>(Kind::Surface)
      || theUID.Counter == 0)
  {
    return BRepGraphSupInc_SurfaceId();
  }
  const BRepGraphSupInc_SurfaceId* anID = myUIDToSurface.Seek(theUID.Counter);
  return anID == nullptr ? BRepGraphSupInc_SurfaceId() : *anID;
}

//=================================================================================================

BRepGraphSupInc_ItemUID BRepGraphSupInc_GeometryStore::ItemUID(
  const BRepGraphSupInc_Curve3dId theID) const
{
  const BRepGraphSupInc::Curve3dDef* aDefinition = Find(theID);
  return aDefinition == nullptr ? BRepGraphSupInc_ItemUID()
                                : itemUID(static_cast<uint32_t>(Kind::Curve3d), aDefinition->UID);
}

//=================================================================================================

BRepGraphSupInc_ItemUID BRepGraphSupInc_GeometryStore::ItemUID(
  const BRepGraphSupInc_Curve2dId theID) const
{
  const BRepGraphSupInc::Curve2dDef* aDefinition = Find(theID);
  return aDefinition == nullptr ? BRepGraphSupInc_ItemUID()
                                : itemUID(static_cast<uint32_t>(Kind::Curve2d), aDefinition->UID);
}

//=================================================================================================

BRepGraphSupInc_ItemUID BRepGraphSupInc_GeometryStore::ItemUID(
  const BRepGraphSupInc_SurfaceId theID) const
{
  const BRepGraphSupInc::SurfaceDef* aDefinition = Find(theID);
  return aDefinition == nullptr ? BRepGraphSupInc_ItemUID()
                                : itemUID(static_cast<uint32_t>(Kind::Surface), aDefinition->UID);
}

//=================================================================================================

bool BRepGraphSupInc_GeometryStore::Change(const BRepGraphSupInc_Curve3dId theID,
                                           const occ::handle<Geom_Curve>&  theCurve)
{
  Standard_NullObject_Raise_if(theCurve.IsNull(),
                               "BRepGraphSupInc_GeometryStore::Change - null 3D curve");
  if (Find(theID) == nullptr)
  {
    return false;
  }
  const BRepGraphSupInc_Curve3dId anExisting = Find(theCurve);
  if (anExisting.IsValid() && anExisting != theID)
  {
    return false;
  }
  BRepGraphSupInc::Curve3dDef& aDefinition =
    myCurves3d.ChangeValue(static_cast<size_t>(theID.Index));
  if (aDefinition.Curve != theCurve)
  {
    unbindCurve3d(aDefinition.Curve, theID);
    aDefinition.Curve = theCurve;
    bindCurve3d(theCurve, theID);
  }
  ++aDefinition.OwnGen;
  return true;
}

//=================================================================================================

bool BRepGraphSupInc_GeometryStore::Change(const BRepGraphSupInc_Curve2dId  theID,
                                           const occ::handle<Geom2d_Curve>& theCurve)
{
  Standard_NullObject_Raise_if(theCurve.IsNull(),
                               "BRepGraphSupInc_GeometryStore::Change - null 2D curve");
  if (Find(theID) == nullptr)
  {
    return false;
  }
  const BRepGraphSupInc_Curve2dId anExisting = Find(theCurve);
  if (anExisting.IsValid() && anExisting != theID)
  {
    return false;
  }
  BRepGraphSupInc::Curve2dDef& aDefinition =
    myCurves2d.ChangeValue(static_cast<size_t>(theID.Index));
  if (aDefinition.Curve != theCurve)
  {
    unbindCurve2d(aDefinition.Curve, theID);
    aDefinition.Curve = theCurve;
    bindCurve2d(theCurve, theID);
  }
  ++aDefinition.OwnGen;
  return true;
}

//=================================================================================================

bool BRepGraphSupInc_GeometryStore::Change(const BRepGraphSupInc_SurfaceId  theID,
                                           const occ::handle<Geom_Surface>& theSurface)
{
  Standard_NullObject_Raise_if(theSurface.IsNull(),
                               "BRepGraphSupInc_GeometryStore::Change - null surface");
  if (Find(theID) == nullptr)
  {
    return false;
  }
  const BRepGraphSupInc_SurfaceId anExisting = Find(theSurface);
  if (anExisting.IsValid() && anExisting != theID)
  {
    return false;
  }
  BRepGraphSupInc::SurfaceDef& aDefinition =
    mySurfaces.ChangeValue(static_cast<size_t>(theID.Index));
  if (aDefinition.Surface != theSurface)
  {
    unbindSurface(aDefinition.Surface, theID);
    aDefinition.Surface = theSurface;
    bindSurface(theSurface, theID);
  }
  ++aDefinition.OwnGen;
  return true;
}

//=================================================================================================

bool BRepGraphSupInc_GeometryStore::Remove(const BRepGraphSupInc_Curve3dId theID)
{
  const BRepGraphSupInc::Curve3dDef* aCurrent = Find(theID);
  if (aCurrent == nullptr)
  {
    return false;
  }
  BRepGraphSupInc::Curve3dDef& aDefinition =
    myCurves3d.ChangeValue(static_cast<size_t>(theID.Index));
  unbindCurve3d(aDefinition.Curve, theID);
  myUIDToCurve3d.UnBind(aDefinition.UID);
  aDefinition.IsRemoved = true;
  --myActive3d;
  return true;
}

//=================================================================================================

bool BRepGraphSupInc_GeometryStore::Remove(const BRepGraphSupInc_Curve2dId theID)
{
  const BRepGraphSupInc::Curve2dDef* aCurrent = Find(theID);
  if (aCurrent == nullptr)
  {
    return false;
  }
  BRepGraphSupInc::Curve2dDef& aDefinition =
    myCurves2d.ChangeValue(static_cast<size_t>(theID.Index));
  unbindCurve2d(aDefinition.Curve, theID);
  myUIDToCurve2d.UnBind(aDefinition.UID);
  aDefinition.IsRemoved = true;
  --myActive2d;
  return true;
}

//=================================================================================================

bool BRepGraphSupInc_GeometryStore::Remove(const BRepGraphSupInc_SurfaceId theID)
{
  const BRepGraphSupInc::SurfaceDef* aCurrent = Find(theID);
  if (aCurrent == nullptr)
  {
    return false;
  }
  BRepGraphSupInc::SurfaceDef& aDefinition =
    mySurfaces.ChangeValue(static_cast<size_t>(theID.Index));
  unbindSurface(aDefinition.Surface, theID);
  myUIDToSurface.UnBind(aDefinition.UID);
  aDefinition.IsRemoved = true;
  --myActiveSurface;
  return true;
}

//=================================================================================================

uint32_t BRepGraphSupInc_GeometryStore::ActiveCount(const Kind theKind) const
{
  switch (theKind)
  {
    case Kind::Curve3d:
      return myActive3d;
    case Kind::Curve2d:
      return myActive2d;
    case Kind::Surface:
      return myActiveSurface;
  }
  return 0;
}

//=================================================================================================

uint32_t BRepGraphSupInc_GeometryStore::Count(const Kind theKind) const
{
  switch (theKind)
  {
    case Kind::Curve3d:
      return static_cast<uint32_t>(myCurves3d.Size());
    case Kind::Curve2d:
      return static_cast<uint32_t>(myCurves2d.Size());
    case Kind::Surface:
      return static_cast<uint32_t>(mySurfaces.Size());
  }
  return 0;
}

//=================================================================================================

BRepGraphSupInc_Curve3dId BRepGraphSupInc_GeometryStore::NextCurve3d(
  const uint32_t theStartIndex) const
{
  for (uint32_t anIndex = theStartIndex; anIndex < myCurves3d.Size(); ++anIndex)
  {
    const BRepGraphSupInc_Curve3dId anId{anIndex};
    if (Find(anId) != nullptr)
    {
      return anId;
    }
  }
  return {};
}

//=================================================================================================

BRepGraphSupInc_Curve2dId BRepGraphSupInc_GeometryStore::NextCurve2d(
  const uint32_t theStartIndex) const
{
  for (uint32_t anIndex = theStartIndex; anIndex < myCurves2d.Size(); ++anIndex)
  {
    const BRepGraphSupInc_Curve2dId anId{anIndex};
    if (Find(anId) != nullptr)
    {
      return anId;
    }
  }
  return {};
}

//=================================================================================================

BRepGraphSupInc_SurfaceId BRepGraphSupInc_GeometryStore::NextSurface(
  const uint32_t theStartIndex) const
{
  for (uint32_t anIndex = theStartIndex; anIndex < mySurfaces.Size(); ++anIndex)
  {
    const BRepGraphSupInc_SurfaceId anId{anIndex};
    if (Find(anId) != nullptr)
    {
      return anId;
    }
  }
  return {};
}

//=================================================================================================

uint32_t BRepGraphSupInc_GeometryStore::allocateCounter(const Kind theKind)
{
  uint32_t* aCounter = nullptr;
  switch (theKind)
  {
    case Kind::Curve3d:
      aCounter = &myNext3d;
      break;
    case Kind::Curve2d:
      aCounter = &myNext2d;
      break;
    case Kind::Surface:
      aCounter = &myNextSurface;
      break;
  }
  Standard_OutOfRange_Raise_if(aCounter == nullptr || *aCounter == 0,
                               "BRepGraphSupInc_GeometryStore - UID counter overflow");
  return (*aCounter)++;
}

//=================================================================================================

void BRepGraphSupInc_GeometryStore::bindCurve3d(const occ::handle<Geom_Curve>&  theCurve,
                                                const BRepGraphSupInc_Curve3dId theID)
{
  NCollection_LinearVector<BRepGraphSupInc_Curve3dId>* anIDs =
    myCurve3dToIDs.ChangeSeek(theCurve.get());
  if (anIDs == nullptr)
  {
    NCollection_LinearVector<BRepGraphSupInc_Curve3dId> aNewIDs;
    aNewIDs.Append(theID);
    myCurve3dToIDs.Bind(theCurve.get(), aNewIDs);
    return;
  }
  anIDs->Append(theID);
}

//=================================================================================================

void BRepGraphSupInc_GeometryStore::bindCurve2d(const occ::handle<Geom2d_Curve>& theCurve,
                                                const BRepGraphSupInc_Curve2dId  theID)
{
  NCollection_LinearVector<BRepGraphSupInc_Curve2dId>* anIDs =
    myCurve2dToIDs.ChangeSeek(theCurve.get());
  if (anIDs == nullptr)
  {
    NCollection_LinearVector<BRepGraphSupInc_Curve2dId> aNewIDs;
    aNewIDs.Append(theID);
    myCurve2dToIDs.Bind(theCurve.get(), aNewIDs);
    return;
  }
  anIDs->Append(theID);
}

//=================================================================================================

void BRepGraphSupInc_GeometryStore::bindSurface(const occ::handle<Geom_Surface>& theSurface,
                                                const BRepGraphSupInc_SurfaceId  theID)
{
  NCollection_LinearVector<BRepGraphSupInc_SurfaceId>* anIDs =
    mySurfaceToIDs.ChangeSeek(theSurface.get());
  if (anIDs == nullptr)
  {
    NCollection_LinearVector<BRepGraphSupInc_SurfaceId> aNewIDs;
    aNewIDs.Append(theID);
    mySurfaceToIDs.Bind(theSurface.get(), aNewIDs);
    return;
  }
  anIDs->Append(theID);
}

//=================================================================================================

void BRepGraphSupInc_GeometryStore::unbindCurve3d(const occ::handle<Geom_Curve>&  theCurve,
                                                  const BRepGraphSupInc_Curve3dId theID)
{
  NCollection_LinearVector<BRepGraphSupInc_Curve3dId>* anIDs =
    myCurve3dToIDs.ChangeSeek(theCurve.get());
  if (anIDs == nullptr)
  {
    return;
  }
  removeID(*anIDs, theID);
  if (anIDs->IsEmpty())
  {
    myCurve3dToIDs.UnBind(theCurve.get());
  }
}

//=================================================================================================

void BRepGraphSupInc_GeometryStore::unbindCurve2d(const occ::handle<Geom2d_Curve>& theCurve,
                                                  const BRepGraphSupInc_Curve2dId  theID)
{
  NCollection_LinearVector<BRepGraphSupInc_Curve2dId>* anIDs =
    myCurve2dToIDs.ChangeSeek(theCurve.get());
  if (anIDs == nullptr)
  {
    return;
  }
  removeID(*anIDs, theID);
  if (anIDs->IsEmpty())
  {
    myCurve2dToIDs.UnBind(theCurve.get());
  }
}

//=================================================================================================

void BRepGraphSupInc_GeometryStore::unbindSurface(const occ::handle<Geom_Surface>& theSurface,
                                                  const BRepGraphSupInc_SurfaceId  theID)
{
  NCollection_LinearVector<BRepGraphSupInc_SurfaceId>* anIDs =
    mySurfaceToIDs.ChangeSeek(theSurface.get());
  if (anIDs == nullptr)
  {
    return;
  }
  removeID(*anIDs, theID);
  if (anIDs->IsEmpty())
  {
    mySurfaceToIDs.UnBind(theSurface.get());
  }
}

//=================================================================================================

void BRepGraphSupInc_GeometryStore::rebuildIndices()
{
  myCurve3dToIDs.Clear();
  myCurve2dToIDs.Clear();
  mySurfaceToIDs.Clear();
  myUIDToCurve3d.Clear();
  myUIDToCurve2d.Clear();
  myUIDToSurface.Clear();
  for (uint32_t anIndex = 0; anIndex < Count(Kind::Curve3d); ++anIndex)
  {
    const BRepGraphSupInc::Curve3dDef& aDefinition = myCurves3d.Value(static_cast<size_t>(anIndex));
    if (!aDefinition.IsRemoved)
    {
      const BRepGraphSupInc_Curve3dId anID{anIndex};
      bindCurve3d(aDefinition.Curve, anID);
      myUIDToCurve3d.Bind(aDefinition.UID, anID);
    }
  }
  for (uint32_t anIndex = 0; anIndex < Count(Kind::Curve2d); ++anIndex)
  {
    const BRepGraphSupInc::Curve2dDef& aDefinition = myCurves2d.Value(static_cast<size_t>(anIndex));
    if (!aDefinition.IsRemoved)
    {
      const BRepGraphSupInc_Curve2dId anID{anIndex};
      bindCurve2d(aDefinition.Curve, anID);
      myUIDToCurve2d.Bind(aDefinition.UID, anID);
    }
  }
  for (uint32_t anIndex = 0; anIndex < Count(Kind::Surface); ++anIndex)
  {
    const BRepGraphSupInc::SurfaceDef& aDefinition = mySurfaces.Value(static_cast<size_t>(anIndex));
    if (!aDefinition.IsRemoved)
    {
      const BRepGraphSupInc_SurfaceId anID{anIndex};
      bindSurface(aDefinition.Surface, anID);
      myUIDToSurface.Bind(aDefinition.UID, anID);
    }
  }
}

//=================================================================================================

BRepGraphSupInc_Curve3dId BRepGraphSupInc_GeometryStore::append(
  const occ::handle<Geom_Curve>& theCurve,
  const uint32_t                 theOwnGen,
  const uint32_t                 theUID)
{
  Standard_NullObject_Raise_if(theCurve.IsNull(),
                               "BRepGraphSupInc_GeometryStore::append - null 3D curve");
  Standard_OutOfRange_Raise_if(myCurves3d.Size() > std::numeric_limits<uint32_t>::max(),
                               "BRepGraphSupInc_GeometryStore - too many 3D curves");
  const BRepGraphSupInc_Curve3dId anID{static_cast<uint32_t>(myCurves3d.Size())};
  BRepGraphSupInc::Curve3dDef     aDefinition;
  aDefinition.UID    = theUID == 0 ? allocateCounter(Kind::Curve3d) : theUID;
  aDefinition.OwnGen = theOwnGen;
  aDefinition.Curve  = theCurve;
  myCurves3d.Append(aDefinition);
  bindCurve3d(theCurve, anID);
  myUIDToCurve3d.Bind(aDefinition.UID, anID);
  ++myActive3d;
  return anID;
}

//=================================================================================================

BRepGraphSupInc_Curve2dId BRepGraphSupInc_GeometryStore::append(
  const occ::handle<Geom2d_Curve>& theCurve,
  const uint32_t                   theOwnGen,
  const uint32_t                   theUID)
{
  Standard_NullObject_Raise_if(theCurve.IsNull(),
                               "BRepGraphSupInc_GeometryStore::append - null 2D curve");
  Standard_OutOfRange_Raise_if(myCurves2d.Size() > std::numeric_limits<uint32_t>::max(),
                               "BRepGraphSupInc_GeometryStore - too many 2D curves");
  const BRepGraphSupInc_Curve2dId anID{static_cast<uint32_t>(myCurves2d.Size())};
  BRepGraphSupInc::Curve2dDef     aDefinition;
  aDefinition.UID    = theUID == 0 ? allocateCounter(Kind::Curve2d) : theUID;
  aDefinition.OwnGen = theOwnGen;
  aDefinition.Curve  = theCurve;
  myCurves2d.Append(aDefinition);
  bindCurve2d(theCurve, anID);
  myUIDToCurve2d.Bind(aDefinition.UID, anID);
  ++myActive2d;
  return anID;
}

//=================================================================================================

BRepGraphSupInc_SurfaceId BRepGraphSupInc_GeometryStore::append(
  const occ::handle<Geom_Surface>& theSurface,
  const uint32_t                   theOwnGen,
  const uint32_t                   theUID)
{
  Standard_NullObject_Raise_if(theSurface.IsNull(),
                               "BRepGraphSupInc_GeometryStore::append - null surface");
  Standard_OutOfRange_Raise_if(mySurfaces.Size() > std::numeric_limits<uint32_t>::max(),
                               "BRepGraphSupInc_GeometryStore - too many surfaces");
  const BRepGraphSupInc_SurfaceId anID{static_cast<uint32_t>(mySurfaces.Size())};
  BRepGraphSupInc::SurfaceDef     aDefinition;
  aDefinition.UID     = theUID == 0 ? allocateCounter(Kind::Surface) : theUID;
  aDefinition.OwnGen  = theOwnGen;
  aDefinition.Surface = theSurface;
  mySurfaces.Append(aDefinition);
  bindSurface(theSurface, anID);
  myUIDToSurface.Bind(aDefinition.UID, anID);
  ++myActiveSurface;
  return anID;
}
