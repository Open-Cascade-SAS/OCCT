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

#include <DESTEP_ConfigurationNode.hxx>
#include <DESTEP_Provider.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <NCollection_Array1.hxx>
#include <Resource_FormatType.hxx>
#include <Resource_Unicode.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>

#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Bnd_Box.hxx>
#include <DE_Provider.hxx>
#include <NCollection_Sequence.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFDimTolObjects_DatumSingleModif.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFDimTolObjects_DimensionType.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <XCAFDimTolObjects_GeomToleranceType.hxx>
#include <XCAFDimTolObjects_GeomToleranceTypeValue.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>

#include <sstream>
#include <string>

#include <gtest/gtest.h>

namespace
{
static occ::handle<TDocStd_Document> NewDocument(occ::handle<TDocStd_Application>& theApplication)
{
  occ::handle<TDocStd_Document> aDocument;
  theApplication->NewDocument("BinXCAF", aDocument);
  EXPECT_FALSE(aDocument.IsNull());
  return aDocument;
}

static TDF_Label AddBoxAndDimension(const occ::handle<TDocStd_Document>& theDocument)
{
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  const TDF_Label aShapeLabel = aShapeTool->AddShape(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  occ::handle<XCAFDoc_DimTolTool> aDimTolTool =
    XCAFDoc_DocumentTool::DimTolTool(theDocument->Main());
  const TDF_Label aDimensionLabel = aDimTolTool->AddDimension();
  aDimTolTool->SetDimension(aShapeLabel, aDimensionLabel);
  return aDimensionLabel;
}

static occ::handle<XCAFDimTolObjects_DimensionObject> DimensionObject(
  const TDF_Label& theDimensionLabel)
{
  occ::handle<XCAFDoc_Dimension> aDimension;
  if (!theDimensionLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension))
  {
    return nullptr;
  }
  return aDimension->GetObject();
}

static occ::handle<XCAFDimTolObjects_DimensionObject> RestoreDimension(
  const occ::handle<TDocStd_Document>& theDocument)
{
  occ::handle<XCAFDoc_DimTolTool> aDimTolTool =
    XCAFDoc_DocumentTool::DimTolTool(theDocument->Main());
  NCollection_Sequence<TDF_Label> aDimensionLabels;
  aDimTolTool->GetDimensionLabels(aDimensionLabels);
  if (aDimensionLabels.IsEmpty())
  {
    return nullptr;
  }
  return DimensionObject(aDimensionLabels.First());
}

static occ::handle<TDocStd_Document> RoundTrip(const occ::handle<TDocStd_Document>& theDocument,
                                               const bool theWriteNonmanifold = false)
{
  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  aNode->InternalParameters.WriteSchema       = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  aNode->InternalParameters.WriteNonmanifold  = theWriteNonmanifold;
  DESTEP_Provider aProvider(aNode);

  std::stringstream            aStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("gdt.step", aStream));
  if (!aProvider.Write(aWriteStreams, theDocument))
  {
    ADD_FAILURE() << "DESTEP_Provider failed to write the GD&T stream";
    return nullptr;
  }

  EXPECT_FALSE(aStream.str().empty());
  aStream.seekg(0);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("gdt.step", aStream));

  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aRestored     = NewDocument(anApplication);
  if (!aProvider.Read(aReadStreams, aRestored))
  {
    ADD_FAILURE() << "DESTEP_Provider failed to read the GD&T stream";
    return nullptr;
  }
  return aRestored;
}

static void ExpectPoint(const gp_Pnt& theActual, const gp_Pnt& theExpected)
{
  EXPECT_DOUBLE_EQ(theActual.X(), theExpected.X());
  EXPECT_DOUBLE_EQ(theActual.Y(), theExpected.Y());
  EXPECT_DOUBLE_EQ(theActual.Z(), theExpected.Z());
}

static void ExpectDirection(const gp_Dir& theActual, const gp_Dir& theExpected)
{
  EXPECT_DOUBLE_EQ(theActual.X(), theExpected.X());
  EXPECT_DOUBLE_EQ(theActual.Y(), theExpected.Y());
  EXPECT_DOUBLE_EQ(theActual.Z(), theExpected.Z());
}

static bool WriteStepStream(const occ::handle<TDocStd_Document>& theDocument,
                            std::stringstream&                   theStream)
{
  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  aNode->InternalParameters.WriteSchema       = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  DESTEP_Provider              aProvider(aNode);
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("draw_step.stream", theStream));
  return aProvider.Write(aWriteStreams, theDocument);
}

static TopoDS_Compound MakeLocatedBoxAssembly(const int theCount)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape();
  for (int anIndex = 0; anIndex < theCount; ++anIndex)
  {
    for (int aSubIndex = 0; aSubIndex < theCount; ++aSubIndex)
    {
      gp_Trsf aTrsf;
      aTrsf.SetTranslation(gp_Vec(5.0 * anIndex, 0.5 * aSubIndex, 0.0));
      aBuilder.Add(aCompound, aBox.Located(TopLoc_Location(aTrsf)));
    }
  }
  return aCompound;
}

static occ::handle<TDocStd_Document> RoundTripWithSTEPCAF(
  const occ::handle<TDocStd_Document>& theDocument)
{
  DESTEP_Parameters aParameters;
  aParameters.WriteSchema = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  STEPCAFControl_Writer aWriter;
  EXPECT_TRUE(aWriter.Transfer(theDocument, aParameters));
  std::stringstream aStream;
  EXPECT_EQ(aWriter.WriteStream(aStream), IFSelect_RetDone);
  EXPECT_FALSE(aStream.str().empty());

  aStream.seekg(0);
  STEPCAFControl_Reader aReader;
  EXPECT_EQ(aReader.ReadStream("stepcaf_vertex.step", aStream), IFSelect_RetDone);
  occ::handle<TDocStd_Application> aRestoredApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aRestoredDocument;
  aRestoredApplication->NewDocument("BinXCAF", aRestoredDocument);
  EXPECT_TRUE(aReader.Transfer(aRestoredDocument));
  return aRestoredDocument;
}

static TDF_Label FindShapeByName(const NCollection_Sequence<TDF_Label>& theLabels,
                                 const char*                            theName)
{
  for (NCollection_Sequence<TDF_Label>::Iterator anIterator(theLabels); anIterator.More();
       anIterator.Next())
  {
    occ::handle<TDataStd_Name> aName;
    if (anIterator.Value().FindAttribute(TDataStd_Name::GetID(), aName)
        && aName->Get() == TCollection_ExtendedString(theName))
    {
      return anIterator.Value();
    }
  }
  return TDF_Label();
}

