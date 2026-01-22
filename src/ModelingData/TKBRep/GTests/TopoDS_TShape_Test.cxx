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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_TShapeDispatch.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Pnt.hxx>

//=================================================================================================
// Test TShape ShapeType() returns correct type for each shape kind
//=================================================================================================

TEST(TopoDS_TShape_Test, ShapeType_AllTypes)
{
  TopoDS_Builder aBuilder;

  // Create various shapes and verify ShapeType
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  EXPECT_EQ(aVertex.TShape()->ShapeType(), TopAbs_VERTEX);

  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  EXPECT_EQ(anEdge.TShape()->ShapeType(), TopAbs_EDGE);

  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  EXPECT_EQ(aWire.TShape()->ShapeType(), TopAbs_WIRE);

  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  EXPECT_EQ(aShell.TShape()->ShapeType(), TopAbs_SHELL);

  TopoDS_Solid aSolid;
  aBuilder.MakeSolid(aSolid);
  EXPECT_EQ(aSolid.TShape()->ShapeType(), TopAbs_SOLID);

  TopoDS_CompSolid aCompSolid;
  aBuilder.MakeCompSolid(aCompSolid);
  EXPECT_EQ(aCompSolid.TShape()->ShapeType(), TopAbs_COMPSOLID);

  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  EXPECT_EQ(aCompound.TShape()->ShapeType(), TopAbs_COMPOUND);
}

//=================================================================================================
// Test TShape flag setters and getters
//=================================================================================================

TEST(TopoDS_TShape_Test, FlagSettersGetters)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_TShape* aTShape = aCompound.TShape().get();

  // Test Free flag
  EXPECT_TRUE(aTShape->Free());
  aTShape->Free(false);
  EXPECT_FALSE(aTShape->Free());
  aTShape->Free(true);
  EXPECT_TRUE(aTShape->Free());

  // Test Modified flag
  EXPECT_TRUE(aTShape->Modified());
  aTShape->Modified(false);
  EXPECT_FALSE(aTShape->Modified());

  // Test Checked flag
  EXPECT_FALSE(aTShape->Checked());
  aTShape->Checked(true);
  EXPECT_TRUE(aTShape->Checked());

  // Test that setting Modified(true) clears Checked
  aTShape->Modified(true);
  EXPECT_FALSE(aTShape->Checked()) << "Modified(true) should clear Checked flag";

  // Test Orientable flag
  EXPECT_FALSE(aTShape->Orientable()) << "Compound should not be orientable";

  // Test Closed flag
  EXPECT_FALSE(aTShape->Closed());
  aTShape->Closed(true);
  EXPECT_TRUE(aTShape->Closed());

  // Test Infinite flag
  EXPECT_FALSE(aTShape->Infinite());
  aTShape->Infinite(true);
  EXPECT_TRUE(aTShape->Infinite());

  // Test Convex flag
  EXPECT_FALSE(aTShape->Convex());
  aTShape->Convex(true);
  EXPECT_TRUE(aTShape->Convex());

  // Test Locked flag
  EXPECT_FALSE(aTShape->Locked());
  aTShape->Locked(true);
  EXPECT_TRUE(aTShape->Locked());
}

//=================================================================================================
// Test TShape NbChildren() for various shapes
//=================================================================================================

TEST(TopoDS_TShape_Test, NbChildren)
{
  TopoDS_Builder aBuilder;

  // Vertex has 0 children
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  EXPECT_EQ(aVertex.TShape()->NbChildren(), 0);

  // Edge has 2 children (vertices)
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  EXPECT_EQ(anEdge.TShape()->NbChildren(), 2);

  // Empty wire has 0 children
  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  EXPECT_EQ(aWire.TShape()->NbChildren(), 0);

  // Wire with edges
  aBuilder.Add(aWire, anEdge);
  EXPECT_EQ(aWire.TShape()->NbChildren(), 1);

  // Compound with multiple children
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  for (int i = 0; i < 10; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }
  EXPECT_EQ(aCompound.TShape()->NbChildren(), 10);
}

//=================================================================================================
// Test TShape GetChild() for various shapes
//=================================================================================================

