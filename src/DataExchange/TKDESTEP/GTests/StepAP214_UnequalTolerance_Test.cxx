// Copyright (c) 2026 OPEN CASCADE SAS
// Distributed under the OCCT LGPL exception; see LICENSE_LGPL_21.txt.

#include <Interface_Check.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_ReaderLib.hxx>
#include <Interface_ReaderModule.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <StepAP214.hxx>
#include <STEPControl_Reader.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_UnequallyDisposedGeometricTolerance.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <sstream>
#include <StepAP214_Protocol.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.hxx>
#include <StepDimTol_GeoTolAndUneqDisGeoTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthModAndUneqDisGeoTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTolAndUneqDisGeoTol.hxx>
#include <gtest/gtest.h>

namespace
{
occ::handle<StepData_ReadWriteModule> readWriteModule()
{
  STEPCAFControl_Controller::Init();
  Interface_ReaderLib aLibrary(StepAP214::Protocol());
  for (aLibrary.Start(); aLibrary.More(); aLibrary.Next())
  {
    if (aLibrary.Protocol()->IsKind(STANDARD_TYPE(StepAP214_Protocol)))
    {
      return occ::down_cast<StepData_ReadWriteModule>(aLibrary.Module());
    }
  }
  return nullptr;
}

NCollection_Sequence<TCollection_AsciiString> constituents(const char* theType,
                                                           bool        theDatum,
                                                           bool        theModifiers,
                                                           bool        theMaximum)
{
  NCollection_Sequence<TCollection_AsciiString> aTypes;
  // Deliberately unordered: recognition must normalize STEP complex names.
  aTypes.Append("UNEQUALLY_DISPOSED_GEOMETRIC_TOLERANCE");
  aTypes.Append(theType);
  aTypes.Append("GEOMETRIC_TOLERANCE");
  if (theDatum)
  {
    aTypes.Append("GEOMETRIC_TOLERANCE_WITH_DATUM_REFERENCE");
  }
  if (theModifiers)
  {
    aTypes.Append("GEOMETRIC_TOLERANCE_WITH_MODIFIERS");
  }
  if (theMaximum)
  {
    aTypes.Append("GEOMETRIC_TOLERANCE_WITH_MAXIMUM_TOLERANCE");
  }
  return aTypes;
}
} // namespace

TEST(StepAP214_UnequalToleranceTest, DispatchAndCreationAgree)
{
  const auto aModule = readWriteModule();
  ASSERT_FALSE(aModule.IsNull());
  const auto                       aProtocol   = StepAP214::Protocol();
  const occ::handle<Standard_Type> aExpected[] = {
    STANDARD_TYPE(StepDimTol_GeoTolAndUneqDisGeoTol),
    STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol),
    STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthModAndUneqDisGeoTol),
    STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol),
    STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol),
    STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTolAndUneqDisGeoTol)};
  Interface_GeneralLib aGeneral(aProtocol);
  for (int i = 0; i < 6; ++i)
  {
    SCOPED_TRACE(i);
    const auto aTypes = constituents("SURFACE_PROFILE_TOLERANCE", i % 2 != 0, i >= 2, i >= 4);
    const int  aCase  = aModule->CaseStep(aTypes);
    ASSERT_GT(aCase, 0);
    EXPECT_EQ(aCase, aProtocol->TypeNumber(aExpected[i]));
    EXPECT_TRUE(aModule->IsComplex(aCase));
    bool hasCreated = false;
    for (aGeneral.Start(); aGeneral.More(); aGeneral.Next())
    {
      if (aGeneral.Protocol() != aProtocol)
      {
        continue;
      }
      occ::handle<Standard_Transient> anEntity;
      ASSERT_TRUE(aGeneral.Module()->NewVoid(aCase, anEntity));
      ASSERT_FALSE(anEntity.IsNull());
      EXPECT_EQ(anEntity->DynamicType(), aExpected[i]);
      hasCreated = true;
    }
    EXPECT_TRUE(hasCreated);
  }
}

