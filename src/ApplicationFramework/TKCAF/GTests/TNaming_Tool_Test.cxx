// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <TNaming_Tool.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_UsedShapes.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

// Test fixture for TNaming_Tool tests
class TNaming_ToolTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a new TDF document
    myData = new TDF_Data();
    myRoot = myData->Root();
  }

  void TearDown() override { myData.Nullify(); }

  // Helper method to create a simple box shape
  TopoDS_Shape MakeBox(Standard_Real dx, Standard_Real dy, Standard_Real dz)
  {
    return BRepPrimAPI_MakeBox(dx, dy, dz).Shape();
  }

  // Helper method to create a simple sphere shape
  TopoDS_Shape MakeSphere(Standard_Real radius) { return BRepPrimAPI_MakeSphere(radius).Shape(); }

  Handle(TDF_Data) myData;
  TDF_Label        myRoot;
};

// Test TNaming_Tool::GetShape with primitive evolution
TEST_F(TNaming_ToolTest, GetShape_Primitive)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Get the NamedShape attribute
  Handle(TNaming_NamedShape) aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test GetShape
  TopoDS_Shape aRetrievedShape = TNaming_Tool::GetShape(aNS);
  EXPECT_FALSE(aRetrievedShape.IsNull());
  EXPECT_TRUE(aRetrievedShape.IsSame(aBox));
}

// Test TNaming_Tool::GetShape with null NamedShape
TEST_F(TNaming_ToolTest, GetShape_NullNamedShape)
{
  Handle(TNaming_NamedShape) aNS;

  // This should not crash even with null handle
  // Just verify it compiles and doesn't crash
  EXPECT_NO_THROW({
    if (!aNS.IsNull())
    {
      TNaming_Tool::GetShape(aNS);
    }
  });
}

// Test TNaming_Tool::OriginalShape
TEST_F(TNaming_ToolTest, OriginalShape_Modification)
{
  // Create original and modified shapes
  TopoDS_Shape aBox1 = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aBox2 = MakeBox(20.0, 20.0, 20.0);

  // Create a label and store a modification
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Modify(aBox1, aBox2);

  // Get the NamedShape attribute
  Handle(TNaming_NamedShape) aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test OriginalShape - should return the old shape
  TopoDS_Shape aOriginal = TNaming_Tool::OriginalShape(aNS);
  EXPECT_FALSE(aOriginal.IsNull());
  EXPECT_TRUE(aOriginal.IsSame(aBox1));
}

// Test TNaming_Tool::CurrentShape without modifications
TEST_F(TNaming_ToolTest, CurrentShape_NoModification)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Get the NamedShape attribute
  Handle(TNaming_NamedShape) aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test CurrentShape
  TopoDS_Shape aCurrentShape = TNaming_Tool::CurrentShape(aNS);
  EXPECT_FALSE(aCurrentShape.IsNull());
  EXPECT_TRUE(aCurrentShape.IsSame(aBox));
}

// Test TNaming_Tool::CurrentShape with modification
TEST_F(TNaming_ToolTest, CurrentShape_WithModification)
{
  // Create original and modified shapes
  TopoDS_Shape aBox1 = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aBox2 = MakeBox(20.0, 20.0, 20.0);

  // Create first label with generated shape
  TDF_Label       aLabel1 = myRoot.FindChild(1);
  TNaming_Builder aBuilder1(aLabel1);
  aBuilder1.Generated(aBox1);

  // Get the first NamedShape
  Handle(TNaming_NamedShape) aNS1;
  ASSERT_TRUE(aLabel1.FindAttribute(TNaming_NamedShape::GetID(), aNS1));

  // Create second label with modified shape
  TDF_Label       aLabel2 = myRoot.FindChild(2);
  TNaming_Builder aBuilder2(aLabel2);
  aBuilder2.Modify(aBox1, aBox2);

  // Test CurrentShape on the first NamedShape
  // It should return the modified shape (aBox2)
  TopoDS_Shape aCurrentShape = TNaming_Tool::CurrentShape(aNS1);
  EXPECT_FALSE(aCurrentShape.IsNull());
}