TEST(TopoDS_TShape_Test, GetChild)
{
  TopoDS_Builder aBuilder;

  // Edge with 2 vertices
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));

  const TopoDS_Shape& aChild0 = anEdge.TShape()->GetChild(0);
  const TopoDS_Shape& aChild1 = anEdge.TShape()->GetChild(1);

  EXPECT_EQ(aChild0.ShapeType(), TopAbs_VERTEX);
  EXPECT_EQ(aChild1.ShapeType(), TopAbs_VERTEX);

  // Compound with known children
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  TopoDS_Vertex aV2 = BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0));
  TopoDS_Vertex aV3 = BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0));

  aBuilder.Add(aCompound, aV1);
  aBuilder.Add(aCompound, aV2);
  aBuilder.Add(aCompound, aV3);

  EXPECT_EQ(aCompound.TShape()->NbChildren(), 3);

  // Verify children are accessible
  for (int i = 0; i < 3; ++i)
  {
    const TopoDS_Shape& aChild = aCompound.TShape()->GetChild(i);
    EXPECT_EQ(aChild.ShapeType(), TopAbs_VERTEX);
  }
}

//=================================================================================================
// Test TShape GetChild() throws for vertex
//=================================================================================================

TEST(TopoDS_TShape_Test, GetChild_VertexThrows)
{
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  // Vertex has no children, GetChild should throw
  EXPECT_THROW(aVertex.TShape()->GetChild(0), Standard_OutOfRange);
}

//=================================================================================================
// Test TopoDS_TShapeDispatch::Apply with NbChildren
//=================================================================================================

TEST(TopoDS_TShapeDispatch_Test, Apply_NbChildren)
{
  TopoDS_Builder aBuilder;

  // Test with edge (2 vertices)
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));

  int aNbEdgeChildren =
      TopoDS_TShapeDispatch::Apply(anEdge.TShape().get(), [](auto* t) { return t->NbChildren(); });
  EXPECT_EQ(aNbEdgeChildren, 2);

  // Test with compound (5 vertices)
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  for (int i = 0; i < 5; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  int aNbCompoundChildren = TopoDS_TShapeDispatch::Apply(aCompound.TShape().get(),
                                                         [](auto* t) { return t->NbChildren(); });
  EXPECT_EQ(aNbCompoundChildren, 5);
}

//=================================================================================================
// Test TopoDS_TShapeDispatch::Apply with GetChild
//=================================================================================================

TEST(TopoDS_TShapeDispatch_Test, Apply_GetChild)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));

  // Get first child using dispatch
  TopoDS_Shape aChild = TopoDS_TShapeDispatch::Apply(
      aCompound.TShape().get(), [](auto* t) { return t->GetChild(0); });

  EXPECT_EQ(aChild.ShapeType(), TopAbs_VERTEX);
}

//=================================================================================================
// Test TopoDS_TShapeDispatch::ApplyWithType
//=================================================================================================

TEST(TopoDS_TShapeDispatch_Test, ApplyWithType)
{
  TopoDS_Builder aBuilder;
  TopoDS_Wire    aWire;
  aBuilder.MakeWire(aWire);

  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  aBuilder.Add(aWire, anEdge);

  TopoDS_TShape*   aTShape   = aWire.TShape().get();
  TopAbs_ShapeEnum aType     = aTShape->ShapeType();

  // Use ApplyWithType with cached type
  int aNbChildren =
      TopoDS_TShapeDispatch::ApplyWithType(aTShape, aType, [](auto* t) { return t->NbChildren(); });

  EXPECT_EQ(aNbChildren, 1);
}

//=================================================================================================
// Test TopoDS_TShapeDispatch handles all shape types
//=================================================================================================

