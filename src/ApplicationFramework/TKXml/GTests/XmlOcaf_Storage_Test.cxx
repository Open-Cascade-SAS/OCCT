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

#include <XmlDrivers.hxx>
#include <XmlDrivers_DocumentRetrievalDriver.hxx>
#include <XmlDrivers_DocumentStorageDriver.hxx>

#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_NurbsConvertModification.hxx>
#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Bnd_Box.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <PCDM_ReaderStatus.hxx>
#include <PCDM_StoreStatus.hxx>
#include <Precision.hxx>
#include <Resource_Manager.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_Directory.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_IntegerList.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_NoteBook.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_RealList.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDataStd_Tick.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_Relation.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Variable.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Placement.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDataXtd_Triangulation.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_TagSource.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TFunction_Function.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <clocale>
#include <cstring>
#include <sstream>
#include <string>

namespace
{
static occ::handle<TDocStd_Application> NewApplication()
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  XmlDrivers::DefineFormat(anApplication);
  return anApplication;
}

static const char* THE_QATEST_DOC_FORMAT = "My Proprietary Format";

// The DRAW regression deliberately uses an application with an explicitly
// empty resource file to verify that the XML format can still be selected.
class CustomXmlApplication : public TDocStd_Application
{
public:
  CustomXmlApplication()
  {
    myResources = new Resource_Manager("");
    myResources->SetResource("xml.FileFormat", THE_QATEST_DOC_FORMAT);
    myResources->SetResource("My Proprietary Format.Description", "Test XML Document");
    myResources->SetResource("My Proprietary Format.FileExtension", "xml");
  }

  occ::handle<PCDM_Reader> ReaderFromFormat(const TCollection_ExtendedString&) override
  {
    return new XmlDrivers_DocumentRetrievalDriver();
  }

  occ::handle<PCDM_StorageDriver> WriterFromFormat(const TCollection_ExtendedString&) override
  {
    return new XmlDrivers_DocumentStorageDriver("Test");
  }

  const char* ResourcesName() override { return ""; }
};

static TDF_Label AttributeLabel(const occ::handle<TDocStd_Document>& theDocument)
{
  return theDocument->GetData()->Root().FindChild(2, true);
}

static TCollection_ExtendedString MakeTemporaryFile(OSD_Directory&       theDirectory,
                                                     const char*           theName,
                                                     OSD_Path&             thePath)
{
  theDirectory.Path(thePath);
  thePath.SetName(theName);
  thePath.SetExtension(".xml");

  TCollection_AsciiString aSystemName;
  thePath.SystemName(aSystemName);
  return TCollection_ExtendedString(aSystemName, true);
}

static occ::handle<TDocStd_Document> SaveAndOpen(
  const occ::handle<TDocStd_Application>& theApplication,
  const occ::handle<TDocStd_Document>&    theDocument,
  const TCollection_ExtendedString&       theFileName,
  const OSD_Path&                          theFilePath)
{
  EXPECT_EQ(theApplication->SaveAs(theDocument, theFileName), PCDM_SS_OK);

  OSD_File aFile(theFilePath);
  EXPECT_TRUE(aFile.Exists());

  theApplication->Close(theDocument);

  occ::handle<TDocStd_Document> aRestoredDocument;
  EXPECT_EQ(theApplication->Open(theFileName, aRestoredDocument), PCDM_RS_OK);
  EXPECT_FALSE(aRestoredDocument.IsNull());

  if (aFile.Exists())
  {
    aFile.Remove();
  }
  return aRestoredDocument;
}

static occ::handle<TDocStd_Document> NewDocument(
  const occ::handle<TDocStd_Application>& theApplication)
{
  occ::handle<TDocStd_Document> aDocument;
  theApplication->NewDocument("XmlOcaf", aDocument);
  EXPECT_FALSE(aDocument.IsNull());
  if (!aDocument.IsNull())
  {
    aDocument->SetUndoLimit(100);
  }
  return aDocument;
}

static occ::handle<TDocStd_Document> SaveNewDocument(
  const occ::handle<TDocStd_Application>& theApplication,
  const occ::handle<TDocStd_Document>&    theDocument,
  const char*                             theName,
  OSD_Directory&                          theDirectory,
  OSD_Path&                                thePath)
{
  const TCollection_ExtendedString aFileName =
    MakeTemporaryFile(theDirectory, theName, thePath);
  theDocument->NewCommand();
  return SaveAndOpen(theApplication, theDocument, aFileName, thePath);
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
  occ::handle<TDocStd_Document> aRestoredDocument;
  EXPECT_EQ(theApplication->Open(aStream, aRestoredDocument), PCDM_RS_OK);
  EXPECT_FALSE(aRestoredDocument.IsNull());
  return aRestoredDocument;
}

static void SetShape(const TDF_Label& theLabel, const TopoDS_Shape& theShape)
{
  TNaming_Builder aBuilder(theLabel);
  aBuilder.Generated(theShape);
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

static TopoDS_Shape MakeLargeTestShape(int& theShapeNb)
{
  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);

  occ::handle<BRepTools_NurbsConvertModification> aNurbsModification =
    new BRepTools_NurbsConvertModification;
  TopoDS_Shape aReferenceShape = BRepPrimAPI_MakeCylinder(50.0, 100.0).Solid();
  BRepTools_Modifier aModifier(aReferenceShape, aNurbsModification);
  if (aModifier.IsDone())
  {
    aReferenceShape = aModifier.ModifiedShape(aReferenceShape);
  }

  int aSiblingNb = 0;
  for (; theShapeNb > 0; --theShapeNb)
  {
    TopoDS_Shape aShape;
    if (++aSiblingNb <= 100)
    {
      aShape = BRepBuilderAPI_Copy(aReferenceShape, true).Shape();
    }
    else
    {
      aShape = MakeLargeTestShape(theShapeNb);
    }
    aBuilder.Add(aCompound, aShape);
  }
  return aCompound;
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

static TDF_Label FindOrCreatePath(const occ::handle<TDocStd_Document>& theDocument,
                                  const int*                           theTags,
                                  const int                            theNbTags)
{
  TDF_Label aLabel = theDocument->GetData()->Root();
  for (int anIndex = 0; anIndex < theNbTags; ++anIndex)
  {
    aLabel = aLabel.FindChild(theTags[anIndex], true);
  }
  return aLabel;
}

static void RestoreAttributesAfterUndo(const occ::handle<TDocStd_Document>& theDocument,
                                        const TDF_Label&                     theLabel)
{
  theDocument->NewCommand();
  theLabel.ForgetAllAttributes();
  theDocument->NewCommand();
  EXPECT_TRUE(theDocument->Undo());
  EXPECT_TRUE(theDocument->Redo());
  EXPECT_TRUE(theDocument->Undo());
}

static void AddEmptyAttribute(const TDF_Label& theLabel, const char* theTypeName)
{
  occ::handle<TDF_Attribute> anAttribute = TDF_DerivedAttribute::Attribute(theTypeName);
  ASSERT_FALSE(anAttribute.IsNull());
  if (!anAttribute.IsNull())
  {
    theLabel.AddAttribute(anAttribute);
  }
}

static bool HasAttributeType(const TDF_Label&                       theLabel,
                             const occ::handle<Standard_Type>& theType)
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

static void ExpectBooleanArray(const occ::handle<TDataStd_BooleanArray>& theArray,
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

static void ExpectRealArray(const occ::handle<TDataStd_RealArray>& theArray,
                            const double                           theFirst,
                            const double                           theSecond)
{
  ASSERT_FALSE(theArray.IsNull());
  EXPECT_EQ(theArray->Lower(), 1);
  EXPECT_EQ(theArray->Upper(), 2);
  EXPECT_EQ(theArray->Length(), 2);
  EXPECT_DOUBLE_EQ(theArray->Value(1), theFirst);
  EXPECT_DOUBLE_EQ(theArray->Value(2), theSecond);
}

static void ExpectIntegerArray(const occ::handle<TDataStd_IntegerArray>& theArray,
                               const int                                 theFirst,
                               const int                                 theSecond)
{
  ASSERT_FALSE(theArray.IsNull());
  EXPECT_EQ(theArray->Lower(), 1);
  EXPECT_EQ(theArray->Upper(), 2);
  EXPECT_EQ(theArray->Length(), 2);
  EXPECT_EQ(theArray->Value(1), theFirst);
  EXPECT_EQ(theArray->Value(2), theSecond);
}

static void ExpectByteArray(const occ::handle<TDataStd_ByteArray>& theArray,
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

static void ExpectExtendedArray(const occ::handle<TDataStd_ExtStringArray>& theArray,
                                const int                                    theLower,
                                const int                                    theUpper,
                                const int                                    theFirstIndex,
                                const TCollection_ExtendedString&             theFirst,
                                const TCollection_ExtendedString&             theSecond)
{
  ASSERT_FALSE(theArray.IsNull());
  EXPECT_EQ(theArray->Lower(), theLower);
  EXPECT_EQ(theArray->Upper(), theUpper);
  EXPECT_EQ(theArray->Length(), theUpper - theLower + 1);
  EXPECT_EQ(theArray->Value(theFirstIndex), theFirst);
  EXPECT_EQ(theArray->Value(theFirstIndex + 1), theSecond);
}

static void ExpectBooleanList(const occ::handle<TDataStd_BooleanList>& theList,
                              const bool                                theFirst,
                              const bool                                theSecond)
{
  ASSERT_FALSE(theList.IsNull());
  EXPECT_EQ(theList->Extent(), 2);
  EXPECT_EQ(theList->First(), theFirst);
  EXPECT_EQ(theList->Last(), theSecond);
}

static void ExpectExtendedList(const occ::handle<TDataStd_ExtStringList>& theList,
                               const TCollection_ExtendedString&           theFirst,
                               const TCollection_ExtendedString&           theSecond)
{
  ASSERT_FALSE(theList.IsNull());
  EXPECT_EQ(theList->Extent(), 2);
  EXPECT_EQ(theList->First(), theFirst);
  EXPECT_EQ(theList->Last(), theSecond);
}

static void ExpectIntegerList(const occ::handle<TDataStd_IntegerList>& theList,
                              const int                                theFirst,
                              const int                                theSecond)
{
  ASSERT_FALSE(theList.IsNull());
  EXPECT_EQ(theList->Extent(), 2);
  EXPECT_EQ(theList->First(), theFirst);
  EXPECT_EQ(theList->Last(), theSecond);
}

static void ExpectRealList(const occ::handle<TDataStd_RealList>& theList,
                           const double                          theFirst,
                           const double                          theSecond)
{
  ASSERT_FALSE(theList.IsNull());
  EXPECT_EQ(theList->Extent(), 2);
  EXPECT_DOUBLE_EQ(theList->First(), theFirst);
  EXPECT_DOUBLE_EQ(theList->Last(), theSecond);
}

static TCollection_AsciiString LabelEntry(const TDF_Label& theLabel)
{
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theLabel, anEntry);
  return anEntry;
}

static TDF_Label LabelByEntry(const occ::handle<TDocStd_Document>& theDocument,
                              const char*                           theEntry)
{
  TDF_Label aLabel;
  TDF_Tool::Label(theDocument->GetData(), TCollection_AsciiString(theEntry), aLabel);
  return aLabel;
}

static void ExpectReferenceArray(const occ::handle<TDataStd_ReferenceArray>& theArray,
                                 const TCollection_AsciiString&              theFirst,
                                 const TCollection_AsciiString&              theSecond)
{
  ASSERT_FALSE(theArray.IsNull());
  EXPECT_EQ(theArray->Lower(), 1);
  EXPECT_EQ(theArray->Upper(), 2);
  EXPECT_EQ(theArray->Length(), 2);
  EXPECT_EQ(LabelEntry(theArray->Value(1)), theFirst);
  EXPECT_EQ(LabelEntry(theArray->Value(2)), theSecond);
}

static void ExpectReferenceList(const occ::handle<TDataStd_ReferenceList>& theList,
                                const TCollection_AsciiString&             theFirst,
                                const TCollection_AsciiString&             theSecond)
{
  ASSERT_FALSE(theList.IsNull());
  EXPECT_EQ(theList->Extent(), 2);
  EXPECT_EQ(LabelEntry(theList->First()), theFirst);
  EXPECT_EQ(LabelEntry(theList->Last()), theSecond);
}

static occ::handle<Poly_Triangulation> MakeTestTriangulation(const bool theDense)
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

static void ExpectTriangulation(const TDF_Label&                         theLabel,
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
    EXPECT_TRUE(anAttribute->Node(anIndex).IsEqual(theExpected->Node(anIndex), Precision::Confusion()));
  }
  for (int anIndex = 1; anIndex <= theExpected->NbTriangles(); ++anIndex)
  {
    int anExpectedNode1 = 0;
    int anExpectedNode2 = 0;
    int anExpectedNode3 = 0;
    int anActualNode1 = 0;
    int anActualNode2 = 0;
    int anActualNode3 = 0;
    theExpected->Triangle(anIndex).Get(anExpectedNode1, anExpectedNode2, anExpectedNode3);
    anAttribute->Triangle(anIndex).Get(anActualNode1, anActualNode2, anActualNode3);
    EXPECT_EQ(anActualNode1, anExpectedNode1);
    EXPECT_EQ(anActualNode2, anExpectedNode2);
    EXPECT_EQ(anActualNode3, anExpectedNode3);
  }
}
} // namespace

