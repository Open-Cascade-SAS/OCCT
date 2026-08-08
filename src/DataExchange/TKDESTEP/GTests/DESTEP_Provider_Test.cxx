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

#include <DESTEP_Provider.hxx>
#include <DESTEP_ConfigurationNode.hxx>
#include <DE_Wrapper.hxx>

#include <BOPAlgo_BOP.hxx>
#include <BOPAlgo_CheckerSI.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepGProp.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <GProp_GProps.hxx>
#include <TDocStd_Document.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDocStd_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSControl_WorkSession.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <gtest/gtest.h>

namespace
{
struct TriangulationStats
{
  int myFaces     = 0;
  int myNodes     = 0;
  int myTriangles = 0;
};

TriangulationStats getTriangulationStats(const TopoDS_Shape& theShape)
{
  TriangulationStats aStats;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ++aStats.myFaces;
    TopLoc_Location                       aLocation;
    const occ::handle<Poly_Triangulation> aTriangulation =
      BRep_Tool::Triangulation(TopoDS::Face(anExplorer.Current()), aLocation);
    if (!aTriangulation.IsNull())
    {
      aStats.myNodes += aTriangulation->NbNodes();
      aStats.myTriangles += aTriangulation->NbTriangles();
    }
  }
  return aStats;
}

bool writeReadTessellatedStep(const TopoDS_Shape&                         theSource,
                              const DESTEP_Parameters::RWMode_Tessellated theWriteMode,
                              const DESTEP_Parameters::RWMode_Tessellated theReadMode,
                              TopoDS_Shape&                               theResult,
                              std::string&                                theContent)
{
  occ::handle<DESTEP_ConfigurationNode> aWriteNode = new DESTEP_ConfigurationNode();
  aWriteNode->InternalParameters.WriteSchema = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  aWriteNode->InternalParameters.WriteTessellated = theWriteMode;
  occ::handle<DESTEP_Provider> aWriteProvider     = new DESTEP_Provider(aWriteNode);

  std::ostringstream           aWriteStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("tessellated.step", aWriteStream));
  if (!aWriteProvider->Write(aWriteStreams, theSource))
  {
    return false;
  }
  theContent = aWriteStream.str();

  occ::handle<DESTEP_ConfigurationNode> aReadNode = new DESTEP_ConfigurationNode();
  aReadNode->InternalParameters.ReadTessellated   = theReadMode;
  occ::handle<DESTEP_Provider> aReadProvider      = new DESTEP_Provider(aReadNode);

  std::istringstream          aReadStream(theContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("tessellated.step", aReadStream));
  return aReadProvider->Read(aReadStreams, theResult);
}
} // namespace

class DESTEP_ProviderTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Initialize provider with default configuration
    occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
    myProvider                                  = new DESTEP_Provider(aNode);

    // Create test BRep shapes (perfect for STEP format)
    myBox      = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
    mySphere   = BRepPrimAPI_MakeSphere(5.0).Shape();
    myCylinder = BRepPrimAPI_MakeCylinder(3.0, 8.0).Shape();

    // Create test document
    occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
    anApp->NewDocument("BinXCAF", myDocument);
  }

  void TearDown() override
  {
    myProvider.Nullify();
    myDocument.Nullify();
  }

  // Helper method to count shape elements
  int CountShapeElements(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
  {
    int aCount = 0;
    for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
    {
      aCount++;
    }
    return aCount;
  }

  // Helper method to validate STEP content
  bool IsValidSTEPContent(const std::string& theContent)
  {
    return !theContent.empty() && theContent.find("ISO-10303-21;") != std::string::npos
           && theContent.find("HEADER;") != std::string::npos
           && theContent.find("DATA;") != std::string::npos
           && theContent.find("ENDSEC;") != std::string::npos;
  }

protected:
  occ::handle<DESTEP_Provider>  myProvider;
  TopoDS_Shape                  myBox;
  TopoDS_Shape                  mySphere;
  TopoDS_Shape                  myCylinder;
  occ::handle<TDocStd_Document> myDocument;
};

// Test basic provider creation and format/vendor information
TEST_F(DESTEP_ProviderTest, BasicProperties)
{
  EXPECT_STREQ("STEP", myProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", myProvider->GetVendor().ToCString());
  EXPECT_FALSE(myProvider->GetNode().IsNull());
}

// Test stream-based shape write and read operations
TEST_F(DESTEP_ProviderTest, StreamShapeWriteRead)
{
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("test.step", anOStream));

  // Write box shape to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myBox));

  std::string aStepContent = anOStream.str();
  EXPECT_FALSE(aStepContent.empty());
  EXPECT_TRUE(IsValidSTEPContent(aStepContent));

  if (!aStepContent.empty())
  {
    // Read back from stream
    std::istringstream          anIStream(aStepContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("test.step", anIStream));

    TopoDS_Shape aReadShape;
    EXPECT_TRUE(myProvider->Read(aReadStreams, aReadShape));
    EXPECT_FALSE(aReadShape.IsNull());

    if (!aReadShape.IsNull())
    {
      // STEP should preserve solid geometry
      int aReadSolids     = CountShapeElements(aReadShape, TopAbs_SOLID);
      int aOriginalSolids = CountShapeElements(myBox, TopAbs_SOLID);
      EXPECT_EQ(aReadSolids, aOriginalSolids);
    }
  }
}

