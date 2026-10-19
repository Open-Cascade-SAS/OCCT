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

#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <DESTEP_ConfigurationNode.hxx>
#include <DESTEP_Provider.hxx>
#include <DE_ValidationUtils.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Buffer.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_ArrayStreamBuffer.hxx>
#include <STEPCAFControl_ExternFile.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_Writer.hxx>
#include <STEPConstruct_ExternRefs.hxx>
#include <StepAP214_AppliedExternalIdentificationAssignment.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_SourceItem.hxx>
#include <StepData_SelectNamed.hxx>
#include <StepData_StepModel.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XSControl_WorkSession.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <cstring>
#include <istream>
#include <sstream>
#include <string_view>

namespace
{
constexpr std::string_view THE_MODEL_FOLDER = "models/";

// Input bytes must outlive this stream.
class STEPCAFControl_TestInput final : public std::istream
{
public:
  explicit STEPCAFControl_TestInput(const NCollection_Buffer& theBytes);

private:
  Standard_ArrayStreamBuffer myBuffer; //!< View of the borrowed input bytes.
};

//==================================================================================================

STEPCAFControl_TestInput::STEPCAFControl_TestInput(const NCollection_Buffer& theBytes)
    : std::istream(nullptr),
      myBuffer(reinterpret_cast<const char*>(theBytes.Data()), theBytes.Size())
{
  rdbuf(&myBuffer);
}

using StepResources = NCollection_DataMap<TCollection_AsciiString, occ::handle<NCollection_Buffer>>;

//==================================================================================================

bool writeModel(STEPControl_Writer& theWriter, occ::handle<NCollection_Buffer>& theBytes)
{
  std::stringstream aStream(std::ios::in | std::ios::out | std::ios::binary);
  return theWriter.WriteStream(aStream) == IFSelect_RetDone
         && DE_ValidationUtils::ReadStreamBuffer(aStream, theBytes) && !theBytes->IsEmpty();
}

//==================================================================================================

bool assemblyStreams(occ::handle<NCollection_Buffer>& theMain,
                     StepResources&                   theResources,
                     double                           theWidth         = 2,
                     bool                             theDistinctParts = false)
{
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  XCAFDoc_DocumentTool::SetLengthUnit(aDocument, 0.001);
  const occ::handle<XCAFDoc_ShapeTool> aShapes = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label aPart = aShapes->AddShape(BRepPrimAPI_MakeBox(theWidth, 3, 4).Shape(), false);
  TDataStd_Name::Set(aPart, "Part");
  TopoDS_Compound aCompound;
  BRep_Builder().MakeCompound(aCompound);
  const TDF_Label anAssembly = aShapes->AddShape(aCompound, true);
  TDataStd_Name::Set(anAssembly, "Assembly");
  aShapes->AddComponent(anAssembly, aPart, TopLoc_Location());
  gp_Trsf aMove;
  aMove.SetTranslation(gp_Vec(20, 0, 0));
  TDF_Label aSecondPart = aPart;
  if (theDistinctParts)
  {
    aSecondPart = aShapes->AddShape(BRepPrimAPI_MakeBox(5, 3, 4).Shape(), false);
    TDataStd_Name::Set(aSecondPart, "Other");
  }
  aShapes->AddComponent(anAssembly, aSecondPart, TopLoc_Location(aMove));
  aShapes->UpdateAssemblies();
  STEPCAFControl_Writer aWriter;
  if (!aWriter.Transfer(aDocument, STEPControl_AsIs, "parts/"))
  {
    return false;
  }
  if (!writeModel(aWriter.ChangeWriter(), theMain))
  {
    return false;
  }
  for (NCollection_DataMap<TCollection_AsciiString,
                           occ::handle<STEPCAFControl_ExternFile>>::Iterator anIt(aWriter
                                                                                    .ExternFiles());
       anIt.More();
       anIt.Next())
  {
    STEPControl_Writer              aPartWriter(anIt.Value()->GetWS(), false);
    occ::handle<NCollection_Buffer> aBytes;
    if (!writeModel(aPartWriter, aBytes))
    {
      return false;
    }
    theResources.Bind(TCollection_AsciiString(THE_MODEL_FOLDER) + anIt.Key(), aBytes);
  }
  return !theResources.IsEmpty();
}

//==================================================================================================

void appendResources(const StepResources&                                theResources,
                     DE_Provider::ReadStreamList&                        theStreams,
                     NCollection_DynamicArray<STEPCAFControl_TestInput>& theInputs)
{
  for (StepResources::Iterator anIt(theResources); anIt.More(); anIt.Next())
  {
    STEPCAFControl_TestInput& anInput = theInputs.EmplaceAppend(*anIt.Value());
    theStreams.Append(DE_Provider::ReadStreamNode(anIt.Key(), anInput));
  }
}

//==================================================================================================

bool replaceName(occ::handle<NCollection_Buffer>& theBytes,
                 const TCollection_AsciiString&   theOld,
                 const TCollection_AsciiString&   theNew)
{
  TCollection_AsciiString aText(
    std::string_view(reinterpret_cast<const char*>(theBytes->Data()), theBytes->Size()));
  size_t anOffset       = 0;
  bool   hasReplacement = false;
  for (;;)
  {
    const std::string_view aView     = static_cast<std::string_view>(aText);
    const size_t           aPosition = aView.find(static_cast<std::string_view>(theOld), anOffset);
    if (aPosition == aView.npos)
    {
      break;
    }
    aText.Remove(static_cast<int>(aPosition) + 1, theOld.Length());
    aText.Insert(static_cast<int>(aPosition) + 1, theNew);
    anOffset       = aPosition + static_cast<size_t>(theNew.Length());
    hasReplacement = true;
  }
  if (!hasReplacement)
  {
    return false;
  }
  theBytes = new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator(),
                                    static_cast<size_t>(aText.Length()));
  if (theBytes->Data() == nullptr)
  {
    return false;
  }
  std::memcpy(theBytes->ChangeData(), aText.ToCString(), static_cast<size_t>(aText.Length()));
  return true;
}