static TDF_Label FindLabelByName(const TDF_Label&                  theLabel,
                                 const TCollection_ExtendedString& theName)
{
  occ::handle<TDataStd_Name> aLabelName;
  if (theLabel.FindAttribute(TDataStd_Name::GetID(), aLabelName) && aLabelName->Get() == theName)
  {
    return theLabel;
  }

  for (TDF_ChildIterator anIterator(theLabel); anIterator.More(); anIterator.Next())
  {
    const TDF_Label aFoundLabel = FindLabelByName(anIterator.Value(), theName);
    if (!aFoundLabel.IsNull())
    {
      return aFoundLabel;
    }
  }
  return TDF_Label();
}

static TCollection_ExtendedString FindFirstLabelName(const TDF_Label& theLabel)
{
  occ::handle<TDataStd_Name> aLabelName;
  if (theLabel.FindAttribute(TDataStd_Name::GetID(), aLabelName))
  {
    return aLabelName->Get();
  }

  for (TDF_ChildIterator anIterator(theLabel); anIterator.More(); anIterator.Next())
  {
    const TCollection_ExtendedString aName = FindFirstLabelName(anIterator.Value());
    if (!aName.IsEmpty())
    {
      return aName;
    }
  }
  return TCollection_ExtendedString();
}

static TCollection_ExtendedString FindFirstSubShapeName(
  const occ::handle<TDocStd_Document>& theDocument)
{
  occ::handle<XCAFDoc_ShapeTool>  aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  NCollection_Sequence<TDF_Label> aShapeLabels;
  aShapeTool->GetShapes(aShapeLabels);
  for (NCollection_Sequence<TDF_Label>::Iterator aShapeIterator(aShapeLabels);
       aShapeIterator.More();
       aShapeIterator.Next())
  {
    NCollection_Sequence<TDF_Label> aSubShapeLabels;
    aShapeTool->GetSubShapes(aShapeIterator.Value(), aSubShapeLabels);
    for (NCollection_Sequence<TDF_Label>::Iterator aSubShapeIterator(aSubShapeLabels);
         aSubShapeIterator.More();
         aSubShapeIterator.Next())
    {
      const TCollection_ExtendedString aName = FindFirstLabelName(aSubShapeIterator.Value());
      if (!aName.IsEmpty())
      {
        return aName;
      }
    }
  }
  return TCollection_ExtendedString();
}

static TCollection_AsciiString MakeStepNameTemplate()
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  if (aDocument.IsNull())
  {
    return TCollection_AsciiString();
  }

  const TopoDS_Shape             aBox        = BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape();
  occ::handle<XCAFDoc_ShapeTool> aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label                aShapeLabel = aShapeTool->AddShape(aBox, false);
  if (aShapeLabel.IsNull())
  {
    return TCollection_AsciiString();
  }

  TopExp_Explorer aFaceExplorer(aBox, TopAbs_FACE);
  if (!aFaceExplorer.More())
  {
    return TCollection_AsciiString();
  }
  const TDF_Label aSubShapeLabel = aShapeTool->AddSubShape(aShapeLabel, aFaceExplorer.Current());
  if (aSubShapeLabel.IsNull()
      || TDataStd_Name::Set(aSubShapeLabel, TCollection_ExtendedString("@tmp_name@", true))
           .IsNull())
  {
    return TCollection_AsciiString();
  }

  occ::handle<DESTEP_ConfigurationNode> aNode  = new DESTEP_ConfigurationNode();
  aNode->InternalParameters.WriteSchema        = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  aNode->InternalParameters.WriteName          = true;
  aNode->InternalParameters.WriteSubshapeNames = true;
  DESTEP_Provider              aProvider(aNode);
  std::stringstream            aStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("codepage_template.step", aStream));
  if (!aProvider.Write(aWriteStreams, aDocument))
  {
    return TCollection_AsciiString();
  }

  return TCollection_AsciiString(aStream.str().c_str());
}

static TCollection_AsciiString EncodeStepName(const TCollection_ExtendedString& theName,
                                              const Resource_FormatType         theFormat)
{
  if (theFormat >= Resource_FormatType_CP1250 && theFormat <= Resource_FormatType_CP850)
  {
    TCollection_AsciiString aResult;
    for (int aCharIndex = 1; aCharIndex <= theName.Length(); ++aCharIndex)
    {
      const char16_t aChar = theName.Value(aCharIndex);
      if (aChar <= 0x7F)
      {
        aResult.AssignCat(static_cast<char>(aChar));
        continue;
      }

      char                             aCharBuffer[4];
      Standard_PCharacter              aCharBufferPtr = aCharBuffer;
      const TCollection_ExtendedString aSingleChar(1, aChar);
      if (!Resource_Unicode::ConvertUnicodeToFormat(theFormat, aSingleChar, aCharBufferPtr, 2))
      {
        return TCollection_AsciiString();
      }
      aResult.AssignCat(aCharBuffer);
    }
    return aResult;
  }

  char                aBuffer[512];
  Standard_PCharacter aBufferPtr = aBuffer;
  const int           aBufferSize =
    theFormat == Resource_FormatType_UTF8 ? theName.LengthOfCString() + 1 : sizeof(aBuffer);
  if (!Resource_Unicode::ConvertUnicodeToFormat(theFormat, theName, aBufferPtr, aBufferSize))
  {
    return TCollection_AsciiString();
  }
  return TCollection_AsciiString(aBuffer);
}

static TCollection_AsciiString ReplaceStepName(const TCollection_AsciiString&    theTemplate,
                                               const TCollection_ExtendedString& theName,
                                               const Resource_FormatType         theFormat)
{
  const TCollection_AsciiString aPlaceholder("@tmp_name@");
  const int                     aPosition     = theTemplate.Search(aPlaceholder);
  const TCollection_AsciiString anEncodedName = EncodeStepName(theName, theFormat);
  if (aPosition < 1 || anEncodedName.IsEmpty())
  {
    return TCollection_AsciiString();
  }

  TCollection_AsciiString aResult;
  if (aPosition > 1)
  {
    aResult.AssignCat(theTemplate.SubString(1, aPosition - 1));
  }
  aResult.AssignCat(anEncodedName);
  const int aSuffixPosition = aPosition + aPlaceholder.Length();
  if (aSuffixPosition <= theTemplate.Length())
  {
    aResult.AssignCat(theTemplate.SubString(aSuffixPosition, theTemplate.Length()));
  }
  return aResult;
}