// Migrated from tests/de_mesh/step_write/B1.  The DRAW suite writes and reads
// a meshed box with AP242 tessellation enabled.
TEST_F(DESTEP_ProviderTest, MeshStep_B1_MeshedBox)
{
  const TopoDS_Shape       aSource = BRepPrimAPI_MakeBox(1., 1., 1.).Shape();
  BRepMesh_IncrementalMesh aMesh(aSource, 0.1);
  (void)aMesh;

  const TriangulationStats aSourceStats = getTriangulationStats(aSource);
  EXPECT_EQ(aSourceStats.myFaces, 6);
  EXPECT_EQ(aSourceStats.myNodes, 24);
  EXPECT_EQ(aSourceStats.myTriangles, 12);

  TopoDS_Shape aResult;
  std::string  aContent;
  ASSERT_TRUE(writeReadTessellatedStep(aSource,
                                       DESTEP_Parameters::RWMode_Tessellated_On,
                                       DESTEP_Parameters::RWMode_Tessellated_On,
                                       aResult,
                                       aContent));
  ASSERT_TRUE(IsValidSTEPContent(aContent));

  const TriangulationStats aResultStats = getTriangulationStats(aResult);
  EXPECT_EQ(aResultStats.myFaces, 6);
  EXPECT_EQ(aResultStats.myNodes, 24);
  EXPECT_EQ(aResultStats.myTriangles, 12);
}

// Migrated from tests/de_mesh/step_write/B2.
TEST_F(DESTEP_ProviderTest, MeshStep_B2_MeshedBoxWithVoid)
{
  const TopoDS_Shape anOuterBox = BRepPrimAPI_MakeBox(gp_Pnt(-5., -5., -5.), 10., 10., 10.).Shape();
  const TopoDS_Shape anInnerBox = BRepPrimAPI_MakeBox(gp_Pnt(-3., -3., -3.), 6., 6., 6.).Shape();
  const TopoDS_Shape aSource    = BRepAlgoAPI_Cut(anOuterBox, anInnerBox).Shape();
  ASSERT_FALSE(aSource.IsNull());
  BRepMesh_IncrementalMesh aMesh(aSource, 0.1);
  (void)aMesh;

  const TriangulationStats aSourceStats = getTriangulationStats(aSource);
  EXPECT_EQ(aSourceStats.myFaces, 12);
  EXPECT_EQ(aSourceStats.myNodes, 48);
  EXPECT_EQ(aSourceStats.myTriangles, 24);

  TopoDS_Shape aResult;
  std::string  aContent;
  ASSERT_TRUE(writeReadTessellatedStep(aSource,
                                       DESTEP_Parameters::RWMode_Tessellated_On,
                                       DESTEP_Parameters::RWMode_Tessellated_On,
                                       aResult,
                                       aContent));
  ASSERT_TRUE(IsValidSTEPContent(aContent));

  const TriangulationStats aResultStats = getTriangulationStats(aResult);
  EXPECT_EQ(aResultStats.myFaces, 12);
  EXPECT_EQ(aResultStats.myNodes, 48);
  EXPECT_EQ(aResultStats.myTriangles, 24);
}

// Migrated from tests/de_mesh/step_write/C1.  tclean -geom removes the BRep
// surfaces and curves but keeps the polygonal representation for STEP export.
TEST_F(DESTEP_ProviderTest, MeshStep_C1_MeshWithoutBRepGeometry)
{
  const TopoDS_Shape       aSource = BRepPrimAPI_MakeBox(1., 1., 1.).Shape();
  BRepMesh_IncrementalMesh aMesh(aSource, 0.1);
  (void)aMesh;
  BRepTools::CleanGeometry(aSource);

  const TriangulationStats aSourceStats = getTriangulationStats(aSource);
  EXPECT_EQ(aSourceStats.myFaces, 6);
  EXPECT_EQ(aSourceStats.myNodes, 24);
  EXPECT_EQ(aSourceStats.myTriangles, 12);

  TopoDS_Shape aResult;
  std::string  aContent;
  ASSERT_TRUE(writeReadTessellatedStep(aSource,
                                       DESTEP_Parameters::RWMode_Tessellated_On,
                                       DESTEP_Parameters::RWMode_Tessellated_On,
                                       aResult,
                                       aContent));
  ASSERT_TRUE(IsValidSTEPContent(aContent));

  const TriangulationStats aResultStats = getTriangulationStats(aResult);
  EXPECT_EQ(aResultStats.myFaces, 6);
  EXPECT_EQ(aResultStats.myNodes, 24);
  EXPECT_EQ(aResultStats.myTriangles, 12);
}

// Migrated from tests/de_mesh/step_write/C2.  OnNoBRep writes the tessellated
// representation without the analytic BRep representation.
TEST_F(DESTEP_ProviderTest, MeshStep_C2_TessellatedWithoutBRep)
{
  const TopoDS_Shape       aSource = BRepPrimAPI_MakeBox(1., 1., 1.).Shape();
  BRepMesh_IncrementalMesh aMesh(aSource, 0.1);
  (void)aMesh;

  const TriangulationStats aSourceStats = getTriangulationStats(aSource);
  EXPECT_EQ(aSourceStats.myFaces, 6);
  EXPECT_EQ(aSourceStats.myNodes, 24);
  EXPECT_EQ(aSourceStats.myTriangles, 12);

  TopoDS_Shape aResult;
  std::string  aContent;
  ASSERT_TRUE(writeReadTessellatedStep(aSource,
                                       DESTEP_Parameters::RWMode_Tessellated_OnNoBRep,
                                       DESTEP_Parameters::RWMode_Tessellated_On,
                                       aResult,
                                       aContent));
  ASSERT_TRUE(IsValidSTEPContent(aContent));

  const TriangulationStats aResultStats = getTriangulationStats(aResult);
  EXPECT_EQ(aResultStats.myFaces, 6);
  EXPECT_EQ(aResultStats.myNodes, 0);
  EXPECT_EQ(aResultStats.myTriangles, 0);
}

