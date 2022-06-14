// Created on : Sat May 02 12:41:16 2020 
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

#include <RWStepKinematics_RWUniversalPairWithRange.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_UniversalPairWithRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWUniversalPairWithRange
//purpose  :
//=======================================================================
RWStepKinematics_RWUniversalPairWithRange::RWStepKinematics_RWUniversalPairWithRange() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWUniversalPairWithRange::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                          const Standard_Integer theNum,
                                                          Handle(Interface_Check)& theArch,
                                                          const Handle(StepKinematics_UniversalPairWithRange)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,17,theArch,"universal_pair_with_range") ) return;

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

  // Inherited fields of LowOrderKinematicPair

  Standard_Boolean aLowOrderKinematicPair_TX;
  theData->ReadBoolean (theNum, 7, "low_order_kinematic_pair.t_x", theArch, aLowOrderKinematicPair_TX);

  Standard_Boolean aLowOrderKinematicPair_TY;
  theData->ReadBoolean (theNum, 8, "low_order_kinematic_pair.t_y", theArch, aLowOrderKinematicPair_TY);

  Standard_Boolean aLowOrderKinematicPair_TZ;
  theData->ReadBoolean (theNum, 9, "low_order_kinematic_pair.t_z", theArch, aLowOrderKinematicPair_TZ);

  Standard_Boolean aLowOrderKinematicPair_RX;
  theData->ReadBoolean (theNum, 10, "low_order_kinematic_pair.r_x", theArch, aLowOrderKinematicPair_RX);

  Standard_Boolean aLowOrderKinematicPair_RY;
  theData->ReadBoolean (theNum, 11, "low_order_kinematic_pair.r_y", theArch, aLowOrderKinematicPair_RY);

  Standard_Boolean aLowOrderKinematicPair_RZ;
  theData->ReadBoolean (theNum, 12, "low_order_kinematic_pair.r_z", theArch, aLowOrderKinematicPair_RZ);

  // Inherited fields of UniversalPair

  Standard_Real aUniversalPair_InputSkewAngle;
  Standard_Boolean hasUniversalPair_InputSkewAngle = Standard_True;
  if ( theData->IsParamDefined (theNum,13) ) {
    theData->ReadReal (theNum, 13, "universal_pair.input_skew_angle", theArch, aUniversalPair_InputSkewAngle);
  }
  else {
    hasUniversalPair_InputSkewAngle = Standard_False;
    aUniversalPair_InputSkewAngle = 0;
  }

  // Own fields of UniversalPairWithRange

  Standard_Real aLowerLimitFirstRotation;
  Standard_Boolean hasLowerLimitFirstRotation = Standard_True;
  if ( theData->IsParamDefined (theNum,14) ) {
    theData->ReadReal (theNum, 14, "lower_limit_first_rotation", theArch, aLowerLimitFirstRotation);
  }
  else {
    hasLowerLimitFirstRotation = Standard_False;
    aLowerLimitFirstRotation = 0;
  }

  Standard_Real aUpperLimitFirstRotation;
  Standard_Boolean hasUpperLimitFirstRotation = Standard_True;
  if ( theData->IsParamDefined (theNum,15) ) {
    theData->ReadReal (theNum, 15, "upper_limit_first_rotation", theArch, aUpperLimitFirstRotation);
  }
  else {
    hasUpperLimitFirstRotation = Standard_False;
    aUpperLimitFirstRotation = 0;
  }

  Standard_Real aLowerLimitSecondRotation;
  Standard_Boolean hasLowerLimitSecondRotation = Standard_True;
  if ( theData->IsParamDefined (theNum,16) ) {
    theData->ReadReal (theNum, 16, "lower_limit_second_rotation", theArch, aLowerLimitSecondRotation);
  }
  else {
    hasLowerLimitSecondRotation = Standard_False;
    aLowerLimitSecondRotation = 0;
  }

  Standard_Real aUpperLimitSecondRotation;
  Standard_Boolean hasUpperLimitSecondRotation = Standard_True;
  if ( theData->IsParamDefined (theNum,17) ) {
    theData->ReadReal (theNum, 17, "upper_limit_second_rotation", theArch, aUpperLimitSecondRotation);
  }
  else {
    hasUpperLimitSecondRotation = Standard_False;
    aUpperLimitSecondRotation = 0;
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aLowOrderKinematicPair_TX,
            aLowOrderKinematicPair_TY,
            aLowOrderKinematicPair_TZ,
            aLowOrderKinematicPair_RX,
            aLowOrderKinematicPair_RY,
            aLowOrderKinematicPair_RZ,
            hasUniversalPair_InputSkewAngle,
            aUniversalPair_InputSkewAngle,
            hasLowerLimitFirstRotation,
            aLowerLimitFirstRotation,
            hasUpperLimitFirstRotation,
            aUpperLimitFirstRotation,
            hasLowerLimitSecondRotation,
            aLowerLimitSecondRotation,
            hasUpperLimitSecondRotation,
            aUpperLimitSecondRotation);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWUniversalPairWithRange::WriteStep (StepData_StepWriter& theSW,
                                                           const Handle(StepKinematics_UniversalPairWithRange)& theEnt) const
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

  // Own fields of LowOrderKinematicPair

  theSW.SendBoolean (theEnt->TX());

  theSW.SendBoolean (theEnt->TY());

  theSW.SendBoolean (theEnt->TZ());

  theSW.SendBoolean (theEnt->RX());

  theSW.SendBoolean (theEnt->RY());

  theSW.SendBoolean (theEnt->RZ());

  // Own fields of UniversalPair

  if ( theEnt->HasInputSkewAngle() ) {
    theSW.Send (theEnt->InputSkewAngle());
  }
  else theSW.SendUndef();

  // Own fields of UniversalPairWithRange

  if ( theEnt->HasLowerLimitFirstRotation() ) {
    theSW.Send (theEnt->LowerLimitFirstRotation());
  }
  else theSW.SendUndef();

  if ( theEnt->HasUpperLimitFirstRotation() ) {
    theSW.Send (theEnt->UpperLimitFirstRotation());
  }
  else theSW.SendUndef();

  if ( theEnt->HasLowerLimitSecondRotation() ) {
    theSW.Send (theEnt->LowerLimitSecondRotation());
  }
  else theSW.SendUndef();

  if ( theEnt->HasUpperLimitSecondRotation() ) {
    theSW.Send (theEnt->UpperLimitSecondRotation());
  }
  else theSW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWUniversalPairWithRange::Share (const Handle(StepKinematics_UniversalPairWithRange)& theEnt,
                                                       Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (theEnt->StepKinematics_KinematicPair::Joint());

  // Inherited fields of LowOrderKinematicPair

  // Inherited fields of UniversalPair

  // Own fields of UniversalPairWithRange
}
