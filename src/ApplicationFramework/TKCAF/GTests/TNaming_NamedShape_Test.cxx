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
#include <NCollection_Array1.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Evolution.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_Tool.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

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

namespace
{
static TopoDS_Shape ShapeAt(const TopoDS_Shape& theShape,
                            const TopAbs_ShapeEnum theType,
                            const int theIndex)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aSeenShapes;
  aSeenShapes.Add(theShape);
  int anIndex = 0;
  for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
  {
    if (!aSeenShapes.Add(anExplorer.Current()))
    {
      continue;
    }
    if (++anIndex == theIndex)
    {
      return anExplorer.Current();
    }
  }
  return TopoDS_Shape();
}

static void BuildPrimitive(const TDF_Label& theLabel,
                           const TopoDS_Shape* theShapes,
                           const int theNbShapes)
{
  TNaming_Builder aBuilder(theLabel);
  for (int anIndex = 0; anIndex < theNbShapes; ++anIndex)
  {
    aBuilder.Generated(theShapes[anIndex]);
  }
}

static void BuildGenerated(const TDF_Label& theLabel,
                           const TopoDS_Shape* theOldShapes,
                           const TopoDS_Shape* theNewShapes,
                           const int theNbShapes)
{
  TNaming_Builder aBuilder(theLabel);
  for (int anIndex = 0; anIndex < theNbShapes; ++anIndex)
  {
    aBuilder.Generated(theOldShapes[anIndex], theNewShapes[anIndex]);
  }
}

static void BuildModified(const TDF_Label& theLabel,
                          const TopoDS_Shape* theOldShapes,
                          const TopoDS_Shape* theNewShapes,
                          const int theNbShapes)
{
  TNaming_Builder aBuilder(theLabel);
  for (int anIndex = 0; anIndex < theNbShapes; ++anIndex)
  {
    aBuilder.Modify(theOldShapes[anIndex], theNewShapes[anIndex]);
  }
}

static void BuildDeleted(const TDF_Label& theLabel,
                         const TopoDS_Shape* theOldShapes,
                         const int theNbShapes)
{
  TNaming_Builder aBuilder(theLabel);
  for (int anIndex = 0; anIndex < theNbShapes; ++anIndex)
  {
    aBuilder.Delete(theOldShapes[anIndex]);
  }
}

static void BuildSelected(const TDF_Label& theLabel,
                          const TopoDS_Shape* theShapes,
                          const int theNbShapes)
{
  TNaming_Builder aBuilder(theLabel);
  for (int anIndex = 0; anIndex < theNbShapes; ++anIndex)
  {
    aBuilder.Select(theShapes[anIndex], theShapes[anIndex]);
  }
}

static int CountNewShapes(const TDF_Label& theLabel)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  if (!theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
  {
    return 0;
  }

  int aCount = 0;
  for (TNaming_Iterator anIterator(aNamedShape); anIterator.More(); anIterator.Next())
  {
    if (!anIterator.NewShape().IsNull())
    {
      ++aCount;
    }
  }
  return aCount;
}

static int CountOldShapes(const TDF_Label& theLabel)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  if (!theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
  {
    return 0;
  }

  int aCount = 0;
  for (TNaming_Iterator anIterator(aNamedShape); anIterator.More(); anIterator.Next())
  {
    if (!anIterator.OldShape().IsNull())
    {
      ++aCount;
    }
  }
  return aCount;
}

static int CountAllNewShapes(const TopoDS_Shape& theShape, const TDF_Label& theAccess)
{
  int aCount = 0;
  for (TNaming_NewShapeIterator anIterator(theShape, theAccess); anIterator.More();
       anIterator.Next())
  {
    if (!anIterator.Shape().IsNull())
    {
      ++aCount;
      aCount += CountAllNewShapes(anIterator.Shape(), theAccess);
    }
  }
  return aCount;
}