// Migrated from tests/de_mesh/step_read/B1.  The read-side OnNoBRep mode
// intentionally restores only the tessellated representation.
TEST_F(DESTEP_ProviderTest, MeshStep_Read_B1_TessellatedWithoutBRep)
{
  const TopoDS_Shape aSource = BRepPrimAPI_MakeBox(gp_Pnt(-5., -5., -5.), 10., 10., 10.).Shape();
  BRepMesh_IncrementalMesh aMesh(aSource, 0.1);
  (void)aMesh;

  const TriangulationStats aSourceStats = getTriangulationStats(aSource);
  EXPECT_EQ(aSourceStats.myFaces, 6);
  EXPECT_EQ(aSourceStats.myNodes, 24);
  EXPECT_EQ(aSourceStats.myTriangles, 12);

  TopoDS_Shape aResult;
  std::string  aContent;
  ASSERT_TRUE(writeReadTessellatedStep(aSource,
                                       DESTEP_Parameters::RWMode_Tessellated_On,
                                       DESTEP_Parameters::RWMode_Tessellated_OnNoBRep,
                                       aResult,
                                       aContent));
  ASSERT_TRUE(IsValidSTEPContent(aContent));

  const TriangulationStats aResultStats = getTriangulationStats(aResult);
  EXPECT_EQ(aResultStats.myFaces, 6);
  EXPECT_EQ(aResultStats.myNodes, 0);
  EXPECT_EQ(aResultStats.myTriangles, 0);
}

// bugs/step/bug25989: a self-generated BOP result keeps its volume through a
// STEP round-trip.  The DRAW file used stepwrite/writeall/stepread; this test
// uses the stream API of DESTEP_Provider for the same shape transfer.
TEST_F(DESTEP_ProviderTest, StepBug_25989_FusedShapeVolumeRoundTrip)
{
  const TopoDS_Shape aBox      = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(2.0, 10.0).Shape();
  const TopoDS_Shape aSphere   = BRepPrimAPI_MakeSphere(5.0).Shape();
  const TopoDS_Shape aTorus    = BRepPrimAPI_MakeTorus(10.0, 2.0).Shape();

  NCollection_List<TopoDS_Shape> aPaveArguments;
  aPaveArguments.Append(aBox);
  aPaveArguments.Append(aCylinder);
  aPaveArguments.Append(aSphere);
  aPaveArguments.Append(aTorus);

  BOPAlgo_PaveFiller aPaveFiller;
  aPaveFiller.SetArguments(aPaveArguments);
  aPaveFiller.SetRunParallel(false);
  aPaveFiller.Perform();
  ASSERT_FALSE(aPaveFiller.HasErrors());

  NCollection_List<TopoDS_Shape> aObjects;
  aObjects.Append(aBox);
  NCollection_List<TopoDS_Shape> aTools;
  aTools.Append(aCylinder);
  aTools.Append(aSphere);
  aTools.Append(aTorus);

  BOPAlgo_BOP aFusion;
  aFusion.SetArguments(aObjects);
  aFusion.SetTools(aTools);
  aFusion.SetOperation(BOPAlgo_FUSE);
  aFusion.SetRunParallel(false);
  aFusion.PerformWithFiller(aPaveFiller);
  ASSERT_FALSE(aFusion.HasErrors());
  const TopoDS_Shape aFusionShape = aFusion.Shape();
  ASSERT_FALSE(aFusionShape.IsNull());
  ASSERT_TRUE(BRepCheck_Analyzer(aFusionShape).IsValid());

  BOPAlgo_CheckerSI              aSourceChecker;
  NCollection_List<TopoDS_Shape> aSourceCheckArguments;
  aSourceCheckArguments.Append(aFusionShape);
  aSourceChecker.SetArguments(aSourceCheckArguments);
  aSourceChecker.SetLevelOfCheck(9);
  aSourceChecker.SetRunParallel(false);
  aSourceChecker.Perform();
  ASSERT_FALSE(aSourceChecker.HasErrors());

  GProp_GProps aSourceVolume;
  BRepGProp::VolumeProperties(aFusionShape, aSourceVolume);

  std::ostringstream           aWriteStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("bug25989.step", aWriteStream));
  ASSERT_TRUE(myProvider->Write(aWriteStreams, aFusionShape));
  ASSERT_FALSE(aWriteStream.str().empty());

  std::istringstream          aReadStream(aWriteStream.str());
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("bug25989.step", aReadStream));
  TopoDS_Shape aRestoredShape;
  ASSERT_TRUE(myProvider->Read(aReadStreams, aRestoredShape));
  ASSERT_FALSE(aRestoredShape.IsNull());
  ASSERT_TRUE(BRepCheck_Analyzer(aRestoredShape).IsValid());

  BOPAlgo_CheckerSI              aRestoredChecker;
  NCollection_List<TopoDS_Shape> aRestoredCheckArguments;
  aRestoredCheckArguments.Append(aRestoredShape);
  aRestoredChecker.SetArguments(aRestoredCheckArguments);
  aRestoredChecker.SetLevelOfCheck(9);
  aRestoredChecker.SetRunParallel(false);
  aRestoredChecker.Perform();
  ASSERT_FALSE(aRestoredChecker.HasErrors());

  GProp_GProps aRestoredVolume;
  BRepGProp::VolumeProperties(aRestoredShape, aRestoredVolume);
  const double aVolumeTolerance = 1.0e-6 * std::max(1.0, std::abs(aSourceVolume.Mass()));
  EXPECT_NEAR(aRestoredVolume.Mass(), aSourceVolume.Mass(), aVolumeTolerance);
}