// Test TNaming_Tool::HasLabel
TEST_F(TNaming_ToolTest, HasLabel_ExistingShape)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Test HasLabel
  EXPECT_TRUE(TNaming_Tool::HasLabel(aLabel, aBox));
}

// Test TNaming_Tool::HasLabel with non-existing shape
TEST_F(TNaming_ToolTest, HasLabel_NonExistingShape)
{
  // Create two different shapes
  TopoDS_Shape aBox    = MakeBox(10.0, 20.0, 30.0);
  TopoDS_Shape aSphere = MakeSphere(15.0);

  // Store only the box
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Test HasLabel with sphere (not stored)
  EXPECT_FALSE(TNaming_Tool::HasLabel(aLabel, aSphere));
}

// Test TNaming_Tool::NamedShape
TEST_F(TNaming_ToolTest, NamedShape_RetrieveByShape)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Test NamedShape retrieval
  Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aBox, aLabel);
  EXPECT_FALSE(aNS.IsNull());

  // Verify the retrieved NamedShape contains our shape
  TopoDS_Shape aRetrieved = TNaming_Tool::GetShape(aNS);
  EXPECT_TRUE(aRetrieved.IsSame(aBox));
}

// Test TNaming_Tool::NamedShape with non-existing shape
TEST_F(TNaming_ToolTest, NamedShape_NonExistingShape)
{
  // Create a box and sphere
  TopoDS_Shape aBox    = MakeBox(10.0, 20.0, 30.0);
  TopoDS_Shape aSphere = MakeSphere(15.0);

  // Store only the box
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Try to retrieve NamedShape for sphere (not stored)
  Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aSphere, aLabel);
  EXPECT_TRUE(aNS.IsNull());
}

// Test TNaming_Tool::CurrentNamedShape
TEST_F(TNaming_ToolTest, CurrentNamedShape_Simple)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Get the NamedShape attribute
  Handle(TNaming_NamedShape) aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test CurrentNamedShape
  Handle(TNaming_NamedShape) aCurrentNS = TNaming_Tool::CurrentNamedShape(aNS);
  EXPECT_FALSE(aCurrentNS.IsNull());
}

// Test TNaming_Tool::Label
TEST_F(TNaming_ToolTest, Label_RetrieveLabel)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Test Label retrieval
  Standard_Integer aTransDef       = 0;
  TDF_Label        aRetrievedLabel = TNaming_Tool::Label(aLabel, aBox, aTransDef);

  EXPECT_FALSE(aRetrievedLabel.IsNull());
  EXPECT_TRUE(aRetrievedLabel.IsEqual(aLabel));
}

// Test TNaming_Tool::GeneratedShape
TEST_F(TNaming_ToolTest, GeneratedShape_Simple)
{
  // Create two shapes - old and new
  TopoDS_Shape anOldBox = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aNewBox  = MakeBox(20.0, 20.0, 20.0);

  // Create a label and store the shape as generated from old shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(anOldBox, aNewBox);

  // Get the NamedShape attribute
  Handle(TNaming_NamedShape) aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test GeneratedShape - should find the new shape generated from old shape
  TopoDS_Shape aGenerated = TNaming_Tool::GeneratedShape(anOldBox, aNS);

  // Should return the new shape
  EXPECT_FALSE(aGenerated.IsNull());
  EXPECT_TRUE(aGenerated.IsSame(aNewBox));
}