// xml/data/ocaf/A1: TDataStd_Integer survives XML storage and retrieval.
TEST(XmlOcaf_Storage_Test, A1_Integer)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel, 100).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_integer", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_Integer> anInteger;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Integer::GetID(), anInteger));
  EXPECT_EQ(anInteger->Get(), 100);
}

// xml/data/ocaf/A2: TDataStd_Real survives XML storage and retrieval.
TEST(XmlOcaf_Storage_Test, A2_Real)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_Real::Set(aLabel, 100.0).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_real", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_Real> aReal;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_DOUBLE_EQ(aReal->Get(), 100.0);
}

// xml/data/ocaf/A3: a two-value TDataStd_RealArray keeps its bounds and values.
TEST(XmlOcaf_Storage_Test, A3_RealArray)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  occ::handle<TDataStd_RealArray> anArray = TDataStd_RealArray::Set(aLabel, 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_real_array", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_RealArray> aRestoredArray;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_RealArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 2);
  EXPECT_DOUBLE_EQ(aRestoredArray->Value(1), 3.0);
  EXPECT_DOUBLE_EQ(aRestoredArray->Value(2), 4.0);
}

// xml/data/ocaf/A4: a two-value TDataStd_IntegerArray keeps its bounds and values.
TEST(XmlOcaf_Storage_Test, A4_IntegerArray)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  occ::handle<TDataStd_IntegerArray> anArray = TDataStd_IntegerArray::Set(aLabel, 1, 2);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_integer_array", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_IntegerArray> aRestoredArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_IntegerArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 2);
  EXPECT_EQ(aRestoredArray->Value(1), 3);
  EXPECT_EQ(aRestoredArray->Value(2), 4);
}

// xml/data/ocaf/A5: a TDataStd_Name survives XML storage and retrieval.
TEST(XmlOcaf_Storage_Test, A5_Name)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TCollection_ExtendedString anExpected("New Attribute");
  ASSERT_FALSE(TDataStd_Name::Set(AttributeLabel(aDocument), anExpected).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_name", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), anExpected);
}

// bugs/demo/bug14673_4: XML storage preserves names in several Unicode scripts.
TEST(XmlOcaf_Storage_Test, DemoBug_14673_4_UnicodeNames)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const char* aNames[] = {"test", "l'épreuve", "опыт", "테스트", "größten 市"};
  for (int anIndex = 0; anIndex < 5; ++anIndex)
  {
    const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(anIndex + 1, true);
    ASSERT_FALSE(TDataStd_Name::Set(aLabel, TCollection_ExtendedString(aNames[anIndex], true))
                   .IsNull());
  }

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  for (int anIndex = 0; anIndex < 5; ++anIndex)
  {
    occ::handle<TDataStd_Name> aName;
    const TDF_Label aLabel = aRestored->GetData()->Root().FindChild(anIndex + 1, true);
    ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Name::GetID(), aName));
    EXPECT_EQ(aName->Get(), TCollection_ExtendedString(aNames[anIndex], true));
  }
}

// xml/data/ocaf/A6: a TDataStd_Comment survives XML storage and retrieval.
TEST(XmlOcaf_Storage_Test, A6_Comment)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TCollection_ExtendedString anExpected("New Attribute");
  ASSERT_FALSE(TDataStd_Comment::Set(AttributeLabel(aDocument), anExpected).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_comment", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_Comment> aComment;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Comment::GetID(), aComment));
  EXPECT_EQ(aComment->Get(), anExpected);
}

// xml/data/ocaf/A7: a TDataXtd_Point survives XML storage and retrieval.
TEST(XmlOcaf_Storage_Test, A7_Point)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const gp_Pnt anExpected(10.0, 20.0, 30.0);
  ASSERT_FALSE(TDataXtd_Point::Set(AttributeLabel(aDocument), anExpected).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_point", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  gp_Pnt aPoint;
  ASSERT_TRUE(TDataXtd_Geometry::Point(AttributeLabel(aRestored), aPoint));
  EXPECT_TRUE(aPoint.IsEqual(anExpected, Precision::Confusion()));
}

// xml/data/ocaf/A8: a TDataXtd_Axis survives XML storage and retrieval.
TEST(XmlOcaf_Storage_Test, A8_Axis)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const gp_Lin anExpected(gp_Pnt(10.0, 20.0, 30.0), gp_Dir(100.0, 200.0, 300.0));
  ASSERT_FALSE(TDataXtd_Axis::Set(AttributeLabel(aDocument), anExpected).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_axis", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  gp_Lin aLine;
  ASSERT_TRUE(TDataXtd_Geometry::Line(AttributeLabel(aRestored), aLine));
  EXPECT_TRUE(aLine.Location().IsEqual(anExpected.Location(), Precision::Confusion()));
  EXPECT_TRUE(aLine.Direction().IsEqual(anExpected.Direction(), Precision::Confusion()));
}

// xml/data/ocaf/A9: a TDataXtd_Plane survives XML storage and retrieval.
TEST(XmlOcaf_Storage_Test, A9_Plane)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const gp_Pln anExpected(gp_Pnt(10.0, 20.0, 30.0), gp_Dir(-1.0, 0.0, 0.0));
  ASSERT_FALSE(TDataXtd_Plane::Set(AttributeLabel(aDocument), anExpected).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_plane", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  gp_Pln aPlane;
  ASSERT_TRUE(TDataXtd_Geometry::Plane(AttributeLabel(aRestored), aPlane));
  EXPECT_TRUE(aPlane.Location().IsEqual(anExpected.Location(), Precision::Confusion()));
  EXPECT_TRUE(
    aPlane.Axis().Direction().IsEqual(anExpected.Axis().Direction(), Precision::Confusion()));
}

// xml/data/ocaf/B1: a user attribute survives XML storage and retrieval.
TEST(XmlOcaf_Storage_Test, B1_UserAttribute)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const Standard_GUID aGuid("c73bd075-22ee-11d2-acde-080009dc4422");
  ASSERT_FALSE(TDataStd_UAttribute::Set(AttributeLabel(aDocument), aGuid).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_user_attribute", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_UAttribute> anAttribute;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, anAttribute));
  EXPECT_EQ(anAttribute->ID(), aGuid);
}

// xml/data/ocaf/B2: two user attributes with different GUIDs remain independent.
TEST(XmlOcaf_Storage_Test, B2_TwoUserAttributes)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const Standard_GUID aGuid1("c73bd075-22ee-11d2-acde-080009dc4422");
  const Standard_GUID aGuid2("c73bd076-22ee-11d2-acde-080009dc4422");
  const TDF_Label     aLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_UAttribute::Set(aLabel, aGuid1).IsNull());
  ASSERT_FALSE(TDataStd_UAttribute::Set(aLabel, aGuid2).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_two_user_attributes", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_UAttribute> anAttribute1;
  occ::handle<TDataStd_UAttribute> anAttribute2;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid1, anAttribute1));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid2, anAttribute2));
  EXPECT_NE(anAttribute1->ID(), anAttribute2->ID());
}

// xml/data/ocaf/B3: a TNaming_NamedShape box survives XML storage and retrieval.
TEST(XmlOcaf_Storage_Test, B3_NamedShape)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  SetShape(aLabel, BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), gp_Pnt(110.0, 220.0, 330.0)));

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_named_shape", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  ExpectBoxBounds(ShapeOf(AttributeLabel(aRestored)), 10.0, 20.0, 30.0, 110.0, 220.0, 330.0);
}

