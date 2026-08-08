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

#include <BinXCAFDrivers.hxx>
#include <XmlXCAFDrivers.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_Sequence.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <PCDM_ReaderStatus.hxx>
#include <PCDM_StoreStatus.hxx>
#include <Precision.hxx>
#include <TDataStd_Name.hxx>
#include <TDataXtd_Position.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsMethods.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_Volume.hxx>

#include <gp_Dir.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

#include <sstream>

namespace
{
enum class StorageFormat
{
  Xml,
  Xbf
};

static occ::handle<TDocStd_Application> NewApplication(const StorageFormat theFormat)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  if (theFormat == StorageFormat::Xml)
  {
    XmlXCAFDrivers::DefineFormat(anApplication);
  }
  else
  {
    BinXCAFDrivers::DefineFormat(anApplication);
  }
  return anApplication;
}

static const char* FormatName(const StorageFormat theFormat)
{
  return theFormat == StorageFormat::Xml ? "XmlXCAF" : "BinXCAF";
}

static const char* FileExtension(const StorageFormat theFormat)
{
  return theFormat == StorageFormat::Xml ? "xml" : "xbf";
}

static TCollection_ExtendedString MakeTemporaryFile(OSD_Directory&      theDirectory,
                                                    const char*         theName,
                                                    const StorageFormat theFormat,
                                                    OSD_Path&           thePath)
{
  theDirectory.Path(thePath);
  thePath.SetName(theName);
  thePath.SetExtension(TCollection_AsciiString(".") + FileExtension(theFormat));

  TCollection_AsciiString aSystemName;
  thePath.SystemName(aSystemName);
  return TCollection_ExtendedString(aSystemName, true);
}

static TDF_Label Label_0_1_1_1(const occ::handle<TDocStd_Document>& theDocument)
{
  TDF_Label aLabel = theDocument->Main();
  aLabel           = aLabel.FindChild(1, true);
  aLabel           = aLabel.FindChild(1, true);
  return aLabel.FindChild(1, true);
}

static occ::handle<TDocStd_Document> SaveAndOpen(
  const occ::handle<TDocStd_Application>& theApplication,
  const occ::handle<TDocStd_Document>&    theDocument,
  const TCollection_ExtendedString&       theFileName,
  const OSD_Path&                         theFilePath)
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

static occ::handle<TDocStd_Document> NewDocument(
  const StorageFormat                     theFormat,
  const occ::handle<TDocStd_Application>& theApplication)
{
  occ::handle<TDocStd_Document> aDocument;
  theApplication->NewDocument(FormatName(theFormat), aDocument);
  EXPECT_FALSE(aDocument.IsNull());
  return aDocument;
}

static void TestArea(const StorageFormat theFormat)
{
  OSD_Directory                    aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path                         aFilePath;
  const TCollection_ExtendedString aFileName =
    MakeTemporaryFile(aDirectory, "draw_xcaf_area", theFormat, aFilePath);
  occ::handle<TDocStd_Application> anApplication = NewApplication(theFormat);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(theFormat, anApplication);
  const TDF_Label                  aLabel        = Label_0_1_1_1(aDocument);
  const double                     aExpected     = 13.777;

  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(XCAFDoc_Area::Set(aLabel, aExpected).IsNull());
  aDocument->NewCommand();

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, aFileName, aFilePath);
  ASSERT_FALSE(aRestored.IsNull());

  double aValue = 0.0;
  ASSERT_TRUE(XCAFDoc_Area::Get(Label_0_1_1_1(aRestored), aValue));
  EXPECT_DOUBLE_EQ(aValue, aExpected);
}

static void TestColor(const StorageFormat theFormat)
{
  OSD_Directory                    aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path                         aFilePath;
  const TCollection_ExtendedString aFileName =
    MakeTemporaryFile(aDirectory, "draw_xcaf_color", theFormat, aFilePath);
  occ::handle<TDocStd_Application> anApplication = NewApplication(theFormat);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(theFormat, anApplication);
  const Quantity_ColorRGBA         anExpected(0.3f, 0.3f, 0.3f, 1.0f);
  occ::handle<XCAFDoc_ColorTool>   aColorTool = XCAFDoc_DocumentTool::ColorTool(aDocument->Main());
  const TDF_Label                  aLabel     = aColorTool->AddColor(anExpected);

  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(aLabel.IsNull());
  Quantity_ColorRGBA aBefore;
  ASSERT_TRUE(XCAFDoc_ColorTool::GetColor(aLabel, aBefore));
  aDocument->NewCommand();

  TCollection_AsciiString aColorEntry;
  TDF_Tool::Entry(aLabel, aColorEntry);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, aFileName, aFilePath);
  ASSERT_FALSE(aRestored.IsNull());

  TDF_Label          aRestoredLabel;
  Quantity_ColorRGBA aValue;
  TDF_Tool::Label(aRestored->GetData(), aColorEntry, aRestoredLabel);
  ASSERT_TRUE(XCAFDoc_ColorTool::GetColor(aRestoredLabel, aValue));
  // XML stores the color through its canonical named-color representation;
  // compare the same semantic value used by DRAW's XGetColor command.
  EXPECT_EQ(aValue.GetRGB().Name(), aBefore.GetRGB().Name());
  EXPECT_FLOAT_EQ(aValue.Alpha(), aBefore.Alpha());
}

