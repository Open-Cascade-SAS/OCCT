// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. See LICENSE_LGPL_21.txt for details.

#include <DEXCAF_ConfigurationNode.hxx>
#include <DEXCAF_Provider.hxx>
#include <DE_Wrapper.hxx>
#include <BinXCAFDrivers.hxx>
#include <XmlXCAFDrivers.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_ArrayStreamBuffer.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <TDF_Data.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Vec.hxx>
#include <Quantity_Color.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Trsf.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <istream>
#include <ostream>
#include <streambuf>
#include <sstream>
#include <Message_ProgressIndicator.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>

namespace
{
// Fixed-capacity seekable output for this small document fixture.
class DEXCAF_TestOutputBuffer final : public std::streambuf
{
public:
  DEXCAF_TestOutputBuffer();
  // Return the initialized byte range, including bytes before a backward seek.
  size_t Size();
  // Borrow fixture storage for a subsequent input stream.
  const char* Data() const;

protected:
  pos_type seekoff(off_type                theOffset,
                   std::ios_base::seekdir  theOrigin,
                   std::ios_base::openmode theMode) override;
  pos_type seekpos(pos_type thePosition, std::ios_base::openmode theMode) override;

private:
  NCollection_Array1<char> myBytes;    //< Output storage.
  size_t                   mySize = 0; //< Highest initialized output position.
};

// ==================================================================================================

DEXCAF_TestOutputBuffer::DEXCAF_TestOutputBuffer()
    : myBytes(size_t(1024 * 1024))
{
  myBytes.Init('\0');
  setp(myBytes.Data(), myBytes.Data() + myBytes.Size());
}

// ==================================================================================================

size_t DEXCAF_TestOutputBuffer::Size()
{
  mySize = std::max(mySize, static_cast<size_t>(pptr() - pbase()));
  return mySize;
}

// ==================================================================================================

const char* DEXCAF_TestOutputBuffer::Data() const
{
  return myBytes.Data();
}

// ==================================================================================================

std::streambuf::pos_type DEXCAF_TestOutputBuffer::seekoff(off_type                theOffset,
                                                          std::ios_base::seekdir  theOrigin,
                                                          std::ios_base::openmode theMode)
{
  const size_t aSize = Size();
  if ((theMode & std::ios_base::out) == 0 || (theMode & std::ios_base::in) != 0)
  {
    return pos_type(off_type(-1));
  }
  off_type aBase = 0;
  if (theOrigin == std::ios_base::cur)
  {
    aBase = pptr() - pbase();
  }
  else if (theOrigin == std::ios_base::end)
  {
    aBase = static_cast<off_type>(aSize);
  }
  else if (theOrigin != std::ios_base::beg)
  {
    return pos_type(off_type(-1));
  }
  if (theOffset < -aBase || theOffset > static_cast<off_type>(myBytes.Size()) - aBase)
  {
    return pos_type(off_type(-1));
  }
  const off_type aPosition = aBase + theOffset;
  setp(myBytes.Data(), myBytes.Data() + myBytes.Size());
  pbump(static_cast<int>(aPosition));
  return pos_type(aPosition);
}

// ==================================================================================================

std::streambuf::pos_type DEXCAF_TestOutputBuffer::seekpos(pos_type                thePosition,
                                                          std::ios_base::openmode theMode)
{
  return seekoff(static_cast<off_type>(thePosition), std::ios_base::beg, theMode);
}

// ==================================================================================================

occ::handle<TDocStd_Document> makeDocument(const char* theFormat = "BinXCAF")
{
  const occ::handle<TDocStd_Document>  aDocument = new TDocStd_Document(theFormat);
  const occ::handle<XCAFDoc_ShapeTool> aShapes = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TopoDS_Shape                   aBox    = BRepPrimAPI_MakeBox(10, 20, 30).Shape();
  const TDF_Label                      aPart   = aShapes->AddShape(aBox, false);
  TDataStd_Name::Set(aPart, TCollection_ExtendedString(u"Part \u00e9"));
  XCAFDoc_DocumentTool::ColorTool(aDocument->Main())
    ->SetColor(aPart, Quantity_Color(Quantity_NOC_RED), XCAFDoc_ColorGen);
  const TDF_Label anAssembly = aShapes->NewShape();
  aShapes->AddComponent(anAssembly, aPart, TopLoc_Location());
  gp_Trsf aTransform;
  aTransform.SetTranslation(gp_Vec(100, 0, 0));
  aShapes->AddComponent(anAssembly, aPart, TopLoc_Location(aTransform));
  aShapes->UpdateAssemblies();
  XCAFDoc_DocumentTool::SetLengthUnit(aDocument, 0.001);
  return aDocument;
}

// ==================================================================================================

void checkDocument(const occ::handle<TDocStd_Document>& theDocument)
{
  EXPECT_EQ(TDocStd_Document::Get(theDocument->Main()), theDocument);
  const occ::handle<XCAFDoc_ShapeTool> aShapes =
    XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  NCollection_Sequence<TDF_Label> aRoots; // Required XCAF output interface.
  aShapes->GetFreeShapes(aRoots);
  ASSERT_EQ(aRoots.Size(), 1);
  ASSERT_TRUE(aShapes->IsAssembly(aRoots.First()));
  NCollection_Sequence<TDF_Label> aComponents; // Required XCAF output interface.
  ASSERT_TRUE(aShapes->GetComponents(aRoots.First(), aComponents));
  ASSERT_EQ(aComponents.Size(), 2);
  TDF_Label aFirst, aSecond;
  ASSERT_TRUE(aShapes->GetReferredShape(aComponents.First(), aFirst));
  ASSERT_TRUE(aShapes->GetReferredShape(aComponents.Last(), aSecond));
  EXPECT_EQ(aFirst, aSecond);
  EXPECT_NEAR(aShapes->GetLocation(aComponents.Last()).Transformation().TranslationPart().X(),
              100,
              1.e-12);
  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(aFirst.FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), TCollection_ExtendedString(u"Part \u00e9"));
  Quantity_Color aColor;
  ASSERT_TRUE(XCAFDoc_DocumentTool::ColorTool(theDocument->Main())
                ->GetColor(aFirst, XCAFDoc_ColorGen, aColor));
  EXPECT_NEAR(aColor.Red(), 1, 1.e-12);
  double aUnit = 0;
  ASSERT_TRUE(XCAFDoc_DocumentTool::GetLengthUnit(theDocument, aUnit));
  EXPECT_NEAR(aUnit, 0.001, 1.e-12);
}