// bugs/step/bug25694: the default STEP schema is AP214IS.
TEST_F(DESTEP_ProviderTest, StepBug_25694_DefaultSchemaIsAP214IS)
{
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("bug25694.step", anOStream));
  ASSERT_TRUE(myProvider->Write(aWriteStreams, myBox));

  const std::string aStepContent = anOStream.str();
  ASSERT_TRUE(IsValidSTEPContent(aStepContent));
  EXPECT_NE(aStepContent.find("AUTOMOTIVE_DESIGN { 1 0 10303 214 1 1 1 1 }"), std::string::npos);
}

// bugs/step/bug27070: writing a null shape reports a translation failure.
TEST_F(DESTEP_ProviderTest, StepBug_27070_NullShapeWriteFails)
{
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("bug27070.step", anOStream));
  TopoDS_Shape aNullShape;
  EXPECT_FALSE(myProvider->Write(aWriteStreams, aNullShape));
}

// bugs/step/bug28024: STEP stream import honors the disabled orientation fix
// and preserves the negative orientation of the input triangular face.
TEST_F(DESTEP_ProviderTest, StepBug_28024_DisabledOrientationFix)
{
  const TopoDS_Vertex aV1  = BRepBuilderAPI_MakeVertex(gp_Pnt(0.0, 0.0, 0.0)).Vertex();
  const TopoDS_Vertex aV2  = BRepBuilderAPI_MakeVertex(gp_Pnt(1.0, 0.0, 0.0)).Vertex();
  const TopoDS_Vertex aV3  = BRepBuilderAPI_MakeVertex(gp_Pnt(0.0, 1.0, 0.0)).Vertex();
  const TopoDS_Edge   anE1 = BRepBuilderAPI_MakeEdge(aV2, aV1).Edge();
  const TopoDS_Edge   anE2 = BRepBuilderAPI_MakeEdge(aV3, aV2).Edge();
  const TopoDS_Edge   anE3 = BRepBuilderAPI_MakeEdge(aV1, aV3).Edge();

  BRepBuilderAPI_MakeWire aWireBuilder;
  aWireBuilder.Add(anE1);
  aWireBuilder.Add(anE3);
  aWireBuilder.Add(anE2);
  ASSERT_TRUE(aWireBuilder.IsDone());

  BRepBuilderAPI_MakeFace aFaceBuilder(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                                       aWireBuilder.Wire(),
                                       false);
  ASSERT_TRUE(aFaceBuilder.IsDone());
  const TopoDS_Face aFace = aFaceBuilder.Face();

  GProp_GProps aSourceProperties;
  BRepGProp::SurfaceProperties(aFace, aSourceProperties);
  EXPECT_NEAR(aSourceProperties.Mass(), -0.5, 1.0e-6);

  occ::handle<DESTEP_ConfigurationNode> aWriteNode     = new DESTEP_ConfigurationNode();
  occ::handle<DESTEP_Provider>          aWriteProvider = new DESTEP_Provider(aWriteNode);
  std::ostringstream                    aWriteStream;
  DE_Provider::WriteStreamList          aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("bug28024.stp", aWriteStream));
  ASSERT_TRUE(aWriteProvider->Write(aWriteStreams, aFace));
  ASSERT_FALSE(aWriteStream.str().empty());

  occ::handle<DESTEP_ConfigurationNode> aReadNode  = new DESTEP_ConfigurationNode();
  aReadNode->ShapeFixParameters.FixOrientationMode = DE_ShapeFixParameters::FixMode::NotFix;
  occ::handle<DESTEP_Provider> aReadProvider       = new DESTEP_Provider(aReadNode);

  std::istringstream          aReadStream(aWriteStream.str());
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("bug28024.stp", aReadStream));
  TopoDS_Shape aRestoredShape;
  ASSERT_TRUE(aReadProvider->Read(aReadStreams, aRestoredShape));
  TopExp_Explorer aFaceExplorer(aRestoredShape, TopAbs_FACE);
  ASSERT_TRUE(aFaceExplorer.More());
  GProp_GProps aRestoredProperties;
  BRepGProp::SurfaceProperties(TopoDS::Face(aFaceExplorer.Current()), aRestoredProperties);
  EXPECT_NEAR(aRestoredProperties.Mass(), -0.5, 1.0e-6);
}

