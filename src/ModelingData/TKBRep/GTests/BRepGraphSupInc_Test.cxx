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

#include <gtest/gtest.h>

#include <BRepGraph.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_Copy.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_LayerSupplementRegistry.hxx>
#include <BRepGraph_LayerSupplement.hxx>
#include <BRepGraph_SupplementsView.hxx>
#include <BRepGraph_Revision.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphSupInc_GeometryStore.hxx>
#include <BRepGraphSupInc_CopyContext.hxx>
#include <BRepGraphSupInc_Storage.hxx>
#include <BRepGraphSupInc_TopologyStore.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Standard_NullObject.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <type_traits>

namespace
{

BRepGraphSupInc_Endpoint coreEndpoint(const BRepGraph_VertexId theVertex)
{
  return BRepGraphSupInc_Endpoint::Core(BRepGraph_ItemId(theVertex));
}

class BRepGraphSupInc_TestStore : public BRepGraphSupInc_Store
{
public:
  DEFINE_STANDARD_RTTI_INLINE(BRepGraphSupInc_TestStore, BRepGraphSupInc_Store)

  static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_ID("3bfc711c-b074-4cd7-bf9c-773f4fadce03");
    return THE_ID;
  }

  const Standard_GUID& ID() const override { return GetID(); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("BRepGraphSupInc_TestStore");
    return THE_NAME;
  }

  BRepGraphSupInc_DefinitionId FindDefinitionByUID(
    const BRepGraphSupInc_ItemUID& theUID) const override
  {
    if (theUID.StoreId != StoreId() || theUID.Kind != 1 || theUID.Counter == 0)
    {
      return BRepGraphSupInc_DefinitionId();
    }
    for (uint32_t anIndex = 0; anIndex < static_cast<uint32_t>(myValues.Size()); ++anIndex)
    {
      const Definition& aDefinition = myValues.Value(static_cast<size_t>(anIndex));
      if (aDefinition.Counter == theUID.Counter)
      {
        return BRepGraphSupInc_DefinitionId{1, anIndex};
      }
    }
    return BRepGraphSupInc_DefinitionId();
  }

  bool IsEmpty() const override { return myValues.IsEmpty(); }

  BRepGraphSupInc_ItemUID Add(const int theValue)
  {
    if (myNextCounter == 0)
    {
      return BRepGraphSupInc_ItemUID();
    }
    const uint32_t aCounter = myNextCounter++;
    myValues.Append({theValue, aCounter});
    return itemUID(1, aCounter);
  }

protected:
  bool Remove(const BRepGraphSupInc_ItemUID&) override { return false; }

  void Clear() noexcept override { myValues.Clear(); }

  void Compact() override {}

  bool IsCompatible(const BRepGraphSupInc_Store& theTarget) const override
  {
    return theTarget.ID() == ID();
  }

  occ::handle<BRepGraphSupInc_Store> NewEmpty() const override
  {
    return new BRepGraphSupInc_TestStore();
  }

  bool CopyTo(BRepGraphSupInc_CopyContext& theContext) const override
  {
    const occ::handle<BRepGraphSupInc_Store> aTargetStore = theContext.TargetStore(ID());
    if (aTargetStore.IsNull() || !IsCompatible(*aTargetStore))
    {
      return false;
    }
    BRepGraphSupInc_TestStore& aTarget = static_cast<BRepGraphSupInc_TestStore&>(*aTargetStore);
    for (const Definition& aDefinition : myValues)
    {
      const BRepGraphSupInc_ItemUID aSourceUID = itemUID(1, aDefinition.Counter);
      if (!theContext.HasTargetUID(aSourceUID)
          && !theContext.AddMapping(aSourceUID, aTarget.Add(aDefinition.Value)))
      {
        return false;
      }
    }
    return true;
  }

private:
  struct Definition
  {
    int      Value;
    uint32_t Counter;
  };

  NCollection_LinearVector<Definition> myValues;
  uint32_t                             myNextCounter = 1;
};

class BRepGraphSupInc_DependencyStore : public BRepGraphSupInc_Store
{
public:
  enum class Type
  {
    First,
    Second,
    Missing,
    CycleFirst,
    CycleSecond
  };

  DEFINE_STANDARD_RTTI_INLINE(BRepGraphSupInc_DependencyStore, BRepGraphSupInc_Store)

  explicit BRepGraphSupInc_DependencyStore(const Type theType)
      : myType(theType)
  {
    if (theType == Type::Second)
    {
      myDependencies.Append(GetID(Type::First));
    }
    else if (theType == Type::Missing)
    {
      myDependencies.Append(Standard_GUID("4b6dd7f5-4029-420a-9d00-7a08f88c6a91"));
    }
    else if (theType == Type::CycleFirst)
    {
      myDependencies.Append(GetID(Type::CycleSecond));
    }
    else if (theType == Type::CycleSecond)
    {
      myDependencies.Append(GetID(Type::CycleFirst));
    }
  }

  static const Standard_GUID& GetID(const Type theType)
  {
    static const Standard_GUID THE_FIRST("a4e604d1-55a4-4fec-a685-9375a64dc790");
    static const Standard_GUID THE_SECOND("a4e604d1-55a4-4fec-a685-9375a64dc791");
    static const Standard_GUID THE_MISSING("a4e604d1-55a4-4fec-a685-9375a64dc792");
    static const Standard_GUID THE_CYCLE_FIRST("a4e604d1-55a4-4fec-a685-9375a64dc793");
    static const Standard_GUID THE_CYCLE_SECOND("a4e604d1-55a4-4fec-a685-9375a64dc794");
    switch (theType)
    {
      case Type::First:
        return THE_FIRST;
      case Type::Second:
        return THE_SECOND;
      case Type::Missing:
        return THE_MISSING;
      case Type::CycleFirst:
        return THE_CYCLE_FIRST;
      case Type::CycleSecond:
        return THE_CYCLE_SECOND;
    }
    return THE_FIRST;
  }

  const Standard_GUID& ID() const override { return GetID(myType); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("BRepGraphSupInc_DependencyStore");
    return THE_NAME;
  }

  BRepGraphSupInc_DefinitionId FindDefinitionByUID(const BRepGraphSupInc_ItemUID&) const override
  {
    return BRepGraphSupInc_DefinitionId();
  }

  bool IsEmpty() const override { return true; }

  const NCollection_Sequence<Standard_GUID>& Dependencies() const override
  {
    return myDependencies;
  }

  bool WasCopied() const { return myWasCopied; }

  bool WasNodeRecordCopied() const { return myWasNodeRecordCopied; }

  void SetNodeOwner(const BRepGraph_NodeId theOwner) { myNodeOwner = theOwner; }

protected:
  bool Remove(const BRepGraphSupInc_ItemUID&) override { return false; }

  void Clear() noexcept override { myWasCopied = false; }

  void Compact() override {}

  bool IsCompatible(const BRepGraphSupInc_Store& theTarget) const override
  {
    return theTarget.ID() == ID();
  }

  occ::handle<BRepGraphSupInc_Store> NewEmpty() const override
  {
    return new BRepGraphSupInc_DependencyStore(myType);
  }

