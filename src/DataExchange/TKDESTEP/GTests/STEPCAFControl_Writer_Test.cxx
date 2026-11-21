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
#include <BRep_Builder.hxx>
#include <DESTEP_Parameters.hxx>
#include <Precision.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_ExternFile.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPConstruct_UnitContext.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_PlaneAngleMeasureWithUnit.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepDimTol_RunoutZoneDefinition.hxx>
#include <StepDimTol_RunoutZoneOrientation.hxx>
#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>
#include <StepShape_ToleranceValue.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <TopoDS_Compound.hxx>
#include <UnitsMethods_LengthUnit.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSControl_WorkSession.hxx>

#include <gtest/gtest.h>

#include <cmath>

//==================================================================================================

TEST(STEPCAFControl_WriterTest, PlusMinusTolerance_PreservesMeasureType)
{
  STEPCAFControl_Controller::Init();
  for (const bool isAngle : {false, true})
  {
    SCOPED_TRACE(isAngle);
    const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
    XCAFDoc_DocumentTool::SetLengthUnit(aDocument, 0.001);
    const occ::handle<XCAFDoc_ShapeTool> aShapes =
      XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
    const TDF_Label aShape = aShapes->AddShape(BRepPrimAPI_MakeBox(10, 10, 10).Shape());
    const occ::handle<XCAFDoc_DimTolTool> aTool =
      XCAFDoc_DocumentTool::DimTolTool(aDocument->Main());
    const TDF_Label aLabel = aTool->AddDimension();
    aTool->SetDimension(aShape, aLabel);
    const occ::handle<XCAFDimTolObjects_DimensionObject> anObject =
      new XCAFDimTolObjects_DimensionObject;
    anObject->SetType(isAngle ? XCAFDimTolObjects_DimensionType_Size_Angular
                              : XCAFDimTolObjects_DimensionType_Size_Diameter);
    anObject->SetValue(isAngle ? 45.0 : 8.0);
    anObject->SetUpperTolValue(0.1);
    anObject->SetLowerTolValue(0.05);
    XCAFDoc_Dimension::Set(aLabel)->SetObject(anObject);
    DESTEP_Parameters aParameters;
    aParameters.WriteSchema = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
    STEPCAFControl_Writer aWriter;
    ASSERT_TRUE(aWriter.Transfer(aDocument, aParameters));
    const occ::handle<StepData_StepModel> aModel      = aWriter.ChangeWriter().Model();
    int                                   aTolerances = 0;
    for (int anIndex = 1; anIndex <= aModel->NbEntities(); ++anIndex)
    {
      const occ::handle<StepShape_ToleranceValue> aTolerance =
        occ::down_cast<StepShape_ToleranceValue>(aModel->Entity(anIndex));
      if (aTolerance.IsNull())
      {
        continue;
      }
      ++aTolerances;
      const occ::handle<StepBasic_MeasureWithUnit> aLower =
        occ::down_cast<StepBasic_MeasureWithUnit>(aTolerance->LowerBound());
      const occ::handle<StepBasic_MeasureWithUnit> anUpper =
        occ::down_cast<StepBasic_MeasureWithUnit>(aTolerance->UpperBound());
      ASSERT_FALSE(aLower.IsNull());
      ASSERT_FALSE(anUpper.IsNull());
      for (const auto& aMeasure : {aLower, anUpper})
      {
        ASSERT_FALSE(aMeasure->ValueComponentMember().IsNull());
        EXPECT_STREQ(aMeasure->ValueComponentMember()->Name(),
                     isAngle ? "PLANE_ANGLE_MEASURE" : "LENGTH_MEASURE");
      }
      STEPConstruct_UnitContext aContext;
      ASSERT_EQ(aContext.ComputeFactors(aLower->UnitComponent().NamedUnit(), StepData_Factors()),
                0);
      const double aFactor =
        isAngle ? aContext.PlaneAngleFactor() * 180.0 / M_PI : aContext.LengthFactor();
      const double aToleranceValue = isAngle ? Precision::Angular() : Precision::Confusion();
      EXPECT_NEAR(aLower->ValueComponent() * aFactor, -0.05, aToleranceValue);
      EXPECT_NEAR(anUpper->ValueComponent() * aFactor, 0.1, aToleranceValue);
    }
    EXPECT_EQ(aTolerances, 1);
  }
}

