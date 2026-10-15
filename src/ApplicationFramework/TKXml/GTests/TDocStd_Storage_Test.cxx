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

#include <BinDrivers.hxx>
#include <BinDrivers_DocumentStorageDriver.hxx>
#include <BinLDrivers.hxx>
#include <BinXCAFDrivers.hxx>
#include <DNaming.hxx>
#include <DNaming_BoxDriver.hxx>
#include <ModelDefinitions.hxx>
#include <XmlDrivers.hxx>

#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_Array1.hxx>
#include <OSD.hxx>
#include <OSD_Thread.hxx>
#include <PCDM_ReaderFilter.hxx>
#include <PCDM_ReaderStatus.hxx>
#include <PCDM_StoreStatus.hxx>
#include <Precision.hxx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_IntegerList.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_RealList.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_Relation.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDataStd_Directory.hxx>
#include <TDataStd_NoteBook.hxx>
#include <TDataStd_Tick.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_Variable.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Pattern.hxx>
#include <TDataXtd_PatternStd.hxx>
#include <TDataXtd_Placement.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataXtd_Position.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDataXtd_Triangulation.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_TagSource.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_XLink.hxx>
#include <TDocStd_XLinkTool.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_DriverTable.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NameType.hxx>
#include <TNaming_Selector.hxx>
#include <TNaming_Tool.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Shape.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <TopLoc_Location.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_Location.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <cmath>
#include <cstring>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

namespace
{
static occ::handle<TDocStd_Application> NewApplication()
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  XmlDrivers::DefineFormat(anApplication);
  BinDrivers::DefineFormat(anApplication);
  BinLDrivers::DefineFormat(anApplication);
  return anApplication;
}

static occ::handle<TDocStd_Document> NewDocument(
  const occ::handle<TDocStd_Application>& theApplication,
  const char*                             theFormat)
{
  occ::handle<TDocStd_Document> aDocument;
  theApplication->NewDocument(theFormat, aDocument);
  EXPECT_FALSE(aDocument.IsNull());
  if (!aDocument.IsNull())
  {
    aDocument->SetUndoLimit(100);
  }
  return aDocument;
}

static occ::handle<TDocStd_Document> SaveAndOpenStream(
  const occ::handle<TDocStd_Application>& theApplication,
  const occ::handle<TDocStd_Document>&    theDocument);

static occ::handle<TDocStd_Document> SaveAndOpen(
  const occ::handle<TDocStd_Application>& theApplication,
  const occ::handle<TDocStd_Document>&    theDocument,
  const char*                             theFormat,
  const char*                             theTestName,
  const bool                              theUseStream = false)
{
  if (theUseStream)
  {
    return SaveAndOpenStream(theApplication, theDocument);
  }

  SCOPED_TRACE(theTestName);
  EXPECT_EQ(theDocument->StorageFormat(), TCollection_ExtendedString(theFormat));
  std::stringstream aStream;
  theDocument->NewCommand();
  EXPECT_EQ(theApplication->SaveAs(theDocument, aStream), PCDM_SS_OK);
  EXPECT_FALSE(aStream.str().empty());

  theApplication->Close(theDocument);
  aStream.seekg(0);
  occ::handle<TDocStd_Document> aRestored;
  EXPECT_EQ(theApplication->Open(aStream, aRestored), PCDM_RS_OK);
  EXPECT_FALSE(aRestored.IsNull());
  return aRestored;
}

static occ::handle<TDocStd_Document> SaveAndOpenStream(
  const occ::handle<TDocStd_Application>& theApplication,
  const occ::handle<TDocStd_Document>&    theDocument)
{
  std::stringstream aStream;
  theDocument->NewCommand();
  EXPECT_EQ(theApplication->SaveAs(theDocument, aStream), PCDM_SS_OK);
  EXPECT_FALSE(aStream.str().empty());

  theApplication->Close(theDocument);
  aStream.seekg(0);
  occ::handle<TDocStd_Document> aRestored;
  EXPECT_EQ(theApplication->Open(aStream, aRestored), PCDM_RS_OK);
  EXPECT_FALSE(aRestored.IsNull());
  return aRestored;
}

static void SaveDocumentStream(const occ::handle<TDocStd_Application>& theApplication,
                               const occ::handle<TDocStd_Document>&    theDocument,
                               std::stringstream&                      theStream)
{
  EXPECT_EQ(theApplication->SaveAs(theDocument, theStream), PCDM_SS_OK);
  EXPECT_FALSE(theStream.str().empty());
}

static occ::handle<TDocStd_Document> OpenDocumentStream(
  const occ::handle<TDocStd_Application>& theApplication,
  std::stringstream&                      theStream)
{
  theStream.clear();
  theStream.seekg(0);
  occ::handle<TDocStd_Document> aRestored;
  EXPECT_EQ(theApplication->Open(theStream, aRestored), PCDM_RS_OK);
  EXPECT_FALSE(aRestored.IsNull());
  return aRestored;
}

static PCDM_ReaderStatus OpenDocumentStream(const occ::handle<TDocStd_Application>& theApplication,
                                            std::stringstream&                      theStream,
                                            occ::handle<TDocStd_Document>&          theDocument,
                                            const occ::handle<PCDM_ReaderFilter>&   theFilter)
{
  theStream.clear();
  theStream.seekg(0);
  return theApplication->Open(theStream, theDocument, theFilter);
}

// bugs/caf/bug33855: partial XBF retrieval resolves a complete chain of TreeNode references.
TEST(TDocStd_Storage_Test, Cbf_CafBug33855_DeepTreeNodeReferencesAfterPartialRead)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const int aChainStarts[] = {1, 5};
  for (const int aStartTag : aChainStarts)
  {
    occ::handle<TDataStd_TreeNode> aParentNode;
    for (size_t anOffset = 0; anOffset < 4; ++anOffset)
    {
      const int                      aTag   = aStartTag + static_cast<int>(anOffset);
      const TDF_Label                aLabel = aDocument->Main().FindChild(aTag, true);
      occ::handle<TDataStd_TreeNode> aNode  = TDataStd_TreeNode::Set(aLabel);
      ASSERT_FALSE(aNode.IsNull());
      if (!aParentNode.IsNull())
      {
        ASSERT_TRUE(aParentNode->Append(aNode));
      }
      aParentNode = aNode;
    }
  }

  std::stringstream aStream;
  SaveDocumentStream(anApplication, aDocument, aStream);
  anApplication->Close(aDocument);

  occ::handle<PCDM_ReaderFilter> aFilter = new PCDM_ReaderFilter;
  aFilter->AddPath("0:1:4");
  aFilter->AddPath("0:1:8");
  occ::handle<TDocStd_Document> aRestored;
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aRestored, aFilter), PCDM_RS_OK);
  ASSERT_FALSE(aRestored.IsNull());

  const int aLeafTags[] = {4, 8};
  for (const int aLeafTag : aLeafTags)
  {
    occ::handle<TDataStd_TreeNode> aNode;
    ASSERT_TRUE(aRestored->Main()
                  .FindChild(aLeafTag, false)
                  .FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode));
    for (size_t anOffset = 1; anOffset < 4; ++anOffset)
    {
      aNode = aNode->Father();
      ASSERT_FALSE(aNode.IsNull());
      ASSERT_FALSE(aNode->Label().IsNull());
      EXPECT_EQ(aNode->Label().Tag(), aLeafTag - static_cast<int>(anOffset));
    }
    EXPECT_TRUE(aNode->Father().IsNull());
  }
  anApplication->Close(aRestored);
}

// bugs/caf/bug33855: XCAF shape references remain usable after partial XBF retrieval.
TEST(TDocStd_Storage_Test, Cbf_CafBug33855_DeepShapeReferencesAfterPartialRead)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  BinXCAFDrivers::DefineFormat(anApplication);
  occ::handle<TDocStd_Document> aDocument = NewDocument(anApplication, "BinXCAF");
  ASSERT_FALSE(aDocument.IsNull());

  const TopoDS_Shape             aBox       = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  ASSERT_FALSE(aShapeTool.IsNull());
  TDF_Label aReferredLabel = aShapeTool->AddShape(aBox);
  ASSERT_FALSE(aReferredLabel.IsNull());

  TDF_Label aReferenceLabel;
  for (size_t aDepth = 0; aDepth < 4; ++aDepth)
  {
    aReferenceLabel = TDF_TagSource::NewChild(aShapeTool->Label());
    ASSERT_FALSE(aReferenceLabel.IsNull());
    ASSERT_FALSE(XCAFDoc_Location::Set(aReferenceLabel, TopLoc_Location()).IsNull());
    occ::handle<TDataStd_TreeNode> aMainNode =
      TDataStd_TreeNode::Set(aReferredLabel, XCAFDoc::ShapeRefGUID());
    occ::handle<TDataStd_TreeNode> aReferenceNode =
      TDataStd_TreeNode::Set(aReferenceLabel, XCAFDoc::ShapeRefGUID());
    ASSERT_FALSE(aMainNode.IsNull());
    ASSERT_FALSE(aReferenceNode.IsNull());
    ASSERT_TRUE(aMainNode->Append(aReferenceNode));
    aReferredLabel = aReferenceLabel;
  }

  TCollection_AsciiString aReferenceEntry;
  TDF_Tool::Entry(aReferenceLabel, aReferenceEntry);
  std::stringstream aStream;
  SaveDocumentStream(anApplication, aDocument, aStream);
  anApplication->Close(aDocument);

  occ::handle<PCDM_ReaderFilter> aFilter = new PCDM_ReaderFilter;
  aFilter->AddPath(aReferenceEntry);
  occ::handle<TDocStd_Document> aRestored;
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aRestored, aFilter), PCDM_RS_OK);
  ASSERT_FALSE(aRestored.IsNull());

  TDF_Label aRestoredReference;
  TDF_Tool::Label(aRestored->GetData(), aReferenceEntry, aRestoredReference, false);
  ASSERT_FALSE(aRestoredReference.IsNull());
  TopoDS_Shape aRestoredShape;
  ASSERT_NO_THROW(EXPECT_TRUE(XCAFDoc_ShapeTool::GetShape(aRestoredReference, aRestoredShape)));
  ASSERT_FALSE(aRestoredShape.IsNull());
  Bnd_Box aBounds;
  BRepBndLib::Add(aRestoredShape, aBounds);
  const auto [aXMin, aXMax, aYMin, aYMax, aZMin, aZMax] = aBounds.Get();
  const double aTolerance                               = 2.0 * Precision::Confusion();
  EXPECT_NEAR(aXMin, 0.0, aTolerance);
  EXPECT_NEAR(aYMin, 0.0, aTolerance);
  EXPECT_NEAR(aZMin, 0.0, aTolerance);
  EXPECT_NEAR(aXMax, 10.0, aTolerance);
  EXPECT_NEAR(aYMax, 20.0, aTolerance);
  EXPECT_NEAR(aZMax, 30.0, aTolerance);
  anApplication->Close(aRestored);
}

static void RunBinaryEmptyLists()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label aRoot = aDocument->GetData()->Root();
  ASSERT_FALSE(TDataStd_BooleanList::Set(aRoot.FindChild(1, true).FindChild(1, true)).IsNull());
  ASSERT_FALSE(TDataStd_IntegerList::Set(aRoot.FindChild(1, true).FindChild(2, true)).IsNull());
  ASSERT_FALSE(TDataStd_RealList::Set(aRoot.FindChild(1, true).FindChild(3, true)).IsNull());
  ASSERT_FALSE(TDataStd_ExtStringList::Set(aRoot.FindChild(1, true).FindChild(4, true)).IsNull());
  ASSERT_FALSE(TDataStd_ReferenceList::Set(aRoot.FindChild(1, true).FindChild(5, true)).IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredRoot = aRestored->GetData()->Root().FindChild(1, false);
  ASSERT_FALSE(aRestoredRoot.IsNull());

  occ::handle<TDataStd_BooleanList> aBooleanList;
  ASSERT_TRUE(
    aRestoredRoot.FindChild(1, false).FindAttribute(TDataStd_BooleanList::GetID(), aBooleanList));
  EXPECT_TRUE(aBooleanList->IsEmpty());
  EXPECT_EQ(aBooleanList->Extent(), 0);

  occ::handle<TDataStd_IntegerList> anIntegerList;
  ASSERT_TRUE(
    aRestoredRoot.FindChild(2, false).FindAttribute(TDataStd_IntegerList::GetID(), anIntegerList));
  EXPECT_TRUE(anIntegerList->IsEmpty());
  EXPECT_EQ(anIntegerList->Extent(), 0);

  occ::handle<TDataStd_RealList> aRealList;
  ASSERT_TRUE(
    aRestoredRoot.FindChild(3, false).FindAttribute(TDataStd_RealList::GetID(), aRealList));
  EXPECT_TRUE(aRealList->IsEmpty());
  EXPECT_EQ(aRealList->Extent(), 0);

  occ::handle<TDataStd_ExtStringList> anExtStringList;
  ASSERT_TRUE(aRestoredRoot.FindChild(4, false).FindAttribute(TDataStd_ExtStringList::GetID(),
                                                              anExtStringList));
  EXPECT_TRUE(anExtStringList->IsEmpty());
  EXPECT_EQ(anExtStringList->Extent(), 0);

  occ::handle<TDataStd_ReferenceList> aReferenceList;
  ASSERT_TRUE(aRestoredRoot.FindChild(5, false).FindAttribute(TDataStd_ReferenceList::GetID(),
                                                              aReferenceList));
  EXPECT_TRUE(aReferenceList->IsEmpty());
  EXPECT_EQ(aReferenceList->Extent(), 0);
}

// bugs/caf/bug25394_1: empty standard lists survive binary OCAF streams.
TEST(TDocStd_Storage_Test, Cbf_CafBug_25394_1_EmptyLists)
{
  RunBinaryEmptyLists();
}

// bugs/caf/bug25394_2: empty standard lists survive binary OCAF streams.
TEST(TDocStd_Storage_Test, Cbf_CafBug_25394_2_EmptyLists)
{
  RunBinaryEmptyLists();
}

class XbfStreamLoader : public OSD_Thread
{
public:
  XbfStreamLoader(const occ::handle<TDocStd_Application>& theApplication,
                  const std::string&                      theData)
      : OSD_Thread(PerformThread),
        myApplication(theApplication),
        myData(theData),
        myStatus(PCDM_RS_DriverFailure)
  {
  }

  PCDM_ReaderStatus Status() const { return myStatus; }

  const occ::handle<TDocStd_Document>& Document() const { return myDocument; }

private:
  void Perform()
  {
    std::istringstream aStream(myData);
    myStatus = myApplication->Open(aStream, myDocument);
  }

  static void* PerformThread(void* theData)
  {
    XbfStreamLoader* aLoader = static_cast<XbfStreamLoader*>(theData);
    OSD::SetThreadLocalSignal(OSD_SignalMode_Set, false);
    try
    {
      aLoader->Perform();
    }
    catch (const Standard_Failure&)
    {
      // Keep the failure status and report it from the joining test thread.
    }
    return nullptr;
  }

private:
  occ::handle<TDocStd_Application> myApplication;
  std::string                      myData;
  occ::handle<TDocStd_Document>    myDocument;
  PCDM_ReaderStatus                myStatus;
};

// bugs/caf/bug31785: an XBF document can be opened from a background thread.
TEST(TDocStd_Storage_Test, CafBug_31785_BackgroundXbfStreamOpen)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApplication);
  occ::handle<TDocStd_Document> aDocument = NewDocument(anApplication, "BinXCAF");
  ASSERT_FALSE(aDocument.IsNull());

  std::stringstream      aStream;
  const PCDM_StoreStatus aStoreStatus = anApplication->SaveAs(aDocument, aStream);
  ASSERT_EQ(aStoreStatus, PCDM_SS_No_Obj);
  const std::string aData = aStream.str();
  ASSERT_FALSE(aData.empty());
  anApplication->Close(aDocument);

  XbfStreamLoader aLoader(anApplication, aData);
  ASSERT_TRUE(aLoader.Run(&aLoader));
  ASSERT_TRUE(aLoader.Wait());
  // The original DRAW case saves an empty document and ignores the resulting read status.
  // The stream is consequently rejected as incomplete, but opening it in the worker must not
  // crash the application.
  EXPECT_EQ(aLoader.Status(), PCDM_RS_UnrecognizedFileFormat);
  EXPECT_FALSE(aLoader.Document().IsNull());
}

// bugs/caf/bug26229_1: binary OCAF SaveAs/Open stream APIs preserve attributes.
TEST(TDocStd_Storage_Test, Cbf_CafBug_26229_1_StreamInteger)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(2, true);
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel, 100).IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label               aRestoredLabel = aRestored->GetData()->Root().FindChild(2, false);
  occ::handle<TDataStd_Integer> anInteger;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  EXPECT_EQ(anInteger->Get(), 100);
}

// bugs/caf/bug23306: an extended-string array is read from binary OCAF without warnings.
TEST(TDocStd_Storage_Test, Cbf_CafBug_23306_ExtStringArray)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(1, true).FindChild(1, true);
  occ::handle<TDataStd_ExtStringArray> anArray = TDataStd_ExtStringArray::Set(aLabel, 1, 3);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, TCollection_ExtendedString("String1"));
  anArray->SetValue(2, TCollection_ExtendedString("String2"));
  anArray->SetValue(3, TCollection_ExtendedString("String3"));

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ExtStringArray> aRestoredArray;
  ASSERT_TRUE(aRestored->GetData()->Root().FindChild(1, false).FindChild(1, false).FindAttribute(
    TDataStd_ExtStringArray::GetID(),
    aRestoredArray));
  ASSERT_FALSE(aRestoredArray.IsNull());
  ASSERT_EQ(aRestoredArray->Lower(), 1);
  ASSERT_EQ(aRestoredArray->Upper(), 3);
  EXPECT_TRUE(aRestoredArray->Value(1).IsEqual(TCollection_ExtendedString("String1")));
  EXPECT_TRUE(aRestoredArray->Value(2).IsEqual(TCollection_ExtendedString("String2")));
  EXPECT_TRUE(aRestoredArray->Value(3).IsEqual(TCollection_ExtendedString("String3")));
}

// bugs/caf/bug425: an empty TDataStd_Name survives binary OCAF streams.
TEST(TDocStd_Storage_Test, Cbf_CafBug_425_EmptyName)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(2, true);
  ASSERT_FALSE(TDataStd_Name::Set(aLabel, TCollection_ExtendedString()).IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_Name> aRestoredName;
  ASSERT_TRUE(aRestored->GetData()->Root().FindChild(2, false).FindAttribute(TDataStd_Name::GetID(),
                                                                             aRestoredName));
  ASSERT_FALSE(aRestoredName.IsNull());
  EXPECT_TRUE(aRestoredName->Get().IsEmpty());
}

// bugs/caf/bug28428: a NamedShape with no stored nodes survives binary OCAF streams.
TEST(TDocStd_Storage_Test, Cbf_CafBug_28428_EmptyNamedShape)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label                 aLabel = aDocument->GetData()->Root().FindChild(1, true);
  TNaming_Builder                 aBuilder(aLabel);
  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TNaming_NamedShape> aRestoredNamedShape;
  const TDF_Label                 aRestoredLabel = aRestored->GetData()->Root().FindChild(1, false);
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TNaming_NamedShape::GetID(), aRestoredNamedShape));
  TNaming_Iterator anIterator(aRestoredNamedShape);
  EXPECT_FALSE(anIterator.More());
}

static void SetShape(const TDF_Label& theLabel, const TopoDS_Shape& theShape)
{
  TNaming_Builder aBuilder(theLabel);
  aBuilder.Generated(theShape);
}