  bool CopyTo(BRepGraphSupInc_CopyContext& theContext) const override
  {
    const occ::handle<BRepGraphSupInc_DependencyStore> aTarget =
      occ::down_cast<BRepGraphSupInc_DependencyStore>(theContext.TargetStore(ID()));
    if (aTarget.IsNull())
    {
      return false;
    }
    for (NCollection_Sequence<Standard_GUID>::Iterator aDependencyIt(myDependencies);
         aDependencyIt.More();
         aDependencyIt.Next())
    {
      const occ::handle<BRepGraphSupInc_DependencyStore> aTargetDependency =
        occ::down_cast<BRepGraphSupInc_DependencyStore>(
          theContext.TargetStore(aDependencyIt.Value()));
      if (aTargetDependency.IsNull() || !aTargetDependency->WasCopied())
      {
        return false;
      }
    }
    aTarget->myNodeOwner           = theContext.TargetNode(myNodeOwner);
    aTarget->myWasNodeRecordCopied = myWasNodeRecordCopied;
    aTarget->myWasCopied           = true;
    return true;
  }

  bool HasRecordsForMappedNodes(const BRepGraphSupInc_CopyContext& theContext) const override
  {
    return myType == Type::Second && theContext.HasTargetNode(myNodeOwner);
  }

  void CopyRecordsForMappedNodes(BRepGraphSupInc_CopyContext& theContext) const override
  {
    const occ::handle<BRepGraphSupInc_DependencyStore> aTarget =
      occ::down_cast<BRepGraphSupInc_DependencyStore>(theContext.TargetStore(ID()));
    const occ::handle<BRepGraphSupInc_DependencyStore> aTargetDependency =
      occ::down_cast<BRepGraphSupInc_DependencyStore>(theContext.TargetStore(GetID(Type::First)));
    ASSERT_FALSE(aTarget.IsNull());
    ASSERT_FALSE(aTargetDependency.IsNull());
    ASSERT_NE(aTargetDependency->StoreId(), 0u);
    aTarget->myWasNodeRecordCopied = true;
  }

private:
  Type                                myType;
  BRepGraph_NodeId                    myNodeOwner;
  bool                                myWasCopied           = false;
  bool                                myWasNodeRecordCopied = false;
  NCollection_Sequence<Standard_GUID> myDependencies;
};

class BRepGraphSupInc_FailingNodeCopyStore : public BRepGraphSupInc_Store
{
public:
  DEFINE_STANDARD_RTTI_INLINE(BRepGraphSupInc_FailingNodeCopyStore, BRepGraphSupInc_Store)

  static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_ID("4977ad72-242b-416d-9db6-cd81605e923a");
    return THE_ID;
  }

  const Standard_GUID& ID() const override { return GetID(); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("BRepGraphSupInc_FailingNodeCopyStore");
    return THE_NAME;
  }

  BRepGraphSupInc_DefinitionId FindDefinitionByUID(const BRepGraphSupInc_ItemUID&) const override
  {
    return BRepGraphSupInc_DefinitionId();
  }

  bool IsEmpty() const override { return false; }

  uint32_t NewEmptyCalls() const { return myNewEmptyCalls; }

protected:
  bool Remove(const BRepGraphSupInc_ItemUID&) override { return false; }

  void Clear() noexcept override {}

  void Compact() override {}

  bool IsCompatible(const BRepGraphSupInc_Store& theTarget) const override
  {
    return theTarget.ID() == ID();
  }

  occ::handle<BRepGraphSupInc_Store> NewEmpty() const override
  {
    ++myNewEmptyCalls;
    return occ::handle<BRepGraphSupInc_Store>();
  }

  bool CopyTo(BRepGraphSupInc_CopyContext& theContext) const override
  {
    const occ::handle<BRepGraphSupInc_Store> aTargetStore = theContext.TargetStore(ID());
    return !aTargetStore.IsNull() && IsCompatible(*aTargetStore);
  }

private:
  mutable uint32_t myNewEmptyCalls = 0;
};

class BRepGraphSupInc_RejectingNodeCopyStore : public BRepGraphSupInc_Store
{
public:
  DEFINE_STANDARD_RTTI_INLINE(BRepGraphSupInc_RejectingNodeCopyStore, BRepGraphSupInc_Store)

  static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_ID("c25f70a2-1ccc-4693-9805-c7e79a1fd1e4");
    return THE_ID;
  }

  const Standard_GUID& ID() const override { return GetID(); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("BRepGraphSupInc_RejectingNodeCopyStore");
    return THE_NAME;
  }

  BRepGraphSupInc_DefinitionId FindDefinitionByUID(const BRepGraphSupInc_ItemUID&) const override
  {
    return BRepGraphSupInc_DefinitionId();
  }

  bool IsEmpty() const override { return false; }

  void SetOwner(const BRepGraph_NodeId theOwner) { myOwner = theOwner; }

  uint32_t PreflightCalls() const { return myPreflightCalls; }

protected:
  bool Remove(const BRepGraphSupInc_ItemUID&) override { return false; }

  void Clear() noexcept override {}

  void Compact() override {}

  bool IsCompatible(const BRepGraphSupInc_Store& theTarget) const override
  {
    return theTarget.ID() == ID();
  }

  occ::handle<BRepGraphSupInc_Store> NewEmpty() const override
  {
    return new BRepGraphSupInc_RejectingNodeCopyStore();
  }

  bool CopyTo(BRepGraphSupInc_CopyContext& theContext) const override
  {
    const occ::handle<BRepGraphSupInc_RejectingNodeCopyStore> aTarget =
      occ::down_cast<BRepGraphSupInc_RejectingNodeCopyStore>(theContext.TargetStore(ID()));
    if (aTarget.IsNull())
    {
      return false;
    }
    aTarget->myOwner = theContext.TargetNode(myOwner);
    return true;
  }

  bool HasRecordsForMappedNodes(const BRepGraphSupInc_CopyContext& theContext) const override
  {
    return theContext.HasTargetNode(myOwner);
  }

  bool CanCopyRecordsForMappedNodes(const BRepGraphSupInc_CopyContext&) const override
  {
    ++myPreflightCalls;
    return false;
  }

  void CopyRecordsForMappedNodes(BRepGraphSupInc_CopyContext&) const override {}

private:
  BRepGraph_NodeId myOwner;
  mutable uint32_t myPreflightCalls = 0;
};

class BRepGraphSupInc_RemapStore : public BRepGraphSupInc_Store
{
public:
  DEFINE_STANDARD_RTTI_INLINE(BRepGraphSupInc_RemapStore, BRepGraphSupInc_Store)

  static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_ID("e4ca3d38-bb48-4e67-a86a-30db2fb8a932");
    return THE_ID;
  }

  const Standard_GUID& ID() const override { return GetID(); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("BRepGraphSupInc_RemapStore");
    return THE_NAME;
  }

  BRepGraphSupInc_DefinitionId FindDefinitionByUID(const BRepGraphSupInc_ItemUID&) const override
  {
    return BRepGraphSupInc_DefinitionId();
  }

  bool IsEmpty() const override { return true; }

  void SetOwner(const BRepGraph_NodeId theOwner) { myOwner = theOwner; }

  bool WasRemapped() const { return myWasRemapped; }

  BRepGraph_NodeId Owner() const { return myOwner; }

