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

#include <DESTL_Provider.hxx>
#include <DESTL_ConfigurationNode.hxx>
#include <DE_Wrapper.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Triangle.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <BRep_Tool.hxx>
#include <TopLoc_Location.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <sstream>
#include <gtest/gtest.h>

class DESTL_ProviderTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Initialize provider with default configuration
    Handle(DESTL_ConfigurationNode) aNode = new DESTL_ConfigurationNode();
    myProvider                            = new DESTL_Provider(aNode);

    // Create triangulated shape for testing (STL format requires triangulated data)

    myTriangularFace = CreateTriangulatedFace();

    // Create test document
    Handle(TDocStd_Application) anApp = new TDocStd_Application();
    anApp->NewDocument("BinXCAF", myDocument);
  }

  void TearDown() override
  {
    myProvider.Nullify();
    myDocument.Nullify();
  }

  // Helper method to count shape elements
  Standard_Integer CountShapeElements(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
  {
    Standard_Integer aCount = 0;
    for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
    {
      aCount++;
    }
    return aCount;
  }

  // Helper method to create a triangulated face with mesh data (suitable for STL)
  TopoDS_Shape CreateTriangulatedFace()
  {
    // Create vertices for triangulation
    TColgp_Array1OfPnt aNodes(1, 4);
    aNodes.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));   // Bottom-left
    aNodes.SetValue(2, gp_Pnt(10.0, 0.0, 0.0));  // Bottom-right
    aNodes.SetValue(3, gp_Pnt(10.0, 10.0, 0.0)); // Top-right
    aNodes.SetValue(4, gp_Pnt(0.0, 10.0, 0.0));  // Top-left

    // Create triangles (two triangles forming a quad)
    Poly_Array1OfTriangle aTriangles(1, 2);
    aTriangles.SetValue(1, Poly_Triangle(1, 2, 3)); // First triangle
    aTriangles.SetValue(2, Poly_Triangle(1, 3, 4)); // Second triangle

    // Create triangulation
    Handle(Poly_Triangulation) aTriangulation = new Poly_Triangulation(aNodes, aTriangles);

    // Create a simple planar face
    gp_Pln                  aPlane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(gp_Dir::D::Z));
    BRepBuilderAPI_MakeFace aFaceBuilder(aPlane, 0.0, 10.0, 0.0, 10.0);

    if (!aFaceBuilder.IsDone())
    {
      return TopoDS_Shape();
    }

    TopoDS_Face aFace = aFaceBuilder.Face();

    // Attach triangulation to the face
    BRep_Builder aBuilder;
    aBuilder.UpdateFace(aFace, aTriangulation);

    return aFace;
  }

protected:
  Handle(DESTL_Provider)   myProvider;
  TopoDS_Shape             myTriangularFace;
  Handle(TDocStd_Document) myDocument;
};

// Test basic provider creation and format/vendor information
TEST_F(DESTL_ProviderTest, BasicProperties)
{
  EXPECT_STREQ("STL", myProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", myProvider->GetVendor().ToCString());
  EXPECT_FALSE(myProvider->GetNode().IsNull());
}

// Test stream-based shape write and read operations
TEST_F(DESTL_ProviderTest, StreamShapeWriteRead)
{
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("test.stl", anOStream));

  // Write triangulated face to stream (STL works with mesh data)
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myTriangularFace));

  std::string aStlContent = anOStream.str();
  EXPECT_FALSE(aStlContent.empty());
  EXPECT_TRUE(aStlContent.find("solid") != std::string::npos
              || aStlContent.find("facet") != std::string::npos);

  if (!aStlContent.empty())
  {
    // Read back from stream
    std::istringstream          anIStream(aStlContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("test.stl", anIStream));

    TopoDS_Shape aReadShape;
    EXPECT_TRUE(myProvider->Read(aReadStreams, aReadShape));
    EXPECT_FALSE(aReadShape.IsNull());

    if (!aReadShape.IsNull())
    {
      // STL should produce faces with triangulation
      Standard_Integer aReadFaces = CountShapeElements(aReadShape, TopAbs_FACE);
      EXPECT_GT(aReadFaces, 0); // Should have faces
    }
  }
}