// ==================================================================================================

class DEXCAF_FailingOutputBuffer : public std::stringbuf
{
protected:
  std::streamsize xsputn(const char*, std::streamsize) override { return 0; }

  int_type overflow(int_type) override { return traits_type::eof(); }
};

class DEXCAF_CancelProgress : public Message_ProgressIndicator
{
public:
  bool UserBreak() override { return true; }

  void Show(const Message_ProgressScope&, bool) override {}
};

class DEXCAF_CancelAfterAppend : public Message_ProgressIndicator
{
public:
  explicit DEXCAF_CancelAfterAppend(const occ::handle<TDocStd_Document>& theDocument)
      : myDocument(theDocument)
  {
  }

  bool UserBreak() override
  {
    occ::handle<TDataStd_Name> aName;
    return myDocument->Main().FindAttribute(TDataStd_Name::GetID(), aName)
           && aName->Get() == TCollection_ExtendedString("source");
  }

  void Show(const Message_ProgressScope&, bool) override {}

private:
  occ::handle<TDocStd_Document> myDocument;
};

std::string documentBytes()
{
  const occ::handle<TDocStd_Document> aSource = new TDocStd_Document("BinXCAF");
  TDataStd_Name::Set(aSource->Main(), TCollection_ExtendedString("source"));
  TDataStd_Name::Set(aSource->Main().FindChild(10), TCollection_ExtendedString("new attribute"));
  const occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);
  std::stringstream aStream;
  EXPECT_EQ(anApp->SaveAs(aSource, aStream), PCDM_SS_OK);
  return aStream.str();
}
} // namespace

