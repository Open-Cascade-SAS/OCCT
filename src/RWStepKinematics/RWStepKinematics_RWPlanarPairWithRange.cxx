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

#include "RWStepKinematics_RWPlanarPairWithRange.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_PlanarPairWithRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <Standard_Real.hxx>

//=================================================================================================

RWStepKinematics_RWPlanarPairWithRange::RWStepKinematics_RWPlanarPairWithRange() {}

//=================================================================================================

void RWStepKinematics_RWPlanarPairWithRange::ReadStep(
  const Handle(StepData_StepReaderData)&            theData,
  const Standard_Integer                            theNum,
  Handle(Interface_Check)&                          theArch,
  const Handle(StepKinematics_PlanarPairWithRange)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 18, theArch, "planar_pair_with_range"))
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

  // Inherited fields of LowOrderKinematicPair

  Standard_Boolean aLowOrderKinematicPair_TX;
  theData->ReadBoolean(theNum,
                       7,
                       "low_order_kinematic_pair.t_x",
                       theArch,
                       aLowOrderKinematicPair_TX);

  Standard_Boolean aLowOrderKinematicPair_TY;
  theData->ReadBoolean(theNum,
                       8,
                       "low_order_kinematic_pair.t_y",
                       theArch,
                       aLowOrderKinematicPair_TY);

  Standard_Boolean aLowOrderKinematicPair_TZ;
  theData->ReadBoolean(theNum,
                       9,
                       "low_order_kinematic_pair.t_z",
                       theArch,
                       aLowOrderKinematicPair_TZ);

  Standard_Boolean aLowOrderKinematicPair_RX;
  theData->ReadBoolean(theNum,
                       10,
                       "low_order_kinematic_pair.r_x",
                       theArch,
                       aLowOrderKinematicPair_RX);

  Standard_Boolean aLowOrderKinematicPair_RY;
  theData->ReadBoolean(theNum,
                       11,
                       "low_order_kinematic_pair.r_y",
                       theArch,
                       aLowOrderKinematicPair_RY);

  Standard_Boolean aLowOrderKinematicPair_RZ;
  theData->ReadBoolean(theNum,
                       12,
                       "low_order_kinematic_pair.r_z",
                       theArch,
                       aLowOrderKinematicPair_RZ);

  // Own fields of PlanarPairWithRange

  Standard_Real    aLowerLimitActualRotation;
  Standard_Boolean hasLowerLimitActualRotation = Standard_True;
  if (theData->IsParamDefined(theNum, 13))
  {
    theData->ReadReal(theNum,
                      13,
                      "lower_limit_actual_rotation",
                      theArch,
                      aLowerLimitActualRotation);
  }
  else
  {
    hasLowerLimitActualRotation = Standard_False;
    aLowerLimitActualRotation   = 0;
  }

  Standard_Real    aUpperLimitActualRotation;
  Standard_Boolean hasUpperLimitActualRotation = Standard_True;
  if (theData->IsParamDefined(theNum, 14))
  {
    theData->ReadReal(theNum,
                      14,
                      "upper_limit_actual_rotation",
                      theArch,
                      aUpperLimitActualRotation);
  }
  else
  {
    hasUpperLimitActualRotation = Standard_False;
    aUpperLimitActualRotation   = 0;
  }

  Standard_Real    aLowerLimitActualTranslationX;
  Standard_Boolean hasLowerLimitActualTranslationX = Standard_True;
  if (theData->IsParamDefined(theNum, 15))
  {
    theData->ReadReal(theNum,
                      15,
                      "lower_limit_actual_translation_x",
                      theArch,
                      aLowerLimitActualTranslationX);
  }
  else
  {
    hasLowerLimitActualTranslationX = Standard_False;
    aLowerLimitActualTranslationX   = 0;
  }

  Standard_Real    aUpperLimitActualTranslationX;
  Standard_Boolean hasUpperLimitActualTranslationX = Standard_True;
  if (theData->IsParamDefined(theNum, 16))
  {
    theData->ReadReal(theNum,
                      16,
                      "upper_limit_actual_translation_x",
                      theArch,
                      aUpperLimitActualTranslationX);
  }
  else
  {
    hasUpperLimitActualTranslationX = Standard_False;
    aUpperLimitActualTranslationX   = 0;
  }

  Standard_Real    aLowerLimitActualTranslationY;
  Standard_Boolean hasLowerLimitActualTranslationY = Standard_True;
  if (theData->IsParamDefined(theNum, 17))
  {
    theData->ReadReal(theNum,
                      17,
                      "lower_limit_actual_translation_y",
                      theArch,
                      aLowerLimitActualTranslationY);
  }
  else
  {
    hasLowerLimitActualTranslationY = Standard_False;
    aLowerLimitActualTranslationY   = 0;
  }

  Standard_Real    aUpperLimitActualTranslationY;
  Standard_Boolean hasUpperLimitActualTranslationY = Standard_True;
  if (theData->IsParamDefined(theNum, 18))
  {
    theData->ReadReal(theNum,
                      18,
                      "upper_limit_actual_translation_y",
                      theArch,
                      aUpperLimitActualTranslationY);
  }
  else
  {
    hasUpperLimitActualTranslationY = Standard_False;
    aUpperLimitActualTranslationY   = 0;
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
               hasLowerLimitActualRotation,
               aLowerLimitActualRotation,
               hasUpperLimitActualRotation,
               aUpperLimitActualRotation,
               hasLowerLimitActualTranslationX,
               aLowerLimitActualTranslationX,
               hasUpperLimitActualTranslationX,
               aUpperLimitActualTranslationX,
               hasLowerLimitActualTranslationY,
               aLowerLimitActualTranslationY,
               hasUpperLimitActualTranslationY,
               aUpperLimitActualTranslationY);
}

//=================================================================================================

void RWStepKinematics_RWPlanarPairWithRange::WriteStep(
  StepData_StepWriter&                              theSW,
  const Handle(StepKinematics_PlanarPairWithRange)& theEnt) const
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

  // Own fields of LowOrderKinematicPair

  theSW.SendBoolean(theEnt->TX());

  theSW.SendBoolean(theEnt->TY());

  theSW.SendBoolean(theEnt->TZ());

  theSW.SendBoolean(theEnt->RX());

  theSW.SendBoolean(theEnt->RY());

  theSW.SendBoolean(theEnt->RZ());

  // Own fields of PlanarPairWithRange

  if (theEnt->HasLowerLimitActualRotation())
  {
    theSW.Send(theEnt->LowerLimitActualRotation());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasUpperLimitActualRotation())
  {
    theSW.Send(theEnt->UpperLimitActualRotation());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasLowerLimitActualTranslationX())
  {
    theSW.Send(theEnt->LowerLimitActualTranslationX());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasUpperLimitActualTranslationX())
  {
    theSW.Send(theEnt->UpperLimitActualTranslationX());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasLowerLimitActualTranslationY())
  {
    theSW.Send(theEnt->LowerLimitActualTranslationY());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasUpperLimitActualTranslationY())
  {
    theSW.Send(theEnt->UpperLimitActualTranslationY());
  }
  else
    theSW.SendUndef();
}

//=================================================================================================

void RWStepKinematics_RWPlanarPairWithRange::Share(
  const Handle(StepKinematics_PlanarPairWithRange)& theEnt,
  Interface_EntityIterator&                         iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem(theEnt->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem(theEnt->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem(theEnt->StepKinematics_KinematicPair::Joint());

  // Inherited fields of LowOrderKinematicPair

  // Own fields of PlanarPairWithRange
}