protected:
  bool Remove(const BRepGraphSupInc_ItemUID&) override { return false; }

  void Clear() noexcept override {}

  void Compact() override {}

  void RemapCoreNodes(
    const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap) override
  {
    myWasRemapped                          = true;
    const BRepGraph_NodeId* aRemappedOwner = theNodeMap.Seek(myOwner);
    myOwner = aRemappedOwner == nullptr ? BRepGraph_NodeId() : *aRemappedOwner;
  }

  occ::handle<BRepGraphSupInc_Store> CloneForCompaction(
    const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap) const override
  {
    occ::handle<BRepGraphSupInc_RemapStore> aResult      = new BRepGraphSupInc_RemapStore();
    const BRepGraph_NodeId*                 aMappedOwner = theNodeMap.Seek(myOwner);
    aResult->myOwner       = aMappedOwner == nullptr ? BRepGraph_NodeId() : *aMappedOwner;
    aResult->myWasRemapped = true;
    return aResult;
  }

  bool IsCompatible(const BRepGraphSupInc_Store& theTarget) const override
  {
    return theTarget.ID() == ID();
  }

  occ::handle<BRepGraphSupInc_Store> NewEmpty() const override
  {
    return new BRepGraphSupInc_RemapStore();
  }

  bool CopyTo(BRepGraphSupInc_CopyContext& theContext) const override
  {
    const occ::handle<BRepGraphSupInc_RemapStore> aTarget =
      occ::down_cast<BRepGraphSupInc_RemapStore>(theContext.TargetStore(ID()));
    if (aTarget.IsNull())
    {
      return false;
    }
    aTarget->myOwner       = theContext.TargetNode(myOwner);
    aTarget->myWasRemapped = myWasRemapped;
    return true;
  }

private:
  BRepGraph_NodeId myOwner;
  bool             myWasRemapped = false;
};

} // namespace

TEST(BRepGraphSupIncTest, Stores_RegistryLifecycleAndIdentity)
{
  BRepGraph aGraph;
  EXPECT_TRUE(aGraph.Supplements().FindStore(BRepGraphSupInc_GeometryStore::GetID()).IsNull());

  const occ::handle<BRepGraphSupInc_GeometryStore> aGeometry =
    aGraph.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  ASSERT_FALSE(aGeometry.IsNull());
  EXPECT_FALSE(aGraph.Supplements().RegisterStore(new BRepGraphSupInc_GeometryStore()));
  const occ::handle<Geom_Line>    aLine        = new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_Curve3dId aCurveId     = aGeometry->Add(aLine);
  const BRepGraphSupInc_ItemUID   aGeometryUID = aGeometry->ItemUID(aCurveId);
  EXPECT_TRUE(aGeometryUID.IsValid());
  EXPECT_EQ(aGeometryUID.StoreId, aGeometry->StoreId());
  EXPECT_TRUE(aGraph.Supplements().Has(aGeometryUID));
  EXPECT_FALSE(aGraph.Supplements().UnregisterStore(BRepGraphSupInc_GeometryStore::GetID()));
  EXPECT_THROW(aGeometry->Add(occ::handle<Geom_Curve>()), Standard_NullObject);

  aGraph.Clear();
  EXPECT_FALSE(aGraph.Supplements().FindStore(BRepGraphSupInc_GeometryStore::GetID()).IsNull());
  EXPECT_TRUE(aGraph.Supplements().UnregisterStore(BRepGraphSupInc_GeometryStore::GetID()));
}

TEST(BRepGraphSupIncTest, RevisionRejectsStoreWithoutRetentionPolicy)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Supplements().RegisterStore(new BRepGraphSupInc_TestStore()));

  const BRepGraph_Revision::CreateResult aResult = BRepGraph_Revision::Create(aGraph);
  EXPECT_EQ(aResult.Status, BRepGraph_RevisionStatus::Code::UnsupportedComponent);
  ASSERT_FALSE(aResult.Diagnostics.IsEmpty());
  EXPECT_TRUE(aResult.Diagnostics.First().Code.IsEqual("UnspecifiedStoreRetention"));
}

TEST(BRepGraphSupIncTest, StoreIteratorMoveInvalidatesSource)
{
  BRepGraphSupInc_Storage aStorage;
  ASSERT_TRUE(aStorage.ChangeRegistry().RegisterStore(new BRepGraphSupInc_TestStore()));

  BRepGraphSupInc_StoreRegistry::Iterator anIterator(aStorage.Registry());
  ASSERT_TRUE(anIterator.More());
  BRepGraphSupInc_StoreRegistry::Iterator aMovedIterator(std::move(anIterator));
  EXPECT_FALSE(anIterator.More());
  ASSERT_TRUE(aMovedIterator.More());
  EXPECT_EQ(aMovedIterator.Value()->ID(), BRepGraphSupInc_TestStore::GetID());
  aMovedIterator.Next();
  EXPECT_FALSE(aMovedIterator.More());
}

TEST(BRepGraphSupIncTest, GeometryStore_RuntimeUIDLookup)
{
  BRepGraph                                        aGraph;
  const occ::handle<BRepGraphSupInc_GeometryStore> aGeometry =
    aGraph.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  ASSERT_FALSE(aGeometry.IsNull());
  const occ::handle<Geom_Line>    aCurve3d    = new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0));
  const occ::handle<Geom2d_Line>  aCurve2d    = new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(1.0, 0.0));
  const occ::handle<Geom_Plane>   aSurface    = new Geom_Plane(gp_Pnt(), gp_Dir(0.0, 0.0, 1.0));
  const BRepGraphSupInc_Curve3dId aCurve3dID  = aGeometry->Add(aCurve3d);
  const BRepGraphSupInc_Curve2dId aCurve2dID  = aGeometry->Add(aCurve2d);
  const BRepGraphSupInc_SurfaceId aSurfaceID  = aGeometry->Add(aSurface);
  const BRepGraphSupInc_ItemUID   aCurve3dUID = aGeometry->ItemUID(aCurve3dID);
  const BRepGraphSupInc_ItemUID   aCurve2dUID = aGeometry->ItemUID(aCurve2dID);
  const BRepGraphSupInc_ItemUID   aSurfaceUID = aGeometry->ItemUID(aSurfaceID);

  const occ::handle<BRepGraphSupInc_Store> aStore      = aGeometry;
  const BRepGraphSupInc_DefinitionId       aDefinition = aStore->FindDefinitionByUID(aCurve3dUID);
  ASSERT_TRUE(aDefinition.IsValid());
  EXPECT_EQ(aDefinition,
            (BRepGraphSupInc_DefinitionId{
              static_cast<uint32_t>(BRepGraphSupInc_GeometryStore::Kind::Curve3d),
              aCurve3dID.Index}));
  EXPECT_EQ(aGeometry->FindByUID<BRepGraphSupInc_Curve3dId>(aCurve3dUID), aCurve3dID);
  EXPECT_EQ(aGeometry->FindByUID<BRepGraphSupInc_Curve2dId>(aCurve2dUID), aCurve2dID);
  EXPECT_EQ(aGeometry->FindByUID<BRepGraphSupInc_SurfaceId>(aSurfaceUID), aSurfaceID);

  const BRepGraphSupInc_ItemUID aWrongStoreUID{0, aCurve3dUID.Kind, aCurve3dUID.Counter};
  const BRepGraphSupInc_ItemUID aWrongKindUID{aGeometry->StoreId(), 42, aCurve3dUID.Counter};
  const BRepGraphSupInc_ItemUID aMissingUID{aGeometry->StoreId(),
                                            aCurve3dUID.Kind,
                                            aCurve3dUID.Counter + 1};
  EXPECT_FALSE(aStore->FindDefinitionByUID(aWrongStoreUID).IsValid());
  EXPECT_FALSE(aStore->FindDefinitionByUID(aWrongKindUID).IsValid());
  EXPECT_FALSE(aGraph.Supplements().FindDefinitionByUID(aWrongStoreUID).IsValid());
  EXPECT_FALSE(aGraph.Supplements().FindDefinitionByUID(aMissingUID).IsValid());
  EXPECT_FALSE(aGeometry->FindByUID<BRepGraphSupInc_Curve3dId>(aCurve2dUID).IsValid());
  EXPECT_FALSE(aGeometry->FindByUID<BRepGraphSupInc_Curve2dId>(aSurfaceUID).IsValid());
  EXPECT_FALSE(aGeometry->FindByUID<BRepGraphSupInc_SurfaceId>(aCurve3dUID).IsValid());
  EXPECT_TRUE(aGraph.Supplements().Remove(aCurve3dUID));
  EXPECT_FALSE(aStore->FindDefinitionByUID(aCurve3dUID).IsValid());
  EXPECT_FALSE(aGeometry->FindByUID<BRepGraphSupInc_Curve3dId>(aCurve3dUID).IsValid());
}