// xml/data/ocaf/B4: a chain of label references and referenced integer values survives storage.
TEST(XmlOcaf_Storage_Test, B4_ReferenceChain)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const int aTags[][6] = {{3},
                          {2076534922, 524398634, 912349856},
                          {3, 9283, 12, 1, 9843, 46793},
                          {4, 81245034, 321, 1, 1, 1},
                          {5, 8, 4, 2, 213, 3242},
                          {2, 15, 123, 31214, 452398, 421},
                          {2, 2, 1, 1, 1, 3},
                          {2, 9}};
  const int aTagLengths[] = {1, 3, 6, 6, 6, 6, 6, 2};
  NCollection_Array1<TDF_Label> aLabels(1, 8);
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

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_reference_chain", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  for (int anIndex = 1; anIndex <= 8; ++anIndex)
  {
    TDF_Label aRestoredLabel;
    TDF_Tool::Label(aRestored->GetData(), aEntries.Value(anIndex), aRestoredLabel);
    occ::handle<TDF_Reference> aReference;
    ASSERT_TRUE(aRestoredLabel.FindAttribute(TDF_Reference::GetID(), aReference));
    const int anExpectedIndex = anIndex == 1 ? 8 : anIndex - 1;
    TCollection_AsciiString anActualEntry;
    TDF_Tool::Entry(aReference->Get(), anActualEntry);
    EXPECT_STREQ(anActualEntry.ToCString(), aEntries.Value(anExpectedIndex).ToCString());

    occ::handle<TDataStd_Integer> anInteger;
    ASSERT_TRUE(aReference->Get().FindAttribute(TDataStd_Integer::GetID(), anInteger));
    EXPECT_EQ(anInteger->Get(), anExpectedIndex);
  }
}

// xml/data/ocaf/B5: a named shape created in a later transaction survives storage.
TEST(XmlOcaf_Storage_Test, B5_NamedShapeAfterTransaction)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  aDocument->NewCommand();
  SetShape(aLabel, BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), gp_Pnt(110.0, 220.0, 330.0)));

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_named_shape_transaction", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  ExpectBoxBounds(ShapeOf(AttributeLabel(aRestored)), 10.0, 20.0, 30.0, 110.0, 220.0, 330.0);
}

// xml/data/ocaf/B8: the label tag source continues after its child labels are restored.
TEST(XmlOcaf_Storage_Test, B8_TagSource)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_Name::Set(aLabel, TCollection_ExtendedString("Label1")).IsNull());
  const TDF_Label aChild = TDF_TagSource::NewChild(aLabel);
  ASSERT_FALSE(TDataStd_Name::Set(aChild, TCollection_ExtendedString("Label11")).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_tag_source", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDF_TagSource> aTagSource;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDF_TagSource::GetID(), aTagSource));
  EXPECT_EQ(aTagSource->NewTag(), 2);
}

// xml/data/ocaf/B9: a directory label and its name survive XML storage.
TEST(XmlOcaf_Storage_Test, B9_Directory)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_Directory::New(aLabel).IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aLabel, TCollection_ExtendedString("Label1")).IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_directory", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_Directory> aRestoredDirectory;
  ASSERT_TRUE(TDataStd_Directory::Find(AttributeLabel(aRestored), aRestoredDirectory));
  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), TCollection_ExtendedString("Label1"));
}

// xml/data/ocaf/C1: child TDataStd_TreeNode order survives XML storage.
TEST(XmlOcaf_Storage_Test, C1_TreeNodeChildren)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aRootLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_Name::Set(aRootLabel, TCollection_ExtendedString("Label_1")).IsNull());
  occ::handle<TDataStd_TreeNode> aRootNode = TDataStd_TreeNode::Set(aRootLabel);
  const TDF_Label                aChildLabel1 = aDocument->GetData()->Root().FindChild(7, true);
  const TDF_Label                aChildLabel2 = aDocument->GetData()->Root().FindChild(8, true);
  const TDF_Label                aChildLabel3 = aDocument->GetData()->Root().FindChild(9, true);
  occ::handle<TDataStd_TreeNode> aChildNode1 = TDataStd_TreeNode::Set(aChildLabel1);
  occ::handle<TDataStd_TreeNode> aChildNode2 = TDataStd_TreeNode::Set(aChildLabel2);
  occ::handle<TDataStd_TreeNode> aChildNode3 = TDataStd_TreeNode::Set(aChildLabel3);
  ASSERT_TRUE(aRootNode->Append(aChildNode1));
  ASSERT_TRUE(aRootNode->Append(aChildNode2));
  ASSERT_TRUE(aRootNode->Append(aChildNode3));

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_tree_nodes", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_TreeNode> aRestoredRoot = TDataStd_TreeNode::Set(AttributeLabel(aRestored));
  ASSERT_FALSE(aRestoredRoot.IsNull());
  EXPECT_EQ(aRestoredRoot->NbChildren(false), 3);
  const int aExpectedTags[] = {7, 8, 9};
  TDataStd_ChildNodeIterator anIterator(aRestoredRoot, false);
  for (int anIndex = 0; anIterator.More() && anIndex < 3; anIterator.Next(), ++anIndex)
  {
    EXPECT_EQ(anIterator.Value()->Label().Tag(), aExpectedTags[anIndex]);
  }
  EXPECT_FALSE(anIterator.More());
}

// xml/data/ocaf/C8: a relation preserves its expression and variable references.
TEST(XmlOcaf_Storage_Test, C8_Relation)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aVariableLabel1 = aDocument->GetData()->Root().FindChild(1, true);
  const TDF_Label aVariableLabel2 = aVariableLabel1.FindChild(2, true);
  const TDF_Label aVariableLabel3 = aVariableLabel1.FindChild(3, true);
  const TDF_Label aRelationLabel  = aVariableLabel1.FindChild(13, true);
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
  NCollection_Array1<TCollection_AsciiString> aVariableEntries(1, 3);
  TDF_Tool::Entry(aVariableLabel1, aVariableEntries.ChangeValue(1));
  TDF_Tool::Entry(aVariableLabel2, aVariableEntries.ChangeValue(2));
  TDF_Tool::Entry(aVariableLabel3, aVariableEntries.ChangeValue(3));

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_relation", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  TDF_Label aRestoredRelationLabel;
  TDF_Tool::Label(aRestored->GetData(), aRelationEntry, aRestoredRelationLabel);
  occ::handle<TDataStd_Relation> aRestoredRelation;
  ASSERT_TRUE(
    aRestoredRelationLabel.FindAttribute(TDataStd_Relation::GetID(), aRestoredRelation));
  EXPECT_EQ(aRestoredRelation->GetRelation(), TCollection_ExtendedString("f = m*a"));

  int anIndex = 1;
  NCollection_List<occ::handle<TDF_Attribute>>::Iterator anIterator;
  for (anIterator.Initialize(aRestoredRelation->GetVariables()); anIterator.More();
       anIterator.Next(), ++anIndex)
  {
    ASSERT_LE(anIndex, 3);
    TCollection_AsciiString anActualEntry;
    TDF_Tool::Entry(anIterator.Value()->Label(), anActualEntry);
    EXPECT_STREQ(anActualEntry.ToCString(), aVariableEntries.Value(anIndex).ToCString());
  }
  EXPECT_EQ(anIndex, 4);
}

// xml/data/ocaf/C9: a variable preserves its constant flag and unit.
TEST(XmlOcaf_Storage_Test, C9_Variable)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(1, true).FindChild(1, true);
  occ::handle<TDataStd_Variable> aVariable = TDataStd_Variable::Set(aLabel);
  ASSERT_FALSE(aVariable.IsNull());
  aVariable->Constant(true);
  aVariable->Unit("kg/m3");

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_variable", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  TDF_Label aRestoredLabel;
  TDF_Tool::Label(aRestored->GetData(), "0:1:1", aRestoredLabel);
  occ::handle<TDataStd_Variable> aRestoredVariable;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_Variable::GetID(), aRestoredVariable));
  EXPECT_TRUE(aRestoredVariable->IsConstant());
  EXPECT_EQ(aRestoredVariable->Unit(), TCollection_AsciiString("kg/m3"));
}

// xml/data/ocaf/C6: a TFunction_Function driver GUID and failure index survive storage.
TEST(XmlOcaf_Storage_Test, C6_Function)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const Standard_GUID aDriverGuid("5b35ca00-5b78-11d1-8940-080009dc3333");
  const TDF_Label     aLabel = aDocument->GetData()->Root().FindChild(1, true).FindChild(1, true);
  occ::handle<TFunction_Function> aFunction = TFunction_Function::Set(aLabel, aDriverGuid);
  ASSERT_FALSE(aFunction.IsNull());
  aFunction->SetFailure(13);

  TCollection_AsciiString aLabelEntry;
  TDF_Tool::Entry(aLabel, aLabelEntry);
  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_function", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());

  TDF_Label aRestoredLabel;
  TDF_Tool::Label(aRestored->GetData(), aLabelEntry, aRestoredLabel);
  occ::handle<TFunction_Function> aRestoredFunction;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TFunction_Function::GetID(), aRestoredFunction));
  EXPECT_EQ(aRestoredFunction->GetDriverGUID(), aDriverGuid);
  EXPECT_EQ(aRestoredFunction->GetFailure(), 13);
}

// xml/data/ocaf/D1: XML preserves names containing quotes and angle brackets.
TEST(XmlOcaf_Storage_Test, D1_SpecialNames)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const char* const                aNames[] = {
    "Tu sais je n'ai jamais t aussi hereux que ce matin-l",
    "\"Tu sais je n'ai jamais t aussi hereux que ce matin-l\"",
    "<Tu sais je n'ai jamais t aussi hereux que ce matin-l>",
    "Tu m'as dit \"J'ai rendez-vous dans un sous-sol avec des fous",
    "\"Tu m'as dit \"J'ai rendez-vous dans un sous-sol avec des fous\"",
    "Il <n'avait plus rien cr> dans ce monde triste"};
  for (int anIndex = 0; anIndex < 6; ++anIndex)
  {
    const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(11 + anIndex, true);
    ASSERT_FALSE(TDataStd_Name::Set(aLabel, TCollection_ExtendedString(aNames[anIndex])).IsNull());
  }

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_special_names", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  for (int anIndex = 0; anIndex < 6; ++anIndex)
  {
    const TDF_Label aLabel = aRestored->GetData()->Root().FindChild(11 + anIndex, true);
    occ::handle<TDataStd_Name> aName;
    ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Name::GetID(), aName));
    EXPECT_EQ(aName->Get(), TCollection_ExtendedString(aNames[anIndex]));
  }
}

// xml/data/ocaf/D4: a one-element integer array survives XML storage.
TEST(XmlOcaf_Storage_Test, D4_SingleIntegerArray)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<TDataStd_IntegerArray> anArray =
    TDataStd_IntegerArray::Set(AttributeLabel(aDocument), 1, 1);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, 3);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_single_integer_array", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_IntegerArray> aRestoredArray;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_IntegerArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 1);
  EXPECT_EQ(aRestoredArray->Value(1), 3);
}