static TCollection_ExtendedString ReadStepName(const TCollection_AsciiString& thePayload,
                                               const Resource_FormatType      theFormat)
{
  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  aNode->InternalParameters.ReadCodePage      = theFormat;
  aNode->InternalParameters.ReadName          = true;
  aNode->InternalParameters.ReadSubshapeNames = true;
  DESTEP_Provider aProvider(aNode);

  std::stringstream aStream(std::ios::in | std::ios::out | std::ios::binary);
  aStream.write(thePayload.ToCString(), thePayload.Length());
  aStream.seekg(0);
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("codepage_test.step", aStream));

  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  if (aDocument.IsNull() || !aProvider.Read(aReadStreams, aDocument))
  {
    return TCollection_ExtendedString();
  }
  return FindFirstSubShapeName(aDocument);
}

static void ExpectStepCodePageName(const TCollection_AsciiString&    theTemplate,
                                   const TCollection_ExtendedString& theExpectedName,
                                   const Resource_FormatType         theFormat)
{
  const TCollection_AsciiString aUtf8Payload =
    ReplaceStepName(theTemplate, theExpectedName, Resource_FormatType_UTF8);
  const TCollection_AsciiString aTargetPayload =
    ReplaceStepName(theTemplate, theExpectedName, theFormat);
  ASSERT_FALSE(aUtf8Payload.IsEmpty());
  ASSERT_FALSE(aTargetPayload.IsEmpty());

  EXPECT_EQ(ReadStepName(aUtf8Payload, Resource_FormatType_UTF8), theExpectedName);
  EXPECT_EQ(ReadStepName(aTargetPayload, theFormat), theExpectedName);
  EXPECT_NE(ReadStepName(aTargetPayload, Resource_FormatType_UTF8), theExpectedName);
}

static TopoDS_Shape TranslateShape(const TopoDS_Shape& theShape,
                                   const double        theX,
                                   const double        theY,
                                   const double        theZ)
{
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(theX, theY, theZ));
  return BRepBuilderAPI_Transform(theShape, aTrsf).Shape();
}

static TopoDS_Shape FindFirstVertex(const TDF_Label& theLabel)
{
  const TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(theLabel);
  if (!aShape.IsNull())
  {
    if (aShape.ShapeType() == TopAbs_VERTEX)
    {
      return aShape;
    }

    TopExp_Explorer anExplorer(aShape, TopAbs_VERTEX);
    if (anExplorer.More())
    {
      return anExplorer.Current();
    }
  }

  for (TDF_ChildIterator anIterator(theLabel); anIterator.More(); anIterator.Next())
  {
    const TopoDS_Shape aVertex = FindFirstVertex(anIterator.Value());
    if (!aVertex.IsNull())
    {
      return aVertex;
    }
  }
  return TopoDS_Shape();
}

static double ShapeDiagonal(const TopoDS_Shape& theShape)
{
  Bnd_Box aBox;
  BRepBndLib::Add(theShape, aBox);
  return aBox.CornerMin().Distance(aBox.CornerMax());
}
} // namespace

// gdt/dimensions/A7: STEP AP242 preserves dimension descriptions.
TEST(GDT_STEP_Storage_Test, A7_DimensionDescriptions)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                aDimensionLabel = AddBoxAndDimension(aDocument);
  occ::handle<XCAFDoc_Dimension> aDimension;
  ASSERT_TRUE(aDimensionLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension));
  occ::handle<XCAFDimTolObjects_DimensionObject> anObject = aDimension->GetObject();
  anObject->SetType(XCAFDimTolObjects_DimensionType_Size_CurveLength);
  anObject->SetValue(10.0);
  anObject->AddDescription(new TCollection_HAsciiString("description1"),
                           new TCollection_HAsciiString());
  anObject->AddDescription(new TCollection_HAsciiString("description2"),
                           new TCollection_HAsciiString("name2"));
  aDimension->SetObject(anObject);

  occ::handle<TDocStd_Document> aRestored = RoundTrip(aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<XCAFDimTolObjects_DimensionObject> aRestoredObject = RestoreDimension(aRestored);
  ASSERT_FALSE(aRestoredObject.IsNull());
  EXPECT_EQ(aRestoredObject->GetType(), XCAFDimTolObjects_DimensionType_Size_CurveLength);
  EXPECT_DOUBLE_EQ(aRestoredObject->GetValue(), 10.0);
  ASSERT_EQ(aRestoredObject->NbDescriptions(), 2);
  EXPECT_STREQ(aRestoredObject->GetDescription(0)->ToCString(), "description1");
  EXPECT_STREQ(aRestoredObject->GetDescriptionName(0)->ToCString(), "");
  EXPECT_STREQ(aRestoredObject->GetDescription(1)->ToCString(), "description2");
  EXPECT_STREQ(aRestoredObject->GetDescriptionName(1)->ToCString(), "name2");
}

// gdt/dimensions/A8: STEP AP242 preserves the dimension annotation plane.
TEST(GDT_STEP_Storage_Test, A8_DimensionAnnotationPlane)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                aDimensionLabel = AddBoxAndDimension(aDocument);
  occ::handle<XCAFDoc_Dimension> aDimension;
  ASSERT_TRUE(aDimensionLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension));
  occ::handle<XCAFDimTolObjects_DimensionObject> anObject = aDimension->GetObject();
  anObject->SetType(XCAFDimTolObjects_DimensionType_Size_CurveLength);
  anObject->SetValue(10.0);
  const gp_Pnt aPoint(-5.0, 0.0, 7.0);
  const gp_Dir aNormal(-1.0, 0.0, 0.0);
  const gp_Dir anXDirection(0.0, 1.0, 0.0);
  anObject->SetPlane(gp_Ax2(aPoint, aNormal, anXDirection));
  anObject->SetPointTextAttach(aPoint);
  anObject->SetPresentation(
    BRepBuilderAPI_MakeEdge(gp_Pnt(-5.0, 0.0, 0.0), gp_Pnt(-5.0, 0.0, 10.0)),
    new TCollection_HAsciiString("presentation"));
  aDimension->SetObject(anObject);

  occ::handle<TDocStd_Document> aRestored = RoundTrip(aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<XCAFDimTolObjects_DimensionObject> aRestoredObject = RestoreDimension(aRestored);
  ASSERT_FALSE(aRestoredObject.IsNull());
  EXPECT_DOUBLE_EQ(aRestoredObject->GetValue(), 10.0);
  ASSERT_TRUE(aRestoredObject->HasPlane());
  ExpectPoint(aRestoredObject->GetPlane().Location(), aPoint);
  ExpectDirection(aRestoredObject->GetPlane().Direction(), aNormal);
  ExpectDirection(aRestoredObject->GetPlane().XDirection(), anXDirection);
  ASSERT_TRUE(aRestoredObject->HasTextPoint());
  ExpectPoint(aRestoredObject->GetPointTextAttach(), aPoint);
}