TEST(BRepGraphSupIncTest, StoreId_IsNotReusedAfterUnregister)
{
  BRepGraph                                        aGraph;
  const occ::handle<BRepGraphSupInc_GeometryStore> aGeometry =
    aGraph.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const BRepGraphSupInc_Storage::StoreId aFirstId = aGeometry->StoreId();
  ASSERT_TRUE(aGraph.Supplements().UnregisterStore(BRepGraphSupInc_GeometryStore::GetID()));
  const occ::handle<BRepGraphSupInc_GeometryStore> aReplacement =
    aGraph.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  EXPECT_NE(aReplacement->StoreId(), aFirstId);
}

TEST(BRepGraphSupIncTest, StoreId_IsUniqueAcrossRegistrationOrderAndForeignGraphs)
{
  BRepGraph                                        aFirstGraph;
  BRepGraph                                        aSecondGraph;
  const occ::handle<BRepGraphSupInc_GeometryStore> aFirstGeometry =
    aFirstGraph.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  (void)aSecondGraph.Supplements().EnsureStore<BRepGraphSupInc_TopologyStore>();
  const occ::handle<BRepGraphSupInc_GeometryStore> aSecondGeometry =
    aSecondGraph.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line>  aFirstLine  = new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0));
  const occ::handle<Geom_Line>  aSecondLine = new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_ItemUID aFirstUID =
    aFirstGeometry->ItemUID(aFirstGeometry->Add(aFirstLine));
  const BRepGraphSupInc_ItemUID aSecondUID =
    aSecondGeometry->ItemUID(aSecondGeometry->Add(aSecondLine));

  EXPECT_NE(aFirstGeometry->StoreId(), aSecondGeometry->StoreId());
  EXPECT_NE(aFirstUID, aSecondUID);
  EXPECT_FALSE(aSecondGraph.Supplements().Has(aFirstUID));
}

TEST(BRepGraphSupIncTest, Copy_OrdersCustomStoreDependencies)
{
  BRepGraphSupInc_Storage                            aSource;
  const occ::handle<BRepGraphSupInc_DependencyStore> aSecond =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::Second);
  const occ::handle<BRepGraphSupInc_DependencyStore> aFirst =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::First);
  ASSERT_TRUE(aSource.ChangeRegistry().RegisterStore(aSecond));
  ASSERT_TRUE(aSource.ChangeRegistry().RegisterStore(aFirst));

  BRepGraphSupInc_Storage     aTarget;
  BRepGraphSupInc_CopyContext aContext(aSource, aTarget);
  ASSERT_TRUE(aSource.CopyTo(aTarget, aContext));
  ASSERT_EQ(aTarget.Registry().Count(), 2u);
  const occ::handle<BRepGraphSupInc_DependencyStore> aCopiedFirst =
    occ::down_cast<BRepGraphSupInc_DependencyStore>(aTarget.Registry().FindStore(
      BRepGraphSupInc_DependencyStore::GetID(BRepGraphSupInc_DependencyStore::Type::First)));
  const occ::handle<BRepGraphSupInc_DependencyStore> aCopiedSecond =
    occ::down_cast<BRepGraphSupInc_DependencyStore>(aTarget.Registry().FindStore(
      BRepGraphSupInc_DependencyStore::GetID(BRepGraphSupInc_DependencyStore::Type::Second)));
  ASSERT_FALSE(aCopiedFirst.IsNull());
  ASSERT_FALSE(aCopiedSecond.IsNull());
  EXPECT_TRUE(aCopiedFirst->WasCopied());
  EXPECT_TRUE(aCopiedSecond->WasCopied());
}

TEST(BRepGraphSupIncTest, Copy_RejectsMissingAndCyclicStoreDependenciesBeforeTargetMutation)
{
  BRepGraphSupInc_Storage                            aMissingSource;
  const occ::handle<BRepGraphSupInc_DependencyStore> aMissing =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::Missing);
  ASSERT_TRUE(aMissingSource.ChangeRegistry().RegisterStore(aMissing));
  BRepGraphSupInc_Storage     aMissingTarget;
  BRepGraphSupInc_CopyContext aMissingContext(aMissingSource, aMissingTarget);
  EXPECT_FALSE(aMissingSource.CopyTo(aMissingTarget, aMissingContext));
  EXPECT_EQ(aMissingTarget.Registry().Count(), 0u);

  BRepGraphSupInc_Storage                            aCyclicSource;
  const occ::handle<BRepGraphSupInc_DependencyStore> aCycleFirst =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::CycleFirst);
  const occ::handle<BRepGraphSupInc_DependencyStore> aCycleSecond =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::CycleSecond);
  ASSERT_TRUE(aCyclicSource.ChangeRegistry().RegisterStore(aCycleFirst));
  ASSERT_TRUE(aCyclicSource.ChangeRegistry().RegisterStore(aCycleSecond));
  BRepGraphSupInc_Storage     aCyclicTarget;
  BRepGraphSupInc_CopyContext aCyclicContext(aCyclicSource, aCyclicTarget);
  EXPECT_FALSE(aCyclicSource.CopyTo(aCyclicTarget, aCyclicContext));
  EXPECT_EQ(aCyclicTarget.Registry().Count(), 0u);
}

TEST(BRepGraphSupIncTest, CopyNode_RejectsInvalidStoreDependenciesBeforeCoreMutation)
{
  BRepGraph                aMissingSource;
  const BRepGraph_VertexId aMissingVertex =
    aMissingSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const occ::handle<BRepGraphSupInc_DependencyStore> aMissing =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::Missing);
  ASSERT_TRUE(aMissingSource.Supplements().RegisterStore(aMissing));
  BRepGraph aMissingTarget;
  (void)aMissingTarget.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);

  EXPECT_FALSE(BRepGraph_Copy::CopyNode(aMissingSource, aMissingTarget, aMissingVertex).IsValid());
  EXPECT_EQ(aMissingTarget.Topo().Vertices().Nb(), 1u);
  EXPECT_TRUE(aMissingTarget.Supplements().FindStore(aMissing->ID()).IsNull());

  BRepGraph                aCyclicSource;
  const BRepGraph_VertexId aCyclicVertex = aCyclicSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const occ::handle<BRepGraphSupInc_DependencyStore> aCycleFirst =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::CycleFirst);
  const occ::handle<BRepGraphSupInc_DependencyStore> aCycleSecond =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::CycleSecond);
  ASSERT_TRUE(aCyclicSource.Supplements().RegisterStore(aCycleFirst));
  ASSERT_TRUE(aCyclicSource.Supplements().RegisterStore(aCycleSecond));
  BRepGraph aCyclicTarget;
  (void)aCyclicTarget.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);

  EXPECT_FALSE(BRepGraph_Copy::CopyNode(aCyclicSource, aCyclicTarget, aCyclicVertex).IsValid());
  EXPECT_EQ(aCyclicTarget.Topo().Vertices().Nb(), 1u);
  EXPECT_TRUE(aCyclicTarget.Supplements().FindStore(aCycleFirst->ID()).IsNull());
  EXPECT_TRUE(aCyclicTarget.Supplements().FindStore(aCycleSecond->ID()).IsNull());
}