// xml/data/ocaf/D5: a one-element real array survives XML storage.
TEST(XmlOcaf_Storage_Test, D5_SingleRealArray)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<TDataStd_RealArray> anArray =
    TDataStd_RealArray::Set(AttributeLabel(aDocument), 1, 1);
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(1, 3.0);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_single_real_array", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_RealArray> aRestoredArray;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_RealArray::GetID(), aRestoredArray));
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), 1);
  EXPECT_DOUBLE_EQ(aRestoredArray->Value(1), 3.0);
}

// xml/data/ocaf/D6: the OCC16497 leading-zero name regression remains fixed.
TEST(XmlOcaf_Storage_Test, D6_LeadingZeroName)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(AttributeLabel(aDocument), TCollection_ExtendedString("00"))
                 .IsNull());

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_ocaf_leading_zero_name", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), TCollection_ExtendedString("00"));
}

// caf/basic/C6: XML preserves default and user-GUID real arrays after undo/redo.
TEST(XmlOcaf_Storage_Test, CafBasic_C6_RealArrayPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const Standard_GUID              aGuid("12e94511-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_RealArray>  anArray = TDataStd_RealArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_RealArray>  aGuidArray = TDataStd_RealArray::Set(aLabel, aGuid, 1, 2);
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);
  aGuidArray->SetValue(1, 3.0);
  aGuidArray->SetValue(2, 4.0);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_real_array", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_RealArray> aRestoredArray;
  occ::handle<TDataStd_RealArray> aRestoredGuidArray;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_RealArray::GetID(), aRestoredArray));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidArray));
  ExpectRealArray(aRestoredArray, 3.0, 4.0);
  ExpectRealArray(aRestoredGuidArray, 3.0, 4.0);
  EXPECT_EQ(aRestoredGuidArray->ID(), aGuid);
}

// caf/basic/D7: XML preserves default and user-GUID integer arrays after undo/redo.
TEST(XmlOcaf_Storage_Test, CafBasic_D7_IntegerArrayPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const Standard_GUID              aGuid("12e94511-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_IntegerArray> anArray = TDataStd_IntegerArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_IntegerArray> aGuidArray =
    TDataStd_IntegerArray::Set(aLabel, aGuid, 1, 2);
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);
  aGuidArray->SetValue(1, 3);
  aGuidArray->SetValue(2, 4);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_integer_array", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_IntegerArray> aRestoredArray;
  occ::handle<TDataStd_IntegerArray> aRestoredGuidArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_IntegerArray::GetID(), aRestoredArray));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidArray));
  ExpectIntegerArray(aRestoredArray, 3, 4);
  ExpectIntegerArray(aRestoredGuidArray, 3, 4);
  EXPECT_EQ(aRestoredGuidArray->ID(), aGuid);
}

// caf/basic/M7: XML preserves default and user-GUID extended-string arrays.
TEST(XmlOcaf_Storage_Test, CafBasic_M7_ExtendedStringArrayPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const Standard_GUID              aGuid("12e94515-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_ExtStringArray> anArray =
    TDataStd_ExtStringArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_ExtStringArray> aGuidArray =
    TDataStd_ExtStringArray::Set(aLabel, aGuid, 1, 2);
  anArray->SetValue(1, TCollection_ExtendedString("xxxxx"));
  anArray->SetValue(2, TCollection_ExtendedString("zzzzzzz"));
  aGuidArray->SetValue(1, TCollection_ExtendedString("xxxxx"));
  aGuidArray->SetValue(2, TCollection_ExtendedString("zzzzzzz"));
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored = SaveNewDocument(
    anApplication, aDocument, "draw_caf_extended_string_array", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ExtStringArray> aRestoredArray;
  occ::handle<TDataStd_ExtStringArray> aRestoredGuidArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_ExtStringArray::GetID(), aRestoredArray));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidArray));
  ExpectExtendedArray(aRestoredArray,
                      1,
                      2,
                      1,
                      TCollection_ExtendedString("xxxxx"),
                      TCollection_ExtendedString("zzzzzzz"));
  ExpectExtendedArray(aRestoredGuidArray,
                      1,
                      2,
                      1,
                      TCollection_ExtendedString("xxxxx"),
                      TCollection_ExtendedString("zzzzzzz"));
}

// caf/basic/O7: XML preserves default and user-GUID boolean arrays.
TEST(XmlOcaf_Storage_Test, CafBasic_O7_BooleanArrayPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const Standard_GUID              aGuid("12e94516-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_BooleanArray> anArray = TDataStd_BooleanArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_BooleanArray> aGuidArray =
    TDataStd_BooleanArray::Set(aLabel, aGuid, 1, 2);
  anArray->SetValue(1, false);
  anArray->SetValue(2, true);
  aGuidArray->SetValue(1, false);
  aGuidArray->SetValue(2, true);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_boolean_array", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_BooleanArray> aRestoredArray;
  occ::handle<TDataStd_BooleanArray> aRestoredGuidArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_BooleanArray::GetID(), aRestoredArray));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidArray));
  ExpectBooleanArray(aRestoredArray, false, true);
  ExpectBooleanArray(aRestoredGuidArray, false, true);
}

// caf/basic/P7: XML preserves default and user-GUID byte arrays.
TEST(XmlOcaf_Storage_Test, CafBasic_P7_ByteArrayPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const Standard_GUID              aGuid("12e94517-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_ByteArray>  anArray = TDataStd_ByteArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_ByteArray>  aGuidArray = TDataStd_ByteArray::Set(aLabel, aGuid, 1, 2);
  anArray->SetValue(1, 10);
  anArray->SetValue(2, 12);
  aGuidArray->SetValue(1, 10);
  aGuidArray->SetValue(2, 12);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_byte_array", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ByteArray> aRestoredArray;
  occ::handle<TDataStd_ByteArray> aRestoredGuidArray;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_ByteArray::GetID(), aRestoredArray));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidArray));
  ExpectByteArray(aRestoredArray, 10, 12);
  ExpectByteArray(aRestoredGuidArray, 10, 12);
}

// caf/basic/Q7: XML preserves default and user-GUID reference arrays.
TEST(XmlOcaf_Storage_Test, CafBasic_Q7_ReferenceArrayPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const TDF_Label                  aReference1   = aDocument->GetData()->Root().FindChild(3, true);
  const TDF_Label                  aReference2   = aDocument->GetData()->Root().FindChild(4, true);
  const TCollection_AsciiString    anEntry1     = LabelEntry(aReference1);
  const TCollection_AsciiString    anEntry2     = LabelEntry(aReference2);
  const Standard_GUID              aGuid("12e94518-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_ReferenceArray> anArray =
    TDataStd_ReferenceArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_ReferenceArray> aGuidArray =
    TDataStd_ReferenceArray::Set(aLabel, aGuid, 1, 2);
  anArray->SetValue(1, aReference1);
  anArray->SetValue(2, aReference2);
  aGuidArray->SetValue(1, aReference1);
  aGuidArray->SetValue(2, aReference2);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_reference_array", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ReferenceArray> aRestoredArray;
  occ::handle<TDataStd_ReferenceArray> aRestoredGuidArray;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_ReferenceArray::GetID(), aRestoredArray));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidArray));
  ExpectReferenceArray(aRestoredArray, anEntry1, anEntry2);
  ExpectReferenceArray(aRestoredGuidArray, anEntry1, anEntry2);
}

// caf/basic/R7: XML preserves default and user-GUID real lists.
TEST(XmlOcaf_Storage_Test, CafBasic_R7_RealListPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const Standard_GUID              aGuid("12e94521-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_RealList>   aList = TDataStd_RealList::Set(aLabel);
  occ::handle<TDataStd_RealList>   aGuidList = TDataStd_RealList::Set(aLabel, aGuid);
  aList->Append(3.0);
  aList->Append(4.0);
  aGuidList->Append(3.0);
  aGuidList->Append(4.0);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_real_list", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_RealList> aRestoredList;
  occ::handle<TDataStd_RealList> aRestoredGuidList;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_RealList::GetID(), aRestoredList));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidList));
  ExpectRealList(aRestoredList, 3.0, 4.0);
  ExpectRealList(aRestoredGuidList, 3.0, 4.0);
}

// caf/basic/S7: XML preserves default and user-GUID integer lists.
TEST(XmlOcaf_Storage_Test, CafBasic_S7_IntegerListPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const Standard_GUID              aGuid("12e94531-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_IntegerList> aList = TDataStd_IntegerList::Set(aLabel);
  occ::handle<TDataStd_IntegerList> aGuidList = TDataStd_IntegerList::Set(aLabel, aGuid);
  aList->Append(33);
  aList->Append(44);
  aGuidList->Append(33);
  aGuidList->Append(44);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_integer_list", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_IntegerList> aRestoredList;
  occ::handle<TDataStd_IntegerList> aRestoredGuidList;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_IntegerList::GetID(), aRestoredList));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidList));
  ExpectIntegerList(aRestoredList, 33, 44);
  ExpectIntegerList(aRestoredGuidList, 33, 44);
}

// caf/basic/T7: XML preserves default and user-GUID extended-string lists.
TEST(XmlOcaf_Storage_Test, CafBasic_T7_ExtendedStringListPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const Standard_GUID              aGuid("12e94541-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_ExtStringList> aList = TDataStd_ExtStringList::Set(aLabel);
  occ::handle<TDataStd_ExtStringList> aGuidList = TDataStd_ExtStringList::Set(aLabel, aGuid);
  aList->Append(TCollection_ExtendedString("xxxx"));
  aList->Append(TCollection_ExtendedString("yyyyy"));
  aGuidList->Append(TCollection_ExtendedString("xxxx"));
  aGuidList->Append(TCollection_ExtendedString("yyyyy"));
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored = SaveNewDocument(
    anApplication, aDocument, "draw_caf_extended_string_list", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ExtStringList> aRestoredList;
  occ::handle<TDataStd_ExtStringList> aRestoredGuidList;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_ExtStringList::GetID(), aRestoredList));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidList));
  ExpectExtendedList(aRestoredList,
                     TCollection_ExtendedString("xxxx"),
                     TCollection_ExtendedString("yyyyy"));
  ExpectExtendedList(aRestoredGuidList,
                     TCollection_ExtendedString("xxxx"),
                     TCollection_ExtendedString("yyyyy"));
}

// caf/basic/U7: XML preserves default and user-GUID boolean lists.
TEST(XmlOcaf_Storage_Test, CafBasic_U7_BooleanListPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const Standard_GUID              aGuid("12e94551-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_BooleanList> aList = TDataStd_BooleanList::Set(aLabel);
  occ::handle<TDataStd_BooleanList> aGuidList = TDataStd_BooleanList::Set(aLabel, aGuid);
  aList->Append(false);
  aList->Append(true);
  aGuidList->Append(false);
  aGuidList->Append(true);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_boolean_list", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_BooleanList> aRestoredList;
  occ::handle<TDataStd_BooleanList> aRestoredGuidList;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_BooleanList::GetID(), aRestoredList));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidList));
  ExpectBooleanList(aRestoredList, false, true);
  ExpectBooleanList(aRestoredGuidList, false, true);
}

