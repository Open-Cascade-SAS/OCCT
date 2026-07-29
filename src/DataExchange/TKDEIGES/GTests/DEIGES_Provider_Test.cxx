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

#include <DEIGES_ConfigurationNode.hxx>
#include <DEIGES_Provider.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_Sequence.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <gtest/gtest.h>

#include <sstream>

namespace
{
occ::handle<DEIGES_Provider> createProvider()
{
  occ::handle<DEIGES_ConfigurationNode> aNode = new DEIGES_ConfigurationNode();
  return new DEIGES_Provider(aNode);
}

occ::handle<TDocStd_Document> createDocument()
{
  occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument;
  anApp->NewDocument("BinXCAF", aDocument);
  return aDocument;
}

TopoDS_Shape createBoxShape()
{
  return BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
}

int countShapeElements(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
{
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
  {
    aCount++;
  }
  return aCount;
}

bool isValidIGESContent(const std::string& theContent)
{
  return !theContent.empty() && theContent.find('S') != std::string::npos
         && theContent.find('G') != std::string::npos && theContent.find('D') != std::string::npos
         && theContent.find('P') != std::string::npos && theContent.find('T') != std::string::npos;
}

} // namespace

TEST(DEIGES_ProviderTest, BasicProperties)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  EXPECT_STREQ("IGES", aProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", aProvider->GetVendor().ToCString());
  EXPECT_FALSE(aProvider->GetNode().IsNull());
}

TEST(DEIGES_ProviderTest, StreamShapeWriteRead)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  const TopoDS_Shape                 aBox      = createBoxShape();

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("shape.igs", anOStream));

  EXPECT_TRUE(aProvider->Write(aWriteStreams, aBox));

  const std::string anIgesContent = anOStream.str();
  EXPECT_TRUE(isValidIGESContent(anIgesContent));

  std::istringstream          anIStream(anIgesContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("shape.igs", anIStream));

  TopoDS_Shape aReadShape;
  EXPECT_TRUE(aProvider->Read(aReadStreams, aReadShape));
  EXPECT_FALSE(aReadShape.IsNull());

  if (!aReadShape.IsNull())
  {
    EXPECT_GT(countShapeElements(aReadShape, TopAbs_FACE), 0);
  }
}

TEST(DEIGES_ProviderTest, StreamDocumentWriteRead)
{
  const occ::handle<DEIGES_Provider>  aProvider = createProvider();
  const occ::handle<TDocStd_Document> aDocument = createDocument();
  const TopoDS_Shape                  aBox      = createBoxShape();

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label                aShapeLabel = aShapeTool->AddShape(aBox);
  EXPECT_FALSE(aShapeLabel.IsNull());

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("document.igs", anOStream));

  EXPECT_TRUE(aProvider->Write(aWriteStreams, aDocument));

  const std::string anIgesContent = anOStream.str();
  EXPECT_TRUE(isValidIGESContent(anIgesContent));

  const occ::handle<TDocStd_Document> aReadDocument = createDocument();

  std::istringstream          anIStream(anIgesContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("document.igs", anIStream));

  EXPECT_TRUE(aProvider->Read(aReadStreams, aReadDocument));

  occ::handle<XCAFDoc_ShapeTool> aReadShapeTool = XCAFDoc_DocumentTool::ShapeTool(aReadDocument->Main());
  NCollection_Sequence<TDF_Label> aLabels;
  aReadShapeTool->GetShapes(aLabels);
  EXPECT_GT(aLabels.Length(), 0);
}