// Test stream-based document write and read operations
TEST_F(DESTL_ProviderTest, StreamDocumentWriteRead)
{
  // Add triangulated shape to document
  Handle(XCAFDoc_ShapeTool) aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label                 aShapeLabel = aShapeTool->AddShape(myTriangularFace);
  EXPECT_FALSE(aShapeLabel.IsNull());

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("document.stl", anOStream));

  // Write document to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument));

  std::string aStlContent = anOStream.str();
  EXPECT_FALSE(aStlContent.empty());

  if (!aStlContent.empty())
  {
    // Create new document for reading
    Handle(TDocStd_Application) anApp = new TDocStd_Application();
    Handle(TDocStd_Document)    aNewDocument;
    anApp->NewDocument("BinXCAF", aNewDocument);

    // Read back from stream
    std::istringstream          anIStream(aStlContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("document.stl", anIStream));

    EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument));

    // Validate document content
    Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
    TDF_LabelSequence         aLabels;
    aNewShapeTool->GetShapes(aLabels);
    EXPECT_GT(aLabels.Length(), 0); // Should have at least one shape in document
  }
}

// Test DE_Wrapper integration for STL operations
TEST_F(DESTL_ProviderTest, DE_WrapperIntegration)
{
  // Initialize DE_Wrapper and bind STL provider
  DE_Wrapper                      aWrapper;
  Handle(DESTL_ConfigurationNode) aNode = new DESTL_ConfigurationNode();

  // Bind the configured node to wrapper
  EXPECT_TRUE(aWrapper.Bind(aNode));

  // Test write with DE_Wrapper using triangular face
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("test.stl", anOStream));

  EXPECT_TRUE(aWrapper.Write(aWriteStreams, myTriangularFace));

  std::string aStlContent = anOStream.str();
  EXPECT_FALSE(aStlContent.empty());

  if (!aStlContent.empty())
  {
    // Test DE_Wrapper stream operations
    std::istringstream          anIStream(aStlContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("test.stl", anIStream));

    TopoDS_Shape aReadShape;
    bool         aWrapperResult = aWrapper.Read(aReadStreams, aReadShape);

    // Test direct provider with same content for comparison
    std::istringstream          anIStream2(aStlContent);
    DE_Provider::ReadStreamList aReadStreams2;
    aReadStreams2.Append(DE_Provider::ReadStreamNode("test.stl", anIStream2));

    Handle(DESTL_Provider) aDirectProvider = new DESTL_Provider(aNode);
    TopoDS_Shape           aDirectShape;
    bool                   aDirectResult = aDirectProvider->Read(aReadStreams2, aDirectShape);

    // REQUIREMENT: DE_Wrapper must work exactly the same as direct provider
    EXPECT_EQ(aWrapperResult, aDirectResult);
    EXPECT_EQ(aReadShape.IsNull(), aDirectShape.IsNull());

    if (aDirectResult && !aDirectShape.IsNull())
    {
      Standard_Integer aFaces = CountShapeElements(aDirectShape, TopAbs_FACE);
      EXPECT_GT(aFaces, 0);
    }
  }
}

// Test error conditions and edge cases with null document validation
TEST_F(DESTL_ProviderTest, ErrorHandling)
{
  // Test with empty streams
  DE_Provider::WriteStreamList anEmptyWriteStreams;
  EXPECT_FALSE(myProvider->Write(anEmptyWriteStreams, myTriangularFace));

  DE_Provider::ReadStreamList anEmptyReadStreams;
  TopoDS_Shape                aShape;
  EXPECT_FALSE(myProvider->Read(anEmptyReadStreams, aShape));

  // Test with null shape
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("null_test.stl", anOStream));
  TopoDS_Shape aNullShape;

  // Writing null shape should succeed but produce minimal content
  myProvider->Write(aWriteStreams, aNullShape);
  std::string aContent = anOStream.str();
  // STL may produce empty content for null shape

  // Test reading invalid STL content
  std::string                 anInvalidContent = "This is not valid STL content";
  std::istringstream          anInvalidStream(anInvalidContent);
  DE_Provider::ReadStreamList anInvalidReadStreams;
  anInvalidReadStreams.Append(DE_Provider::ReadStreamNode("invalid.stl", anInvalidStream));

  TopoDS_Shape anInvalidShape;
  EXPECT_FALSE(myProvider->Read(anInvalidReadStreams, anInvalidShape));

  // Test with null document
  Handle(TDocStd_Document) aNullDoc;
  EXPECT_FALSE(myProvider->Write(aWriteStreams, aNullDoc));
  EXPECT_FALSE(myProvider->Read(anEmptyReadStreams, aNullDoc));
}

