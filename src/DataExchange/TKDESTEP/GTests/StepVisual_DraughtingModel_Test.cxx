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
#include <STEPControl_Reader.hxx>
#include <STEPControl_ActorRead.hxx>
#include <StepData_Factors.hxx>
#include <Transfer_TransientProcess.hxx>
#include <StepAP214.hxx>
#include <StepAP214_Protocol.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_DraughtingModel.hxx>
#include <StepVisual_TessellatedCurveSet.hxx>
#include <StepVisual_CharacterizedDraughtingModelAndTessellatedShapeRepresentation.hxx>
#include <StepVisual_DraughtingModelAndTessellatedShapeRepresentation.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

#include <sstream>

//==================================================================================================

TEST(StepVisual_DraughtingModelTest, TessellatedComplexPreservesTypeItemsAndContext)
{
  for (const bool isCharacterized : {false, true})
  {
    SCOPED_TRACE(isCharacterized);
    TCollection_AsciiString aText(
      "ISO-10303-21;HEADER;FILE_DESCRIPTION((''),'2;1');"
      "FILE_NAME('','',(''),(''),'','','');"
      "FILE_SCHEMA(('AP242_MANAGED_MODEL_BASED_3D_ENGINEERING_MIM_LF'));ENDSEC;DATA;#1=(");
    if (isCharacterized)
    {
      aText += "CHARACTERIZED_OBJECT(*,*)CHARACTERIZED_REPRESENTATION()";
    }
    aText += "DRAUGHTING_MODEL()REPRESENTATION('PMI',(#2),#3)"
             "SHAPE_REPRESENTATION()TESSELLATED_SHAPE_REPRESENTATION());"
             "#2=TESSELLATED_CURVE_SET('',#10,((1,2)));"
             "#3=(GEOMETRIC_REPRESENTATION_CONTEXT(3)GLOBAL_UNIT_ASSIGNED_CONTEXT((#4,#8,#9))"
             "REPRESENTATION_CONTEXT('',''));"
             "#4=(CONVERSION_BASED_UNIT('INCH',#5)LENGTH_UNIT()NAMED_UNIT(#6));"
             "#5=LENGTH_MEASURE_WITH_UNIT(LENGTH_MEASURE(25.4),#7);"
             "#6=DIMENSIONAL_EXPONENTS(1.,0.,0.,0.,0.,0.,0.);"
             "#7=(LENGTH_UNIT()NAMED_UNIT(*)SI_UNIT(.MILLI.,.METRE.));"
             "#8=(NAMED_UNIT(*)PLANE_ANGLE_UNIT()SI_UNIT($,.RADIAN.));"
             "#9=(NAMED_UNIT(*)SI_UNIT($,.STERADIAN.)SOLID_ANGLE_UNIT());"
             "#10=COORDINATES_LIST('',2,((1.,2.,3.),(2.,3.,4.)));"
             "ENDSEC;END-ISO-10303-21;";
    std::stringstream aStream(aText.ToCString());
    for (int aRead = 0; aRead < 2; ++aRead)
    {
      STEPControl_Reader aReader;
      ASSERT_EQ(aReader.ReadStream("draughting.step", aStream), IFSelect_RetDone);
      const occ::handle<StepData_StepModel> aModel = aReader.StepModel();
      ASSERT_FALSE(aModel.IsNull());
      ASSERT_GE(aModel->NbEntities(), 3);
      const occ::handle<StepVisual_DraughtingModel> anEntity =
        occ::down_cast<StepVisual_DraughtingModel>(aModel->Entity(1));
      ASSERT_FALSE(anEntity.IsNull());
      EXPECT_EQ(
        anEntity->DynamicType(),
        isCharacterized
          ? STANDARD_TYPE(StepVisual_CharacterizedDraughtingModelAndTessellatedShapeRepresentation)
          : STANDARD_TYPE(StepVisual_DraughtingModelAndTessellatedShapeRepresentation));
      ASSERT_FALSE(anEntity->Name().IsNull());
      EXPECT_STREQ(anEntity->Name()->ToCString(), "PMI");
      ASSERT_EQ(anEntity->NbItems(), 1);
      const occ::handle<StepVisual_TessellatedCurveSet> aCurve =
        occ::down_cast<StepVisual_TessellatedCurveSet>(anEntity->ItemsValue(1));
      ASSERT_FALSE(aCurve.IsNull());
      ASSERT_FALSE(aCurve->CoordList().IsNull());
      ASSERT_FALSE(aCurve->CoordList()->Points().IsNull());
      ASSERT_EQ(aCurve->CoordList()->Points()->Size(), 2);
      EXPECT_DOUBLE_EQ(aCurve->CoordList()->Points()->At(0).X(), 1.0);
      ASSERT_EQ(anEntity->ContextOfItems(), aModel->Entity(3));
      const occ::handle<Transfer_TransientProcess> aProcess =
        new Transfer_TransientProcess(aModel->NbEntities());
      aProcess->SetModel(aModel);
      STEPControl_ActorRead anActor(aModel);
      StepData_Factors      aFactors;
      anActor.PrepareUnits(anEntity, aProcess, aFactors);
      EXPECT_DOUBLE_EQ(aFactors.LengthFactor(), 25.4);

      Interface_GeneralLib                 aLibrary(StepAP214::Protocol());
      occ::handle<Interface_GeneralModule> aModule;
      int                                  aCase = 0;
      ASSERT_TRUE(aLibrary.Select(anEntity, aModule, aCase));
      occ::handle<Standard_Transient> anEmpty;
      ASSERT_TRUE(aModule->NewVoid(aCase, anEmpty));
      ASSERT_FALSE(anEmpty.IsNull());
      EXPECT_EQ(anEmpty->DynamicType(), anEntity->DynamicType());
      Interface_EntityIterator aShared;
      aModule->FillSharedCase(aCase, anEntity, aShared);
      EXPECT_EQ(aShared.NbEntities(), 2);

      StepData_StepWriter aWriter(aModel);
      aWriter.SendModel(StepAP214::Protocol());
      aStream.str("");
      aStream.clear();
      ASSERT_TRUE(aWriter.Print(aStream));
    }
  }
}