static void TestCentroid(const StorageFormat theFormat)
{
  OSD_Directory                    aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path                         aFilePath;
  const TCollection_ExtendedString aFileName =
    MakeTemporaryFile(aDirectory, "draw_xcaf_centroid", theFormat, aFilePath);
  occ::handle<TDocStd_Application> anApplication = NewApplication(theFormat);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(theFormat, anApplication);
  const TDF_Label                  aLabel        = Label_0_1_1_1(aDocument);
  const gp_Pnt                     anExpected(12.34, 56.78, 90.12);

  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(XCAFDoc_Centroid::Set(aLabel, anExpected).IsNull());
  aDocument->NewCommand();

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, aFileName, aFilePath);
  ASSERT_FALSE(aRestored.IsNull());

  gp_Pnt aValue;
  ASSERT_TRUE(XCAFDoc_Centroid::Get(Label_0_1_1_1(aRestored), aValue));
  EXPECT_TRUE(aValue.IsEqual(anExpected, Precision::Confusion()));
}

static void TestName(const StorageFormat theFormat)
{
  OSD_Directory                    aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path                         aFilePath;
  const TCollection_ExtendedString aFileName =
    MakeTemporaryFile(aDirectory, "draw_xcaf_name", theFormat, aFilePath);
  occ::handle<TDocStd_Application> anApplication = NewApplication(theFormat);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(theFormat, anApplication);
  const TDF_Label                  aLabel        = Label_0_1_1_1(aDocument);
  const TCollection_ExtendedString anExpected("00");

  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aLabel, anExpected).IsNull());
  aDocument->NewCommand();

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, aFileName, aFilePath);
  ASSERT_FALSE(aRestored.IsNull());

  occ::handle<TDataStd_Name> aName;
  ASSERT_TRUE(Label_0_1_1_1(aRestored).FindAttribute(TDataStd_Name::GetID(), aName));
  EXPECT_EQ(aName->Get(), anExpected);
}

static void TestLayers(const StorageFormat theFormat)
{
  OSD_Directory                    aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path                         aFilePath;
  const TCollection_ExtendedString aFileName =
    MakeTemporaryFile(aDirectory, "draw_xcaf_layers", theFormat, aFilePath);
  occ::handle<TDocStd_Application> anApplication = NewApplication(theFormat);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(theFormat, anApplication);
  occ::handle<XCAFDoc_ShapeTool>   aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  occ::handle<XCAFDoc_LayerTool>   aLayerTool = XCAFDoc_DocumentTool::LayerTool(aDocument->Main());
  const TDF_Label aShapeLabel = aShapeTool->AddShape(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  const TDF_Label aLayerLabel = aLayerTool->AddLayer(TCollection_ExtendedString("Layer_1"));

  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(aShapeLabel.IsNull());
  ASSERT_FALSE(aLayerLabel.IsNull());
  aLayerTool->SetLayer(aShapeLabel, aLayerLabel);
  aDocument->NewCommand();

  TCollection_AsciiString aShapeEntry;
  TCollection_AsciiString aLayerEntry;
  TDF_Tool::Entry(aShapeLabel, aShapeEntry);
  TDF_Tool::Entry(aLayerLabel, aLayerEntry);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, aFileName, aFilePath);
  ASSERT_FALSE(aRestored.IsNull());

  TDF_Label aRestoredShape;
  TDF_Label aRestoredLayer;
  TDF_Tool::Label(aRestored->GetData(), aShapeEntry, aRestoredShape);
  TDF_Tool::Label(aRestored->GetData(), aLayerEntry, aRestoredLayer);
  ASSERT_FALSE(aRestoredShape.IsNull());
  ASSERT_FALSE(aRestoredLayer.IsNull());

  occ::handle<XCAFDoc_LayerTool> aRestoredLayerTool =
    XCAFDoc_DocumentTool::LayerTool(aRestored->Main());
  NCollection_Sequence<TDF_Label> aLayers;
  ASSERT_TRUE(aRestoredLayerTool->GetLayers(aRestoredShape, aLayers));
  ASSERT_EQ(aLayers.Length(), 1);
  EXPECT_EQ(aLayers.Value(1), aRestoredLayer);

  TCollection_ExtendedString aLayerName;
  ASSERT_TRUE(aRestoredLayerTool->GetLayer(aLayers.Value(1), aLayerName));
  EXPECT_EQ(aLayerName, TCollection_ExtendedString("Layer_1"));
}