TEST(DEXCAF_Provider_Test, ReadSeekableStreamPreservesAssemblyAndMetadata)
{
  const occ::handle<TDocStd_Document>    aSource = makeDocument();
  const occ::handle<TDocStd_Application> anApp   = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);
  DEXCAF_TestOutputBuffer aBytes;
  std::ostream            anOutput(&aBytes);
  ASSERT_EQ(anApp->SaveAs(aSource, anOutput), PCDM_SS_OK);
  Standard_ArrayStreamBuffer  aBuffer(aBytes.Data(), aBytes.Size());
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("virtual/project/model.xbf", anInput));
  const occ::handle<TDocStd_Document> aTarget = new TDocStd_Document("BinXCAF");
  const occ::handle<DE_Provider> aProvider    = new DEXCAF_Provider(new DEXCAF_ConfigurationNode());
  const occ::handle<TDF_Data>    aPreviousData = aTarget->GetData();
  ASSERT_TRUE(aProvider->Read(aStreams, aTarget));
  EXPECT_TRUE(TDocStd_Document::Get(aPreviousData->Root()).IsNull());
  checkDocument(aTarget);
}

TEST(DEXCAF_Provider_Test, WrapperStreamRoundTrip)
{
  const occ::handle<TDocStd_Document> aSource = makeDocument();
  DE_Wrapper                          aWrapper;
  ASSERT_TRUE(aWrapper.Bind(new DEXCAF_ConfigurationNode()));
  DEXCAF_TestOutputBuffer      aBytes;
  std::ostream                 anOutput(&aBytes);
  DE_Provider::WriteStreamList aWrites;
  aWrites.Append(DE_Provider::WriteStreamNode("virtual/project/model.xbf", anOutput));
  ASSERT_TRUE(aWrapper.Write(aWrites, aSource));
  ASSERT_GT(aBytes.Size(), 0);
  Standard_ArrayStreamBuffer  aBuffer(aBytes.Data(), aBytes.Size());
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aReads;
  aReads.Append(DE_Provider::ReadStreamNode("virtual/project/model.xbf", anInput));
  const occ::handle<TDocStd_Document> aTarget = new TDocStd_Document("BinXCAF");
  ASSERT_TRUE(aWrapper.Read(aReads, aTarget));
  checkDocument(aTarget);
}

