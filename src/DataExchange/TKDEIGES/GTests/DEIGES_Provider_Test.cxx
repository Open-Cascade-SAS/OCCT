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
#include <DE_Wrapper.hxx>
#include <DEIGES_Provider.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESFile_Read.hxx>
#include <NCollection_Sequence.hxx>
#include <TDF_Label.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDocStd_Document.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <gtest/gtest.h>

#include <mutex>
#include <sstream>

namespace
{
//! Serializes tests because the legacy IGES reader stores parser state in process-global data.
std::mutex THE_IGES_TEST_MUTEX;

class DEIGES_ProviderTest : public testing::Test
{
public:
  DEIGES_ProviderTest()
      : myLock(THE_IGES_TEST_MUTEX)
  {
  }

private:
  std::unique_lock<std::mutex> myLock;
};

//! Creates an IGES provider with default configuration.
//! @return configured provider
occ::handle<DEIGES_Provider> createProvider()
{
  occ::handle<DEIGES_ConfigurationNode> aNode = new DEIGES_ConfigurationNode();
  return new DEIGES_Provider(aNode);
}

//! Creates an empty XCAF document.
//! @return empty document
occ::handle<TDocStd_Document> createDocument()
{
  return new TDocStd_Document("BinXCAF");
}

//! Creates a box shape.
//! @return box shape, or a null shape if construction failed
TopoDS_Shape createBoxShape()
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aBoxMaker.Build();
  return aBoxMaker.IsDone() ? aBoxMaker.Shape() : TopoDS_Shape();
}

//! Counts subshapes of the requested type.
//! @param[in] theShape shape to inspect
//! @param[in] theType subshape type
//! @return number of matching subshapes
int countShapeElements(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
{
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
  {
    ++aCount;
  }
  return aCount;
}

//! Checks that content consists of valid 80-column records and contains all IGES sections.
//! @param[in] theContent serialized IGES content
//! @return true if all mandatory sections are present
bool isValidIGESContent(const TCollection_AsciiString& theContent)
{
  bool         aFoundSections[5] = {};
  const char*  aContent          = theContent.ToCString();
  const size_t aSize             = static_cast<size_t>(theContent.Length());
  size_t       aLineStart        = 0;
  for (size_t anEnd = 0; anEnd <= aSize; ++anEnd)
  {
    if (anEnd < aSize && aContent[anEnd] != '\n')
    {
      continue;
    }
    if (aLineStart >= aSize)
    {
      break; // A trailing newline does not introduce an empty record.
    }
    size_t aLength = anEnd - aLineStart;
    if (aLength > 0 && aContent[anEnd - 1] == '\r')
    {
      --aLength;
    }
    if (aLength != 80)
    {
      return false;
    }
    switch (aContent[aLineStart + 72])
    {
      case 'S':
        aFoundSections[0] = true;
        break;
      case 'G':
        aFoundSections[1] = true;
        break;
      case 'D':
        aFoundSections[2] = true;
        break;
      case 'P':
        aFoundSections[3] = true;
        break;
      case 'T':
        aFoundSections[4] = true;
        break;
      default:
        return false;
    }
    aLineStart = anEnd + 1;
  }
  return aFoundSections[0] && aFoundSections[1] && aFoundSections[2] && aFoundSections[3]
         && aFoundSections[4];
}

//! Serializes a shape to IGES content.
//! @param[in] theProvider provider used for writing
//! @param[in] theShape shape to serialize
//! @param[out] theContent serialized content
//! @return true if serialization succeeded
bool writeShape(const occ::handle<DEIGES_Provider>& theProvider,
                const TopoDS_Shape&                 theShape,
                TCollection_AsciiString&            theContent)
{
  std::ostringstream           anOutputStream;
  DE_Provider::WriteStreamList aStreams;
  aStreams.Append(DE_Provider::WriteStreamNode("shape.igs", anOutputStream));
  if (!theProvider->Write(aStreams, theShape))
  {
    return false;
  }
  theContent = anOutputStream.str().c_str();
  return true;
}

//! Reads a shape from IGES content.
//! @param[in] theProvider provider used for reading
//! @param[in] theContent serialized content
//! @param[out] theShape translated shape
//! @return true if reading and translation succeeded
bool readShape(const occ::handle<DEIGES_Provider>& theProvider,
               const TCollection_AsciiString&      theContent,
               TopoDS_Shape&                       theShape)
{
  std::istringstream          anInputStream(theContent.ToCString());
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("shape.igs", anInputStream));
  return theProvider->Read(aStreams, theShape);
}

//! Reads IGES text directly into a model without transferring its entities.
//! @param[in] theContent serialized IGES content
//! @param[in] theIsFNES whether FNES compatibility mode is enabled
//! @return parsed model, or a null handle on failure
occ::handle<IGESData_IGESModel> readModel(const TCollection_AsciiString& theContent,
                                          bool                           theIsFNES = false)
{
  std::istringstream                   anInputStream(theContent.ToCString());
  occ::handle<IGESData_IGESModel>      aModel    = new IGESData_IGESModel();
  occ::handle<IGESData_Protocol>       aProtocol = new IGESData_Protocol();
  occ::handle<IGESData_FileRecognizer> aRecognizer;
  const int                            aStatus =
    IGESFile_Read("memory.igs", anInputStream, aModel, aProtocol, aRecognizer, theIsFNES);
  return aStatus == 0 ? aModel : occ::handle<IGESData_IGESModel>();
}

} // namespace