static void TestLocation(const StorageFormat theFormat)
{
  OSD_Directory                    aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path                         aFilePath;
  const TCollection_ExtendedString aFileName =
    MakeTemporaryFile(aDirectory, "draw_xcaf_location", theFormat, aFilePath);
  occ::handle<TDocStd_Application> anApplication = NewApplication(theFormat);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(theFormat, anApplication);
  occ::handle<XCAFDoc_ShapeTool>   aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label                  anAssembly = aShapeTool->NewShape();
  const TDF_Label aBaseShape = aShapeTool->AddShape(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  gp_Trsf         aTransformation;
  aTransformation.SetTranslation(gp_Vec(333.0, 777.0, 13.0));
  const TopLoc_Location anExpectedLocation(aTransformation);
  const TDF_Label aComponent = aShapeTool->AddComponent(anAssembly, aBaseShape, anExpectedLocation);

  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(anAssembly.IsNull());
  ASSERT_FALSE(aComponent.IsNull());
  aDocument->NewCommand();

  TCollection_AsciiString anAssemblyEntry;
  TCollection_AsciiString aComponentEntry;
  TDF_Tool::Entry(anAssembly, anAssemblyEntry);
  TDF_Tool::Entry(aComponent, aComponentEntry);

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, aFileName, aFilePath);
  ASSERT_FALSE(aRestored.IsNull());

  TDF_Label aRestoredAssembly;
  TDF_Label aRestoredComponent;
  TDF_Tool::Label(aRestored->GetData(), anAssemblyEntry, aRestoredAssembly);
  TDF_Tool::Label(aRestored->GetData(), aComponentEntry, aRestoredComponent);
  ASSERT_FALSE(aRestoredAssembly.IsNull());
  ASSERT_FALSE(aRestoredComponent.IsNull());
  const gp_Trsf aRestoredTransformation =
    XCAFDoc_ShapeTool::GetLocation(aRestoredComponent).Transformation();
  const gp_Trsf anExpectedTransformation = anExpectedLocation.Transformation();
  for (int aRow = 1; aRow <= 3; ++aRow)
  {
    for (int aColumn = 1; aColumn <= 4; ++aColumn)
    {
      EXPECT_DOUBLE_EQ(aRestoredTransformation.Value(aRow, aColumn),
                       anExpectedTransformation.Value(aRow, aColumn));
    }
  }
}

static void TestVolume(const StorageFormat theFormat)
{
  OSD_Directory                    aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path                         aFilePath;
  const TCollection_ExtendedString aFileName =
    MakeTemporaryFile(aDirectory, "draw_xcaf_volume", theFormat, aFilePath);
  occ::handle<TDocStd_Application> anApplication = NewApplication(theFormat);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(theFormat, anApplication);
  const TDF_Label                  aLabel        = Label_0_1_1_1(aDocument);
  const double                     anExpected    = 13.777;

  ASSERT_FALSE(aDocument.IsNull());
  ASSERT_FALSE(XCAFDoc_Volume::Set(aLabel, anExpected).IsNull());
  aDocument->NewCommand();

  occ::handle<TDocStd_Document> aRestored =
    SaveAndOpen(anApplication, aDocument, aFileName, aFilePath);
  ASSERT_FALSE(aRestored.IsNull());

  double aValue = 0.0;
  ASSERT_TRUE(XCAFDoc_Volume::Get(Label_0_1_1_1(aRestored), aValue));
  EXPECT_DOUBLE_EQ(aValue, anExpected);
}

static void TestLengthUnit(const StorageFormat theFormat, const double theExpectedMeterScale)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication(theFormat);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(theFormat, anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  ASSERT_FALSE(aShapeTool->AddShape(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape()).IsNull());
  XCAFDoc_DocumentTool::SetLengthUnit(aDocument, theExpectedMeterScale);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  double aRestoredMeterScale = 0.0;
  ASSERT_TRUE(XCAFDoc_DocumentTool::GetLengthUnit(aRestored, aRestoredMeterScale));
  EXPECT_NEAR(aRestoredMeterScale, theExpectedMeterScale, 1.0e-2);
}