TEST(TopoDS_TShapeDispatch_Test, AllShapeTypes)
{
  TopoDS_Builder aBuilder;

  // Test Edge
  {
    TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
    int         aNb = TopoDS_TShapeDispatch::Apply(anEdge.TShape().get(),
                                           [](auto* t) { return t->NbChildren(); });
    EXPECT_EQ(aNb, 2);
  }

  // Test Wire
  {
    TopoDS_Wire aWire;
    aBuilder.MakeWire(aWire);
    int aNb =
        TopoDS_TShapeDispatch::Apply(aWire.TShape().get(), [](auto* t) { return t->NbChildren(); });
    EXPECT_EQ(aNb, 0);
  }

  // Test Face (from box)
  {
    TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1, 1, 1).Shape();
    for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      const TopoDS_Shape& aFace = anExp.Current();
      int                 aNb   = TopoDS_TShapeDispatch::Apply(aFace.TShape().get(),
                                              [](auto* t) { return t->NbChildren(); });
      EXPECT_GE(aNb, 1) << "Face should have at least 1 wire";
      break; // Just test one face
    }
  }

  // Test Shell
  {
    TopoDS_Shell aShell;
    aBuilder.MakeShell(aShell);
    int aNb = TopoDS_TShapeDispatch::Apply(aShell.TShape().get(),
                                           [](auto* t) { return t->NbChildren(); });
    EXPECT_EQ(aNb, 0);
  }

  // Test Solid
  {
    TopoDS_Solid aSolid;
    aBuilder.MakeSolid(aSolid);
    int aNb = TopoDS_TShapeDispatch::Apply(aSolid.TShape().get(),
                                           [](auto* t) { return t->NbChildren(); });
    EXPECT_EQ(aNb, 0);
  }

  // Test CompSolid
  {
    TopoDS_CompSolid aCompSolid;
    aBuilder.MakeCompSolid(aCompSolid);
    int aNb = TopoDS_TShapeDispatch::Apply(aCompSolid.TShape().get(),
                                           [](auto* t) { return t->NbChildren(); });
    EXPECT_EQ(aNb, 0);
  }

  // Test Compound
  {
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound(aCompound);
    int aNb = TopoDS_TShapeDispatch::Apply(aCompound.TShape().get(),
                                           [](auto* t) { return t->NbChildren(); });
    EXPECT_EQ(aNb, 0);
  }
}

//=================================================================================================
// Test TShape EmptyCopy creates same type with no children
//=================================================================================================

TEST(TopoDS_TShape_Test, EmptyCopy)
{
  TopoDS_Builder aBuilder;

  // Create compound with children
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));

  EXPECT_EQ(aCompound.TShape()->NbChildren(), 2);

  // EmptyCopy should create same type with no children
  occ::handle<TopoDS_TShape> aCopy = aCompound.TShape()->EmptyCopy();

  EXPECT_EQ(aCopy->ShapeType(), TopAbs_COMPOUND);
  EXPECT_EQ(aCopy->NbChildren(), 0) << "EmptyCopy should have no children";
}

//=================================================================================================
// Test TShape EmptyCopy for all shape types
//=================================================================================================

TEST(TopoDS_TShape_Test, EmptyCopy_AllTypes)
{
  TopoDS_Builder aBuilder;

  // Test Edge EmptyCopy
  {
    TopoDS_Edge                anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
    occ::handle<TopoDS_TShape> aCopy  = anEdge.TShape()->EmptyCopy();
    EXPECT_EQ(aCopy->ShapeType(), TopAbs_EDGE);
    EXPECT_EQ(aCopy->NbChildren(), 0);
  }

  // Test Wire EmptyCopy
  {
    TopoDS_Wire aWire;
    aBuilder.MakeWire(aWire);
    occ::handle<TopoDS_TShape> aCopy = aWire.TShape()->EmptyCopy();
    EXPECT_EQ(aCopy->ShapeType(), TopAbs_WIRE);
  }

  // Test Shell EmptyCopy
  {
    TopoDS_Shell aShell;
    aBuilder.MakeShell(aShell);
    occ::handle<TopoDS_TShape> aCopy = aShell.TShape()->EmptyCopy();
    EXPECT_EQ(aCopy->ShapeType(), TopAbs_SHELL);
  }

  // Test Solid EmptyCopy
  {
    TopoDS_Solid aSolid;
    aBuilder.MakeSolid(aSolid);
    occ::handle<TopoDS_TShape> aCopy = aSolid.TShape()->EmptyCopy();
    EXPECT_EQ(aCopy->ShapeType(), TopAbs_SOLID);
  }

  // Test CompSolid EmptyCopy
  {
    TopoDS_CompSolid aCompSolid;
    aBuilder.MakeCompSolid(aCompSolid);
    occ::handle<TopoDS_TShape> aCopy = aCompSolid.TShape()->EmptyCopy();
    EXPECT_EQ(aCopy->ShapeType(), TopAbs_COMPSOLID);
  }
}

