// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <Standard_Transient.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>

#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_SiPrefix.hxx>
#include <StepBasic_SiUnitName.hxx>

#include <StepDimTol_GeometricTolerance.hxx>
#include <StepRepr_MakeFromUsageOption.hxx>
#include <StepRepr_ParallelOffset.hxx>
#include <StepRepr_QuantifiedAssemblyComponentUsage.hxx>
#include <StepShape_MeasureQualification.hxx>
#include <StepData_Logical.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepShape_HArray1OfValueQualifier.hxx>

#include <TCollection_HAsciiString.hxx>

// Helper functions to create test objects
namespace
{

// Create a MeasureWithUnit
Handle(StepBasic_MeasureWithUnit) CreateMeasureWithUnit(const Standard_Real theValue)
{
  Handle(StepBasic_MeasureWithUnit) aMeasure = new StepBasic_MeasureWithUnit();

  // Set value component
  Handle(StepBasic_MeasureValueMember) aValueMember = new StepBasic_MeasureValueMember();
  aValueMember->SetName("POSITIVE_LENGTH_MEASURE");
  aValueMember->SetReal(theValue);
  aMeasure->SetValueComponentMember(aValueMember);

  // Create a dummy SiUnit for unit component
  Handle(StepBasic_SiUnit) aSiUnit = new StepBasic_SiUnit();
  aSiUnit->Init(Standard_False,
                StepBasic_SiPrefix::StepBasic_spMilli,
                StepBasic_SiUnitName::StepBasic_sunMetre);
  StepBasic_Unit aUnit;
  aUnit.SetValue(aSiUnit);
  aMeasure->SetUnitComponent(aUnit);

  return aMeasure;
}

// Create a ReprItemAndMeasureWithUnit
Handle(StepRepr_ReprItemAndMeasureWithUnit) CreateReprItemAndMeasureWithUnit(
  const Standard_Real theValue)
{
  Handle(StepRepr_ReprItemAndMeasureWithUnit) aReprMeasure =
    new StepRepr_ReprItemAndMeasureWithUnit();
  Handle(StepBasic_MeasureWithUnit) aMeasure = CreateMeasureWithUnit(theValue);
  aReprMeasure->SetMeasureWithUnit(aMeasure);

  // Set other required fields
  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("TestReprItem");
  aReprMeasure->SetName(aName);

  return aReprMeasure;
}
} // namespace

// Test fixture for all transient replacement tests
class StepTransientReplacements : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create common test objects
    myMeasureWithUnit            = CreateMeasureWithUnit(5.0);
    myReprItemAndMeasureWithUnit = CreateReprItemAndMeasureWithUnit(10.0);

    // Create dimensional exponents
    myDimensionalExponents = new StepBasic_DimensionalExponents();
    myDimensionalExponents->Init(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    // Create name string
    myName = new TCollection_HAsciiString("TestName");

    // Create description string
    myDescription = new TCollection_HAsciiString("TestDescription");

    // Create product definition
    myProductDefinition = new StepBasic_ProductDefinition();

    // Create product definition shape
    myProductDefinitionShape = new StepRepr_ProductDefinitionShape();

    // Create qualifiers array
    myQualifiers = new StepShape_HArray1OfValueQualifier(1, 1);
  }

  // Common test objects
  Handle(StepBasic_MeasureWithUnit)           myMeasureWithUnit;
  Handle(StepRepr_ReprItemAndMeasureWithUnit) myReprItemAndMeasureWithUnit;
  Handle(StepBasic_DimensionalExponents)      myDimensionalExponents;
  Handle(TCollection_HAsciiString)            myName;
  Handle(TCollection_HAsciiString)            myDescription;
  Handle(StepBasic_ProductDefinition)         myProductDefinition;
  Handle(StepRepr_ProductDefinitionShape)     myProductDefinitionShape;
  Handle(StepShape_HArray1OfValueQualifier)   myQualifiers;
};