TEST(BRepGraphSupIncTest, CopyNode_RejectsFailingNewEmptyBeforeCoreMutation)
{
  BRepGraph                aSource;
  const BRepGraph_VertexId aSourceVertex = aSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const occ::handle<BRepGraphSupInc_FailingNodeCopyStore> aFailingStore =
    new BRepGraphSupInc_FailingNodeCopyStore();
  ASSERT_TRUE(aSource.Supplements().RegisterStore(aFailingStore));

  BRepGraph aTarget;
  (void)aTarget.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const size_t aTargetVertexCount = aTarget.Topo().Vertices().Nb();
  EXPECT_TRUE(
    aTarget.Supplements().FindStore(BRepGraphSupInc_FailingNodeCopyStore::GetID()).IsNull());

  EXPECT_FALSE(BRepGraph_Copy::CopyNode(aSource, aTarget, aSourceVertex).IsValid());
  EXPECT_EQ(aFailingStore->NewEmptyCalls(), 1u);
  EXPECT_EQ(aTarget.Topo().Vertices().Nb(), aTargetVertexCount);
  EXPECT_TRUE(
    aTarget.Supplements().FindStore(BRepGraphSupInc_FailingNodeCopyStore::GetID()).IsNull());
}

TEST(BRepGraphSupIncTest, FullCopyFailureKeepsNonEmptyTargetUnchanged)
{
  BRepGraph aSource;
  ASSERT_TRUE(aSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7).IsValid());
  const occ::handle<BRepGraphSupInc_FailingNodeCopyStore> aFailingStore =
    new BRepGraphSupInc_FailingNodeCopyStore();
  ASSERT_TRUE(aSource.Supplements().RegisterStore(aFailingStore));

  BRepGraph                aTarget;
  const BRepGraph_VertexId aTargetVertex =
    aTarget.Editor().Vertices().Add(gp_Pnt(3.0, 4.0, 5.0), 1.0e-7);
  ASSERT_TRUE(aTargetVertex.IsValid());
  const Standard_GUID aGraphGUID = aTarget.UIDs().GraphGUID();

  EXPECT_FALSE(BRepGraph_Copy::Perform(aSource, aTarget));
  EXPECT_EQ(aTarget.UIDs().GraphGUID(), aGraphGUID);
  EXPECT_EQ(aTarget.Topo().Vertices().Nb(), 1u);
  EXPECT_EQ(aTarget.Topo().Vertices().Definition(aTargetVertex).Point.X(), 3.0);
  EXPECT_TRUE(
    aTarget.Supplements().FindStore(BRepGraphSupInc_FailingNodeCopyStore::GetID()).IsNull());
}

TEST(BRepGraphSupIncTest, CopyNode_RejectsStorePreflightBeforeCoreOrRegistryMutation)
{
  BRepGraph                aSource;
  const BRepGraph_VertexId aSourceVertex = aSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const occ::handle<BRepGraphSupInc_RejectingNodeCopyStore> aRejectingStore =
    new BRepGraphSupInc_RejectingNodeCopyStore();
  aRejectingStore->SetOwner(aSourceVertex);
  ASSERT_TRUE(aSource.Supplements().RegisterStore(aRejectingStore));

  BRepGraph aTarget;
  (void)aTarget.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const uint32_t aTargetVertexCount = aTarget.Topo().Vertices().Nb();

  EXPECT_FALSE(BRepGraph_Copy::CopyNode(aSource, aTarget, aSourceVertex).IsValid());
  EXPECT_EQ(aRejectingStore->PreflightCalls(), 1u);
  EXPECT_EQ(aTarget.Topo().Vertices().Nb(), aTargetVertexCount);
  EXPECT_TRUE(
    aTarget.Supplements().FindStore(BRepGraphSupInc_RejectingNodeCopyStore::GetID()).IsNull());
}

TEST(BRepGraphSupIncTest, CopyNode_DoesNotRegisterStoreWithoutMappedNodeRecords)
{
  BRepGraph                aSource;
  const BRepGraph_VertexId aSourceVertex = aSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const occ::handle<BRepGraphSupInc_GeometryStore> aGeometry =
    aSource.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0));
  (void)aGeometry->Add(aLine);

  BRepGraph aTarget;
  EXPECT_TRUE(BRepGraph_Copy::CopyNode(aSource, aTarget, aSourceVertex).IsValid());
  EXPECT_TRUE(aTarget.Supplements().FindStore(BRepGraphSupInc_GeometryStore::GetID()).IsNull());
}

TEST(BRepGraphSupIncTest, CopyNode_ClonesRegisteredTargetStoreWithMappedRecords)
{
  BRepGraph                aSource;
  const BRepGraph_VertexId aSourceVertex = aSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);

  BRepGraph                aTarget;
  const BRepGraph_VertexId aTargetVertex =
    aTarget.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<BRepGraphSupInc_DependencyStore> aTargetDependency =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::First);
  const occ::handle<BRepGraphSupInc_DependencyStore> aTargetDependent =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::Second);
  aTargetDependent->SetNodeOwner(aTargetVertex);
  ASSERT_TRUE(aTarget.Supplements().RegisterStore(aTargetDependent));
  ASSERT_TRUE(aTarget.Supplements().RegisterStore(aTargetDependency));

  EXPECT_TRUE(BRepGraph_Copy::CopyNode(aSource, aTarget, aSourceVertex).IsValid());
  EXPECT_FALSE(aTargetDependent->WasCopied());
}

TEST(BRepGraphSupIncTest, CopyNode_RegistersDependencyClosureForMappedStore)
{
  BRepGraph                aSource;
  const BRepGraph_VertexId aSourceVertex = aSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const occ::handle<BRepGraphSupInc_DependencyStore> aDependent =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::Second);
  const occ::handle<BRepGraphSupInc_DependencyStore> aDependency =
    new BRepGraphSupInc_DependencyStore(BRepGraphSupInc_DependencyStore::Type::First);
  aDependent->SetNodeOwner(BRepGraph_NodeId(aSourceVertex));
  ASSERT_TRUE(aSource.Supplements().RegisterStore(aDependent));
  ASSERT_TRUE(aSource.Supplements().RegisterStore(aDependency));

  BRepGraph aTarget;
  EXPECT_TRUE(BRepGraph_Copy::CopyNode(aSource, aTarget, aSourceVertex).IsValid());
  const occ::handle<BRepGraphSupInc_DependencyStore> aCopiedDependent =
    occ::down_cast<BRepGraphSupInc_DependencyStore>(
      aTarget.Supplements().FindStore(aDependent->ID()));
  const occ::handle<BRepGraphSupInc_DependencyStore> aCopiedDependency =
    occ::down_cast<BRepGraphSupInc_DependencyStore>(
      aTarget.Supplements().FindStore(aDependency->ID()));
  ASSERT_FALSE(aCopiedDependent.IsNull());
  ASSERT_FALSE(aCopiedDependency.IsNull());
  EXPECT_NE(aCopiedDependency->StoreId(), 0u);
  EXPECT_FALSE(aCopiedDependency->WasNodeRecordCopied());
  EXPECT_TRUE(aCopiedDependent->WasNodeRecordCopied());
}