TEST_F(DEIGES_ProviderTest, BasicProperties)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  EXPECT_STREQ("IGES", aProvider->GetFormat().ToCString());
  EXPECT_STREQ("OCC", aProvider->GetVendor().ToCString());
  EXPECT_FALSE(aProvider->GetNode().IsNull());
}

TEST_F(DEIGES_ProviderTest, StreamShapeWriteRead)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  const TopoDS_Shape                 aBox      = createBoxShape();
  ASSERT_FALSE(aBox.IsNull());

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("shape.igs", anOStream));

  EXPECT_TRUE(aProvider->Write(aWriteStreams, aBox));

  const TCollection_AsciiString anIgesContent(anOStream.str().c_str());
  EXPECT_TRUE(isValidIGESContent(anIgesContent));

  std::istringstream          anIStream(anIgesContent.ToCString());
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("shape.igs", anIStream));

  TopoDS_Shape aReadShape;
  ASSERT_TRUE(aProvider->Read(aReadStreams, aReadShape));
  ASSERT_FALSE(aReadShape.IsNull());
  EXPECT_EQ(countShapeElements(aReadShape, TopAbs_FACE), 6);
}

TEST_F(DEIGES_ProviderTest, StreamDocumentWriteRead)
{
  const occ::handle<DEIGES_Provider>  aProvider = createProvider();
  const occ::handle<TDocStd_Document> aDocument = createDocument();
  const TopoDS_Shape                  aBox      = createBoxShape();
  ASSERT_FALSE(aBox.IsNull());

  occ::handle<XCAFDoc_ShapeTool> aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label                aShapeLabel = aShapeTool->AddShape(aBox);
  EXPECT_FALSE(aShapeLabel.IsNull());

  std::ostringstream           anOStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("document.igs", anOStream));

  EXPECT_TRUE(aProvider->Write(aWriteStreams, aDocument));

  const TCollection_AsciiString anIgesContent(anOStream.str().c_str());
  EXPECT_TRUE(isValidIGESContent(anIgesContent));

  const occ::handle<TDocStd_Document> aReadDocument = createDocument();

  std::istringstream          anIStream(anIgesContent.ToCString());
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("document.igs", anIStream));

  EXPECT_TRUE(aProvider->Read(aReadStreams, aReadDocument));

  occ::handle<XCAFDoc_ShapeTool> aReadShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aReadDocument->Main());
  NCollection_Sequence<TDF_Label> aLabels;
  aReadShapeTool->GetShapes(aLabels);
  EXPECT_EQ(aLabels.Length(), 1);
}

TEST_F(DEIGES_ProviderTest, ReadStream_CRRecordSeparators_ReadsShape)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  const TopoDS_Shape                 aBox      = createBoxShape();
  ASSERT_FALSE(aBox.IsNull());

  TCollection_AsciiString anIGESContent;
  ASSERT_TRUE(writeShape(aProvider, aBox, anIGESContent));

  TCollection_AsciiString aCRContent;
  for (int anIndex = 1; anIndex <= anIGESContent.Length(); ++anIndex)
  {
    const char aCharacter = anIGESContent.Value(anIndex);
    if (aCharacter == '\n')
    {
      aCRContent += '\r';
    }
    else if (aCharacter != '\r')
    {
      aCRContent += aCharacter;
    }
  }

  TopoDS_Shape aReadShape;
  EXPECT_TRUE(readShape(aProvider, aCRContent, aReadShape));
  EXPECT_FALSE(aReadShape.IsNull());
}

TEST_F(DEIGES_ProviderTest, ReadStream_CRInsideFixedWidthRecord_PreservesStartRecord)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  const TopoDS_Shape                 aBox      = createBoxShape();
  ASSERT_FALSE(aBox.IsNull());

  TCollection_AsciiString anIGESContent;
  ASSERT_TRUE(writeShape(aProvider, aBox, anIGESContent));
  ASSERT_GE(anIGESContent.Length(), 80u);
  ASSERT_EQ(anIGESContent.Value(73), 'S');
  const TCollection_AsciiString aStartText = "Legacy fixed-width Start record";
  anIGESContent.Remove(1, 45);
  anIGESContent.Insert(1, aStartText + TCollection_AsciiString(45 - aStartText.Length(), ' '));
  anIGESContent.SetValue(46, '\r');

  const occ::handle<IGESData_IGESModel> aModel = readModel(anIGESContent);
  ASSERT_FALSE(aModel.IsNull());
  EXPECT_EQ(aModel->NbStartLines(), 1);
  EXPECT_EQ(aModel->NbEntities(), 49);
}

