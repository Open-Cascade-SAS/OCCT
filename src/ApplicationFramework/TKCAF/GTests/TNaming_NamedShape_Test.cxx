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

#include <BRepPrimAPI_MakeBox.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Evolution.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

class TNaming_NamedShapeTest : public testing::Test
{
protected:
  void SetUp() override
  {
    myApp = new TDocStd_Application();
    myApp->NewDocument("BinOcaf", myDoc);
    myDoc->SetUndoLimit(10);
    myLabel = myDoc->Main().FindChild(1);
  }

  TopoDS_Shape MakeBox(double theDx, double theDy, double theDz)
  {
    return BRepPrimAPI_MakeBox(theDx, theDy, theDz).Shape();
  }

  occ::handle<TDocStd_Application> myApp;
  occ::handle<TDocStd_Document>    myDoc;
  TDF_Label                        myLabel;
};

// ===================== TNaming_Builder tests =====================

TEST_F(TNaming_NamedShapeTest, Generated_Primitive)
{
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Generated(aBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));
  EXPECT_EQ(aNS->Evolution(), TNaming_PRIMITIVE);
}

TEST_F(TNaming_NamedShapeTest, Generated_FromOld)
{
  TopoDS_Shape anOldBox = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aNewBox  = MakeBox(20.0, 20.0, 20.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Generated(anOldBox, aNewBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));
  EXPECT_EQ(aNS->Evolution(), TNaming_GENERATED);
}

TEST_F(TNaming_NamedShapeTest, Modify_Shape)
{
  TopoDS_Shape anOldBox = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aNewBox  = MakeBox(20.0, 20.0, 20.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Modify(anOldBox, aNewBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));
  EXPECT_EQ(aNS->Evolution(), TNaming_MODIFY);
}

TEST_F(TNaming_NamedShapeTest, Delete_Shape)
{
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Delete(aBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));
  EXPECT_EQ(aNS->Evolution(), TNaming_DELETE);
}

TEST_F(TNaming_NamedShapeTest, Select_Shape)
{
  TopoDS_Shape aBox     = MakeBox(10.0, 20.0, 30.0);
  TopoDS_Shape aContext = MakeBox(50.0, 50.0, 50.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Select(aBox, aContext);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));
  EXPECT_EQ(aNS->Evolution(), TNaming_SELECTED);
}

TEST_F(TNaming_NamedShapeTest, NamedShape_ReturnsAttribute)
{
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Generated(aBox);

  occ::handle<TNaming_NamedShape> aNS = aBuilder.NamedShape();
  ASSERT_FALSE(aNS.IsNull());

  // Verify it is the same attribute stored on the label
  occ::handle<TNaming_NamedShape> aNSFromLabel;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNSFromLabel));
  EXPECT_TRUE(aNS == aNSFromLabel);
}

// ===================== TNaming_NamedShape tests =====================

TEST_F(TNaming_NamedShapeTest, IsEmpty_NewAttribute)
{
  // Creating a builder attaches a NamedShape, but without adding shapes
  // we need to check emptiness on a fresh attribute.
  // The builder constructor clears the attribute, so an empty builder
  // without any shape calls leaves it empty in a sense,
  // but actually the attribute exists with no nodes.
  // We test by creating a primitive and then clearing it.
  TopoDS_Shape aBox = MakeBox(10.0, 10.0, 10.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Generated(aBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));
  EXPECT_FALSE(aNS->IsEmpty());

  aNS->Clear();
  EXPECT_TRUE(aNS->IsEmpty());
}

TEST_F(TNaming_NamedShapeTest, Evolution_ReflectsBuilder)
{
  TopoDS_Shape anOldBox = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aNewBox  = MakeBox(20.0, 20.0, 20.0);

  // Test MODIFY evolution
  TNaming_Builder aBuilder(myLabel);
  aBuilder.Modify(anOldBox, aNewBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));
  EXPECT_EQ(aNS->Evolution(), TNaming_MODIFY);

  // Rebuild with GENERATED on a new label
  TDF_Label       aLabel2 = myDoc->Main().FindChild(2);
  TNaming_Builder aBuilder2(aLabel2);
  aBuilder2.Generated(anOldBox, aNewBox);

  occ::handle<TNaming_NamedShape> aNS2;
  ASSERT_TRUE(aLabel2.FindAttribute(TNaming_NamedShape::GetID(), aNS2));
  EXPECT_EQ(aNS2->Evolution(), TNaming_GENERATED);
}