//==================================================================================================

void addChild(const TCollection_AsciiString&         theFolder,
              const occ::handle<NCollection_Buffer>& theMain,
              const StepResources&                   theParts,
              StepResources&                         theAll)
{
  theAll.Bind(theFolder + "/main.step", theMain);
  for (StepResources::Iterator anIt(theParts); anIt.More(); anIt.Next())
  {
    theAll.Bind(theFolder + "/"
                  + TCollection_AsciiString(
                    static_cast<std::string_view>(anIt.Key()).substr(THE_MODEL_FOLDER.size())),
                anIt.Value());
  }
}

} // namespace

//==================================================================================================

TEST(STEPCAFControl_ReaderTest, NamedExternalPartRetainsRepeatedUses)
{
  occ::handle<NCollection_Buffer> aMain;
  StepResources                   aResources;
  ASSERT_TRUE(assemblyStreams(aMain, aResources));
  STEPCAFControl_TestInput    aMainInput(*aMain);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
  NCollection_DynamicArray<STEPCAFControl_TestInput> anInputs(2);
  appendResources(aResources, aStreams, anInputs);
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  DESTEP_Provider                     aProvider(new DESTEP_ConfigurationNode());
  ASSERT_TRUE(aProvider.Read(aStreams, aDocument));
  const occ::handle<XCAFDoc_ShapeTool> aShapes = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  NCollection_Sequence<TDF_Label>      aRoots;
  aShapes->GetFreeShapes(aRoots);
  ASSERT_EQ(aRoots.Length(), 1);
  GProp_GProps aMass;
  BRepGProp::VolumeProperties(aShapes->GetShape(aRoots.First()), aMass);
  EXPECT_NEAR(aMass.Mass(), 48, 1.0e-8);
  NCollection_Sequence<TDF_Label> aComponents;
  ASSERT_TRUE(aShapes->GetComponents(aRoots.First(), aComponents));
  ASSERT_EQ(aComponents.Length(), 2);
  TDF_Label aFirst, aSecond;
  ASSERT_TRUE(aShapes->GetReferredShape(aComponents.Value(1), aFirst));
  ASSERT_TRUE(aShapes->GetReferredShape(aComponents.Value(2), aSecond));
  EXPECT_EQ(aFirst, aSecond);
  EXPECT_DOUBLE_EQ(
    aShapes->GetLocation(aComponents.Value(2)).Transformation().TranslationPart().X(),
    20);
}

//==================================================================================================

TEST(STEPCAFControl_ReaderTest, MissingExternalPartFailsTransfer)
{
  occ::handle<NCollection_Buffer> aMain;
  StepResources                   aResources;
  ASSERT_TRUE(assemblyStreams(aMain, aResources));
  STEPCAFControl_TestInput    aMainInput(*aMain);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  DESTEP_Provider                     aProvider(new DESTEP_ConfigurationNode());
  EXPECT_FALSE(aProvider.Read(aStreams, aDocument));
}

