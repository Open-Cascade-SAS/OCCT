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

#include <AIS_GlobalStatus.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Selection.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_AsciiString.hxx>
#include <V3d_Viewer.hxx>

#include <gtest/gtest.h>

namespace
{

class AIS_TestCStructure : public Graphic3d_CStructure
{
public:
  AIS_TestCStructure(const occ::handle<Graphic3d_StructureManager>& theManager)
      : Graphic3d_CStructure(theManager)
  {
  }

  void OnVisibilityChanged() override {}

  void Clear() override { myGroups.Clear(); }

  void Connect(Graphic3d_CStructure&) override {}

  void Disconnect(Graphic3d_CStructure&) override {}

  void GraphicHighlight(const occ::handle<Graphic3d_PresentationAttributes>& theStyle) override
  {
    myHighlightStyle = theStyle;
    highlight        = 1;
  }

  void GraphicUnhighlight() override
  {
    myHighlightStyle.Nullify();
    highlight = 0;
  }

  occ::handle<Graphic3d_CStructure> ShadowLink(
    const occ::handle<Graphic3d_StructureManager>& theManager) const override
  {
    return new AIS_TestCStructure(theManager);
  }

  occ::handle<Graphic3d_Group> NewGroup(const occ::handle<Graphic3d_Structure>&) override
  {
    return occ::handle<Graphic3d_Group>();
  }

  void RemoveGroup(const occ::handle<Graphic3d_Group>&) override {}

  DEFINE_STANDARD_RTTI_INLINE(AIS_TestCStructure, Graphic3d_CStructure)
};

class AIS_TestGraphicDriver : public Graphic3d_GraphicDriver
{
public:
  AIS_TestGraphicDriver()
      : Graphic3d_GraphicDriver(occ::handle<Aspect_DisplayConnection>())
  {
  }

  int InquireLimit(const Graphic3d_TypeOfLimit theType) const override
  {
    switch (theType)
    {
      case Graphic3d_TypeOfLimit_MaxNbLights:
        return 8;
      case Graphic3d_TypeOfLimit_MaxNbClipPlanes:
        return 6;
      case Graphic3d_TypeOfLimit_MaxNbViews:
        return 32;
      case Graphic3d_TypeOfLimit_MaxTextureSize:
      case Graphic3d_TypeOfLimit_MaxViewDumpSizeX:
      case Graphic3d_TypeOfLimit_MaxViewDumpSizeY:
        return 4096;
      default:
        return 0;
    }
  }

  occ::handle<Graphic3d_CStructure> CreateStructure(
    const occ::handle<Graphic3d_StructureManager>& theManager) override
  {
    return new AIS_TestCStructure(theManager);
  }

  void RemoveStructure(occ::handle<Graphic3d_CStructure>& theCStructure) override
  {
    theCStructure.Nullify();
  }

  occ::handle<Graphic3d_CView> CreateView(const occ::handle<Graphic3d_StructureManager>&) override
  {
    return occ::handle<Graphic3d_CView>();
  }

  void RemoveView(const occ::handle<Graphic3d_CView>&) override {}

  void EnableVBO(const bool) override {}

  bool IsVerticalSync() const override { return false; }

  void SetVerticalSync(bool) override {}

  bool MemoryInfo(size_t& theFreeBytes, TCollection_AsciiString& theInfo) const override
  {
    theFreeBytes = 0;
    theInfo.Clear();
    return false;
  }

  float DefaultTextHeight() const override { return 12.0f; }

  void TextSize(const occ::handle<Graphic3d_CView>&,
                const char* const,
                const float,
                float& theWidth,
                float& theAscent,
                float& theDescent) const override
  {
    theWidth   = 0.0f;
    theAscent  = 0.0f;
    theDescent = 0.0f;
  }

  void InsertLayerBefore(const Graphic3d_ZLayerId,
                         const Graphic3d_ZLayerSettings&,
                         const Graphic3d_ZLayerId) override
  {
  }

  void InsertLayerAfter(const Graphic3d_ZLayerId,
                        const Graphic3d_ZLayerSettings&,
                        const Graphic3d_ZLayerId) override
  {
  }

  void RemoveZLayer(const Graphic3d_ZLayerId) override {}

  void SetZLayerSettings(const Graphic3d_ZLayerId, const Graphic3d_ZLayerSettings&) override {}

  bool ViewExists(const occ::handle<Aspect_Window>&, occ::handle<Graphic3d_CView>& theView) override
  {
    theView.Nullify();
    return false;
  }

