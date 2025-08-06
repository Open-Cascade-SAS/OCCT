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

#include <DEIGES_Provider.hxx>
#include <DEIGES_ConfigurationNode.hxx>
#include <DE_Wrapper.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSControl_WorkSession.hxx>

#include <sstream>
#include <gtest/gtest.h>

class DEIGES_ProviderTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Initialize provider with default configuration
    Handle(DEIGES_ConfigurationNode) aNode = new DEIGES_ConfigurationNode();
    myProvider = new DEIGES_Provider(aNode);
    
    // Create test BRep shapes (perfect for IGES format)
    myBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
    mySphere = BRepPrimAPI_MakeSphere(5.0).Shape();
    myCylinder = BRepPrimAPI_MakeCylinder(3.0, 8.0).Shape();
    
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

  // Helper method to validate IGES content
  bool IsValidIGESContent(const std::string& theContent)
  {
    return !theContent.empty() &&
           theContent.find("START") != std::string::npos &&
           theContent.find("GLOBAL") != std::string::npos &&
           theContent.find("DIRECTORY") != std::string::npos &&
           theContent.find("PARAMETER") != std::string::npos;
  }

protected:
  Handle(DEIGES_Provider) myProvider;
  TopoDS_Shape myBox;
  TopoDS_Shape mySphere;
  TopoDS_Shape myCylinder;
  Handle(TDocStd_Document) myDocument;
};

// Test basic provider creation and format/vendor information
TEST_F(DEIGES_ProviderTest, BasicProperties)
{
  EXPECT_STREQ("IGES", myProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", myProvider->GetVendor().ToCString());
  EXPECT_FALSE(myProvider->GetNode().IsNull());
}

// Test stream-based shape write and read operations
TEST_F(DEIGES_ProviderTest, StreamShapeWriteRead)
{
  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("test.iges", anOStream));

  // Write box shape to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myBox));
  
  std::string aIgesContent = anOStream.str();
  EXPECT_FALSE(aIgesContent.empty());
  EXPECT_TRUE(IsValidIGESContent(aIgesContent));

  if (!aIgesContent.empty()) {
    // Read back from stream
    std::istringstream anIStream(aIgesContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("test.iges", anIStream));
    
    TopoDS_Shape aReadShape;
    EXPECT_TRUE(myProvider->Read(aReadStreams, aReadShape));
    EXPECT_FALSE(aReadShape.IsNull());
    
    if (!aReadShape.IsNull()) {
      // IGES should preserve solid geometry
      Standard_Integer aReadSolids = CountShapeElements(aReadShape, TopAbs_SOLID);
      Standard_Integer aOriginalSolids = CountShapeElements(myBox, TopAbs_SOLID);
      EXPECT_EQ(aReadSolids, aOriginalSolids);
    }
  }
}

// Test stream-based document write and read operations
TEST_F(DEIGES_ProviderTest, StreamDocumentWriteRead)
{
  // Add box to document
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label aShapeLabel = aShapeTool->AddShape(myBox);
  EXPECT_FALSE(aShapeLabel.IsNull());

  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("document.iges", anOStream));

  // Write document to stream
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument));
  
  std::string aIgesContent = anOStream.str();
  EXPECT_FALSE(aIgesContent.empty());
  EXPECT_TRUE(IsValidIGESContent(aIgesContent));

  if (!aIgesContent.empty()) {
    // Create new document for reading
    Handle(TDocStd_Application) anApp = new TDocStd_Application();
    Handle(TDocStd_Document) aNewDocument;
    anApp->NewDocument("BinXCAF", aNewDocument);

    // Read back from stream
    std::istringstream anIStream(aIgesContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("document.iges", anIStream));
    
    EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument));
    
    // Validate document content
    Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
    TDF_LabelSequence aLabels;
    aNewShapeTool->GetShapes(aLabels);
    EXPECT_GT(aLabels.Length(), 0);  // Should have at least one shape in document
  }
}

