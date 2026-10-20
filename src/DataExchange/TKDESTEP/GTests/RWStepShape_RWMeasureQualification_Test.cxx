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
#include <StepAP214_Protocol.hxx>
#include <StepData_StepModel.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepShape_MeasureQualification.hxx>
#include <StepShape_TypeQualifier.hxx>
#include <StepShape_ValueQualifier.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

//==================================================================================================

TEST(StepDimTol_ReferencesTest, MeasureQualification_SharesQualifiedMeasure)
{
  STEPControl_Controller::Init();
  const occ::handle<StepBasic_MeasureWithUnit> aMeasure = new StepBasic_MeasureWithUnit;
  const occ::handle<StepShape_TypeQualifier>   aType    = new StepShape_TypeQualifier;
  aType->Init(new TCollection_HAsciiString("minimum"));
  StepShape_ValueQualifier aValue;
  aValue.SetValue(aType);
  const occ::handle<NCollection_HArray1<StepShape_ValueQualifier>> aQualifiers =
    new NCollection_HArray1<StepShape_ValueQualifier>(1, 1);
  aQualifiers->SetValue(1, aValue);
  const occ::handle<StepShape_MeasureQualification> aQualification =
    new StepShape_MeasureQualification;
  aQualification->Init(new TCollection_HAsciiString(),
                       new TCollection_HAsciiString(),
                       aMeasure,
                       aQualifiers);
  Interface_EntityIterator              aShared;
  const occ::handle<StepAP214_Protocol> aProtocol = new StepAP214_Protocol;
  Interface_GeneralLib                  aGeneralLibrary(aProtocol);
  occ::handle<Interface_GeneralModule>  aGeneralModule;
  int                                   aGeneralCase = 0;
  ASSERT_TRUE(aGeneralLibrary.Select(aQualification, aGeneralModule, aGeneralCase));
  aGeneralModule->FillSharedCase(aGeneralCase, aQualification, aShared);
  bool hasMeasure = false;
  for (; aShared.More(); aShared.Next())
  {
    hasMeasure = hasMeasure || aShared.Value() == aMeasure;
  }
  EXPECT_TRUE(hasMeasure);
  // Model traversal must also discover the dependency through the protocol.
  const occ::handle<StepData_StepModel> aModel = new StepData_StepModel;
  aModel->SetProtocol(aProtocol);
  aModel->AddWithRefs(aQualification);
  EXPECT_GT(aModel->Number(aMeasure), 0);
}
