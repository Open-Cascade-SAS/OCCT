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
#include <sstream>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_Sequence.hxx>
#include <TDF_Label.hxx>
#include <DESTEP_Parameters.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <Quantity_Color.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

// Test OCC33657 (case 1): STEPCAFControl_Reader and STEPCAFControl_Writer constructors
// can be created and destroyed in parallel without crashing.
TEST(STEPCAFControl_ControllerTest, OCC33657_ParallelConstructors)
{
  STEPCAFControl_Controller::Init();
  EXPECT_NO_FATAL_FAILURE(OSD_Parallel::For(0, 1000, [](int) {
    STEPCAFControl_Reader aReader;
    aReader.SetColorMode(true);
    STEPCAFControl_Writer aWriter;
    aWriter.SetDimTolMode(true);
  }));
}

// Test OCC33657 (case 3): STEPControl_Writer can write in parallel to in-memory buffers.
// Each thread creates its own shape and writer to avoid shared-state issues.
TEST(STEPCAFControl_ControllerTest, OCC33657_ParallelWritersToBuffer)
{
  STEPCAFControl_Controller::Init();
  std::atomic<bool> allOk{true};
  EXPECT_NO_FATAL_FAILURE(OSD_Parallel::For(0, 100, [&](int) {
    const TopoDS_Shape aShape = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
    STEPControl_Writer  aWriter;
    aWriter.SetShapeFixParameters(DESTEP_Parameters::GetDefaultShapeFixParameters());
    aWriter.Transfer(aShape, STEPControl_StepModelType::STEPControl_AsIs, DESTEP_Parameters{});
    std::ostringstream aStream;
    aWriter.WriteStream(aStream);
    if (aStream.str().empty())
      allOk = false;
  }));
  EXPECT_TRUE(allOk);
}

// Test OCC33657 (case 2): STEPControl_Reader can read a STEP stream in parallel without crashing.
TEST(STEPCAFControl_ControllerTest, OCC33657_ParallelReadersFromStream)
{
  STEPCAFControl_Controller::Init();

  // Write a box to a STEP stream once, then read it in parallel.
  const TopoDS_Shape aShape = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  STEPControl_Writer aWriter;
  aWriter.Transfer(aShape, STEPControl_StepModelType::STEPControl_AsIs, DESTEP_Parameters{});
  std::ostringstream anOutStream;
  aWriter.WriteStream(anOutStream);
  const std::string aStepContent = anOutStream.str();
  ASSERT_FALSE(aStepContent.empty()) << "STEP content should not be empty";

  EXPECT_NO_FATAL_FAILURE(OSD_Parallel::For(0, 100, [&](int) {
    std::istringstream anInStream(aStepContent);
    STEPControl_Reader aReader;
    aReader.ReadStream("", DESTEP_Parameters{}, anInStream);
    aReader.TransferRoots();
  }));
}

// Test OCC33657 (case 4): STEPControl_Writer and STEPControl_Reader work in parallel
// and produce a shape with the same topology as the source.
TEST(STEPCAFControl_ControllerTest, OCC33657_ParallelReadersAndWriters)
{
  STEPCAFControl_Controller::Init();

  // Acquire source shape and analyze its topology.
  const TopoDS_Shape aSourceShape = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  ShapeAnalysis_ShapeContents aSourceAnalyzer;
  aSourceAnalyzer.Perform(aSourceShape);

  std::atomic<bool> allOk{true};
  EXPECT_NO_FATAL_FAILURE(OSD_Parallel::For(0, 100, [&](int) {
    if (!allOk.load(std::memory_order_relaxed))
      return;

    // Write source shape to a per-thread stream.
    STEPControl_Writer aWriter;
    aWriter.Transfer(aSourceShape, STEPControl_StepModelType::STEPControl_AsIs, DESTEP_Parameters{});
    std::ostringstream anOutStream;
    aWriter.WriteStream(anOutStream);

    // Read it back and compare topology counts.
    std::istringstream      anInStream(anOutStream.str());
    STEPControl_Reader      aReader;
    aReader.ReadStream("", DESTEP_Parameters{}, anInStream);
    aReader.TransferRoots();
    const TopoDS_Shape aResultShape = aReader.OneShape();

    ShapeAnalysis_ShapeContents aResultAnalyzer;
    aResultAnalyzer.Perform(aResultShape);

    if (aSourceAnalyzer.NbSolids() != aResultAnalyzer.NbSolids()
        || aSourceAnalyzer.NbShells() != aResultAnalyzer.NbShells()
        || aSourceAnalyzer.NbFaces() != aResultAnalyzer.NbFaces()
        || aSourceAnalyzer.NbWires() != aResultAnalyzer.NbWires()
        || aSourceAnalyzer.NbEdges() != aResultAnalyzer.NbEdges()
        || aSourceAnalyzer.NbVertices() != aResultAnalyzer.NbVertices())
    {
      allOk.store(false, std::memory_order_relaxed);
    }
  }));
  EXPECT_TRUE(allOk);
}