// gdt/dimensions/A9: STEP AP242 preserves both dimension connection points.
TEST(GDT_STEP_Storage_Test, A9_DimensionConnectionPoints)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  const TDF_Label                aDimensionLabel = AddBoxAndDimension(aDocument);
  occ::handle<XCAFDoc_Dimension> aDimension;
  ASSERT_TRUE(aDimensionLabel.FindAttribute(XCAFDoc_Dimension::GetID(), aDimension));
  occ::handle<XCAFDimTolObjects_DimensionObject> anObject = aDimension->GetObject();
  anObject->SetType(XCAFDimTolObjects_DimensionType_Location_LinearDistance);
  anObject->SetValue(10.0);
  const gp_Pnt aPoint1(1.0, 0.0, 0.0);
  const gp_Pnt aPoint2(9.0, 0.0, 0.0);
  anObject->SetPoint(aPoint1);
  anObject->SetPoint2(aPoint2);
  aDimension->SetObject(anObject);

  occ::handle<TDocStd_Document> aRestored = RoundTrip(aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<XCAFDimTolObjects_DimensionObject> aRestoredObject = RestoreDimension(aRestored);
  ASSERT_FALSE(aRestoredObject.IsNull());
  EXPECT_DOUBLE_EQ(aRestoredObject->GetValue(), 10.0);
  ASSERT_TRUE(aRestoredObject->HasPoint());
  ASSERT_TRUE(aRestoredObject->HasPoint2());
  ExpectPoint(aRestoredObject->GetPoint(), aPoint1);
  ExpectPoint(aRestoredObject->GetPoint2(), aPoint2);
}

static TCollection_ExtendedString StepSpecialName()
{
  return TCollection_ExtendedString("a'''\\b\n\t\\c\\\\\\\\");
}

static TCollection_ExtendedString RestoredFirstShapeName(
  const occ::handle<TDocStd_Document>& theDocument)
{
  occ::handle<XCAFDoc_ShapeTool>  aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  NCollection_Sequence<TDF_Label> aShapeLabels;
  aShapeTool->GetShapes(aShapeLabels);
  if (aShapeLabels.IsEmpty())
  {
    return TCollection_ExtendedString();
  }

  occ::handle<TDataStd_Name> aName;
  if (!aShapeLabels.First().FindAttribute(TDataStd_Name::GetID(), aName))
  {
    return TCollection_ExtendedString();
  }
  return aName->Get();
}

// bugs/step/bug32310: STEP stream export/import preserves names containing
// quotes, backslashes, newlines, and tabs.
TEST(GDT_STEP_Storage_Test, StepBug_32310_SpecialName)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label aShapeLabel = aShapeTool->AddShape(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape());
  ASSERT_FALSE(aShapeLabel.IsNull());
  const TCollection_ExtendedString anExpected = StepSpecialName();
  ASSERT_FALSE(TDataStd_Name::Set(aShapeLabel, anExpected).IsNull());

  occ::handle<TDocStd_Document> aRestored = RoundTrip(aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  EXPECT_EQ(RestoredFirstShapeName(aRestored), anExpected);
}

// bugs/step/bug28454_1: STEP names encoded in ISO-8859-N are decoded using the selected code page.
TEST(GDT_STEP_Storage_Test, StepBug_28454_1_ISO8859Names)
{
  const TCollection_AsciiString aTemplate = MakeStepNameTemplate();
  ASSERT_FALSE(aTemplate.IsEmpty());

  NCollection_Array1<Resource_FormatType> aFormats(1, 9);
  aFormats.SetValue(1, Resource_FormatType_iso8859_1);
  aFormats.SetValue(2, Resource_FormatType_iso8859_2);
  aFormats.SetValue(3, Resource_FormatType_iso8859_3);
  aFormats.SetValue(4, Resource_FormatType_iso8859_4);
  aFormats.SetValue(5, Resource_FormatType_iso8859_5);
  aFormats.SetValue(6, Resource_FormatType_iso8859_6);
  aFormats.SetValue(7, Resource_FormatType_iso8859_7);
  aFormats.SetValue(8, Resource_FormatType_iso8859_8);
  aFormats.SetValue(9, Resource_FormatType_iso8859_9);

  NCollection_Array1<TCollection_ExtendedString> aNames(1, 9);
  aNames.SetValue(1, TCollection_ExtendedString(u"Test Prob\u00EDh\u00E1"));
  aNames.SetValue(2, TCollection_ExtendedString(u"Test Prob\u00EDh\u00E1"));
  aNames.SetValue(3, TCollection_ExtendedString(u"Test \u00D6l\u00E7ek"));
  aNames.SetValue(4, TCollection_ExtendedString(u"Test m\u0113\u0123iniet"));
  aNames.SetValue(5, TCollection_ExtendedString(u"Test \u0422\u0435\u0441\u0442"));
  aNames.SetValue(6, TCollection_ExtendedString(u"Test \u0627\u062e\u062a\u0628\u0627\u0631"));
  aNames.SetValue(7, TCollection_ExtendedString(u"Test \u03B4\u03C0\u03A8\u03AE"));
  aNames.SetValue(8, TCollection_ExtendedString(u"Test \u00B1\u05E4\u05DE\u05DC\u05E9"));
  aNames.SetValue(9, TCollection_ExtendedString(u"Test \u011F\u015F\u011E\u015E\u00C6"));

  for (int anIndex = aFormats.Lower(); anIndex <= aFormats.Upper(); ++anIndex)
  {
    SCOPED_TRACE(anIndex);
    ExpectStepCodePageName(aTemplate, aNames.Value(anIndex), aFormats.Value(anIndex));
  }
}

// bugs/step/bug30694: STEP names encoded in GB2312 are decoded using the GB code page.
TEST(GDT_STEP_Storage_Test, StepBug_30694_GB2312Name)
{
  const TCollection_AsciiString aTemplate = MakeStepNameTemplate();
  ASSERT_FALSE(aTemplate.IsEmpty());
  ExpectStepCodePageName(aTemplate,
                         TCollection_ExtendedString(u"\u56FD\u6807"),
                         Resource_FormatType_GB);
}