// bugs/step/bug23203: STEP stream import preserves the ellipse basis curve of
// a revolved surface, including its local X and Y directions.
TEST_F(DESTEP_ProviderTest, StepBug_23203_EllipseRevolutionAxes)
{
  const gp_Ax2 anEllipseAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, -1.0, 0.0), gp_Dir(1.0, 0.0, -1.0));
  const occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anEllipseAxis, 10.0, 5.0);
  const occ::handle<Geom_SurfaceOfRevolution> aSurface =
    new Geom_SurfaceOfRevolution(anEllipse, gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));
  BRepBuilderAPI_MakeFace aFaceBuilder(aSurface, 0.0, 3.0, 0.0, 1.0, Precision::Confusion());
  ASSERT_TRUE(aFaceBuilder.IsDone());
  const TopoDS_Face aFace = aFaceBuilder.Face();

  std::ostringstream           aWriteStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("bug23203.step", aWriteStream));
  ASSERT_TRUE(myProvider->Write(aWriteStreams, aFace));
  ASSERT_FALSE(aWriteStream.str().empty());

  std::istringstream          aReadStream(aWriteStream.str());
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("bug23203.step", aReadStream));
  TopoDS_Shape aRestoredShape;
  ASSERT_TRUE(myProvider->Read(aReadStreams, aRestoredShape));
  ASSERT_FALSE(aRestoredShape.IsNull());

  TopExp_Explorer aFaceExplorer(aRestoredShape, TopAbs_FACE);
  ASSERT_TRUE(aFaceExplorer.More());
  const TopoDS_Face                           aRestoredFace = TopoDS::Face(aFaceExplorer.Current());
  const occ::handle<Geom_Surface>             aRestoredSurface = BRep_Tool::Surface(aRestoredFace);
  const occ::handle<Geom_SurfaceOfRevolution> aRestoredRevolution =
    occ::down_cast<Geom_SurfaceOfRevolution>(aRestoredSurface);
  ASSERT_FALSE(aRestoredRevolution.IsNull());
  const occ::handle<Geom_Ellipse> aRestoredEllipse =
    occ::down_cast<Geom_Ellipse>(aRestoredRevolution->BasisCurve());
  ASSERT_FALSE(aRestoredEllipse.IsNull());

  const gp_Dir anExpectedX = anEllipse->XAxis().Direction();
  const gp_Dir anExpectedY = anEllipse->YAxis().Direction();
  const gp_Dir anActualX   = aRestoredEllipse->XAxis().Direction();
  const gp_Dir anActualY   = aRestoredEllipse->YAxis().Direction();
  EXPECT_NEAR(anActualX.X(), anExpectedX.X(), 1.0e-6);
  EXPECT_NEAR(anActualX.Y(), anExpectedX.Y(), 1.0e-6);
  EXPECT_NEAR(anActualX.Z(), anExpectedX.Z(), 1.0e-6);
  EXPECT_NEAR(anActualY.X(), anExpectedY.X(), 1.0e-6);
  EXPECT_NEAR(anActualY.Y(), anExpectedY.Y(), 1.0e-6);
  EXPECT_NEAR(anActualY.Z(), anExpectedY.Z(), 1.0e-6);
}

// bugs/xde/bug27722: STEP round-trip of an ellipse revolved around the Y axis
// retains the original DRAW TODO diagnostics.
TEST_F(DESTEP_ProviderTest, XdeBug_27722_EllipseRevolutionTopology)
{
  const occ::handle<Geom_Ellipse> anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 20.0, 10.0);
  const TopoDS_Edge  anEdge = BRepBuilderAPI_MakeEdge(anEllipse, 0.0, 0.5 * M_PI).Edge();
  const TopoDS_Shape aSource =
    BRepPrimAPI_MakeRevol(anEdge, gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)), 2.0 * M_PI)
      .Shape();
  ASSERT_FALSE(aSource.IsNull());
  ASSERT_TRUE(BRepCheck_Analyzer(aSource).IsValid());

  std::ostringstream           aWriteStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("bug27722.step", aWriteStream));
  ASSERT_TRUE(myProvider->Write(aWriteStreams, aSource));
  ASSERT_FALSE(aWriteStream.str().empty());

  std::istringstream          aReadStream(aWriteStream.str());
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("bug27722.step", aReadStream));
  TopoDS_Shape aRestored;
  ASSERT_TRUE(myProvider->Read(aReadStreams, aRestored));
  ASSERT_FALSE(aRestored.IsNull());
  // The original DRAW test is marked TODO because STEP restoration still
  // produces a faulty shape for this case. Keep that known result explicit.
  const bool isRestoredValid = BRepCheck_Analyzer(aRestored).IsValid();
  RecordProperty("restored_shape_valid", isRestoredValid ? "true" : "false");

  NCollection_Array1<TopAbs_ShapeEnum> aTypes(1, 9);
  aTypes.SetValue(1, TopAbs_VERTEX);
  aTypes.SetValue(2, TopAbs_EDGE);
  aTypes.SetValue(3, TopAbs_WIRE);
  aTypes.SetValue(4, TopAbs_FACE);
  aTypes.SetValue(5, TopAbs_SHELL);
  aTypes.SetValue(6, TopAbs_SOLID);
  aTypes.SetValue(7, TopAbs_COMPSOLID);
  aTypes.SetValue(8, TopAbs_COMPOUND);
  aTypes.SetValue(9, TopAbs_SHAPE);
  NCollection_Array1<const char*> aTypeNames(1, 9);
  aTypeNames.SetValue(1, "vertices");
  aTypeNames.SetValue(2, "edges");
  aTypeNames.SetValue(3, "wires");
  aTypeNames.SetValue(4, "faces");
  aTypeNames.SetValue(5, "shells");
  aTypeNames.SetValue(6, "solids");
  aTypeNames.SetValue(7, "compsolids");
  aTypeNames.SetValue(8, "compounds");
  aTypeNames.SetValue(9, "shapes");
  bool hasTopologyDifference = false;
  for (int anIndex = 1; anIndex <= 9; ++anIndex)
  {
    const TopAbs_ShapeEnum aType          = aTypes.Value(anIndex);
    const int              aSourceCount   = CountShapeElements(aSource, aType);
    const int              aRestoredCount = CountShapeElements(aRestored, aType);
    hasTopologyDifference                 = hasTopologyDifference || aSourceCount != aRestoredCount;
    RecordProperty((std::string("source_") + aTypeNames.Value(anIndex)).c_str(), aSourceCount);
    RecordProperty((std::string("restored_") + aTypeNames.Value(anIndex)).c_str(), aRestoredCount);
  }
  EXPECT_FALSE(isRestoredValid);
  EXPECT_TRUE(hasTopologyDifference);
  EXPECT_GT(CountShapeElements(aRestored, TopAbs_FACE), 0);
}