// Test OCC23951: STEPCAFControl_Writer can write an XCAF document with a box
// whose visibility is set to false. Verifies the write completes successfully.
TEST(STEPCAFControl_ControllerTest, OCC23951_WriteDocumentWithVisibility)
{
  STEPCAFControl_Controller::Init();

  occ::handle<TDocStd_Document> aDoc = new TDocStd_Document("dummy");
  const TopoDS_Shape             aBox = BRepPrimAPI_MakeBox(1, 1, 1).Shape();

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  TDF_Label                      aLab1      = aShapeTool->NewShape();
  aShapeTool->SetShape(aLab1, aBox);
  TDataStd_Name::Set(aLab1, "Box1");

  Quantity_Color aYellow(Quantity_NOC_YELLOW);
  XCAFDoc_DocumentTool::ColorTool(aDoc->Main())->SetColor(aLab1, aYellow, XCAFDoc_ColorGen);
  XCAFDoc_DocumentTool::ColorTool(aDoc->Main())->SetVisibility(aLab1, false);

  STEPCAFControl_Writer aWriter;
  ASSERT_TRUE(aWriter.Transfer(aDoc)) << "Transfer failed";

  std::ostringstream          aStream;
  const IFSelect_ReturnStatus aStatus = aWriter.WriteStream(aStream);
  EXPECT_EQ(aStatus, IFSelect_RetDone) << "WriteStream did not return RetDone";
  EXPECT_FALSE(aStream.str().empty()) << "Written STEP stream should not be empty";

  // Verify yellow color and INVISIBILITY entity are encoded in the STEP stream.
  // STEP writes predefined colors using DRAUGHTING_PRE_DEFINED_COLOUR, not COLOUR_RGB.
  const std::string aContent = aStream.str();
  EXPECT_NE(aContent.find("DRAUGHTING_PRE_DEFINED_COLOUR('yellow')"), std::string::npos)
    << "Yellow DRAUGHTING_PRE_DEFINED_COLOUR entity not found in STEP stream";
  EXPECT_NE(aContent.find("INVISIBILITY"), std::string::npos)
    << "INVISIBILITY entity not found in STEP stream";

  // Read back the written stream into an XCAF document and verify shape topology.
  std::istringstream            anInStream(aContent);
  occ::handle<TDocStd_Document> aReadDoc = new TDocStd_Document("dummy");
  STEPCAFControl_Reader         aCafReader;
  aCafReader.SetColorMode(true);
  ASSERT_EQ(aCafReader.ReadStream("", anInStream), IFSelect_RetDone)
    << "ReadStream failed on written STEP content";
  ASSERT_TRUE(aCafReader.Transfer(aReadDoc)) << "Transfer to XCAF document failed";

  occ::handle<XCAFDoc_ShapeTool> aReadShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aReadDoc->Main());
  NCollection_Sequence<TDF_Label> aRoots;
  aReadShapeTool->GetFreeShapes(aRoots);
  ASSERT_FALSE(aRoots.IsEmpty()) << "No shapes in read-back document";
  const TopoDS_Shape aResult = aReadShapeTool->GetShape(aRoots.First());
  ASSERT_FALSE(aResult.IsNull()) << "Read-back shape should not be null";

  ShapeAnalysis_ShapeContents aSourceAnalyzer;
  aSourceAnalyzer.Perform(aBox);
  ShapeAnalysis_ShapeContents aResultAnalyzer;
  aResultAnalyzer.Perform(aResult);
  EXPECT_EQ(aResultAnalyzer.NbSolids(),   aSourceAnalyzer.NbSolids())   << "Solid count mismatch";
  EXPECT_EQ(aResultAnalyzer.NbFaces(),    aSourceAnalyzer.NbFaces())    << "Face count mismatch";
  EXPECT_EQ(aResultAnalyzer.NbEdges(),    aSourceAnalyzer.NbEdges())    << "Edge count mismatch";
  EXPECT_EQ(aResultAnalyzer.NbVertices(), aSourceAnalyzer.NbVertices()) << "Vertex count mismatch";

  // Verify yellow color is present on faces of the read-back shape.
  occ::handle<XCAFDoc_ColorTool> aColorTool =
    XCAFDoc_DocumentTool::ColorTool(aReadDoc->Main());
  bool           aFoundColor = false;
  Quantity_Color aReadColor;
  for (TopExp_Explorer anFaceExp(aResult, TopAbs_FACE); anFaceExp.More(); anFaceExp.Next())
  {
    if (aColorTool->GetColor(anFaceExp.Current(), XCAFDoc_ColorSurf, aReadColor) ||
        aColorTool->GetColor(anFaceExp.Current(), XCAFDoc_ColorGen, aReadColor))
    {
      aFoundColor = true;
      break;
    }
  }
  if (!aFoundColor)
  {
    // If not found on faces, try on the solid shape itself
    aFoundColor = aColorTool->GetColor(aResult, XCAFDoc_ColorGen, aReadColor) ||
                  aColorTool->GetColor(aResult, XCAFDoc_ColorSurf, aReadColor);
  }
  EXPECT_TRUE(aFoundColor) << "Yellow color not found on read-back shape";
  if (aFoundColor)
  {
    EXPECT_NEAR(aReadColor.Red(),   aYellow.Red(),   0.05) << "Color red channel mismatch";
    EXPECT_NEAR(aReadColor.Green(), aYellow.Green(), 0.05) << "Color green channel mismatch";
    EXPECT_NEAR(aReadColor.Blue(),  aYellow.Blue(),  0.05) << "Color blue channel mismatch";
  }
}