//=================================================================================================
// Test TShape Orientable flag for different shape types
//=================================================================================================

TEST(TopoDS_TShape_Test, Orientable_DifferentTypes)
{
  TopoDS_Builder aBuilder;

  // Edge should be orientable
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  EXPECT_TRUE(anEdge.TShape()->Orientable()) << "Edge should be orientable";

  // Compound should not be orientable
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  EXPECT_FALSE(aCompound.TShape()->Orientable()) << "Compound should not be orientable";

  // Wire should be orientable
  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  EXPECT_TRUE(aWire.TShape()->Orientable()) << "Wire should be orientable";
}

//=================================================================================================
// Test TopoDS_TShapeDispatch::Apply returns default for vertex
//=================================================================================================

TEST(TopoDS_TShapeDispatch_Test, Apply_VertexReturnsDefault)
{
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  // Dispatch on vertex should return default-constructed value (0 for int)
  int aNb = TopoDS_TShapeDispatch::Apply(aVertex.TShape().get(),
                                         [](auto* t) { return t->NbChildren(); });

  // Vertex case falls through to default which returns ReturnType{} = 0
  EXPECT_EQ(aNb, 0);
}

//=================================================================================================
// Test TopoDS_TShapeDispatch with void return type
//=================================================================================================

TEST(TopoDS_TShapeDispatch_Test, Apply_VoidReturn)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  int aVisitCount = 0;

  // Lambda with side effect, void return
  TopoDS_TShapeDispatch::Apply(aCompound.TShape().get(), [&aVisitCount](auto* t) {
    (void)t; // unused
    ++aVisitCount;
  });

  EXPECT_EQ(aVisitCount, 1) << "Lambda should be called exactly once";
}

//=================================================================================================
// Test TShape flags are independent
//=================================================================================================

TEST(TopoDS_TShape_Test, FlagsIndependent)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_TShape* aTShape = aCompound.TShape().get();

  // Set all flags
  aTShape->Closed(true);
  aTShape->Infinite(true);
  aTShape->Convex(true);
  aTShape->Locked(true);

  // Verify all are set
  EXPECT_TRUE(aTShape->Closed());
  EXPECT_TRUE(aTShape->Infinite());
  EXPECT_TRUE(aTShape->Convex());
  EXPECT_TRUE(aTShape->Locked());

  // Clear one flag, others should remain
  aTShape->Closed(false);
  EXPECT_FALSE(aTShape->Closed());
  EXPECT_TRUE(aTShape->Infinite()) << "Clearing Closed should not affect Infinite";
  EXPECT_TRUE(aTShape->Convex()) << "Clearing Closed should not affect Convex";
  EXPECT_TRUE(aTShape->Locked()) << "Clearing Closed should not affect Locked";
}

//=================================================================================================
// Test TShape GetChild out of range throws
//=================================================================================================

#ifndef No_Exception
TEST(TopoDS_TShape_Test, GetChild_OutOfRangeThrows)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add 2 children
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));

  EXPECT_EQ(aCompound.TShape()->NbChildren(), 2);

  // Index 2 is out of range (valid: 0, 1)
  EXPECT_THROW(aCompound.TShape()->GetChild(2), Standard_OutOfRange);
}
#endif

//=================================================================================================
// Test TopoDS_TShapeDispatch consistency with direct call
//=================================================================================================

TEST(TopoDS_TShapeDispatch_Test, ConsistencyWithDirectCall)
{
  TopoDS_Builder aBuilder;

  // Create wire with edge
  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  aBuilder.Add(aWire, anEdge);

  // Get NbChildren via dispatch
  int aNbViaDispatch =
      TopoDS_TShapeDispatch::Apply(aWire.TShape().get(), [](auto* t) { return t->NbChildren(); });

  // Get NbChildren via direct virtual call
  int aNbViaDirect = aWire.TShape()->NbChildren();

  EXPECT_EQ(aNbViaDispatch, aNbViaDirect) << "Dispatch should return same result as direct call";
  EXPECT_EQ(aNbViaDispatch, 1);
}