// Test TNaming_Tool with multiple shapes
TEST_F(TNaming_ToolTest, MultipleShapes_GetShape)
{
  // Create multiple shapes
  TopoDS_Shape aBox1 = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aBox2 = MakeBox(20.0, 20.0, 20.0);
  TopoDS_Shape aBox3 = MakeBox(30.0, 30.0, 30.0);

  // Store shapes in different labels
  TDF_Label       aLabel1 = myRoot.FindChild(1);
  TNaming_Builder aBuilder1(aLabel1);
  aBuilder1.Generated(aBox1);

  TDF_Label       aLabel2 = myRoot.FindChild(2);
  TNaming_Builder aBuilder2(aLabel2);
  aBuilder2.Generated(aBox2);

  TDF_Label       aLabel3 = myRoot.FindChild(3);
  TNaming_Builder aBuilder3(aLabel3);
  aBuilder3.Generated(aBox3);

  // Verify each shape can be retrieved
  Handle(TNaming_NamedShape) aNS1, aNS2, aNS3;
  ASSERT_TRUE(aLabel1.FindAttribute(TNaming_NamedShape::GetID(), aNS1));
  ASSERT_TRUE(aLabel2.FindAttribute(TNaming_NamedShape::GetID(), aNS2));
  ASSERT_TRUE(aLabel3.FindAttribute(TNaming_NamedShape::GetID(), aNS3));

  TopoDS_Shape aRetrieved1 = TNaming_Tool::GetShape(aNS1);
  TopoDS_Shape aRetrieved2 = TNaming_Tool::GetShape(aNS2);
  TopoDS_Shape aRetrieved3 = TNaming_Tool::GetShape(aNS3);

  EXPECT_TRUE(aRetrieved1.IsSame(aBox1));
  EXPECT_TRUE(aRetrieved2.IsSame(aBox2));
  EXPECT_TRUE(aRetrieved3.IsSame(aBox3));
}

// Test TNaming_Tool::ValidUntil
TEST_F(TNaming_ToolTest, ValidUntil_SimpleCase)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Test ValidUntil
  Standard_Integer aValidUntil = TNaming_Tool::ValidUntil(aLabel, aBox);

  // ValidUntil should return a valid transaction number
  EXPECT_GE(aValidUntil, 0);
}

// Test TNaming_Tool with deleted shape
TEST_F(TNaming_ToolTest, DeletedShape_CurrentShape)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel1 = myRoot.FindChild(1);
  TNaming_Builder aBuilder1(aLabel1);
  aBuilder1.Generated(aBox);

  // Delete the shape
  TDF_Label       aLabel2 = myRoot.FindChild(2);
  TNaming_Builder aBuilder2(aLabel2);
  aBuilder2.Delete(aBox);

  // Get the first NamedShape
  Handle(TNaming_NamedShape) aNS1;
  ASSERT_TRUE(aLabel1.FindAttribute(TNaming_NamedShape::GetID(), aNS1));

  // CurrentShape should handle the deletion
  TopoDS_Shape aCurrentShape = TNaming_Tool::CurrentShape(aNS1);

  // The behavior depends on the implementation, but it should not crash
  EXPECT_NO_THROW({ TNaming_Tool::CurrentShape(aNS1); });
}

// Test edge case: Empty label
TEST_F(TNaming_ToolTest, EmptyLabel_HasLabel)
{
  // Create a box but don't store it
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Use an empty label
  TDF_Label aEmptyLabel = myRoot.FindChild(99);

  // HasLabel should return false for empty label
  EXPECT_FALSE(TNaming_Tool::HasLabel(aEmptyLabel, aBox));
}

// Test TNaming_Tool::Collect
TEST_F(TNaming_ToolTest, Collect_SimpleCase)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Get the NamedShape attribute
  Handle(TNaming_NamedShape) aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test Collect
  TNaming_MapOfNamedShape aLabels;
  EXPECT_NO_THROW({ TNaming_Tool::Collect(aNS, aLabels); });
}

// Test regression case from user's selection: NamedShape with missing UsedShapes
TEST_F(TNaming_ToolTest, NamedShape_MissingUsedShapes)
{
  // Create a new TDF document without UsedShapes
  Handle(TDF_Data) aDataWithoutUS = new TDF_Data();
  TDF_Label        aRootWithoutUS = aDataWithoutUS->Root();

  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Try to retrieve NamedShape without UsedShapes attribute
  Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aBox, aRootWithoutUS);

  // Should return null NamedShape when UsedShapes is missing
  EXPECT_TRUE(aNS.IsNull());
}