// Test DE_Wrapper integration for IGES operations
TEST_F(DEIGES_ProviderTest, DE_WrapperIntegration)
{
  // Initialize DE_Wrapper and bind IGES provider
  DE_Wrapper aWrapper;
  Handle(DEIGES_ConfigurationNode) aNode = new DEIGES_ConfigurationNode();
  
  // Bind the configured node to wrapper
  EXPECT_TRUE(aWrapper.Bind(aNode));

  // Test write with DE_Wrapper using sphere
  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("test.iges", anOStream));

  EXPECT_TRUE(aWrapper.Write(aWriteStreams, mySphere));
  
  std::string aIgesContent = anOStream.str();
  EXPECT_FALSE(aIgesContent.empty());
  EXPECT_TRUE(IsValidIGESContent(aIgesContent));

  if (!aIgesContent.empty()) {
    // Test DE_Wrapper stream operations
    std::istringstream anIStream(aIgesContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode("test.iges", anIStream));
    
    TopoDS_Shape aReadShape;
    bool aWrapperResult = aWrapper.Read(aReadStreams, aReadShape);
    
    // Test direct provider with same content for comparison
    std::istringstream anIStream2(aIgesContent);
    DE_Provider::ReadStreamList aReadStreams2;
    aReadStreams2.Append(DE_Provider::ReadStreamNode("test.iges", anIStream2));
    
    Handle(DEIGES_Provider) aDirectProvider = new DEIGES_Provider(aNode);
    TopoDS_Shape aDirectShape;
    bool aDirectResult = aDirectProvider->Read(aReadStreams2, aDirectShape);
    
    // REQUIREMENT: DE_Wrapper must work exactly the same as direct provider
    EXPECT_EQ(aWrapperResult, aDirectResult);
    EXPECT_EQ(aReadShape.IsNull(), aDirectShape.IsNull());
    
    if (aDirectResult && !aDirectShape.IsNull()) {
      Standard_Integer aSolids = CountShapeElements(aDirectShape, TopAbs_SOLID);
      EXPECT_GT(aSolids, 0);
    }
  }
}

// Test multiple shapes in single document
TEST_F(DEIGES_ProviderTest, MultipleShapesInDocument)
{
  // Add multiple shapes to document
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label aBoxLabel = aShapeTool->AddShape(myBox);
  TDF_Label aSphereLabel = aShapeTool->AddShape(mySphere);
  TDF_Label aCylinderLabel = aShapeTool->AddShape(myCylinder);
  
  EXPECT_FALSE(aBoxLabel.IsNull());
  EXPECT_FALSE(aSphereLabel.IsNull());
  EXPECT_FALSE(aCylinderLabel.IsNull());

  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("multi_shapes.iges", anOStream));

  // Write document with multiple shapes
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument));
  
  std::string aIgesContent = anOStream.str();
  EXPECT_FALSE(aIgesContent.empty());
  EXPECT_TRUE(IsValidIGESContent(aIgesContent));

  // Read back into new document
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  Handle(TDocStd_Document) aNewDocument;
  anApp->NewDocument("BinXCAF", aNewDocument);

  std::istringstream anIStream(aIgesContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("multi_shapes.iges", anIStream));
  
  EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument));
  
  // Validate document content
  Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
  TDF_LabelSequence aLabels;
  aNewShapeTool->GetShapes(aLabels);
  EXPECT_EQ(aLabels.Length(), 3);  // Should have exactly 3 shapes in document
}

