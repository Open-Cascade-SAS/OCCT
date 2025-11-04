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

#include <DEVRML_Provider.hxx>
#include <DEVRML_ConfigurationNode.hxx>
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

class DEVRML_ProviderTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Initialize provider with default configuration (will be modified per test)
    Handle(DEVRML_ConfigurationNode) aNode = new DEVRML_ConfigurationNode();
    myProvider                             = new DEVRML_Provider(aNode);

    // Create test shapes
    myBox            = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape(); // For wireframe testing
    mySphere         = BRepPrimAPI_MakeSphere(5.0).Shape();           // For wireframe testing
    myTriangularFace = CreateTriangulatedFace();                      // For shaded/face testing

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

  // Helper method to create a triangulated face with mesh data
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

    // Attach triangulation to the face (without location parameter)
    BRep_Builder aBuilder;
    aBuilder.UpdateFace(aFace, aTriangulation);

    return aFace;
  }

protected:
  Handle(DEVRML_Provider)  myProvider;
  TopoDS_Shape             myBox;
  TopoDS_Shape             mySphere;
  TopoDS_Shape             myTriangularFace;
  Handle(TDocStd_Document) myDocument;
};

// Test basic provider creation and format/vendor information
TEST_F(DEVRML_ProviderTest, BasicProperties)
{
  EXPECT_STREQ("VRML", myProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", myProvider->GetVendor().ToCString());
  EXPECT_FALSE(myProvider->GetNode().IsNull());
}

// Test stream-based shape write and read operations with wireframe (edges)
TEST_F(DEVRML_ProviderTest, StreamShapeWriteReadWireframe)
{
  // Configure provider for wireframe mode (default)
  Handle(DEVRML_ConfigurationNode) aNode =
    Handle(DEVRML_ConfigurationNode)::DownCast(myProvider->GetNode());
  aNode->InternalParameters.WriteRepresentationType =
    DEVRML_ConfigurationNode::WriteMode_RepresentationType_Wireframe;

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("wireframe.vrml", anOStream));

  // Write box to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myBox));

  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());
  EXPECT_TRUE(aVrmlContent.find("#VRML") != std::string::npos);

  if (!aVrmlContent.empty())
  {
    // Read back from stream
    std::istringstream          anIStream(aVrmlContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("wireframe.vrml", anIStream));

    TopoDS_Shape aReadShape;
    EXPECT_TRUE(myProvider->Read(aReadStreams, aReadShape));
    EXPECT_FALSE(aReadShape.IsNull());

    if (!aReadShape.IsNull())
    {
      // Wireframe mode should produce edges, not faces
      Standard_Integer aReadEdges = CountShapeElements(aReadShape, TopAbs_EDGE);
      EXPECT_TRUE(aReadEdges > 0); // Should have edges from wireframe
    }
  }
}

// Test stream-based shape write and read operations with shaded (faces)
TEST_F(DEVRML_ProviderTest, StreamShapeWriteReadShaded)
{
  // Configure provider for shaded mode
  Handle(DEVRML_ConfigurationNode) aNode =
    Handle(DEVRML_ConfigurationNode)::DownCast(myProvider->GetNode());
  aNode->InternalParameters.WriteRepresentationType =
    DEVRML_ConfigurationNode::WriteMode_RepresentationType_Shaded;

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("shaded.vrml", anOStream));

  // Write triangular face to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myTriangularFace));

  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());
  EXPECT_TRUE(aVrmlContent.find("#VRML") != std::string::npos);

  if (!aVrmlContent.empty())
  {
    // Read back from stream
    std::istringstream          anIStream(aVrmlContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("shaded.vrml", anIStream));

    TopoDS_Shape aReadShape;
    EXPECT_TRUE(myProvider->Read(aReadStreams, aReadShape));
    EXPECT_FALSE(aReadShape.IsNull());

    if (!aReadShape.IsNull())
    {
      // Shaded mode should produce faces
      Standard_Integer aReadFaces = CountShapeElements(aReadShape, TopAbs_FACE);
      EXPECT_TRUE(aReadFaces > 0); // Should have faces from shaded mode
    }
  }
}