// Test DESTL configuration modes
TEST_F(DESTL_ProviderTest, ConfigurationModes)
{
  Handle(DESTL_ConfigurationNode) aNode =
    Handle(DESTL_ConfigurationNode)::DownCast(myProvider->GetNode());

  // Test ASCII mode configuration
  aNode->InternalParameters.WriteAscii = Standard_True;
  EXPECT_TRUE(aNode->InternalParameters.WriteAscii);

  // Test Binary mode configuration
  aNode->InternalParameters.WriteAscii = Standard_False;
  EXPECT_FALSE(aNode->InternalParameters.WriteAscii);

  // Test merge angle configuration
  aNode->InternalParameters.ReadMergeAngle = 45.0;
  EXPECT_DOUBLE_EQ(45.0, aNode->InternalParameters.ReadMergeAngle);

  // Test that provider format and vendor are correct
  EXPECT_STREQ("STL", myProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", myProvider->GetVendor().ToCString());
}

// Test ASCII vs Binary mode configurations
TEST_F(DESTL_ProviderTest, AsciiVsBinaryModes)
{
  Handle(DESTL_ConfigurationNode) aNode =
    Handle(DESTL_ConfigurationNode)::DownCast(myProvider->GetNode());

  // Test ASCII mode
  aNode->InternalParameters.WriteAscii = Standard_True;

  std::ostringstream           anAsciiStream;
  DE_Provider::WriteStreamList anAsciiWriteStreams;
  anAsciiWriteStreams.Append(DE_Provider::WriteStreamNode("ascii_test.stl", anAsciiStream));

  EXPECT_TRUE(myProvider->Write(anAsciiWriteStreams, myTriangularFace));
  std::string anAsciiContent = anAsciiStream.str();
  EXPECT_FALSE(anAsciiContent.empty());
  EXPECT_TRUE(anAsciiContent.find("solid") != std::string::npos);

  // Test Binary mode
  aNode->InternalParameters.WriteAscii = Standard_False;

  std::ostringstream           aBinaryStream;
  DE_Provider::WriteStreamList aBinaryWriteStreams;
  aBinaryWriteStreams.Append(DE_Provider::WriteStreamNode("binary_test.stl", aBinaryStream));

  EXPECT_TRUE(myProvider->Write(aBinaryWriteStreams, myTriangularFace));
  std::string aBinaryContent = aBinaryStream.str();
  EXPECT_FALSE(aBinaryContent.empty());

  // Binary and ASCII content should be different
  EXPECT_NE(anAsciiContent, aBinaryContent);
}

// Test multiple shapes in single document
TEST_F(DESTL_ProviderTest, MultipleShapesInDocument)
{
  // Add triangulated face to document multiple times (to create multiple shapes)
  Handle(XCAFDoc_ShapeTool) aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label                 aFaceLabel1 = aShapeTool->AddShape(myTriangularFace);
  TDF_Label aFaceLabel2 = aShapeTool->AddShape(myTriangularFace); // Add same face again

  EXPECT_FALSE(aFaceLabel1.IsNull());
  EXPECT_FALSE(aFaceLabel2.IsNull());

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("multi_shapes.stl", anOStream));

  // Write document with multiple shapes
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument));

  std::string aStlContent = anOStream.str();
  EXPECT_FALSE(aStlContent.empty());

  // Read back into new document
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  Handle(TDocStd_Document)    aNewDocument;
  anApp->NewDocument("BinXCAF", aNewDocument);

  std::istringstream          anIStream(aStlContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("multi_shapes.stl", anIStream));

  EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument));

  // Validate document content
  Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
  TDF_LabelSequence         aLabels;
  aNewShapeTool->GetShapes(aLabels);
  EXPECT_GT(aLabels.Length(), 0);
}

