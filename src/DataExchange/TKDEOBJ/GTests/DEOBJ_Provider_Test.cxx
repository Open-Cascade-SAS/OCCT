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

#include <gp_Pnt.hxx>
#include <DEOBJ_ConfigurationNode.hxx>
#include <DEOBJ_Provider.hxx>
#include <DE_Wrapper.hxx>
#include <BRep_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp_Explorer.hxx>
#include <Poly_Triangulation.hxx>
#include <XSControl_WorkSession.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include <gtest/gtest.h>

#include <sstream>

TEST(DEOBJ_ProviderTest, ReadsMainStreamShapeAndSession)
{
  for (const bool aUseSession : {false, true})
  {
    std::istringstream                         anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
    const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
    aNode->InternalParameters.FileCS                 = aNode->InternalParameters.SystemCS;
    aNode->InternalParameters.FileLengthUnit         = 0.001;
    const occ::handle<DE_Provider> aProvider         = new DEOBJ_Provider(aNode);
    EXPECT_TRUE(aNode->IsStreamSupported());
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("triangle.obj", anInput));
    TopoDS_Shape                       aShape;
    occ::handle<XSControl_WorkSession> aSession;
    ASSERT_TRUE(aUseSession ? aProvider->Read(aStreams, aShape, aSession)
                            : aProvider->Read(aStreams, aShape));
    ASSERT_FALSE(aShape.IsNull());
    TopExp_Explorer aFaces(aShape, TopAbs_FACE);
    ASSERT_TRUE(aFaces.More());
    TopLoc_Location                        aLocation;
    const occ::handle<Poly_Triangulation>& aMesh =
      BRep_Tool::Triangulation(TopoDS::Face(aFaces.Current()), aLocation);
    ASSERT_FALSE(aMesh.IsNull());
    EXPECT_EQ(aMesh->NbNodes(), 3);
    EXPECT_EQ(aMesh->NbTriangles(), 1);
    EXPECT_EQ(aMesh->Node(2).Distance(aMesh->Node(1)), 1.0);
    EXPECT_TRUE(aSession.IsNull());
  }
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, ReadsMainStreamDocumentAndWrapper)
{
  for (const bool aUseSession : {false, true})
  {
    std::istringstream          anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("triangle.obj", anInput));
    const occ::handle<DEOBJ_ConfigurationNode> aNode     = new DEOBJ_ConfigurationNode;
    const occ::handle<TDocStd_Document>        aDocument = new TDocStd_Document("BinXCAF");
    const occ::handle<DE_Provider>             aProvider = new DEOBJ_Provider(aNode);
    occ::handle<XSControl_WorkSession>         aSession;
    ASSERT_TRUE(aUseSession ? aProvider->Read(aStreams, aDocument, aSession)
                            : aProvider->Read(aStreams, aDocument));
    EXPECT_GT(aDocument->Main().NbChildren(), 0);
  }

  std::istringstream          anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("triangle.obj", anInput));
  DE_Wrapper aWrapper;
  ASSERT_TRUE(aWrapper.Bind(new DEOBJ_ConfigurationNode));
  TopoDS_Shape aShape;
  ASSERT_TRUE(aWrapper.Read(aStreams, aShape));
  EXPECT_FALSE(aShape.IsNull());
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, StreamShapeHonorsGroupingOption)
{
  for (const bool aCreateShapes : {false, true})
  {
    std::istringstream                         anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nv 1 1 0\n"
                                                       "g first\nf 1 2 3\ng second\nf 2 4 3\n");
    const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
    aNode->InternalParameters.ReadCreateShapes       = aCreateShapes;
    DEOBJ_Provider              aProvider(aNode);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("groups.obj", anInput));
    TopoDS_Shape aShape;
    ASSERT_TRUE(aProvider.Read(aStreams, aShape));
    int aFaceCount = 0;
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      ++aFaceCount;
    }
    EXPECT_EQ(aFaceCount, aCreateShapes ? 2 : 1);
  }
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, RejectsInvalidStreamInput)
{
  const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
  DEOBJ_Provider                             aProvider(aNode);
  DE_Provider::ReadStreamList                aStreams;
  TopoDS_Shape                               aShape;
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
  std::istringstream anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 4\n");
  aStreams.Append(DE_Provider::ReadStreamNode("invalid.obj", anInput));
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
  EXPECT_TRUE(aShape.IsNull());
  EXPECT_FALSE(aProvider.Read(aStreams, occ::handle<TDocStd_Document>()));
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, FailedStreamReadPreservesExistingShape)
{
  const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
  DEOBJ_Provider                             aProvider(aNode);
  const TopoDS_Shape anOriginal = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 2, 3)).Vertex();
  for (const char* aText : {"", "# no mesh\n", "v 0 0 0\nf 1 2 3\n"})
  {
    std::istringstream          anInput(aText);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("invalid.obj", anInput));
    TopoDS_Shape aShape = anOriginal;
    EXPECT_FALSE(aProvider.Read(aStreams, aShape));
    EXPECT_TRUE(aShape.IsSame(anOriginal));
  }
  std::istringstream anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
  anInput.setstate(std::ios::failbit);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("invalid.obj", anInput));
  TopoDS_Shape aShape = anOriginal;
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
  EXPECT_TRUE(aShape.IsSame(anOriginal));
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, InvalidMemoryLimitPreservesExistingShape)
{
  const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
  aNode->InternalParameters.ReadMemoryLimitMiB     = -2;
  DEOBJ_Provider              aProvider(aNode);
  std::istringstream          anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("triangle.obj", anInput));
  const TopoDS_Shape anOriginal = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 2, 3)).Vertex();
  TopoDS_Shape       aShape     = anOriginal;
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
  EXPECT_TRUE(aShape.IsSame(anOriginal));
}