// caf/basic/V7: XML preserves default and user-GUID reference lists.
TEST(XmlOcaf_Storage_Test, CafBasic_V7_ReferenceListPersistence)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  const TDF_Label                  aReference1   = aDocument->GetData()->Root().FindChild(3, true);
  const TDF_Label                  aReference2   = aDocument->GetData()->Root().FindChild(4, true);
  const TCollection_AsciiString    anEntry1     = LabelEntry(aReference1);
  const TCollection_AsciiString    anEntry2     = LabelEntry(aReference2);
  const Standard_GUID              aGuid("12e94561-6dbc-11d4-b9c8-0060b0ee281b");
  aDocument->NewCommand();
  occ::handle<TDataStd_ReferenceList> aList = TDataStd_ReferenceList::Set(aLabel);
  occ::handle<TDataStd_ReferenceList> aGuidList = TDataStd_ReferenceList::Set(aLabel, aGuid);
  aList->Append(aReference1);
  aList->Append(aReference2);
  aGuidList->Append(aReference1);
  aGuidList->Append(aReference2);
  RestoreAttributesAfterUndo(aDocument, aLabel);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_reference_list", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_ReferenceList> aRestoredList;
  occ::handle<TDataStd_ReferenceList> aRestoredGuidList;
  ASSERT_TRUE(
    AttributeLabel(aRestored).FindAttribute(TDataStd_ReferenceList::GetID(), aRestoredList));
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(aGuid, aRestoredGuidList));
  ExpectReferenceList(aRestoredList, anEntry1, anEntry2);
  ExpectReferenceList(aRestoredGuidList, anEntry1, anEntry2);
}

// caf/basic/M4: XML preserves separators, empty strings, and empty arrays.
TEST(XmlOcaf_Storage_Test, CafBasic_M4_ExtendedStringArrayEdgeCases)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel1       = aDocument->GetData()->Root().FindChild(1, true);
  const TDF_Label                  aLabel2       = aDocument->GetData()->Root().FindChild(2, true);
  const TDF_Label                  aLabel3       = aDocument->GetData()->Root().FindChild(3, true);
  const TCollection_ExtendedString anEmpty;
  occ::handle<TDataStd_ExtStringArray> anArray1 =
    TDataStd_ExtStringArray::Set(aLabel1, 0, 6);
  const char* const aValues[] = {"Hello 1", "Hello_2", "Hello*3", "Hello-4", "Hello5", ""};
  for (int anIndex = 1; anIndex <= 6; ++anIndex)
  {
    anArray1->SetValue(anIndex, TCollection_ExtendedString(aValues[anIndex - 1]));
  }
  occ::handle<TDataStd_ExtStringArray> anArray2 =
    TDataStd_ExtStringArray::Set(aLabel2, 0, 3);
  anArray2->SetValue(0, anEmpty);
  anArray2->SetValue(1, TCollection_ExtendedString("H"));
  anArray2->SetValue(2, anEmpty);
  anArray2->SetValue(3, anEmpty);
  occ::handle<TDataStd_ExtStringArray> anArray3 =
    TDataStd_ExtStringArray::Set(aLabel3, 0, 0);
  anArray3->SetValue(0, anEmpty);
  occ::handle<TDataStd_TreeNode> aNode1 = TDataStd_TreeNode::Set(aLabel1);
  occ::handle<TDataStd_TreeNode> aNode2 = TDataStd_TreeNode::Set(aLabel2);
  occ::handle<TDataStd_TreeNode> aNode3 = TDataStd_TreeNode::Set(aLabel3);
  ASSERT_TRUE(aNode1->Append(aNode2));
  ASSERT_TRUE(aNode1->Append(aNode3));

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_extended_array_edges", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel1 = aRestored->GetData()->Root().FindChild(1, false);
  const TDF_Label aRestoredLabel2 = aRestored->GetData()->Root().FindChild(2, false);
  const TDF_Label aRestoredLabel3 = aRestored->GetData()->Root().FindChild(3, false);
  occ::handle<TDataStd_ExtStringArray> aRestoredArray1;
  occ::handle<TDataStd_ExtStringArray> aRestoredArray2;
  occ::handle<TDataStd_ExtStringArray> aRestoredArray3;
  ASSERT_TRUE(aRestoredLabel1.FindAttribute(TDataStd_ExtStringArray::GetID(), aRestoredArray1));
  ASSERT_TRUE(aRestoredLabel2.FindAttribute(TDataStd_ExtStringArray::GetID(), aRestoredArray2));
  ASSERT_TRUE(aRestoredLabel3.FindAttribute(TDataStd_ExtStringArray::GetID(), aRestoredArray3));
  EXPECT_EQ(aRestoredArray1->Lower(), 0);
  EXPECT_EQ(aRestoredArray1->Upper(), 6);
  for (int anIndex = 1; anIndex <= 6; ++anIndex)
  {
    EXPECT_EQ(aRestoredArray1->Value(anIndex), TCollection_ExtendedString(aValues[anIndex - 1]));
  }
  EXPECT_EQ(aRestoredArray2->Lower(), 0);
  EXPECT_EQ(aRestoredArray2->Upper(), 3);
  EXPECT_EQ(aRestoredArray2->Value(0), anEmpty);
  EXPECT_EQ(aRestoredArray2->Value(1), TCollection_ExtendedString("H"));
  EXPECT_EQ(aRestoredArray2->Value(2), anEmpty);
  EXPECT_EQ(aRestoredArray2->Value(3), anEmpty);
  EXPECT_EQ(aRestoredArray3->Lower(), 0);
  EXPECT_EQ(aRestoredArray3->Upper(), 0);
  EXPECT_EQ(aRestoredArray3->Value(0), anEmpty);
}

// caf/basic/F5: all registered empty attributes survive XML storage.
TEST(XmlOcaf_Storage_Test, CafBasic_F5_EmptyAttributes)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aLabel        = AttributeLabel(aDocument);
  aDocument->NewCommand();
  AddEmptyAttribute(aLabel, "TDataXtd_Axis");
  AddEmptyAttribute(aLabel, "TDataXtd_Placement");
  AddEmptyAttribute(aLabel, "TDataXtd_Plane");
  AddEmptyAttribute(aLabel, "TDataXtd_Point");
  AddEmptyAttribute(aLabel, "TDataXtd_Shape");
  AddEmptyAttribute(aLabel, "TDataStd_Tick");
  AddEmptyAttribute(aLabel, "TDataStd_Directory");
  AddEmptyAttribute(aLabel, "TDataStd_NoteBook");
  aDocument->CommitCommand();

  auto NbAttributes = [](const TDF_Label& theLabel) {
    int                   aCount = 0;
    TDF_AttributeIterator anIterator(theLabel);
    for (; anIterator.More(); anIterator.Next())
    {
      ++aCount;
    }
    return aCount;
  };
  EXPECT_EQ(NbAttributes(aLabel), 8);
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataXtd_Axis)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataXtd_Placement)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataXtd_Plane)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataXtd_Point)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataXtd_Shape)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataStd_Tick)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataStd_Directory)));
  EXPECT_TRUE(HasAttributeType(aLabel, STANDARD_TYPE(TDataStd_NoteBook)));
  ASSERT_TRUE(aDocument->Undo());
  EXPECT_EQ(NbAttributes(aLabel), 0);
  ASSERT_TRUE(aDocument->Redo());
  EXPECT_EQ(NbAttributes(aLabel), 8);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_empty_attributes", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  EXPECT_EQ(NbAttributes(AttributeLabel(aRestored)), 8);
  EXPECT_TRUE(HasAttributeType(AttributeLabel(aRestored), STANDARD_TYPE(TDataXtd_Axis)));
  EXPECT_TRUE(HasAttributeType(AttributeLabel(aRestored), STANDARD_TYPE(TDataXtd_Placement)));
  EXPECT_TRUE(HasAttributeType(AttributeLabel(aRestored), STANDARD_TYPE(TDataXtd_Plane)));
  EXPECT_TRUE(HasAttributeType(AttributeLabel(aRestored), STANDARD_TYPE(TDataXtd_Point)));
  EXPECT_TRUE(HasAttributeType(AttributeLabel(aRestored), STANDARD_TYPE(TDataXtd_Shape)));
  EXPECT_TRUE(HasAttributeType(AttributeLabel(aRestored), STANDARD_TYPE(TDataStd_Tick)));
  EXPECT_TRUE(HasAttributeType(AttributeLabel(aRestored), STANDARD_TYPE(TDataStd_Directory)));
  EXPECT_TRUE(HasAttributeType(AttributeLabel(aRestored), STANDARD_TYPE(TDataStd_NoteBook)));
}

// caf/basic/Y2: XML save-empty-label mode preserves empty children selectively.
TEST(XmlOcaf_Storage_Test, CafBasic_Y2_SaveEmptyLabels)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  const TDF_Label                  aParent       = aDocument->GetData()->Root().FindChild(1, true);
  aParent.FindChild(1, true);
  const TDF_Label aValueLabel = aParent.FindChild(2, true);
  aParent.FindChild(3, true);
  TDataStd_Integer::Set(aValueLabel, 321);
  TDataStd_Real::Set(aValueLabel, 871.33);

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPathWithEmpty;
  aDirectory.Path(aPathWithEmpty);
  aPathWithEmpty.SetName("draw_caf_empty_labels");
  aPathWithEmpty.SetExtension(".xml");
  TCollection_AsciiString aSystemNameWithEmpty;
  aPathWithEmpty.SystemName(aSystemNameWithEmpty);
  const TCollection_ExtendedString aFileWithEmpty(aSystemNameWithEmpty, true);
  aDocument->SetEmptyLabelsSavingMode(true);
  ASSERT_EQ(anApplication->SaveAs(aDocument, aFileWithEmpty), PCDM_SS_OK);

  OSD_Path aPathWithoutEmpty;
  aDirectory.Path(aPathWithoutEmpty);
  aPathWithoutEmpty.SetName("draw_caf_nonempty_labels");
  aPathWithoutEmpty.SetExtension(".xml");
  TCollection_AsciiString aSystemNameWithoutEmpty;
  aPathWithoutEmpty.SystemName(aSystemNameWithoutEmpty);
  const TCollection_ExtendedString aFileWithoutEmpty(aSystemNameWithoutEmpty, true);
  aDocument->SetEmptyLabelsSavingMode(false);
  ASSERT_EQ(anApplication->SaveAs(aDocument, aFileWithoutEmpty), PCDM_SS_OK);
  anApplication->Close(aDocument);

  occ::handle<TDocStd_Document> aRestoredWithEmpty;
  occ::handle<TDocStd_Document> aRestoredWithoutEmpty;
  ASSERT_EQ(anApplication->Open(aFileWithEmpty, aRestoredWithEmpty), PCDM_RS_OK);
  ASSERT_EQ(anApplication->Open(aFileWithoutEmpty, aRestoredWithoutEmpty), PCDM_RS_OK);
  ASSERT_FALSE(aRestoredWithEmpty.IsNull());
  ASSERT_FALSE(aRestoredWithoutEmpty.IsNull());
  EXPECT_EQ(aRestoredWithEmpty->GetData()->Root().FindChild(1, false).NbChildren(), 3);
  EXPECT_EQ(aRestoredWithoutEmpty->GetData()->Root().FindChild(1, false).NbChildren(), 1);

  OSD_File aFile1(aPathWithEmpty);
  OSD_File aFile2(aPathWithoutEmpty);
  if (aFile1.Exists())
  {
    aFile1.Remove();
  }
  if (aFile2.Exists())
  {
    aFile2.Remove();
  }
}