//==================================================================================================

TEST(STEPCAFControl_ReaderTest, ShapeReadResolvesExternalPartsAndUnits)
{
  occ::handle<NCollection_Buffer> aMain;
  StepResources                   aResources;
  ASSERT_TRUE(assemblyStreams(aMain, aResources));
  STEPCAFControl_TestInput    aMainInput(*aMain);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
  NCollection_DynamicArray<STEPCAFControl_TestInput> anInputs(2);
  appendResources(aResources, aStreams, anInputs);
  const occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  aNode->GlobalParameters.LengthUnit                = 1000;
  DESTEP_Provider                    aProvider(aNode);
  occ::handle<XSControl_WorkSession> aSession;
  TopoDS_Shape                       aShape;
  ASSERT_TRUE(aProvider.Read(aStreams, aShape, aSession));
  ASSERT_FALSE(aSession.IsNull());
  GProp_GProps aMass;
  BRepGProp::VolumeProperties(aShape, aMass);
  EXPECT_NEAR(aMass.Mass(), 48.0e-9, 1.0e-16);
}

//==================================================================================================

TEST(STEPCAFControl_ReaderTest, FailedPartLeavesPreviousShapeUnchanged)
{
  occ::handle<NCollection_Buffer> aMain;
  StepResources                   aResources;
  ASSERT_TRUE(assemblyStreams(aMain, aResources));
  for (int aCase = 0; aCase < 3; ++aCase)
  {
    STEPCAFControl_TestInput    aMainInput(*aMain);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
    aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
    const char                      aBad[] = "not a STEP model";
    Standard_ArrayStreamBuffer      aBadBuffer(aBad, sizeof(aBad) - 1);
    std::istream                    aBadStream(&aBadBuffer);
    occ::handle<NCollection_Buffer> anEmpty;
    STEPControl_Writer              anEmptyWriter;
    ASSERT_TRUE(writeModel(anEmptyWriter, anEmpty));
    STEPCAFControl_TestInput anEmptyStream(*anEmpty);
    if (aCase != 0)
    {
      for (StepResources::Iterator anIt(aResources); anIt.More(); anIt.Next())
      {
        aStreams.Append(
          DE_Provider::ReadStreamNode(anIt.Key(), aCase == 1 ? aBadStream : anEmptyStream));
      }
    }
    DESTEP_Provider    aProvider(new DESTEP_ConfigurationNode());
    const TopoDS_Shape anOriginal = BRepPrimAPI_MakeBox(1, 1, 1).Shape();
    TopoDS_Shape       aShape     = anOriginal;
    EXPECT_FALSE(aProvider.Read(aStreams, aShape));
    EXPECT_TRUE(aShape.IsEqual(anOriginal));
  }
}

//==================================================================================================

TEST(STEPCAFControl_ReaderTest, NestedPartsWithSameFilenameRemainDistinct)
{
  occ::handle<NCollection_Buffer> aMain, aLeft, aRight;
  StepResources                   aRootParts, aLeftParts, aRightParts, aResources;
  ASSERT_TRUE(assemblyStreams(aMain, aRootParts, 2, true));
  ASSERT_TRUE(assemblyStreams(aLeft, aLeftParts, 2));
  ASSERT_TRUE(assemblyStreams(aRight, aRightParts, 5));
  ASSERT_TRUE(replaceName(aMain, "parts/Part.stp", "../left/main.step"));
  ASSERT_TRUE(replaceName(aMain, "parts/Other.stp", "../right/main.step"));
  addChild("left", aLeft, aLeftParts, aResources);
  addChild("right", aRight, aRightParts, aResources);
  STEPCAFControl_TestInput    aMainInput(*aMain);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
  NCollection_DynamicArray<STEPCAFControl_TestInput> anInputs(2);
  appendResources(aResources, aStreams, anInputs);
  DESTEP_Provider aProvider(new DESTEP_ConfigurationNode());
  TopoDS_Shape    aShape;
  ASSERT_TRUE(aProvider.Read(aStreams, aShape));
  GProp_GProps aMass;
  BRepGProp::VolumeProperties(aShape, aMass);
  EXPECT_NEAR(aMass.Mass(), 168, 1.0e-8);
}