  DEFINE_STANDARD_RTTI_INLINE(AIS_TestGraphicDriver, Graphic3d_GraphicDriver)
};

class Graphic3d_TestLayerDriver : public AIS_TestGraphicDriver
{
public:
  void InsertLayerBefore(const Graphic3d_ZLayerId        theLayerId,
                         const Graphic3d_ZLayerSettings& theSettings,
                         const Graphic3d_ZLayerId        theLayerAfter) override
  {
    Graphic3d_GraphicDriver::InsertLayerBefore(theLayerId, theSettings, theLayerAfter);
  }

  void InsertLayerAfter(const Graphic3d_ZLayerId        theLayerId,
                        const Graphic3d_ZLayerSettings& theSettings,
                        const Graphic3d_ZLayerId        theLayerBefore) override
  {
    Graphic3d_GraphicDriver::InsertLayerAfter(theLayerId, theSettings, theLayerBefore);
  }

  void RemoveZLayer(const Graphic3d_ZLayerId theLayerId) override
  {
    Graphic3d_GraphicDriver::RemoveZLayer(theLayerId);
  }

  void SetZLayerSettings(const Graphic3d_ZLayerId        theLayerId,
                         const Graphic3d_ZLayerSettings& theSettings) override
  {
    Graphic3d_GraphicDriver::SetZLayerSettings(theLayerId, theSettings);
  }

  DEFINE_STANDARD_RTTI_INLINE(Graphic3d_TestLayerDriver, AIS_TestGraphicDriver)
};

class AIS_InteractiveContextTestAccessor : public AIS_InteractiveContext
{
public:
  AIS_InteractiveContextTestAccessor(const occ::handle<V3d_Viewer>& theViewer)
      : AIS_InteractiveContext(theViewer)
  {
  }

  void AddErasedObject(const occ::handle<AIS_InteractiveObject>& theObject)
  {
    setContextToObject(theObject);
    setObjectStatus(theObject, PrsMgr_DisplayStatus_Erased, 0, -1);
  }

  bool IsSubIntensityOn(const occ::handle<AIS_InteractiveObject>& theObject) const
  {
    occ::handle<AIS_GlobalStatus> aStatus;
    return myObjects.Find(theObject, aStatus) && aStatus->IsSubIntensityOn();
  }

  DEFINE_STANDARD_RTTI_INLINE(AIS_InteractiveContextTestAccessor, AIS_InteractiveContext)
};

class PrsMgr_PresentationManagerTestAccessor : public PrsMgr_PresentationManager
{
public:
  PrsMgr_PresentationManagerTestAccessor(
    const occ::handle<Graphic3d_StructureManager>& theStructureManager)
      : PrsMgr_PresentationManager(theStructureManager)
  {
  }

  size_t ImmediateListSize() const { return myImmediateList.Size(); }

  DEFINE_STANDARD_RTTI_INLINE(PrsMgr_PresentationManagerTestAccessor, PrsMgr_PresentationManager)
};

} // namespace

class AIS_InteractiveContextTest : public testing::Test
{
protected:
  void SetUp() override
  {
    myDriver  = new AIS_TestGraphicDriver();
    myViewer  = new V3d_Viewer(myDriver);
    myContext = new AIS_InteractiveContextTestAccessor(myViewer);
  }

  occ::handle<AIS_Shape> MakeBoxObject() const
  {
    return new AIS_Shape(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape());
  }

  occ::handle<Graphic3d_GraphicDriver>            myDriver;
  occ::handle<V3d_Viewer>                         myViewer;
  occ::handle<AIS_InteractiveContextTestAccessor> myContext;
};

TEST_F(AIS_InteractiveContextTest, SubIntensityOn_NullObject_SkipsErasedObjects)
{
  occ::handle<AIS_Shape> aShape = MakeBoxObject();
  myContext->AddErasedObject(aShape);

  EXPECT_NO_THROW(myContext->SubIntensityOn(occ::handle<AIS_InteractiveObject>(), false));
  EXPECT_FALSE(myContext->IsSubIntensityOn(aShape));
}

TEST_F(AIS_InteractiveContextTest, SubIntensityOn_ErasedObject_DoesNotSetSubIntensity)
{
  occ::handle<AIS_Shape> aShape = MakeBoxObject();
  myContext->AddErasedObject(aShape);

  EXPECT_NO_THROW(myContext->SubIntensityOn(aShape, false));
  EXPECT_FALSE(myContext->IsSubIntensityOn(aShape));
}