// Test triangulated face handling (suitable for STL)
TEST_F(DESTL_ProviderTest, TriangulatedFaceHandling)
{
  // Use our triangulated face which already has mesh data
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("triangulated_face.stl", anOStream));

  EXPECT_TRUE(myProvider->Write(aWriteStreams, myTriangularFace));

  std::string aStlContent = anOStream.str();
  EXPECT_FALSE(aStlContent.empty());
  EXPECT_GT(aStlContent.size(), 100); // Should have meaningful content

  // Read back
  std::istringstream          anIStream(aStlContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("triangulated_face.stl", anIStream));

  TopoDS_Shape aReadShape;
  EXPECT_TRUE(myProvider->Read(aReadStreams, aReadShape));
  EXPECT_FALSE(aReadShape.IsNull());
}

// Test DE_Wrapper with different file extensions
TEST_F(DESTL_ProviderTest, DE_WrapperFileExtensions)
{
  DE_Wrapper                      aWrapper;
  Handle(DESTL_ConfigurationNode) aNode = new DESTL_ConfigurationNode();
  EXPECT_TRUE(aWrapper.Bind(aNode));

  // Test different STL extensions
  std::vector<std::string> aExtensions = {"test.stl", "test.STL", "mesh.stl"};

  for (const auto& anExt : aExtensions)
  {
    std::ostringstream           anOStream;
    DE_Provider::WriteStreamList aWriteStreams;
    aWriteStreams.Append(DE_Provider::WriteStreamNode(anExt.c_str(), anOStream));

    EXPECT_TRUE(aWrapper.Write(aWriteStreams, myTriangularFace))
      << "Failed to write with extension: " << anExt;

    std::string aContent = anOStream.str();
    EXPECT_FALSE(aContent.empty()) << "Empty content for extension: " << anExt;

    // Test read back
    std::istringstream          anIStream(aContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode(anExt.c_str(), anIStream));

    TopoDS_Shape aReadShape;
    EXPECT_TRUE(aWrapper.Read(aReadStreams, aReadShape))
      << "Failed to read with extension: " << anExt;
    EXPECT_FALSE(aReadShape.IsNull()) << "Null shape read with extension: " << anExt;
  }
}

// Test stream operations with empty and invalid data
TEST_F(DESTL_ProviderTest, StreamErrorConditions)
{
  // Test empty stream list
  DE_Provider::WriteStreamList anEmptyWriteStreams;
  EXPECT_FALSE(myProvider->Write(anEmptyWriteStreams, myTriangularFace));

  DE_Provider::ReadStreamList anEmptyReadStreams;
  TopoDS_Shape                aShape;
  EXPECT_FALSE(myProvider->Read(anEmptyReadStreams, aShape));

  // Test corrupted STL data
  std::string                 aCorruptedData = "This is not STL data at all";
  std::istringstream          aCorruptedStream(aCorruptedData);
  DE_Provider::ReadStreamList aCorruptedReadStreams;
  aCorruptedReadStreams.Append(DE_Provider::ReadStreamNode("corrupted.stl", aCorruptedStream));

  TopoDS_Shape aCorruptedShape;
  EXPECT_FALSE(myProvider->Read(aCorruptedReadStreams, aCorruptedShape));

  // Test partial STL data
  std::string                 aPartialData = "solid test\n  facet normal 0 0 1\n"; // Incomplete
  std::istringstream          aPartialStream(aPartialData);
  DE_Provider::ReadStreamList aPartialReadStreams;
  aPartialReadStreams.Append(DE_Provider::ReadStreamNode("partial.stl", aPartialStream));

  TopoDS_Shape aPartialShape;
  EXPECT_FALSE(myProvider->Read(aPartialReadStreams, aPartialShape));
}