// Test different BRep geometry types
TEST_F(DEIGES_ProviderTest, DifferentBRepGeometries)
{
  // Test box geometry
  std::ostringstream aBoxStream;
  DE_Provider::WriteStreamList aBoxWriteStreams;
  aBoxWriteStreams.Append(DE_Provider::WriteStreamNode("box.iges", aBoxStream));
  
  EXPECT_TRUE(myProvider->Write(aBoxWriteStreams, myBox));
  std::string aBoxContent = aBoxStream.str();
  
  // Test sphere geometry
  std::ostringstream aSphereStream;
  DE_Provider::WriteStreamList aSphereWriteStreams;
  aSphereWriteStreams.Append(DE_Provider::WriteStreamNode("sphere.iges", aSphereStream));
  
  EXPECT_TRUE(myProvider->Write(aSphereWriteStreams, mySphere));
  std::string aSphereContent = aSphereStream.str();
  
  // Test cylinder geometry
  std::ostringstream aCylinderStream;
  DE_Provider::WriteStreamList aCylinderWriteStreams;
  aCylinderWriteStreams.Append(DE_Provider::WriteStreamNode("cylinder.iges", aCylinderStream));
  
  EXPECT_TRUE(myProvider->Write(aCylinderWriteStreams, myCylinder));
  std::string aCylinderContent = aCylinderStream.str();
  
  // All content should be valid IGES format
  EXPECT_TRUE(IsValidIGESContent(aBoxContent));
  EXPECT_TRUE(IsValidIGESContent(aSphereContent));
  EXPECT_TRUE(IsValidIGESContent(aCylinderContent));
  
  // Different geometries should produce different IGES content
  EXPECT_NE(aBoxContent, aSphereContent);
  EXPECT_NE(aBoxContent, aCylinderContent);
  EXPECT_NE(aSphereContent, aCylinderContent);
  
  // All should read back successfully
  std::istringstream aBoxIStream(aBoxContent);
  DE_Provider::ReadStreamList aBoxReadStreams;
  aBoxReadStreams.Append(DE_Provider::ReadStreamNode("box.iges", aBoxIStream));
  
  TopoDS_Shape aBoxReadShape;
  EXPECT_TRUE(myProvider->Read(aBoxReadStreams, aBoxReadShape));
  EXPECT_FALSE(aBoxReadShape.IsNull());
  
  std::istringstream aSphereIStream(aSphereContent);
  DE_Provider::ReadStreamList aSphereReadStreams;
  aSphereReadStreams.Append(DE_Provider::ReadStreamNode("sphere.iges", aSphereIStream));
  
  TopoDS_Shape aSphereReadShape;
  EXPECT_TRUE(myProvider->Read(aSphereReadStreams, aSphereReadShape));
  EXPECT_FALSE(aSphereReadShape.IsNull());
}

// Test DE_Wrapper with different file extensions
TEST_F(DEIGES_ProviderTest, DE_WrapperFileExtensions)
{
  DE_Wrapper aWrapper;
  Handle(DEIGES_ConfigurationNode) aNode = new DEIGES_ConfigurationNode();
  EXPECT_TRUE(aWrapper.Bind(aNode));

  // Test different IGES extensions
  std::vector<std::string> aExtensions = {"test.iges", "test.IGES", "test.igs", "test.IGS"};
  
  for (const auto& anExt : aExtensions) {
    std::ostringstream anOStream;
    DE_Provider::WriteStreamList aWriteStreams;
    aWriteStreams.Append(DE_Provider::WriteStreamNode(anExt.c_str(), anOStream));

    EXPECT_TRUE(aWrapper.Write(aWriteStreams, myBox)) 
      << "Failed to write with extension: " << anExt;
    
    std::string aContent = anOStream.str();
    EXPECT_FALSE(aContent.empty()) << "Empty content for extension: " << anExt;
    EXPECT_TRUE(IsValidIGESContent(aContent)) << "Invalid IGES content for extension: " << anExt;
    
    // Test read back
    std::istringstream anIStream(aContent);
    DE_Provider::ReadStreamList aReadStreams;
    aReadStreams.Append(DE_Provider::ReadStreamNode(anExt.c_str(), anIStream));
    
    TopoDS_Shape aReadShape;
    EXPECT_TRUE(aWrapper.Read(aReadStreams, aReadShape))
      << "Failed to read with extension: " << anExt;
    EXPECT_FALSE(aReadShape.IsNull()) << "Null shape read with extension: " << anExt;
  }
}

