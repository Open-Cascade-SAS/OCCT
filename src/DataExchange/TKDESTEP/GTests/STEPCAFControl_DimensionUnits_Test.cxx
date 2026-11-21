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

#include <BRepPrimAPI_MakeBox.hxx>
#include <DESTEP_Parameters.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <NCollection_Sequence.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <Standard_Transient.hxx>
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_SiUnitAndPlaneAngleUnit.hxx>
#include <StepData_StepModel.hxx>
#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>
#include <StepShape_ToleranceValue.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Document.hxx>
#include <UnitsMethods_LengthUnit.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <gtest/gtest.h>

#include <sstream>

namespace
{
struct DimensionMeasures
{
  occ::handle<StepBasic_MeasureWithUnit> Nominal;
  occ::handle<StepBasic_MeasureWithUnit> LowerLimit;
  occ::handle<StepBasic_MeasureWithUnit> UpperLimit;
  occ::handle<StepBasic_MeasureWithUnit> LowerTolerance;
  occ::handle<StepBasic_MeasureWithUnit> UpperTolerance;
};

//==================================================================================================

occ::handle<TDocStd_Document> createDimensionDocument(const bool theIsAngle, const bool theIsRange)
{
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  // Length inputs are in centimetres.
  XCAFDoc_DocumentTool::SetLengthUnit(aDocument, 0.01);
  const occ::handle<XCAFDoc_ShapeTool> aShapes = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label aShape = aShapes->AddShape(BRepPrimAPI_MakeBox(2.54, 1.0, 1.0).Shape());
  const occ::handle<XCAFDoc_DimTolTool> aTool = XCAFDoc_DocumentTool::DimTolTool(aDocument->Main());
  const TDF_Label                       aLabel = aTool->AddDimension();
  aTool->SetDimension(aShape, aLabel);

  const occ::handle<XCAFDimTolObjects_DimensionObject> anObject =
    new XCAFDimTolObjects_DimensionObject;
  anObject->SetType(theIsAngle ? XCAFDimTolObjects_DimensionType_Size_Angular
                               : XCAFDimTolObjects_DimensionType_Size_CurveLength);
  anObject->SetValue(theIsAngle ? 45.0 : 2.54);
  if (theIsRange)
  {
    anObject->SetLowerBound(theIsAngle ? 30.0 : 1.27);
    anObject->SetUpperBound(theIsAngle ? 60.0 : 3.81);
  }
  else
  {
    anObject->SetLowerTolValue(theIsAngle ? 5.0 : 0.127);
    anObject->SetUpperTolValue(theIsAngle ? 10.0 : 0.254);
  }
  XCAFDoc_Dimension::Set(aLabel)->SetObject(anObject);
  return aDocument;
}

//==================================================================================================

bool transferDimension(STEPCAFControl_Writer& theWriter,
                       const bool             theIsAngle,
                       const bool             theIsRange)
{
  DESTEP_Parameters aParameters;
  aParameters.WriteSchema = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  aParameters.WriteUnit   = UnitsMethods_LengthUnit_Inch;
  return theWriter.Transfer(createDimensionDocument(theIsAngle, theIsRange), aParameters);
}

//==================================================================================================

DimensionMeasures readDimensionMeasures(const occ::handle<StepData_StepModel>& theModel)
{
  DimensionMeasures aResult;
  for (int anIndex = 1; anIndex <= theModel->NbEntities(); ++anIndex)
  {
    const occ::handle<Standard_Transient>&                 anEntity = theModel->Entity(anIndex);
    const occ::handle<StepRepr_ReprItemAndMeasureWithUnit> aMeasure =
      occ::down_cast<StepRepr_ReprItemAndMeasureWithUnit>(anEntity);
    if (!aMeasure.IsNull() && !aMeasure->Name().IsNull())
    {
      const TCollection_AsciiString& aName = aMeasure->Name()->String();
      if (aName.IsEqual("nominal value"))
      {
        aResult.Nominal = aMeasure->GetMeasureWithUnit();
      }
      else if (aName.IsEqual("lower limit"))
      {
        aResult.LowerLimit = aMeasure->GetMeasureWithUnit();
      }
      else if (aName.IsEqual("upper limit"))
      {
        aResult.UpperLimit = aMeasure->GetMeasureWithUnit();
      }
    }
    const occ::handle<StepShape_ToleranceValue> aTolerance =
      occ::down_cast<StepShape_ToleranceValue>(anEntity);
    if (!aTolerance.IsNull())
    {
      aResult.LowerTolerance = occ::down_cast<StepBasic_MeasureWithUnit>(aTolerance->LowerBound());
      aResult.UpperTolerance = occ::down_cast<StepBasic_MeasureWithUnit>(aTolerance->UpperBound());
    }
  }
  return aResult;
}

//==================================================================================================

void checkMeasure(const occ::handle<StepBasic_MeasureWithUnit>& theMeasure,
                  const bool                                    theIsAngle,
                  const bool                                    theIsTolerance,
                  const double                                  theExpected)
{
  ASSERT_FALSE(theMeasure.IsNull());
  EXPECT_NEAR(theMeasure->ValueComponent(), theExpected, 1.0e-12);
  ASSERT_FALSE(theMeasure->ValueComponentMember().IsNull());
  const char* aName = theIsAngle
                        ? (theIsTolerance ? "PLANE_ANGLE_MEASURE" : "POSITIVE_PLANE_ANGLE_MEASURE")
                        : (theIsTolerance ? "LENGTH_MEASURE" : "POSITIVE_LENGTH_MEASURE");
  EXPECT_STREQ(theMeasure->ValueComponentMember()->Name(), aName);
  const occ::handle<StepBasic_NamedUnit> aUnit = theMeasure->UnitComponent().NamedUnit();
  if (theIsAngle)
  {
    const occ::handle<StepBasic_SiUnitAndPlaneAngleUnit> anAngle =
      occ::down_cast<StepBasic_SiUnitAndPlaneAngleUnit>(aUnit);
    ASSERT_FALSE(anAngle.IsNull());
    EXPECT_EQ(anAngle->Name(), StepBasic_sunRadian);
    EXPECT_FALSE(anAngle->HasPrefix());
  }
  else
  {
    const occ::handle<StepBasic_ConversionBasedUnitAndLengthUnit> aLength =
      occ::down_cast<StepBasic_ConversionBasedUnitAndLengthUnit>(aUnit);
    ASSERT_FALSE(aLength.IsNull());
    ASSERT_FALSE(aLength->Name().IsNull());
    EXPECT_STREQ(aLength->Name()->ToCString(), "INCH");
  }
}
} // namespace