// caf/basic/N2: triangulation attributes preserve payload, transactions, and XML storage.
TEST(XmlOcaf_Storage_Test, CafBasic_N2_TriangulationStorage)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(1, true);
  const occ::handle<Poly_Triangulation> aFirstMesh = MakeTestTriangulation(false);
  const occ::handle<Poly_Triangulation> aSecondMesh = MakeTestTriangulation(true);

  aDocument->NewCommand();
  ASSERT_FALSE(TDataXtd_Triangulation::Set(aLabel, aFirstMesh).IsNull());
  aDocument->CommitCommand();

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath1;
  OSD_Path     aPath2;
  const TCollection_ExtendedString aFileName1 =
    MakeTemporaryFile(aDirectory, "draw_caf_triangulation_first", aPath1);
  const TCollection_ExtendedString aFileName2 =
    MakeTemporaryFile(aDirectory, "draw_caf_triangulation_second", aPath2);
  ASSERT_EQ(anApplication->SaveAs(aDocument, aFileName1), PCDM_SS_OK);
  OSD_File aFile1(aPath1);
  ASSERT_TRUE(aFile1.Exists());

  aDocument->NewCommand();
  ASSERT_FALSE(TDataXtd_Triangulation::Set(aLabel, aSecondMesh).IsNull());
  aDocument->CommitCommand();
  ASSERT_EQ(anApplication->SaveAs(aDocument, aFileName2), PCDM_SS_OK);
  OSD_File aFile2(aPath2);
  ASSERT_TRUE(aFile2.Exists());

  ASSERT_TRUE(aDocument->Undo());
  ExpectTriangulation(aLabel, aFirstMesh);
  ASSERT_TRUE(aDocument->Redo());
  ExpectTriangulation(aLabel, aSecondMesh);

  anApplication->Close(aDocument);
  occ::handle<TDocStd_Document> aRestoredFirst;
  occ::handle<TDocStd_Document> aRestoredSecond;
  ASSERT_EQ(anApplication->Open(aFileName1, aRestoredFirst), PCDM_RS_OK);
  ASSERT_EQ(anApplication->Open(aFileName2, aRestoredSecond), PCDM_RS_OK);
  ASSERT_FALSE(aRestoredFirst.IsNull());
  ASSERT_FALSE(aRestoredSecond.IsNull());
  ExpectTriangulation(aRestoredFirst->GetData()->Root().FindChild(1, false), aFirstMesh);
  ExpectTriangulation(aRestoredSecond->GetData()->Root().FindChild(1, false), aSecondMesh);
  anApplication->Close(aRestoredFirst);
  anApplication->Close(aRestoredSecond);

  if (aFile1.Exists())
  {
    aFile1.Remove();
  }
  if (aFile2.Exists())
  {
    aFile2.Remove();
  }
}

// caf/bugs/D1: XML preserves a mixed set of large and collection attributes.
TEST(XmlOcaf_Storage_Test, CafBug_D1_MixedAttributeStorage)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(1, true);

  occ::handle<TDataStd_ByteArray> aByteArray = TDataStd_ByteArray::Set(aLabel, 1, 10);
  ASSERT_FALSE(aByteArray.IsNull());
  for (int anIndex = 1; anIndex <= 10; ++anIndex)
  {
    aByteArray->SetValue(anIndex, static_cast<uint8_t>(anIndex));
  }

  occ::handle<TDataStd_TreeNode> aRootNode = TDataStd_TreeNode::Set(aLabel);
  ASSERT_FALSE(aRootNode.IsNull());
  for (int anIndex = 1; anIndex <= 8; ++anIndex)
  {
    const TDF_Label aChildLabel = aLabel.FindChild(anIndex, true);
    occ::handle<TDataStd_TreeNode> aChildNode = TDataStd_TreeNode::Set(aChildLabel);
    ASSERT_FALSE(aChildNode.IsNull());
    ASSERT_TRUE(aRootNode->Append(aChildNode));
  }

  occ::handle<TDataStd_IntegerArray> anIntegerArray =
    TDataStd_IntegerArray::Set(aLabel, 1, 5);
  ASSERT_FALSE(anIntegerArray.IsNull());
  const int anIntegerValues[] = {111, 222, 333, 444, 555};
  for (int anIndex = 1; anIndex <= 5; ++anIndex)
  {
    anIntegerArray->SetValue(anIndex, anIntegerValues[anIndex - 1]);
  }

  occ::handle<TDataStd_RealArray> aRealArray = TDataStd_RealArray::Set(aLabel, 1, 5);
  ASSERT_FALSE(aRealArray.IsNull());
  const double aRealValues[] = {1.12345678987654321, 2.2, 3.3, 4.4, 5.5};
  for (int anIndex = 1; anIndex <= 5; ++anIndex)
  {
    aRealArray->SetValue(anIndex, aRealValues[anIndex - 1]);
  }

  occ::handle<TDataStd_IntPackedMap> aPackedMap = TDataStd_IntPackedMap::Set(aLabel);
  ASSERT_FALSE(aPackedMap.IsNull());
  const int aPackedValues[] = {1, 10, 100, 1000, 10000};
  for (const int aValue : aPackedValues)
  {
    ASSERT_TRUE(aPackedMap->Add(aValue));
  }

  occ::handle<TDataStd_BooleanArray> aBooleanArray = TDataStd_BooleanArray::Set(aLabel, 1, 5);
  ASSERT_FALSE(aBooleanArray.IsNull());
  const bool aBooleanValues[] = {false, false, false, true, true};
  for (int anIndex = 1; anIndex <= 5; ++anIndex)
  {
    aBooleanArray->SetValue(anIndex, aBooleanValues[anIndex - 1]);
  }

  occ::handle<TDataStd_IntegerList> anIntegerList = TDataStd_IntegerList::Set(aLabel);
  ASSERT_FALSE(anIntegerList.IsNull());
  for (int aValue = 1971; aValue <= 1975; ++aValue)
  {
    anIntegerList->Append(aValue);
  }

  occ::handle<TDataStd_BooleanList> aBooleanList = TDataStd_BooleanList::Set(aLabel);
  ASSERT_FALSE(aBooleanList.IsNull());
  const bool aBooleanListValues[] = {true, false, false, false, true};
  for (const bool aValue : aBooleanListValues)
  {
    aBooleanList->Append(aValue);
  }

  occ::handle<TDataStd_RealList> aRealList = TDataStd_RealList::Set(aLabel);
  ASSERT_FALSE(aRealList.IsNull());
  const double aRealListValues[] = {0.98765432123456789e+21,
                                    0.98765432123456789e+22,
                                    0.98765432123456789e+23};
  for (const double aValue : aRealListValues)
  {
    aRealList->Append(aValue);
  }

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path     aPath;
  occ::handle<TDocStd_Document> aRestored =
    SaveNewDocument(anApplication, aDocument, "draw_caf_bug23850", aDirectory, aPath);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredLabel = aRestored->GetData()->Root().FindChild(1, false);

  occ::handle<TDataStd_ByteArray> aRestoredByteArray;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_ByteArray::GetID(), aRestoredByteArray));
  ASSERT_EQ(aRestoredByteArray->Length(), 10);
  for (int anIndex = 1; anIndex <= 10; ++anIndex)
  {
    EXPECT_EQ(static_cast<int>(aRestoredByteArray->Value(anIndex)), anIndex);
  }

  occ::handle<TDataStd_TreeNode> aRestoredRootNode = TDataStd_TreeNode::Set(aRestoredLabel);
  ASSERT_FALSE(aRestoredRootNode.IsNull());
  EXPECT_EQ(aRestoredRootNode->NbChildren(false), 8);
  TDataStd_ChildNodeIterator aNodeIterator(aRestoredRootNode, false);
  for (int anIndex = 1; aNodeIterator.More() && anIndex <= 8;
       aNodeIterator.Next(), ++anIndex)
  {
    EXPECT_EQ(aNodeIterator.Value()->Label().Tag(), anIndex);
  }
  EXPECT_FALSE(aNodeIterator.More());

  occ::handle<TDataStd_IntegerArray> aRestoredIntegerArray;
  ASSERT_TRUE(
    aRestoredLabel.FindAttribute(TDataStd_IntegerArray::GetID(), aRestoredIntegerArray));
  for (int anIndex = 1; anIndex <= 5; ++anIndex)
  {
    EXPECT_EQ(aRestoredIntegerArray->Value(anIndex), anIntegerValues[anIndex - 1]);
  }

  occ::handle<TDataStd_RealArray> aRestoredRealArray;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_RealArray::GetID(), aRestoredRealArray));
  for (int anIndex = 1; anIndex <= 5; ++anIndex)
  {
    EXPECT_DOUBLE_EQ(aRestoredRealArray->Value(anIndex), aRealValues[anIndex - 1]);
  }

  occ::handle<TDataStd_IntPackedMap> aRestoredPackedMap;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_IntPackedMap::GetID(), aRestoredPackedMap));
  EXPECT_EQ(aRestoredPackedMap->Extent(), 5);
  for (const int aValue : aPackedValues)
  {
    EXPECT_TRUE(aRestoredPackedMap->Contains(aValue));
  }

  occ::handle<TDataStd_BooleanArray> aRestoredBooleanArray;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_BooleanArray::GetID(), aRestoredBooleanArray));
  for (int anIndex = 1; anIndex <= 5; ++anIndex)
  {
    EXPECT_EQ(aRestoredBooleanArray->Value(anIndex), aBooleanValues[anIndex - 1]);
  }

  occ::handle<TDataStd_IntegerList> aRestoredIntegerList;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_IntegerList::GetID(), aRestoredIntegerList));
  int anIntegerIndex = 0;
  for (NCollection_List<int>::Iterator anIterator(aRestoredIntegerList->List());
       anIterator.More();
       anIterator.Next(), ++anIntegerIndex)
  {
    ASSERT_LT(anIntegerIndex, 5);
    EXPECT_EQ(anIterator.Value(), 1971 + anIntegerIndex);
  }
  EXPECT_EQ(anIntegerIndex, 5);

  occ::handle<TDataStd_BooleanList> aRestoredBooleanList;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_BooleanList::GetID(), aRestoredBooleanList));
  int aBooleanIndex = 0;
  for (NCollection_List<uint8_t>::Iterator anIterator(aRestoredBooleanList->List());
       anIterator.More();
       anIterator.Next(), ++aBooleanIndex)
  {
    ASSERT_LT(aBooleanIndex, 5);
    EXPECT_EQ(anIterator.Value(), aBooleanListValues[aBooleanIndex]);
  }
  EXPECT_EQ(aBooleanIndex, 5);

  occ::handle<TDataStd_RealList> aRestoredRealList;
  ASSERT_TRUE(aRestoredLabel.FindAttribute(TDataStd_RealList::GetID(), aRestoredRealList));
  int aRealIndex = 0;
  for (NCollection_List<double>::Iterator anIterator(aRestoredRealList->List());
       anIterator.More();
       anIterator.Next(), ++aRealIndex)
  {
    ASSERT_LT(aRealIndex, 3);
    EXPECT_DOUBLE_EQ(anIterator.Value(), aRealListValues[aRealIndex]);
  }
  EXPECT_EQ(aRealIndex, 3);
}