//==================================================================================================

TEST(STEPCAFControl_WriterTest, LengthDimensionUsesOutputUnit)
{
  STEPCAFControl_Controller::Init();
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  XCAFDoc_DocumentTool::SetLengthUnit(aDocument, 0.001);
  const occ::handle<XCAFDoc_ShapeTool> aShapes = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  const TDF_Label aShape = aShapes->AddShape(BRepPrimAPI_MakeBox(25.4, 10, 10).Shape());
  const occ::handle<XCAFDoc_DimTolTool> aTool = XCAFDoc_DocumentTool::DimTolTool(aDocument->Main());
  const TDF_Label                       aLabel = aTool->AddDimension();
  aTool->SetDimension(aShape, aLabel);
  const occ::handle<XCAFDimTolObjects_DimensionObject> anObject =
    new XCAFDimTolObjects_DimensionObject;
  anObject->SetType(XCAFDimTolObjects_DimensionType_Size_CurveLength);
  anObject->SetValue(25.4);
  XCAFDoc_Dimension::Set(aLabel)->SetObject(anObject);
  DESTEP_Parameters aParameters;
  aParameters.WriteSchema = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  aParameters.WriteUnit   = UnitsMethods_LengthUnit_Inch;
  STEPCAFControl_Writer aWriter;
  ASSERT_TRUE(aWriter.Transfer(aDocument, aParameters));
  const occ::handle<StepData_StepModel> aModel = aWriter.ChangeWriter().Model();
  int                                   aFound = 0;
  for (int anIndex = 1; anIndex <= aModel->NbEntities(); ++anIndex)
  {
    const occ::handle<StepRepr_ReprItemAndMeasureWithUnit> aMeasure =
      occ::down_cast<StepRepr_ReprItemAndMeasureWithUnit>(aModel->Entity(anIndex));
    if (aMeasure.IsNull())
    {
      continue;
    }
    ASSERT_FALSE(aMeasure->Name().IsNull());
    if (!aMeasure->Name()->String().IsEqual("nominal value"))
    {
      continue;
    }
    ++aFound;
    const occ::handle<StepBasic_MeasureWithUnit> aValue = aMeasure->GetMeasureWithUnit();
    ASSERT_FALSE(aValue.IsNull());
    STEPConstruct_UnitContext aContext;
    ASSERT_EQ(aContext.ComputeFactors(aValue->UnitComponent().NamedUnit(), StepData_Factors()), 0);
    EXPECT_NEAR(aValue->ValueComponent() * aContext.LengthFactor(), 25.4, Precision::Confusion());
  }
  EXPECT_EQ(aFound, 1);
}

//==================================================================================================