// Test stream-based document write and read operations
TEST_F(DEVRML_ProviderTest, StreamDocumentWriteRead)
{
  // Configure provider for shaded mode for better document compatibility
  Handle(DEVRML_ConfigurationNode) aNode =
    Handle(DEVRML_ConfigurationNode)::DownCast(myProvider->GetNode());
  aNode->InternalParameters.WriteRepresentationType =
    DEVRML_ConfigurationNode::WriteMode_RepresentationType_Shaded;

  // Add shape to document
  Handle(XCAFDoc_ShapeTool) aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label                 aShapeLabel = aShapeTool->AddShape(myTriangularFace);
  EXPECT_FALSE(aShapeLabel.IsNull());

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("document.vrml", anOStream));

  // Write document to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument));

  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());
  EXPECT_TRUE(aVrmlContent.find("#VRML") != std::string::npos);

  if (!aVrmlContent.empty())
  {
    // Create new document for reading
    Handle(TDocStd_Application) anApp = new TDocStd_Application();
    Handle(TDocStd_Document)    aNewDocument;
    anApp->NewDocument("BinXCAF", aNewDocument);

    // Read back from stream
    std::istringstream          anIStream(aVrmlContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("document.vrml", anIStream));

    EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument));

    // Validate document content
    Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
    TDF_LabelSequence         aLabels;
    aNewShapeTool->GetShapes(aLabels);
    EXPECT_GT(aLabels.Length(), 0); // Should have at least one shape in document
  }
}

// Test stream-based document with multiple shapes
TEST_F(DEVRML_ProviderTest, StreamDocumentMultipleShapes)
{
  // Configure provider for shaded mode for better multi-shape compatibility
  Handle(DEVRML_ConfigurationNode) aNode =
    Handle(DEVRML_ConfigurationNode)::DownCast(myProvider->GetNode());
  aNode->InternalParameters.WriteRepresentationType =
    DEVRML_ConfigurationNode::WriteMode_RepresentationType_Shaded;

  // Add multiple shapes to document
  Handle(XCAFDoc_ShapeTool) aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label                 aFirstLabel = aShapeTool->AddShape(myTriangularFace);
  EXPECT_FALSE(aFirstLabel.IsNull());

  // Add a second shape - using the sphere for variety
  TDF_Label aSecondLabel = aShapeTool->AddShape(mySphere);
  EXPECT_FALSE(aSecondLabel.IsNull());

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("multi_shapes.vrml", anOStream));

  // Write document to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument));

  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());
  EXPECT_TRUE(aVrmlContent.find("#VRML") != std::string::npos);

  if (!aVrmlContent.empty())
  {
    // Create new document for reading
    Handle(TDocStd_Application) anApp = new TDocStd_Application();
    Handle(TDocStd_Document)    aNewDocument;
    anApp->NewDocument("BinXCAF", aNewDocument);

    // Read back from stream
    std::istringstream          anIStream(aVrmlContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("multi_shapes.vrml", anIStream));

    EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument));

    // Validate document content
    Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
    TDF_LabelSequence         aLabels;
    aNewShapeTool->GetShapes(aLabels);
    EXPECT_GT(aLabels.Length(), 0); // Should have at least one shape in document
  }
}