// bugs/step/bug31670_1: STEP names encoded in CP1250..CP1258 are decoded using the selected page.
TEST(GDT_STEP_Storage_Test, StepBug_31670_1_CP125xNames)
{
  const TCollection_AsciiString aTemplate = MakeStepNameTemplate();
  ASSERT_FALSE(aTemplate.IsEmpty());

  NCollection_Array1<Resource_FormatType>        aFormats(1, 9);
  NCollection_Array1<TCollection_ExtendedString> aNames(1, 9);
  for (int anIndex = 1; anIndex <= 9; ++anIndex)
  {
    aFormats.SetValue(anIndex,
                      static_cast<Resource_FormatType>(Resource_FormatType_CP1250 + anIndex - 1));
  }
  aNames.SetValue(1, TCollection_ExtendedString(u"Test Prob\u00EDh\u00E1"));
  aNames.SetValue(2, TCollection_ExtendedString(u"Test \u0422\u0435\u0441\u0442"));
  aNames.SetValue(3, TCollection_ExtendedString(u"Test Prob\u00EDh\u00E1"));
  aNames.SetValue(4, TCollection_ExtendedString(u"Test \u0394\u03BF\u03BA\u03B9\u03BC\u03AE"));
  aNames.SetValue(5, TCollection_ExtendedString(u"Test \u00D6l\u00E7ek"));
  aNames.SetValue(6,
                  TCollection_ExtendedString(u"Test \u05DE\u05B4\u05D1\u05B0\u05D7\u05B8\u05DF"));
  aNames.SetValue(7, TCollection_ExtendedString(u"Test \u0627\u062E\u062A\u0628\u0627\u0631"));
  aNames.SetValue(8, TCollection_ExtendedString(u"Test P\u0101rbaude"));
  // CP1258 represents 'ệ' as 'ê' followed by combining dot below.
  aNames.SetValue(9, TCollection_ExtendedString(u"Test Th\u00ED nghi\u00EA\u0323m"));

  for (int anIndex = aFormats.Lower(); anIndex <= aFormats.Upper(); ++anIndex)
  {
    SCOPED_TRACE(anIndex);
    ExpectStepCodePageName(aTemplate, aNames.Value(anIndex), aFormats.Value(anIndex));
  }
}

// bugs/step/bug31923: STEP names encoded in DOS CP850 are decoded using the selected page.
TEST(GDT_STEP_Storage_Test, StepBug_31923_CP850Name)
{
  const TCollection_AsciiString aTemplate = MakeStepNameTemplate();
  ASSERT_FALSE(aTemplate.IsEmpty());
  ExpectStepCodePageName(
    aTemplate,
    TCollection_ExtendedString(u"\u00DCberpr\u00FCfung de codifica\u00E7\u00E3o"),
    Resource_FormatType_CP850);
}

// bugs/xde/bug22728: STEP export can write the result to a caller-owned stream.
TEST(GDT_STEP_Storage_Test, XdeBug_22728_WriteStream)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  ASSERT_FALSE(aShapeTool->AddShape(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape()).IsNull());

  std::stringstream aStream;
  ASSERT_TRUE(WriteStepStream(aDocument, aStream));
  const std::string aContent = aStream.str();
  EXPECT_FALSE(aContent.empty());
  EXPECT_NE(aContent.find("ISO-10303-21;"), std::string::npos);
  EXPECT_NE(aContent.find("MANIFOLD_SOLID_BREP"), std::string::npos);
}

// bugs/step/bug33815: non-manifold STEP export retains the same special name.
TEST(GDT_STEP_Storage_Test, StepBug_33815_NonmanifoldSpecialName)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label aShapeLabel = aShapeTool->AddShape(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape());
  ASSERT_FALSE(aShapeLabel.IsNull());
  const TCollection_ExtendedString anExpected = StepSpecialName();
  ASSERT_FALSE(TDataStd_Name::Set(aShapeLabel, anExpected).IsNull());

  occ::handle<TDocStd_Document> aRestored = RoundTrip(aDocument, true);
  ASSERT_FALSE(aRestored.IsNull());
  EXPECT_EQ(RestoredFirstShapeName(aRestored), anExpected);
}

static void ExpectStepEdgeExport(const TopoDS_Shape& theEdge)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  ASSERT_FALSE(aShapeTool->AddShape(theEdge).IsNull());

  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  aNode->InternalParameters.WriteSchema       = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  DESTEP_Provider              aProvider(aNode);
  std::stringstream            aStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("untrimmed_edge.step", aStream));
  EXPECT_TRUE(aProvider.Write(aWriteStreams, aDocument));
  EXPECT_FALSE(aStream.str().empty());
  EXPECT_NE(aStream.str().find("LINE"), std::string::npos);
}

// bugs/step/bug32817_1: STEP export accepts an untrimmed line edge.
TEST(GDT_STEP_Storage_Test, StepBug_32817_1_UntrimmedLine)
{
  const gp_Lin       aLine(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  const TopoDS_Shape anEdge = BRepBuilderAPI_MakeEdge(aLine).Edge();
  ASSERT_FALSE(anEdge.IsNull());
  ExpectStepEdgeExport(anEdge);
}

// bugs/step/bug32817_2: STEP export accepts a line edge with a huge end value.
TEST(GDT_STEP_Storage_Test, StepBug_32817_2_HugeEndParameter)
{
  const gp_Lin       aLine(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  const TopoDS_Shape anEdge = BRepBuilderAPI_MakeEdge(aLine, 10.0, 2.0e100).Edge();
  ASSERT_FALSE(anEdge.IsNull());
  ExpectStepEdgeExport(anEdge);
}

// bugs/step/bug32817_3: STEP export accepts a line edge with a huge negative start value.
TEST(GDT_STEP_Storage_Test, StepBug_32817_3_HugeStartParameter)
{
  const gp_Lin       aLine(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 1.0, 0.0));
  const TopoDS_Shape anEdge = BRepBuilderAPI_MakeEdge(aLine, -2.0e100, 10.0).Edge();
  ASSERT_FALSE(anEdge.IsNull());
  ExpectStepEdgeExport(anEdge);
}

// bugs/xde/bug25910: STEP writing accepts a material with zero density.
TEST(GDT_STEP_Storage_Test, XdeBug_25910_ZeroDensityMaterial)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label aShapeLabel = aShapeTool->AddShape(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aShapeLabel.IsNull());
  occ::handle<XCAFDoc_MaterialTool> aMaterialTool =
    XCAFDoc_DocumentTool::MaterialTool(aDocument->Main());
  aMaterialTool->SetMaterial(aShapeLabel,
                             new TCollection_HAsciiString("Vacuum"),
                             new TCollection_HAsciiString(),
                             0.0,
                             new TCollection_HAsciiString("density measure"),
                             new TCollection_HAsciiString("POSITIVE_RATIO_MEASURE"));

  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  aNode->InternalParameters.WriteSchema       = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  DESTEP_Provider              aProvider(aNode);
  std::stringstream            aStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("zero_density.step", aStream));
  EXPECT_TRUE(aProvider.Write(aWriteStreams, aDocument));
  EXPECT_FALSE(aStream.str().empty());
}