TEST(AIS_GlobalStatusTest, SelectionModes_PreserveOrderAfterRemove)
{
  occ::handle<AIS_GlobalStatus> aStatus = new AIS_GlobalStatus();

  EXPECT_TRUE(aStatus->AddSelectionMode(0));
  EXPECT_TRUE(aStatus->AddSelectionMode(2));
  EXPECT_TRUE(aStatus->AddSelectionMode(4));
  EXPECT_FALSE(aStatus->AddSelectionMode(2));
  EXPECT_EQ(size_t(3), aStatus->NbSelectionModes());
  EXPECT_FALSE(aStatus->HasOnlySelectionMode(0));

  EXPECT_TRUE(aStatus->RemoveSelectionMode(0));
  EXPECT_FALSE(aStatus->RemoveSelectionMode(0));
  ASSERT_EQ(size_t(2), aStatus->SelectionModes().Size());
  EXPECT_EQ(2, aStatus->SelectionModes().Mode(0));
  EXPECT_EQ(4, aStatus->SelectionModes().Mode(1));
  EXPECT_TRUE(aStatus->IsSModeIn(2));
  EXPECT_FALSE(aStatus->IsSModeIn(0));
}

TEST(Graphic3d_GraphicDriverTest, ZLayers_InsertRemoveAndImmediateOrdering)
{
  occ::handle<Graphic3d_TestLayerDriver> aDriver = new Graphic3d_TestLayerDriver();

  constexpr int THE_BEFORE_DEFAULT_LAYER = 1001;
  constexpr int THE_AFTER_DEFAULT_LAYER  = 1002;

  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetImmediate(false);
  aDriver->InsertLayerBefore(THE_BEFORE_DEFAULT_LAYER, aSettings, Graphic3d_ZLayerId_Default);
  aDriver->InsertLayerAfter(THE_AFTER_DEFAULT_LAYER, aSettings, Graphic3d_ZLayerId_Default);

  NCollection_Sequence<int> aLayers;
  aDriver->ZLayers(aLayers);

  ASSERT_EQ(aLayers.Length(), 7);
  EXPECT_EQ(aLayers.Value(1), Graphic3d_ZLayerId_BotOSD);
  EXPECT_EQ(aLayers.Value(2), THE_BEFORE_DEFAULT_LAYER);
  EXPECT_EQ(aLayers.Value(3), Graphic3d_ZLayerId_Default);
  EXPECT_EQ(aLayers.Value(4), THE_AFTER_DEFAULT_LAYER);
  EXPECT_EQ(aLayers.Value(5), Graphic3d_ZLayerId_Top);
  EXPECT_EQ(aLayers.Value(6), Graphic3d_ZLayerId_Topmost);
  EXPECT_EQ(aLayers.Value(7), Graphic3d_ZLayerId_TopOSD);

  aDriver->RemoveZLayer(THE_BEFORE_DEFAULT_LAYER);
  EXPECT_THROW(aDriver->ZLayerSettings(THE_BEFORE_DEFAULT_LAYER), Standard_OutOfRange);

  Graphic3d_ZLayerSettings anImmediateSettings;
  anImmediateSettings.SetImmediate(true);
  aDriver->SetZLayerSettings(THE_AFTER_DEFAULT_LAYER, anImmediateSettings);

  aDriver->ZLayers(aLayers);

  ASSERT_EQ(aLayers.Length(), 6);
  EXPECT_EQ(aLayers.Value(1), Graphic3d_ZLayerId_BotOSD);
  EXPECT_EQ(aLayers.Value(2), Graphic3d_ZLayerId_Default);
  EXPECT_EQ(aLayers.Value(3), THE_AFTER_DEFAULT_LAYER);
  EXPECT_EQ(aLayers.Value(4), Graphic3d_ZLayerId_Top);
  EXPECT_EQ(aLayers.Value(5), Graphic3d_ZLayerId_Topmost);
  EXPECT_EQ(aLayers.Value(6), Graphic3d_ZLayerId_TopOSD);
}

TEST(V3d_ViewerTest, ZLayerIds_RemoveRejectsMissingLayer)
{
  occ::handle<Graphic3d_GraphicDriver> aDriver = new AIS_TestGraphicDriver();
  occ::handle<V3d_Viewer>              aViewer = new V3d_Viewer(aDriver);

  Graphic3d_ZLayerSettings aSettings;
  Graphic3d_ZLayerId       aLayerId = Graphic3d_ZLayerId_UNKNOWN;
  ASSERT_TRUE(aViewer->InsertLayerAfter(aLayerId, aSettings, Graphic3d_ZLayerId_Default));
  EXPECT_TRUE(aViewer->RemoveZLayer(aLayerId));
  EXPECT_FALSE(aViewer->RemoveZLayer(aLayerId));
  EXPECT_FALSE(aViewer->RemoveZLayer(Graphic3d_ZLayerId_Default));
}