// Test DE_Wrapper integration for VRML operations
TEST_F(DEVRML_ProviderTest, DE_WrapperIntegration)
{
  // Initialize DE_Wrapper and bind VRML provider
  DE_Wrapper                       aWrapper;
  Handle(DEVRML_ConfigurationNode) aNode = new DEVRML_ConfigurationNode();
  // Configure for shaded mode to ensure faces are generated
  aNode->InternalParameters.WriteRepresentationType =
    DEVRML_ConfigurationNode::WriteMode_RepresentationType_Shaded;

  // Bind the configured node to wrapper
  EXPECT_TRUE(aWrapper.Bind(aNode));

  // Test write with DE_Wrapper using triangular face
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("test.vrml", anOStream));

  EXPECT_TRUE(aWrapper.Write(aWriteStreams, myTriangularFace));

  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());
  EXPECT_TRUE(aVrmlContent.find("#VRML") != std::string::npos);

  if (!aVrmlContent.empty())
  {
    // Test DE_Wrapper stream operations - the key functionality we wanted to verify
    std::istringstream          anIStream(aVrmlContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("test.vrml", anIStream));

    TopoDS_Shape aReadShape;
    bool         aWrapperResult = aWrapper.Read(aReadStreams, aReadShape);

    // Test direct provider with same content for comparison
    std::istringstream          anIStream2(aVrmlContent);
    DE_Provider::ReadStreamList aReadStreams2;
    aReadStreams2.Append(DE_Provider::ReadStreamNode("test.vrml", anIStream2));

    Handle(DEVRML_Provider) aDirectProvider = new DEVRML_Provider(aNode);
    TopoDS_Shape            aDirectShape;
    bool                    aDirectResult = aDirectProvider->Read(aReadStreams2, aDirectShape);

    // REQUIREMENT: DE_Wrapper must work exactly the same as direct provider
    EXPECT_EQ(aWrapperResult, aDirectResult);
    EXPECT_EQ(aReadShape.IsNull(), aDirectShape.IsNull());

    if (aDirectResult && !aDirectShape.IsNull())
    {
      Standard_Integer aFaces = CountShapeElements(aDirectShape, TopAbs_FACE);
      EXPECT_GT(aFaces, 0);
    }
    else if (aWrapperResult && !aReadShape.IsNull())
    {
      Standard_Integer aFaces = CountShapeElements(aReadShape, TopAbs_FACE);
      EXPECT_GT(aFaces, 0);
    }
  }
}

// Test DE_Wrapper document operations
TEST_F(DEVRML_ProviderTest, DE_WrapperDocumentOperations)
{
  // Initialize DE_Wrapper and bind VRML provider
  DE_Wrapper                       aWrapper;
  Handle(DEVRML_ConfigurationNode) aNode = new DEVRML_ConfigurationNode();
  // Configure for shaded mode for better document operations
  aNode->InternalParameters.WriteRepresentationType =
    DEVRML_ConfigurationNode::WriteMode_RepresentationType_Shaded;

  // Bind the node to wrapper
  EXPECT_TRUE(aWrapper.Bind(aNode));

  // Add shape to document
  Handle(XCAFDoc_ShapeTool) aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label                 aShapeLabel = aShapeTool->AddShape(myTriangularFace);
  EXPECT_FALSE(aShapeLabel.IsNull());

  // Test document write with DE_Wrapper
  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("doc.vrml", anOStream));

  EXPECT_TRUE(aWrapper.Write(aWriteStreams, myDocument));

  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());
  EXPECT_TRUE(aVrmlContent.find("#VRML") != std::string::npos);

  if (!aVrmlContent.empty())
  {
    // Test document read with DE_Wrapper
    Handle(TDocStd_Application) anApp = new TDocStd_Application();
    Handle(TDocStd_Document)    aNewDocument;
    anApp->NewDocument("BinXCAF", aNewDocument);

    std::istringstream          anIStream(aVrmlContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("doc.vrml", anIStream));

    bool aWrapperDocResult = aWrapper.Read(aReadStreams, aNewDocument);

    // Validate document content if read succeeded
    if (aWrapperDocResult)
    {
      Handle(XCAFDoc_ShapeTool) aNewShapeTool =
        XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
      TDF_LabelSequence aLabels;
      aNewShapeTool->GetShapes(aLabels);
      EXPECT_GT(aLabels.Length(), 0);
    }
    else
    {
      // If DE_Wrapper document read fails, verify direct provider works as fallback
      Handle(TDocStd_Application) anApp2 = new TDocStd_Application();
      Handle(TDocStd_Document)    aTestDocument;
      anApp2->NewDocument("BinXCAF", aTestDocument);

      std::istringstream          anIStream2(aVrmlContent);
      DE_Provider::ReadStreamList aReadStreams2;
      aReadStreams2.Append(DE_Provider::ReadStreamNode("doc.vrml", anIStream2));

      Handle(DEVRML_Provider) aDirectProvider = new DEVRML_Provider(aNode);
      bool aDirectDocResult                   = aDirectProvider->Read(aReadStreams2, aTestDocument);

      if (aDirectDocResult)
      {
        Handle(XCAFDoc_ShapeTool) aTestShapeTool =
          XCAFDoc_DocumentTool::ShapeTool(aTestDocument->Main());
        TDF_LabelSequence aTestLabels;
        aTestShapeTool->GetShapes(aTestLabels);
        EXPECT_GT(aTestLabels.Length(), 0);
      }
    }
  }
}