// bugs/step/bug27313: STEP writing accepts a PMI document with a datum and
// angularity tolerance.
TEST(GDT_STEP_Storage_Test, StepBug_27313_PmiExport)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label aShapeLabel = aShapeTool->AddShape(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aShapeLabel.IsNull());
  occ::handle<XCAFDoc_DimTolTool> aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(aDocument->Main());
  const TDF_Label                 aToleranceLabel = aDimTolTool->AddGeomTolerance();
  aDimTolTool->SetGeomTolerance(aShapeLabel, aToleranceLabel);
  const TDF_Label                 aDatumLabel = aDimTolTool->AddDatum();
  NCollection_Sequence<TDF_Label> aShapeLabels;
  aShapeLabels.Append(aShapeLabel);
  aDimTolTool->SetDatum(aShapeLabels, aDatumLabel);

  occ::handle<XCAFDoc_Datum> aDatum;
  ASSERT_TRUE(aDatumLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum));
  occ::handle<XCAFDimTolObjects_DatumObject> aDatumObject = aDatum->GetObject();
  aDatumObject->SetName(new TCollection_HAsciiString("A"));
  aDatumObject->SetPosition(1);
  aDatumObject->AddModifier(XCAFDimTolObjects_DatumSingleModif_DistanceVariable);
  aDatum->SetObject(aDatumObject);
  aDimTolTool->SetDatumToGeomTol(aDatumLabel, aToleranceLabel);

  occ::handle<XCAFDoc_GeomTolerance> aTolerance;
  ASSERT_TRUE(aToleranceLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aTolerance));
  occ::handle<XCAFDimTolObjects_GeomToleranceObject> aToleranceObject = aTolerance->GetObject();
  aToleranceObject->SetValue(0.5);
  aToleranceObject->SetType(XCAFDimTolObjects_GeomToleranceType_Angularity);
  aToleranceObject->SetTypeOfValue(XCAFDimTolObjects_GeomToleranceTypeValue_Diameter);
  aTolerance->SetObject(aToleranceObject);

  std::stringstream aStream;
  EXPECT_TRUE(WriteStepStream(aDocument, aStream));
  EXPECT_FALSE(aStream.str().empty());
}

// bugs/modalg_8/bug33165: STEP preserves names of assembly instances.
TEST(GDT_STEP_Storage_Test, ModalgBug_33165_AssemblyInstanceNames)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const TopoDS_Shape aFirstBox = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();
  gp_Trsf            aTranslation;
  aTranslation.SetTranslation(gp_Vec(1.0, 1.0, 1.0));
  const TopoDS_Shape aSecondBox = BRepBuilderAPI_Transform(aFirstBox, aTranslation).Shape();
  BRep_Builder       aBuilder;
  TopoDS_Compound    aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aFirstBox);
  aBuilder.Add(aCompound, aSecondBox);

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label                anAssembly = aShapeTool->AddShape(aCompound, true);
  ASSERT_FALSE(anAssembly.IsNull());
  NCollection_Sequence<TDF_Label> aComponents;
  ASSERT_TRUE(aShapeTool->GetComponents(anAssembly, aComponents));
  ASSERT_EQ(aComponents.Length(), 2);
  ASSERT_FALSE(TDataStd_Name::Set(aComponents.Value(1), "name1").IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aComponents.Value(2), "name2").IsNull());

  occ::handle<TDocStd_Document> aRestored = RoundTrip(aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<XCAFDoc_ShapeTool> aRestoredShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aRestored->Main());
  NCollection_Sequence<TDF_Label> aRestoredAssemblies;
  aRestoredShapeTool->GetFreeShapes(aRestoredAssemblies);
  ASSERT_EQ(aRestoredAssemblies.Length(), 1);
  NCollection_Sequence<TDF_Label> aRestoredComponents;
  ASSERT_TRUE(aRestoredShapeTool->GetComponents(aRestoredAssemblies.First(), aRestoredComponents));
  ASSERT_EQ(aRestoredComponents.Length(), 2);

  occ::handle<TDataStd_Name> aName1;
  occ::handle<TDataStd_Name> aName2;
  ASSERT_TRUE(aRestoredComponents.Value(1).FindAttribute(TDataStd_Name::GetID(), aName1));
  ASSERT_TRUE(aRestoredComponents.Value(2).FindAttribute(TDataStd_Name::GetID(), aName2));
  EXPECT_EQ(aName1->Get(), TCollection_ExtendedString("name1"));
  EXPECT_EQ(aName2->Get(), TCollection_ExtendedString("name2"));
}

// bugs/xde/bug7141: STEP export handles a large assembly of located instances
// when the output is written to a caller-owned stream.
TEST(GDT_STEP_Storage_Test, XdeBug_7141_LargeAssemblyExport)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const TopoDS_Compound          aCompound  = MakeLocatedBoxAssembly(20);
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label                anAssembly = aShapeTool->AddShape(aCompound, true);
  ASSERT_FALSE(anAssembly.IsNull());
  NCollection_Sequence<TDF_Label> aComponents;
  ASSERT_TRUE(aShapeTool->GetComponents(anAssembly, aComponents));
  ASSERT_EQ(aComponents.Length(), 400);

  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  aNode->InternalParameters.WriteSchema       = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  aNode->InternalParameters.WriteAssembly     = DESTEP_Parameters::WriteMode_Assembly_On;
  DESTEP_Provider              aProvider(aNode);
  std::stringstream            aStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("bug7141.step", aStream));
  ASSERT_TRUE(aProvider.Write(aWriteStreams, aDocument));

  const std::string aContent = aStream.str();
  EXPECT_FALSE(aContent.empty());
  EXPECT_NE(aContent.find("ISO-10303-21;"), std::string::npos);
  EXPECT_NE(aContent.find("NEXT_ASSEMBLY_USAGE_OCCURRENCE"), std::string::npos);
  EXPECT_NE(aContent.find("MANIFOLD_SOLID_BREP"), std::string::npos);
}