// Test ConversionBasedUnit with different transient types
TEST_F(StepTransientReplacements, ConversionBasedUnit_WorksWithBothTypes)
{
  // Create ConversionBasedUnit
  Handle(StepBasic_ConversionBasedUnit) aUnit = new StepBasic_ConversionBasedUnit();

  // Test with MeasureWithUnit
  aUnit->Init(myDimensionalExponents, myName, myMeasureWithUnit);
  EXPECT_FALSE(aUnit->ConversionFactor().IsNull());
  EXPECT_TRUE(aUnit->ConversionFactor()->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)));

  Handle(StepBasic_MeasureWithUnit) aMeasure =
    Handle(StepBasic_MeasureWithUnit)::DownCast(aUnit->ConversionFactor());
  EXPECT_FALSE(aMeasure.IsNull());
  EXPECT_NEAR(aMeasure->ValueComponent(), 5.0, 1e-7);

  // Test with ReprItemAndMeasureWithUnit
  aUnit->SetConversionFactor(myReprItemAndMeasureWithUnit);
  EXPECT_FALSE(aUnit->ConversionFactor().IsNull());
  EXPECT_TRUE(
    aUnit->ConversionFactor()->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)));

  Handle(StepRepr_ReprItemAndMeasureWithUnit) aReprMeasure =
    Handle(StepRepr_ReprItemAndMeasureWithUnit)::DownCast(aUnit->ConversionFactor());
  EXPECT_FALSE(aReprMeasure.IsNull());
  Handle(StepBasic_MeasureWithUnit) aExtractedMeasure = aReprMeasure->GetMeasureWithUnit();
  EXPECT_NEAR(aExtractedMeasure->ValueComponent(), 10.0, 1e-7);
}

// Test GeometricTolerance with different transient types
TEST_F(StepTransientReplacements, GeometricTolerance_WorksWithBothTypes)
{
  // Create GeometricTolerance
  Handle(StepDimTol_GeometricTolerance) aTolerance = new StepDimTol_GeometricTolerance();

  // Create a dummy tolerance target
  StepDimTol_GeometricToleranceTarget aTarget;

  // Test with MeasureWithUnit
  aTolerance->Init(myName, myDescription, myMeasureWithUnit, aTarget);
  EXPECT_FALSE(aTolerance->Magnitude().IsNull());
  EXPECT_TRUE(aTolerance->Magnitude()->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)));

  Handle(StepBasic_MeasureWithUnit) aMeasure =
    Handle(StepBasic_MeasureWithUnit)::DownCast(aTolerance->Magnitude());
  EXPECT_FALSE(aMeasure.IsNull());
  EXPECT_NEAR(aMeasure->ValueComponent(), 5.0, 1e-7);

  // Test with ReprItemAndMeasureWithUnit
  aTolerance->SetMagnitude(myReprItemAndMeasureWithUnit);
  EXPECT_FALSE(aTolerance->Magnitude().IsNull());
  EXPECT_TRUE(aTolerance->Magnitude()->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)));

  Handle(StepRepr_ReprItemAndMeasureWithUnit) aReprMeasure =
    Handle(StepRepr_ReprItemAndMeasureWithUnit)::DownCast(aTolerance->Magnitude());
  EXPECT_FALSE(aReprMeasure.IsNull());
  Handle(StepBasic_MeasureWithUnit) aExtractedMeasure = aReprMeasure->GetMeasureWithUnit();
  EXPECT_NEAR(aExtractedMeasure->ValueComponent(), 10.0, 1e-7);
}

// Test MakeFromUsageOption with different transient types
TEST_F(StepTransientReplacements, MakeFromUsageOption_WorksWithBothTypes)
{
  // Create MakeFromUsageOption
  Handle(StepRepr_MakeFromUsageOption) aMakeFromUsage = new StepRepr_MakeFromUsageOption();

  // Test with MeasureWithUnit
  // Use proper function signature for Init
  Standard_Boolean hasDescription = Standard_True;
  aMakeFromUsage->Init(myName,
                       myName,
                       hasDescription,
                       myDescription,
                       myProductDefinition,
                       myProductDefinition,
                       1,
                       myDescription,
                       myMeasureWithUnit);

  EXPECT_FALSE(aMakeFromUsage->Quantity().IsNull());
  EXPECT_TRUE(aMakeFromUsage->Quantity()->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)));

  Handle(StepBasic_MeasureWithUnit) aMeasure =
    Handle(StepBasic_MeasureWithUnit)::DownCast(aMakeFromUsage->Quantity());
  EXPECT_FALSE(aMeasure.IsNull());
  EXPECT_NEAR(aMeasure->ValueComponent(), 5.0, 1e-7);

  // Test with ReprItemAndMeasureWithUnit
  aMakeFromUsage->SetQuantity(myReprItemAndMeasureWithUnit);
  EXPECT_FALSE(aMakeFromUsage->Quantity().IsNull());
  EXPECT_TRUE(
    aMakeFromUsage->Quantity()->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)));

  Handle(StepRepr_ReprItemAndMeasureWithUnit) aReprMeasure =
    Handle(StepRepr_ReprItemAndMeasureWithUnit)::DownCast(aMakeFromUsage->Quantity());
  EXPECT_FALSE(aReprMeasure.IsNull());
  Handle(StepBasic_MeasureWithUnit) aExtractedMeasure = aReprMeasure->GetMeasureWithUnit();
  EXPECT_NEAR(aExtractedMeasure->ValueComponent(), 10.0, 1e-7);
}