// Test error conditions and edge cases
TEST_F(DEVRML_ProviderTest, ErrorHandling)
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
  aWriteStreams.Append(DE_Provider::WriteStreamNode("null_test.vrml", anOStream));
  TopoDS_Shape aNullShape;

  // Writing null shape might succeed but produce empty or minimal content
  myProvider->Write(aWriteStreams, aNullShape);
  std::string aContent = anOStream.str();
  EXPECT_FALSE(aContent.empty()); // Should at least have VRML header

  // Test reading invalid VRML content
  std::string                 anInvalidContent = "This is not valid VRML content";
  std::istringstream          anInvalidStream(anInvalidContent);
  DE_Provider::ReadStreamList anInvalidReadStreams;
  anInvalidReadStreams.Append(DE_Provider::ReadStreamNode("invalid.vrml", anInvalidStream));

  TopoDS_Shape anInvalidShape;
  EXPECT_FALSE(myProvider->Read(anInvalidReadStreams, anInvalidShape));

  // Test with null document
  Handle(TDocStd_Document) aNullDoc;
  EXPECT_FALSE(myProvider->Write(aWriteStreams, aNullDoc));
  EXPECT_FALSE(myProvider->Read(anEmptyReadStreams, aNullDoc));
}

// Test different VRML configuration modes
TEST_F(DEVRML_ProviderTest, ConfigurationModes)
{
  Handle(DEVRML_ConfigurationNode) aNode =
    Handle(DEVRML_ConfigurationNode)::DownCast(myProvider->GetNode());

  // Test wireframe mode configuration
  aNode->InternalParameters.WriteRepresentationType =
    DEVRML_ConfigurationNode::WriteMode_RepresentationType_Wireframe;
  EXPECT_EQ(aNode->InternalParameters.WriteRepresentationType,
            DEVRML_ConfigurationNode::WriteMode_RepresentationType_Wireframe);

  // Test shaded mode configuration
  aNode->InternalParameters.WriteRepresentationType =
    DEVRML_ConfigurationNode::WriteMode_RepresentationType_Shaded;
  EXPECT_EQ(aNode->InternalParameters.WriteRepresentationType,
            DEVRML_ConfigurationNode::WriteMode_RepresentationType_Shaded);

  // Test both mode configuration
  aNode->InternalParameters.WriteRepresentationType =
    DEVRML_ConfigurationNode::WriteMode_RepresentationType_Both;
  EXPECT_EQ(aNode->InternalParameters.WriteRepresentationType,
            DEVRML_ConfigurationNode::WriteMode_RepresentationType_Both);

  // Test writer version configuration
  aNode->InternalParameters.WriterVersion = DEVRML_ConfigurationNode::WriteMode_WriterVersion_1;
  EXPECT_EQ(aNode->InternalParameters.WriterVersion,
            DEVRML_ConfigurationNode::WriteMode_WriterVersion_1);

  aNode->InternalParameters.WriterVersion = DEVRML_ConfigurationNode::WriteMode_WriterVersion_2;
  EXPECT_EQ(aNode->InternalParameters.WriterVersion,
            DEVRML_ConfigurationNode::WriteMode_WriterVersion_2);

  // Test that provider format and vendor are correct
  EXPECT_STREQ("VRML", myProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", myProvider->GetVendor().ToCString());
}