// bugs/step/bug30189_2: STEP preserves the top-level assembly structure and sharing.
TEST(GDT_STEP_Storage_Test, StepBug_30189_2_LocatedRootStructure)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();
  const TopoDS_Shape aSecondBox =
    BRepPrimAPI_MakeBox(gp_Pnt(2.0, 0.0, 0.0), gp_Pnt(3.0, 1.0, 1.0)).Shape();
  const TopoDS_Shape aThirdBox =
    BRepPrimAPI_MakeBox(gp_Pnt(4.0, 0.0, 0.0), gp_Pnt(5.0, 1.0, 1.0)).Shape();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aSecondBox);
  const TopoDS_Shape aLocatedCompound = TranslateShape(aCompound, 0.0, 2.0, 0.0);

  TopoDS_Compound aRootCompound;
  aBuilder.MakeCompound(aRootCompound);
  aBuilder.Add(aRootCompound, aThirdBox);
  aBuilder.Add(aRootCompound, aCompound);

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  ASSERT_FALSE(aShapeTool->AddShape(aRootCompound).IsNull());
  ASSERT_FALSE(aShapeTool->AddShape(aLocatedCompound).IsNull());

  NCollection_Sequence<TDF_Label> aShapes;
  aShapeTool->GetShapes(aShapes);
  ASSERT_EQ(aShapes.Length(), 6);
  NCollection_Sequence<TDF_Label> aUsers;
  EXPECT_EQ(XCAFDoc_ShapeTool::GetUsers(aShapes.Value(3), aUsers), 2);

  occ::handle<TDocStd_Document> aRestored = RoundTrip(aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<XCAFDoc_ShapeTool> aRestoredShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aRestored->Main());
  NCollection_Sequence<TDF_Label> aRestoredShapes;
  aRestoredShapeTool->GetShapes(aRestoredShapes);
  ASSERT_EQ(aRestoredShapes.Length(), 6);
  NCollection_Sequence<TDF_Label> aRestoredUsers;
  EXPECT_EQ(XCAFDoc_ShapeTool::GetUsers(aRestoredShapes.Value(3), aRestoredUsers), 2);
  EXPECT_EQ(XCAFDoc_ShapeTool::NbComponents(aRestoredShapes.Value(6)), 1);
}

// bugs/xde/bug1669: STEP preserves names throughout a nested assembly.
TEST(GDT_STEP_Storage_Test, XdeBug_1669_NestedAssemblyNames)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const TopoDS_Shape aBox1 =
    TranslateShape(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape(), 10.0, 0.0, 0.0);
  const TopoDS_Shape aBox2 =
    TranslateShape(BRepPrimAPI_MakeBox(20.0, 10.0, 10.0).Shape(), 20.0, 0.0, 0.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aBlock;
  aBuilder.MakeCompound(aBlock);
  aBuilder.Add(aBlock, aBox1);
  aBuilder.Add(aBlock, aBox2);

  const TopoDS_Shape aNextBlock = TranslateShape(aBlock, 30.0, 0.0, 0.0);

  TopoDS_Compound aRow;
  aBuilder.MakeCompound(aRow);
  aBuilder.Add(aRow, aBlock);
  aBuilder.Add(aRow, aNextBlock);

  const TopoDS_Shape aHalfBrick = TranslateShape(aRow, -10.0, 0.0, 10.0);
  const TopoDS_Shape aBrick     = TranslateShape(aRow, 0.0, 0.0, 20.0);

  TopoDS_Compound aWall;
  aBuilder.MakeCompound(aWall);
  aBuilder.Add(aWall, aRow);
  aBuilder.Add(aWall, aHalfBrick);
  aBuilder.Add(aWall, aBrick);

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label                aWallLabel = aShapeTool->AddShape(aWall, true);
  ASSERT_FALSE(aWallLabel.IsNull());

  NCollection_Sequence<TDF_Label> aShapes;
  aShapeTool->GetShapes(aShapes);
  ASSERT_EQ(aShapes.Length(), 5);
  EXPECT_FALSE(TDataStd_Name::Set(aShapes.Value(1), "Wall").IsNull());
  EXPECT_FALSE(TDataStd_Name::Set(aShapes.Value(2), "Block-Array").IsNull());
  EXPECT_FALSE(TDataStd_Name::Set(aShapes.Value(3), "Block").IsNull());
  EXPECT_FALSE(TDataStd_Name::Set(aShapes.Value(4), "Half-Brick").IsNull());
  EXPECT_FALSE(TDataStd_Name::Set(aShapes.Value(5), "Brick").IsNull());

  NCollection_Sequence<TDF_Label> aComponents;
  ASSERT_TRUE(aShapeTool->GetComponents(aShapes.Value(1), aComponents));
  ASSERT_EQ(aComponents.Length(), 3);
  EXPECT_FALSE(TDataStd_Name::Set(aComponents.Value(1), "base-array").IsNull());
  EXPECT_FALSE(TDataStd_Name::Set(aComponents.Value(2), "next-array").IsNull());
  EXPECT_FALSE(TDataStd_Name::Set(aComponents.Value(3), "top-array").IsNull());

  aComponents.Clear();
  ASSERT_TRUE(aShapeTool->GetComponents(aShapes.Value(2), aComponents));
  ASSERT_EQ(aComponents.Length(), 2);
  EXPECT_FALSE(TDataStd_Name::Set(aComponents.Value(1), "left-block").IsNull());
  EXPECT_FALSE(TDataStd_Name::Set(aComponents.Value(2), "right-block").IsNull());

  aComponents.Clear();
  ASSERT_TRUE(aShapeTool->GetComponents(aShapes.Value(3), aComponents));
  ASSERT_EQ(aComponents.Length(), 2);
  EXPECT_FALSE(TDataStd_Name::Set(aComponents.Value(1), "half-brick").IsNull());
  EXPECT_FALSE(TDataStd_Name::Set(aComponents.Value(2), "brick").IsNull());

  occ::handle<TDocStd_Document> aRestored = RoundTrip(aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  const TDF_Label aRestoredRoot = aRestored->Main();
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "Wall").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "Block-Array").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "Block").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "Half-Brick").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "Brick").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "base-array").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "next-array").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "top-array").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "left-block").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "right-block").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "half-brick").IsNull());
  EXPECT_FALSE(FindLabelByName(aRestoredRoot, "brick").IsNull());
}

