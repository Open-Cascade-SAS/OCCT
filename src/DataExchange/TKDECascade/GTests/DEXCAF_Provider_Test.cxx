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

occ::handle<TDocStd_Document> makeDocument()
{
  const occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);
  occ::handle<TDocStd_Document> aDocument;
  anApp->NewDocument("BinXCAF", aDocument);
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