TEST(DEXCAF_Provider_Test, StreamShapeOverloads)
{
  const TopoDS_Shape             aSource   = BRepPrimAPI_MakeBox(10, 20, 30).Shape();
  const occ::handle<DE_Provider> aProvider = new DEXCAF_Provider(new DEXCAF_ConfigurationNode());
  DEXCAF_TestOutputBuffer        aBytes;
  std::ostream                   anOutput(&aBytes);
  DE_Provider::WriteStreamList   aWrites;
  aWrites.Append(DE_Provider::WriteStreamNode("virtual/shape.xbf", anOutput));
  occ::handle<XSControl_WorkSession> aSession;
  ASSERT_TRUE(aProvider->Write(aWrites, aSource, aSession));
  Standard_ArrayStreamBuffer  aBuffer(aBytes.Data(), aBytes.Size());
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aReads;
  aReads.Append(DE_Provider::ReadStreamNode("virtual/shape.xbf", anInput));
  TopoDS_Shape aTarget;
  ASSERT_TRUE(aProvider->Read(aReads, aTarget, aSession));
  GProp_GProps aProperties;
  BRepGProp::VolumeProperties(aTarget, aProperties);
  EXPECT_NEAR(aProperties.Mass(), 6000, 1.e-7);
  int aFaces = 0;
  for (TopExp_Explorer anExplorer(aTarget, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ++aFaces;
  }
  EXPECT_EQ(aFaces, 6);
  EXPECT_TRUE(aSession.IsNull());
}

TEST(DEXCAF_Provider_Test, InvalidReadDoesNotReplaceDocumentData)
{
  constexpr char              THE_INVALID[] = "Invalid XCAF bytes";
  Standard_ArrayStreamBuffer  aBuffer(THE_INVALID, sizeof(THE_INVALID) - 1);
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aReads;
  aReads.Append(DE_Provider::ReadStreamNode("virtual/broken.xbf", anInput));
  const occ::handle<TDocStd_Document> aTarget = makeDocument();
  const occ::handle<TDF_Data>         aData   = aTarget->GetData();
  const occ::handle<DE_Provider> aProvider    = new DEXCAF_Provider(new DEXCAF_ConfigurationNode());
  EXPECT_FALSE(aProvider->Read(aReads, aTarget));
  EXPECT_EQ(aTarget->GetData(), aData);
  checkDocument(aTarget);
}

TEST(DEXCAF_Provider_Test, InvalidStreamAndConfiguration)
{
  DEXCAF_TestOutputBuffer             aBytes;
  std::ostream                        anOutput(&aBytes);
  DE_Provider::WriteStreamList        aWrites;
  const occ::handle<TDocStd_Document> aSource = makeDocument();
  const occ::handle<DE_Provider> aProvider    = new DEXCAF_Provider(new DEXCAF_ConfigurationNode());
  EXPECT_FALSE(aProvider->Write(aWrites, aSource));
  aWrites.Append(DE_Provider::WriteStreamNode("virtual/model.xbf", anOutput));
  anOutput.setstate(std::ios_base::badbit);
  EXPECT_FALSE(aProvider->Write(aWrites, aSource));
  anOutput.clear();
  const occ::handle<DE_Provider> anUnconfigured = new DEXCAF_Provider();
  EXPECT_FALSE(anUnconfigured->Write(aWrites, aSource));
  EXPECT_FALSE(aProvider->Write(aWrites, occ::handle<TDocStd_Document>()));
  EXPECT_FALSE(aProvider->Write(aWrites, TopoDS_Shape()));
}

TEST(DEXCAF_Provider_Test, StreamShapeReadCombinesFreeRoots)
{
  const occ::handle<TDocStd_Document> aSource = makeDocument();
  XCAFDoc_DocumentTool::ShapeTool(aSource->Main())
    ->AddShape(BRepPrimAPI_MakeBox(1, 1, 1).Shape(), false);
  const occ::handle<DE_Provider> aProvider = new DEXCAF_Provider(new DEXCAF_ConfigurationNode());
  DEXCAF_TestOutputBuffer        aBytes;
  std::ostream                   anOutput(&aBytes);
  DE_Provider::WriteStreamList   aWrites;
  aWrites.Append(DE_Provider::WriteStreamNode("virtual/roots.xbf", anOutput));
  ASSERT_TRUE(aProvider->Write(aWrites, aSource));
  Standard_ArrayStreamBuffer  aBuffer(aBytes.Data(), aBytes.Size());
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aReads;
  aReads.Append(DE_Provider::ReadStreamNode("virtual/roots.xbf", anInput));
  TopoDS_Shape aResult;
  ASSERT_TRUE(aProvider->Read(aReads, aResult));
  GProp_GProps aProperties;
  BRepGProp::VolumeProperties(aResult, aProperties);
  EXPECT_NEAR(aProperties.Mass(), 12001, 1.e-7);
  int aSolids = 0;
  for (TopExp_Explorer anExplorer(aResult, TopAbs_SOLID); anExplorer.More(); anExplorer.Next())
  {
    ++aSolids;
  }
  EXPECT_EQ(aSolids, 3);
}

TEST(DEXCAF_Provider_Test, StreamShapeReadRejectsEmptyDocument)
{
  const occ::handle<TDocStd_Document> aSource = new TDocStd_Document("BinXCAF");
  TDataStd_Name::Set(aSource->Main(), TCollection_ExtendedString("Document without shapes"));
  const occ::handle<DE_Provider> aProvider = new DEXCAF_Provider(new DEXCAF_ConfigurationNode());
  DEXCAF_TestOutputBuffer        aBytes;
  std::ostream                   anOutput(&aBytes);
  DE_Provider::WriteStreamList   aWrites;
  aWrites.Append(DE_Provider::WriteStreamNode("virtual/empty.xbf", anOutput));
  ASSERT_TRUE(aProvider->Write(aWrites, aSource));
  Standard_ArrayStreamBuffer  aBuffer(aBytes.Data(), aBytes.Size());
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aReads;
  aReads.Append(DE_Provider::ReadStreamNode("virtual/empty.xbf", anInput));
  TopoDS_Shape aResult = BRepPrimAPI_MakeBox(1, 1, 1).Shape();
  EXPECT_FALSE(aProvider->Read(aReads, aResult));
  EXPECT_TRUE(aResult.IsNull());
}

TEST(DEXCAF_Provider_Test, StreamWritePreservesOwningApplication)
{
  const occ::handle<TDocStd_Document> aSource = makeDocument();
  const occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  anApplication->CDF_Application::Open(aSource);

  struct DocumentCloser
  {
    occ::handle<TDocStd_Application> Application;
    occ::handle<TDocStd_Document>    Document;

    ~DocumentCloser() { Application->Close(Document); }
  } aCloser{anApplication, aSource};
  const occ::handle<CDM_Application>  anOwner = aSource->Application();
  const occ::handle<DE_Provider> aProvider    = new DEXCAF_Provider(new DEXCAF_ConfigurationNode());
  DEXCAF_TestOutputBuffer        aBytes;
  std::ostream                   anOutput(&aBytes);
  DE_Provider::WriteStreamList   aWrites;
  aWrites.Append(DE_Provider::WriteStreamNode("virtual/model.xbf", anOutput));
  ASSERT_TRUE(aProvider->Write(aWrites, aSource));
  EXPECT_EQ(aSource->Application(), anOwner);
}

TEST(DEXCAF_Provider_Test, StreamSaveDoesNotOpenDetachedDocument)
{
  const occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);
  const occ::handle<TDocStd_Document> aSource = new TDocStd_Document("BinXCAF");
  XCAFDoc_DocumentTool::ShapeTool(aSource->Main())->AddShape(BRepPrimAPI_MakeBox(1, 2, 3).Shape());
  ASSERT_FALSE(aSource->IsOpened());
  DEXCAF_TestOutputBuffer aBytes;
  std::ostream            anOutput(&aBytes);
  ASSERT_EQ(anApp->SaveAs(aSource, anOutput), PCDM_SS_OK);
  EXPECT_FALSE(aSource->IsOpened());
}