// bugs/step/bug_ocp1949_2: STEP tessellated export applies the document unit scaling.
TEST(GDT_STEP_Storage_Test, StepBug_Ocp1949_2_TessellatedScaling)
{
  const TopoDS_Shape       aBox = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();
  BRepMesh_IncrementalMesh aMesh(aBox, 1.0);
  ASSERT_TRUE(aMesh.IsDone());

  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  XCAFDoc_DocumentTool::SetLengthUnit(aDocument, 1.0);
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  ASSERT_FALSE(aShapeTool->AddShape(aBox).IsNull());

  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  aNode->InternalParameters.WriteSchema       = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  aNode->InternalParameters.WriteTessellated  = DESTEP_Parameters::RWMode_Tessellated_On;
  DESTEP_Provider aProvider(aNode);

  std::stringstream            aStream;
  DE_Provider::WriteStreamList aWriteStreams;
  aWriteStreams.Append(DE_Provider::WriteStreamNode("ocp1949_2.step", aStream));
  ASSERT_TRUE(aProvider.Write(aWriteStreams, aDocument));
  ASSERT_FALSE(aStream.str().empty());
  EXPECT_NE(aStream.str().find("TESSELLATED_SHAPE_REPRESENTATION"), std::string::npos);

  aStream.seekg(0);
  occ::handle<TDocStd_Application> aRestoredApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aRestoredDocument    = NewDocument(aRestoredApplication);
  ASSERT_FALSE(aRestoredDocument.IsNull());
  DE_Provider::ReadStreamList aReadStreams;
  aReadStreams.Append(DE_Provider::ReadStreamNode("ocp1949_2.step", aStream));
  ASSERT_TRUE(aProvider.Read(aReadStreams, aRestoredDocument));

  occ::handle<XCAFDoc_ShapeTool> aRestoredShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aRestoredDocument->Main());
  const TopoDS_Shape aRestoredShape = aRestoredShapeTool->GetOneShape();
  ASSERT_FALSE(aRestoredShape.IsNull());
  EXPECT_NEAR(ShapeDiagonal(aBox) * 1000.0, ShapeDiagonal(aRestoredShape), 0.1);
}

// bugs/step/bug30189_3: STEP preserves names and sharing of located roots.
TEST(GDT_STEP_Storage_Test, StepBug_30189_3_LocatedRootSharing)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());

  const TopoDS_Shape             aBaseBox = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();
  gp_Trsf                        aTranslation;
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label                aBaseLabel = aShapeTool->AddShape(aBaseBox);
  ASSERT_FALSE(aBaseLabel.IsNull());

  aTranslation.SetTranslation(gp_Vec(2.0, 0.0, 0.0));
  const TDF_Label aReference1 =
    aShapeTool->AddShape(BRepBuilderAPI_Transform(aBaseBox, aTranslation).Shape());
  aTranslation.SetTranslation(gp_Vec(0.0, 2.0, 0.0));
  const TDF_Label aReference2 =
    aShapeTool->AddShape(BRepBuilderAPI_Transform(aBaseBox, aTranslation).Shape());
  aTranslation.SetTranslation(gp_Vec(0.0, 0.0, 2.0));
  const TDF_Label aReference3 =
    aShapeTool->AddShape(BRepBuilderAPI_Transform(aBaseBox, aTranslation).Shape());
  ASSERT_FALSE(aReference1.IsNull());
  ASSERT_FALSE(aReference2.IsNull());
  ASSERT_FALSE(aReference3.IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aBaseLabel, "box").IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aReference1, "ref1_box").IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aReference2, "ref2_box").IsNull());
  ASSERT_FALSE(TDataStd_Name::Set(aReference3, "ref3_box").IsNull());

  NCollection_Sequence<TDF_Label> aShapes;
  aShapeTool->GetShapes(aShapes);
  ASSERT_EQ(aShapes.Length(), 4);
  NCollection_Sequence<TDF_Label> aUsers;
  EXPECT_EQ(XCAFDoc_ShapeTool::GetUsers(aBaseLabel, aUsers), 3);

  occ::handle<TDocStd_Document> aRestored = RoundTrip(aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<XCAFDoc_ShapeTool> aRestoredShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aRestored->Main());
  NCollection_Sequence<TDF_Label> aRestoredShapes;
  aRestoredShapeTool->GetShapes(aRestoredShapes);
  ASSERT_EQ(aRestoredShapes.Length(), 4);

  const TDF_Label aRestoredBase = FindShapeByName(aRestoredShapes, "box");
  ASSERT_FALSE(aRestoredBase.IsNull());
  ASSERT_FALSE(FindShapeByName(aRestoredShapes, "ref1_box").IsNull());
  ASSERT_FALSE(FindShapeByName(aRestoredShapes, "ref2_box").IsNull());
  ASSERT_FALSE(FindShapeByName(aRestoredShapes, "ref3_box").IsNull());
  NCollection_Sequence<TDF_Label> aRestoredUsers;
  EXPECT_EQ(XCAFDoc_ShapeTool::GetUsers(aRestoredBase, aRestoredUsers), 3);
}

// bugs/xde/bug13175: STEP preserves a document containing a single vertex.
TEST(GDT_STEP_Storage_Test, XdeBug_13175_SingleVertex)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<XCAFDoc_ShapeTool> aShapeTool   = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label                aVertexLabel = aShapeTool->NewShape();
  aShapeTool->SetShape(aVertexLabel, BRepBuilderAPI_MakeVertex(gp_Pnt(10.0, 10.0, 10.0)).Shape());

  occ::handle<TDocStd_Document> aRestored = RoundTripWithSTEPCAF(aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  occ::handle<XCAFDoc_ShapeTool> aRestoredShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aRestored->Main());
  NCollection_Sequence<TDF_Label> aShapes;
  aRestoredShapeTool->GetShapes(aShapes);
  ASSERT_EQ(aShapes.Length(), 1);
  const TopoDS_Shape aVertex = FindFirstVertex(aShapes.First());
  ASSERT_EQ(aVertex.ShapeType(), TopAbs_VERTEX);
  const gp_Pnt aPoint = BRep_Tool::Pnt(TopoDS::Vertex(aVertex));
  EXPECT_DOUBLE_EQ(aPoint.X(), 10.0);
  EXPECT_DOUBLE_EQ(aPoint.Y(), 10.0);
  EXPECT_DOUBLE_EQ(aPoint.Z(), 10.0);
}

// bugs/xde/bug31851: STEP preserves Unicode shape names.
TEST(GDT_STEP_Storage_Test, XdeBug_31851_UnicodeShapeName)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label aShapeLabel = aShapeTool->AddShape(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aShapeLabel.IsNull());
  const TCollection_ExtendedString anExpected(u8"ボックス", true);
  ASSERT_FALSE(TDataStd_Name::Set(aShapeLabel, anExpected).IsNull());

  occ::handle<TDocStd_Document> aRestored = RoundTrip(aDocument);
  ASSERT_FALSE(aRestored.IsNull());
  EXPECT_EQ(RestoredFirstShapeName(aRestored), anExpected);
}