TEST(BRepGraphSupIncTest, Endpoint_UsesCompactVariantItemAccessors)
{
  static_assert(sizeof(BRepGraphSupInc_Endpoint) <= 16);
  static_assert(!std::is_same_v<BRepGraphSupInc_Endpoint, BRepGraphSupInc_ItemUID>);

  const BRepGraphSupInc_Endpoint anInvalidEndpoint;
  EXPECT_EQ(anInvalidEndpoint.ItemDomain(), BRepGraphSupInc_Endpoint::Domain::None);
  EXPECT_FALSE(anInvalidEndpoint.IsValid());
  EXPECT_EQ(anInvalidEndpoint.CoreItem(), nullptr);
  EXPECT_EQ(anInvalidEndpoint.SupplementalItem(), nullptr);

  BRepGraph                aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const occ::handle<BRepGraphSupInc_GeometryStore> aGeometry =
    aGraph.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line>   aLine       = new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_ItemUID  aUID        = aGeometry->ItemUID(aGeometry->Add(aLine));
  const BRepGraphSupInc_Endpoint aCore       = coreEndpoint(aVertex);
  const BRepGraphSupInc_Endpoint aSupplement = BRepGraphSupInc_Endpoint::Supplemental(aUID);
  EXPECT_EQ(aCore.ItemDomain(), BRepGraphSupInc_Endpoint::Domain::CoreItem);
  ASSERT_NE(aCore.CoreItem(), nullptr);
  EXPECT_EQ(*aCore.CoreItem(), BRepGraph_ItemId(aVertex));
  EXPECT_EQ(aCore.SupplementalItem(), nullptr);
  EXPECT_EQ(aSupplement.ItemDomain(), BRepGraphSupInc_Endpoint::Domain::SupplementalItem);
  ASSERT_NE(aSupplement.SupplementalItem(), nullptr);
  EXPECT_EQ(*aSupplement.SupplementalItem(), aUID);
  EXPECT_EQ(aSupplement.CoreItem(), nullptr);
}

TEST(BRepGraphSupIncTest, StorageMoveAndCompact_RebindStoreIdentity)
{
  BRepGraphSupInc_Storage                          aStorage;
  const occ::handle<BRepGraphSupInc_GeometryStore> aGeometry =
    aStorage.ChangeRegistry().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line>           aLine    = new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_ItemUID          aUID     = aGeometry->ItemUID(aGeometry->Add(aLine));
  const BRepGraphSupInc_Storage::StoreId aStoreId = aGeometry->StoreId();

  BRepGraphSupInc_Storage aMovedStorage(std::move(aStorage));
  EXPECT_EQ(aGeometry->StoreId(), aStoreId);
  EXPECT_TRUE(aMovedStorage.Has(aUID));
  aMovedStorage.Compact();
  EXPECT_TRUE(aMovedStorage.Has(aUID));
}

TEST(BRepGraphSupIncTest, StoreRegistration_RejectsStoreBoundToAnotherRegistry)
{
  BRepGraphSupInc_Storage                          aFirstStorage;
  BRepGraphSupInc_Storage                          aSecondStorage;
  const occ::handle<BRepGraphSupInc_GeometryStore> aStore = new BRepGraphSupInc_GeometryStore();

  ASSERT_TRUE(aFirstStorage.ChangeRegistry().RegisterStore(aStore));
  const uint32_t aFirstStoreId = aStore->StoreId();
  EXPECT_FALSE(aSecondStorage.ChangeRegistry().RegisterStore(aStore));
  EXPECT_EQ(aFirstStorage.Registry().Count(), 1u);
  EXPECT_EQ(aSecondStorage.Registry().Count(), 0u);
  EXPECT_EQ(aStore->StoreId(), aFirstStoreId);
  EXPECT_EQ(aFirstStorage.Registry().FindStore(BRepGraphSupInc_GeometryStore::GetID()), aStore);
}

