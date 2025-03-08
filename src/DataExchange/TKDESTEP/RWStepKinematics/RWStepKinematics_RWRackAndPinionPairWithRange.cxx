// Created on : Sat May 02 12:41:15 2020
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include "RWStepKinematics_RWRackAndPinionPairWithRange.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_RackAndPinionPairWithRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <Standard_Real.hxx>

//=================================================================================================

RWStepKinematics_RWRackAndPinionPairWithRange::RWStepKinematics_RWRackAndPinionPairWithRange() {}

//=================================================================================================

void RWStepKinematics_RWRackAndPinionPairWithRange::ReadStep(
  const Handle(StepData_StepReaderData)&                   theData,
  const Standard_Integer                                   theNum,
  Handle(Interface_Check)&                                 theArch,
  const Handle(StepKinematics_RackAndPinionPairWithRange)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 9, theArch, "rack_and_pinion_pair_with_range"))
    return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);

  // Inherited fields of ItemDefinedTransformation

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Name;
  theData->ReadString(theNum,
                      2,
                      "item_defined_transformation.name",
                      theArch,
                      aItemDefinedTransformation_Name);

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Description;
  Standard_Boolean                 hasItemDefinedTransformation_Description = Standard_True;
  if (theData->IsParamDefined(theNum, 3))
  {
    theData->ReadString(theNum,
                        3,
                        "item_defined_transformation.description",
                        theArch,
                        aItemDefinedTransformation_Description);
  }
  else
  {
    hasItemDefinedTransformation_Description = Standard_False;
    aItemDefinedTransformation_Description.Nullify();
  }

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem1;
  theData->ReadEntity(theNum,
                      4,
                      "item_defined_transformation.transform_item1",
                      theArch,
                      STANDARD_TYPE(StepRepr_RepresentationItem),
                      aItemDefinedTransformation_TransformItem1);

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem2;
  theData->ReadEntity(theNum,
                      5,
                      "item_defined_transformation.transform_item2",
                      theArch,
                      STANDARD_TYPE(StepRepr_RepresentationItem),
                      aItemDefinedTransformation_TransformItem2);

  // Inherited fields of KinematicPair

  Handle(StepKinematics_KinematicJoint) aKinematicPair_Joint;
  theData->ReadEntity(theNum,
                      6,
                      "kinematic_pair.joint",
                      theArch,
                      STANDARD_TYPE(StepKinematics_KinematicJoint),
                      aKinematicPair_Joint);

  // Inherited fields of RackAndPinionPair

  Standard_Real aRackAndPinionPair_PinionRadius;
  theData->ReadReal(theNum,
                    7,
                    "rack_and_pinion_pair.pinion_radius",
                    theArch,
                    aRackAndPinionPair_PinionRadius);

  // Own fields of RackAndPinionPairWithRange

  Standard_Real    aLowerLimitRackDisplacement;
  Standard_Boolean hasLowerLimitRackDisplacement = Standard_True;
  if (theData->IsParamDefined(theNum, 8))
  {
    theData->ReadReal(theNum,
                      8,
                      "lower_limit_rack_displacement",
                      theArch,
                      aLowerLimitRackDisplacement);
  }
  else
  {
    hasLowerLimitRackDisplacement = Standard_False;
    aLowerLimitRackDisplacement   = 0;
  }

  Standard_Real    aUpperLimitRackDisplacement;
  Standard_Boolean hasUpperLimitRackDisplacement = Standard_True;
  if (theData->IsParamDefined(theNum, 9))
  {
    theData->ReadReal(theNum,
                      9,
                      "upper_limit_rack_displacement",
                      theArch,
                      aUpperLimitRackDisplacement);
  }
  else
  {
    hasUpperLimitRackDisplacement = Standard_False;
    aUpperLimitRackDisplacement   = 0;
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
               aItemDefinedTransformation_Name,
               hasItemDefinedTransformation_Description,
               aItemDefinedTransformation_Description,
               aItemDefinedTransformation_TransformItem1,
               aItemDefinedTransformation_TransformItem2,
               aKinematicPair_Joint,
               aRackAndPinionPair_PinionRadius,
               hasLowerLimitRackDisplacement,
               aLowerLimitRackDisplacement,
               hasUpperLimitRackDisplacement,
               aUpperLimitRackDisplacement);
}

//=================================================================================================

void RWStepKinematics_RWRackAndPinionPairWithRange::WriteStep(
  StepData_StepWriter&                                     theSW,
  const Handle(StepKinematics_RackAndPinionPairWithRange)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Inherited fields of ItemDefinedTransformation

  theSW.Send(theEnt->ItemDefinedTransformation()->Name());

  if (theEnt->ItemDefinedTransformation()->HasDescription())
  {
    theSW.Send(theEnt->ItemDefinedTransformation()->Description());
  }
  else
    theSW.SendUndef();

  theSW.Send(theEnt->ItemDefinedTransformation()->TransformItem1());

  theSW.Send(theEnt->ItemDefinedTransformation()->TransformItem2());

  // Own fields of KinematicPair

  theSW.Send(theEnt->Joint());

  // Own fields of RackAndPinionPair

  theSW.Send(theEnt->PinionRadius());

  // Own fields of RackAndPinionPairWithRange

  if (theEnt->HasLowerLimitRackDisplacement())
  {
    theSW.Send(theEnt->LowerLimitRackDisplacement());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasUpperLimitRackDisplacement())
  {
    theSW.Send(theEnt->UpperLimitRackDisplacement());
  }
  else
    theSW.SendUndef();
}

//=================================================================================================

void RWStepKinematics_RWRackAndPinionPairWithRange::Share(
  const Handle(StepKinematics_RackAndPinionPairWithRange)& theEnt,
  Interface_EntityIterator&                                iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem(theEnt->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem(theEnt->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem(theEnt->StepKinematics_KinematicPair::Joint());

  // Inherited fields of RackAndPinionPair

  // Own fields of RackAndPinionPairWithRange
}