TEST(StepAP214_UnequalToleranceTest, RejectsIncompatibleConstituents)
{
  const auto aModule = readWriteModule();
  ASSERT_FALSE(aModule.IsNull());
  EXPECT_EQ(aModule->CaseStep(constituents("SURFACE_PROFILE_TOLERANCE", false, false, true)), 0);
  for (const char* aType : {"FLATNESS_TOLERANCE",
                            "CYLINDRICITY_TOLERANCE",
                            "ROUNDNESS_TOLERANCE",
                            "STRAIGHTNESS_TOLERANCE"})
  {
    for (int aVariant = 0; aVariant < 3; ++aVariant)
    {
      const bool hasModifiers = aVariant >= 1;
      const bool hasMaximum   = aVariant >= 2;
      EXPECT_EQ(aModule->CaseStep(constituents(aType, true, hasModifiers, hasMaximum)), 0)
        << aType << ", variant=" << aVariant;
      EXPECT_GT(aModule->CaseStep(constituents(aType, false, hasModifiers, hasMaximum)), 0)
        << aType << ", variant=" << aVariant;
    }
  }
  for (const char* aType : {"ANGULARITY_TOLERANCE",
                            "CIRCULAR_RUNOUT_TOLERANCE",
                            "COAXIALITY_TOLERANCE",
                            "CONCENTRICITY_TOLERANCE",
                            "PARALLELISM_TOLERANCE",
                            "PERPENDICULARITY_TOLERANCE",
                            "SYMMETRY_TOLERANCE",
                            "TOTAL_RUNOUT_TOLERANCE"})
  {
    for (int aVariant = 0; aVariant < 3; ++aVariant)
    {
      const bool hasModifiers = aVariant >= 1;
      const bool hasMaximum   = aVariant >= 2;
      EXPECT_EQ(aModule->CaseStep(constituents(aType, false, hasModifiers, hasMaximum)), 0)
        << aType << ", variant=" << aVariant;
      EXPECT_GT(aModule->CaseStep(constituents(aType, true, hasModifiers, hasMaximum)), 0)
        << aType << ", variant=" << aVariant;
    }
  }
  auto aTypes = constituents("SURFACE_PROFILE_TOLERANCE", false, false, false);
  aTypes.Append("GEOMETRIC_TOLERANCE");
  EXPECT_EQ(aModule->CaseStep(aTypes), 0);
  aTypes = constituents("SURFACE_PROFILE_TOLERANCE", false, false, false);
  aTypes.Append("POSITION_TOLERANCE");
  EXPECT_EQ(aModule->CaseStep(aTypes), 0);
  aTypes = constituents("SURFACE_PROFILE_TOLERANCE", false, false, false);
  aTypes.Append("UNSUPPORTED_CONSTITUENT");
  EXPECT_EQ(aModule->CaseStep(aTypes), 0);
}