static void TestGlobalLengthUnit(const StorageFormat theFormat,
                                 const int           theCascadeUnit,
                                 const double        theExpectedMeterScale)
{
  UnitsMethods::SetCasCadeLengthUnit(theCascadeUnit);
  occ::handle<TDocStd_Application> anApplication = NewApplication(theFormat);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(theFormat, anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  ASSERT_FALSE(aShapeTool->AddShape(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape()).IsNull());
  XCAFDoc_DocumentTool::SetLengthUnit(
    aDocument,
    UnitsMethods::GetCasCadeLengthUnit(UnitsMethods_LengthUnit_Meter));

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  UnitsMethods::SetCasCadeLengthUnit(2);
  ASSERT_FALSE(aRestored.IsNull());
  double aRestoredMeterScale = 0.0;
  ASSERT_TRUE(XCAFDoc_DocumentTool::GetLengthUnit(aRestored, aRestoredMeterScale));
  EXPECT_NEAR(aRestoredMeterScale, theExpectedMeterScale, 1.0e-2);
}

static void TestAffectedPlane()
{
  occ::handle<TDocStd_Application> anApplication = NewApplication(StorageFormat::Xbf);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(StorageFormat::Xbf, anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  occ::handle<XCAFDoc_ShapeTool>  aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  occ::handle<XCAFDoc_DimTolTool> aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(aDocument->Main());
  const TDF_Label aShapeLabel = aShapeTool->AddShape(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape());
  const TDF_Label aToleranceLabel = aDimTolTool->AddGeomTolerance();
  aDimTolTool->SetGeomTolerance(aShapeLabel, aToleranceLabel);

  const gp_Pln                       aExpectedPlane(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 1.0, 0.0));
  occ::handle<XCAFDoc_GeomTolerance> aTolerance;
  ASSERT_TRUE(aToleranceLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aTolerance));
  occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObject = aTolerance->GetObject();
  anObject->SetAffectedPlane(aExpectedPlane,
                             XCAFDimTolObjects_ToleranceZoneAffectedPlane_Orientation);
  aTolerance->SetObject(anObject);

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<XCAFDoc_DimTolTool> aRestoredDimTolTool =
    XCAFDoc_DocumentTool::DimTolTool(aRestored->Main());
  NCollection_Sequence<TDF_Label> aTolerances;
  aRestoredDimTolTool->GetGeomToleranceLabels(aTolerances);
  ASSERT_EQ(aTolerances.Length(), 1);

  occ::handle<XCAFDoc_GeomTolerance> aRestoredTolerance;
  ASSERT_TRUE(
    aTolerances.Value(1).FindAttribute(XCAFDoc_GeomTolerance::GetID(), aRestoredTolerance));
  const occ::handle<XCAFDimTolObjects_GeomToleranceObject> aRestoredObject =
    aRestoredTolerance->GetObject();
  ASSERT_TRUE(aRestoredObject->HasAffectedPlane());
  EXPECT_EQ(aRestoredObject->GetAffectedPlaneType(),
            XCAFDimTolObjects_ToleranceZoneAffectedPlane_Orientation);
  const gp_Pln& aRestoredPlane = aRestoredObject->GetAffectedPlane();
  EXPECT_TRUE(aRestoredPlane.Location().IsEqual(aExpectedPlane.Location(), Precision::Confusion()));
  EXPECT_TRUE(aRestoredPlane.Axis().Direction().IsEqual(aExpectedPlane.Axis().Direction(),
                                                        Precision::Confusion()));
}

} // namespace

// xml/data/xcaf/A1: XCAFDoc_Area survives XML and XBF document storage.
TEST(XmlXCAF_Storage_Test, Xml_A1_Area)
{
  TestArea(StorageFormat::Xml);
}

TEST(XmlXCAF_Storage_Test, Xbf_A1_Area)
{
  TestArea(StorageFormat::Xbf);
}

// xml/data/xcaf/A4: the color table keeps the exact RGB/RGBA value.
TEST(XmlXCAF_Storage_Test, Xml_A4_Color)
{
  TestColor(StorageFormat::Xml);
}

TEST(XmlXCAF_Storage_Test, Xbf_A4_Color)
{
  TestColor(StorageFormat::Xbf);
}

