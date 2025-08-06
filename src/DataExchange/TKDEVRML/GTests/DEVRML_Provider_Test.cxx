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
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
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
    // Initialize provider with proper configuration node
    Handle(DEVRML_ConfigurationNode) aNode = new DEVRML_ConfigurationNode();
    myProvider = new DEVRML_Provider(aNode);
    
    // Create simple test shapes
    myBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
    mySphere = BRepPrimAPI_MakeSphere(5.0).Shape();
    
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

protected:
  Handle(DEVRML_Provider) myProvider;
  TopoDS_Shape myBox;
  TopoDS_Shape mySphere;
  Handle(TDocStd_Document) myDocument;
};

// Test basic provider creation and format/vendor information
TEST_F(DEVRML_ProviderTest, BasicProperties)
{
  EXPECT_STREQ("VRML", myProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", myProvider->GetVendor().ToCString());
  EXPECT_FALSE(myProvider->GetNode().IsNull());
}

// Test stream-based shape write and read operations
TEST_F(DEVRML_ProviderTest, StreamShapeWriteRead)
{
  // Test with box shape
  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("test.vrml", anOStream));

  // Write box to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myBox));
  
  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());
  EXPECT_TRUE(aVrmlContent.find("#VRML") != std::string::npos);

  // Read back from stream
  std::istringstream anIStream(aVrmlContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("test.vrml", anIStream));
  
  TopoDS_Shape aReadShape;
  EXPECT_TRUE(myProvider->Read(aReadStreams, aReadShape));
  EXPECT_FALSE(aReadShape.IsNull());
  
  // Validate shape structure - should have faces after reading
  Standard_Integer aReadFaces = CountShapeElements(aReadShape, TopAbs_FACE);
  EXPECT_GT(aReadFaces, 0);  // Should have at least some faces
}


// Test stream-based document write and read operations
TEST_F(DEVRML_ProviderTest, StreamDocumentWriteRead)
{
  // Add shape to document
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  aShapeTool->AddShape(myBox);

  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("document.vrml", anOStream));

  // Write document to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument));
  
  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());

  // Create new document for reading
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  Handle(TDocStd_Document) aNewDocument;
  anApp->NewDocument("BinXCAF", aNewDocument);

  // Read back from stream
  std::istringstream anIStream(aVrmlContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("document.vrml", anIStream));
  
  EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument));
  
  // Validate document content
  Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
  TDF_LabelSequence aLabels;
  aNewShapeTool->GetShapes(aLabels);
  EXPECT_GT(aLabels.Length(), 0);
}

// Test stream-based document with multiple shapes
TEST_F(DEVRML_ProviderTest, StreamDocumentMultipleShapes)
{
  // Add shapes to document
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  aShapeTool->AddShape(myBox);
  aShapeTool->AddShape(mySphere);

  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("multi_shapes.vrml", anOStream));

  // Write document to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument));
  
  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());

  // Create new document for reading
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  Handle(TDocStd_Document) aNewDocument;
  anApp->NewDocument("BinXCAF", aNewDocument);

  // Read back from stream
  std::istringstream anIStream(aVrmlContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("multi_shapes.vrml", anIStream));
  
  EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument));
  
  // Validate document content
  Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
  TDF_LabelSequence aLabels;
  aNewShapeTool->GetShapes(aLabels);
  EXPECT_GT(aLabels.Length(), 0);
}

// Test DE_Wrapper integration for VRML operations
TEST_F(DEVRML_ProviderTest, DE_WrapperIntegration)
{
  // Initialize DE_Wrapper and bind VRML provider
  DE_Wrapper aWrapper;
  Handle(DEVRML_ConfigurationNode) aNode = new DEVRML_ConfigurationNode();
  aWrapper.Bind(aNode);

  // Test write with DE_Wrapper
  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("wrapper_test.vrml", anOStream));

  EXPECT_TRUE(aWrapper.Write(aWriteStreams, myBox));
  
  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());
  EXPECT_TRUE(aVrmlContent.find("#VRML") != std::string::npos);

  // Test read with DE_Wrapper
  std::istringstream anIStream(aVrmlContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("wrapper_test.vrml", anIStream));
  
  TopoDS_Shape aReadShape;
  EXPECT_TRUE(aWrapper.Read(aReadStreams, aReadShape));
  EXPECT_FALSE(aReadShape.IsNull());
  
  // Validate read shape has geometry
  Standard_Integer aReadFaces = CountShapeElements(aReadShape, TopAbs_FACE);
  EXPECT_GT(aReadFaces, 0);
}

// Test DE_Wrapper document operations
TEST_F(DEVRML_ProviderTest, DE_WrapperDocumentOperations)
{
  // Initialize DE_Wrapper and bind VRML provider
  DE_Wrapper aWrapper;
  Handle(DEVRML_ConfigurationNode) aNode = new DEVRML_ConfigurationNode();
  aWrapper.Bind(aNode);

  // Add shape to document
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  aShapeTool->AddShape(mySphere);

  // Test document write with DE_Wrapper
  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("wrapper_doc.vrml", anOStream));

  EXPECT_TRUE(aWrapper.Write(aWriteStreams, myDocument));
  
  std::string aVrmlContent = anOStream.str();
  EXPECT_FALSE(aVrmlContent.empty());

  // Test document read with DE_Wrapper
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  Handle(TDocStd_Document) aNewDocument;
  anApp->NewDocument("BinXCAF", aNewDocument);

  std::istringstream anIStream(aVrmlContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("wrapper_doc.vrml", anIStream));
  
  EXPECT_TRUE(aWrapper.Read(aReadStreams, aNewDocument));
  
  // Validate document content
  Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
  TDF_LabelSequence aLabels;
  aNewShapeTool->GetShapes(aLabels);
  EXPECT_GT(aLabels.Length(), 0);
}

// Test error conditions
TEST_F(DEVRML_ProviderTest, ErrorHandling)
{
  // Test with empty streams
  DE_Provider::WriteStreamList anEmptyWriteStreams;
  EXPECT_FALSE(myProvider->Write(anEmptyWriteStreams, myBox));
  
  DE_Provider::ReadStreamList anEmptyReadStreams;
  TopoDS_Shape aShape;
  EXPECT_FALSE(myProvider->Read(anEmptyReadStreams, aShape));
  
  // Test with null shape
  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("null_test.vrml", anOStream));
  TopoDS_Shape aNullShape;
  
  // Writing null shape might succeed but produce empty or minimal content
  myProvider->Write(aWriteStreams, aNullShape);
  std::string aContent = anOStream.str();
  EXPECT_FALSE(aContent.empty()); // Should at least have VRML header
}