// Qualified measure structure from NIST CTC 02, reduced to its semantic dependencies.
TEST(StepAP214_UnequalToleranceTest, PreservesQualifiedLengthMeasureReference)
{
  readWriteModule();
  for (const bool hasMaximum : {false, true})
  {
    SCOPED_TRACE(hasMaximum);
    std::stringstream aStream(R"STEP(ISO-10303-21;
HEADER;
FILE_DESCRIPTION((''),'2;1');
FILE_NAME('','',(''),(''),'','','');
FILE_SCHEMA(('AP242_MANAGED_MODEL_BASED_3D_ENGINEERING_MIM_LF'));
ENDSEC;
DATA;
#1=SHAPE_ASPECT('','',#2,.T.);
#2=PRODUCT_DEFINITION_SHAPE('','',#3);
#3=PRODUCT_DEFINITION('','',#4,#5);
#4=PRODUCT_DEFINITION_FORMATION('','',#6);
#5=PRODUCT_DEFINITION_CONTEXT('',#7,'design');
#6=PRODUCT('','','',(#8));
#7=APPLICATION_CONTEXT('');
#8=PRODUCT_CONTEXT('',#7,'');
#9=(LENGTH_UNIT()NAMED_UNIT(*)SI_UNIT(.MILLI.,.METRE.));
#10=LENGTH_MEASURE_WITH_UNIT(LENGTH_MEASURE(2.5),#9);
#11=TYPE_QUALIFIER('basic');
#12=(LENGTH_MEASURE_WITH_UNIT()MEASURE_REPRESENTATION_ITEM()
MEASURE_WITH_UNIT(LENGTH_MEASURE(0.5),#9)
QUALIFIED_REPRESENTATION_ITEM((#11))REPRESENTATION_ITEM(''));
#13=(GEOMETRIC_TOLERANCE('nist','',#10,#1)SURFACE_PROFILE_TOLERANCE()
UNEQUALLY_DISPOSED_GEOMETRIC_TOLERANCE(#12));
ENDSEC;
END-ISO-10303-21;
)STEP");
    if (hasMaximum)
    {
      std::string       aText = aStream.str();
      const std::string aMaximum =
        "#14=(LENGTH_MEASURE_WITH_UNIT()MEASURE_REPRESENTATION_ITEM()"
        "MEASURE_WITH_UNIT(LENGTH_MEASURE(3.0),#9)"
        "QUALIFIED_REPRESENTATION_ITEM((#11))REPRESENTATION_ITEM(''));\n";
      aText.insert(aText.find("#13="), aMaximum);
      aText.insert(aText.find("SURFACE_PROFILE_TOLERANCE()"),
                   "GEOMETRIC_TOLERANCE_WITH_MAXIMUM_TOLERANCE(#14)"
                   "GEOMETRIC_TOLERANCE_WITH_MODIFIERS((.MAXIMUM_MATERIAL_REQUIREMENT.))");
      aStream.str(aText);
    }
    for (int aPass = 0; aPass < 2; ++aPass)
    {
      SCOPED_TRACE(aPass);
      STEPControl_Reader aReader;
      ASSERT_EQ(aReader.ReadStream("qualified.step", aStream), IFSelect_RetDone);
      const auto aModel = aReader.StepModel();
      for (int i = 1; i <= aModel->NbEntities(); ++i)
      {
        EXPECT_EQ(aModel->Check(i, true)->NbFails(), 0) << i;
      }
      aModel->InternalParameters.WriteSchema = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
      occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance> anUnequal;
      occ::handle<Standard_Transient>                             aMaximum;
      for (int i = 1; i <= aModel->NbEntities(); ++i)
      {
        if (const auto anEntity =
              occ::down_cast<StepDimTol_GeoTolAndUneqDisGeoTol>(aModel->Value(i)))
        {
          anUnequal = anEntity->GetUnequallyDisposedGeometricTolerance();
        }
        else if (const auto anEntity =
                   occ::down_cast<StepDimTol_GeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol>(
                     aModel->Value(i)))
        {
          anUnequal = anEntity->GetUnequallyDisposedGeometricTolerance();
          aMaximum  = anEntity->GetMaxTolerance();
        }
      }
      ASSERT_FALSE(anUnequal.IsNull());
      if (hasMaximum)
      {
        const auto aQualified =
          occ::down_cast<StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI>(aMaximum);
        ASSERT_FALSE(aQualified.IsNull());
        ASSERT_FALSE(aQualified->GetMeasureWithUnit().IsNull());
        EXPECT_DOUBLE_EQ(aQualified->GetMeasureWithUnit()->ValueComponent(), 3.0);
      }
      const auto aMeasure =
        occ::down_cast<StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI>(anUnequal->Displacement());
      ASSERT_FALSE(aMeasure.IsNull());
      ASSERT_FALSE(aMeasure->GetMeasureWithUnit().IsNull());
      EXPECT_DOUBLE_EQ(aMeasure->GetMeasureWithUnit()->ValueComponent(), 0.5);
      ASSERT_FALSE(aMeasure->GetQualifiedRepresentationItem().IsNull());
      StepData_StepWriter aWriter(aModel);
      aWriter.SendModel(StepAP214::Protocol());
      aStream.str("");
      aStream.clear();
      ASSERT_TRUE(aWriter.Print(aStream));
    }
  }
}