// Test stream-based document write and read operations
TEST_F(DESTEP_ProviderTest, StreamDocumentWriteRead)
{
  // Add box to document
  occ::handle<XCAFDoc_ShapeTool> aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label                      aShapeLabel = aShapeTool->AddShape(myBox);
  EXPECT_FALSE(aShapeLabel.IsNull());

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("document.step", anOStream));

  // Write document to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument));

  std::string aStepContent = anOStream.str();
  EXPECT_FALSE(aStepContent.empty());
  EXPECT_TRUE(IsValidSTEPContent(aStepContent));

  if (!aStepContent.empty())
  {
    // Create new document for reading
    occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
    occ::handle<TDocStd_Document>    aNewDocument;
    anApp->NewDocument("BinXCAF", aNewDocument);

    // Read back from stream
    std::istringstream          anIStream(aStepContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("document.step", anIStream));

    EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument));

    // Validate document content
    occ::handle<XCAFDoc_ShapeTool> aNewShapeTool =
      XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
    NCollection_Sequence<TDF_Label> aLabels;
    aNewShapeTool->GetShapes(aLabels);
    EXPECT_GT(aLabels.Length(), 0); // Should have at least one shape in document
  }
}

// Migrated from tests/bugs/xde/bug1540.  Writing an XCAF document containing
// an invisible layer must succeed without touching the filesystem.
TEST_F(DESTEP_ProviderTest, XdeBug_1540_WriteDocumentWithInvisibleLayer)
{
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  const TDF_Label aShapeLabel = aShapeTool->AddShape(BRepPrimAPI_MakeBox(10., 10., 10.).Shape());
  ASSERT_FALSE(aShapeLabel.IsNull());

  occ::handle<XCAFDoc_LayerTool> aLayerTool = XCAFDoc_DocumentTool::LayerTool(myDocument->Main());
  const TDF_Label aLayerLabel = aLayerTool->AddLayer(TCollection_ExtendedString("invis"));
  ASSERT_FALSE(aLayerLabel.IsNull());
  aLayerTool->SetLayer(aShapeLabel, aLayerLabel);
  aLayerTool->SetVisibility(aLayerLabel, false);
  EXPECT_TRUE(aLayerTool->IsSet(aShapeLabel, aLayerLabel));
  EXPECT_FALSE(aLayerTool->IsVisible(aLayerLabel));

  std::ostringstream           aStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("bug1540.step", aStream));
  ASSERT_TRUE(myProvider->Write(aWriteStreams, myDocument));

  const std::string aContent = aStream.str();
  EXPECT_TRUE(IsValidSTEPContent(aContent));
  EXPECT_NE(aContent.find("invis"), std::string::npos);
  EXPECT_NE(aContent.find("INVISIBILITY"), std::string::npos);
}

// Test DE_Wrapper integration for STEP operations
TEST_F(DESTEP_ProviderTest, DE_WrapperIntegration)
{
  // Initialize DE_Wrapper and bind STEP provider
  DE_Wrapper                            aWrapper;
  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();

  // Bind the configured node to wrapper
  EXPECT_TRUE(aWrapper.Bind(aNode));

  // Test write with DE_Wrapper using sphere
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("test.step", anOStream));

  EXPECT_TRUE(aWrapper.Write(aWriteStreams, mySphere));

  std::string aStepContent = anOStream.str();
  EXPECT_FALSE(aStepContent.empty());
  EXPECT_TRUE(IsValidSTEPContent(aStepContent));

  if (!aStepContent.empty())
  {
    // Test DE_Wrapper stream operations
    std::istringstream          anIStream(aStepContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("test.step", anIStream));

    TopoDS_Shape aReadShape;
    bool         aWrapperResult = aWrapper.Read(aReadStreams, aReadShape);

    // Test direct provider with same content for comparison
    std::istringstream          anIStream2(aStepContent);
    DE_Provider::ReadStreamList aReadStreams2;
    aReadStreams2.Append(DE_Provider::ReadStreamNode("test.step", anIStream2));

    occ::handle<DESTEP_Provider> aDirectProvider = new DESTEP_Provider(aNode);
    TopoDS_Shape                 aDirectShape;
    bool                         aDirectResult = aDirectProvider->Read(aReadStreams2, aDirectShape);

    // REQUIREMENT: DE_Wrapper must work exactly the same as direct provider
    EXPECT_EQ(aWrapperResult, aDirectResult);
    EXPECT_EQ(aReadShape.IsNull(), aDirectShape.IsNull());

    if (aDirectResult && !aDirectShape.IsNull())
    {
      int aSolids = CountShapeElements(aDirectShape, TopAbs_SOLID);
      EXPECT_GT(aSolids, 0);
    }
  }
}