// xml/data/xcaf/A5: XCAFDoc_Centroid survives document storage.
TEST(XmlXCAF_Storage_Test, Xml_A5_Centroid)
{
  TestCentroid(StorageFormat::Xml);
}

TEST(XmlXCAF_Storage_Test, Xbf_A5_Centroid)
{
  TestCentroid(StorageFormat::Xbf);
}

// xml/data/xcaf/A6: leading zeroes in a TDataStd_Name are preserved.
TEST(XmlXCAF_Storage_Test, Xml_A6_NameLeadingZeroes)
{
  TestName(StorageFormat::Xml);
}

TEST(XmlXCAF_Storage_Test, Xbf_A6_NameLeadingZeroes)
{
  TestName(StorageFormat::Xbf);
}

// xml/data/xcaf/A7: shape-to-layer graph links survive document storage.
TEST(XmlXCAF_Storage_Test, Xml_A7_LayerLink)
{
  TestLayers(StorageFormat::Xml);
}

TEST(XmlXCAF_Storage_Test, Xbf_A7_LayerLink)
{
  TestLayers(StorageFormat::Xbf);
}

// xml/data/xcaf/A8: a located component keeps its XCAFDoc_Location.
TEST(XmlXCAF_Storage_Test, Xml_A8_ComponentLocation)
{
  TestLocation(StorageFormat::Xml);
}

TEST(XmlXCAF_Storage_Test, Xbf_A8_ComponentLocation)
{
  TestLocation(StorageFormat::Xbf);
}

// xml/data/xcaf/A9: XCAFDoc_Volume survives document storage.
TEST(XmlXCAF_Storage_Test, Xml_A9_Volume)
{
  TestVolume(StorageFormat::Xml);
}

TEST(XmlXCAF_Storage_Test, Xbf_A9_Volume)
{
  TestVolume(StorageFormat::Xbf);
}

TEST(XmlXCAF_Storage_Test, Xml_CafBug_31382_InchUnit)
{
  TestLengthUnit(StorageFormat::Xml, 0.0254);
}

TEST(XmlXCAF_Storage_Test, Xbf_CafBug_31382_MeterUnit)
{
  TestLengthUnit(StorageFormat::Xbf, 1.0);
}

TEST(XmlXCAF_Storage_Test, Xml_CafBug_31382_GlobalMileUnit)
{
  TestGlobalLengthUnit(StorageFormat::Xml, 5, 1609.344);
}

TEST(XmlXCAF_Storage_Test, Xbf_CafBug_31382_GlobalKilometerUnit)
{
  TestGlobalLengthUnit(StorageFormat::Xbf, 7, 1000.0);
}

TEST(XmlXCAF_Storage_Test, Xbf_CafBug_29338_AffectedPlane)
{
  TestAffectedPlane();
}

// bugs/caf/bug25537: XML-XCAF storage handles many long decimal position and
// centroid attributes without an exception or value corruption.
TEST(XmlXCAF_Storage_Test, CafBug_25537_LongPositionAndCentroid)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication(StorageFormat::Xml);
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(StorageFormat::Xml, anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const double    aBigNumber = -0.0000000000123456789123456789;
  const gp_Pnt    anExpected(aBigNumber, aBigNumber, aBigNumber);
  const TDF_Label aRoot = aDocument->GetData()->Root();

  for (int anIndex = 1; anIndex < 100; ++anIndex)
  {
    TDataXtd_Position::Set(aRoot.FindChild(1, true).FindChild(anIndex, true), anExpected);
    ASSERT_FALSE(
      XCAFDoc_Centroid::Set(aRoot.FindChild(2, true).FindChild(anIndex, true), anExpected)
        .IsNull());
  }

  occ::handle<TDocStd_Document> aRestored = SaveAndOpenStream(anApplication, aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredRoot = aRestored->GetData()->Root();
  for (int anIndex = 1; anIndex < 100; ++anIndex)
  {
    gp_Pnt aPosition;
    ASSERT_TRUE(TDataXtd_Position::Get(aRestoredRoot.FindChild(1, false).FindChild(anIndex, false),
                                       aPosition));
    EXPECT_TRUE(aPosition.IsEqual(anExpected, Precision::Confusion()));

    gp_Pnt aCentroid;
    ASSERT_TRUE(XCAFDoc_Centroid::Get(aRestoredRoot.FindChild(2, false).FindChild(anIndex, false),
                                      aCentroid));
    EXPECT_TRUE(aCentroid.IsEqual(anExpected, Precision::Confusion()));
  }
}