static occ::handle<TDataStd_UAttribute> AddNamingObject(
  const occ::handle<TDocStd_Document>& theDocument)
{
  occ::handle<TDataStd_TreeNode>   aRootNode     = TDataStd_TreeNode::Set(theDocument->Main());
  const TDF_Label                  anObjectLabel = TDF_TagSource::NewChild(theDocument->Main());
  occ::handle<TDataStd_UAttribute> anObject =
    TDataStd_UAttribute::Set(anObjectLabel, GEOMOBJECT_GUID);
  occ::handle<TDataStd_TreeNode> anObjectNode = TDataStd_TreeNode::Set(anObjectLabel);
  if (aRootNode.IsNull() || anObject.IsNull() || anObjectNode.IsNull()
      || !aRootNode->Append(anObjectNode))
  {
    ADD_FAILURE() << "Failed to create the naming object";
    return occ::handle<TDataStd_UAttribute>();
  }
  return anObject;
}

static occ::handle<TFunction_Function> AddNamingFunction(
  const occ::handle<TDataStd_UAttribute>& theObject,
  const Standard_GUID&                    theDriverGuid)
{
  const TDF_Label                 aFunctionLabel = TDF_TagSource::NewChild(theObject->Label());
  occ::handle<TFunction_Function> aFunction =
    TFunction_Function::Set(aFunctionLabel, theDriverGuid);
  occ::handle<TDataStd_TreeNode> aFunctionNode = TDataStd_TreeNode::Set(aFunctionLabel);
  occ::handle<TDataStd_TreeNode> anObjectNode;
  if (aFunction.IsNull() || aFunctionNode.IsNull()
      || !theObject->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), anObjectNode)
      || anObjectNode.IsNull() || !anObjectNode->Append(aFunctionNode))
  {
    ADD_FAILURE() << "Failed to create the naming function";
    return occ::handle<TFunction_Function>();
  }

  const TDF_Label                anArgumentsLabel = TDF_TagSource::NewChild(aFunctionLabel);
  const TDF_Label                aResultLabel     = TDF_TagSource::NewChild(aFunctionLabel);
  occ::handle<TDataStd_TreeNode> anArgumentsNode  = TDataStd_TreeNode::Set(anArgumentsLabel);
  occ::handle<TDataStd_TreeNode> aResultNode      = TDataStd_TreeNode::Set(aResultLabel);
  if (anArgumentsNode.IsNull() || aResultNode.IsNull() || !aFunctionNode->Append(anArgumentsNode)
      || !aFunctionNode->Append(aResultNode)
      || TDF_Reference::Set(theObject->Label(), aResultLabel).IsNull())
  {
    ADD_FAILURE() << "Failed to initialize the naming function labels";
    return occ::handle<TFunction_Function>();
  }
  return aFunction;
}

static int ExecuteNamingFunction(const occ::handle<TFunction_Function>& theFunction,
                                 const occ::handle<TFunction_Driver>&   theDriver)
{
  occ::handle<TFunction_Logbook> aLogbook = TFunction_Logbook::Set(theFunction->Label());
  aLogbook->Clear();
  theDriver->Init(theFunction->Label());
  return theDriver->Execute(aLogbook);
}

static TopoDS_Shape NamingFunctionShape(const occ::handle<TFunction_Function>& theFunction)
{
  const occ::handle<TNaming_NamedShape> aNamedShape = DNaming::GetFunctionResult(theFunction);
  return aNamedShape.IsNull() ? TopoDS_Shape() : TNaming_Tool::GetShape(aNamedShape);
}

static TopoDS_Shape ShapeOf(const TDF_Label& theLabel)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  if (!theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
  {
    return TopoDS_Shape();
  }
  return TNaming_Tool::GetShape(aNamedShape);
}

static void ExpectBoxBounds(const TopoDS_Shape& theShape,
                            const double        theXMin,
                            const double        theYMin,
                            const double        theZMin,
                            const double        theXMax,
                            const double        theYMax,
                            const double        theZMax)
{
  ASSERT_FALSE(theShape.IsNull());
  Bnd_Box aBounds;
  BRepBndLib::Add(theShape, aBounds);
  double aXMin = 0.0;
  double aYMin = 0.0;
  double aZMin = 0.0;
  double aXMax = 0.0;
  double aYMax = 0.0;
  double aZMax = 0.0;
  aBounds.Get(aXMin, aYMin, aZMin, aXMax, aYMax, aZMax);
  EXPECT_NEAR(aXMin, theXMin, 1.0e-6);
  EXPECT_NEAR(aYMin, theYMin, 1.0e-6);
  EXPECT_NEAR(aZMin, theZMin, 1.0e-6);
  EXPECT_NEAR(aXMax, theXMax, 1.0e-6);
  EXPECT_NEAR(aYMax, theYMax, 1.0e-6);
  EXPECT_NEAR(aZMax, theZMax, 1.0e-6);
}

static void ExpectLinked(const TDF_Label& theTarget, const TDF_Label& theSource)
{
  occ::handle<TDocStd_XLink> aLink;
  ASSERT_TRUE(theTarget.FindAttribute(TDocStd_XLink::GetID(), aLink));
  EXPECT_FALSE(aLink->LabelEntry().IsEmpty());

  occ::handle<TDF_Reference> aReference;
  ASSERT_TRUE(theTarget.FindAttribute(TDF_Reference::GetID(), aReference));
  EXPECT_TRUE(aReference->Get() == theSource);
}

static TDF_Label LabelByEntry(const occ::handle<TDocStd_Document>& theDocument,
                              const TCollection_AsciiString&       theEntry)
{
  TDF_Label aLabel;
  TDF_Tool::Label(theDocument->GetData(), theEntry, aLabel);
  return aLabel;
}

static int CollectUniqueSubShapes(const TopoDS_Shape&    theShape,
                                  const TopAbs_ShapeEnum theType,
                                  TopoDS_Shape*          theSubShapes,
                                  const int              theCapacity)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShapeMap;
  aShapeMap.Add(theShape);
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
  {
    if (aShapeMap.Add(anExplorer.Current()))
    {
      if (aCount < theCapacity)
      {
        theSubShapes[aCount] = anExplorer.Current();
      }
      ++aCount;
    }
  }
  return aCount;
}

static void SelectImportedShape(const TDF_Label&    theSelectionLabel,
                                const TopoDS_Shape& theSelection,
                                const TopoDS_Shape& theContext)
{
  TNaming_Selector aSelector(theSelectionLabel);
  ASSERT_TRUE(aSelector.Select(theSelection, theContext, false));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector.NamedShape()).IsSame(theSelection));
}

static void ExpectNamingSelection(const TDF_Label&       theSelectionLabel,
                                  const TNaming_NameType theExpectedType,
                                  const TopAbs_ShapeEnum theExpectedShapeType,
                                  const TDF_Label*       theExpectedArguments,
                                  const int              theExpectedArgumentCount)
{
  occ::handle<TNaming_Naming> aNaming;
  ASSERT_TRUE(theSelectionLabel.FindAttribute(TNaming_Naming::GetID(), aNaming));
  const TNaming_Name& aName = aNaming->GetName();
  EXPECT_EQ(aName.Type(), theExpectedType);
  EXPECT_EQ(aName.ShapeType(), theExpectedShapeType);
  EXPECT_EQ(aName.Arguments().Extent(), theExpectedArgumentCount);

  // The DRAW DumpSelection output for this case is TODO and its textual
  // argument order is not a stable naming invariant. Check the complete set
  // of referenced labels instead.
  for (int anExpectedIndex = 0; anExpectedIndex < theExpectedArgumentCount; ++anExpectedIndex)
  {
    bool hasArgument = false;
    for (NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator anIterator(aName.Arguments());
         anIterator.More();
         anIterator.Next())
    {
      if (!anIterator.Value().IsNull()
          && anIterator.Value()->Label().IsEqual(theExpectedArguments[anExpectedIndex]))
      {
        hasArgument = true;
        break;
      }
    }
    EXPECT_TRUE(hasArgument);
  }
}

static void RunB7(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label aSource = aDocument->Main().FindChild(2, true);
  const TDF_Label aTarget = aDocument->Main().FindChild(3, true);
  ASSERT_FALSE(TDataStd_Name::Set(aSource, "SLabel").IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aTarget, "TLabel1").IsNull());
  const TopoDS_Shape aBox =
    BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), gp_Pnt(110.0, 220.0, 330.0)).Shape();
  SetShape(aSource, aBox);

  aDocument->NewCommand();
  TDocStd_XLinkTool aTool;
  aTool.CopyWithLink(aTarget, aSource);
  ASSERT_TRUE(aTool.IsDone());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_copy_with_link", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredSource = aRestored->Main().FindChild(2, false);
  const TDF_Label aRestoredTarget = aRestored->Main().FindChild(3, false);
  ASSERT_FALSE(aRestoredSource.IsNull());
  ASSERT_FALSE(aRestoredTarget.IsNull());
  ExpectLinked(aRestoredTarget, aRestoredSource);
  ExpectBoxBounds(ShapeOf(aRestoredTarget), 10.0, 20.0, 30.0, 110.0, 220.0, 330.0);
}

static void RunC2(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label    aShapeLabel = aDocument->Main().FindChild(2, true);
  const TopoDS_Shape aBox =
    BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), gp_Pnt(110.0, 220.0, 330.0)).Shape();
  SetShape(aShapeLabel, aBox);
  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(aShapeLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));

  const TDF_Label            aRealLabel = aDocument->Main().FindChild(1, true).FindChild(1, true);
  const TDF_Label            aConstraintLabel = aDocument->Main().FindChild(3, true);
  occ::handle<TDataStd_Real> aReal            = TDataStd_Real::Set(aRealLabel, 123.456789);
  ASSERT_FALSE(aReal.IsNull());
  occ::handle<TDataXtd_Constraint> aConstraint = TDataXtd_Constraint::Set(aConstraintLabel);
  aConstraint->SetType(TDataXtd_TANGENT);
  aConstraint->SetGeometry(1, aNamedShape);
  aConstraint->SetPlane(aNamedShape);
  aConstraint->SetValue(aReal);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_constraint");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredConstraintLabel = aRestored->Main().FindChild(3, false);
  ASSERT_FALSE(aRestoredConstraintLabel.IsNull());
  occ::handle<TDataXtd_Constraint> aRestoredConstraint;
  ASSERT_TRUE(
    aRestoredConstraintLabel.FindAttribute(TDataXtd_Constraint::GetID(), aRestoredConstraint));
  EXPECT_EQ(aRestoredConstraint->GetType(), TDataXtd_TANGENT);
  ASSERT_EQ(aRestoredConstraint->NbGeometries(), 1);
  ASSERT_FALSE(aRestoredConstraint->GetGeometry(1).IsNull());
  ASSERT_FALSE(aRestoredConstraint->GetPlane().IsNull());
  ExpectBoxBounds(TNaming_Tool::GetShape(aRestoredConstraint->GetGeometry(1)),
                  10.0,
                  20.0,
                  30.0,
                  110.0,
                  220.0,
                  330.0);
  ASSERT_FALSE(aRestoredConstraint->GetValue().IsNull());
  EXPECT_DOUBLE_EQ(aRestoredConstraint->GetValue()->Get(), 123.456789);
}

static void RunC3(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label    aLabel = aDocument->Main().FindChild(2, true);
  const TopoDS_Shape aBox   = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  SetShape(aLabel, aBox);
  occ::handle<TDataXtd_Geometry> aGeometry = TDataXtd_Geometry::Set(aLabel);
  ASSERT_FALSE(aGeometry.IsNull());
  aGeometry->SetType(TDataXtd_POINT);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_geometry_type");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  occ::handle<TDataXtd_Geometry> aRestoredGeometry;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataXtd_Geometry::GetID(), aRestoredGeometry));
  EXPECT_EQ(aRestoredGeometry->GetType(), TDataXtd_POINT);
}

static void RunC4(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TopoDS_Shape aBox        = BRepPrimAPI_MakeBox(10.0, 20.0, 777.0).Shape();
  const TDF_Label    aShapeLabel = aDocument->Main().FindChild(1, true);
  SetShape(aShapeLabel, aBox);
  const TDF_Label  aSelectionLabel = aShapeLabel.FindChild(1, true);
  TNaming_Selector aSelector(aSelectionLabel);
  ASSERT_TRUE(aSelector.Select(aBox));

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_selection");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredSelection = aRestored->Main().FindChild(1, false).FindChild(1, false);
  ASSERT_FALSE(aRestoredSelection.IsNull());
  occ::handle<TNaming_Naming> aNaming;
  ASSERT_TRUE(aRestoredSelection.FindAttribute(TNaming_Naming::GetID(), aNaming));
  ExpectBoxBounds(ShapeOf(aRestoredSelection), 0.0, 0.0, 0.0, 10.0, 20.0, 777.0);
}

static void RunC5(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aShapeLabel    = aDocument->Main().FindChild(2, true);
  const TDF_Label aRealLabel     = aDocument->Main().FindChild(3, true);
  const TDF_Label anIntegerLabel = aDocument->Main().FindChild(4, true);
  const TDF_Label aPatternLabel  = aDocument->Main().FindChild(1, true).FindChild(1, true);
  SetShape(aShapeLabel, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(aShapeLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  occ::handle<TDataStd_Real>    aReal     = TDataStd_Real::Set(aRealLabel, 45.0);
  occ::handle<TDataStd_Integer> anInteger = TDataStd_Integer::Set(anIntegerLabel, 7);
  ASSERT_FALSE(aReal.IsNull());
  ASSERT_FALSE(anInteger.IsNull());
  occ::handle<TDataXtd_PatternStd> aPattern = TDataXtd_PatternStd::Set(aPatternLabel);
  aPattern->Signature(1);
  aPattern->Axis1(aNamedShape);
  aPattern->Value1(aReal);
  aPattern->NbInstances1(anInteger);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_pattern");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredPatternLabel = aRestored->Main().FindChild(1, false).FindChild(1, false);
  ASSERT_FALSE(aRestoredPatternLabel.IsNull());
  occ::handle<TDataXtd_PatternStd> aRestoredPattern;
  ASSERT_TRUE(aRestoredPatternLabel.FindAttribute(TDataXtd_Pattern::GetID(), aRestoredPattern));
  EXPECT_EQ(aRestoredPattern->Signature(), 1);
  ASSERT_FALSE(aRestoredPattern->Axis1().IsNull());
  ExpectBoxBounds(TNaming_Tool::GetShape(aRestoredPattern->Axis1()),
                  0.0,
                  0.0,
                  0.0,
                  10.0,
                  20.0,
                  30.0);
  ASSERT_FALSE(aRestoredPattern->Value1().IsNull());
  ASSERT_FALSE(aRestoredPattern->NbInstances1().IsNull());
  EXPECT_DOUBLE_EQ(aRestoredPattern->Value1()->Get(), 45.0);
  EXPECT_EQ(aRestoredPattern->NbInstances1()->Get(), 7);
}

static void RunC7(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = aDocument->Main().FindChild(1, true).FindChild(1, true);
  const gp_Pnt    anExpected(123.456, 789.012, 345.678);
  TDataXtd_Position::Set(aLabel, anExpected);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_position");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(1, false).FindChild(1, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  gp_Pnt anActual;
  ASSERT_TRUE(TDataXtd_Position::Get(aRestoredLabel, anActual));
  EXPECT_TRUE(anActual.IsEqual(anExpected, 1.0e-12));
}

static void RunD2(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const int                                   aTags[][6]    = {{3},
                                                               {2076534922, 524398634, 912349856},
                                                               {3, 9283, 12, 1, 9843, 46793},
                                                               {4, 81245034, 321, 1, 1, 1},
                                                               {5, 8, 4, 2, 213, 3242},
                                                               {2, 15, 123, 31214, 452398, 421},
                                                               {2, 2, 1, 1, 1, 3},
                                                               {2, 9}};
  const int                                   aTagLengths[] = {1, 3, 6, 6, 6, 6, 6, 2};
  NCollection_Array1<TDF_Label>               aLabels(1, 8);
  NCollection_Array1<TCollection_AsciiString> anEntries(1, 8);
  for (int anIndex = 1; anIndex <= 8; ++anIndex)
  {
    TDF_Label aLabel = aDocument->Main();
    for (int aTagIndex = 0; aTagIndex < aTagLengths[anIndex - 1]; ++aTagIndex)
    {
      aLabel = aLabel.FindChild(aTags[anIndex - 1][aTagIndex], true);
    }
    aLabels.SetValue(anIndex, aLabel);
    TDF_Tool::Entry(aLabel, anEntries.ChangeValue(anIndex));
  }

  for (int anIndex = 1; anIndex <= 8; ++anIndex)
  {
    TDataStd_Integer::Set(aLabels.Value(anIndex), anIndex);
    TDF_Reference::Set(aLabels.Value(anIndex), aLabels.Value(anIndex == 1 ? 8 : anIndex - 1));
  }

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_reference_chain_alias");
  ASSERT_FALSE(aRestored.IsNull());
  for (int anIndex = 1; anIndex <= 8; ++anIndex)
  {
    const TDF_Label aRestoredLabel = LabelByEntry(aRestored, anEntries.Value(anIndex));
    ASSERT_FALSE(aRestoredLabel.IsNull());
    occ::handle<TDF_Reference> aReference;
    ASSERT_TRUE(aRestoredLabel.FindAttribute(TDF_Reference::GetID(), aReference));
    const int anExpectedIndex = anIndex == 1 ? 8 : anIndex - 1;
    EXPECT_TRUE(aReference->Get() == LabelByEntry(aRestored, anEntries.Value(anExpectedIndex)));
    occ::handle<TDataStd_Integer> anInteger;
    ASSERT_TRUE(aReference->Get().FindAttribute(TDataStd_Integer::GetID(), anInteger));
    EXPECT_EQ(anInteger->Get(), anExpectedIndex);
  }
}

static void RunD3(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aVariableLabel1 =
    aDocument->GetData()->Root().FindChild(1, true).FindChild(1, true);
  const TDF_Label aVariableLabel2 =
    aDocument->GetData()->Root().FindChild(1, true).FindChild(2, true);
  const TDF_Label aVariableLabel3 =
    aDocument->GetData()->Root().FindChild(1, true).FindChild(3, true);
  const TDF_Label aRelationLabel =
    aDocument->GetData()->Root().FindChild(1, true).FindChild(13, true);
  occ::handle<TDataStd_Variable> aVariable1 = TDataStd_Variable::Set(aVariableLabel1);
  occ::handle<TDataStd_Variable> aVariable2 = TDataStd_Variable::Set(aVariableLabel2);
  occ::handle<TDataStd_Variable> aVariable3 = TDataStd_Variable::Set(aVariableLabel3);
  aVariable1->Unit("N");
  aVariable2->Unit("kg");
  aVariable3->Unit("m/s2");
  aVariable1->Constant(true);
  aVariable2->Constant(false);
  aVariable3->Constant(false);
  occ::handle<TDataStd_Relation> aRelation = TDataStd_Relation::Set(aRelationLabel);
  aRelation->SetRelation(TCollection_ExtendedString("f = m*a"));
  aRelation->GetVariables().Append(aVariable1);
  aRelation->GetVariables().Append(aVariable2);
  aRelation->GetVariables().Append(aVariable3);

  TCollection_AsciiString aRelationEntry;
  TDF_Tool::Entry(aRelationLabel, aRelationEntry);
  TCollection_AsciiString aVariableEntry1;
  TCollection_AsciiString aVariableEntry2;
  TCollection_AsciiString aVariableEntry3;
  TDF_Tool::Entry(aVariableLabel1, aVariableEntry1);
  TDF_Tool::Entry(aVariableLabel2, aVariableEntry2);
  TDF_Tool::Entry(aVariableLabel3, aVariableEntry3);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_relation_alias");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label                aRestoredRelationLabel = LabelByEntry(aRestored, aRelationEntry);
  occ::handle<TDataStd_Relation> aRestoredRelation;
  ASSERT_TRUE(aRestoredRelationLabel.FindAttribute(TDataStd_Relation::GetID(), aRestoredRelation));
  EXPECT_EQ(aRestoredRelation->GetRelation(), TCollection_ExtendedString("f = m*a"));
  const TCollection_AsciiString anExpectedEntries[] = {aVariableEntry1,
                                                       aVariableEntry2,
                                                       aVariableEntry3};
  int                           anIndex             = 0;
  for (NCollection_List<occ::handle<TDF_Attribute>>::Iterator anIterator(
         aRestoredRelation->GetVariables());
       anIterator.More();
       anIterator.Next(), ++anIndex)
  {
    ASSERT_LT(anIndex, 3);
    TCollection_AsciiString anActualEntry;
    TDF_Tool::Entry(anIterator.Value()->Label(), anActualEntry);
    EXPECT_STREQ(anActualEntry.ToCString(), anExpectedEntries[anIndex].ToCString());
  }
  EXPECT_EQ(anIndex, 3);

  const TDF_Label                aRestoredVariable1 = LabelByEntry(aRestored, aVariableEntry1);
  const TDF_Label                aRestoredVariable2 = LabelByEntry(aRestored, aVariableEntry2);
  const TDF_Label                aRestoredVariable3 = LabelByEntry(aRestored, aVariableEntry3);
  occ::handle<TDataStd_Variable> aRestoredVariable;
  ASSERT_TRUE(aRestoredVariable1.FindAttribute(TDataStd_Variable::GetID(), aRestoredVariable));
  EXPECT_TRUE(aRestoredVariable->IsConstant());
  EXPECT_EQ(aRestoredVariable->Unit(), TCollection_AsciiString("N"));
  ASSERT_TRUE(aRestoredVariable2.FindAttribute(TDataStd_Variable::GetID(), aRestoredVariable));
  EXPECT_FALSE(aRestoredVariable->IsConstant());
  EXPECT_EQ(aRestoredVariable->Unit(), TCollection_AsciiString("kg"));
  ASSERT_TRUE(aRestoredVariable3.FindAttribute(TDataStd_Variable::GetID(), aRestoredVariable));
  EXPECT_FALSE(aRestoredVariable->IsConstant());
  EXPECT_EQ(aRestoredVariable->Unit(), TCollection_AsciiString("m/s2"));
}

static TDF_Label AttributeLabel(const occ::handle<TDocStd_Document>& theDocument)
{
  return theDocument->Main().FindChild(2, true);
}

static void RunA1(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(TDataStd_Integer::Set(AttributeLabel(aDocument), 100).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_integer_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_Integer> anInteger;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Integer::GetID(), anInteger));
  EXPECT_EQ(anInteger->Get(), 100);
}

static void RunA2(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(TDataStd_Real::Set(AttributeLabel(aDocument), 100.0).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_real_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_Real> aReal;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_DOUBLE_EQ(aReal->Get(), 100.0);
}

static void RunA3(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<TDataStd_RealArray> anArray =
    TDataStd_RealArray::Set(AttributeLabel(aDocument), 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_real_array_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_RealArray> aRestoredArray;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_RealArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 2);
  EXPECT_DOUBLE_EQ(aRestoredArray->Value(1), 3.0);
  EXPECT_DOUBLE_EQ(aRestoredArray->Value(2), 4.0);
}

static void RunA4(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<TDataStd_IntegerArray> anArray =
    TDataStd_IntegerArray::Set(AttributeLabel(aDocument), 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_integer_array_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_IntegerArray> aRestoredArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_IntegerArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 2);
  EXPECT_EQ(aRestoredArray->Value(1), 3);
  EXPECT_EQ(aRestoredArray->Value(2), 4);
}

static void RunA5(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TCollection_ExtendedString anExpected("New Attribute");
  ASSERT_FALSE(TDataStd_Name::Set(AttributeLabel(aDocument), anExpected).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_name_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), anExpected);
}

static void RunA6(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TCollection_ExtendedString anExpected("New Attribute");
  ASSERT_FALSE(TDataStd_Comment::Set(AttributeLabel(aDocument), anExpected).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_comment_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_Comment> aComment;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Comment::GetID(), aComment));
  EXPECT_EQ(aComment->Get(), anExpected);
}