// Test configuration parameter validation
TEST_F(DESTL_ProviderTest, ConfigurationParameterValidation)
{
  Handle(DESTL_ConfigurationNode) aNode =
    Handle(DESTL_ConfigurationNode)::DownCast(myProvider->GetNode());

  // Test valid merge angle
  aNode->InternalParameters.ReadMergeAngle = 30.0;
  EXPECT_DOUBLE_EQ(30.0, aNode->InternalParameters.ReadMergeAngle);

  // Test edge case merge angles
  aNode->InternalParameters.ReadMergeAngle = 0.0; // Minimum
  EXPECT_DOUBLE_EQ(0.0, aNode->InternalParameters.ReadMergeAngle);

  aNode->InternalParameters.ReadMergeAngle = 90.0; // Maximum
  EXPECT_DOUBLE_EQ(90.0, aNode->InternalParameters.ReadMergeAngle);

  // Test BRep vs triangulation modes
  aNode->InternalParameters.ReadBRep = Standard_True;
  EXPECT_TRUE(aNode->InternalParameters.ReadBRep);

  aNode->InternalParameters.ReadBRep = Standard_False;
  EXPECT_FALSE(aNode->InternalParameters.ReadBRep);
}

// Test multiple triangulated faces
TEST_F(DESTL_ProviderTest, MultipleTriangulatedFaces)
{
  // Create another triangulated face with different geometry
  TColgp_Array1OfPnt aNodes(1, 3);
  aNodes.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aNodes.SetValue(2, gp_Pnt(5.0, 0.0, 0.0));
  aNodes.SetValue(3, gp_Pnt(2.5, 5.0, 0.0));

  Poly_Array1OfTriangle aTriangles(1, 1);
  aTriangles.SetValue(1, Poly_Triangle(1, 2, 3));

  Handle(Poly_Triangulation) aTriangulation = new Poly_Triangulation(aNodes, aTriangles);

  gp_Pln                  aPlane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(gp_Dir::D::Z));
  BRepBuilderAPI_MakeFace aFaceBuilder(aPlane, 0.0, 5.0, 0.0, 5.0);
  TopoDS_Face             aTriangleFace = aFaceBuilder.Face();

  BRep_Builder aBuilder;
  aBuilder.UpdateFace(aTriangleFace, aTriangulation);

  // Test first triangulated face
  std::ostringstream           aStream1;
  DE_Provider::WriteStreamList aWriteStreams1;
  aWriteStreams1.Append(DE_Provider::WriteStreamNode("face1.stl", aStream1));

  EXPECT_TRUE(myProvider->Write(aWriteStreams1, myTriangularFace));
  std::string aContent1 = aStream1.str();

  // Test second triangulated face
  std::ostringstream           aStream2;
  DE_Provider::WriteStreamList aWriteStreams2;
  aWriteStreams2.Append(DE_Provider::WriteStreamNode("face2.stl", aStream2));

  EXPECT_TRUE(myProvider->Write(aWriteStreams2, aTriangleFace));
  std::string aContent2 = aStream2.str();

  // Both content should be non-empty
  EXPECT_FALSE(aContent1.empty());
  EXPECT_FALSE(aContent2.empty());

  // Different triangulated faces should produce different STL content
  EXPECT_NE(aContent1, aContent2);

  // Both should read back successfully
  std::istringstream          aIStream1(aContent1);
  DE_Provider::ReadStreamList aReadStreams1;
  aReadStreams1.Append(DE_Provider::ReadStreamNode("face1.stl", aIStream1));

  TopoDS_Shape aReadShape1;
  EXPECT_TRUE(myProvider->Read(aReadStreams1, aReadShape1));
  EXPECT_FALSE(aReadShape1.IsNull());

  std::istringstream          aIStream2(aContent2);
  DE_Provider::ReadStreamList aReadStreams2;
  aReadStreams2.Append(DE_Provider::ReadStreamNode("face2.stl", aIStream2));

  TopoDS_Shape aReadShape2;
  EXPECT_TRUE(myProvider->Read(aReadStreams2, aReadShape2));
  EXPECT_FALSE(aReadShape2.IsNull());
}