TEST(STEPCAFControl_DimensionUnitsTest, WritesInchAndRadianLiterals)
{
  STEPCAFControl_Controller::Init();
  for (const bool isAngle : {false, true})
  {
    for (const bool isRange : {false, true})
    {
      SCOPED_TRACE(::testing::Message() << "angle=" << isAngle << " range=" << isRange);
      STEPCAFControl_Writer aWriter;
      ASSERT_TRUE(transferDimension(aWriter, isAngle, isRange));
      const DimensionMeasures aMeasures = readDimensionMeasures(aWriter.ChangeWriter().Model());
      checkMeasure(aMeasures.Nominal, isAngle, false, isAngle ? 0.7853981633974483 : 1.0);
      if (isRange)
      {
        checkMeasure(aMeasures.LowerLimit, isAngle, false, isAngle ? 0.5235987755982988 : 0.5);
        checkMeasure(aMeasures.UpperLimit, isAngle, false, isAngle ? 1.0471975511965976 : 1.5);
      }
      else
      {
        checkMeasure(aMeasures.LowerTolerance,
                     isAngle,
                     true,
                     isAngle ? -0.0872664625997165 : -0.05);
        checkMeasure(aMeasures.UpperTolerance, isAngle, true, isAngle ? 0.174532925199433 : 0.1);
      }
    }
  }
}

TEST(STEPCAFControl_DimensionUnitsTest, ReadsLiteralGenericRadianTolerance)
{
  STEPCAFControl_Controller::Init();
  STEPCAFControl_Writer aWriter;
  ASSERT_TRUE(transferDimension(aWriter, true, false));
  const DimensionMeasures aMeasures = readDimensionMeasures(aWriter.ChangeWriter().Model());
  ASSERT_FALSE(aMeasures.Nominal.IsNull());
  ASSERT_FALSE(aMeasures.LowerTolerance.IsNull());
  ASSERT_FALSE(aMeasures.UpperTolerance.IsNull());
  // Author the radian values independently of the writer's scaling.
  aMeasures.Nominal->ValueComponentMember()->SetReal(0.7853981633974483);
  aMeasures.LowerTolerance->ValueComponentMember()->SetName("PLANE_ANGLE_MEASURE");
  aMeasures.LowerTolerance->ValueComponentMember()->SetReal(-0.0872664625997165);
  aMeasures.UpperTolerance->ValueComponentMember()->SetName("PLANE_ANGLE_MEASURE");
  aMeasures.UpperTolerance->ValueComponentMember()->SetReal(0.174532925199433);
  std::stringstream aStream;
  ASSERT_EQ(aWriter.WriteStream(aStream), IFSelect_RetDone);
  aStream.seekg(0);

  STEPCAFControl_Reader aReader;
  ASSERT_EQ(aReader.ReadStream("angular-dimension.stp", aStream), IFSelect_RetDone);
  const occ::handle<TDocStd_Document> aResult = new TDocStd_Document("BinXCAF");
  ASSERT_TRUE(aReader.Transfer(aResult));
  NCollection_Sequence<TDF_Label> aLabels;
  XCAFDoc_DocumentTool::DimTolTool(aResult->Main())->GetDimensionLabels(aLabels);
  ASSERT_EQ(aLabels.Length(), 1);
  occ::handle<XCAFDoc_Dimension> anAttribute;
  ASSERT_TRUE(aLabels.First().FindAttribute(XCAFDoc_Dimension::GetID(), anAttribute));
  const occ::handle<XCAFDimTolObjects_DimensionObject> anObject = anAttribute->GetObject();
  ASSERT_FALSE(anObject.IsNull());
  ASSERT_TRUE(anObject->IsDimWithPlusMinusTolerance());
  EXPECT_NEAR(anObject->GetValue(), 45.0, 1.0e-10);
  EXPECT_NEAR(anObject->GetLowerTolValue(), 5.0, 1.0e-10);
  EXPECT_NEAR(anObject->GetUpperTolValue(), 10.0, 1.0e-10);
}