// bugs/caf/bug170_3: XML storage preserves punctuation and UTF-8 in comments.
TEST(XmlOcaf_Storage_Test, CafBug_170_3_CommentCharacters)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const TCollection_ExtendedString anExpected(
    "~!@#$%^&*():;'{}|?/.>,<~!@#$%^&*():;'{}|?/.>,<#$%#$%$%^&*():;'{}|$%^&*(@#$%^&*():;'{}|"
    "\xEF\xBF\xBD"
    "/",
    true);
  const TDF_Label aLabel = AttributeLabel(aDocument);
  ASSERT_FALSE(TDataStd_Comment::Set(aLabel, anExpected).IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_Comment> aComment;
  ASSERT_TRUE(AttributeLabel(aRestored).FindAttribute(TDataStd_Comment::GetID(), aComment));
  EXPECT_EQ(aComment->Get(), anExpected);
}

// bugs/caf/bug25317: a large real array survives XML stream storage.
TEST(XmlOcaf_Storage_Test, CafBug_25317_LargeRealArray)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const int aSize = 50000;
  const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(1, true);
  occ::handle<TDataStd_RealArray> anArray = TDataStd_RealArray::Set(aLabel, 1, aSize);
  ASSERT_FALSE(anArray.IsNull());
  for (int anIndex = 1; anIndex <= aSize; ++anIndex)
  {
    anArray->SetValue(anIndex, 1.234);
  }

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_RealArray> aRestoredArray;
  ASSERT_TRUE(LabelByEntry(aRestored, "0:1")
                .FindAttribute(TDataStd_RealArray::GetID(), aRestoredArray));
  ASSERT_FALSE(aRestoredArray.IsNull());
  EXPECT_EQ(aRestoredArray->Lower(), 1);
  EXPECT_EQ(aRestoredArray->Upper(), aSize);
  EXPECT_EQ(aRestoredArray->Length(), aSize);
  for (int anIndex = 1; anIndex <= aSize; ++anIndex)
  {
    EXPECT_DOUBLE_EQ(aRestoredArray->Value(anIndex), 1.234);
  }
}

// bugs/caf/bug25394_3: empty standard list attributes survive XML storage.
TEST(XmlOcaf_Storage_Test, CafBug_25394_3_EmptyLists)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
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
  ASSERT_TRUE(aRestoredRoot.FindChild(1, false)
                .FindAttribute(TDataStd_BooleanList::GetID(), aBooleanList));
  EXPECT_TRUE(aBooleanList->IsEmpty());
  EXPECT_EQ(aBooleanList->Extent(), 0);

  occ::handle<TDataStd_IntegerList> anIntegerList;
  ASSERT_TRUE(aRestoredRoot.FindChild(2, false)
                .FindAttribute(TDataStd_IntegerList::GetID(), anIntegerList));
  EXPECT_TRUE(anIntegerList->IsEmpty());
  EXPECT_EQ(anIntegerList->Extent(), 0);

  occ::handle<TDataStd_RealList> aRealList;
  ASSERT_TRUE(aRestoredRoot.FindChild(3, false)
                .FindAttribute(TDataStd_RealList::GetID(), aRealList));
  EXPECT_TRUE(aRealList->IsEmpty());
  EXPECT_EQ(aRealList->Extent(), 0);

  occ::handle<TDataStd_ExtStringList> anExtStringList;
  ASSERT_TRUE(aRestoredRoot.FindChild(4, false)
                .FindAttribute(TDataStd_ExtStringList::GetID(), anExtStringList));
  EXPECT_TRUE(anExtStringList->IsEmpty());
  EXPECT_EQ(anExtStringList->Extent(), 0);

  occ::handle<TDataStd_ReferenceList> aReferenceList;
  ASSERT_TRUE(aRestoredRoot.FindChild(5, false)
                .FindAttribute(TDataStd_ReferenceList::GetID(), aReferenceList));
  EXPECT_TRUE(aReferenceList->IsEmpty());
  EXPECT_EQ(aReferenceList->Extent(), 0);
}

// bugs/caf/bug25536: XML storage preserves the less common geometry types.
TEST(XmlOcaf_Storage_Test, CafBug_25536_GeometryTypes)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label aRoot = aDocument->GetData()->Root().FindChild(2, true);
  occ::handle<TDataXtd_Geometry> aSpline = TDataXtd_Geometry::Set(aRoot.FindChild(1, true));
  occ::handle<TDataXtd_Geometry> aPlane = TDataXtd_Geometry::Set(aRoot.FindChild(2, true));
  occ::handle<TDataXtd_Geometry> aCylinder = TDataXtd_Geometry::Set(aRoot.FindChild(3, true));
  ASSERT_FALSE(aSpline.IsNull());
  ASSERT_FALSE(aPlane.IsNull());
  ASSERT_FALSE(aCylinder.IsNull());
  aSpline->SetType(TDataXtd_SPLINE);
  aPlane->SetType(TDataXtd_PLANE);
  aCylinder->SetType(TDataXtd_CYLINDER);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredRoot = aRestored->GetData()->Root().FindChild(2, false);
  ASSERT_FALSE(aRestoredRoot.IsNull());
  occ::handle<TDataXtd_Geometry> aRestoredSpline;
  occ::handle<TDataXtd_Geometry> aRestoredPlane;
  occ::handle<TDataXtd_Geometry> aRestoredCylinder;
  ASSERT_TRUE(aRestoredRoot.FindChild(1, false)
                .FindAttribute(TDataXtd_Geometry::GetID(), aRestoredSpline));
  ASSERT_TRUE(aRestoredRoot.FindChild(2, false)
                .FindAttribute(TDataXtd_Geometry::GetID(), aRestoredPlane));
  ASSERT_TRUE(aRestoredRoot.FindChild(3, false)
                .FindAttribute(TDataXtd_Geometry::GetID(), aRestoredCylinder));
  EXPECT_EQ(aRestoredSpline->GetType(), TDataXtd_SPLINE);
  EXPECT_EQ(aRestoredPlane->GetType(), TDataXtd_PLANE);
  EXPECT_EQ(aRestoredCylinder->GetType(), TDataXtd_CYLINDER);
}

// bugs/caf/bug26229_2: XML OCAF SaveAs/Open stream APIs preserve attributes.
TEST(XmlOcaf_Storage_Test, CafBug_26229_2_StreamInteger)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label aLabel = aDocument->GetData()->Root().FindChild(2, true);
  ASSERT_FALSE(TDataStd_Integer::Set(aLabel, 100).IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<TDataStd_Integer> anInteger;
  ASSERT_TRUE(LabelByEntry(aRestored, "0:2")
                .FindAttribute(TDataStd_Integer::GetID(), anInteger));
  EXPECT_EQ(anInteger->Get(), 100);
}

// bugs/caf/bug27187: XML storage preserves references to labels containing a zero tag.
TEST(XmlOcaf_Storage_Test, CafBug_27187_ReferenceToZeroTag)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const char* aDeepEntry = "0:1:4:9999:0:1:4:2001:4:2001:4:1:4:1001:4:1001";
  const TDF_Label aTarget = aDocument->GetData()->Root().FindChild(1, true);
  const int aTags[] = {1, 4, 9999, 0, 1, 4, 2001, 4, 2001, 4, 1, 4, 1001, 4, 1001};
  const TDF_Label aDeepLabel = FindOrCreatePath(aDocument, aTags, 15);
  ASSERT_FALSE(aDeepLabel.IsNull());
  ASSERT_FALSE(TDF_Reference::Set(aTarget, aDeepLabel).IsNull());

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredTarget = LabelByEntry(aRestored, "0:1");
  const TDF_Label aRestoredDeepLabel = LabelByEntry(aRestored, aDeepEntry);
  ASSERT_FALSE(aRestoredTarget.IsNull());
  ASSERT_FALSE(aRestoredDeepLabel.IsNull());
  occ::handle<TDF_Reference> aReference;
  ASSERT_TRUE(aRestoredTarget.FindAttribute(TDF_Reference::GetID(), aReference));
  ASSERT_FALSE(aReference->Get().IsNull());
  EXPECT_TRUE(aReference->Get() == aRestoredDeepLabel);
  EXPECT_EQ(LabelEntry(aReference->Get()), TCollection_AsciiString(aDeepEntry));
}

// bugs/caf/bug24852: storing an empty XML OCAF document to a stream does not
// crash the application.
TEST(XmlOcaf_Storage_Test, CafBug_24852_EmptyDocumentStreamStorage)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  std::stringstream aStream;
  PCDM_StoreStatus  aStoreStatus = PCDM_SS_Failure;
  EXPECT_NO_THROW(aStoreStatus = anApplication->SaveAs(aDocument, aStream));
  EXPECT_EQ(aStoreStatus, PCDM_SS_No_Obj);
  EXPECT_TRUE(aStream.str().empty());
}