// Test ParallelOffset with different transient types
TEST_F(StepTransientReplacements, ParallelOffset_WorksWithBothTypes)
{
  // Create ParallelOffset
  Handle(StepRepr_ParallelOffset) aParallelOffset = new StepRepr_ParallelOffset();

  // Test with MeasureWithUnit
  aParallelOffset->Init(myName,
                        myDescription,
                        myProductDefinitionShape,
                        StepData_LTrue,
                        myMeasureWithUnit);

  EXPECT_FALSE(aParallelOffset->Offset().IsNull());
  EXPECT_TRUE(aParallelOffset->Offset()->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)));

  Handle(StepBasic_MeasureWithUnit) aMeasure =
    Handle(StepBasic_MeasureWithUnit)::DownCast(aParallelOffset->Offset());
  EXPECT_FALSE(aMeasure.IsNull());
  EXPECT_NEAR(aMeasure->ValueComponent(), 5.0, 1e-7);

  // Test with ReprItemAndMeasureWithUnit
  aParallelOffset->SetOffset(myReprItemAndMeasureWithUnit);
  EXPECT_FALSE(aParallelOffset->Offset().IsNull());
  EXPECT_TRUE(
    aParallelOffset->Offset()->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)));

  Handle(StepRepr_ReprItemAndMeasureWithUnit) aReprMeasure =
    Handle(StepRepr_ReprItemAndMeasureWithUnit)::DownCast(aParallelOffset->Offset());
  EXPECT_FALSE(aReprMeasure.IsNull());
  Handle(StepBasic_MeasureWithUnit) aExtractedMeasure = aReprMeasure->GetMeasureWithUnit();
  EXPECT_NEAR(aExtractedMeasure->ValueComponent(), 10.0, 1e-7);
}

// Test QuantifiedAssemblyComponentUsage with different transient types
TEST_F(StepTransientReplacements, QuantifiedAssemblyComponentUsage_WorksWithBothTypes)
{
  // Create QuantifiedAssemblyComponentUsage
  Handle(StepRepr_QuantifiedAssemblyComponentUsage) aUsage =
    new StepRepr_QuantifiedAssemblyComponentUsage();

  // Test with MeasureWithUnit
  // Use proper function signature for Init
  Standard_Boolean hasDescription         = Standard_True;
  Standard_Boolean hasReferenceDesignator = Standard_True;
  aUsage->Init(myName,
               myName,
               hasDescription,
               myDescription,
               myProductDefinition,
               myProductDefinition,
               hasReferenceDesignator,
               myName,
               myMeasureWithUnit);

  EXPECT_FALSE(aUsage->Quantity().IsNull());
  EXPECT_TRUE(aUsage->Quantity()->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)));

  Handle(StepBasic_MeasureWithUnit) aMeasure =
    Handle(StepBasic_MeasureWithUnit)::DownCast(aUsage->Quantity());
  EXPECT_FALSE(aMeasure.IsNull());
  EXPECT_NEAR(aMeasure->ValueComponent(), 5.0, 1e-7);

  // Test with ReprItemAndMeasureWithUnit
  aUsage->SetQuantity(myReprItemAndMeasureWithUnit);
  EXPECT_FALSE(aUsage->Quantity().IsNull());
  EXPECT_TRUE(aUsage->Quantity()->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)));

  Handle(StepRepr_ReprItemAndMeasureWithUnit) aReprMeasure =
    Handle(StepRepr_ReprItemAndMeasureWithUnit)::DownCast(aUsage->Quantity());
  EXPECT_FALSE(aReprMeasure.IsNull());
  Handle(StepBasic_MeasureWithUnit) aExtractedMeasure = aReprMeasure->GetMeasureWithUnit();
  EXPECT_NEAR(aExtractedMeasure->ValueComponent(), 10.0, 1e-7);
}