static void RunA7(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const gp_Pnt anExpected(10.0, 20.0, 30.0);
  ASSERT_FALSE(TDataXtd_Point::Set(AttributeLabel(aDocument), anExpected).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_point_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  gp_Pnt aPoint;
  ASSERT_TRUE(TDataXtd_Geometry::Point(AttributeLabel(aRestored), aPoint));
  EXPECT_TRUE(aPoint.IsEqual(anExpected, Precision::Confusion()));
}

static void RunA8(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const gp_Lin anExpected(gp_Pnt(10.0, 20.0, 30.0), gp_Dir(100.0, 200.0, 300.0));
  ASSERT_FALSE(TDataXtd_Axis::Set(AttributeLabel(aDocument), anExpected).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_axis_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  gp_Lin aLine;
  ASSERT_TRUE(TDataXtd_Geometry::Line(AttributeLabel(aRestored), aLine));
  EXPECT_TRUE(aLine.Location().IsEqual(anExpected.Location(), Precision::Confusion()));
  EXPECT_TRUE(aLine.Direction().IsEqual(anExpected.Direction(), Precision::Confusion()));
}

static void RunA9(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const gp_Pln anExpected(gp_Pnt(10.0, 20.0, 30.0), gp_Dir(-1.0, 0.0, 0.0));
  ASSERT_FALSE(TDataXtd_Plane::Set(AttributeLabel(aDocument), anExpected).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_plane_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  gp_Pln aPlane;
  ASSERT_TRUE(TDataXtd_Geometry::Plane(AttributeLabel(aRestored), aPlane));
  EXPECT_TRUE(aPlane.Location().IsEqual(anExpected.Location(), Precision::Confusion()));
  EXPECT_TRUE(
    aPlane.Axis().Direction().IsEqual(anExpected.Axis().Direction(), Precision::Confusion()));
}

static void RunB1(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const Standard_GUID aGuid("c73bd075-22ee-11d2-acde-080009dc4422");
  ASSERT_FALSE(TDataStd_UAttribute::Set(AttributeLabel(aDocument), aGuid).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_user_attribute_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_UAttribute> anAttribute;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, anAttribute));
  EXPECT_EQ(anAttribute->ID(), aGuid);
}

static void RunB2(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const Standard_GUID aGuid1("c73bd075-22ee-11d2-acde-080009dc4422");
  const Standard_GUID aGuid2("c73bd076-22ee-11d2-acde-080009dc4422");
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_UAttribute::Set(aLabel, aGuid1).IsNull());
  ASSERT_FALSE(TDataStd_UAttribute::Set(aLabel, aGuid2).IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpen(anApplication,
                                                        aDocument,
                                                        theFormat,
                                                        "draw_ocaf_two_user_attributes_bin",
                                                        theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_UAttribute> anAttribute1;
  occ::handle<TDataStd_UAttribute> anAttribute2;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid1, anAttribute1));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid2, anAttribute2));
  EXPECT_NE(anAttribute1->ID(), anAttribute2->ID());
}

static void RunB3(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  SetShape(AttributeLabel(aDocument),
           BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), gp_Pnt(110.0, 220.0, 330.0)));

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_named_shape_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  ExpectBoxBounds(ShapeOf(AttributeLabel(aRestored)), 10.0, 20.0, 30.0, 110.0, 220.0, 330.0);
}

static void RunB5(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  aDocument->NewCommand();
  SetShape(AttributeLabel(aDocument),
           BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), gp_Pnt(110.0, 220.0, 330.0)));

  occ::handle<TDocStd_Document> aRestored = SaveAndOpen(anApplication,
                                                        aDocument,
                                                        theFormat,
                                                        "draw_ocaf_named_shape_transaction_bin",
                                                        theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  ExpectBoxBounds(ShapeOf(AttributeLabel(aRestored)), 10.0, 20.0, 30.0, 110.0, 220.0, 330.0);
}

static void RunB8(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_Name::Set(aLabel, TCollection_ExtendedString("Label1")).IsNull());
  const TDF_Label aChild = TDF_TagSource::NewChild(aLabel);
  ASSERT_FALSE(TDataStd_Name::Set(aChild, TCollection_ExtendedString("Label11")).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_tag_source_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDF_TagSource> aTagSource;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDF_TagSource::GetID(), aTagSource));
  EXPECT_EQ(aTagSource->NewTag(), 2);
  const TDF_Label aRestoredChild = AttributeLabel(aRestored).FindChild(1, false);
  ASSERT_FALSE(aRestoredChild.IsNull());
  occ::handle<TDataStd_Name> aChildName;
  ASSERT_TRUE(aRestoredChild.FindAttribute(TDataStd_Name::GetID(), aChildName));
  EXPECT_EQ(aChildName->Get(), TCollection_ExtendedString("Label11"));
}

static void RunB9(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_Directory::New(aLabel).IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aLabel, TCollection_ExtendedString("Label1")).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_directory_bin", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_Directory> aRestoredDirectory;
  ASSERT_TRUE(TDataStd_Directory::Find(AttributeLabel(aRestored), aRestoredDirectory));
  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), TCollection_ExtendedString("Label1"));
}

static void RunCafBugC1Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label aLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_Name::Set(aLabel, TCollection_ExtendedString("Label_1")).IsNull());
  ASSERT_FALSE(TDataStd_TreeNode::Set(aLabel).IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  occ::handle<TDataStd_TreeNode> aRestoredNode;
  ASSERT_TRUE(TDataStd_TreeNode::Find(aRestoredLabel, aRestoredNode));
  ASSERT_FALSE(aRestoredNode.IsNull());
  EXPECT_TRUE(aRestoredNode->IsRoot());
  ASSERT_FALSE(aRestoredNode->Root().IsNull());
  EXPECT_TRUE(aRestoredNode->Root()->Label() == aRestoredLabel);
  occ::handle<TDataStd_Name> aRestoredName;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_Name::GetID(), aRestoredName));
  EXPECT_EQ(aRestoredName->Get(), TCollection_ExtendedString("Label_1"));
}

static void RunCafBugC2Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label                      aLabel  = AttributeLabel(aDocument);
  occ::handle<TDataStd_ExtStringArray> anArray = TDataStd_ExtStringArray::Set(aLabel, 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, TCollection_ExtendedString("TDataStd"));
  anArray->SetValue(2, TCollection_ExtendedString("ExtStringArray"));

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ExtStringArray> aRestoredArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_ExtStringArray::GetID(), aRestoredArray));
  ASSERT_FALSE(aRestoredArray.IsNull());
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 2);
  EXPECT_EQ(aRestoredArray->Value(1), TCollection_ExtendedString("TDataStd"));
  EXPECT_EQ(aRestoredArray->Value(2), TCollection_ExtendedString("ExtStringArray"));
}

static void RestoreAfterForget(const occ::handle<TDocStd_Document>& theDocument,
                               const TDF_Label&                     theLabel)
{
  theDocument->NewCommand();
  theLabel.ForgetAllAttributes();
  theDocument->NewCommand();
  ASSERT_TRUE(theDocument->Undo());
  ASSERT_TRUE(theDocument->Redo());
  ASSERT_TRUE(theDocument->Undo());
}

static void ExpectCafExtendedArray(const occ::handle<TDataStd_ExtStringArray>& theArray,
                                   const TCollection_ExtendedString&           theFirst,
                                   const TCollection_ExtendedString&           theSecond)
{
  ASSERT_FALSE(theArray.IsNull());
  EXPECT_EQ(theArray->Lower(), 1);
  EXPECT_EQ(theArray->Upper(), 2);
  EXPECT_EQ(theArray->Length(), 2);
  EXPECT_EQ(theArray->Value(1), theFirst);
  EXPECT_EQ(theArray->Value(2), theSecond);
}

static void ExpectCafBooleanArray(const occ::handle<TDataStd_BooleanArray>& theArray,
                                  const bool                                theFirst,
                                  const bool                                theSecond)
{
  ASSERT_FALSE(theArray.IsNull());
  EXPECT_EQ(theArray->Lower(), 1);
  EXPECT_EQ(theArray->Upper(), 2);
  EXPECT_EQ(theArray->Length(), 2);
  EXPECT_EQ(theArray->Value(1), theFirst);
  EXPECT_EQ(theArray->Value(2), theSecond);
}

static void ExpectCafByteArray(const occ::handle<TDataStd_ByteArray>& theArray,
                               const int                              theFirst,
                               const int                              theSecond)
{
  ASSERT_FALSE(theArray.IsNull());
  EXPECT_EQ(theArray->Lower(), 1);
  EXPECT_EQ(theArray->Upper(), 2);
  EXPECT_EQ(theArray->Length(), 2);
  EXPECT_EQ(static_cast<int>(theArray->Value(1)), theFirst);
  EXPECT_EQ(static_cast<int>(theArray->Value(2)), theSecond);
}

static void RunCafBasicM3Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                      aLabel  = AttributeLabel(aDocument);
  occ::handle<TDataStd_ExtStringArray> anArray = TDataStd_ExtStringArray::Set(aLabel, 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, TCollection_ExtendedString("TDataStd"));
  anArray->SetValue(2, TCollection_ExtendedString("ExtStringArray"));

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ExtStringArray> aRestoredArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_ExtStringArray::GetID(), aRestoredArray));
  ExpectCafExtendedArray(aRestoredArray,
                         TCollection_ExtendedString("TDataStd"),
                         TCollection_ExtendedString("ExtStringArray"));
}

static void RunCafBasicM6Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                      aLabel = AttributeLabel(aDocument);
  const Standard_GUID                  aGuid("12e94515-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_ExtStringArray> anArray = TDataStd_ExtStringArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_ExtStringArray> aGuidArray =
    TDataStd_ExtStringArray::Set(aLabel, aGuid, 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  ASSERT_FALSE(aGuidArray.IsNull());
  anArray->SetValue(1, TCollection_ExtendedString("xxxxxxxx"));
  anArray->SetValue(2, TCollection_ExtendedString("yyyyyyy"));
  aGuidArray->SetValue(1, TCollection_ExtendedString("xxxxxxxx"));
  aGuidArray->SetValue(2, TCollection_ExtendedString("yyyyyyy"));
  RestoreAfterForget(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ExtStringArray> aRestoredArray;
  occ::handle<TDataStd_ExtStringArray> aRestoredGuidArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_ExtStringArray::GetID(), aRestoredArray));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidArray));
  ExpectCafExtendedArray(aRestoredArray,
                         TCollection_ExtendedString("xxxxxxxx"),
                         TCollection_ExtendedString("yyyyyyy"));
  ExpectCafExtendedArray(aRestoredGuidArray,
                         TCollection_ExtendedString("xxxxxxxx"),
                         TCollection_ExtendedString("yyyyyyy"));
}

static void RunCafBasicN6Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  const Standard_GUID aGuid1("12e94571-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID aGuid2("12e94572-6dbc-11d4-b9c8-0060b0ee281b");
  ASSERT_FALSE(TDataStd_AsciiString::Set(aLabel, aGuid1, "New Attribute_1").IsNull());
  ASSERT_FALSE(TDataStd_AsciiString::Set(aLabel, aGuid2, "New Attribute_2").IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_AsciiString> anAttribute1;
  occ::handle<TDataStd_AsciiString> anAttribute2;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid1, anAttribute1));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid2, anAttribute2));
  EXPECT_EQ(anAttribute1->Get(), TCollection_AsciiString("New Attribute_1"));
  EXPECT_EQ(anAttribute2->Get(), TCollection_AsciiString("New Attribute_2"));
}

static void RunCafBasicN7Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  const Standard_GUID aGuid("12e94572-6dbc-11d4-b9c8-0060b0ee281b");
  ASSERT_FALSE(TDataStd_AsciiString::Set(aLabel, "New Attribute_1").IsNull());
  ASSERT_FALSE(TDataStd_AsciiString::Set(aLabel, aGuid, "New Attribute_2").IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_AsciiString> aDefaultAttribute;
  occ::handle<TDataStd_AsciiString> aGuidAttribute;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_AsciiString::GetID(), aDefaultAttribute));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aGuidAttribute));
  EXPECT_EQ(aDefaultAttribute->Get(), TCollection_AsciiString("New Attribute_1"));
  EXPECT_EQ(aGuidAttribute->Get(), TCollection_AsciiString("New Attribute_2"));
}

static void RunCafBasicO6Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                    aLabel = AttributeLabel(aDocument);
  const Standard_GUID                aGuid("12e94516-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_BooleanArray> anArray    = TDataStd_BooleanArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_BooleanArray> aGuidArray = TDataStd_BooleanArray::Set(aLabel, aGuid, 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  ASSERT_FALSE(aGuidArray.IsNull());
  anArray->SetValue(1, false);
  anArray->SetValue(2, true);
  aGuidArray->SetValue(1, false);
  aGuidArray->SetValue(2, true);
  RestoreAfterForget(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_BooleanArray> aRestoredArray;
  occ::handle<TDataStd_BooleanArray> aRestoredGuidArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_BooleanArray::GetID(), aRestoredArray));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidArray));
  ExpectCafBooleanArray(aRestoredArray, false, true);
  ExpectCafBooleanArray(aRestoredGuidArray, false, true);
}

static void RunCafBasicP6Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                 aLabel = AttributeLabel(aDocument);
  const Standard_GUID             aGuid("12e94517-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_ByteArray> anArray    = TDataStd_ByteArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_ByteArray> aGuidArray = TDataStd_ByteArray::Set(aLabel, aGuid, 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  ASSERT_FALSE(aGuidArray.IsNull());
  anArray->SetValue(1, 10);
  anArray->SetValue(2, 12);
  aGuidArray->SetValue(1, 10);
  aGuidArray->SetValue(2, 12);
  RestoreAfterForget(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ByteArray> aRestoredArray;
  occ::handle<TDataStd_ByteArray> aRestoredGuidArray;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_ByteArray::GetID(), aRestoredArray));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidArray));
  ExpectCafByteArray(aRestoredArray, 10, 12);
  ExpectCafByteArray(aRestoredGuidArray, 10, 12);
}

static void RunCafBasicQ6Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                      aLabel   = AttributeLabel(aDocument);
  const TDF_Label                      aTarget1 = aDocument->Main().FindChild(3, true);
  const TDF_Label                      aTarget2 = aDocument->Main().FindChild(4, true);
  const Standard_GUID                  aGuid("12e94518-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_ReferenceArray> anArray = TDataStd_ReferenceArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_ReferenceArray> aGuidArray =
    TDataStd_ReferenceArray::Set(aLabel, aGuid, 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  ASSERT_FALSE(aGuidArray.IsNull());
  anArray->SetValue(1, aTarget1);
  anArray->SetValue(2, aTarget2);
  aGuidArray->SetValue(1, aTarget1);
  aGuidArray->SetValue(2, aTarget2);
  RestoreAfterForget(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel   = AttributeLabel(aRestored);
  const TDF_Label aRestoredTarget1 = aRestored->Main().FindChild(3, false);
  const TDF_Label aRestoredTarget2 = aRestored->Main().FindChild(4, false);
  ASSERT_FALSE(aRestoredTarget1.IsNull());
  ASSERT_FALSE(aRestoredTarget2.IsNull());
  occ::handle<TDataStd_ReferenceArray> aRestoredArray;
  occ::handle<TDataStd_ReferenceArray> aRestoredGuidArray;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_ReferenceArray::GetID(), aRestoredArray));
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid, aRestoredGuidArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 2);
  EXPECT_EQ(aRestoredArray->Value(1), aRestoredTarget1);
  EXPECT_EQ(aRestoredArray->Value(2), aRestoredTarget2);
  EXPECT_EQ(aRestoredGuidArray->Value(1), aRestoredTarget1);
  EXPECT_EQ(aRestoredGuidArray->Value(2), aRestoredTarget2);
}