TEST_F(DEIGES_ProviderTest, ReadFile_EmptyName_ReturnsOpenError)
{
  char                                 anEmptyName[] = "";
  occ::handle<IGESData_IGESModel>      aModel        = new IGESData_IGESModel();
  occ::handle<IGESData_Protocol>       aProtocol     = new IGESData_Protocol();
  occ::handle<IGESData_FileRecognizer> aRecognizer;

  EXPECT_EQ(IGESFile_Read(anEmptyName, aModel, aProtocol, aRecognizer, false), -1);
}

TEST_F(DEIGES_ProviderTest, ReadStream_DelimiterFreeRecords_ReadsShape)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  const TopoDS_Shape                 aBox      = createBoxShape();
  ASSERT_FALSE(aBox.IsNull());

  TCollection_AsciiString anIGESContent;
  ASSERT_TRUE(writeShape(aProvider, aBox, anIGESContent));

  TCollection_AsciiString aFixedRecords;
  for (int anIndex = 1; anIndex <= anIGESContent.Length(); ++anIndex)
  {
    const char aCharacter = anIGESContent.Value(anIndex);
    if (aCharacter != '\r' && aCharacter != '\n')
    {
      aFixedRecords += aCharacter;
    }
  }

  TopoDS_Shape aReadShape;
  EXPECT_TRUE(readShape(aProvider, aFixedRecords, aReadShape));
  EXPECT_FALSE(aReadShape.IsNull());
}

TEST_F(DEIGES_ProviderTest, ReadStream_ShiftedDExponentRecord_RestoresValue)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  TCollection_AsciiString            anIGESContent;
  ASSERT_TRUE(writeShape(aProvider, createBoxShape(), anIGESContent));
  ASSERT_GE(anIGESContent.Length(), 81u);

  const TCollection_AsciiString aStartValue = ".123D-4";
  anIGESContent.Remove(1, 72);
  anIGESContent.Insert(1, aStartValue + TCollection_AsciiString(72 - aStartValue.Length(), ' '));
  ASSERT_EQ(anIGESContent.Value(73), 'S');
  anIGESContent.Remove(1, 1);

  const occ::handle<IGESData_IGESModel> aModel = readModel(anIGESContent);
  ASSERT_FALSE(aModel.IsNull());
  ASSERT_EQ(aModel->NbStartLines(), 1);
  EXPECT_STREQ(aStartValue.ToCString(), aModel->StartLine(1));
}

TEST_F(DEIGES_ProviderTest, ReadStream_PlainPreamble_IsSkipped)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  TCollection_AsciiString            anIGESContent;
  ASSERT_TRUE(writeShape(aProvider, createBoxShape(), anIGESContent));

  const TCollection_AsciiString         aPreamble = "FNES compatibility preamble\n";
  const occ::handle<IGESData_IGESModel> aModel    = readModel(aPreamble + anIGESContent);
  ASSERT_FALSE(aModel.IsNull());
  EXPECT_EQ(aModel->NbEntities(), 49);
}

TEST_F(DEIGES_ProviderTest, ReadStream_LongFNESPreamble_IsSkippedAsPhysicalLine)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  TCollection_AsciiString            anIGESContent;
  ASSERT_TRUE(writeShape(aProvider, createBoxShape(), anIGESContent));

  TCollection_AsciiString aPreamble(96, 'X');
  aPreamble += '\n';
  const occ::handle<IGESData_IGESModel> aModel = readModel(aPreamble + anIGESContent, true);
  ASSERT_FALSE(aModel.IsNull());
  EXPECT_EQ(aModel->NbEntities(), 49);
}

TEST_F(DEIGES_ProviderTest, ReadStream_FailedStream_ReturnsFalse)
{
  const occ::handle<DEIGES_Provider> aProvider = createProvider();
  std::istringstream                 anInputStream;
  anInputStream.setstate(std::ios::badbit);

  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("failed.igs", anInputStream));

  TopoDS_Shape aReadShape;
  EXPECT_FALSE(aProvider->Read(aStreams, aReadShape));
  EXPECT_TRUE(aReadShape.IsNull());
}

//==================================================================================================

TEST_F(DEIGES_ProviderTest, WrapperStreamDocumentWriteRead)
{
  occ::handle<DE_Wrapper> aWrapper = new DE_Wrapper();
  ASSERT_TRUE(aWrapper->Bind(new DEIGES_ConfigurationNode()));
  const occ::handle<TDocStd_Document> aDocument = createDocument();
  XCAFDoc_DocumentTool::ShapeTool(aDocument->Main())->AddShape(createBoxShape());
  std::stringstream            aStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("model.igs", aStream));
  ASSERT_TRUE(aWrapper->Write(aWriteStreams, aDocument));
  aStream.seekg(0);
  const occ::handle<TDocStd_Document> aReadDocument = createDocument();
  DE_Provider::ReadStreamList         aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("model.igs", aStream));
  ASSERT_TRUE(aWrapper->Read(aReadStreams, aReadDocument));
  NCollection_Sequence<TDF_Label> aRoots;
  XCAFDoc_DocumentTool::ShapeTool(aReadDocument->Main())->GetFreeShapes(aRoots);
  ASSERT_EQ(aRoots.Size(), 1);
  EXPECT_FALSE(XCAFDoc_ShapeTool::GetShape(aRoots.First()).IsNull());
}