TEST(BRepGraphSupIncTest, GeometryStore_ClearDoesNotReuseUIDCounter)
{
  BRepGraphSupInc_Storage                          aStorage;
  const occ::handle<BRepGraphSupInc_GeometryStore> aGeometry =
    aStorage.ChangeRegistry().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line> aFirstLine =
    new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_ItemUID aFirstUID = aGeometry->ItemUID(aGeometry->Add(aFirstLine));

  aStorage.Clear();
  const occ::handle<Geom_Line> aSecondLine =
    new Geom_Line(gp_Pnt(2.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_ItemUID aSecondUID = aGeometry->ItemUID(aGeometry->Add(aSecondLine));
  EXPECT_FALSE(aStorage.Has(aFirstUID));
  EXPECT_EQ(aSecondUID.StoreId, aFirstUID.StoreId);
  EXPECT_NE(aSecondUID, aFirstUID);
  EXPECT_GT(aSecondUID.Counter, aFirstUID.Counter);
}

TEST(BRepGraphSupIncTest, GenericLayer_RemovalDropsOnlyAffectedReferences)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<BRepGraphSupInc_GeometryStore> aGeometry =
    aGraph.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line> aGeometryLine =
    new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_ItemUID  aGeometryUID = aGeometry->ItemUID(aGeometry->Add(aGeometryLine));
  const BRepGraphSupInc_Endpoint aCore        = coreEndpoint(aVertex);
  const BRepGraphSupInc_Endpoint aSupplement = BRepGraphSupInc_Endpoint::Supplemental(aGeometryUID);

  const occ::handle<BRepGraph_LayerSupplement> aLayer =
    aGraph.LayerSupplementRegistry().Ensure<BRepGraph_LayerSupplement>();
  EXPECT_TRUE(aGraph.LayerRegistry().Find<BRepGraph_LayerSupplement>().IsNull());
  ASSERT_NE(aLayer->Add(aCore, aSupplement, BRepGraph_LayerSupplement::GetID()), 0u);
  EXPECT_EQ(aLayer->Related(aCore).Size(), 1u);
  EXPECT_EQ(aLayer->Related(aSupplement).Size(), 1u);

  EXPECT_TRUE(aGraph.Supplements().Remove(aGeometryUID));
  EXPECT_TRUE(aLayer->Related(aCore).IsEmpty());
  EXPECT_TRUE(aLayer->Related(aSupplement).IsEmpty());
  EXPECT_FALSE(aVertex.IsRemoved(aGraph));

  const occ::handle<Geom_Line> aRetainedLine =
    new Geom_Line(gp_Pnt(2.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_ItemUID  aRetainedUID = aGeometry->ItemUID(aGeometry->Add(aRetainedLine));
  const BRepGraphSupInc_Endpoint aRetainedSupplement =
    BRepGraphSupInc_Endpoint::Supplemental(aRetainedUID);
  ASSERT_NE(aLayer->Add(aCore, aRetainedSupplement, BRepGraph_LayerSupplement::GetID()), 0u);
  aGraph.Editor().Gen().RemoveNode(aVertex);
  EXPECT_TRUE(aLayer->Related(aRetainedSupplement).IsEmpty());
  EXPECT_TRUE(aGraph.Supplements().Has(aRetainedUID));
}

TEST(BRepGraphSupIncTest, GenericLayer_SelfRelationIsIndexedAndRemovedOnce)
{
  BRepGraph                      aGraph;
  const BRepGraph_VertexId       aVertex    = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraphSupInc_Endpoint anEndpoint = coreEndpoint(aVertex);
  const occ::handle<BRepGraph_LayerSupplement> aLayer =
    aGraph.LayerSupplementRegistry().Ensure<BRepGraph_LayerSupplement>();

  const uint32_t aReference =
    aLayer->Add(anEndpoint, anEndpoint, BRepGraph_LayerSupplement::GetID());
  ASSERT_NE(aReference, 0u);
  EXPECT_EQ(aLayer->Related(anEndpoint).Size(), 1u);
  EXPECT_TRUE(aLayer->Remove(aReference));
  EXPECT_TRUE(aLayer->Related(anEndpoint).IsEmpty());
}

TEST(BRepGraphSupIncTest, Copy_AllocatesTargetUIDsAndRemapsReferences)
{
  BRepGraph                aSource;
  const BRepGraph_VertexId aSourceVertex =
    aSource.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<BRepGraphSupInc_GeometryStore> aSourceGeometry =
    aSource.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line> aSourceLine =
    new Geom_Line(gp_Pnt(2.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_ItemUID aSourceUID =
    aSourceGeometry->ItemUID(aSourceGeometry->Add(aSourceLine));
  const occ::handle<BRepGraph_LayerSupplement> aSourceLayer =
    aSource.LayerSupplementRegistry().Ensure<BRepGraph_LayerSupplement>();
  ASSERT_NE(aSourceLayer->Add(coreEndpoint(aSourceVertex),
                              BRepGraphSupInc_Endpoint::Supplemental(aSourceUID),
                              BRepGraph_LayerSupplement::GetID()),
            0u);

  BRepGraph anIdentityTarget;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource, anIdentityTarget));
  EXPECT_FALSE(anIdentityTarget.Supplements().Has(aSourceUID));
  const occ::handle<BRepGraph_LayerSupplement> anIdentityLayer =
    anIdentityTarget.LayerSupplementRegistry().Find<BRepGraph_LayerSupplement>();
  ASSERT_FALSE(anIdentityLayer.IsNull());
  ASSERT_EQ(anIdentityLayer->Related(coreEndpoint(aSourceVertex)).Size(), 1u);
  const BRepGraph_LayerSupplement::Reference* anIdentityReference =
    anIdentityLayer->Find(anIdentityLayer->Related(coreEndpoint(aSourceVertex)).First());
  ASSERT_NE(anIdentityReference, nullptr);
  const BRepGraphSupInc_ItemUID* anIdentityItem = anIdentityReference->Target.SupplementalItem();
  ASSERT_NE(anIdentityItem, nullptr);
  EXPECT_NE(*anIdentityItem, aSourceUID);
  EXPECT_TRUE(anIdentityTarget.Supplements().Has(*anIdentityItem));

  BRepGraph anAppendTarget;
  (void)anAppendTarget.Editor().Vertices().Add(gp_Pnt(-1.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<BRepGraphSupInc_GeometryStore> anAppendGeometry =
    anAppendTarget.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line> anAppendLine =
    new Geom_Line(gp_Pnt(-2.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  (void)anAppendGeometry->Add(anAppendLine);
  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource, anAppendTarget));
  const occ::handle<BRepGraph_LayerSupplement> anAppendLayer =
    anAppendTarget.LayerSupplementRegistry().Find<BRepGraph_LayerSupplement>();
  ASSERT_FALSE(anAppendLayer.IsNull());
  const BRepGraph_VertexId aCopiedVertex(1);
  ASSERT_EQ(anAppendLayer->Related(coreEndpoint(aCopiedVertex)).Size(), 1u);
  const BRepGraph_LayerSupplement::Reference* aReference =
    anAppendLayer->Find(anAppendLayer->Related(coreEndpoint(aCopiedVertex)).First());
  ASSERT_NE(aReference, nullptr);
  const BRepGraphSupInc_ItemUID* anAppendItem = aReference->Target.SupplementalItem();
  ASSERT_NE(anAppendItem, nullptr);
  EXPECT_NE(*anAppendItem, aSourceUID);
  EXPECT_TRUE(anAppendTarget.Supplements().Has(*anAppendItem));
}

TEST(BRepGraphSupIncTest, CopyToClearedTargetDoesNotReactivateSourceUID)
{
  BRepGraph                aSource;
  const BRepGraph_VertexId aSourceVertex = aSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const occ::handle<BRepGraphSupInc_GeometryStore> aSourceGeometry =
    aSource.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line>  aSourceLine = new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_ItemUID aSourceUID =
    aSourceGeometry->ItemUID(aSourceGeometry->Add(aSourceLine));
  const occ::handle<BRepGraph_LayerSupplement> aLayer =
    aSource.LayerSupplementRegistry().Ensure<BRepGraph_LayerSupplement>();
  ASSERT_NE(aLayer->Add(coreEndpoint(aSourceVertex),
                        BRepGraphSupInc_Endpoint::Supplemental(aSourceUID),
                        BRepGraph_LayerSupplement::GetID()),
            0u);

  BRepGraph                                        aTarget;
  const occ::handle<BRepGraphSupInc_GeometryStore> aTargetGeometry =
    aTarget.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line> aTargetLine = new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0));
  (void)aTargetGeometry->Add(aTargetLine);
  aTarget.Clear();
  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource, aTarget));
  EXPECT_FALSE(aTarget.Supplements().Has(aSourceUID));
  EXPECT_NE(aSourceGeometry->StoreId(), aTargetGeometry->StoreId());
}

TEST(BRepGraphSupIncTest, Copy_CustomStoreAllocatesTargetUIDsAndRemapsReferences)
{
  BRepGraph                aSource;
  const BRepGraph_VertexId aSourceVertex = aSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const occ::handle<BRepGraphSupInc_TestStore> aSourceStore = new BRepGraphSupInc_TestStore();
  ASSERT_TRUE(aSource.Supplements().RegisterStore(aSourceStore));
  const BRepGraphSupInc_ItemUID                aSourceUID = aSourceStore->Add(42);
  const occ::handle<BRepGraph_LayerSupplement> aSourceLayer =
    aSource.LayerSupplementRegistry().Ensure<BRepGraph_LayerSupplement>();
  ASSERT_NE(aSourceLayer->Add(coreEndpoint(aSourceVertex),
                              BRepGraphSupInc_Endpoint::Supplemental(aSourceUID),
                              BRepGraph_LayerSupplement::GetID()),
            0u);

  BRepGraph anIdentityTarget;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource, anIdentityTarget));
  const occ::handle<BRepGraphSupInc_TestStore> anIdentityStore =
    occ::down_cast<BRepGraphSupInc_TestStore>(
      anIdentityTarget.Supplements().FindStore(BRepGraphSupInc_TestStore::GetID()));
  ASSERT_FALSE(anIdentityStore.IsNull());
  EXPECT_NE(anIdentityStore->StoreId(), aSourceStore->StoreId());
  const occ::handle<BRepGraph_LayerSupplement> anIdentityLayer =
    anIdentityTarget.LayerSupplementRegistry().Find<BRepGraph_LayerSupplement>();
  ASSERT_FALSE(anIdentityLayer.IsNull());
  ASSERT_EQ(anIdentityLayer->Related(coreEndpoint(aSourceVertex)).Size(), 1u);
  const BRepGraph_LayerSupplement::Reference* anIdentityReference =
    anIdentityLayer->Find(anIdentityLayer->Related(coreEndpoint(aSourceVertex)).First());
  ASSERT_NE(anIdentityReference, nullptr);
  const BRepGraphSupInc_ItemUID* anIdentityUID = anIdentityReference->Target.SupplementalItem();
  ASSERT_NE(anIdentityUID, nullptr);
  EXPECT_NE(*anIdentityUID, aSourceUID);
  EXPECT_TRUE(anIdentityStore->Has(*anIdentityUID));

  BRepGraph anAppendTarget;
  (void)anAppendTarget.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<BRepGraphSupInc_TestStore> anAppendStore = new BRepGraphSupInc_TestStore();
  ASSERT_TRUE(anAppendTarget.Supplements().RegisterStore(anAppendStore));
  (void)anAppendStore->Add(7);
  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource, anAppendTarget));
  const occ::handle<BRepGraph_LayerSupplement> anAppendLayer =
    anAppendTarget.LayerSupplementRegistry().Find<BRepGraph_LayerSupplement>();
  ASSERT_FALSE(anAppendLayer.IsNull());
  const BRepGraph_VertexId aCopiedVertex(1);
  ASSERT_EQ(anAppendLayer->Related(coreEndpoint(aCopiedVertex)).Size(), 1u);
  const BRepGraph_LayerSupplement::Reference* anAppendReference =
    anAppendLayer->Find(anAppendLayer->Related(coreEndpoint(aCopiedVertex)).First());
  ASSERT_NE(anAppendReference, nullptr);
  const BRepGraphSupInc_ItemUID* anAppendUID = anAppendReference->Target.SupplementalItem();
  ASSERT_NE(anAppendUID, nullptr);
  EXPECT_NE(*anAppendUID, aSourceUID);
  const occ::handle<BRepGraphSupInc_TestStore> aCopiedAppendStore =
    occ::down_cast<BRepGraphSupInc_TestStore>(
      anAppendTarget.Supplements().FindStore(BRepGraphSupInc_TestStore::GetID()));
  ASSERT_FALSE(aCopiedAppendStore.IsNull());
  EXPECT_NE(aCopiedAppendStore, anAppendStore);
  EXPECT_TRUE(aCopiedAppendStore->Has(*anAppendUID));
}