//==================================================================================================

TEST(STEPCAFControl_ReaderTest, SelfAndMutualReferencesFailWithoutRecursion)
{
  for (const bool isMutual : {false, true})
  {
    occ::handle<NCollection_Buffer> aMain;
    StepResources                   aParts, aResources;
    ASSERT_TRUE(assemblyStreams(aMain, aParts));
    if (isMutual)
    {
      occ::handle<NCollection_Buffer> aChild;
      StepResources                   aChildParts;
      ASSERT_TRUE(assemblyStreams(aChild, aChildParts));
      ASSERT_TRUE(replaceName(aMain, "parts/Part.stp", "../child/main.step"));
      ASSERT_TRUE(replaceName(aChild, "parts/Part.stp", "../models/main.step"));
      aResources.Bind("child/main.step", aChild);
    }
    else
    {
      ASSERT_TRUE(replaceName(aMain, "parts/Part.stp", "./main.step"));
    }
    STEPCAFControl_TestInput    aMainInput(*aMain);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
    aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
    NCollection_DynamicArray<STEPCAFControl_TestInput> anInputs(2);
    appendResources(aResources, aStreams, anInputs);
    DESTEP_Provider                     aProvider(new DESTEP_ConfigurationNode());
    const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
    EXPECT_FALSE(aProvider.Read(aStreams, aDocument));
  }
}

//==================================================================================================

TEST(STEPCAFControl_ReaderTest, InvalidReferencesAndDuplicateNamesFail)
{
  for (const char* aReference : {"../../part.stp", "/part.stp", "https://example.invalid/part.stp"})
  {
    occ::handle<NCollection_Buffer> aMain;
    StepResources                   aResources;
    ASSERT_TRUE(assemblyStreams(aMain, aResources));
    ASSERT_TRUE(replaceName(aMain, "parts/Part.stp", aReference));
    STEPCAFControl_TestInput    aMainInput(*aMain);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
    aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
    DESTEP_Provider                     aProvider(new DESTEP_ConfigurationNode());
    const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
    EXPECT_FALSE(aProvider.Read(aStreams, aDocument));
  }
  occ::handle<NCollection_Buffer> aMain;
  StepResources                   aResources;
  ASSERT_TRUE(assemblyStreams(aMain, aResources));
  STEPCAFControl_TestInput    aMainInput(*aMain);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
  aStreams.Append(DE_Provider::ReadStreamNode("models/./main.step", aMainInput));
  DESTEP_Provider aProvider(new DESTEP_ConfigurationNode());
  TopoDS_Shape    aShape;
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
}

//==================================================================================================

TEST(STEPCAFControl_ReaderTest, ReaderCachesResolvedNamesAndInitClearsBinding)
{
  occ::handle<NCollection_Buffer> aMain;
  StepResources                   aResources;
  ASSERT_TRUE(assemblyStreams(aMain, aResources));
  STEPCAFControl_TestInput    aMainInput(*aMain);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
  NCollection_DynamicArray<STEPCAFControl_TestInput> anInputs(2);
  appendResources(aResources, aStreams, anInputs);
  STEPCAFControl_Reader aReader;
  aReader.SetResourceStreams(aStreams);
  ASSERT_EQ(aReader.ReadStream("models/main.step", aMainInput), IFSelect_RetDone);
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  ASSERT_TRUE(aReader.Transfer(aDocument));
  EXPECT_EQ(aReader.ExternFiles().Extent(), 1);
  for (StepResources::Iterator anIt(aResources); anIt.More(); anIt.Next())
  {
    occ::handle<STEPCAFControl_ExternFile> aPart;
    EXPECT_TRUE(aReader.ExternFile(anIt.Key().ToCString(), aPart));
    ASSERT_FALSE(aPart.IsNull());
    EXPECT_EQ(aPart->GetLoadStatus(), IFSelect_RetDone);
    EXPECT_TRUE(aPart->GetTransferStatus());
  }
  aReader.Init(new XSControl_WorkSession());
  EXPECT_TRUE(aReader.ExternFiles().IsEmpty());
}

//==================================================================================================