// bugs/xde/bug533: XML OCAF storage to a caller-owned stream completes for an
// empty document without raising an exception.
TEST(XmlOcaf_Storage_Test, XdeBug_533_EmptyDocumentStreamStorage)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  std::stringstream aStream;
  PCDM_StoreStatus  aStoreStatus = PCDM_SS_Failure;
  EXPECT_NO_THROW(aStoreStatus = anApplication->SaveAs(aDocument, aStream));
  EXPECT_EQ(aStoreStatus, PCDM_SS_No_Obj);
  EXPECT_TRUE(aStream.str().empty());
}

// bugs/fclasses/bug24925: XML storage works with a custom format and an empty
// resource manager when both save and load use streams.
TEST(XmlOcaf_Storage_Test, FClassesBug_24925_CustomFormatStream)
{
  occ::handle<TDocStd_Application> anApplication = new CustomXmlApplication();
  occ::handle<TDocStd_Document>    aDocument;
  anApplication->NewDocument(THE_QATEST_DOC_FORMAT, aDocument);
  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(TDataStd_Integer::Set(aDocument->Main(), 0).IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aDocument->Main(), "QABugs_19.cxx").IsNull());

  std::stringstream aStream;
  ASSERT_EQ(anApplication->SaveAs(aDocument, aStream), PCDM_SS_OK);
  const std::string aContent = aStream.str();
  ASSERT_FALSE(aContent.empty());
  EXPECT_NE(aContent.find("My Proprietary Format"), std::string::npos);

  anApplication->Close(aDocument);
  aStream.seekg(0);
  occ::handle<TDocStd_Document> aRestored;
  ASSERT_EQ(anApplication->Open(aStream, aRestored), PCDM_RS_OK);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_Integer> anInteger;
  ASSERT_TRUE(aRestored->Main().FindAttribute(TDataStd_Integer::GetID(), anInteger));
  EXPECT_EQ(anInteger->Get(), 0);
  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(aRestored->Main().FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), TCollection_ExtendedString("QABugs_19.cxx"));
}

// bugs/fclasses/bug24931: XML storage can serialize the original 10,000-shape
// stress document through a stream and restore its named shape.
TEST(XmlOcaf_Storage_Test, FClassesBug_24931_LargeShapeStream)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(TDataStd_Integer::Set(aDocument->Main(), 0).IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aDocument->Main(), "QABugs_19.cxx").IsNull());

  int aShapeCount = 10000;
  const TopoDS_Shape aShape = MakeLargeTestShape(aShapeCount);
  SetShape(aDocument->Main(), aShape);

  std::stringstream aStream;
  ASSERT_EQ(anApplication->SaveAs(aDocument, aStream), PCDM_SS_OK);
  ASSERT_FALSE(aStream.str().empty());

  anApplication->Close(aDocument);
  aStream.seekg(0);
  occ::handle<TDocStd_Document> aRestored;
  ASSERT_EQ(anApplication->Open(aStream, aRestored), PCDM_RS_OK);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_Integer> anInteger;
  ASSERT_TRUE(aRestored->Main().FindAttribute(TDataStd_Integer::GetID(), anInteger));
  EXPECT_EQ(anInteger->Get(), 0);
  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(aRestored->Main().FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), TCollection_ExtendedString("QABugs_19.cxx"));

  Bnd_Box aSourceBounds;
  BRepBndLib::Add(aShape, aSourceBounds);
  double aSourceXMin = 0.0;
  double aSourceYMin = 0.0;
  double aSourceZMin = 0.0;
  double aSourceXMax = 0.0;
  double aSourceYMax = 0.0;
  double aSourceZMax = 0.0;
  aSourceBounds.Get(aSourceXMin, aSourceYMin, aSourceZMin, aSourceXMax, aSourceYMax, aSourceZMax);

  Bnd_Box aRestoredBounds;
  BRepBndLib::Add(ShapeOf(aRestored->Main()), aRestoredBounds);
  double aRestoredXMin = 0.0;
  double aRestoredYMin = 0.0;
  double aRestoredZMin = 0.0;
  double aRestoredXMax = 0.0;
  double aRestoredYMax = 0.0;
  double aRestoredZMax = 0.0;
  aRestoredBounds.Get(aRestoredXMin,
                       aRestoredYMin,
                       aRestoredZMin,
                       aRestoredXMax,
                       aRestoredYMax,
                       aRestoredZMax);
  EXPECT_NEAR(aRestoredXMin, aSourceXMin, 1.0e-6);
  EXPECT_NEAR(aRestoredYMin, aSourceYMin, 1.0e-6);
  EXPECT_NEAR(aRestoredZMin, aSourceZMin, 1.0e-6);
  EXPECT_NEAR(aRestoredXMax, aSourceXMax, 1.0e-6);
  EXPECT_NEAR(aRestoredYMax, aSourceYMax, 1.0e-6);
  EXPECT_NEAR(aRestoredZMax, aSourceZMax, 1.0e-6);
}

// bugs/caf/bug1919: XML stream retrieval must keep real values independent of
// the numeric locale. The DRAW source is explicitly TODO OCC12345: the
// localized comma input is currently read as 123 instead of 123.456. Keep the
// port disabled until the underlying locale handling is fixed.
TEST(XmlOcaf_Storage_Test, DISABLED_CafBug_1919_LocaleRoundTrip)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const TDF_Label anEnglishLabel = aDocument->GetData()->Root().FindChild(10, true);
  const TDF_Label anItalianLabel = aDocument->GetData()->Root().FindChild(20, true);
  const TCollection_AsciiString anOldLocale((const char*)setlocale(LC_ALL, nullptr));
  aDocument->NewCommand();

  setlocale(LC_ALL, "");
  const char* anEnglishLocale = setlocale(LC_NUMERIC, "en_US");
  const bool  hasEnglishLocale = anEnglishLocale != nullptr
                                && std::strcmp(anEnglishLocale, "en_US") == 0;
  const char* anItalianLocale = setlocale(LC_NUMERIC, "it");
  const bool  hasItalianLocale = anItalianLocale != nullptr
                                && std::strcmp(anItalianLocale, "it") == 0;
  if (!hasEnglishLocale || !hasItalianLocale)
  {
    setlocale(LC_ALL, anOldLocale.ToCString());
    GTEST_SKIP() << "Required DRAW locales en_US and it are unavailable";
  }

  setlocale(LC_ALL, "");
  const char* anEnglishLocaleAfterReset = setlocale(LC_NUMERIC, "en_US");
  const bool  isEnglishLocaleAfterReset = anEnglishLocaleAfterReset != nullptr
                                         && std::strcmp(anEnglishLocaleAfterReset, "en_US") == 0;
  const TCollection_AsciiString anEnglishInput("123.456");
  const bool                   isEnglishInput = anEnglishInput.IsRealValue();
  const double                 anEnglishValue = anEnglishInput.RealValue();
  occ::handle<TDataStd_Real>   anEnglishReal = TDataStd_Real::Set(anEnglishLabel, anEnglishValue);

  setlocale(LC_ALL, "");
  setlocale(LC_NUMERIC, "it");
  const TCollection_AsciiString anItalianInput("123,456");
  const bool                   isItalianInput = anItalianInput.IsRealValue();
  const double                 anItalianValue = anItalianInput.RealValue();
  occ::handle<TDataStd_Real>   anItalianReal = TDataStd_Real::Set(anItalianLabel, anItalianValue);

  const bool   hasCurrentEnglish = !anEnglishReal.IsNull();
  const bool   hasCurrentItalian = !anItalianReal.IsNull();
  const double aCurrentEnglishValue = hasCurrentEnglish ? anEnglishReal->Get() : 0.0;
  const double aCurrentItalianValue = hasCurrentItalian ? anItalianReal->Get() : 0.0;

  std::stringstream aStream;
  const PCDM_StoreStatus aStoreStatus = anApplication->SaveAs(aDocument, aStream);
  const bool             hasSerializedData = !aStream.str().empty();
  anApplication->Close(aDocument);

  setlocale(LC_ALL, "");
  const char* anEnglishLocaleBeforeRead = setlocale(LC_NUMERIC, "en_US");
  const bool  isEnglishLocaleBeforeRead = anEnglishLocaleBeforeRead != nullptr
                                         && std::strcmp(anEnglishLocaleBeforeRead, "en_US") == 0;
  aStream.seekg(0);
  occ::handle<TDocStd_Document> aRestored;
  const PCDM_ReaderStatus aReadStatus = anApplication->Open(aStream, aRestored);

  double aRestoredEnglishValue = 0.0;
  double aRestoredItalianValue = 0.0;
  bool   hasRestoredEnglish    = false;
  bool   hasRestoredItalian    = false;
  if (!aRestored.IsNull())
  {
    occ::handle<TDataStd_Real> anEnglishReal;
    occ::handle<TDataStd_Real> anItalianReal;
    hasRestoredEnglish = aRestored->GetData()->Root().FindChild(10, false).FindAttribute(
      TDataStd_Real::GetID(), anEnglishReal);
    hasRestoredItalian = aRestored->GetData()->Root().FindChild(20, false).FindAttribute(
      TDataStd_Real::GetID(), anItalianReal);
    if (hasRestoredEnglish)
    {
      aRestoredEnglishValue = anEnglishReal->Get();
    }
    if (hasRestoredItalian)
    {
      aRestoredItalianValue = anItalianReal->Get();
    }
  }
  setlocale(LC_ALL, anOldLocale.ToCString());

  EXPECT_TRUE(isEnglishInput);
  EXPECT_TRUE(isItalianInput);
  EXPECT_TRUE(isEnglishLocaleAfterReset);
  EXPECT_TRUE(hasCurrentEnglish);
  EXPECT_TRUE(hasCurrentItalian);
  EXPECT_DOUBLE_EQ(anEnglishValue, 123.456);
  EXPECT_DOUBLE_EQ(anItalianValue, 123.456);
  EXPECT_DOUBLE_EQ(aCurrentEnglishValue, 123.456);
  EXPECT_DOUBLE_EQ(aCurrentItalianValue, 123.456);
  EXPECT_EQ(aStoreStatus, PCDM_SS_OK);
  EXPECT_TRUE(hasSerializedData);
  EXPECT_TRUE(isEnglishLocaleBeforeRead);
  EXPECT_EQ(aReadStatus, PCDM_RS_OK);
  EXPECT_TRUE(hasRestoredEnglish);
  EXPECT_TRUE(hasRestoredItalian);
  EXPECT_DOUBLE_EQ(aRestoredEnglishValue, 123.456);
  EXPECT_DOUBLE_EQ(aRestoredItalianValue, 123.456);
}