static void RunCafBasicR6Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                aLabel = AttributeLabel(aDocument);
  const Standard_GUID            aGuid("12e94521-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_RealList> aList     = TDataStd_RealList::Set(aLabel);
  occ::handle<TDataStd_RealList> aGuidList = TDataStd_RealList::Set(aLabel, aGuid);
  ASSERT_FALSE(aList.IsNull());
  ASSERT_FALSE(aGuidList.IsNull());
  aList->Append(3.0);
  aList->Append(4.0);
  aGuidList->Append(3.0);
  aGuidList->Append(4.0);
  RestoreAfterForget(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_RealList> aRestoredList;
  occ::handle<TDataStd_RealList> aRestoredGuidList;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_RealList::GetID(), aRestoredList));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidList));
  EXPECT_EQ(aRestoredList->Extent(), 2);
  EXPECT_DOUBLE_EQ(aRestoredList->First(), 3.0);
  EXPECT_DOUBLE_EQ(aRestoredList->Last(), 4.0);
  EXPECT_EQ(aRestoredGuidList->Extent(), 2);
  EXPECT_DOUBLE_EQ(aRestoredGuidList->First(), 3.0);
  EXPECT_DOUBLE_EQ(aRestoredGuidList->Last(), 4.0);
}

static void RunCafBasicS6Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                   aLabel = AttributeLabel(aDocument);
  const Standard_GUID               aGuid("12e94531-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_IntegerList> aList     = TDataStd_IntegerList::Set(aLabel);
  occ::handle<TDataStd_IntegerList> aGuidList = TDataStd_IntegerList::Set(aLabel, aGuid);
  ASSERT_FALSE(aList.IsNull());
  ASSERT_FALSE(aGuidList.IsNull());
  aList->Append(33);
  aList->Append(44);
  aGuidList->Append(33);
  aGuidList->Append(44);
  RestoreAfterForget(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_IntegerList> aRestoredList;
  occ::handle<TDataStd_IntegerList> aRestoredGuidList;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_IntegerList::GetID(), aRestoredList));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidList));
  EXPECT_EQ(aRestoredList->Extent(), 2);
  EXPECT_EQ(aRestoredList->First(), 33);
  EXPECT_EQ(aRestoredList->Last(), 44);
  EXPECT_EQ(aRestoredGuidList->Extent(), 2);
  EXPECT_EQ(aRestoredGuidList->First(), 33);
  EXPECT_EQ(aRestoredGuidList->Last(), 44);
}

static void RunCafBasicT6Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                     aLabel = AttributeLabel(aDocument);
  const Standard_GUID                 aGuid("12e94541-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_ExtStringList> aList     = TDataStd_ExtStringList::Set(aLabel);
  occ::handle<TDataStd_ExtStringList> aGuidList = TDataStd_ExtStringList::Set(aLabel, aGuid);
  ASSERT_FALSE(aList.IsNull());
  ASSERT_FALSE(aGuidList.IsNull());
  aList->Append(TCollection_ExtendedString("xxxx"));
  aList->Append(TCollection_ExtendedString("yyyyy"));
  aGuidList->Append(TCollection_ExtendedString("xxxx"));
  aGuidList->Append(TCollection_ExtendedString("yyyyy"));
  RestoreAfterForget(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ExtStringList> aRestoredList;
  occ::handle<TDataStd_ExtStringList> aRestoredGuidList;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_ExtStringList::GetID(), aRestoredList));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidList));
  EXPECT_EQ(aRestoredList->Extent(), 2);
  EXPECT_EQ(aRestoredList->First(), TCollection_ExtendedString("xxxx"));
  EXPECT_EQ(aRestoredList->Last(), TCollection_ExtendedString("yyyyy"));
  EXPECT_EQ(aRestoredGuidList->Extent(), 2);
  EXPECT_EQ(aRestoredGuidList->First(), TCollection_ExtendedString("xxxx"));
  EXPECT_EQ(aRestoredGuidList->Last(), TCollection_ExtendedString("yyyyy"));
}

static void RunCafBasicU6Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                   aLabel = AttributeLabel(aDocument);
  const Standard_GUID               aGuid("12e94551-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_BooleanList> aList     = TDataStd_BooleanList::Set(aLabel);
  occ::handle<TDataStd_BooleanList> aGuidList = TDataStd_BooleanList::Set(aLabel, aGuid);
  ASSERT_FALSE(aList.IsNull());
  ASSERT_FALSE(aGuidList.IsNull());
  aList->Append(false);
  aList->Append(true);
  aGuidList->Append(false);
  aGuidList->Append(true);
  RestoreAfterForget(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_BooleanList> aRestoredList;
  occ::handle<TDataStd_BooleanList> aRestoredGuidList;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_BooleanList::GetID(), aRestoredList));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidList));
  EXPECT_EQ(aRestoredList->Extent(), 2);
  EXPECT_EQ(aRestoredList->First(), false);
  EXPECT_EQ(aRestoredList->Last(), true);
  EXPECT_EQ(aRestoredGuidList->Extent(), 2);
  EXPECT_EQ(aRestoredGuidList->First(), false);
  EXPECT_EQ(aRestoredGuidList->Last(), true);
}

static void RunCafBasicV6Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                     aLabel   = AttributeLabel(aDocument);
  const TDF_Label                     aTarget1 = aDocument->Main().FindChild(3, true);
  const TDF_Label                     aTarget2 = aDocument->Main().FindChild(4, true);
  const Standard_GUID                 aGuid("12e94561-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_ReferenceList> aList     = TDataStd_ReferenceList::Set(aLabel);
  occ::handle<TDataStd_ReferenceList> aGuidList = TDataStd_ReferenceList::Set(aLabel, aGuid);
  ASSERT_FALSE(aList.IsNull());
  ASSERT_FALSE(aGuidList.IsNull());
  aList->Append(aTarget1);
  aList->Append(aTarget2);
  aGuidList->Append(aTarget1);
  aGuidList->Append(aTarget2);
  RestoreAfterForget(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredTarget1 = aRestored->Main().FindChild(3, false);
  const TDF_Label aRestoredTarget2 = aRestored->Main().FindChild(4, false);
  ASSERT_FALSE(aRestoredTarget1.IsNull());
  ASSERT_FALSE(aRestoredTarget2.IsNull());
  occ::handle<TDataStd_ReferenceList> aRestoredList;
  occ::handle<TDataStd_ReferenceList> aRestoredGuidList;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_ReferenceList::GetID(), aRestoredList));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidList));
  EXPECT_EQ(aRestoredList->Extent(), 2);
  EXPECT_EQ(aRestoredList->First(), aRestoredTarget1);
  EXPECT_EQ(aRestoredList->Last(), aRestoredTarget2);
  EXPECT_EQ(aRestoredGuidList->Extent(), 2);
  EXPECT_EQ(aRestoredGuidList->First(), aRestoredTarget1);
  EXPECT_EQ(aRestoredGuidList->Last(), aRestoredTarget2);
}

static occ::handle<Poly_Triangulation> MakeCafTriangulation(const bool theDense)
{
  if (!theDense)
  {
    NCollection_Array1<gp_Pnt> aNodes(1, 4);
    aNodes.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
    aNodes.SetValue(2, gp_Pnt(10.0, 0.0, 0.0));
    aNodes.SetValue(3, gp_Pnt(10.0, 10.0, 0.0));
    aNodes.SetValue(4, gp_Pnt(0.0, 10.0, 0.0));
    NCollection_Array1<Poly_Triangle> aTriangles(1, 2);
    aTriangles.SetValue(1, Poly_Triangle(1, 2, 3));
    aTriangles.SetValue(2, Poly_Triangle(1, 3, 4));
    occ::handle<Poly_Triangulation> aResult = new Poly_Triangulation(aNodes, aTriangles);
    aResult->Deflection(10.0);
    return aResult;
  }

  NCollection_Array1<gp_Pnt> aNodes(1, 5);
  aNodes.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aNodes.SetValue(2, gp_Pnt(10.0, 0.0, 0.0));
  aNodes.SetValue(3, gp_Pnt(10.0, 10.0, 0.0));
  aNodes.SetValue(4, gp_Pnt(0.0, 10.0, 0.0));
  aNodes.SetValue(5, gp_Pnt(5.0, 5.0, 0.0));
  NCollection_Array1<Poly_Triangle> aTriangles(1, 4);
  aTriangles.SetValue(1, Poly_Triangle(1, 2, 5));
  aTriangles.SetValue(2, Poly_Triangle(2, 3, 5));
  aTriangles.SetValue(3, Poly_Triangle(3, 4, 5));
  aTriangles.SetValue(4, Poly_Triangle(4, 1, 5));
  occ::handle<Poly_Triangulation> aResult = new Poly_Triangulation(aNodes, aTriangles);
  aResult->Deflection(1.0);
  return aResult;
}

static void ExpectCafTriangulation(const TDF_Label&                       theLabel,
                                   const occ::handle<Poly_Triangulation>& theExpected)
{
  occ::handle<TDataXtd_Triangulation> anAttribute;
  ASSERT_TRUE(theLabel.FindAttribute(TDataXtd_Triangulation::GetID(), anAttribute));
  ASSERT_FALSE(anAttribute.IsNull());
  ASSERT_FALSE(anAttribute->Get().IsNull());
  EXPECT_DOUBLE_EQ(anAttribute->Deflection(), theExpected->Deflection());
  EXPECT_EQ(anAttribute->NbNodes(), theExpected->NbNodes());
  EXPECT_EQ(anAttribute->NbTriangles(), theExpected->NbTriangles());
  EXPECT_EQ(anAttribute->HasUVNodes(), theExpected->HasUVNodes());
  EXPECT_EQ(anAttribute->HasNormals(), theExpected->HasNormals());
  for (int anIndex = 1; anIndex <= theExpected->NbNodes(); ++anIndex)
  {
    EXPECT_TRUE(
      anAttribute->Node(anIndex).IsEqual(theExpected->Node(anIndex), Precision::Confusion()));
  }
  for (int anIndex = 1; anIndex <= theExpected->NbTriangles(); ++anIndex)
  {
    int anExpectedNode1 = 0;
    int anExpectedNode2 = 0;
    int anExpectedNode3 = 0;
    int anActualNode1   = 0;
    int anActualNode2   = 0;
    int anActualNode3   = 0;
    theExpected->Triangle(anIndex).Get(anExpectedNode1, anExpectedNode2, anExpectedNode3);
    anAttribute->Triangle(anIndex).Get(anActualNode1, anActualNode2, anActualNode3);
    EXPECT_EQ(anActualNode1, anExpectedNode1);
    EXPECT_EQ(anActualNode2, anExpectedNode2);
    EXPECT_EQ(anActualNode3, anExpectedNode3);
  }
}

static void RunCafBasicN1Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                       aLabel      = aDocument->Main().FindChild(1, true);
  const occ::handle<Poly_Triangulation> aFirstMesh  = MakeCafTriangulation(false);
  const occ::handle<Poly_Triangulation> aSecondMesh = MakeCafTriangulation(true);

  aDocument->NewCommand();
  ASSERT_FALSE(TDataXtd_Triangulation::Set(aLabel, aFirstMesh).IsNull());
  aDocument->CommitCommand();
  std::stringstream aFirstStream;
  SaveDocumentStream(anApplication, aDocument, aFirstStream);

  aDocument->NewCommand();
  ASSERT_FALSE(TDataXtd_Triangulation::Set(aLabel, aSecondMesh).IsNull());
  aDocument->CommitCommand();
  std::stringstream aSecondStream;
  SaveDocumentStream(anApplication, aDocument, aSecondStream);

  ASSERT_TRUE(aDocument->Undo());
  ExpectCafTriangulation(aLabel, aFirstMesh);
  ASSERT_TRUE(aDocument->Redo());
  ExpectCafTriangulation(aLabel, aSecondMesh);

  anApplication->Close(aDocument);
  occ::handle<TDocStd_Document> aRestoredFirst  = OpenDocumentStream(anApplication, aFirstStream);
  occ::handle<TDocStd_Document> aRestoredSecond = OpenDocumentStream(anApplication, aSecondStream);
  ASSERT_FALSE(aRestoredFirst.IsNull());
  ASSERT_FALSE(aRestoredSecond.IsNull());
  ExpectCafTriangulation(aRestoredFirst->Main().FindChild(1, false), aFirstMesh);
  ExpectCafTriangulation(aRestoredSecond->Main().FindChild(1, false), aSecondMesh);
}

static void RunCafBug24164_2Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aRoot   = aDocument->Main().FindChild(1, true);
  const TDF_Label aLabel1 = aRoot.FindChild(1, true);
  const TDF_Label aLabel2 = aRoot.FindChild(2, true);
  const TDF_Label aLabel3 = aRoot.FindChild(3, true);
  const TDF_Label aLabel4 = aRoot.FindChild(4, true);
  const TDF_Label aLabel5 = aRoot.FindChild(5, true);
  const TDF_Label aLabel6 = aRoot.FindChild(6, true);

  occ::handle<TDataStd_ReferenceArray> anArray1 = TDataStd_ReferenceArray::Set(aLabel1, 1, 2);
  occ::handle<TDataStd_ReferenceArray> anArray2 = TDataStd_ReferenceArray::Set(aLabel2, 1, 2);
  occ::handle<TDataStd_ReferenceArray> anArray3 = TDataStd_ReferenceArray::Set(aLabel3, 1, 1);
  occ::handle<TDataStd_ReferenceArray> anArray4 = TDataStd_ReferenceArray::Set(aLabel4, 1, 2);
  occ::handle<TDataStd_ReferenceArray> anArray5 = TDataStd_ReferenceArray::Set(aLabel5, 1, 1);
  occ::handle<TDataStd_ReferenceArray> anArray6 = TDataStd_ReferenceArray::Set(aLabel6, 1, 2);
  ASSERT_FALSE(anArray1.IsNull());
  ASSERT_FALSE(anArray2.IsNull());
  ASSERT_FALSE(anArray3.IsNull());
  ASSERT_FALSE(anArray4.IsNull());
  ASSERT_FALSE(anArray5.IsNull());
  ASSERT_FALSE(anArray6.IsNull());
  anArray1->SetValue(1, aLabel2);
  anArray1->SetValue(2, aLabel1);
  anArray2->SetValue(1, aLabel3);
  anArray2->SetValue(2, aLabel4);
  anArray3->SetValue(1, aLabel1);
  anArray4->SetValue(1, aLabel5);
  anArray4->SetValue(2, aLabel1);
  anArray5->SetValue(1, aLabel6);
  anArray6->SetValue(1, aLabel3);
  anArray6->SetValue(2, aLabel4);

  aDocument->NewCommand();
  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredRoot = aRestored->Main().FindChild(1, false);
  ASSERT_FALSE(aRestoredRoot.IsNull());
  const TDF_Label aRestoredLabel1 = aRestoredRoot.FindChild(1, false);
  const TDF_Label aRestoredLabel2 = aRestoredRoot.FindChild(2, false);
  const TDF_Label aRestoredLabel3 = aRestoredRoot.FindChild(3, false);
  const TDF_Label aRestoredLabel4 = aRestoredRoot.FindChild(4, false);
  const TDF_Label aRestoredLabel5 = aRestoredRoot.FindChild(5, false);
  const TDF_Label aRestoredLabel6 = aRestoredRoot.FindChild(6, false);
  ASSERT_FALSE(aRestoredLabel1.IsNull());
  ASSERT_FALSE(aRestoredLabel2.IsNull());
  ASSERT_FALSE(aRestoredLabel3.IsNull());
  ASSERT_FALSE(aRestoredLabel4.IsNull());
  ASSERT_FALSE(aRestoredLabel5.IsNull());
  ASSERT_FALSE(aRestoredLabel6.IsNull());

  occ::handle<TDataStd_ReferenceArray> aRestoredArray;
  ASSERT_TRUE(aRestoredLabel1.FindAttribute(TDataStd_ReferenceArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 2);
  EXPECT_EQ(aRestoredArray->Value(1), aRestoredLabel2);
  EXPECT_EQ(aRestoredArray->Value(2), aRestoredLabel1);
  ASSERT_TRUE(aRestoredLabel2.FindAttribute(TDataStd_ReferenceArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Value(1), aRestoredLabel3);
  EXPECT_EQ(aRestoredArray->Value(2), aRestoredLabel4);
  ASSERT_TRUE(aRestoredLabel3.FindAttribute(TDataStd_ReferenceArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Upper(), 1);
  EXPECT_EQ(aRestoredArray->Value(1), aRestoredLabel1);
  ASSERT_TRUE(aRestoredLabel4.FindAttribute(TDataStd_ReferenceArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Value(1), aRestoredLabel5);
  EXPECT_EQ(aRestoredArray->Value(2), aRestoredLabel1);
  ASSERT_TRUE(aRestoredLabel5.FindAttribute(TDataStd_ReferenceArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Upper(), 1);
  EXPECT_EQ(aRestoredArray->Value(1), aRestoredLabel6);
  ASSERT_TRUE(aRestoredLabel6.FindAttribute(TDataStd_ReferenceArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Value(1), aRestoredLabel3);
  EXPECT_EQ(aRestoredArray->Value(2), aRestoredLabel4);
}

static void RunCafBug24164_1Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  occ::handle<TFunction_DriverTable> aDriverTable = TFunction_DriverTable::Get();
  aDriverTable->AddDriver(BOX_GUID, new DNaming_BoxDriver());

  const occ::handle<TDataStd_UAttribute> anObject = AddNamingObject(aDocument);
  ASSERT_FALSE(anObject.IsNull());
  const occ::handle<TFunction_Function> aFunction = AddNamingFunction(anObject, BOX_GUID);
  ASSERT_FALSE(aFunction.IsNull());
  DNaming::GetReal(aFunction, BOX_DX)->Set(190.0);
  DNaming::GetReal(aFunction, BOX_DY)->Set(290.0);
  DNaming::GetReal(aFunction, BOX_DZ)->Set(390.0);
  ASSERT_EQ(ExecuteNamingFunction(aFunction, new DNaming_BoxDriver()), 0);
  ExpectBoxBounds(NamingFunctionShape(aFunction), 0.0, 0.0, 0.0, 190.0, 290.0, 390.0);

  TCollection_AsciiString aFunctionEntry;
  TDF_Tool::Entry(aFunction->Label(), aFunctionEntry);
  std::stringstream aStream;
  SaveDocumentStream(anApplication, aDocument, aStream);

  EXPECT_NO_THROW(anApplication->Close(aDocument));
  occ::handle<TDocStd_Document> aRestored = OpenDocumentStream(anApplication, aStream);
  ASSERT_FALSE(aRestored.IsNull());

  const TDF_Label aRestoredFunctionLabel = LabelByEntry(aRestored, aFunctionEntry);
  ASSERT_FALSE(aRestoredFunctionLabel.IsNull());
  occ::handle<TFunction_Function> aRestoredFunction;
  ASSERT_TRUE(aRestoredFunctionLabel.FindAttribute(TFunction_Function::GetID(), aRestoredFunction));
  EXPECT_EQ(aRestoredFunction->GetDriverGUID(), BOX_GUID);
  EXPECT_EQ(aRestoredFunction->GetFailure(), DONE);
  ExpectBoxBounds(NamingFunctionShape(aRestoredFunction), 0.0, 0.0, 0.0, 190.0, 290.0, 390.0);

  const TDF_Label aRestoredResultLabel =
    aRestoredFunctionLabel.FindChild(FUNCTION_RESULT_LABEL, false);
  ASSERT_FALSE(aRestoredResultLabel.IsNull());
  EXPECT_EQ(TopExp_Explorer(NamingFunctionShape(aRestoredFunction), TopAbs_FACE).More(), true);
  EXPECT_NO_THROW(anApplication->Close(aRestored));
}

static void RunC1(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aRootLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_Name::Set(aRootLabel, TCollection_ExtendedString("Label_1")).IsNull());
  occ::handle<TDataStd_TreeNode> aRootNode    = TDataStd_TreeNode::Set(aRootLabel);
  const TDF_Label                aChildLabel1 = aDocument->Main().FindChild(7, true);
  const TDF_Label                aChildLabel2 = aDocument->Main().FindChild(8, true);
  const TDF_Label                aChildLabel3 = aDocument->Main().FindChild(9, true);
  occ::handle<TDataStd_TreeNode> aChildNode1  = TDataStd_TreeNode::Set(aChildLabel1);
  occ::handle<TDataStd_TreeNode> aChildNode2  = TDataStd_TreeNode::Set(aChildLabel2);
  occ::handle<TDataStd_TreeNode> aChildNode3  = TDataStd_TreeNode::Set(aChildLabel3);
  ASSERT_TRUE(aRootNode->Append(aChildNode1));
  ASSERT_TRUE(aRootNode->Append(aChildNode2));
  ASSERT_TRUE(aRootNode->Append(aChildNode3));

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_tree_nodes_bin");
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_TreeNode> aRestoredRoot = TDataStd_TreeNode::Set(AttributeLabel(aRestored));
  ASSERT_FALSE(aRestoredRoot.IsNull());
  EXPECT_EQ(aRestoredRoot->NbChildren(false), 3);
  const int                  aExpectedTags[] = {7, 8, 9};
  TDataStd_ChildNodeIterator anIterator(aRestoredRoot, false);
  for (int anIndex = 0; anIterator.More() && anIndex < 3; anIterator.Next(), ++anIndex)
  {
    EXPECT_EQ(anIterator.Value()->Label().Tag(), aExpectedTags[anIndex]);
  }
  EXPECT_FALSE(anIterator.More());
}

static void RunC6(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const Standard_GUID aDriverGuid("5b35ca00-5b78-11d1-8940-080009dc3333");
  const TDF_Label     aLabel = aDocument->GetData()->Root().FindChild(1, true).FindChild(1, true);
  occ::handle<TFunction_Function> aFunction = TFunction_Function::Set(aLabel, aDriverGuid);
  ASSERT_FALSE(aFunction.IsNull());
  aFunction->SetFailure(13);
  TCollection_AsciiString aLabelEntry;
  TDF_Tool::Entry(aLabel, aLabelEntry);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_function_bin");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label                 aRestoredLabel = LabelByEntry(aRestored, aLabelEntry);
  occ::handle<TFunction_Function> aRestoredFunction;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TFunction_Function::GetID(), aRestoredFunction));
  EXPECT_EQ(aRestoredFunction->GetDriverGUID(), aDriverGuid);
  EXPECT_EQ(aRestoredFunction->GetFailure(), 13);
}

static void RunD1(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const char* const aNames[] = {"Tu sais je n'ai jamais t aussi hereux que ce matin-l",
                                "\"Tu sais je n'ai jamais t aussi hereux que ce matin-l\"",
                                "<Tu sais je n'ai jamais t aussi hereux que ce matin-l>",
                                "Tu m'as dit \"J'ai rendez-vous dans un sous-sol avec des fous",
                                "\"Tu m'as dit \"J'ai rendez-vous dans un sous-sol avec des fous\"",
                                "Il <n'avait plus rien cr> dans ce monde triste"};
  for (int anIndex = 0; anIndex < 6; ++anIndex)
  {
    const TDF_Label aLabel = aDocument->Main().FindChild(11 + anIndex, true);
    ASSERT_FALSE(TDataStd_Name::Set(aLabel, TCollection_ExtendedString(aNames[anIndex])).IsNull());
  }

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_special_names_bin");
  ASSERT_FALSE(aRestored.IsNull());
  for (int anIndex = 0; anIndex < 6; ++anIndex)
  {
    const TDF_Label            aLabel = aRestored->Main().FindChild(11 + anIndex, false);
    occ::handle<TDataStd_Name> aName;
    ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Name::GetID(), aName));
    EXPECT_EQ(aName->Get(), TCollection_ExtendedString(aNames[anIndex]));
  }
}