TEST_F(TNaming_NamedShapeTest, Get_ReturnsPrimitiveShape)
{
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Generated(aBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  TopoDS_Shape aResult = aNS->Get();
  EXPECT_FALSE(aResult.IsNull());
  EXPECT_TRUE(aResult.IsSame(aBox));
}

TEST_F(TNaming_NamedShapeTest, Version_SetAndGet)
{
  TopoDS_Shape aBox = MakeBox(10.0, 10.0, 10.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Generated(aBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  aNS->SetVersion(42);
  EXPECT_EQ(aNS->Version(), 42);

  aNS->SetVersion(0);
  EXPECT_EQ(aNS->Version(), 0);
}

TEST_F(TNaming_NamedShapeTest, Clear_EmptiesAttribute)
{
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Generated(aBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));
  EXPECT_FALSE(aNS->IsEmpty());

  aNS->Clear();
  EXPECT_TRUE(aNS->IsEmpty());

  // Get() on cleared attribute should return a null shape
  TopoDS_Shape aResult = aNS->Get();
  EXPECT_TRUE(aResult.IsNull());
}

// ===================== TNaming_Iterator tests =====================

TEST_F(TNaming_NamedShapeTest, Iterator_TraversesPairs)
{
  TopoDS_Shape anOldBox = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aNewBox  = MakeBox(20.0, 20.0, 20.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Generated(anOldBox, aNewBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  int aCount = 0;
  for (TNaming_Iterator anIter(aNS); anIter.More(); anIter.Next())
  {
    const TopoDS_Shape& anOld = anIter.OldShape();
    const TopoDS_Shape& aNew  = anIter.NewShape();

    EXPECT_TRUE(anOld.IsSame(anOldBox));
    EXPECT_TRUE(aNew.IsSame(aNewBox));
    ++aCount;
  }
  EXPECT_EQ(aCount, 1);
}

TEST_F(TNaming_NamedShapeTest, Iterator_IsModification)
{
  TopoDS_Shape anOldBox = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aNewBox  = MakeBox(20.0, 20.0, 20.0);

  // MODIFY evolution should report IsModification = true
  TNaming_Builder aBuilder(myLabel);
  aBuilder.Modify(anOldBox, aNewBox);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  for (TNaming_Iterator anIter(aNS); anIter.More(); anIter.Next())
  {
    EXPECT_TRUE(anIter.IsModification());
    EXPECT_EQ(anIter.Evolution(), TNaming_MODIFY);
  }

  // GENERATED evolution should report IsModification = false
  TDF_Label       aLabel2 = myDoc->Main().FindChild(2);
  TNaming_Builder aBuilder2(aLabel2);
  aBuilder2.Generated(anOldBox, aNewBox);

  occ::handle<TNaming_NamedShape> aNS2;
  ASSERT_TRUE(aLabel2.FindAttribute(TNaming_NamedShape::GetID(), aNS2));

  for (TNaming_Iterator anIter2(aNS2); anIter2.More(); anIter2.Next())
  {
    EXPECT_FALSE(anIter2.IsModification());
    EXPECT_EQ(anIter2.Evolution(), TNaming_GENERATED);
  }
}

TEST_F(TNaming_NamedShapeTest, Iterator_MultipleShapes)
{
  TopoDS_Shape aBox1 = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aBox2 = MakeBox(20.0, 20.0, 20.0);
  TopoDS_Shape aBox3 = MakeBox(30.0, 30.0, 30.0);

  TNaming_Builder aBuilder(myLabel);
  aBuilder.Generated(aBox1);
  aBuilder.Generated(aBox2);
  aBuilder.Generated(aBox3);

  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  int aCount = 0;
  for (TNaming_Iterator anIter(aNS); anIter.More(); anIter.Next())
  {
    EXPECT_TRUE(anIter.OldShape().IsNull());
    EXPECT_FALSE(anIter.NewShape().IsNull());
    ++aCount;
  }
  EXPECT_EQ(aCount, 3);
}

// ===================== TNaming_NewShapeIterator tests =====================

TEST_F(TNaming_NamedShapeTest, NewShapeIterator_FollowsModification)
{
  TopoDS_Shape aBox1 = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aBox2 = MakeBox(20.0, 20.0, 20.0);
  TopoDS_Shape aBox3 = MakeBox(30.0, 30.0, 30.0);

  // Label 1: primitive aBox1
  TDF_Label       aLabel1 = myDoc->Main().FindChild(1);
  TNaming_Builder aBuilder1(aLabel1);
  aBuilder1.Generated(aBox1);

  // Label 2: modify aBox1 -> aBox2
  TDF_Label       aLabel2 = myDoc->Main().FindChild(2);
  TNaming_Builder aBuilder2(aLabel2);
  aBuilder2.Modify(aBox1, aBox2);

  // Label 3: modify aBox2 -> aBox3
  TDF_Label       aLabel3 = myDoc->Main().FindChild(3);
  TNaming_Builder aBuilder3(aLabel3);
  aBuilder3.Modify(aBox2, aBox3);

  // Follow the modification chain from aBox1
  TNaming_NewShapeIterator anIter(aBox1, myDoc->Main());
  ASSERT_TRUE(anIter.More());
  EXPECT_TRUE(anIter.Shape().IsSame(aBox2));
  EXPECT_TRUE(anIter.IsModification());

  // Follow from aBox2
  TNaming_NewShapeIterator anIter2(aBox2, myDoc->Main());
  ASSERT_TRUE(anIter2.More());
  EXPECT_TRUE(anIter2.Shape().IsSame(aBox3));
}

TEST_F(TNaming_NamedShapeTest, NewShapeIterator_MultipleBranches)
{
  TopoDS_Shape aBox1    = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aBoxMod1 = MakeBox(20.0, 20.0, 20.0);
  TopoDS_Shape aBoxMod2 = MakeBox(30.0, 30.0, 30.0);

  // Label 1: primitive aBox1
  TDF_Label       aLabel1 = myDoc->Main().FindChild(1);
  TNaming_Builder aBuilder1(aLabel1);
  aBuilder1.Generated(aBox1);

  // Label 2: modify aBox1 -> aBoxMod1
  TDF_Label       aLabel2 = myDoc->Main().FindChild(2);
  TNaming_Builder aBuilder2(aLabel2);
  aBuilder2.Modify(aBox1, aBoxMod1);

  // Label 3: also modify aBox1 -> aBoxMod2 (split scenario)
  TDF_Label       aLabel3 = myDoc->Main().FindChild(3);
  TNaming_Builder aBuilder3(aLabel3);
  aBuilder3.Modify(aBox1, aBoxMod2);

  // NewShapeIterator from aBox1 should find both modifications
  int aCount = 0;
  for (TNaming_NewShapeIterator anIter(aBox1, myDoc->Main()); anIter.More(); anIter.Next())
  {
    const TopoDS_Shape& aNewShape = anIter.Shape();
    EXPECT_TRUE(aNewShape.IsSame(aBoxMod1) || aNewShape.IsSame(aBoxMod2));
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

// ===================== Undo/Redo tests =====================

TEST_F(TNaming_NamedShapeTest, UndoRedo_RestoresNamedShape)
{
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Open a transaction, create a named shape, commit
  myDoc->NewCommand();
  {
    TNaming_Builder aBuilder(myLabel);
    aBuilder.Generated(aBox);
  }
  myDoc->CommitCommand();

  // Verify the attribute exists
  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));
  EXPECT_FALSE(aNS->IsEmpty());
  EXPECT_TRUE(aNS->Get().IsSame(aBox));

  // Undo: the attribute should be removed
  myDoc->Undo();
  occ::handle<TNaming_NamedShape> aNSAfterUndo;
  EXPECT_FALSE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNSAfterUndo));

  // Redo: the attribute should be restored
  myDoc->Redo();
  occ::handle<TNaming_NamedShape> aNSAfterRedo;
  ASSERT_TRUE(myLabel.FindAttribute(TNaming_NamedShape::GetID(), aNSAfterRedo));
  EXPECT_FALSE(aNSAfterRedo->IsEmpty());
  EXPECT_TRUE(aNSAfterRedo->Get().IsSame(aBox));
}