TEST(DEXCAF_Provider_Test, FailedStreamSaveDoesNotOpenDetachedDocument)
{
  const occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);
  const occ::handle<TDocStd_Document> aSource = new TDocStd_Document("BinXCAF");
  XCAFDoc_DocumentTool::ShapeTool(aSource->Main())->AddShape(BRepPrimAPI_MakeBox(1, 2, 3).Shape());
  std::ostream anOutput(nullptr);
  EXPECT_NE(anApp->SaveAs(aSource, anOutput), PCDM_SS_OK);
  EXPECT_FALSE(aSource->IsOpened());
}

TEST(DEXCAF_Provider_Test, StreamRoundTripPreservesAuthoredToleranceMetadata)
{
  const occ::handle<TDocStd_Document>                      aSource = makeDocument();
  const occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObject =
    new XCAFDimTolObjects_GeomToleranceObject();
  anObject->SetDescription(new TCollection_HAsciiString("unequally disposed tolerance"));
  anObject->SetUnequalDisplacement(-0.125);
  XCAFDoc_GeomTolerance::Set(aSource->Main().FindChild(100))->SetObject(anObject);
  const occ::handle<DE_Provider> aProvider = new DEXCAF_Provider(new DEXCAF_ConfigurationNode());
  DEXCAF_TestOutputBuffer        aBytes;
  std::ostream                   anOutput(&aBytes);
  DE_Provider::WriteStreamList   aWrites;
  aWrites.Append(DE_Provider::WriteStreamNode("virtual/metadata.xbf", anOutput));
  ASSERT_TRUE(aProvider->Write(aWrites, aSource));
  Standard_ArrayStreamBuffer  aBuffer(aBytes.Data(), aBytes.Size());
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aReads;
  aReads.Append(DE_Provider::ReadStreamNode("virtual/metadata.xbf", anInput));
  const occ::handle<TDocStd_Document> aTarget = new TDocStd_Document("BinXCAF");
  ASSERT_TRUE(aProvider->Read(aReads, aTarget));
  occ::handle<XCAFDoc_GeomTolerance> anAttribute;
  ASSERT_TRUE(aTarget->Main()
                .FindChild(100, false)
                .FindAttribute(XCAFDoc_GeomTolerance::GetID(), anAttribute));
  const occ::handle<XCAFDimTolObjects_GeomToleranceObject> aResult = anAttribute->GetObject();
  ASSERT_TRUE(aResult->GetUnequalDisplacement().has_value());
  EXPECT_DOUBLE_EQ(*aResult->GetUnequalDisplacement(), -0.125);
  ASSERT_FALSE(aResult->GetDescription().IsNull());
  EXPECT_STREQ(aResult->GetDescription()->ToCString(), "unequally disposed tolerance");
}