TEST(STEPCAFControl_ReaderTest, AuthoredIdentifierIsIndependentOfDiskAndNullBytesAreRejected)
{
  occ::handle<NCollection_Buffer> aMain;
  StepResources                   aResources;
  ASSERT_TRUE(assemblyStreams(aMain, aResources));
  STEPCAFControl_TestInput    aMainInput(*aMain);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
  NCollection_DynamicArray<STEPCAFControl_TestInput> anInputs(2);
  appendResources(aResources, aStreams, anInputs);
  STEPCAFControl_Reader aReader;
  aReader.SetResourceStreams(aStreams);
  ASSERT_EQ(aReader.ReadStream("models/main.step", aMainInput), IFSelect_RetDone);
  STEPConstruct_ExternRefs aRefs(aReader.ChangeReader().WS());
  ASSERT_TRUE(aRefs.LoadExternRefs());
  ASSERT_EQ(aRefs.NbExternRefs(), 1);
  const occ::handle<StepBasic_DocumentFile> aFile = aRefs.DocFile(1);
  ASSERT_FALSE(aFile.IsNull());
  aFile->SetId(new TCollection_HAsciiString("different-part.stp"));
  aFile->SetName(new TCollection_HAsciiString("another-part.stp"));
  EXPECT_EQ(aRefs.FileIdentifier(1), "parts/Part.stp");
  EXPECT_STREQ(aRefs.FileName(1), "parts/Part.stp");
  const occ::handle<TCollection_HAsciiString> aFormat = aRefs.Format(1);
  ASSERT_FALSE(aFormat.IsNull());
  EXPECT_EQ(aFormat->String(), "STEP AP214");
  const occ::handle<StepData_StepModel> aModel = aReader.ChangeReader().StepModel();
  occ::handle<StepAP214_AppliedExternalIdentificationAssignment> anAssignment;
  for (int anIndex = 1; anIndex <= aModel->NbEntities(); ++anIndex)
  {
    anAssignment =
      occ::down_cast<StepAP214_AppliedExternalIdentificationAssignment>(aModel->Value(anIndex));
    if (!anAssignment.IsNull())
    {
      break;
    }
  }
  ASSERT_FALSE(anAssignment.IsNull());
  anAssignment->SetAssignedId(new TCollection_HAsciiString("#1"));
  const occ::handle<StepData_SelectNamed> aSource = new StepData_SelectNamed();
  aSource->SetString("parts/Part.stp");
  StepBasic_SourceItem aSourceItem;
  aSourceItem.SetValue(aSource);
  anAssignment->Source()->SetSourceId(aSourceItem);
  EXPECT_EQ(aRefs.FileIdentifier(1), "parts/Part.stp");
  const char              aName[] = "parts/Part.stp\0ignored";
  TCollection_AsciiString aCounted;
  aCounted.SetValue(1, aName, sizeof(aName) - 1);
  anAssignment->SetAssignedId(new TCollection_HAsciiString(aCounted));
  const TCollection_AsciiString anExtracted = aRefs.FileIdentifier(1);
  EXPECT_EQ(static_cast<size_t>(anExtracted.Length()), sizeof(aName) - 1);
  EXPECT_NE(static_cast<std::string_view>(anExtracted).find('\0'), std::string_view::npos);
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  EXPECT_FALSE(aReader.Transfer(aDocument));
}

//==================================================================================================

TEST(STEPCAFControl_ReaderTest, SupportedExternalFormatLabelsAndUnsupportedFormat)
{
  for (const char* aFormat : {"STEP AP203", "STEP AP242", "unrelated format"})
  {
    occ::handle<NCollection_Buffer> aMain;
    StepResources                   aResources;
    ASSERT_TRUE(assemblyStreams(aMain, aResources));
    ASSERT_TRUE(replaceName(aMain, "STEP AP214", aFormat));
    STEPCAFControl_TestInput    aMainInput(*aMain);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("models/main.step", aMainInput));
    aStreams.Append(DE_Provider::ReadStreamNode("unused.step", aMainInput));
    NCollection_DynamicArray<STEPCAFControl_TestInput> anInputs(2);
    appendResources(aResources, aStreams, anInputs);
    DESTEP_Provider aProvider(new DESTEP_ConfigurationNode());
    TopoDS_Shape    aShape;
    if (std::strcmp(aFormat, "unrelated format") == 0)
    {
      EXPECT_FALSE(aProvider.Read(aStreams, aShape));
    }
    else
    {
      ASSERT_TRUE(aProvider.Read(aStreams, aShape));
      GProp_GProps aMass;
      BRepGProp::VolumeProperties(aShape, aMass);
      EXPECT_NEAR(aMass.Mass(), 48, 1.0e-8);
    }
  }
}