TEST(AIS_SelectionTest, AddRemoveAndClear_KeepOrderAndSelectionState)
{
  occ::handle<AIS_Shape> aFirstShape  = new AIS_Shape(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape());
  occ::handle<AIS_Shape> aSecondShape = new AIS_Shape(BRepPrimAPI_MakeBox(2.0, 2.0, 2.0).Shape());
  occ::handle<SelectMgr_EntityOwner> aFirstOwner  = new SelectMgr_EntityOwner(aFirstShape);
  occ::handle<SelectMgr_EntityOwner> aSecondOwner = new SelectMgr_EntityOwner(aSecondShape);

  occ::handle<AIS_Selection> aSelection = new AIS_Selection();
  EXPECT_EQ(AIS_SS_Added, aSelection->AddSelect(aFirstOwner));
  EXPECT_EQ(AIS_SS_Added, aSelection->AddSelect(aSecondOwner));
  EXPECT_EQ(AIS_SS_NotDone, aSelection->AddSelect(aFirstOwner));
  EXPECT_EQ(2, aSelection->Extent());
  EXPECT_TRUE(aSelection->IsSelected(aFirstOwner));
  EXPECT_TRUE(aSelection->IsSelected(aSecondOwner));

  NCollection_OrderedMap<occ::handle<SelectMgr_EntityOwner>>::Iterator anOwnerIter(
    aSelection->Objects());
  ASSERT_TRUE(anOwnerIter.More());
  EXPECT_EQ(aFirstOwner, anOwnerIter.Value());
  anOwnerIter.Next();
  ASSERT_TRUE(anOwnerIter.More());
  EXPECT_EQ(aSecondOwner, anOwnerIter.Value());

  EXPECT_EQ(AIS_SS_Removed,
            aSelection->Select(aFirstOwner,
                               occ::handle<SelectMgr_Filter>(),
                               AIS_SelectionScheme_Remove,
                               false));
  EXPECT_FALSE(aFirstOwner->IsSelected());
  EXPECT_TRUE(aSecondOwner->IsSelected());
  ASSERT_EQ(1, aSelection->Extent());
  EXPECT_EQ(aSecondOwner, aSelection->Objects().First());

  aSelection->Clear();
  EXPECT_TRUE(aSelection->IsEmpty());
  EXPECT_FALSE(aSecondOwner->IsSelected());
}

TEST(SelectMgr_SelectableObjectTest, Selections_AreKeyedByModeAndPreserveOrder)
{
  occ::handle<AIS_Shape> aShape = new AIS_Shape(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape());
  occ::handle<SelectMgr_Selection> aMode0 = new SelectMgr_Selection(0);
  occ::handle<SelectMgr_Selection> aMode2 = new SelectMgr_Selection(2);

  aShape->AddSelection(aMode0, 0);
  aShape->AddSelection(aMode2, 2);

  EXPECT_EQ(aMode0, aShape->Selection(0));
  EXPECT_EQ(aMode2, aShape->Selection(2));
  ASSERT_EQ(size_t(2), aShape->Selections().Size());

  EXPECT_EQ(0, aShape->Selections().Value(0)->Mode());
  EXPECT_EQ(aMode0, aShape->Selections().Value(0));
  EXPECT_EQ(2, aShape->Selections().Value(1)->Mode());
  EXPECT_EQ(aMode2, aShape->Selections().Value(1));

  occ::handle<SelectMgr_Selection> aMode0Replacement = new SelectMgr_Selection(0);
  aShape->AddSelection(aMode0Replacement, 0);

  EXPECT_EQ(aMode0Replacement, aShape->Selection(0));
  ASSERT_EQ(size_t(2), aShape->Selections().Size());

  // Replacement appends after existing entries - mode 2 is now first, the new mode-0 last.
  EXPECT_EQ(2, aShape->Selections().Value(0)->Mode());
  EXPECT_EQ(aMode2, aShape->Selections().Value(0));
  EXPECT_EQ(0, aShape->Selections().Value(1)->Mode());
  EXPECT_EQ(aMode0Replacement, aShape->Selections().Value(1));
}

TEST(PrsMgr_PresentationManagerTest, AddToImmediateList_Deduplicates)
{
  occ::handle<Graphic3d_GraphicDriver>    aDriver = new AIS_TestGraphicDriver();
  occ::handle<Graphic3d_StructureManager> aStructureManager =
    new Graphic3d_StructureManager(aDriver);
  occ::handle<PrsMgr_PresentationManagerTestAccessor> aPresentationManager =
    new PrsMgr_PresentationManagerTestAccessor(aStructureManager);
  occ::handle<Prs3d_Presentation> aPresentation = new Prs3d_Presentation(aStructureManager);

  aPresentationManager->AddToImmediateList(aPresentation);
  EXPECT_EQ(size_t(0), aPresentationManager->ImmediateListSize());

  aPresentationManager->BeginImmediateDraw();
  aPresentationManager->AddToImmediateList(aPresentation);
  aPresentationManager->AddToImmediateList(aPresentation);
  EXPECT_EQ(size_t(1), aPresentationManager->ImmediateListSize());

  aPresentationManager->ClearImmediateDraw();
  EXPECT_EQ(size_t(0), aPresentationManager->ImmediateListSize());
}