static void RunD4(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<TDataStd_IntegerArray> anArray =
    TDataStd_IntegerArray::Set(AttributeLabel(aDocument), 1, 1);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, 3);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_single_integer_array_bin");
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_IntegerArray> aRestoredArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_IntegerArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 1);
  EXPECT_EQ(aRestoredArray->Value(1), 3);
}

static void RunD5(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<TDataStd_RealArray> anArray =
    TDataStd_RealArray::Set(AttributeLabel(aDocument), 1, 1);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, 3.0);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_single_real_array_bin");
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_RealArray> aRestoredArray;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_RealArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 1);
  EXPECT_DOUBLE_EQ(aRestoredArray->Value(1), 3.0);
}

static void RunD6(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(
    TDataStd_Name::Set(AttributeLabel(aDocument), TCollection_ExtendedString("00")).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_leading_zero_name_bin");
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), TCollection_ExtendedString("00"));
}

static void RunBasicIntegerUserGUID(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  const Standard_GUID aGuid1("12e94541-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID aGuid2("12e94542-6dbc-11d4-b9c8-0060b0ee281b");
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel, aGuid1, 100).IsNull());
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel, aGuid2, 200).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_caf_basic_integer_user_guids");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  occ::handle<TDataStd_Integer> anInteger1;
  occ::handle<TDataStd_Integer> anInteger2;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid1, anInteger1));
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid2, anInteger2));
  EXPECT_EQ(anInteger1->Get(), 100);
  EXPECT_EQ(anInteger2->Get(), 200);
}

static void RunBasicIntegerDefaultAndUserGUID(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  const Standard_GUID aGuid("12e94541-6dbc-11d4-b9c8-0060b0ee281b");
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel, 100).IsNull());
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel, aGuid, 200).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_caf_basic_integer_default_and_guid");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  occ::handle<TDataStd_Integer> anInteger;
  occ::handle<TDataStd_Integer> aGuidInteger;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid, aGuidInteger));
  EXPECT_EQ(anInteger->Get(), 100);
  EXPECT_EQ(aGuidInteger->Get(), 200);
}

static void RunBug28425(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label aLabel = aDocument->Main().FindChild(1, true);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(BRepBuilderAPI_MakeVertex(gp_Pnt(1.0, 2.0, 3.0)).Shape());
  aBuilder.Generated(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());

  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  TNaming_Iterator anIterator(aNamedShape);
  ASSERT_TRUE(anIterator.More());
  EXPECT_EQ(anIterator.NewShape().ShapeType(), TopAbs_SOLID);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(1, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  TNaming_Iterator aRestoredIterator(aNamedShape);
  ASSERT_TRUE(aRestoredIterator.More());
  EXPECT_EQ(aRestoredIterator.NewShape().ShapeType(), TopAbs_SOLID);
}

static void RunBug29669(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label     aLabel = aDocument->Main().FindChild(1, true);
  const Standard_GUID aGuid1("12e94541-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID aGuid2("12e94541-6dbc-11d4-b9c8-0060b0ee281d");
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel, aGuid1, 123).IsNull());
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel, 100).IsNull());
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel, aGuid2, 321).IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(1, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());

  occ::handle<TDataStd_Integer> anInteger1;
  occ::handle<TDataStd_Integer> anInteger2;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid1, anInteger1));
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid2, anInteger2));
  EXPECT_EQ(anInteger1->Get(), 123);
  EXPECT_EQ(anInteger2->Get(), 321);
}

static void RunBasicRealUserGUID(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  const Standard_GUID aGuid1("12e94551-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID aGuid2("12e94552-6dbc-11d4-b9c8-0060b0ee281b");
  ASSERT_FALSE(TDataStd_Real::Set(aLabel, aGuid1, 100.11).IsNull());
  ASSERT_FALSE(TDataStd_Real::Set(aLabel, aGuid2, 200.11).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_caf_basic_real_user_guids");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  occ::handle<TDataStd_Real> aReal1;
  occ::handle<TDataStd_Real> aReal2;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid1, aReal1));
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid2, aReal2));
  EXPECT_DOUBLE_EQ(aReal1->Get(), 100.11);
  EXPECT_DOUBLE_EQ(aReal2->Get(), 200.11);
}

static void RunBasicRealDefaultAndUserGUID(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  const Standard_GUID aGuid("12e94552-6dbc-11d4-b9c8-0060b0ee281b");
  ASSERT_FALSE(TDataStd_Real::Set(aLabel, 100.11).IsNull());
  ASSERT_FALSE(TDataStd_Real::Set(aLabel, aGuid, 200.11).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_caf_basic_real_default_and_guid");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  occ::handle<TDataStd_Real> aReal;
  occ::handle<TDataStd_Real> aGuidReal;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid, aGuidReal));
  EXPECT_DOUBLE_EQ(aReal->Get(), 100.11);
  EXPECT_DOUBLE_EQ(aGuidReal->Get(), 200.11);
}

static void RestoreAttributesAfterUndo(const occ::handle<TDocStd_Document>& theDocument,
                                       const TDF_Label&                     theLabel)
{
  theDocument->NewCommand();
  theLabel.ForgetAllAttributes();
  theDocument->NewCommand();
  ASSERT_TRUE(theDocument->Undo());
  ASSERT_TRUE(theDocument->Redo());
  ASSERT_TRUE(theDocument->Undo());
}

static void RunBasicRealArrayDefaultAndUserGUID(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  const Standard_GUID aGuid("12e94511-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_RealArray> anArray    = TDataStd_RealArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_RealArray> aGuidArray = TDataStd_RealArray::Set(aLabel, aGuid, 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  ASSERT_FALSE(aGuidArray.IsNull());
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);
  aGuidArray->SetValue(1, 3.0);
  aGuidArray->SetValue(2, 4.0);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_caf_basic_real_array_user_guid");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  occ::handle<TDataStd_RealArray> aRestoredArray;
  occ::handle<TDataStd_RealArray> aRestoredGuidArray;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_RealArray::GetID(), aRestoredArray));
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid, aRestoredGuidArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 2);
  EXPECT_DOUBLE_EQ(aRestoredArray->Value(1), 3.0);
  EXPECT_DOUBLE_EQ(aRestoredArray->Value(2), 4.0);
  EXPECT_EQ(aRestoredGuidArray->Lower(), 1);
  EXPECT_EQ(aRestoredGuidArray->Upper(), 2);
  EXPECT_DOUBLE_EQ(aRestoredGuidArray->Value(1), 3.0);
  EXPECT_DOUBLE_EQ(aRestoredGuidArray->Value(2), 4.0);
  EXPECT_EQ(aRestoredGuidArray->ID(), aGuid);
}

static void RunBasicIntegerArrayDefaultAndUserGUID(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  const Standard_GUID aGuid("12e94511-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_IntegerArray> anArray    = TDataStd_IntegerArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_IntegerArray> aGuidArray = TDataStd_IntegerArray::Set(aLabel, aGuid, 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  ASSERT_FALSE(aGuidArray.IsNull());
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);
  aGuidArray->SetValue(1, 3);
  aGuidArray->SetValue(2, 4);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_caf_basic_integer_array_user_guid");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  occ::handle<TDataStd_IntegerArray> aRestoredArray;
  occ::handle<TDataStd_IntegerArray> aRestoredGuidArray;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_IntegerArray::GetID(), aRestoredArray));
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid, aRestoredGuidArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 2);
  EXPECT_EQ(aRestoredArray->Value(1), 3);
  EXPECT_EQ(aRestoredArray->Value(2), 4);
  EXPECT_EQ(aRestoredGuidArray->Lower(), 1);
  EXPECT_EQ(aRestoredGuidArray->Upper(), 2);
  EXPECT_EQ(aRestoredGuidArray->Value(1), 3);
  EXPECT_EQ(aRestoredGuidArray->Value(2), 4);
  EXPECT_EQ(aRestoredGuidArray->ID(), aGuid);
}

static void RunBasicNameDefaultAndUserGUID(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  const Standard_GUID aGuid1("12e94561-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID aGuid2("12e94562-6dbc-11d4-b9c8-0060b0ee281b");
  ASSERT_FALSE(
    TDataStd_Name::Set(aLabel, aGuid1, TCollection_ExtendedString("New Attribute_1")).IsNull());
  ASSERT_FALSE(
    TDataStd_Name::Set(aLabel, aGuid2, TCollection_ExtendedString("New Attribute_2")).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_caf_basic_name_user_guids");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  occ::handle<TDataStd_Name> aName1;
  occ::handle<TDataStd_Name> aName2;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid1, aName1));
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid2, aName2));
  EXPECT_EQ(aName1->Get(), TCollection_ExtendedString("New Attribute_1"));
  EXPECT_EQ(aName2->Get(), TCollection_ExtendedString("New Attribute_2"));
}

static void RunBasicNameDefaultAndUserGUIDPair(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  const Standard_GUID aGuid("12e94562-6dbc-11d4-b9c8-0060b0ee281b");
  ASSERT_FALSE(TDataStd_Name::Set(aLabel, TCollection_ExtendedString("New Attribute_1")).IsNull());
  ASSERT_FALSE(
    TDataStd_Name::Set(aLabel, aGuid, TCollection_ExtendedString("New Attribute_2")).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_caf_basic_name_default_and_guid");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  occ::handle<TDataStd_Name> aName;
  occ::handle<TDataStd_Name> aGuidName;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_Name::GetID(), aName));
  ASSERT_TRUE(aRestoredLabel.FindAttribute(aGuid, aGuidName));
  EXPECT_EQ(aName->Get(), TCollection_ExtendedString("New Attribute_1"));
  EXPECT_EQ(aGuidName->Get(), TCollection_ExtendedString("New Attribute_2"));
}

static void AddEmptyAttribute(const TDF_Label& theLabel, const char* theTypeName)
{
  if (strcmp(theTypeName, "TDataStd_Tick") == 0)
  {
    theLabel.AddAttribute(new TDataStd_Tick());
  }
  else if (strcmp(theTypeName, "TDataStd_Directory") == 0)
  {
    theLabel.AddAttribute(new TDataStd_Directory());
  }
  else if (strcmp(theTypeName, "TDataStd_NoteBook") == 0)
  {
    theLabel.AddAttribute(new TDataStd_NoteBook());
  }
  else if (strcmp(theTypeName, "TDataXtd_Axis") == 0)
  {
    theLabel.AddAttribute(new TDataXtd_Axis());
  }
  else if (strcmp(theTypeName, "TDataXtd_Placement") == 0)
  {
    theLabel.AddAttribute(new TDataXtd_Placement());
  }
  else if (strcmp(theTypeName, "TDataXtd_Plane") == 0)
  {
    theLabel.AddAttribute(new TDataXtd_Plane());
  }
  else if (strcmp(theTypeName, "TDataXtd_Point") == 0)
  {
    theLabel.AddAttribute(new TDataXtd_Point());
  }
  else if (strcmp(theTypeName, "TDataXtd_Shape") == 0)
  {
    theLabel.AddAttribute(new TDataXtd_Shape());
  }
  else
  {
    ASSERT_TRUE(false);
  }
}

static int NumberOfAttributes(const TDF_Label& theLabel)
{
  int                   aCount = 0;
  TDF_AttributeIterator anIterator(theLabel);
  for (; anIterator.More(); anIterator.Next())
  {
    ++aCount;
  }
  return aCount;
}

static bool HasAttributeType(const TDF_Label& theLabel, const occ::handle<Standard_Type>& theType)
{
  for (TDF_AttributeIterator anIterator(theLabel); anIterator.More(); anIterator.Next())
  {
    if (anIterator.Value()->IsKind(theType))
    {
      return true;
    }
  }
  return false;
}

static void RunBasicEmptyAttributes(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  aDocument->NewCommand();
  AddEmptyAttribute(aLabel, "TDataStd_Tick");
  AddEmptyAttribute(aLabel, "TDataStd_Directory");
  AddEmptyAttribute(aLabel, "TDataStd_NoteBook");
  AddEmptyAttribute(aLabel, "TDataXtd_Axis");
  AddEmptyAttribute(aLabel, "TDataXtd_Placement");
  AddEmptyAttribute(aLabel, "TDataXtd_Plane");
  AddEmptyAttribute(aLabel, "TDataXtd_Point");
  AddEmptyAttribute(aLabel, "TDataXtd_Shape");
  ASSERT_TRUE(aDocument->CommitCommand());
  EXPECT_EQ(NumberOfAttributes(aLabel), 8);
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataStd_Tick)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataStd_Directory)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataStd_NoteBook)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataXtd_Axis)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataXtd_Placement)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataXtd_Plane)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataXtd_Point)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataXtd_Shape)));

  aDocument->NewCommand();
  ASSERT_TRUE(aLabel.ForgetAttribute(TDataXtd_Point::GetID()));
  ASSERT_TRUE(aDocument->CommitCommand());
  EXPECT_EQ(NumberOfAttributes(aLabel), 7);
  ASSERT_TRUE(aDocument->Undo());
  ASSERT_TRUE(aDocument->Undo());
  EXPECT_EQ(NumberOfAttributes(aLabel), 0);
  ASSERT_TRUE(aDocument->Redo());
  EXPECT_EQ(NumberOfAttributes(aLabel), 8);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_caf_basic_empty_attributes");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->Main().FindChild(2, false);
  ASSERT_FALSE(aRestoredLabel.IsNull());
  EXPECT_EQ(NumberOfAttributes(aRestoredLabel), 8);
  EXPECT_TRUE(HasAttributeType(aRestoredLabel, STANDARD_TYPE(TDataStd_Tick)));
  EXPECT_TRUE(HasAttributeType(aRestoredLabel, STANDARD_TYPE(TDataStd_Directory)));
  EXPECT_TRUE(HasAttributeType(aRestoredLabel, STANDARD_TYPE(TDataStd_NoteBook)));
  EXPECT_TRUE(HasAttributeType(aRestoredLabel, STANDARD_TYPE(TDataXtd_Axis)));
  EXPECT_TRUE(HasAttributeType(aRestoredLabel, STANDARD_TYPE(TDataXtd_Placement)));
  EXPECT_TRUE(HasAttributeType(aRestoredLabel, STANDARD_TYPE(TDataXtd_Plane)));
  EXPECT_TRUE(HasAttributeType(aRestoredLabel, STANDARD_TYPE(TDataXtd_Point)));
  EXPECT_TRUE(HasAttributeType(aRestoredLabel, STANDARD_TYPE(TDataXtd_Shape)));
}