TEST(BRepGraphSupIncTest, Compact_PreservesSupplementalUIDAndRemapsCoreEndpoint)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aRemovedVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(-1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aLinkedVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<BRepGraphSupInc_GeometryStore> aGeometry =
    aGraph.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const occ::handle<Geom_Line> aGeometryLine =
    new Geom_Line(gp_Pnt(3.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraphSupInc_ItemUID aGeometryUID = aGeometry->ItemUID(aGeometry->Add(aGeometryLine));
  const occ::handle<BRepGraph_LayerSupplement> aLayer =
    aGraph.LayerSupplementRegistry().Ensure<BRepGraph_LayerSupplement>();
  ASSERT_NE(aLayer->Add(coreEndpoint(aLinkedVertex),
                        BRepGraphSupInc_Endpoint::Supplemental(aGeometryUID),
                        BRepGraph_LayerSupplement::GetID()),
            0u);

  aGraph.Editor().Gen().RemoveNode(aRemovedVertex);
  ASSERT_TRUE(aRemovedVertex.IsRemoved(aGraph));
  const BRepGraph_Compact::Result aResult = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aResult.NbRemovedVertices, 1u);
  const BRepGraph_VertexId                     aCompactedVertex(0);
  const occ::handle<BRepGraph_LayerSupplement> aCompactedLayer =
    aGraph.LayerSupplementRegistry().Find<BRepGraph_LayerSupplement>();
  ASSERT_FALSE(aCompactedLayer.IsNull());
  ASSERT_EQ(aCompactedLayer->Related(coreEndpoint(aCompactedVertex)).Size(), 1u);
  const BRepGraph_LayerSupplement::Reference* aReference =
    aCompactedLayer->Find(aCompactedLayer->Related(coreEndpoint(aCompactedVertex)).First());
  ASSERT_NE(aReference, nullptr);
  const BRepGraphSupInc_ItemUID* aCompactedItem = aReference->Target.SupplementalItem();
  ASSERT_NE(aCompactedItem, nullptr);
  EXPECT_EQ(*aCompactedItem, aGeometryUID);
  EXPECT_TRUE(aGraph.Supplements().Has(aGeometryUID));
}

TEST(BRepGraphSupIncTest, Compact_DispatchesCoreNodeRemapToCustomStores)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aRemovedVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(-1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aTrackedVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<BRepGraphSupInc_RemapStore> aStore = new BRepGraphSupInc_RemapStore();
  aStore->SetOwner(aTrackedVertex);
  ASSERT_TRUE(aGraph.Supplements().RegisterStore(aStore));

  aGraph.Editor().Gen().RemoveNode(aRemovedVertex);
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  const occ::handle<BRepGraphSupInc_RemapStore> aCompactedStore =
    occ::down_cast<BRepGraphSupInc_RemapStore>(
      aGraph.Supplements().FindStore(BRepGraphSupInc_RemapStore::GetID()));
  ASSERT_FALSE(aCompactedStore.IsNull());
  EXPECT_TRUE(aCompactedStore->WasRemapped());
  EXPECT_EQ(aCompactedStore->Owner(), BRepGraph_NodeId(BRepGraph_VertexId::Start()));
  EXPECT_FALSE(aStore->WasRemapped());
  EXPECT_EQ(aStore->Owner(), BRepGraph_NodeId(aTrackedVertex));
}

TEST(BRepGraphSupIncTest, Compact_UnsupportedStoreKeepsSourceGraphUnchanged)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aRemovedVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(-1.0, 0.0, 0.0), 1.0e-7);
  (void)aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<BRepGraphSupInc_TestStore> aStore = new BRepGraphSupInc_TestStore();
  ASSERT_TRUE(aGraph.Supplements().RegisterStore(aStore));
  const BRepGraphSupInc_ItemUID anItemUID  = aStore->Add(42);
  const Standard_GUID           aGraphGUID = aGraph.UIDs().GraphGUID();

  aGraph.Editor().Gen().RemoveNode(aRemovedVertex);
  const BRepGraph_Compact::Result aResult = BRepGraph_Compact::Perform(aGraph);

  EXPECT_EQ(aResult.StatusCode, BRepGraph_Compact::Status::MigrationFailed);
  EXPECT_EQ(aGraph.UIDs().GraphGUID(), aGraphGUID);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 2u);
  EXPECT_TRUE(aRemovedVertex.IsRemoved(aGraph));
  EXPECT_EQ(aGraph.Supplements().FindStore(BRepGraphSupInc_TestStore::GetID()), aStore);
  EXPECT_TRUE(aStore->Has(anItemUID));
}

TEST(BRepGraphSupIncTest, Compact_SupplementOnlyTombstoneUsesAtomicReplacement)
{
  BRepGraph aGraph;
  (void)aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const occ::handle<BRepGraphSupInc_GeometryStore> aSourceStore =
    aGraph.Supplements().EnsureStore<BRepGraphSupInc_GeometryStore>();
  const BRepGraphSupInc_ItemUID aRemovedUID =
    aSourceStore->ItemUID(aSourceStore->Add(new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0))));
  const BRepGraphSupInc_ItemUID aRetainedUID =
    aSourceStore->ItemUID(aSourceStore->Add(new Geom_Line(gp_Pnt(), gp_Dir(0.0, 1.0, 0.0))));
  ASSERT_TRUE(aGraph.Supplements().Remove(aRemovedUID));

  const BRepGraph_Compact::Result aResult = BRepGraph_Compact::Perform(aGraph);

  ASSERT_EQ(aResult.StatusCode, BRepGraph_Compact::Status::Done);
  const occ::handle<BRepGraphSupInc_GeometryStore> aCompactedStore =
    occ::down_cast<BRepGraphSupInc_GeometryStore>(
      aGraph.Supplements().FindStore(BRepGraphSupInc_GeometryStore::GetID()));
  ASSERT_FALSE(aCompactedStore.IsNull());
  EXPECT_NE(aCompactedStore, aSourceStore);
  EXPECT_EQ(aCompactedStore->Count(BRepGraphSupInc_GeometryStore::Kind::Curve3d), 1u);
  EXPECT_FALSE(aCompactedStore->Has(aRemovedUID));
  EXPECT_TRUE(aCompactedStore->Has(aRetainedUID));
  EXPECT_EQ(aSourceStore->Count(BRepGraphSupInc_GeometryStore::Kind::Curve3d), 2u);
}