TEST(STEPCAFControl_WriterTest, RunoutOnlyCreatedForRunoutModifier)
{
  STEPCAFControl_Controller::Init();
  for (const bool isRunout : {false, true})
  {
    SCOPED_TRACE(isRunout);
    const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
    XCAFDoc_DocumentTool::SetLengthUnit(aDocument, 0.001);
    const occ::handle<XCAFDoc_ShapeTool> aShapes =
      XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
    const TDF_Label aShape = aShapes->AddShape(BRepPrimAPI_MakeBox(10, 10, 10).Shape());
    const occ::handle<XCAFDoc_DimTolTool> aTool =
      XCAFDoc_DocumentTool::DimTolTool(aDocument->Main());
    const TDF_Label aLabel = aTool->AddGeomTolerance();
    aTool->SetGeomTolerance(aShape, aLabel);
    const occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObject =
      new XCAFDimTolObjects_GeomToleranceObject;
    anObject->SetType(XCAFDimTolObjects_GeomToleranceType_Position);
    anObject->SetValue(0.1);
    anObject->SetTypeOfValue(XCAFDimTolObjects_GeomToleranceTypeValue_Diameter);
    if (isRunout)
    {
      anObject->SetZoneModifier(XCAFDimTolObjects_GeomToleranceZoneModif_Runout);
      anObject->SetValueOfZoneModifier(30.0);
    }
    XCAFDoc_GeomTolerance::Set(aLabel)->SetObject(anObject);
    DESTEP_Parameters aParameters;
    aParameters.WriteSchema = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
    STEPCAFControl_Writer aWriter;
    ASSERT_TRUE(aWriter.Transfer(aDocument, aParameters));
    const occ::handle<StepData_StepModel> aModel   = aWriter.ChangeWriter().Model();
    int                                   aRunouts = 0;
    for (int anIndex = 1; anIndex <= aModel->NbEntities(); ++anIndex)
    {
      const occ::handle<StepDimTol_RunoutZoneDefinition> aDefinition =
        occ::down_cast<StepDimTol_RunoutZoneDefinition>(aModel->Entity(anIndex));
      if (aDefinition.IsNull())
      {
        continue;
      }
      ++aRunouts;
      if (isRunout)
      {
        ASSERT_FALSE(aDefinition->Orientation().IsNull());
        const occ::handle<StepBasic_PlaneAngleMeasureWithUnit> anAngle =
          aDefinition->Orientation()->Angle();
        ASSERT_FALSE(anAngle.IsNull());
        STEPConstruct_UnitContext aContext;
        ASSERT_EQ(aContext.ComputeFactors(anAngle->UnitComponent().NamedUnit(), StepData_Factors()),
                  0);
        const double aDegrees =
          anAngle->ValueComponent() * aContext.PlaneAngleFactor() * 180.0 / M_PI;
        EXPECT_NEAR(aDegrees, 30.0, Precision::Angular() * 180.0 / M_PI);
      }
    }
    EXPECT_EQ(aRunouts, isRunout ? 1 : 0);
  }
}

// Test that the files of the parts (external references) get the write parameters of the main file
TEST(STEPCAFControl_WriterTest, ExternFilesGetTheParametersOfTheMainFile)
{
  STEPCAFControl_Controller::Init();
  const occ::handle<TDocStd_Document>  aDocument = new TDocStd_Document("BinXCAF");
  const occ::handle<XCAFDoc_ShapeTool> aShapes = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  TopoDS_Compound                      anAssembly;
  BRep_Builder                         aBuilder;
  aBuilder.MakeCompound(anAssembly);
  aBuilder.Add(anAssembly, BRepPrimAPI_MakeBox(10, 10, 10).Shape());
  aBuilder.Add(anAssembly, BRepPrimAPI_MakeBox(gp_Pnt(20, 0, 0), 5, 5, 5).Shape());
  aShapes->AddShape(anAssembly, true);

  DESTEP_Parameters aParameters;
  aParameters.WriteSchema         = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
  aParameters.WriteStringEncoding = DESTEP_Parameters::WriteMode_StringEncoding_Escape;
  STEPCAFControl_Writer aWriter;
  ASSERT_TRUE(aWriter.Transfer(aDocument, aParameters, STEPControl_AsIs, "part_"));
  ASSERT_EQ(aWriter.ExternFiles().Extent(), 2);
  for (NCollection_DataMap<TCollection_AsciiString,
                           occ::handle<STEPCAFControl_ExternFile>>::Iterator
         aFile(aWriter.ExternFiles());
       aFile.More();
       aFile.Next())
  {
    const occ::handle<StepData_StepModel> aModel =
      occ::down_cast<StepData_StepModel>(aFile.Value()->GetWS()->Model());
    ASSERT_FALSE(aModel.IsNull());
    EXPECT_EQ(aModel->InternalParameters.WriteStringEncoding,
              DESTEP_Parameters::WriteMode_StringEncoding_Escape);
    EXPECT_EQ(aModel->InternalParameters.WriteSchema,
              DESTEP_Parameters::WriteMode_StepSchema_AP242DIS);
  }
}