static void RunBasicReference(const char* theFormat, const bool theUseStream = false)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel1 = aDocument->Main().FindChild(2, true);
  const TDF_Label aLabel2 = aDocument->Main().FindChild(3, true);
  const TDF_Label aLabel3 = aDocument->Main().FindChild(4, true);
  const TDF_Label aLabel4 = aDocument->Main().FindChild(5, true);
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel1, 100).IsNull());
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel2, 200).IsNull());
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel3, 300).IsNull());
  ASSERT_FALSE(TDF_Reference::Set(aLabel4, aLabel2).IsNull());

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_caf_basic_reference", theUseStream);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel2 = aRestored->Main().FindChild(3, false);
  const TDF_Label aRestoredLabel4 = aRestored->Main().FindChild(5, false);
  ASSERT_FALSE(aRestoredLabel2.IsNull());
  ASSERT_FALSE(aRestoredLabel4.IsNull());
  occ::handle<TDF_Reference> aReference;
  ASSERT_TRUE(aRestoredLabel4.FindAttribute(TDF_Reference::GetID(), aReference));
  ASSERT_TRUE(aReference->Get() == aRestoredLabel2);
  occ::handle<TDataStd_Integer> anInteger;
  ASSERT_TRUE(aRestoredLabel2.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  EXPECT_EQ(anInteger->Get(), 200);
}

static TDF_Label FindOrCreatePath(const occ::handle<TDocStd_Document>& theDocument,
                                  const int*                           theTags,
                                  const int                            theNbTags)
{
  TDF_Label aLabel = theDocument->Main();
  for (int anIndex = 0; anIndex < theNbTags; ++anIndex)
  {
    aLabel = aLabel.FindChild(theTags[anIndex], true);
  }
  return aLabel;
}

static void RunB4(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const int        aTags1[] = {3};
  const int        aTags2[] = {2076534922, 524398634, 912349856};
  const int        aTags3[] = {3, 9283, 12, 1, 9843, 46793, 321};
  const int        aTags4[] = {4, 81245034, 321, 1, 1, 1, 345, 8542, 1, 90, 6453};
  const int        aTags5[] = {5, 8, 4, 2, 213, 3242, 123456789, 987654321};
  const int        aTags6[] = {2, 15, 123, 31214, 452398, 421, 3, 2, 1, 3, 1, 2, 9, 10, 2, 1, 1, 4};
  const int        aTags7[] = {2, 2, 1, 1, 1, 3, 1, 2, 1, 34, 1, 2, 4, 1, 5,
                               4, 1, 9, 3, 1, 2, 1, 8, 2, 3,  1, 9, 5, 2, 7};
  const int        aTags8[] = {2, 9};
  const int* const aTags[]  = {aTags1, aTags2, aTags3, aTags4, aTags5, aTags6, aTags7, aTags8};
  const int        aTagLengths[] = {1, 3, 7, 11, 8, 18, 30, 2};
  NCollection_Array1<TDF_Label>               aLabels(1, 8);
  NCollection_Array1<TCollection_AsciiString> aEntries(1, 8);
  for (int anIndex = 1; anIndex <= 8; ++anIndex)
  {
    aLabels.SetValue(anIndex,
                     FindOrCreatePath(aDocument, aTags[anIndex - 1], aTagLengths[anIndex - 1]));
    TDF_Tool::Entry(aLabels.Value(anIndex), aEntries.ChangeValue(anIndex));
  }

  TDataStd_Integer::Set(aLabels.Value(8), 0);
  for (int anIndex = 1; anIndex <= 8; ++anIndex)
  {
    TDataStd_Integer::Set(aLabels.Value(anIndex), anIndex);
    TDF_Reference::Set(aLabels.Value(anIndex), aLabels.Value(anIndex == 1 ? 8 : anIndex - 1));
  }

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_reference_chain_b4_bin");
  ASSERT_FALSE(aRestored.IsNull());
  for (int anIndex = 1; anIndex <= 8; ++anIndex)
  {
    const TDF_Label aRestoredLabel = LabelByEntry(aRestored, aEntries.Value(anIndex));
    ASSERT_FALSE(aRestoredLabel.IsNull());
    occ::handle<TDF_Reference> aReference;
    ASSERT_TRUE(aRestoredLabel.FindAttribute(TDF_Reference::GetID(), aReference));
    const int anExpectedIndex = anIndex == 1 ? 8 : anIndex - 1;
    EXPECT_TRUE(aReference->Get() == LabelByEntry(aRestored, aEntries.Value(anExpectedIndex)));
    occ::handle<TDataStd_Integer> anInteger;
    ASSERT_TRUE(aReference->Get().FindAttribute(TDataStd_Integer::GetID(), anInteger));
    EXPECT_EQ(anInteger->Get(), anExpectedIndex);
  }
}

static void RunC8(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aVariableLabel1 =
    aDocument->GetData()->Root().FindChild(1, true).FindChild(1, true);
  const TDF_Label aVariableLabel2 =
    aDocument->GetData()->Root().FindChild(1, true).FindChild(2, true);
  const TDF_Label aVariableLabel3 =
    aDocument->GetData()->Root().FindChild(1, true).FindChild(3, true);
  const TDF_Label aRelationLabel =
    aDocument->GetData()->Root().FindChild(1, true).FindChild(13, true);
  occ::handle<TDataStd_Variable> aVariable1 = TDataStd_Variable::Set(aVariableLabel1);
  occ::handle<TDataStd_Variable> aVariable2 = TDataStd_Variable::Set(aVariableLabel2);
  occ::handle<TDataStd_Variable> aVariable3 = TDataStd_Variable::Set(aVariableLabel3);
  aVariable1->Unit("N");
  aVariable2->Unit("kg");
  aVariable3->Unit("m/s2");
  aVariable1->Constant(true);
  aVariable2->Constant(false);
  aVariable3->Constant(false);
  occ::handle<TDataStd_Relation> aRelation = TDataStd_Relation::Set(aRelationLabel);
  aRelation->SetRelation(TCollection_ExtendedString("f = m*a"));
  aRelation->GetVariables().Append(aVariable1);
  aRelation->GetVariables().Append(aVariable2);
  aRelation->GetVariables().Append(aVariable3);

  TCollection_AsciiString aRelationEntry;
  TDF_Tool::Entry(aRelationLabel, aRelationEntry);
  const TCollection_AsciiString anExpectedEntries[] = {TCollection_AsciiString("0:1:1"),
                                                       TCollection_AsciiString("0:1:2"),
                                                       TCollection_AsciiString("0:1:3")};

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_relation_c8_bin");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label                aRestoredRelationLabel = LabelByEntry(aRestored, aRelationEntry);
  occ::handle<TDataStd_Relation> aRestoredRelation;
  ASSERT_TRUE(aRestoredRelationLabel.FindAttribute(TDataStd_Relation::GetID(), aRestoredRelation));
  EXPECT_EQ(aRestoredRelation->GetRelation(), TCollection_ExtendedString("f = m*a"));
  int anIndex = 0;
  for (NCollection_List<occ::handle<TDF_Attribute>>::Iterator anIterator(
         aRestoredRelation->GetVariables());
       anIterator.More();
       anIterator.Next(), ++anIndex)
  {
    ASSERT_LT(anIndex, 3);
    TCollection_AsciiString anActualEntry;
    TDF_Tool::Entry(anIterator.Value()->Label(), anActualEntry);
    EXPECT_EQ(anActualEntry, anExpectedEntries[anIndex]);
  }
  EXPECT_EQ(anIndex, 3);

  const TDF_Label                aRestoredVariableLabel1 = LabelByEntry(aRestored, "0:1:1");
  const TDF_Label                aRestoredVariableLabel2 = LabelByEntry(aRestored, "0:1:2");
  const TDF_Label                aRestoredVariableLabel3 = LabelByEntry(aRestored, "0:1:3");
  occ::handle<TDataStd_Variable> aRestoredVariable;
  ASSERT_TRUE(aRestoredVariableLabel1.FindAttribute(TDataStd_Variable::GetID(), aRestoredVariable));
  EXPECT_TRUE(aRestoredVariable->IsConstant());
  EXPECT_EQ(aRestoredVariable->Unit(), TCollection_AsciiString("N"));
  ASSERT_TRUE(aRestoredVariableLabel2.FindAttribute(TDataStd_Variable::GetID(), aRestoredVariable));
  EXPECT_FALSE(aRestoredVariable->IsConstant());
  EXPECT_EQ(aRestoredVariable->Unit(), TCollection_AsciiString("kg"));
  ASSERT_TRUE(aRestoredVariableLabel3.FindAttribute(TDataStd_Variable::GetID(), aRestoredVariable));
  EXPECT_FALSE(aRestoredVariable->IsConstant());
  EXPECT_EQ(aRestoredVariable->Unit(), TCollection_AsciiString("m/s2"));
}

static void RunC9(const char* theFormat)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(1, true).FindChild(1, true);
  occ::handle<TDataStd_Variable> aVariable = TDataStd_Variable::Set(aLabel);
  ASSERT_FALSE(aVariable.IsNull());
  aVariable->Constant(true);
  aVariable->Unit("kg/m3");

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, theFormat, "draw_ocaf_variable_c9_bin");
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label                aRestoredLabel = LabelByEntry(aRestored, "0:1:1");
  occ::handle<TDataStd_Variable> aRestoredVariable;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_Variable::GetID(), aRestoredVariable));
  EXPECT_TRUE(aRestoredVariable->IsConstant());
  EXPECT_EQ(aRestoredVariable->Unit(), TCollection_AsciiString("kg/m3"));
}

static void RunCafBug31839_1Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label aObjectsLabel = aDocument->Main();
  aDocument->NewCommand();
  for (int anObjectIndex = 1; anObjectIndex <= 100; ++anObjectIndex)
  {
    const TDF_Label anObjectLabel = aObjectsLabel.FindChild(anObjectIndex, true);
    for (int aSubLabelIndex = 1; aSubLabelIndex <= 10; ++aSubLabelIndex)
    {
      const TDF_Label aSubLabel = anObjectLabel.FindChild(aSubLabelIndex, true);
      ASSERT_FALSE(TDataStd_Integer::Set(aSubLabel, 10).IsNull());
      ASSERT_FALSE(TDataStd_Real::Set(aSubLabel, 12.3).IsNull());
    }
  }
  aDocument->CommitCommand();

  std::stringstream aStream;
  SaveDocumentStream(anApplication, aDocument, aStream);
  anApplication->Close(aDocument);

  occ::handle<TDocStd_Document> aFullDocument = OpenDocumentStream(anApplication, aStream);
  ASSERT_FALSE(aFullDocument.IsNull());
  const TDF_Label aFullLabel = LabelByEntry(aFullDocument, "0:1:1:1");
  ASSERT_FALSE(aFullLabel.IsNull());
  occ::handle<TDataStd_Integer> anInteger;
  occ::handle<TDataStd_Real>    aReal;
  ASSERT_TRUE(aFullLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  ASSERT_TRUE(aFullLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_EQ(anInteger->Get(), 10);
  EXPECT_DOUBLE_EQ(aReal->Get(), 12.3);
  anApplication->Close(aFullDocument);

  occ::handle<PCDM_ReaderFilter> aSkipInteger = new PCDM_ReaderFilter;
  aSkipInteger->AddSkipped("TDataStd_Integer");
  occ::handle<TDocStd_Document> aPartialDocument;
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aPartialDocument, aSkipInteger), PCDM_RS_OK);
  ASSERT_FALSE(aPartialDocument.IsNull());
  const TDF_Label aFirstSubLabel = LabelByEntry(aPartialDocument, "0:1:1:1");
  ASSERT_FALSE(aFirstSubLabel.IsNull());
  anInteger.Nullify();
  aReal.Nullify();
  EXPECT_FALSE(aFirstSubLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  ASSERT_TRUE(aFirstSubLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_DOUBLE_EQ(aReal->Get(), 12.3);

  occ::handle<PCDM_ReaderFilter> aReadOneInteger =
    new PCDM_ReaderFilter(PCDM_ReaderFilter::AppendMode_Protect);
  aReadOneInteger->AddPath("0:1:1:1");
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aPartialDocument, aReadOneInteger),
            PCDM_RS_OK);
  anInteger.Nullify();
  aReal.Nullify();
  ASSERT_TRUE(aFirstSubLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  ASSERT_TRUE(aFirstSubLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_EQ(anInteger->Get(), 10);
  EXPECT_DOUBLE_EQ(aReal->Get(), 12.3);

  const TDF_Label aTenthSubLabel = LabelByEntry(aPartialDocument, "0:1:1:10");
  ASSERT_FALSE(aTenthSubLabel.IsNull());
  anInteger.Nullify();
  aReal.Nullify();
  EXPECT_FALSE(aTenthSubLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  ASSERT_TRUE(aTenthSubLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_DOUBLE_EQ(aReal->Get(), 12.3);

  occ::handle<PCDM_ReaderFilter> aReadFirstObject =
    new PCDM_ReaderFilter(PCDM_ReaderFilter::AppendMode_Protect);
  aReadFirstObject->AddPath("0:1:1");
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aPartialDocument, aReadFirstObject),
            PCDM_RS_OK);
  anInteger.Nullify();
  ASSERT_TRUE(aTenthSubLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  EXPECT_EQ(anInteger->Get(), 10);
  const TDF_Label aSecondObjectSubLabel = LabelByEntry(aPartialDocument, "0:1:2:5");
  ASSERT_FALSE(aSecondObjectSubLabel.IsNull());
  anInteger.Nullify();
  aReal.Nullify();
  EXPECT_FALSE(aSecondObjectSubLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  ASSERT_TRUE(aSecondObjectSubLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_DOUBLE_EQ(aReal->Get(), 12.3);

  const TDF_Label aChangedIntegerLabel = LabelByEntry(aPartialDocument, "0:1:1:5");
  const TDF_Label aChangedRealLabel    = LabelByEntry(aPartialDocument, "0:1:1:7");
  ASSERT_FALSE(TDataStd_Integer::Set(aChangedIntegerLabel, 21).IsNull());
  ASSERT_FALSE(TDataStd_Real::Set(aChangedRealLabel, 32.1).IsNull());

  occ::handle<PCDM_ReaderFilter> anOverwrite =
    new PCDM_ReaderFilter(PCDM_ReaderFilter::AppendMode_Overwrite);
  anOverwrite->AddPath("0:1:1");
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aPartialDocument, anOverwrite), PCDM_RS_OK);
  ASSERT_TRUE(aChangedIntegerLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  ASSERT_TRUE(aChangedRealLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_EQ(anInteger->Get(), 10);
  EXPECT_DOUBLE_EQ(aReal->Get(), 12.3);

  ASSERT_FALSE(TDataStd_Integer::Set(aChangedIntegerLabel, 21).IsNull());
  ASSERT_FALSE(TDataStd_Real::Set(aChangedRealLabel, 32.1).IsNull());
  occ::handle<PCDM_ReaderFilter> anAppend =
    new PCDM_ReaderFilter(PCDM_ReaderFilter::AppendMode_Protect);
  anAppend->AddPath("0:1:1");
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aPartialDocument, anAppend), PCDM_RS_OK);
  ASSERT_TRUE(aChangedIntegerLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  ASSERT_TRUE(aChangedRealLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_EQ(anInteger->Get(), 21);
  EXPECT_DOUBLE_EQ(aReal->Get(), 32.1);
  anApplication->Close(aPartialDocument);
}

static void RunCafBug31839_2Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const TopoDS_Shape               aBox = BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape();
  NCollection_Array1<TopoDS_Shape> aFaces(1, 6);
  int                              aFaceIndex = 1;
  for (TopExp_Explorer anExplorer(aBox, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ASSERT_LE(aFaceIndex, 6);
    aFaces.SetValue(aFaceIndex++, anExplorer.Current());
  }
  ASSERT_EQ(aFaceIndex, 7);

  const TDF_Label aBoxLabel = aDocument->Main();
  aDocument->NewCommand();
  SetShape(aBoxLabel, aBox);
  for (int anIndex = 1; anIndex <= 6; ++anIndex)
  {
    SetShape(aBoxLabel.FindChild(anIndex, true), aFaces.Value(anIndex));
  }
  aDocument->CommitCommand();

  std::stringstream aStream;
  SaveDocumentStream(anApplication, aDocument, aStream);
  anApplication->Close(aDocument);

  occ::handle<PCDM_ReaderFilter> aSkipShapes = new PCDM_ReaderFilter;
  aSkipShapes->AddSkipped("TNaming_NamedShape");
  occ::handle<TDocStd_Document> aRestored;
  const PCDM_ReaderStatus       aSkipStatus =
    OpenDocumentStream(anApplication, aStream, aRestored, aSkipShapes);
  // BinOcaf reports a generic driver failure when the filter rejects every attribute;
  // DRAW ignores this status and validates the resulting document contents instead.
  EXPECT_EQ(aSkipStatus, PCDM_RS_DriverFailure);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aSkippedBoxLabel = LabelByEntry(aRestored, "0:1");
  const TDF_Label aSkippedFace1    = LabelByEntry(aRestored, "0:1:1");
  if (!aSkippedBoxLabel.IsNull())
  {
    EXPECT_TRUE(ShapeOf(aSkippedBoxLabel).IsNull());
  }
  if (!aSkippedFace1.IsNull())
  {
    EXPECT_TRUE(ShapeOf(aSkippedFace1).IsNull());
  }

  occ::handle<PCDM_ReaderFilter> aReadFirstFace =
    new PCDM_ReaderFilter(PCDM_ReaderFilter::AppendMode_Protect);
  aReadFirstFace->AddPath("0:1:1");
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aRestored, aReadFirstFace), PCDM_RS_OK);
  const TDF_Label aRestoredBoxLabel = LabelByEntry(aRestored, "0:1");
  const TDF_Label aRestoredFace1    = LabelByEntry(aRestored, "0:1:1");
  ASSERT_FALSE(aRestoredBoxLabel.IsNull());
  ASSERT_FALSE(aRestoredFace1.IsNull());
  EXPECT_TRUE(ShapeOf(aRestoredBoxLabel).IsNull());
  const TopoDS_Shape aRestoredFace = ShapeOf(aRestoredFace1);
  ASSERT_FALSE(aRestoredFace.IsNull());
  EXPECT_TRUE(ShapeOf(LabelByEntry(aRestored, "0:1:2")).IsNull());

  occ::handle<PCDM_ReaderFilter> anOverwriteShapes =
    new PCDM_ReaderFilter(PCDM_ReaderFilter::AppendMode_Overwrite);
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aRestored, anOverwriteShapes), PCDM_RS_OK);
  const TopoDS_Shape aRestoredBox                 = ShapeOf(aRestoredBoxLabel);
  const TopoDS_Shape aRestoredFace1AfterOverwrite = ShapeOf(aRestoredFace1);
  ASSERT_FALSE(aRestoredBox.IsNull());
  ASSERT_FALSE(aRestoredFace1AfterOverwrite.IsNull());
  ExpectBoxBounds(aRestoredBox, 0.0, 0.0, 0.0, 1.0, 2.0, 3.0);

  bool hasSharedFace = false;
  for (TopExp_Explorer aBoxExplorer(aRestoredBox, TopAbs_FACE); aBoxExplorer.More();
       aBoxExplorer.Next())
  {
    for (TopExp_Explorer aFaceExplorer(aRestoredFace1AfterOverwrite, TopAbs_FACE);
         aFaceExplorer.More();
         aFaceExplorer.Next())
    {
      if (aBoxExplorer.Current().IsSame(aFaceExplorer.Current()))
      {
        hasSharedFace = true;
      }
    }
  }
  EXPECT_TRUE(hasSharedFace);
  anApplication->Close(aRestored);
}

static void ExpectCafBug31918IntegerArray(const TDF_Label& theLabel)
{
  occ::handle<TDataStd_IntegerArray> anArray;
  ASSERT_TRUE(theLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anArray));
  ASSERT_FALSE(anArray.IsNull());
  EXPECT_EQ(anArray->Lower(), 0);
  EXPECT_EQ(anArray->Upper(), 1000);
  EXPECT_EQ(anArray->Length(), 1001);
  EXPECT_EQ(anArray->Value(0), 100);
  EXPECT_EQ(anArray->Value(500), 600);
  EXPECT_EQ(anArray->Value(1000), 1100);
}

