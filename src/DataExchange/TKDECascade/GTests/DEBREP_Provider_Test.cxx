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

#include <DEBREP_ConfigurationNode.hxx>
#include <DEBREP_Provider.hxx>
#include <DE_Provider.hxx>
#include <DE_Wrapper.hxx>

#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Sequence.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <gtest/gtest.h>

#include <sstream>
#include <string>

namespace
{
static TopoDS_Shape MakeSourceShape()
{
  return BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
}

static std::string MakeAsciiBRep(const TopoDS_Shape& theShape)
{
  std::ostringstream aStream;
  BRepTools::Write(theShape, aStream);
  EXPECT_TRUE(aStream.good());
  EXPECT_FALSE(aStream.str().empty());
  return aStream.str();
}

static int CountSubShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
  {
    ++aCount;
  }
  return aCount;
}

static void ExpectSameShape(const TopoDS_Shape& theExpected, const TopoDS_Shape& theActual)
{
  ASSERT_FALSE(theExpected.IsNull());
  ASSERT_FALSE(theActual.IsNull());
  EXPECT_EQ(theActual.ShapeType(), theExpected.ShapeType());
  EXPECT_EQ(CountSubShapes(theActual, TopAbs_SOLID), CountSubShapes(theExpected, TopAbs_SOLID));
  EXPECT_EQ(CountSubShapes(theActual, TopAbs_SHELL), CountSubShapes(theExpected, TopAbs_SHELL));
  EXPECT_EQ(CountSubShapes(theActual, TopAbs_FACE), CountSubShapes(theExpected, TopAbs_FACE));
  EXPECT_EQ(CountSubShapes(theActual, TopAbs_WIRE), CountSubShapes(theExpected, TopAbs_WIRE));
  EXPECT_EQ(CountSubShapes(theActual, TopAbs_EDGE), CountSubShapes(theExpected, TopAbs_EDGE));
  EXPECT_EQ(CountSubShapes(theActual, TopAbs_VERTEX), CountSubShapes(theExpected, TopAbs_VERTEX));

  GProp_GProps anExpectedProps;
  GProp_GProps anActualProps;
  BRepGProp::VolumeProperties(theExpected, anExpectedProps);
  BRepGProp::VolumeProperties(theActual, anActualProps);
  EXPECT_NEAR(anActualProps.Mass(), anExpectedProps.Mass(), 1.0e-7);
}

static void BindWrapper(DE_Wrapper& theWrapper)
{
  EXPECT_TRUE(theWrapper.Bind(new DEBREP_ConfigurationNode()));
}
} // namespace

// de_wrapper/brep/A1: direct BRep write followed by wrapper stream read.
TEST(DEBREP_Provider_Test, DEWrapper_Brep_A1_ShapeRoundTrip)
{
  const TopoDS_Shape aFirstShape = MakeSourceShape();
  const std::string  aContent    = MakeAsciiBRep(aFirstShape);

  std::istringstream          aInput(aContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("source.brep", aInput));

  DE_Wrapper aWrapper;
  BindWrapper(aWrapper);
  TopoDS_Shape aSecondShape;
  ASSERT_TRUE(aWrapper.Read(aReadStreams, aSecondShape));
  ExpectSameShape(aFirstShape, aSecondShape);
}

// de_wrapper/brep/A2: wrapper document write followed by direct BRep read.
TEST(DEBREP_Provider_Test, DEWrapper_Brep_A2_DocumentWrite)
{
  const TopoDS_Shape               aFirstShape   = MakeSourceShape();
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument;
  anApplication->NewDocument("BinXCAF", aDocument);
  ASSERT_FALSE(aDocument.IsNull());
  XCAFDoc_DocumentTool::ShapeTool(aDocument->Main())->AddShape(aFirstShape);

  std::ostringstream           aOutput;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("document.brep", aOutput));

  DE_Wrapper aWrapper;
  BindWrapper(aWrapper);
  ASSERT_TRUE(aWrapper.Write(aWriteStreams, aDocument));
  ASSERT_FALSE(aOutput.str().empty());

  std::istringstream          aInput(aOutput.str());
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("document.brep", aInput));
  occ::handle<DEBREP_Provider> aProvider = new DEBREP_Provider(new DEBREP_ConfigurationNode());
  TopoDS_Shape                 aSecondShape;
  ASSERT_TRUE(aProvider->Read(aReadStreams, aSecondShape));
  ExpectSameShape(aFirstShape, aSecondShape);
}

// de_wrapper/brep/A3: wrapper shape write followed by wrapper document read.
TEST(DEBREP_Provider_Test, DEWrapper_Brep_A3_DocumentRead)
{
  const TopoDS_Shape           aFirstShape = MakeSourceShape();
  std::ostringstream           aOutput;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("shape.brep", aOutput));

  DE_Wrapper aWrapper;
  BindWrapper(aWrapper);
  ASSERT_TRUE(aWrapper.Write(aWriteStreams, aFirstShape));
  ASSERT_FALSE(aOutput.str().empty());

  std::istringstream          aInput(aOutput.str());
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("shape.brep", aInput));
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument;
  anApplication->NewDocument("BinXCAF", aDocument);
  ASSERT_TRUE(aWrapper.Read(aReadStreams, aDocument));

  NCollection_Sequence<TDF_Label> aLabels;
  XCAFDoc_DocumentTool::ShapeTool(aDocument->Main())->GetFreeShapes(aLabels);
  ASSERT_EQ(aLabels.Length(), 1);
  ExpectSameShape(aFirstShape,
                  XCAFDoc_DocumentTool::ShapeTool(aDocument->Main())->GetShape(aLabels.First()));
}

// de_wrapper/brep/A4: wrapper shape stream read/write/read round trip.
TEST(DEBREP_Provider_Test, DEWrapper_Brep_A4_ShapeRoundTrip)
{
  const TopoDS_Shape aFirstShape = MakeSourceShape();
  const std::string  aContent    = MakeAsciiBRep(aFirstShape);
  DE_Wrapper         aWrapper;
  BindWrapper(aWrapper);

  std::istringstream          aFirstInput(aContent);
  DE_Provider::ReadStreamList aFirstReadStreams;
  aFirstReadStreams.Append(DE_Provider::ReadStreamNode("source.brep", aFirstInput));
  TopoDS_Shape aReadShape;
  ASSERT_TRUE(aWrapper.Read(aFirstReadStreams, aReadShape));

  std::ostringstream           aOutput;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("roundtrip.brep", aOutput));
  ASSERT_TRUE(aWrapper.Write(aWriteStreams, aReadShape));

  std::istringstream          aSecondInput(aOutput.str());
  DE_Provider::ReadStreamList aSecondReadStreams;
  aSecondReadStreams.Append(DE_Provider::ReadStreamNode("roundtrip.brep", aSecondInput));
  TopoDS_Shape aSecondShape;
  ASSERT_TRUE(aWrapper.Read(aSecondReadStreams, aSecondShape));
  ExpectSameShape(aFirstShape, aSecondShape);
}