TEST(DEXCAF_Provider_Test, ReplacementResetsHistoryAndPreservesModificationPolicy)
{
  const occ::handle<TDocStd_Document> aTarget = makeDocument();
  aTarget->SetUndoLimit(10);
  aTarget->SetModificationMode(true);
  for (const char* aName : {"first edit", "second edit"})
  {
    aTarget->NewCommand();
    TDataStd_Name::Set(aTarget->Main(), TCollection_ExtendedString(aName));
    ASSERT_TRUE(aTarget->CommitCommand());
  }
  ASSERT_TRUE(aTarget->Undo());
  ASSERT_EQ(aTarget->GetAvailableUndos(), 1);
  ASSERT_EQ(aTarget->GetAvailableRedos(), 1);
  aTarget->SetNestedTransactionMode();
  aTarget->OpenCommand();
  aTarget->OpenCommand();
  TDataStd_Name::Set(aTarget->Main(), TCollection_ExtendedString("uncommitted edit"));
  const occ::handle<TDF_Data> aPreviousData = aTarget->GetData();
  std::stringstream           aStream(documentBytes());
  DE_Provider::ReadStreamList aReads;
  aReads.Append(DE_Provider::ReadStreamNode("virtual/model.xbf", aStream));
  DEXCAF_Provider aProvider(new DEXCAF_ConfigurationNode());
  ASSERT_TRUE(aProvider.Read(aReads, aTarget));
  EXPECT_EQ(aTarget->GetAvailableUndos(), 0);
  EXPECT_EQ(aTarget->GetAvailableRedos(), 0);
  EXPECT_FALSE(aTarget->HasOpenCommand());
  EXPECT_FALSE(aTarget->Undo());
  EXPECT_FALSE(aTarget->Redo());
  EXPECT_TRUE(aTarget->ModificationMode());
  EXPECT_FALSE(aTarget->GetData()->IsModificationAllowed());
  EXPECT_TRUE(aTarget->IsChanged());
  EXPECT_TRUE(TDocStd_Document::Get(aPreviousData->Root()).IsNull());
  EXPECT_EQ(TDocStd_Document::Get(aTarget->Main()), aTarget);
  aTarget->NewCommand();
  TDataStd_Name::Set(aTarget->Main(), TCollection_ExtendedString("new edit"));
  EXPECT_TRUE(aTarget->CommitCommand());
  EXPECT_TRUE(aTarget->Undo());
}

