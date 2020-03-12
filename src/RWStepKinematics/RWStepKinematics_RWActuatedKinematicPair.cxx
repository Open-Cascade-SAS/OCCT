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

#include <RWStepKinematics_RWActuatedKinematicPair.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_ActuatedKinematicPair.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepKinematics_ActuatedDirection.hxx>

//=======================================================================
//function : RWStepKinematics_RWActuatedKinematicPair
//purpose  :
//=======================================================================
RWStepKinematics_RWActuatedKinematicPair::RWStepKinematics_RWActuatedKinematicPair() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWActuatedKinematicPair::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                         const Standard_Integer theNum,
                                                         Handle(Interface_Check)& theArch,
                                                         const Handle(StepKinematics_ActuatedKinematicPair)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,12,theArch,"actuated_kinematic_pair") ) return;

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

  // Own fields of ActuatedKinematicPair

  StepKinematics_ActuatedDirection aTX = StepKinematics_adNotActuated;
  Standard_Boolean hasTX = Standard_True;
  if ( theData->IsParamDefined (theNum,7) ) {
    if (theData->ParamType (theNum, 7) == Interface_ParamEnum) {
      Standard_CString text = theData->ParamCValue(theNum, 7);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aTX = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aTX = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aTX = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aTX = StepKinematics_adNotActuated;
      else theArch->AddFail("Parameter #7 (t_x) has not allowed value");
    }
    else theArch->AddFail("Parameter #7 (t_x) is not enumeration");
  }
  else {
    hasTX = Standard_False;
  }

  StepKinematics_ActuatedDirection aTY = StepKinematics_adNotActuated;
  Standard_Boolean hasTY = Standard_True;
  if ( theData->IsParamDefined (theNum,8) ) {
    if (theData->ParamType (theNum, 8) == Interface_ParamEnum) {
      Standard_CString text = theData->ParamCValue(theNum, 8);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aTY = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aTY = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aTY = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aTY = StepKinematics_adNotActuated;
      else theArch->AddFail("Parameter #8 (t_y) has not allowed value");
    }
    else theArch->AddFail("Parameter #8 (t_y) is not enumeration");
  }
  else {
    hasTY = Standard_False;
  }

  StepKinematics_ActuatedDirection aTZ = StepKinematics_adNotActuated;
  Standard_Boolean hasTZ = Standard_True;
  if ( theData->IsParamDefined (theNum,9) ) {
    if (theData->ParamType (theNum, 9) == Interface_ParamEnum) {
      Standard_CString text = theData->ParamCValue(theNum, 9);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aTZ = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aTZ = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aTZ = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aTZ = StepKinematics_adNotActuated;
      else theArch->AddFail("Parameter #9 (t_z) has not allowed value");
    }
    else theArch->AddFail("Parameter #9 (t_z) is not enumeration");
  }
  else {
    hasTZ = Standard_False;
  }

  StepKinematics_ActuatedDirection aRX = StepKinematics_adNotActuated;
  Standard_Boolean hasRX = Standard_True;
  if ( theData->IsParamDefined (theNum,10) ) {
    if (theData->ParamType (theNum, 10) == Interface_ParamEnum) {
      Standard_CString text = theData->ParamCValue(theNum, 10);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aRX = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aRX = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aRX = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aRX = StepKinematics_adNotActuated;
      else theArch->AddFail("Parameter #10 (r_x) has not allowed value");
    }
    else theArch->AddFail("Parameter #10 (r_x) is not enumeration");
  }
  else {
    hasRX = Standard_False;
  }

  StepKinematics_ActuatedDirection aRY = StepKinematics_adNotActuated;
  Standard_Boolean hasRY = Standard_True;
  if ( theData->IsParamDefined (theNum,11) ) {
    if (theData->ParamType (theNum, 11) == Interface_ParamEnum) {
      Standard_CString text = theData->ParamCValue(theNum, 11);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aRY = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aRY = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aRY = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aRY = StepKinematics_adNotActuated;
      else theArch->AddFail("Parameter #11 (r_y) has not allowed value");
    }
    else theArch->AddFail("Parameter #11 (r_y) is not enumeration");
  }
  else {
    hasRY = Standard_False;
  }

  StepKinematics_ActuatedDirection aRZ = StepKinematics_adNotActuated;
  Standard_Boolean hasRZ = Standard_True;
  if ( theData->IsParamDefined (theNum,12) ) {
    if (theData->ParamType (theNum, 12) == Interface_ParamEnum) {
      Standard_CString text = theData->ParamCValue(theNum, 12);
      if      (strcmp(text, ".BIDIRECTIONAL.")) aRZ = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY.")) aRZ = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY.")) aRZ = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED.")) aRZ = StepKinematics_adNotActuated;
      else theArch->AddFail("Parameter #12 (r_z) has not allowed value");
    }
    else theArch->AddFail("Parameter #12 (r_z) is not enumeration");
  }
  else {
    hasRZ = Standard_False;
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            hasTX,
            aTX,
            hasTY,
            aTY,
            hasTZ,
            aTZ,
            hasRX,
            aRX,
            hasRY,
            aRY,
            hasRZ,
            aRZ);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWActuatedKinematicPair::WriteStep (StepData_StepWriter& theSW,
                                                          const Handle(StepKinematics_ActuatedKinematicPair)& theEnt) const
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

  // Own fields of ActuatedKinematicPair

  if ( theEnt->HasTX() ) {
    switch (theEnt->TX()) {
      case StepKinematics_adBidirectional: theSW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: theSW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: theSW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: theSW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else theSW.SendUndef();

  if ( theEnt->HasTY() ) {
    switch (theEnt->TY()) {
      case StepKinematics_adBidirectional: theSW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: theSW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: theSW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: theSW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else theSW.SendUndef();

  if ( theEnt->HasTZ() ) {
    switch (theEnt->TZ()) {
      case StepKinematics_adBidirectional: theSW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: theSW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: theSW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: theSW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else theSW.SendUndef();

  if ( theEnt->HasRX() ) {
    switch (theEnt->RX()) {
      case StepKinematics_adBidirectional: theSW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: theSW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: theSW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: theSW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else theSW.SendUndef();

  if ( theEnt->HasRY() ) {
    switch (theEnt->RY()) {
      case StepKinematics_adBidirectional: theSW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: theSW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: theSW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: theSW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else theSW.SendUndef();

  if ( theEnt->HasRZ() ) {
    switch (theEnt->RZ()) {
      case StepKinematics_adBidirectional: theSW.SendEnum (".BIDIRECTIONAL."); break;
      case StepKinematics_adPositiveOnly: theSW.SendEnum (".POSITIVE_ONLY."); break;
      case StepKinematics_adNegativeOnly: theSW.SendEnum (".NEGATIVE_ONLY."); break;
      case StepKinematics_adNotActuated: theSW.SendEnum (".NOT_ACTUATED."); break;
    }
  }
  else theSW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWActuatedKinematicPair::Share (const Handle(StepKinematics_ActuatedKinematicPair)& theEnt,
                                                      Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (theEnt->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (theEnt->StepKinematics_KinematicPair::Joint());

  // Own fields of ActuatedKinematicPair
}