// Test OCC23950: STEPCAFControl_Writer writes vertex names when vertex mode is SingleVertex.
// Verifies that the STEP stream contains the expected "Point1" name.
TEST(STEPCAFControl_ControllerTest, OCC23950_WriteDocumentWithVertexName)
{
  STEPCAFControl_Controller::Init();

  occ::handle<TDocStd_Document> aDoc = new TDocStd_Document("dummy");
  const TopoDS_Shape    aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(75, 0, 0));
  const gp_Trsf         aTrsf;
  const TopLoc_Location aLoc(aTrsf);

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());

  TDF_Label aLab1 = aShapeTool->NewShape();
  aShapeTool->SetShape(aLab1, aVertex);
  TDataStd_Name::Set(aLab1, "Point1");

  TDF_Label aAssemblyLabel = aShapeTool->NewShape();
  TDataStd_Name::Set(aAssemblyLabel, "ASSEMBLY");

  const TDF_Label aComponentLabel = aShapeTool->AddComponent(aAssemblyLabel, aLab1, aLoc);
  aShapeTool->UpdateAssemblies();

  Quantity_Color aYellow(Quantity_NOC_YELLOW);
  XCAFDoc_DocumentTool::ColorTool(aDoc->Main())->SetColor(aComponentLabel, aYellow, XCAFDoc_ColorGen);
  XCAFDoc_DocumentTool::ColorTool(aDoc->Main())->SetVisibility(aComponentLabel, false);

  // Enable writing of individual vertex names
  DESTEP_Parameters aParams;
  aParams.WriteVertexMode = DESTEP_Parameters::WriteMode_VertexMode_SingleVertex;

  STEPCAFControl_Writer aWriter;
  ASSERT_TRUE(aWriter.Transfer(aDoc, aParams)) << "Transfer failed";

  std::ostringstream          aStream;
  const IFSelect_ReturnStatus aStatus = aWriter.WriteStream(aStream);
  ASSERT_EQ(aStatus, IFSelect_RetDone) << "WriteStream did not return RetDone";

  const std::string aContent = aStream.str();
  EXPECT_FALSE(aContent.empty()) << "Written STEP stream should not be empty";
  EXPECT_NE(aContent.find("Point1"), std::string::npos)
    << "Vertex name 'Point1' not found in STEP stream";

  // Verify yellow color and INVISIBILITY entity are encoded in the STEP stream.
  // STEP writes predefined colors using DRAUGHTING_PRE_DEFINED_COLOUR, not COLOUR_RGB.
  EXPECT_NE(aContent.find("DRAUGHTING_PRE_DEFINED_COLOUR('yellow')"), std::string::npos)
    << "Yellow DRAUGHTING_PRE_DEFINED_COLOUR entity not found in STEP stream";
  EXPECT_NE(aContent.find("INVISIBILITY"), std::string::npos)
    << "INVISIBILITY entity not found in STEP stream";

  // Read back the written stream into an XCAF document and verify vertex position.
  std::istringstream            anInStream(aContent);
  occ::handle<TDocStd_Document> aReadDoc = new TDocStd_Document("dummy");
  STEPCAFControl_Reader         aCafReader;
  aCafReader.SetColorMode(true);
  ASSERT_EQ(aCafReader.ReadStream("", anInStream), IFSelect_RetDone)
    << "ReadStream failed on written STEP content";
  ASSERT_TRUE(aCafReader.Transfer(aReadDoc)) << "Transfer to XCAF document failed";

  occ::handle<XCAFDoc_ShapeTool> aReadShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aReadDoc->Main());
  NCollection_Sequence<TDF_Label> aRoots;
  aReadShapeTool->GetFreeShapes(aRoots);
  ASSERT_FALSE(aRoots.IsEmpty()) << "No shapes in read-back document";
  const TopoDS_Shape aRootShape = aReadShapeTool->GetShape(aRoots.First());
  ASSERT_FALSE(aRootShape.IsNull()) << "Read-back shape should not be null";

  TopExp_Explorer anExp(aRootShape, TopAbs_VERTEX);
  ASSERT_TRUE(anExp.More()) << "No vertex found in read-back shape";
  const gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(anExp.Current()));
  EXPECT_NEAR(aPnt.X(), 75.0, Precision::Confusion()) << "Vertex X coordinate mismatch";
  EXPECT_NEAR(aPnt.Y(),  0.0, Precision::Confusion()) << "Vertex Y coordinate mismatch";
  EXPECT_NEAR(aPnt.Z(),  0.0, Precision::Confusion()) << "Vertex Z coordinate mismatch";
}
