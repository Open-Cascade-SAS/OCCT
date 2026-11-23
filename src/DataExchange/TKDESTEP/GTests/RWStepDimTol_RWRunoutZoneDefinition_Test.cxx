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

#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <NCollection_HArray1.hxx>
#include <STEPControl_Controller.hxx>
#include <StepBasic_PlaneAngleMeasureWithUnit.hxx>
#include <StepAP214_Protocol.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <StepData_WriterLib.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepDimTol_RunoutZoneDefinition.hxx>
#include <StepDimTol_RunoutZoneOrientation.hxx>
#include <StepDimTol_ToleranceZone.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

//==================================================================================================

TEST(StepDimTol_ReferencesTest, RunoutZone_WritesAndSharesOrientation)
{
  STEPControl_Controller::Init();
  const occ::handle<StepAP214_Protocol>               aProtocol = new StepAP214_Protocol;
  const occ::handle<StepDimTol_ToleranceZone>         aZone     = new StepDimTol_ToleranceZone;
  const occ::handle<StepDimTol_RunoutZoneOrientation> anOrientation =
    new StepDimTol_RunoutZoneOrientation;
  const occ::handle<StepBasic_PlaneAngleMeasureWithUnit> anAngle =
    new StepBasic_PlaneAngleMeasureWithUnit;
  anOrientation->Init(anAngle);
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_ShapeAspect>>> aBoundaries =
    new NCollection_HArray1<occ::handle<StepRepr_ShapeAspect>>(1, 1);
  aBoundaries->SetValue(1, new StepRepr_ShapeAspect);
  const occ::handle<StepDimTol_RunoutZoneDefinition> aDefinition =
    new StepDimTol_RunoutZoneDefinition;
  aDefinition->Init(aZone, aBoundaries, anOrientation);
  const occ::handle<StepData_StepModel> aModel = new StepData_StepModel;
  aModel->AddEntity(aZone);
  aModel->AddEntity(aBoundaries->Value(1));
  aModel->AddEntity(anOrientation);
  StepData_StepWriter aWriter(aModel);
  aWriter.StartEntity("RUNOUT_ZONE_DEFINITION");
  StepData_WriterLib                    aWriterLibrary(aProtocol);
  occ::handle<StepData_ReadWriteModule> aWriterModule;
  int                                   aWriterCase = 0;
  ASSERT_TRUE(aWriterLibrary.Select(aDefinition, aWriterModule, aWriterCase));
  aWriterModule->WriteStep(aWriterCase, aWriter, aDefinition);
  aWriter.EndEntity();
  EXPECT_STREQ(aWriter.Line(1)->ToCString(), "RUNOUT_ZONE_DEFINITION(#1,(#2),#3);");
  Interface_EntityIterator             aShared;
  Interface_GeneralLib                 aGeneralLibrary(aProtocol);
  occ::handle<Interface_GeneralModule> aGeneralModule;
  int                                  aGeneralCase = 0;
  ASSERT_TRUE(aGeneralLibrary.Select(aDefinition, aGeneralModule, aGeneralCase));
  aGeneralModule->FillSharedCase(aGeneralCase, aDefinition, aShared);
  bool hasOrientation = false;
  for (; aShared.More(); aShared.Next())
  {
    hasOrientation = hasOrientation || aShared.Value() == anOrientation;
  }
  EXPECT_TRUE(hasOrientation);

  const occ::handle<StepData_StepModel> aReferencedModel = new StepData_StepModel;
  aReferencedModel->AddWithRefs(aDefinition, aProtocol);
  EXPECT_NE(aReferencedModel->Number(anOrientation), 0);
  EXPECT_NE(aReferencedModel->Number(anAngle), 0);
}