// Test multiple shapes in single document
TEST_F(DESTEP_ProviderTest, MultipleShapesInDocument)
{
  // Add multiple shapes to document
  occ::handle<XCAFDoc_ShapeTool> aShapeTool   = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label                      aBoxLabel    = aShapeTool->AddShape(myBox);
  TDF_Label                      aSphereLabel = aShapeTool->AddShape(mySphere);
  TDF_Label                      aCylinderLabel = aShapeTool->AddShape(myCylinder);

  EXPECT_FALSE(aBoxLabel.IsNull());
  EXPECT_FALSE(aSphereLabel.IsNull());
  EXPECT_FALSE(aCylinderLabel.IsNull());

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("multi_shapes.step", anOStream));

  // Write document with multiple shapes
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument));

  std::string aStepContent = anOStream.str();
  EXPECT_FALSE(aStepContent.empty());
  EXPECT_TRUE(IsValidSTEPContent(aStepContent));

  // Read back into new document
  occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aNewDocument;
  anApp->NewDocument("BinXCAF", aNewDocument);

  std::istringstream          anIStream(aStepContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("multi_shapes.step", anIStream));

  EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument));

  // Validate document content
  occ::handle<XCAFDoc_ShapeTool> aNewShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
  NCollection_Sequence<TDF_Label> aLabels;
  aNewShapeTool->GetShapes(aLabels);
  EXPECT_EQ(aLabels.Length(), 3); // Should have exactly 3 shapes in document
}

// Test different BRep geometry types
TEST_F(DESTEP_ProviderTest, DifferentBRepGeometries)
{
  // Test box geometry
  std::ostringstream           aBoxStream;
  DE_Provider::WriteStreamList aBoxWriteStreams;
  aBoxWriteStreams.Append(DE_Provider::WriteStreamNode("box.step", aBoxStream));

  EXPECT_TRUE(myProvider->Write(aBoxWriteStreams, myBox));
  std::string aBoxContent = aBoxStream.str();

  // Test sphere geometry
  std::ostringstream           aSphereStream;
  DE_Provider::WriteStreamList aSphereWriteStreams;
  aSphereWriteStreams.Append(DE_Provider::WriteStreamNode("sphere.step", aSphereStream));

  EXPECT_TRUE(myProvider->Write(aSphereWriteStreams, mySphere));
  std::string aSphereContent = aSphereStream.str();

  // Test cylinder geometry
  std::ostringstream           aCylinderStream;
  DE_Provider::WriteStreamList aCylinderWriteStreams;
  aCylinderWriteStreams.Append(DE_Provider::WriteStreamNode("cylinder.step", aCylinderStream));

  EXPECT_TRUE(myProvider->Write(aCylinderWriteStreams, myCylinder));
  std::string aCylinderContent = aCylinderStream.str();

  // All content should be valid STEP format
  EXPECT_TRUE(IsValidSTEPContent(aBoxContent));
  EXPECT_TRUE(IsValidSTEPContent(aSphereContent));
  EXPECT_TRUE(IsValidSTEPContent(aCylinderContent));

  // Different geometries should produce different STEP content
  EXPECT_NE(aBoxContent, aSphereContent);
  EXPECT_NE(aBoxContent, aCylinderContent);
  EXPECT_NE(aSphereContent, aCylinderContent);

  // All should read back successfully
  std::istringstream          aBoxIStream(aBoxContent);
  DE_Provider::ReadStreamList aBoxReadStreams;
  aBoxReadStreams.Append(DE_Provider::ReadStreamNode("box.step", aBoxIStream));

  TopoDS_Shape aBoxReadShape;
  EXPECT_TRUE(myProvider->Read(aBoxReadStreams, aBoxReadShape));
  EXPECT_FALSE(aBoxReadShape.IsNull());

  std::istringstream          aSphereIStream(aSphereContent);
  DE_Provider::ReadStreamList aSphereReadStreams;
  aSphereReadStreams.Append(DE_Provider::ReadStreamNode("sphere.step", aSphereIStream));

  TopoDS_Shape aSphereReadShape;
  EXPECT_TRUE(myProvider->Read(aSphereReadStreams, aSphereReadShape));
  EXPECT_FALSE(aSphereReadShape.IsNull());
}

// Test DE_Wrapper with different file extensions
TEST_F(DESTEP_ProviderTest, DE_WrapperFileExtensions)
{
  DE_Wrapper                            aWrapper;
  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  EXPECT_TRUE(aWrapper.Bind(aNode));

  // Test different STEP extensions
  NCollection_Array1<const char*> aExtensions(1, 4);
  aExtensions.SetValue(1, "test.step");
  aExtensions.SetValue(2, "test.STEP");
  aExtensions.SetValue(3, "test.stp");
  aExtensions.SetValue(4, "test.STP");

  for (NCollection_Array1<const char*>::Iterator anIterator(aExtensions); anIterator.More();
       anIterator.Next())
  {
    const char*                  anExt = anIterator.Value();
    std::ostringstream           anOStream;
    DE_Provider::WriteStreamList aWriteStreams;
    aWriteStreams.Append(DE_Provider::WriteStreamNode(anExt, anOStream));

    EXPECT_TRUE(aWrapper.Write(aWriteStreams, myBox))
      << "Failed to write with extension: " << anExt;

    std::string aContent = anOStream.str();
    EXPECT_FALSE(aContent.empty()) << "Empty content for extension: " << anExt;
    EXPECT_TRUE(IsValidSTEPContent(aContent)) << "Invalid STEP content for extension: " << anExt;

    // Test read back
    std::istringstream          anIStream(aContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode(anExt, anIStream));

    TopoDS_Shape aReadShape;
    EXPECT_TRUE(aWrapper.Read(aReadStreams, aReadShape))
      << "Failed to read with extension: " << anExt;
    EXPECT_FALSE(aReadShape.IsNull()) << "Null shape read with extension: " << anExt;
  }
}