// Test error conditions and edge cases
TEST_F(DEIGES_ProviderTest, ErrorHandling)
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
  aWriteStreams.Append(DE_Provider::WriteStreamNode("null_test.iges", anOStream));
  TopoDS_Shape aNullShape;
  
  // Writing null shape should fail
  EXPECT_FALSE(myProvider->Write(aWriteStreams, aNullShape));
  
  // Test reading invalid IGES content
  std::string anInvalidContent = "This is not valid IGES content";
  std::istringstream anInvalidStream(anInvalidContent);
  DE_Provider::ReadStreamList anInvalidReadStreams;
  anInvalidReadStreams.Append(DE_Provider::ReadStreamNode("invalid.iges", anInvalidStream));
  
  TopoDS_Shape anInvalidShape;
  EXPECT_FALSE(myProvider->Read(anInvalidReadStreams, anInvalidShape));
  
  // Test with null document
  Handle(TDocStd_Document) aNullDoc;
  EXPECT_FALSE(myProvider->Write(aWriteStreams, aNullDoc));
  EXPECT_FALSE(myProvider->Read(anEmptyReadStreams, aNullDoc));
}

// Test DEIGES configuration modes
TEST_F(DEIGES_ProviderTest, ConfigurationModes)
{
  Handle(DEIGES_ConfigurationNode) aNode = Handle(DEIGES_ConfigurationNode)::DownCast(myProvider->GetNode());
  
  // Test basic configuration access
  EXPECT_FALSE(aNode.IsNull());
  
  // Test provider format and vendor are correct
  EXPECT_STREQ("IGES", myProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", myProvider->GetVendor().ToCString());
  
  // Test that we can create provider with different configuration
  Handle(DEIGES_ConfigurationNode) aNewNode = new DEIGES_ConfigurationNode();
  Handle(DEIGES_Provider) aNewProvider = new DEIGES_Provider(aNewNode);
  
  EXPECT_STREQ("IGES", aNewProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", aNewProvider->GetVendor().ToCString());
  EXPECT_FALSE(aNewProvider->GetNode().IsNull());
}

// Test WorkSession integration
TEST_F(DEIGES_ProviderTest, WorkSessionIntegration)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  
  // Test write operation with work session
  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("ws_test.iges", anOStream));
  
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myBox, aWS));
  
  std::string aIgesContent = anOStream.str();
  EXPECT_FALSE(aIgesContent.empty());
  EXPECT_TRUE(IsValidIGESContent(aIgesContent));
  
  // Test read operation with work session
  std::istringstream anIStream(aIgesContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("ws_test.iges", anIStream));
  
  TopoDS_Shape aReadShape;
  EXPECT_TRUE(myProvider->Read(aReadStreams, aReadShape, aWS));
  EXPECT_FALSE(aReadShape.IsNull());
}

// Test document operations with WorkSession
TEST_F(DEIGES_ProviderTest, DocumentWorkSessionIntegration)
{
  // Add shape to document
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(myDocument->Main());
  TDF_Label aShapeLabel = aShapeTool->AddShape(mySphere);
  EXPECT_FALSE(aShapeLabel.IsNull());

  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  
  std::ostringstream anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("doc_ws_test.iges", anOStream));

  // Test document write with work session
  EXPECT_TRUE(myProvider->Write(aWriteStreams, myDocument, aWS));
  
  std::string aIgesContent = anOStream.str();
  EXPECT_FALSE(aIgesContent.empty());
  EXPECT_TRUE(IsValidIGESContent(aIgesContent));

  // Create new document for reading
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  Handle(TDocStd_Document) aNewDocument;
  anApp->NewDocument("BinXCAF", aNewDocument);

  // Test document read with work session
  std::istringstream anIStream(aIgesContent);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("doc_ws_test.iges", anIStream));
  
  EXPECT_TRUE(myProvider->Read(aReadStreams, aNewDocument, aWS));
  
  // Validate document content
  Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(aNewDocument->Main());
  TDF_LabelSequence aLabels;
  aNewShapeTool->GetShapes(aLabels);
  EXPECT_GT(aLabels.Length(), 0);
}