TEST(DEXCAF_Provider_Test, AppendModesPreserveDataAndSupportUndo)
{
  for (const auto aMode :
       {PCDM_ReaderFilter::AppendMode_Protect, PCDM_ReaderFilter::AppendMode_Overwrite})
  {
    for (const int anUndoLimit : {0, 10})
    {
      const occ::handle<TDocStd_Document> aTarget = new TDocStd_Document("BinXCAF");
      TDataStd_Name::Set(aTarget->Main(), TCollection_ExtendedString("target"));
      aTarget->SetUndoLimit(anUndoLimit);
      if (anUndoLimit > 0)
      {
        aTarget->SetModificationMode(true);
        aTarget->OpenCommand();
      }
      const occ::handle<TDF_Data>                 aData = aTarget->GetData();
      const occ::handle<DEXCAF_ConfigurationNode> aNode = new DEXCAF_ConfigurationNode();
      aNode->InternalParameters.ReadAppendMode          = aMode;
      DEXCAF_Provider             aProvider(aNode);
      std::stringstream           aStream(documentBytes());
      DE_Provider::ReadStreamList aReads;
      aReads.Append(DE_Provider::ReadStreamNode("virtual/model.xbf", aStream));
      ASSERT_TRUE(aProvider.Read(aReads, aTarget));
      EXPECT_EQ(aTarget->GetData(), aData);
      EXPECT_FALSE(aTarget->IsOpened());
      EXPECT_EQ(aTarget->HasOpenCommand(), anUndoLimit > 0);
      EXPECT_TRUE(aData->IsModificationAllowed());
      occ::handle<TDataStd_Name> aName;
      ASSERT_TRUE(aTarget->Main().FindAttribute(TDataStd_Name::GetID(), aName));
      EXPECT_EQ(aName->Get(),
                TCollection_ExtendedString(
                  aMode == PCDM_ReaderFilter::AppendMode_Protect ? "target" : "source"));
      ASSERT_TRUE(aTarget->Main().FindChild(10).FindAttribute(TDataStd_Name::GetID(), aName));
      EXPECT_EQ(aName->Get(), TCollection_ExtendedString("new attribute"));
      if (anUndoLimit > 0)
      {
        ASSERT_TRUE(aTarget->CommitCommand());
        EXPECT_EQ(aTarget->GetAvailableUndos(), 1);
        ASSERT_TRUE(aTarget->Undo());
        EXPECT_FALSE(aTarget->Main().FindChild(10).FindAttribute(TDataStd_Name::GetID(), aName));
        ASSERT_TRUE(aTarget->Main().FindAttribute(TDataStd_Name::GetID(), aName));
        EXPECT_EQ(aName->Get(), TCollection_ExtendedString("target"));
      }
    }
  }
}

TEST(DEXCAF_Provider_Test, CallerCanAbortCancelledAppend)
{
  const occ::handle<TDocStd_Document> aTarget = new TDocStd_Document("BinXCAF");
  aTarget->SetUndoLimit(10);
  TDataStd_Name::Set(aTarget->Main(), TCollection_ExtendedString("target"));
  aTarget->SetModificationMode(true);
  aTarget->NewCommand();
  const occ::handle<TDF_Data>                 aData = aTarget->GetData();
  const occ::handle<DEXCAF_ConfigurationNode> aNode = new DEXCAF_ConfigurationNode();
  aNode->InternalParameters.ReadAppendMode          = PCDM_ReaderFilter::AppendMode_Overwrite;
  DEXCAF_Provider             aProvider(aNode);
  std::stringstream           aStream(documentBytes());
  DE_Provider::ReadStreamList aReads;
  aReads.Append(DE_Provider::ReadStreamNode("virtual/model.xbf", aStream));
  const occ::handle<Message_ProgressIndicator> aProgress = new DEXCAF_CancelAfterAppend(aTarget);
  EXPECT_FALSE(aProvider.Read(aReads, aTarget, aProgress->Start()));
  EXPECT_EQ(aTarget->GetData(), aData);
  EXPECT_TRUE(aTarget->HasOpenCommand());
  aTarget->AbortCommand();
  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(aTarget->Main().FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), TCollection_ExtendedString("target"));
  EXPECT_FALSE(aTarget->Main().FindChild(10).FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_FALSE(aTarget->HasOpenCommand());
  EXPECT_EQ(aTarget->GetAvailableUndos(), 0);
}