// Test error conditions and edge cases
TEST_F(DESTEP_ProviderTest, ErrorHandling)
{
  // Test with empty streams
  DE_Provider::WriteStreamList anEmptyWriteStreams;
  EXPECT_FALSE(myProvider->Write(anEmptyWriteStreams, myBox));

  DE_Provider::ReadStreamList anEmptyReadStreams;
  TopoDS_Shape                aShape;
  EXPECT_FALSE(myProvider->Read(anEmptyReadStreams, aShape));

  // Test with null shape
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("null_test.step", anOStream));
  TopoDS_Shape aNullShape;

  // Writing null shape should fail
  EXPECT_FALSE(myProvider->Write(aWriteStreams, aNullShape));

  // Test reading invalid STEP content
  std::string                 anInvalidContent = "This is not valid STEP content";
  std::istringstream          anInvalidStream(anInvalidContent);
  DE_Provider::ReadStreamList anInvalidReadStreams;
  anInvalidReadStreams.Append(DE_Provider::ReadStreamNode("invalid.step", anInvalidStream));

  TopoDS_Shape anInvalidShape;
  EXPECT_FALSE(myProvider->Read(anInvalidReadStreams, anInvalidShape));

  // Test with null document
  occ::handle<TDocStd_Document> aNullDoc;
  EXPECT_FALSE(myProvider->Write(aWriteStreams, aNullDoc));
  EXPECT_FALSE(myProvider->Read(anEmptyReadStreams, aNullDoc));
}

// Test DESTEP configuration modes
TEST_F(DESTEP_ProviderTest, ConfigurationModes)
{
  occ::handle<DESTEP_ConfigurationNode> aNode =
    occ::down_cast<DESTEP_ConfigurationNode>(myProvider->GetNode());

  // Test basic configuration access
  EXPECT_FALSE(aNode.IsNull());

  // Test provider format and vendor are correct
  EXPECT_STREQ("STEP", myProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", myProvider->GetVendor().ToCString());

  // Test that we can create provider with different configuration
  occ::handle<DESTEP_ConfigurationNode> aNewNode     = new DESTEP_ConfigurationNode();
  occ::handle<DESTEP_Provider>          aNewProvider = new DESTEP_Provider(aNewNode);

  EXPECT_STREQ("STEP", aNewProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", aNewProvider->GetVendor().ToCString());
  EXPECT_FALSE(aNewProvider->GetNode().IsNull());
}

// Test WorkSession integration
TEST_F(DESTEP_ProviderTest, WorkSessionIntegration)
{
  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession();

  // Test write operation with work session
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("ws_test.step", anOStream));

  EXPECT_TRUE(myProvider->Write(aWriteStreams, myBox, aWS));

  std::string aStepContent = anOStream.str();
  EXPECT_FALSE(aStepContent.empty());
  EXPECT_TRUE(IsValidSTEPContent(aStepContent));

  // Test read operation with work session
  std::istringstream          anIStream(aStepContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("ws_test.step", anIStream));

  TopoDS_Shape aReadShape;
  EXPECT_TRUE(myProvider->Read(aReadStreams, aReadShape, aWS));
  EXPECT_FALSE(aReadShape.IsNull());
}

// Test document operations with WorkSession
TEST_F(DESTEP_ProviderTest, DocumentWorkSessionIntegration)
{
  // Add shape to document
  occ::handle<XCAFDoc_ShapeTool> aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label                      aShapeLabel = aShapeTool->AddShape(mySphere);
  EXPECT_FALSE(aShapeLabel.IsNull());

  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession();

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("doc_ws_test.step", anOStream));

  // Test document write with work session
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument, aWS));

  std::string aStepContent = anOStream.str();
  EXPECT_FALSE(aStepContent.empty());
  EXPECT_TRUE(IsValidSTEPContent(aStepContent));

  // Create new document for reading
  occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aNewDocument;
  anApp->NewDocument("BinXCAF", aNewDocument);

  // Test document read with work session
  std::istringstream          anIStream(aStepContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("doc_ws_test.step", anIStream));

  EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument, aWS));

  // Validate document content
  occ::handle<XCAFDoc_ShapeTool> aNewShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
  NCollection_Sequence<TDF_Label> aLabels;
  aNewShapeTool->GetShapes(aLabels);
  EXPECT_GT(aLabels.Length(), 0);
}

// perf/de/bug29830_3: writing a large XCAF compound must complete and the
// document must be safely destructible afterwards.  The DRAW test used a
// temporary STEP file; the stream keeps the same transfer and lifetime path
// without depending on the filesystem.
TEST_F(DESTEP_ProviderTest, StepBug_29830_LargeCompoundWriteAndDestruction)
{
  const int          aShapeCount = 1000;
  const TopoDS_Shape aBox        = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();

  std::ostringstream aWriteStream;
  {
    BRep_Builder    aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound(aCompound);
    for (int anIndex = 0; anIndex < aShapeCount; ++anIndex)
    {
      aBuilder.Add(aCompound, aBox);
    }

    occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
    occ::handle<TDocStd_Document>    aDocument;
    anApp->NewDocument("BinXCAF", aDocument);
    ASSERT_FALSE(aDocument.IsNull());

    occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
    ASSERT_FALSE(aShapeTool.IsNull());
    const TDF_Label aShapeLabel = aShapeTool->AddShape(aCompound);
    ASSERT_FALSE(aShapeLabel.IsNull());

    DE_Provider::WriteStreamList aWriteStreams;
    aWriteStreams.Append(DE_Provider::WriteStreamNode("bug29830.step", aWriteStream));
    ASSERT_TRUE(myProvider->Write(aWriteStreams, aDocument));
  }

  const std::string aStepContent = aWriteStream.str();
  ASSERT_TRUE(IsValidSTEPContent(aStepContent));
  EXPECT_GT(aStepContent.size(), static_cast<std::size_t>(aShapeCount));
}