static int CountAllOldShapes(const TopoDS_Shape& theShape, const TDF_Label& theAccess)
{
  occ::handle<TNaming_NamedShape> aNamedShape = TNaming_Tool::NamedShape(theShape, theAccess);
  if (aNamedShape.IsNull())
  {
    return 0;
  }

  int aCount = 0;
  for (TNaming_Iterator anIterator(aNamedShape); anIterator.More(); anIterator.Next())
  {
    if (!anIterator.OldShape().IsNull() && !anIterator.NewShape().IsNull()
        && anIterator.NewShape().IsSame(theShape))
    {
      ++aCount;
      aCount += CountAllOldShapes(anIterator.OldShape(), theAccess);
    }
  }
  return aCount;
}

static int CountAllNewShapesAtLabel(const TDF_Label& theLabel, const TDF_Label& theAccess)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  if (!theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
  {
    return 0;
  }

  int aCount = 0;
  for (TNaming_Iterator anIterator(aNamedShape); anIterator.More(); anIterator.Next())
  {
    if (!anIterator.NewShape().IsNull())
    {
      aCount += CountAllNewShapes(anIterator.NewShape(), theAccess);
    }
  }
  return aCount;
}

static int CountAllOldShapesAtLabel(const TDF_Label& theLabel, const TDF_Label& theAccess)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  if (!theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
  {
    return 0;
  }

  int aCount = 0;
  for (TNaming_Iterator anIterator(aNamedShape); anIterator.More(); anIterator.Next())
  {
    if (!anIterator.NewShape().IsNull())
    {
      aCount += CountAllOldShapes(anIterator.NewShape(), theAccess);
    }
  }
  return aCount;
}

static void CollectAllNewShapes(const TopoDS_Shape& theShape,
                                const TDF_Label& theAccess,
                                NCollection_List<TopoDS_Shape>& theShapes)
{
  for (TNaming_NewShapeIterator anIterator(theShape, theAccess); anIterator.More();
       anIterator.Next())
  {
    if (!anIterator.Shape().IsNull())
    {
      theShapes.Append(anIterator.Shape());
      CollectAllNewShapes(anIterator.Shape(), theAccess, theShapes);
    }
  }
}

static void CollectAllOldShapes(const TopoDS_Shape& theShape,
                                const TDF_Label& theAccess,
                                NCollection_List<TopoDS_Shape>& theShapes)
{
  occ::handle<TNaming_NamedShape> aNamedShape = TNaming_Tool::NamedShape(theShape, theAccess);
  if (aNamedShape.IsNull())
  {
    return;
  }

  for (TNaming_Iterator anIterator(aNamedShape); anIterator.More(); anIterator.Next())
  {
    if (!anIterator.OldShape().IsNull() && !anIterator.NewShape().IsNull()
        && anIterator.NewShape().IsSame(theShape))
    {
      theShapes.Append(anIterator.OldShape());
      CollectAllOldShapes(anIterator.OldShape(), theAccess, theShapes);
    }
  }
}

static void CollectAllNewShapesAtLabel(const TDF_Label& theLabel,
                                       const TDF_Label& theAccess,
                                       NCollection_List<TopoDS_Shape>& theShapes)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  if (!theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
  {
    return;
  }

  for (TNaming_Iterator anIterator(aNamedShape); anIterator.More(); anIterator.Next())
  {
    if (!anIterator.NewShape().IsNull())
    {
      CollectAllNewShapes(anIterator.NewShape(), theAccess, theShapes);
    }
  }
}

static void CollectAllOldShapesAtLabel(const TDF_Label& theLabel,
                                       const TDF_Label& theAccess,
                                       NCollection_List<TopoDS_Shape>& theShapes)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  if (!theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
  {
    return;
  }

  for (TNaming_Iterator anIterator(aNamedShape); anIterator.More(); anIterator.Next())
  {
    if (!anIterator.NewShape().IsNull())
    {
      CollectAllOldShapes(anIterator.NewShape(), theAccess, theShapes);
    }
  }
}

