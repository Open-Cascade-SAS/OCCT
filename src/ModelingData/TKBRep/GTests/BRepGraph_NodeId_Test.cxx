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

#include <BRepGraph.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

#include <gtest/gtest.h>

// ============ TypedNodeId tests ============

TEST(BRepGraph_NodeIdTest, Construction_DefaultInvalid)
{
  BRepGraph_FaceId aFace;
  EXPECT_FALSE(aFace.IsValid());
  EXPECT_EQ(aFace.Index, -1);
}

TEST(BRepGraph_NodeIdTest, Construction_FromIndex)
{
  BRepGraph_FaceId aFace(5);
  EXPECT_TRUE(aFace.IsValid());
  EXPECT_EQ(aFace.Index, 5);
}

TEST(BRepGraph_NodeIdTest, TypedAliases_ConstructFromIndex)
{
  BRepGraph_FaceId aFace = BRepGraph_FaceId(3);
  EXPECT_TRUE(aFace.IsValid());
  EXPECT_EQ(aFace.Index, 3);

  BRepGraph_EdgeId anEdge = BRepGraph_EdgeId(7);
  EXPECT_EQ(anEdge.Index, 7);
}

TEST(BRepGraph_NodeIdTest, ImplicitConversion_ToNodeId)
{
  BRepGraph_FaceId aFace(5);

  // Implicit conversion to BRepGraph_NodeId.
  BRepGraph_NodeId aNodeId = aFace;
  EXPECT_EQ(aNodeId.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(aNodeId.Index, 5);
}

TEST(BRepGraph_NodeIdTest, ImplicitConversion_PassToFunction)
{
  // Typed ids work with existing APIs that take BRepGraph_NodeId.
  BRepGraph aGraph;
  aGraph.Clear(); [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 = BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_FaceId aFace(0);
  // AdjacentFaces takes BRepGraph_FaceId - typed id works directly.
  NCollection_DynamicArray<BRepGraph_FaceId> aAdj =
    aGraph.Topo().Faces().Adjacent(aFace, aGraph.Allocator());
  EXPECT_GT(aAdj.Length(), 0);
}

TEST(BRepGraph_NodeIdTest, FromNodeId_CorrectKind)
{
  BRepGraph_NodeId aNodeId = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 3);
  BRepGraph_EdgeId anEdge  = BRepGraph_EdgeId::FromNodeId(aNodeId);
  EXPECT_EQ(anEdge.Index, 3);
}

TEST(BRepGraph_NodeIdTest, Comparison_TypedVsTyped)
{
  BRepGraph_FaceId aFace1(3);
  BRepGraph_FaceId aFace2(3);
  BRepGraph_FaceId aFace3(5);

  EXPECT_EQ(aFace1, aFace2);
  EXPECT_NE(aFace1, aFace3);
}

TEST(BRepGraph_NodeIdTest, Comparison_TypedVsNodeId)
{
  BRepGraph_FaceId aFace(3);
  BRepGraph_NodeId aMatch(BRepGraph_NodeId::Kind::Face, 3);
  BRepGraph_NodeId aDiffKind(BRepGraph_NodeId::Kind::Edge, 3);
  BRepGraph_NodeId aDiffIdx(BRepGraph_NodeId::Kind::Face, 5);

  EXPECT_EQ(aFace, aMatch);
  EXPECT_EQ(aMatch, aFace);
  EXPECT_NE(aFace, aDiffKind);
  EXPECT_NE(aFace, aDiffIdx);
}

TEST(BRepGraph_NodeIdTest, Hash_ConsistentWithNodeId)
{
  BRepGraph_FaceId aFace(5);
  BRepGraph_NodeId aNodeId = aFace;

  std::hash<BRepGraph_FaceId> aTypedHash;
  std::hash<BRepGraph_NodeId> aNodeHash;

  EXPECT_EQ(aTypedHash(aFace), aNodeHash(aNodeId));
}

TEST(BRepGraph_NodeIdTest, UntypedArithmetic_PreservesKindAndIndex)
{
  BRepGraph_NodeId aNodeId(BRepGraph_NodeId::Kind::Face, 2);

  const BRepGraph_NodeId aPrev = aNodeId++;
  EXPECT_EQ(aPrev.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(aPrev.Index, 2);
  EXPECT_EQ(aNodeId.Index, 3);

  ++aNodeId;
  EXPECT_EQ(aNodeId.Index, 4);

  const BRepGraph_NodeId anAdvanced = aNodeId + 3;
  EXPECT_EQ(anAdvanced.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anAdvanced.Index, 7);

  const BRepGraph_NodeId aRetreated = anAdvanced - 5;
  EXPECT_EQ(aRetreated.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(aRetreated.Index, 2);
}

TEST(BRepGraph_NodeIdTest, TypedArithmetic_PreservesKindAndIndex)
{
  BRepGraph_FaceId aFaceId(2);

  const BRepGraph_FaceId aPrev = aFaceId++;
  EXPECT_EQ(aPrev.Index, 2);
  EXPECT_EQ(aFaceId.Index, 3);

  ++aFaceId;
  EXPECT_EQ(aFaceId.Index, 4);

  const BRepGraph_FaceId anAdvanced = aFaceId + 3;
  EXPECT_EQ(anAdvanced.Index, 7);

  const BRepGraph_FaceId aRetreated = anAdvanced - 5;
  EXPECT_EQ(aRetreated.Index, 2);

  // Verify kind is preserved through implicit conversion.
  const BRepGraph_NodeId aNodeId = anAdvanced;
  EXPECT_EQ(aNodeId.NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(aNodeId.Index, 7);
}

TEST(BRepGraph_NodeIdTest, TypedArithmetic_IndexZeroBoundary)
{
  BRepGraph_EdgeId anEdge(0);
  EXPECT_TRUE(anEdge.IsValid());

  // Increment from zero.
  ++anEdge;
  EXPECT_EQ(anEdge.Index, 1);

  // Retreat back to zero  still valid.
  const BRepGraph_EdgeId aZero = anEdge - 1;
  EXPECT_EQ(aZero.Index, 0);
  EXPECT_TRUE(aZero.IsValid());

  // Subtract to -1  produces invalid id (allowed by constructor).
  const BRepGraph_EdgeId anInvalid = aZero - 1;
  EXPECT_EQ(anInvalid.Index, -1);
  EXPECT_FALSE(anInvalid.IsValid());
}