TEST(DEXCAF_Provider_Test, StreamSaveOverloadsRestoreStateAndRecoverAfterCancellation)
{
  for (const char* aFormat : {"BinXCAF", "XmlXCAF"})
  {
    SCOPED_TRACE(aFormat);
    const occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
    BinXCAFDrivers::DefineFormat(anApp);
    XmlXCAFDrivers::DefineFormat(anApp);
    const occ::handle<TDocStd_Document>          aSource   = makeDocument(aFormat);
    const occ::handle<Message_ProgressIndicator> aProgress = new DEXCAF_CancelProgress();
    std::stringstream                            aCancelled;
    EXPECT_EQ(anApp->SaveAs(aSource, aCancelled, aProgress->Start()), PCDM_SS_UserBreak);
    EXPECT_FALSE(aSource->IsOpened());
    std::stringstream          aValid;
    TCollection_ExtendedString aStatusMessage("old message");
    ASSERT_EQ(anApp->SaveAs(aSource, aValid, aStatusMessage), PCDM_SS_OK);
    EXPECT_TRUE(aStatusMessage.IsEmpty());
    EXPECT_FALSE(aSource->IsOpened());
    EXPECT_FALSE(aValid.str().empty());
    const occ::handle<TDocStd_Document> aRestored = new TDocStd_Document("BinXCAF");
    DE_Provider::ReadStreamList         aReads;
    aReads.Append(DE_Provider::ReadStreamNode("recovered.xcaf", aValid));
    DEXCAF_Provider aProvider(new DEXCAF_ConfigurationNode());
    ASSERT_TRUE(aProvider.Read(aReads, aRestored));
    checkDocument(aRestored);
  }
}

TEST(DEXCAF_Provider_Test, ThrowingOutputReportsFailureAndRestoresState)
{
  const occ::handle<TDocStd_Document> aSource = makeDocument();
  DEXCAF_FailingOutputBuffer          aBuffer;
  std::ostream                        aStream(&aBuffer);
  aStream.exceptions(std::ios::badbit | std::ios::failbit);
  DE_Provider::WriteStreamList aWrites;
  aWrites.Append(DE_Provider::WriteStreamNode("virtual/model.xbf", aStream));
  DEXCAF_Provider aProvider(new DEXCAF_ConfigurationNode());
  EXPECT_FALSE(aProvider.Write(aWrites, aSource));
  EXPECT_FALSE(aSource->IsOpened());
  aStream.clear();
  const occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);
  TCollection_ExtendedString aMessage;
  EXPECT_EQ(anApp->SaveAs(aSource, aStream, aMessage), PCDM_SS_WriteFailure);
  EXPECT_FALSE(aMessage.IsEmpty());
  EXPECT_FALSE(aSource->IsOpened());
  EXPECT_EQ(anApp->SaveAs(nullptr, aStream, aMessage), PCDM_SS_Doc_IsNull);
}

TEST(DEXCAF_Provider_Test, FileAppendPreservesExistingData)
{
  struct TemporaryFile
  {
    OSD_File File;

    ~TemporaryFile() { File.Remove(); }
  } aTemporary;

  aTemporary.File.BuildTemporary();
  ASSERT_FALSE(aTemporary.File.Failed());
  std::string aBytes = documentBytes();
  aTemporary.File.Write(aBytes.data(), static_cast<int>(aBytes.size()));
  aTemporary.File.Close();
  ASSERT_FALSE(aTemporary.File.Failed());
  OSD_Path aPath;
  aTemporary.File.Path(aPath);
  TCollection_AsciiString aFileName;
  aPath.SystemName(aFileName);
  for (const auto aMode :
       {PCDM_ReaderFilter::AppendMode_Protect, PCDM_ReaderFilter::AppendMode_Overwrite})
  {
    const occ::handle<TDocStd_Document> aTarget = new TDocStd_Document("BinXCAF");
    TDataStd_Name::Set(aTarget->Main(), "existing");
    const auto                                  aData = aTarget->GetData();
    const occ::handle<DEXCAF_ConfigurationNode> aNode = new DEXCAF_ConfigurationNode();
    aNode->InternalParameters.ReadAppendMode          = aMode;
    DEXCAF_Provider aProvider(aNode);
    ASSERT_TRUE(aProvider.Read(aFileName, aTarget));
    EXPECT_EQ(aTarget->GetData(), aData);
    occ::handle<TDataStd_Name> aName;
    ASSERT_TRUE(aTarget->Main().FindAttribute(TDataStd_Name::GetID(), aName));
    EXPECT_EQ(aName->Get(),
              TCollection_ExtendedString(
                aMode == PCDM_ReaderFilter::AppendMode_Protect ? "existing" : "source"));
    EXPECT_TRUE(aTarget->Main().FindChild(10).FindAttribute(TDataStd_Name::GetID(), aName));
    EXPECT_FALSE(aTarget->HasOpenCommand());
  }
}