static int CountSameShapes(const TopoDS_Shape& theShape, const TDF_Label& theAccess)
{
  int aCount = 0;
  for (TNaming_SameShapeIterator anIterator(theShape, theAccess); anIterator.More();
       anIterator.Next())
  {
    if (!anIterator.Label().IsNull())
    {
      ++aCount;
    }
  }
  return aCount;
}

static int CountSameShapesInDrawResult(const TopoDS_Shape& theShape, const TDF_Label& theAccess)
{
  const int aCount = CountSameShapes(theShape, theAccess);
  // GetSameShapes concatenates its first two labels without a separator; Tcl's
  // llength consequently reports one fewer item whenever two or more labels exist.
  return aCount > 1 ? aCount - 1 : aCount;
}
} // namespace

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

// caf/named_shape/A1: SetShape is restored by redo after an undo.
TEST(TNaming_NamedShape_Test, CafNamedShape_A1_SetShapeUndoRedo)
{
  occ::handle<TDocStd_Document> aDoc = new TDocStd_Document("BinOcaf");
  aDoc->SetUndoLimit(10);
  TDF_Label                    aLabel = aDoc->Main().FindChild(2, true);
  const TopoDS_Shape           aBox = BRepPrimAPI_MakeBox(100.0, 200.0, 300.0).Shape();

  aDoc->NewCommand();
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);
  aDoc->NewCommand();
  aDoc->Undo();

  TDF_Label aAfterUndo = aDoc->Main().FindChild(2, false);
  if (!aAfterUndo.IsNull())
  {
    occ::handle<TNaming_NamedShape> aShapeAfterUndo;
    EXPECT_FALSE(aAfterUndo.FindAttribute(TNaming_NamedShape::GetID(), aShapeAfterUndo));
  }

  aDoc->Redo();
  aLabel = aDoc->Main().FindChild(2, true);
  ASSERT_FALSE(aLabel.IsNull());
  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  EXPECT_TRUE(aNamedShape->Get().IsSame(aBox));
  aDoc->ClearUndos();
  aDoc->ClearRedos();
}

