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

#include <atomic>
#include <thread>
#include <vector>

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
// Test NbChildren consistency with iterator
//=================================================================================================

TEST(TopoDS_TShape_Test, NbChildren_ConsistencyWithIterator)
{
  TopoDS_Builder aBuilder;

  // Create wire with edge
  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  aBuilder.Add(aWire, anEdge);

  // Get NbChildren via TShape
  int aNbViaTShape = aWire.TShape()->NbChildren();

  // Count via iterator
  int aNbViaIterator = 0;
  for (TopoDS_Iterator anIt(aWire); anIt.More(); anIt.Next())
  {
    ++aNbViaIterator;
  }

  EXPECT_EQ(aNbViaTShape, aNbViaIterator) << "NbChildren should match iterator count";
  EXPECT_EQ(aNbViaTShape, 1);
}

// Six threads each set a different flag of one TShape, released together by a barrier so their
// read-modify-write sequences overlap. With a plain word a lost update clears another thread's
// flag.
//
// Only flags that are independent of each other are used. Modified(true) also clears Checked by
// design ("when a TShape is modified it is also unchecked"), so a Modified/Checked pair cannot be
// asserted to both survive and is not part of this test.
namespace
{
// File scope, so the worker lambda reads it without capturing it: MSVC rejects an uncaptured
// constexpr local (C3493) and Clang with -Werror rejects a captured one (-Wunused-lambda-capture).
constexpr int THE_FLAG_THREADS = 6;
} // namespace

TEST(TopoDS_TShape_Test, ConcurrentFlagWritesDoNotLoseEachOther)
{
  constexpr int aRepeats = 2000;

  TopoDS_Shape               aBox    = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();
  occ::handle<TopoDS_TShape> aTShape = aBox.TShape();

  for (int aRepeat = 0; aRepeat < aRepeats; ++aRepeat)
  {
    aTShape->Free(false);
    aTShape->Locked(false);
    aTShape->Orientable(false);
    aTShape->Closed(false);
    aTShape->Infinite(false);
    aTShape->Convex(false);

    std::atomic<int>         aReady(0);
    std::vector<std::thread> aWorkers;
    for (int aWorker = 0; aWorker < THE_FLAG_THREADS; ++aWorker)
    {
      aWorkers.emplace_back([&aTShape, &aReady, aWorker]() {
        aReady.fetch_add(1);
        while (aReady.load() < THE_FLAG_THREADS)
        {
          // spin until every worker is running, so the writes below start together
        }
        switch (aWorker)
        {
          case 0:
            aTShape->Free(true);
            break;
          case 1:
            aTShape->Locked(true);
            break;
          case 2:
            aTShape->Orientable(true);
            break;
          case 3:
            aTShape->Closed(true);
            break;
          case 4:
            aTShape->Infinite(true);
            break;
          default:
            aTShape->Convex(true);
            break;
        }
      });
    }
    for (auto& aWorker : aWorkers)
    {
      aWorker.join();
    }

    ASSERT_TRUE(aTShape->Free()) << "Free was lost on repeat " << aRepeat;
    ASSERT_TRUE(aTShape->Locked()) << "Locked was lost on repeat " << aRepeat;
    ASSERT_TRUE(aTShape->Orientable()) << "Orientable was lost on repeat " << aRepeat;
    ASSERT_TRUE(aTShape->Closed()) << "Closed was lost on repeat " << aRepeat;
    ASSERT_TRUE(aTShape->Infinite()) << "Infinite was lost on repeat " << aRepeat;
    ASSERT_TRUE(aTShape->Convex()) << "Convex was lost on repeat " << aRepeat;
  }
}

// The shape type shares the same word as the flags, so a flag write must not disturb it.
TEST(TopoDS_TShape_Test, ConcurrentFlagWritesPreserveShapeType)
{
  TopoDS_Shape               aBox      = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();
  occ::handle<TopoDS_TShape> aTShape   = aBox.TShape();
  const TopAbs_ShapeEnum     aExpected = aTShape->ShapeType();

  std::vector<std::thread> aThreads;
  for (int aThreadIndex = 0; aThreadIndex < 4; ++aThreadIndex)
  {
    aThreads.emplace_back([&aTShape]() {
      for (int i = 0; i < 500; ++i)
      {
        aTShape->Modified(i % 2 == 0);
        aTShape->Checked(i % 3 == 0);
      }
    });
  }
  for (auto& aThread : aThreads)
  {
    aThread.join();
  }

  EXPECT_EQ(aExpected, aTShape->ShapeType());
}
