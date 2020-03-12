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

#include <RWStepKinematics_RWGearPairWithRange.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_GearPairWithRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWGearPairWithRange
//purpose  :
//=======================================================================
RWStepKinematics_RWGearPairWithRange::RWStepKinematics_RWGearPairWithRange() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWGearPairWithRange::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                     const Standard_Integer theNum,
                                                     Handle(Interface_Check)& theArch,
                                                     const Handle(StepKinematics_GearPairWithRange)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,13,theArch,"gear_pair_with_range") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString (theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);

  // Inherited fields of ItemDefinedTransformation

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Name;
  theData->ReadString (theNum, 2, "item_defined_transformation.name", theArch, aItemDefinedTransformation_Name);

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Description;
  Standard_Boolean hasItemDefinedTransformation_Description = Standard_True;
  if ( theData->IsParamDefined (theNum,3) ) {
    theData->ReadString (theNum, 3, "item_defined_transformation.description", theArch, aItemDefinedTransformation_Description);
  }
  else {
    hasItemDefinedTransformation_Description = Standard_False;
    aItemDefinedTransformation_Description.Nullify();
  }

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem1;
  theData->ReadEntity (theNum, 4, "item_defined_transformation.transform_item1", theArch, STANDARD_TYPE(StepRepr_RepresentationItem), aItemDefinedTransformation_TransformItem1);

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem2;
  theData->ReadEntity (theNum, 5, "item_defined_transformation.transform_item2", theArch, STANDARD_TYPE(StepRepr_RepresentationItem), aItemDefinedTransformation_TransformItem2);

  // Inherited fields of KinematicPair

  Handle(StepKinematics_KinematicJoint) aKinematicPair_Joint;
  theData->ReadEntity (theNum, 6, "kinematic_pair.joint", theArch, STANDARD_TYPE(StepKinematics_KinematicJoint), aKinematicPair_Joint);

  // Inherited fields of GearPair

  Standard_Real aGearPair_RadiusFirstLink;
  theData->ReadReal (theNum, 7, "gear_pair.radius_first_link", theArch, aGearPair_RadiusFirstLink);

  Standard_Real aGearPair_RadiusSecondLink;
  theData->ReadReal (theNum, 8, "gear_pair.radius_second_link", theArch, aGearPair_RadiusSecondLink);

  Standard_Real aGearPair_Bevel;
  theData->ReadReal (theNum, 9, "gear_pair.bevel", theArch, aGearPair_Bevel);

  Standard_Real aGearPair_HelicalAngle;
  theData->ReadReal (theNum, 10, "gear_pair.helical_angle", theArch, aGearPair_HelicalAngle);

  Standard_Real aGearPair_GearRatio;
  theData->ReadReal (theNum, 11, "gear_pair.gear_ratio", theArch, aGearPair_GearRatio);

  // Own fields of GearPairWithRange

  Standard_Real aLowerLimitActualRotation1;
  Standard_Boolean hasLowerLimitActualRotation1 = Standard_True;
  if ( theData->IsParamDefined (theNum,12) ) {
    theData->ReadReal (theNum, 12, "lower_limit_actual_rotation1", theArch, aLowerLimitActualRotation1);
  }
  else {
    hasLowerLimitActualRotation1 = Standard_False;
    aLowerLimitActualRotation1 = 0;
  }

  Standard_Real aUpperLimitActualRotation1;
  Standard_Boolean hasUpperLimitActualRotation1 = Standard_True;
  if ( theData->IsParamDefined (theNum,13) ) {
    theData->ReadReal (theNum, 13, "upper_limit_actual_rotation1", theArch, aUpperLimitActualRotation1);
  }
  else {
    hasUpperLimitActualRotation1 = Standard_False;
    aUpperLimitActualRotation1 = 0;
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aGearPair_RadiusFirstLink,
            aGearPair_RadiusSecondLink,
            aGearPair_Bevel,
            aGearPair_HelicalAngle,
            aGearPair_GearRatio,
            hasLowerLimitActualRotation1,
            aLowerLimitActualRotation1,
            hasUpperLimitActualRotation1,
            aUpperLimitActualRotation1);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWGearPairWithRange::WriteStep (StepData_StepWriter& theSW,
                                                      const Handle(StepKinematics_GearPairWithRange)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send (theEnt->Name());

  // Inherited fields of ItemDefinedTransformation

  theSW.Send (theEnt->ItemDefinedTransformation()->Name());

  if ( theEnt->ItemDefinedTransformation()->HasDescription() ) {
    theSW.Send (theEnt->ItemDefinedTransformation()->Description());
  }
  else theSW.SendUndef();

  theSW.Send (theEnt->ItemDefinedTransformation()->TransformItem1());

  theSW.Send (theEnt->ItemDefinedTransformation()->TransformItem2());

  // Own fields of KinematicPair

  theSW.Send (theEnt->Joint());

  // Own fields of GearPair

  theSW.Send (theEnt->RadiusFirstLink());

  theSW.Send (theEnt->RadiusSecondLink());

  theSW.Send (theEnt->Bevel());

  theSW.Send (theEnt->HelicalAngle());

  theSW.Send (theEnt->GearRatio());

  // Own fields of GearPairWithRange

  if ( theEnt->HasLowerLimitActualRotation1() ) {
    theSW.Send (theEnt->LowerLimitActualRotation1());
  }
  else theSW.SendUndef();

  if ( theEnt->HasUpperLimitActualRotation1() ) {
    theSW.Send (theEnt->UpperLimitActualRotation1());
  }
  else theSW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWGearPairWithRange::Share (const Handle(StepKinematics_GearPairWithRange)& theEnt,
                                                  Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (theEnt->StepKinematics_KinematicPair::Joint());

  // Inherited fields of GearPair

  // Own fields of GearPairWithRange
}