// caf/nam/A2: the complete TNaming iterator graph preserves direct, recursive,
// and same-shape relationships from the DRAW case.
TEST(TNaming_NamedShape_Test, CafNamedShape_A2_IteratorGraph)
{
  occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinOcaf");
  const TDF_Label               aTestLabel = aDocument->Main().FindChild(1, true);
  const TDF_Label               anAccessLabel = aDocument->GetData()->Root();

  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(100.0, 200.0, 300.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(200.0, 300.0, 400.0).Shape();
  const TopoDS_Shape aBox3 = BRepPrimAPI_MakeBox(300.0, 400.0, 500.0).Shape();
  const TopoDS_Shape aBox4 =
    BRepPrimAPI_MakeBox(gp_Pnt(100.0, 100.0, 100.0), gp_Pnt(300.0, 400.0, 500.0)).Shape();

  TopoDS_Shape anEdges[12];
  TopoDS_Shape aBox2Faces[6];
  TopoDS_Shape aBox3Faces[6];
  TopoDS_Shape aBox4Faces[6];
  for (int anIndex = 1; anIndex <= 12; ++anIndex)
  {
    anEdges[anIndex - 1] = ShapeAt(aBox, TopAbs_EDGE, anIndex);
    ASSERT_FALSE(anEdges[anIndex - 1].IsNull());
  }
  for (int anIndex = 1; anIndex <= 6; ++anIndex)
  {
    aBox2Faces[anIndex - 1] = ShapeAt(aBox2, TopAbs_FACE, anIndex);
    aBox3Faces[anIndex - 1] = ShapeAt(aBox3, TopAbs_FACE, anIndex);
    aBox4Faces[anIndex - 1] = ShapeAt(aBox4, TopAbs_FACE, anIndex);
    ASSERT_FALSE(aBox2Faces[anIndex - 1].IsNull());
    ASSERT_FALSE(aBox3Faces[anIndex - 1].IsNull());
    ASSERT_FALSE(aBox4Faces[anIndex - 1].IsNull());
  }

  const TDF_Label aLabel1 = aTestLabel.FindChild(1, true);
  const TDF_Label aLabel11 = aLabel1.FindChild(1, true);
  const TDF_Label aLabel12 = aLabel1.FindChild(2, true);
  const TDF_Label aLabel2 = aTestLabel.FindChild(2, true);
  const TDF_Label aLabel21 = aLabel2.FindChild(1, true);
  const TDF_Label aLabel22 = aLabel2.FindChild(2, true);
  const TDF_Label aLabel3 = aTestLabel.FindChild(3, true);
  const TDF_Label aLabel31 = aLabel3.FindChild(1, true);
  const TDF_Label aLabel32 = aLabel3.FindChild(2, true);
  const TDF_Label aLabel41 = aTestLabel.FindChild(4, true).FindChild(1, true);
  const TDF_Label aLabel5 = aTestLabel.FindChild(5, true);
  const TDF_Label aLabel51 = aLabel5.FindChild(1, true);
  const TDF_Label aLabel6 = aTestLabel.FindChild(6, true);
  const TDF_Label aLabel61 = aLabel6.FindChild(1, true);

  BuildPrimitive(aLabel1, &aBox, 1);
  BuildPrimitive(aLabel11, anEdges, 1);
  BuildPrimitive(aLabel11, anEdges, 3);
  BuildPrimitive(aLabel12, anEdges + 3, 3);

  const TopoDS_Shape aGenerated2Old[] = {aBox};
  const TopoDS_Shape aGenerated2New[] = {aBox2};
  BuildGenerated(aLabel2, aGenerated2Old, aGenerated2New, 1);

  const TopoDS_Shape aGenerated21FirstOld[] = {anEdges[0]};
  const TopoDS_Shape aGenerated21FirstNew[] = {aBox2Faces[0]};
  BuildGenerated(aLabel21, aGenerated21FirstOld, aGenerated21FirstNew, 1);

  const TopoDS_Shape aGenerated22Old[] = {anEdges[1], anEdges[1], anEdges[2]};
  const TopoDS_Shape aGenerated22New[] = {aBox2Faces[1], aBox2Faces[2], aBox2Faces[3]};
  BuildGenerated(aLabel22, aGenerated22Old, aGenerated22New, 3);

  const TopoDS_Shape aGenerated21Old[] = {anEdges[2], anEdges[2], anEdges[3]};
  const TopoDS_Shape aGenerated21New[] = {aBox2Faces[0], aBox2Faces[2], aBox2Faces[3]};
  BuildGenerated(aLabel21, aGenerated21Old, aGenerated21New, 3);

  const TopoDS_Shape aModified3Old[] = {aBox2};
  const TopoDS_Shape aModified3New[] = {aBox3};
  BuildModified(aLabel3, aModified3Old, aModified3New, 1);

  const TopoDS_Shape aModified31FirstOld[] = {aBox2Faces[0]};
  const TopoDS_Shape aModified31FirstNew[] = {aBox3Faces[0]};
  BuildModified(aLabel31, aModified31FirstOld, aModified31FirstNew, 1);

  const TopoDS_Shape aModified32Old[] = {aBox2Faces[4], aBox2Faces[1], aBox2Faces[2]};
  const TopoDS_Shape aModified32New[] = {aBox3Faces[1], aBox3Faces[2], aBox3Faces[3]};
  BuildModified(aLabel32, aModified32Old, aModified32New, 3);

  const TopoDS_Shape aModified31Old[] = {aBox2Faces[2], anEdges[2], aBox2Faces[3]};
  const TopoDS_Shape aModified31New[] = {aBox3Faces[0], aBox3Faces[2], aBox3Faces[3]};
  BuildModified(aLabel31, aModified31Old, aModified31New, 3);

  const TopoDS_Shape aDeleted41FirstOld[] = {aBox3Faces[0]};
  BuildDeleted(aLabel41, aDeleted41FirstOld, 1);
  const TopoDS_Shape aDeleted41Old[] = {aBox3Faces[0], aBox3Faces[3]};
  BuildDeleted(aLabel41, aDeleted41Old, 2);

  const TopoDS_Shape aModified5Old[] = {aBox3};
  const TopoDS_Shape aModified5New[] = {aBox4};
  BuildModified(aLabel5, aModified5Old, aModified5New, 1);

  const TopoDS_Shape aModified51Old[] = {aBox3Faces[1], aBox3Faces[2]};
  const TopoDS_Shape aModified51New[] = {aBox4Faces[1], aBox4Faces[2]};
  BuildModified(aLabel51, aModified51Old, aModified51New, 2);

  const TopoDS_Shape aSelected6[] = {aBox4};
  BuildSelected(aLabel6, aSelected6, 1);
  const TopoDS_Shape aSelected61[] = {aBox4Faces[1], aBox2Faces[4]};
  BuildSelected(aLabel61, aSelected61, 2);

  NCollection_Array1<TDF_Label> aLabels(1, 14);
  aLabels.SetValue(1, aLabel1);
  aLabels.SetValue(2, aLabel11);
  aLabels.SetValue(3, aLabel12);
  aLabels.SetValue(4, aLabel2);
  aLabels.SetValue(5, aLabel21);
  aLabels.SetValue(6, aLabel22);
  aLabels.SetValue(7, aLabel3);
  aLabels.SetValue(8, aLabel31);
  aLabels.SetValue(9, aLabel32);
  aLabels.SetValue(10, aLabel41);
  aLabels.SetValue(11, aLabel5);
  aLabels.SetValue(12, aLabel51);
  aLabels.SetValue(13, aLabel6);
  aLabels.SetValue(14, aLabel61);

  const TNaming_Evolution anExpectedEvolution[] = {TNaming_PRIMITIVE,
                                                   TNaming_PRIMITIVE,
                                                   TNaming_PRIMITIVE,
                                                   TNaming_GENERATED,
                                                   TNaming_GENERATED,
                                                   TNaming_GENERATED,
                                                   TNaming_MODIFY,
                                                   TNaming_MODIFY,
                                                   TNaming_MODIFY,
                                                   TNaming_DELETE,
                                                   TNaming_MODIFY,
                                                   TNaming_MODIFY,
                                                   TNaming_SELECTED,
                                                   TNaming_SELECTED};
  const int anExpectedNew[] = {1, 3, 3, 1, 3, 3, 1, 3, 3, 0, 1, 2, 1, 2};
  const int anExpectedOld[] = {0, 0, 0, 1, 3, 3, 1, 3, 3, 2, 1, 2, 1, 2};
  const int anExpectedAllNew[] = {3, 14, 2, 2, 3, 5, 1, 1, 2, 0, 0, 0, 0, 2};
  const int anExpectedAllOld[] = {0, 0, 0, 1, 3, 3, 2, 6, 5, 0, 3, 5, 3, 2};
  for (int anIndex = 1; anIndex <= aLabels.Upper(); ++anIndex)
  {
    occ::handle<TNaming_NamedShape> aNamedShape;
    ASSERT_TRUE(aLabels.Value(anIndex).FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
    EXPECT_EQ(aNamedShape->Evolution(), anExpectedEvolution[anIndex - 1]);
    EXPECT_EQ(CountNewShapes(aLabels.Value(anIndex)), anExpectedNew[anIndex - 1]);
    EXPECT_EQ(CountOldShapes(aLabels.Value(anIndex)), anExpectedOld[anIndex - 1]);
    EXPECT_EQ(CountAllNewShapesAtLabel(aLabels.Value(anIndex), anAccessLabel),
              anExpectedAllNew[anIndex - 1]);
    EXPECT_EQ(CountAllOldShapesAtLabel(aLabels.Value(anIndex), anAccessLabel),
              anExpectedAllOld[anIndex - 1]);
  }

  NCollection_List<TopoDS_Shape> anIteratorShapes;
  CollectAllNewShapesAtLabel(aLabel11, anAccessLabel, anIteratorShapes);
  NCollection_List<TopoDS_Shape> anOldIteratorShapes;
  CollectAllOldShapesAtLabel(aLabel31, anAccessLabel, anOldIteratorShapes);
  EXPECT_EQ(anIteratorShapes.Extent(), 14);
  EXPECT_EQ(anOldIteratorShapes.Extent(), 6);
  NCollection_Array1<TopoDS_Shape> aCollectedOldShapes(1, 6);
  int                              anOldIndex = 1;
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(anOldIteratorShapes);
       anIterator.More(); anIterator.Next())
  {
    aCollectedOldShapes.SetValue(anOldIndex++, anIterator.Value());
  }
  const int anOldShapeOrder[] = {5, 6, 1, 2, 3, 4};
  NCollection_Array1<TopoDS_Shape> anOrderedOldShapes(1, 6);
  for (int anIndex = 1; anIndex <= 6; ++anIndex)
  {
    anOrderedOldShapes.SetValue(anIndex, aCollectedOldShapes.Value(anOldShapeOrder[anIndex - 1]));
  }
  NCollection_Array1<TopoDS_Shape> aShapes(1, 21);
  aShapes.SetValue(1, aBox);
  int anShapeIndex = 2;
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(anIteratorShapes);
       anIterator.More(); anIterator.Next())
  {
    ASSERT_LE(anShapeIndex, 15);
    aShapes.SetValue(anShapeIndex++, anIterator.Value());
  }
  for (NCollection_Array1<TopoDS_Shape>::Iterator anIterator(anOrderedOldShapes); anIterator.More();
       anIterator.Next())
  {
    ASSERT_LE(anShapeIndex, 21);
    aShapes.SetValue(anShapeIndex++, anIterator.Value());
  }
  ASSERT_EQ(anShapeIndex, 22);

  const int anExpectedSame[] = {1, 2, 2, 1, 3, 2, 1, 2, 1, 1, 2,
                                1, 3, 2, 1, 3, 4, 3, 4, 1, 2};
  const int anExpectedSameLabels[] = {2, 3, 3, 1, 4, 3, 2, 3, 1, 2, 3,
                                      1, 4, 3, 2, 4, 5, 4, 5, 2, 3};
  for (int anIndex = 1; anIndex <= aShapes.Upper(); ++anIndex)
  {
    EXPECT_EQ(CountSameShapes(aShapes.Value(anIndex), anAccessLabel),
              anExpectedSameLabels[anIndex - 1]);
    EXPECT_EQ(CountSameShapesInDrawResult(aShapes.Value(anIndex), anAccessLabel),
              anExpectedSame[anIndex - 1]);
  }

  const int anExpectedSamePairs[][2] = {{20, 10}, {19, 17}, {18, 16}, {18, 13},
                                        {18, 5},  {16, 13}, {16, 5},  {15, 7},
                                        {14, 6},  {14, 3},  {13, 5},  {12, 9},
                                        {11, 8},  {6, 3}};
  for (const int (&aPair)[2] : anExpectedSamePairs)
  {
    EXPECT_TRUE(aShapes.Value(aPair[0]).IsSame(aShapes.Value(aPair[1])));
  }
  for (int anFirst = 1; anFirst <= aShapes.Upper(); ++anFirst)
  {
    for (int anSecond = 1; anSecond < anFirst; ++anSecond)
    {
      if (!aShapes.Value(anFirst).IsSame(aShapes.Value(anSecond)))
      {
        continue;
      }

      bool isExpectedPair = false;
      for (const int (&aPair)[2] : anExpectedSamePairs)
      {
        if ((aPair[0] == anFirst && aPair[1] == anSecond)
            || (aPair[0] == anSecond && aPair[1] == anFirst))
        {
          isExpectedPair = true;
          break;
        }
      }
      EXPECT_TRUE(isExpectedPair);
    }
  }
}