// Test MeasureQualification with different transient types
TEST_F(StepTransientReplacements, MeasureQualification_WorksWithBothTypes)
{
  // Create MeasureQualification
  Handle(StepShape_MeasureQualification) aQualification = new StepShape_MeasureQualification();

  // Test with MeasureWithUnit
  aQualification->Init(myName, myDescription, myMeasureWithUnit, myQualifiers);

  EXPECT_FALSE(aQualification->QualifiedMeasure().IsNull());
  EXPECT_TRUE(aQualification->QualifiedMeasure()->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)));

  Handle(StepBasic_MeasureWithUnit) aMeasure =
    Handle(StepBasic_MeasureWithUnit)::DownCast(aQualification->QualifiedMeasure());
  EXPECT_FALSE(aMeasure.IsNull());
  EXPECT_NEAR(aMeasure->ValueComponent(), 5.0, 1e-7);

  // Test with ReprItemAndMeasureWithUnit
  aQualification->SetQualifiedMeasure(myReprItemAndMeasureWithUnit);
  EXPECT_FALSE(aQualification->QualifiedMeasure().IsNull());
  EXPECT_TRUE(
    aQualification->QualifiedMeasure()->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)));

  Handle(StepRepr_ReprItemAndMeasureWithUnit) aReprMeasure =
    Handle(StepRepr_ReprItemAndMeasureWithUnit)::DownCast(aQualification->QualifiedMeasure());
  EXPECT_FALSE(aReprMeasure.IsNull());
  Handle(StepBasic_MeasureWithUnit) aExtractedMeasure = aReprMeasure->GetMeasureWithUnit();
  EXPECT_NEAR(aExtractedMeasure->ValueComponent(), 10.0, 1e-7);
}

// Test GetMeasureWithUnit helper function from STEPCAFControl_Reader namespace
TEST_F(StepTransientReplacements, GetMeasureWithUnit_ExtractsCorrectly)
{
  // This tests the helper function that was added in STEPCAFControl_Reader.cxx
  // We recreate the function here for testing

  auto GetMeasureWithUnit =
    [](const Handle(Standard_Transient)& theMeasure) -> Handle(StepBasic_MeasureWithUnit) {
    if (theMeasure.IsNull())
    {
      return nullptr;
    }

    Handle(StepBasic_MeasureWithUnit) aMeasureWithUnit;
    if (theMeasure->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)))
    {
      aMeasureWithUnit = Handle(StepBasic_MeasureWithUnit)::DownCast(theMeasure);
    }
    else if (theMeasure->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)))
    {
      Handle(StepRepr_ReprItemAndMeasureWithUnit) aReprMeasureItem =
        Handle(StepRepr_ReprItemAndMeasureWithUnit)::DownCast(theMeasure);
      aMeasureWithUnit = aReprMeasureItem->GetMeasureWithUnit();
    }
    return aMeasureWithUnit;
  };

  // Test with null
  Handle(Standard_Transient)        aNullTransient;
  Handle(StepBasic_MeasureWithUnit) aExtracted = GetMeasureWithUnit(aNullTransient);
  EXPECT_TRUE(aExtracted.IsNull());

  // Test with MeasureWithUnit
  aExtracted = GetMeasureWithUnit(myMeasureWithUnit);
  EXPECT_FALSE(aExtracted.IsNull());
  EXPECT_NEAR(aExtracted->ValueComponent(), 5.0, 1e-7);

  // Test with ReprItemAndMeasureWithUnit
  aExtracted = GetMeasureWithUnit(myReprItemAndMeasureWithUnit);
  EXPECT_FALSE(aExtracted.IsNull());
  EXPECT_NEAR(aExtracted->ValueComponent(), 10.0, 1e-7);

  // Test with unrelated type
  Handle(Standard_Transient) anUnrelatedTransient = myName;
  aExtracted                                      = GetMeasureWithUnit(anUnrelatedTransient);
  EXPECT_TRUE(aExtracted.IsNull());
}