static void RunCafBug31918_1Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinLOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  NCollection_Array1<int> aValues(0, 1000);
  for (int anIndex = 0; anIndex <= 1000; ++anIndex)
  {
    aValues.SetValue(anIndex, 100 + anIndex);
  }

  aDocument->NewCommand();
  for (int aLabelIndex = 1; aLabelIndex <= 4; ++aLabelIndex)
  {
    for (int aSubLabelIndex = 1; aSubLabelIndex <= 100; ++aSubLabelIndex)
    {
      const TDF_Label aLabel =
        aDocument->GetData()->Root().FindChild(aLabelIndex, true).FindChild(aSubLabelIndex, true);
      occ::handle<TDataStd_IntegerArray> anArray =
        TDataStd_IntegerArray::Set(aLabel, aValues.Lower(), aValues.Upper());
      ASSERT_FALSE(anArray.IsNull());
      for (int anIndex = aValues.Lower(); anIndex <= aValues.Upper(); ++anIndex)
      {
        anArray->SetValue(anIndex, aValues.Value(anIndex));
      }
      ASSERT_FALSE(TDataStd_Real::Set(aLabel, 0.1).IsNull());
    }
  }
  aDocument->CommitCommand();

  std::stringstream aStream;
  SaveDocumentStream(anApplication, aDocument, aStream);
  anApplication->Close(aDocument);

  auto ExpectOpen = [&](const occ::handle<PCDM_ReaderFilter>& theFilter) {
    occ::handle<TDocStd_Document> anOpened;
    EXPECT_EQ(OpenDocumentStream(anApplication, aStream, anOpened, theFilter), PCDM_RS_OK);
    EXPECT_FALSE(anOpened.IsNull());
    if (!anOpened.IsNull())
    {
      anApplication->Close(anOpened);
    }
  };

  ExpectOpen(occ::handle<PCDM_ReaderFilter>());
  occ::handle<PCDM_ReaderFilter> aQuarterFilter = new PCDM_ReaderFilter;
  aQuarterFilter->AddPath("0:2");
  ExpectOpen(aQuarterFilter);
  occ::handle<PCDM_ReaderFilter> aFourQuarterFilter = new PCDM_ReaderFilter;
  aFourQuarterFilter->AddPath("0:1");
  aFourQuarterFilter->AddPath("0:2");
  aFourQuarterFilter->AddPath("0:3");
  aFourQuarterFilter->AddPath("0:4");
  ExpectOpen(aFourQuarterFilter);

  occ::handle<TDocStd_Document>  aNoArrayDocument;
  occ::handle<PCDM_ReaderFilter> aNoArrayFilter = new PCDM_ReaderFilter;
  aNoArrayFilter->AddSkipped("TDataStd_IntegerArray");
  aNoArrayFilter->AddPath("0:2");
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aNoArrayDocument, aNoArrayFilter),
            PCDM_RS_OK);
  ASSERT_FALSE(aNoArrayDocument.IsNull());

  const TDF_Label aQuarterLabel = LabelByEntry(aNoArrayDocument, "0:2:13");
  ASSERT_FALSE(aQuarterLabel.IsNull());
  occ::handle<TDataStd_Real>         aReal;
  occ::handle<TDataStd_IntegerArray> anIntegerArray;
  ASSERT_TRUE(aQuarterLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_DOUBLE_EQ(aReal->Get(), 0.1);
  EXPECT_FALSE(aQuarterLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anIntegerArray));

  const TDF_Label aFirstTreeLabel  = LabelByEntry(aNoArrayDocument, "0:1:1:13");
  const TDF_Label aThirdTreeLabel  = LabelByEntry(aNoArrayDocument, "0:1:3:14");
  const TDF_Label aFourthTreeLabel = LabelByEntry(aNoArrayDocument, "0:1:4:1");
  if (!aFirstTreeLabel.IsNull())
  {
    EXPECT_FALSE(aFirstTreeLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  }
  if (!aThirdTreeLabel.IsNull())
  {
    EXPECT_FALSE(aThirdTreeLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  }
  if (!aFourthTreeLabel.IsNull())
  {
    EXPECT_FALSE(aFourthTreeLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  }

  occ::handle<PCDM_ReaderFilter> anAppendArrays =
    new PCDM_ReaderFilter(PCDM_ReaderFilter::AppendMode_Protect);
  anAppendArrays->AddRead("TDataStd_IntegerArray");
  anAppendArrays->AddPath("0:2");
  anAppendArrays->AddPath("0:3");
  ASSERT_EQ(OpenDocumentStream(anApplication, aStream, aNoArrayDocument, anAppendArrays),
            PCDM_RS_OK);
  ASSERT_TRUE(aQuarterLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_DOUBLE_EQ(aReal->Get(), 0.1);
  ExpectCafBug31918IntegerArray(aQuarterLabel);
  const TDF_Label aThirdQuarterLabel = LabelByEntry(aNoArrayDocument, "0:3:1");
  ASSERT_FALSE(aThirdQuarterLabel.IsNull());
  ExpectCafBug31918IntegerArray(aThirdQuarterLabel);

  anIntegerArray.Nullify();
  if (!aFirstTreeLabel.IsNull())
  {
    EXPECT_FALSE(aFirstTreeLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anIntegerArray));
  }
  if (!aFourthTreeLabel.IsNull())
  {
    EXPECT_FALSE(aFourthTreeLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anIntegerArray));
  }
  anApplication->Close(aNoArrayDocument);
}

static void RunCafBasicY1Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label aParent = aDocument->Main();
  aParent.FindChild(1, true);
  const TDF_Label aValueLabel = aParent.FindChild(2, true);
  aParent.FindChild(3, true);
  ASSERT_FALSE(TDataStd_Integer::Set(aValueLabel, 321).IsNull());
  ASSERT_FALSE(TDataStd_Real::Set(aValueLabel, 871.33).IsNull());

  aDocument->SetEmptyLabelsSavingMode(true);
  std::stringstream aStreamWithEmptyLabels;
  SaveDocumentStream(anApplication, aDocument, aStreamWithEmptyLabels);

  aDocument->SetEmptyLabelsSavingMode(false);
  std::stringstream aStreamWithoutEmptyLabels;
  SaveDocumentStream(anApplication, aDocument, aStreamWithoutEmptyLabels);
  anApplication->Close(aDocument);

  occ::handle<TDocStd_Document> aRestoredWithEmptyLabels =
    OpenDocumentStream(anApplication, aStreamWithEmptyLabels);
  occ::handle<TDocStd_Document> aRestoredWithoutEmptyLabels =
    OpenDocumentStream(anApplication, aStreamWithoutEmptyLabels);
  ASSERT_FALSE(aRestoredWithEmptyLabels.IsNull());
  ASSERT_FALSE(aRestoredWithoutEmptyLabels.IsNull());

  const TDF_Label aParentWithEmptyLabels    = aRestoredWithEmptyLabels->Main();
  const TDF_Label aParentWithoutEmptyLabels = aRestoredWithoutEmptyLabels->Main();
  EXPECT_EQ(aParentWithEmptyLabels.NbChildren(), 3);
  EXPECT_EQ(aParentWithoutEmptyLabels.NbChildren(), 1);

  occ::handle<TDataStd_Integer> anInteger;
  occ::handle<TDataStd_Real>    aReal;
  const TDF_Label aRestoredValueWithEmptyLabels = aParentWithEmptyLabels.FindChild(2, false);
  ASSERT_TRUE(aRestoredValueWithEmptyLabels.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  ASSERT_TRUE(aRestoredValueWithEmptyLabels.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_EQ(anInteger->Get(), 321);
  EXPECT_DOUBLE_EQ(aReal->Get(), 871.33);

  const TDF_Label aRestoredValueWithoutEmptyLabels = aParentWithoutEmptyLabels.FindChild(2, false);
  ASSERT_TRUE(aRestoredValueWithoutEmptyLabels.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  ASSERT_TRUE(aRestoredValueWithoutEmptyLabels.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_EQ(anInteger->Get(), 321);
  EXPECT_DOUBLE_EQ(aReal->Get(), 871.33);

  anApplication->Close(aRestoredWithEmptyLabels);
  anApplication->Close(aRestoredWithoutEmptyLabels);
}

// bugs/caf/bug9746: BinOcaf stream persistence keeps multi-level naming
// selections, including the integer-array naming driver regression.
static void RunCafBug9746Stream()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  gp_Trsf aBoxTranslation;
  aBoxTranslation.SetTranslation(gp_Vec(35.0, 1.0, 1.0));
  const TopoDS_Shape aBox =
    BRepPrimAPI_MakeBox(130.0, 111.0, 145.0).Shape().Moved(TopLoc_Location(aBoxTranslation));
  const TopoDS_Shape aCylinderBase =
    BRepPrimAPI_MakeCylinder(50.0, 120.0, 230.0 * M_PI / 180.0).Shape();
  gp_Trsf aCylinderTranslation;
  aCylinderTranslation.SetTranslation(gp_Vec(1.0, 1.0, 75.0));
  const TopoDS_Shape aCylinder = aCylinderBase.Moved(TopLoc_Location(aCylinderTranslation));
  ASSERT_EQ(aBox.ShapeType(), TopAbs_SOLID);
  ASSERT_EQ(aCylinder.ShapeType(), TopAbs_SOLID);

  TopoDS_Shape aBoxFaces[8];
  TopoDS_Shape aBoxEdges[24];
  TopoDS_Shape aBoxVertices[16];
  ASSERT_EQ(CollectUniqueSubShapes(aBox, TopAbs_FACE, aBoxFaces, 8), 6);
  ASSERT_EQ(CollectUniqueSubShapes(aBox, TopAbs_EDGE, aBoxEdges, 24), 12);
  ASSERT_EQ(CollectUniqueSubShapes(aBox, TopAbs_VERTEX, aBoxVertices, 16), 8);

  TopoDS_Shape aCylinderFaces[8];
  TopoDS_Shape aCylinderVertices[16];
  const int aCylinderFaceCount = CollectUniqueSubShapes(aCylinder, TopAbs_FACE, aCylinderFaces, 8);
  const int aCylinderVertexCount =
    CollectUniqueSubShapes(aCylinder, TopAbs_VERTEX, aCylinderVertices, 16);
  ASSERT_GE(aCylinderFaceCount, 3);
  ASSERT_GE(aCylinderVertexCount, 3);

  const TDF_Label aRoot          = aDocument->GetData()->Root();
  const TDF_Label aBoxLabel      = aRoot.FindChild(2, true);
  const TDF_Label aCylinderLabel = aRoot.FindChild(3, true);
  DNaming::LoadImportedShape(aBoxLabel, aBox);
  DNaming::LoadImportedShape(aCylinderLabel, aCylinder);
  ASSERT_FALSE(TDataStd_Name::Set(aBoxLabel, TCollection_ExtendedString("Box1", true)).IsNull());
  ASSERT_FALSE(
    TDataStd_Name::Set(aCylinderLabel, TCollection_ExtendedString("Cylinder", true)).IsNull());

  const TDF_Label aBoxSelectionRoot = aRoot.FindChild(4, true);
  for (int aFaceIndex = 0; aFaceIndex < 6; ++aFaceIndex)
  {
    SelectImportedShape(aBoxSelectionRoot.FindChild(aFaceIndex + 1, true),
                        aBoxFaces[aFaceIndex],
                        aBox);
  }
  SelectImportedShape(aBoxSelectionRoot.FindChild(10, true), aBoxEdges[0], aBox);
  SelectImportedShape(aBoxSelectionRoot.FindChild(11, true), aBoxEdges[2], aBox);
  SelectImportedShape(aBoxSelectionRoot.FindChild(21, true), aBoxVertices[2], aBox);
  SelectImportedShape(aBoxSelectionRoot.FindChild(22, true), aBoxVertices[4], aBox);

  const TDF_Label aCylinderSelectionRoot = aRoot.FindChild(5, true);
  SelectImportedShape(aCylinderSelectionRoot.FindChild(1, true), aCylinderFaces[0], aCylinder);
  SelectImportedShape(aCylinderSelectionRoot.FindChild(2, true), aCylinderFaces[1], aCylinder);
  SelectImportedShape(aCylinderSelectionRoot.FindChild(10, true), aCylinderVertices[0], aCylinder);
  SelectImportedShape(aCylinderSelectionRoot.FindChild(11, true), aCylinderVertices[1], aCylinder);
  SelectImportedShape(aCylinderSelectionRoot.FindChild(12, true), aCylinderVertices[2], aCylinder);

  std::stringstream aStream;
  SaveDocumentStream(anApplication, aDocument, aStream);
  anApplication->Close(aDocument);
  occ::handle<TDocStd_Document> aRestored = OpenDocumentStream(anApplication, aStream);
  ASSERT_FALSE(aRestored.IsNull());

  const TDF_Label aRestoredRoot          = aRestored->GetData()->Root();
  const TDF_Label aRestoredBoxLabel      = aRestoredRoot.FindChild(2, false);
  const TDF_Label aRestoredCylinderLabel = aRestoredRoot.FindChild(3, false);
  ASSERT_FALSE(aRestoredBoxLabel.IsNull());
  ASSERT_FALSE(aRestoredCylinderLabel.IsNull());
  const TopoDS_Shape aRestoredBox      = ShapeOf(aRestoredBoxLabel);
  const TopoDS_Shape aRestoredCylinder = ShapeOf(aRestoredCylinderLabel);
  ASSERT_FALSE(aRestoredBox.IsNull());
  ASSERT_FALSE(aRestoredCylinder.IsNull());
  ExpectBoxBounds(aRestoredBox, 35.0, 1.0, 1.0, 165.0, 112.0, 146.0);

  TopoDS_Shape aRestoredBoxFaces[8];
  TopoDS_Shape aRestoredBoxEdges[24];
  TopoDS_Shape aRestoredBoxVertices[16];
  ASSERT_EQ(CollectUniqueSubShapes(aRestoredBox, TopAbs_FACE, aRestoredBoxFaces, 8), 6);
  ASSERT_EQ(CollectUniqueSubShapes(aRestoredBox, TopAbs_EDGE, aRestoredBoxEdges, 24), 12);
  ASSERT_EQ(CollectUniqueSubShapes(aRestoredBox, TopAbs_VERTEX, aRestoredBoxVertices, 16), 8);

  TopoDS_Shape aRestoredCylinderFaces[8];
  TopoDS_Shape aRestoredCylinderVertices[16];
  ASSERT_EQ(CollectUniqueSubShapes(aRestoredCylinder, TopAbs_FACE, aRestoredCylinderFaces, 8),
            aCylinderFaceCount);
  ASSERT_EQ(CollectUniqueSubShapes(aRestoredCylinder, TopAbs_VERTEX, aRestoredCylinderVertices, 16),
            aCylinderVertexCount);

  const TDF_Label aRestoredBoxSelectionRoot      = aRestoredRoot.FindChild(4, false);
  const TDF_Label aRestoredCylinderSelectionRoot = aRestoredRoot.FindChild(5, false);
  ASSERT_FALSE(aRestoredBoxSelectionRoot.IsNull());
  ASSERT_FALSE(aRestoredCylinderSelectionRoot.IsNull());
  for (int aFaceIndex = 0; aFaceIndex < 6; ++aFaceIndex)
  {
    EXPECT_TRUE(ShapeOf(aRestoredBoxSelectionRoot.FindChild(aFaceIndex + 1, false))
                  .IsSame(aRestoredBoxFaces[aFaceIndex]));
  }
  EXPECT_TRUE(ShapeOf(aRestoredBoxSelectionRoot.FindChild(10, false)).IsSame(aRestoredBoxEdges[0]));
  EXPECT_TRUE(ShapeOf(aRestoredBoxSelectionRoot.FindChild(11, false)).IsSame(aRestoredBoxEdges[2]));
  EXPECT_TRUE(
    ShapeOf(aRestoredBoxSelectionRoot.FindChild(21, false)).IsSame(aRestoredBoxVertices[2]));
  EXPECT_TRUE(
    ShapeOf(aRestoredBoxSelectionRoot.FindChild(22, false)).IsSame(aRestoredBoxVertices[4]));
  EXPECT_TRUE(
    ShapeOf(aRestoredCylinderSelectionRoot.FindChild(1, false)).IsSame(aRestoredCylinderFaces[0]));
  EXPECT_TRUE(
    ShapeOf(aRestoredCylinderSelectionRoot.FindChild(2, false)).IsSame(aRestoredCylinderFaces[1]));
  EXPECT_TRUE(ShapeOf(aRestoredCylinderSelectionRoot.FindChild(10, false))
                .IsSame(aRestoredCylinderVertices[0]));
  EXPECT_TRUE(ShapeOf(aRestoredCylinderSelectionRoot.FindChild(11, false))
                .IsSame(aRestoredCylinderVertices[1]));
  EXPECT_TRUE(ShapeOf(aRestoredCylinderSelectionRoot.FindChild(12, false))
                .IsSame(aRestoredCylinderVertices[2]));

  const TDF_Label aCylSelection10      = aRestoredCylinderSelectionRoot.FindChild(10, false);
  const TDF_Label aCylSelection11      = aRestoredCylinderSelectionRoot.FindChild(11, false);
  const TDF_Label aCylSelection12      = aRestoredCylinderSelectionRoot.FindChild(12, false);
  const TDF_Label aCylSelection10Child = aCylSelection10.FindChild(1, false);
  const TDF_Label aCylSelection11Child = aCylSelection11.FindChild(1, false);
  const TDF_Label aCylSelection12Child = aCylSelection12.FindChild(1, false);
  ASSERT_FALSE(aCylSelection10Child.IsNull());
  ASSERT_FALSE(aCylSelection11Child.IsNull());
  ASSERT_FALSE(aCylSelection12Child.IsNull());

  const TDF_Label aCylinderChild1 = aRestoredCylinderLabel.FindChild(1, false);
  const TDF_Label aCylinderChild2 = aRestoredCylinderLabel.FindChild(2, false);
  const TDF_Label aCylinderChild3 = aRestoredCylinderLabel.FindChild(3, false);
  const TDF_Label aCylinderChild5 = aRestoredCylinderLabel.FindChild(5, false);
  ASSERT_FALSE(aCylinderChild1.IsNull());
  ASSERT_FALSE(aCylinderChild2.IsNull());
  ASSERT_FALSE(aCylinderChild3.IsNull());
  ASSERT_FALSE(aCylinderChild5.IsNull());
  const TDF_Label aIntersectionArguments[] = {aCylinderChild5, aCylinderChild1, aCylinderChild2};
  const TDF_Label aVertexArguments[]       = {aCylinderChild3, aCylinderChild5, aCylinderChild1};
  ExpectNamingSelection(aCylSelection10, TNaming_IDENTITY, TopAbs_VERTEX, &aCylSelection10Child, 1);
  ExpectNamingSelection(aCylSelection10Child,
                        TNaming_INTERSECTION,
                        TopAbs_VERTEX,
                        aIntersectionArguments,
                        3);
  ExpectNamingSelection(aCylSelection11, TNaming_IDENTITY, TopAbs_VERTEX, &aCylSelection11Child, 1);
  ExpectNamingSelection(aCylSelection12, TNaming_IDENTITY, TopAbs_VERTEX, &aCylSelection12Child, 1);
  ExpectNamingSelection(aCylSelection12Child,
                        TNaming_INTERSECTION,
                        TopAbs_VERTEX,
                        aVertexArguments,
                        3);

  anApplication->Close(aRestored);
}
} // namespace

// xml/data/ocaf/B7: a linked named shape survives XML and binary OCAF storage.
TEST(TDocStd_Storage_Test, Xml_B7_CopyWithLink)
{
  RunB7("XmlOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_B7_CopyWithLink)
{
  RunB7("BinOcaf");
}

// xml/data/ocaf/C2: a constraint and its shape/value references survive storage.
TEST(TDocStd_Storage_Test, Xml_C2_Constraint)
{
  RunC2("XmlOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_C2_Constraint)
{
  RunC2("BinOcaf");
}

// xml/data/ocaf/C3: the TDataXtd_Geometry type survives storage.
TEST(TDocStd_Storage_Test, Xml_C3_GeometryType)
{
  RunC3("XmlOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_C3_GeometryType)
{
  RunC3("BinOcaf");
}

// xml/data/ocaf/C4: a selected shape and its naming survive storage.
TEST(TDocStd_Storage_Test, Xml_C4_Selection)
{
  RunC4("XmlOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_C4_Selection)
{
  RunC4("BinOcaf");
}

// xml/data/ocaf/C5: pattern references and scalar parameters survive storage.
TEST(TDocStd_Storage_Test, Xml_C5_Pattern)
{
  RunC5("XmlOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_C5_Pattern)
{
  RunC5("BinOcaf");
}

// xml/data/ocaf/C7: a position survives storage.
TEST(TDocStd_Storage_Test, Xml_C7_Position)
{
  RunC7("XmlOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_C7_Position)
{
  RunC7("BinOcaf");
}

// xml/data/ocaf/D2: references to labels with large tags survive storage.
TEST(TDocStd_Storage_Test, Xml_D2_ReferenceChain)
{
  RunD2("XmlOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_D2_ReferenceChain)
{
  RunD2("BinOcaf");
}

// xml/data/ocaf/D3: relation variables, units, and flags survive storage.
TEST(TDocStd_Storage_Test, Xml_D3_Relation)
{
  RunD3("XmlOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_D3_Relation)
{
  RunD3("BinOcaf");
}

// xml/data/ocaf/A1-A9: binary OCAF preserves scalar, array, and geometry attributes.
TEST(TDocStd_Storage_Test, Cbf_A1_Integer)
{
  RunA1("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_A2_Real)
{
  RunA2("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_A3_RealArray)
{
  RunA3("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_A4_IntegerArray)
{
  RunA4("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_A5_Name)
{
  RunA5("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_A6_Comment)
{
  RunA6("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_A7_Point)
{
  RunA7("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_A8_Axis)
{
  RunA8("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_A9_Plane)
{
  RunA9("BinOcaf");
}

// xml/data/ocaf/B1-B5, B8-B9: binary OCAF preserves user, shape, tree,
// and directory attributes.
TEST(TDocStd_Storage_Test, Cbf_B1_UserAttribute)
{
  RunB1("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_B2_TwoUserAttributes)
{
  RunB2("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_B3_NamedShape)
{
  RunB3("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_B4_ReferenceChain)
{
  RunB4("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_B5_NamedShapeAfterTransaction)
{
  RunB5("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_B8_TagSource)
{
  RunB8("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_B9_Directory)
{
  RunB9("BinOcaf");
}

// xml/data/ocaf/C1, C6, C8-C9: binary OCAF preserves tree and function data,
// relations, and variables.
TEST(TDocStd_Storage_Test, Cbf_C1_TreeNodeChildren)
{
  RunC1("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_C6_Function)
{
  RunC6("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_C8_Relation)
{
  RunC8("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_C9_Variable)
{
  RunC9("BinOcaf");
}

// xml/data/ocaf/D1, D4-D6: binary OCAF preserves names and one-element arrays.
TEST(TDocStd_Storage_Test, Cbf_D1_SpecialNames)
{
  RunD1("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_D4_SingleIntegerArray)
{
  RunD4("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_D5_SingleRealArray)
{
  RunD5("BinOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_D6_LeadingZeroName)
{
  RunD6("BinOcaf");
}

// caf/basic/A3: a default TDataStd_Integer survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_A3_Integer)
{
  RunA1("BinOcaf");
}

// caf/basic/A6: two user-GUID TDataStd_Integer attributes survive storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_A6_IntegerUserGUIDs)
{
  RunBasicIntegerUserGUID("BinOcaf");
}

// caf/basic/A7: a default and a user-GUID integer coexist after storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_A7_IntegerDefaultAndGUID)
{
  RunBasicIntegerDefaultAndUserGUID("BinOcaf");
}

// caf/basic/B3: a default TDataStd_Real survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_B3_Real)
{
  RunA2("BinOcaf");
}

// caf/basic/B6: two user-GUID TDataStd_Real attributes survive storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_B6_RealUserGUIDs)
{
  RunBasicRealUserGUID("BinOcaf");
}

// caf/basic/B7: a default and a user-GUID real coexist after storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_B7_RealDefaultAndGUID)
{
  RunBasicRealDefaultAndUserGUID("BinOcaf");
}

// caf/basic/C3: a default real array survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_C3_RealArray)
{
  RunA3("BinOcaf");
}

// caf/basic/C5: default and user-GUID real arrays survive undo/redo and storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_C5_RealArrayDefaultAndGUID)
{
  RunBasicRealArrayDefaultAndUserGUID("BinOcaf");
}

// caf/basic/D3: a default integer array survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_D3_IntegerArray)
{
  RunA4("BinOcaf");
}

// caf/basic/D6: default and user-GUID integer arrays survive undo/redo and storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_D6_IntegerArrayDefaultAndGUID)
{
  RunBasicIntegerArrayDefaultAndUserGUID("BinOcaf");
}

// caf/basic/E3: a default TDataStd_Name survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_E3_Name)
{
  RunA5("BinOcaf");
}

// caf/basic/E6: two user-GUID TDataStd_Name attributes survive storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_E6_NameUserGUIDs)
{
  RunBasicNameDefaultAndUserGUID("BinOcaf");
}

// caf/basic/E7: a default and a user-GUID name coexist after storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_E7_NameDefaultAndGUID)
{
  RunBasicNameDefaultAndUserGUIDPair("BinOcaf");
}

TEST(TDocStd_Storage_Test, Xml_CafBug_28425_NamedShapeOrder)
{
  RunBug28425("XmlOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_CafBug_28425_NamedShapeOrder)
{
  RunBug28425("BinOcaf");
}

TEST(TDocStd_Storage_Test, Xml_CafBug_29669_UserIntegerIds)
{
  RunBug29669("XmlOcaf");
}

TEST(TDocStd_Storage_Test, Cbf_CafBug_29669_UserIntegerIds)
{
  RunBug29669("BinOcaf");
}

// caf/basic/F3: a default TDataStd_Comment survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_F3_Comment)
{
  RunA6("BinOcaf");
}

// caf/basic/F4: empty attributes preserve deletion, undo/redo, and storage state.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_F4_EmptyAttributes)
{
  RunBasicEmptyAttributes("BinOcaf");
}

// caf/basic/G3: a TDataXtd_Point survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_G3_Point)
{
  RunA7("BinOcaf");
}

// caf/basic/H3: a TDataXtd_Axis survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_H3_Axis)
{
  RunA8("BinOcaf");
}

// caf/basic/I3: a TDataXtd_Plane survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_I3_Plane)
{
  RunA9("BinOcaf");
}

// caf/basic/J3: a TDataStd_UAttribute survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_J3_UserAttribute)
{
  RunB1("BinOcaf");
}

// caf/basic/J6: two TDataStd_UAttribute identifiers survive storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_J6_TwoUserAttributes)
{
  RunB2("BinOcaf");
}

// caf/basic/K3: a named box shape survives binary OCAF storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_K3_Shape)
{
  RunB3("BinOcaf");
}

// caf/basic/L3: a TDF_Reference resolves to the referenced label after storage.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_L3_Reference)
{
  RunBasicReference("BinOcaf");
}

// caf/bugs/A1: a binary OCAF stream preserves TDataStd_Integer.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_A1_IntegerStream)
{
  RunA1("BinOcaf", true);
}

// caf/bugs/A2: a binary OCAF stream preserves TDataStd_Real.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_A2_RealStream)
{
  RunA2("BinOcaf", true);
}

// caf/bugs/A3: a binary OCAF stream preserves TDataStd_RealArray bounds and values.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_A3_RealArrayStream)
{
  RunA3("BinOcaf", true);
}

// caf/bugs/A4: a binary OCAF stream preserves TDataStd_IntegerArray bounds and values.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_A4_IntegerArrayStream)
{
  RunA4("BinOcaf", true);
}

// caf/bugs/A5: a binary OCAF stream preserves TDataStd_Name.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_A5_NameStream)
{
  RunA5("BinOcaf", true);
}

// caf/bugs/A6: a binary OCAF stream preserves TDataStd_Comment.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_A6_CommentStream)
{
  RunA6("BinOcaf", true);
}

// caf/bugs/A7: a binary OCAF stream preserves TDataXtd_Point geometry.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_A7_PointStream)
{
  RunA7("BinOcaf", true);
}

// caf/bugs/A8: a binary OCAF stream preserves TDataXtd_Axis geometry.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_A8_AxisStream)
{
  RunA8("BinOcaf", true);
}

// caf/bugs/A9: a binary OCAF stream preserves TDataXtd_Plane geometry.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_A9_PlaneStream)
{
  RunA9("BinOcaf", true);
}

// caf/bugs/B1: a binary OCAF stream preserves one TDataStd_UAttribute identifier.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_B1_UserAttributeStream)
{
  RunB1("BinOcaf", true);
}

// caf/bugs/B2: a binary OCAF stream preserves two distinct UAttribute identifiers.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_B2_TwoUserAttributesStream)
{
  RunB2("BinOcaf", true);
}

// caf/bugs/B3: a binary OCAF stream preserves a named shape and its bounds.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_B3_NamedShapeStream)
{
  RunB3("BinOcaf", true);
}

// caf/bugs/B4: a binary OCAF stream preserves a TDF_Reference target.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_B4_ReferenceStream)
{
  RunBasicReference("BinOcaf", true);
}

// caf/bugs/B5: a binary OCAF stream preserves a named shape after a transaction.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_B5_NamedShapeAfterTransactionStream)
{
  RunB5("BinOcaf", true);
}

// caf/bugs/B7: a binary OCAF stream preserves a same-document CopyWithLink result.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_B7_CopyWithLinkStream)
{
  RunB7("BinOcaf", true);
}

// caf/bugs/B8: a binary OCAF stream preserves a TDF_TagSource child label and name.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_B8_TagSourceStream)
{
  RunB8("BinOcaf", true);
}

// caf/bugs/B9: a binary OCAF stream preserves a TDataStd_Directory and its name.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_B9_DirectoryStream)
{
  RunB9("BinOcaf", true);
}

// caf/bugs/C1: a binary OCAF stream preserves a root TDataStd_TreeNode.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_C1_TreeNodeRootStream)
{
  RunCafBugC1Stream();
}

// caf/bugs/C2: a binary OCAF stream preserves TDataStd_ExtStringArray values.
TEST(TDocStd_Storage_Test, Cbf_CafBugs_C2_ExtStringArrayStream)
{
  RunCafBugC2Stream();
}

// caf/basic/M3: a binary OCAF stream preserves a default extended-string array.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_M3_ExtStringArrayStream)
{
  RunCafBasicM3Stream();
}

// caf/basic/M6: binary OCAF preserves default and user-GUID extended-string arrays after undo.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_M6_ExtStringArrayStream)
{
  RunCafBasicM6Stream();
}

// caf/basic/N6: a binary OCAF stream preserves two user-GUID ASCII-string attributes.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_N6_AsciiStringStream)
{
  RunCafBasicN6Stream();
}

// caf/basic/N7: a binary OCAF stream preserves default and user-GUID ASCII strings.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_N7_AsciiStringStream)
{
  RunCafBasicN7Stream();
}

// caf/basic/O6: binary OCAF preserves default and user-GUID boolean arrays after undo.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_O6_BooleanArrayStream)
{
  RunCafBasicO6Stream();
}

// caf/basic/P6: binary OCAF preserves default and user-GUID byte arrays after undo.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_P6_ByteArrayStream)
{
  RunCafBasicP6Stream();
}

// caf/basic/Q6: binary OCAF preserves default and user-GUID reference arrays after undo.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_Q6_ReferenceArrayStream)
{
  RunCafBasicQ6Stream();
}

// caf/basic/R6: binary OCAF preserves default and user-GUID real lists after undo.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_R6_RealListStream)
{
  RunCafBasicR6Stream();
}

// caf/basic/S6: binary OCAF preserves default and user-GUID integer lists after undo.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_S6_IntegerListStream)
{
  RunCafBasicS6Stream();
}

// caf/basic/T6: binary OCAF preserves default and user-GUID extended-string lists after undo.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_T6_ExtStringListStream)
{
  RunCafBasicT6Stream();
}

// caf/basic/U6: binary OCAF preserves default and user-GUID boolean lists after undo.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_U6_BooleanListStream)
{
  RunCafBasicU6Stream();
}

// caf/basic/V6: binary OCAF preserves default and user-GUID reference lists after undo.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_V6_ReferenceListStream)
{
  RunCafBasicV6Stream();
}

// caf/tree/A3: a binary OCAF stream preserves a TDF_TagSource child name.
TEST(TDocStd_Storage_Test, Cbf_CafTree_A3_TagSourceStream)
{
  RunB8("BinOcaf", true);
}

// caf/tree/B3: a binary OCAF stream preserves a TDataStd_Directory name.
TEST(TDocStd_Storage_Test, Cbf_CafTree_B3_DirectoryStream)
{
  RunB9("BinOcaf", true);
}

// caf/tree/C2: a binary OCAF stream preserves a root TDataStd_TreeNode label.
TEST(TDocStd_Storage_Test, Cbf_CafTree_C2_TreeNodeRootStream)
{
  RunCafBugC1Stream();
}

// caf/named_shape/C1: a binary OCAF stream preserves a named shape after transactions.
TEST(TDocStd_Storage_Test, Cbf_CafNamedShape_C1_Stream)
{
  RunB5("BinOcaf", true);
}

// caf/xlink/B3: a binary OCAF stream preserves a same-document CopyWithLink shape.
TEST(TDocStd_Storage_Test, Cbf_CafXLink_B3_CopyWithLinkStream)
{
  RunB7("BinOcaf", true);
}

// caf/basic/N1: binary OCAF streams preserve two triangulation snapshots and undo/redo state.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_N1_TriangulationStream)
{
  RunCafBasicN1Stream();
}

// bugs/caf/bug24164_2: a binary OCAF stream preserves a six-label reference-array graph.
TEST(TDocStd_Storage_Test, Cbf_CafBug_24164_2_ReferenceArraysStream)
{
  RunCafBug24164_2Stream();
}

// bugs/caf/bug24164_1: a binary OCAF stream preserves a computed naming function.
TEST(TDocStd_Storage_Test, Cbf_CafBug_24164_1_FunctionStream)
{
  RunCafBug24164_1Stream();
}

// bugs/caf/bug158: an empty BinOcaf document can be stored through the stream API.
TEST(TDocStd_Storage_Test, Cbf_CafBug_158_EmptyDocumentStreamSave)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  std::stringstream      aStream;
  const PCDM_StoreStatus aStatus = anApplication->SaveAs(aDocument, aStream);
  EXPECT_EQ(aStatus, PCDM_SS_No_Obj);
  EXPECT_FALSE(aStream.str().empty());
  EXPECT_NO_THROW(anApplication->Close(aDocument));
}

// bugs/caf/bug267_1: an empty BinOcaf document can be stored through the
// stream API and produces the expected empty-document payload.
TEST(TDocStd_Storage_Test, Cbf_CafBug_267_1_EmptyDocumentStreamSave)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, "BinOcaf");
  ASSERT_FALSE(aDocument.IsNull());

  std::stringstream      aStream;
  const PCDM_StoreStatus aStatus = anApplication->SaveAs(aDocument, aStream);
  EXPECT_EQ(aStatus, PCDM_SS_No_Obj);
  EXPECT_FALSE(aStream.str().empty());
  EXPECT_NO_THROW(anApplication->Close(aDocument));
}

// bugs/caf/bug9746: naming selections and their integer-array arguments survive
// a BinOcaf stream round trip.
TEST(TDocStd_Storage_Test, Cbf_CafBug_9746_NamingSelectionsStream)
{
  RunCafBug9746Stream();
}

// bugs/caf/bug31839_1: BinOcaf stream filters support skipped attributes and append modes.
TEST(TDocStd_Storage_Test, Cbf_CafBug_31839_1_PartialStream)
{
  RunCafBug31839_1Stream();
}

// bugs/caf/bug31839_2: BinOcaf stream filters preserve shared topology in append/overwrite modes.
TEST(TDocStd_Storage_Test, Cbf_CafBug_31839_2_PartialShapeStream)
{
  RunCafBug31839_2Stream();
}

// bugs/caf/bug31918_1: BinLOcaf streams support fast subtree and attribute-filtered loading.
TEST(TDocStd_Storage_Test, Cbfl_CafBug_31918_1_PartialStream)
{
  RunCafBug31918_1Stream();
}

// caf/basic/Y1: BinOcaf streams preserve the document's empty-label saving mode.
TEST(TDocStd_Storage_Test, Cbf_CafBasic_Y1_SaveEmptyLabelsStream)
{
  RunCafBasicY1Stream();
}

// bugs/moddata_3/bug31136_5: StoreTriangulation updates the BinXCAF and
// BinOcaf storage-driver triangulation/normal switches consistently. The DRAW
// command is only a driver-setting command, so no document or file is needed.
TEST(TDocStd_Storage_Test, Cbf_CafBug_31136_5_StorageDriverOptions)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  BinDrivers::DefineFormat(anApplication);
  BinXCAFDrivers::DefineFormat(anApplication);

  const occ::handle<BinDrivers_DocumentStorageDriver> aXcafDriver =
    occ::down_cast<BinDrivers_DocumentStorageDriver>(anApplication->WriterFromFormat("BinXCAF"));
  const occ::handle<BinDrivers_DocumentStorageDriver> anOcafDriver =
    occ::down_cast<BinDrivers_DocumentStorageDriver>(anApplication->WriterFromFormat("BinOcaf"));
  ASSERT_FALSE(aXcafDriver.IsNull());
  ASSERT_FALSE(anOcafDriver.IsNull());

  aXcafDriver->SetWithTriangles(anApplication->MessageDriver(), true);
  anOcafDriver->SetWithTriangles(anApplication->MessageDriver(), true);
  aXcafDriver->SetWithNormals(anApplication->MessageDriver(), true);
  anOcafDriver->SetWithNormals(anApplication->MessageDriver(), true);
  EXPECT_TRUE(aXcafDriver->IsWithTriangles());
  EXPECT_TRUE(anOcafDriver->IsWithTriangles());
  EXPECT_TRUE(aXcafDriver->IsWithNormals());
  EXPECT_TRUE(anOcafDriver->IsWithNormals());

  aXcafDriver->SetWithNormals(anApplication->MessageDriver(), false);
  anOcafDriver->SetWithNormals(anApplication->MessageDriver(), false);
  EXPECT_FALSE(aXcafDriver->IsWithNormals());
  EXPECT_FALSE(anOcafDriver->IsWithNormals());

  aXcafDriver->SetWithTriangles(anApplication->MessageDriver(), false);
  anOcafDriver->SetWithTriangles(anApplication->MessageDriver(), false);
  EXPECT_FALSE(aXcafDriver->IsWithTriangles());
  EXPECT_FALSE(anOcafDriver->IsWithTriangles());
}
