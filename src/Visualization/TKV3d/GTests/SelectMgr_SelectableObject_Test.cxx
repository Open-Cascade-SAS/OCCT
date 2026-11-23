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
#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_ViewerSelector.hxx>

TEST(SelectMgr_SelectableObjectTest, ClearOneModePreservesSlotsAndReactivation)
{
  occ::handle<AIS_Shape> anObject = new AIS_Shape(BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape());
  occ::handle<SelectMgr_ViewerSelector> aSelector = new SelectMgr_ViewerSelector;
  SelectMgr_SelectionManager            aManager(aSelector);
  aManager.Load(anObject);
  aManager.Activate(anObject, 0);
  aManager.Activate(anObject, 2);
  const occ::handle<SelectMgr_Selection> aWhole  = anObject->Selection(0);
  const occ::handle<SelectMgr_Selection> anEdges = anObject->Selection(2);
  ASSERT_FALSE(aWhole.IsNull());
  ASSERT_FALSE(anEdges.IsNull());
  ASSERT_FALSE(aWhole->IsEmpty());
  ASSERT_FALSE(anEdges->IsEmpty());
  const size_t anEdgeCount = anEdges->Entities().Size();
  aManager.Deactivate(anObject, 0);
  aManager.ClearSelectionStructures(anObject, 0);
  anObject->ClearSelection(0);
  EXPECT_EQ(anObject->Selection(0), aWhole);
  EXPECT_TRUE(aWhole->IsEmpty());
  EXPECT_EQ(aWhole->UpdateStatus(), SelectMgr_TOU_Full);
  EXPECT_EQ(aWhole->BVHUpdateStatus(), SelectMgr_TBU_Remove);
  EXPECT_EQ(anObject->Selection(2), anEdges);
  EXPECT_EQ(anEdges->Entities().Size(), anEdgeCount);
  EXPECT_TRUE(aManager.IsActivated(anObject, 2));
  aManager.Activate(anObject, 0);
  EXPECT_EQ(anObject->Selection(0), aWhole);
  EXPECT_FALSE(aWhole->IsEmpty());
  EXPECT_TRUE(aManager.IsActivated(anObject, 0));
  EXPECT_EQ(aWhole->UpdateStatus(), SelectMgr_TOU_None);
  EXPECT_EQ(aWhole->BVHUpdateStatus(), SelectMgr_TBU_None);
}

TEST(SelectMgr_SelectableObjectTest, ClearingMissingModeDoesNotCreateSelection)
{
  occ::handle<AIS_Shape> anObject = new AIS_Shape(BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape());
  anObject->RecomputePrimitives(0);
  const occ::handle<SelectMgr_Selection> aWhole = anObject->Selection(0);
  ASSERT_FALSE(aWhole.IsNull());
  const size_t aCount = anObject->Selections().Size();
  anObject->ClearSelection(123);
  EXPECT_TRUE(anObject->Selection(123).IsNull());
  EXPECT_EQ(anObject->Selections().Size(), aCount);
  EXPECT_FALSE(aWhole->IsEmpty());
}
