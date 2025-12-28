// Created on: 2020-05-26
// Created by: PASUKHIN DMITRY
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include "RWStepKinematics_RWActuatedKinPairAndOrderKinPair.pxx"

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_ActuatedKinPairAndOrderKinPair.hxx>
#include <StepKinematics_ActuatedKinematicPair.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepKinematics_CylindricalPairWithRange.hxx>
#include <StepKinematics_GearPairWithRange.hxx>
#include <StepKinematics_LowOrderKinematicPair.hxx>
#include <StepKinematics_PlanarCurvePairRange.hxx>
#include <StepKinematics_PlanarPairWithRange.hxx>
#include <StepKinematics_PointOnPlanarCurvePairWithRange.hxx>
#include <StepKinematics_PointOnSurfacePairWithRange.hxx>
#include <StepKinematics_PrismaticPairWithRange.hxx>
#include <StepKinematics_RackAndPinionPairWithRange.hxx>
#include <StepKinematics_RevolutePairWithRange.hxx>
#include <StepKinematics_ScrewPairWithRange.hxx>
#include <StepKinematics_SphericalPairWithPinAndRange.hxx>
#include <StepKinematics_SphericalPairWithRange.hxx>
#include <StepKinematics_SurfacePairWithRange.hxx>
#include <StepKinematics_UniversalPairWithRange.hxx>

//=================================================================================================

RWStepKinematics_RWActuatedKinPairAndOrderKinPair::
  RWStepKinematics_RWActuatedKinPairAndOrderKinPair()
{
}

//=================================================================================================

void RWStepKinematics_RWActuatedKinPairAndOrderKinPair::ReadStep(
  const occ::handle<StepData_StepReaderData>&                       theData,
  const int                                                         num0,
  occ::handle<Interface_Check>&                                     theArch,
  const occ::handle<StepKinematics_ActuatedKinPairAndOrderKinPair>& theEnt) const
{
  int theNum = 0; // num0;
  theData->NamedForComplex("ACTUATED_KINEMATIC_PAIR", "ACKNPR", num0, theNum, theArch);
  if (!theData->CheckNbParams(theNum, 6, theArch, "kinematic actuated_kinematic_pair"))
    return;
  // Own fields of ActuatedKinematicPair
  occ::handle<StepKinematics_ActuatedKinematicPair> anActuatedKinematicPair =
    new StepKinematics_ActuatedKinematicPair;
  StepKinematics_ActuatedDirection aTX   = StepKinematics_adNotActuated;
  bool                             hasTX = true;
  if (theData->IsParamDefined(theNum, 1))
  {
    if (theData->ParamType(theNum, 1) == Interface_ParamEnum)
    {
      const char* text = theData->ParamCValue(theNum, 1);
      if (strcmp(text, ".BIDIRECTIONAL."))
        aTX = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY."))
        aTX = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY."))
        aTX = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED."))
        aTX = StepKinematics_adNotActuated;
      else
        theArch->AddFail("Parameter #1 (t_x) has not allowed value");
    }
    else
      theArch->AddFail("Parameter #1 (t_x) is not enumeration");
  }
  else
  {
    hasTX = false;
  }

  StepKinematics_ActuatedDirection aTY   = StepKinematics_adNotActuated;
  bool                             hasTY = true;
  if (theData->IsParamDefined(theNum, 2))
  {
    if (theData->ParamType(theNum, 2) == Interface_ParamEnum)
    {
      const char* text = theData->ParamCValue(theNum, 8);
      if (strcmp(text, ".BIDIRECTIONAL."))
        aTY = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY."))
        aTY = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY."))
        aTY = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED."))
        aTY = StepKinematics_adNotActuated;
      else
        theArch->AddFail("Parameter #2 (t_y) has not allowed value");
    }
    else
      theArch->AddFail("Parameter #2 (t_y) is not enumeration");
  }
  else
  {
    hasTY = false;
  }

  StepKinematics_ActuatedDirection aTZ   = StepKinematics_adNotActuated;
  bool                             hasTZ = true;
  if (theData->IsParamDefined(theNum, 3))
  {
    if (theData->ParamType(theNum, 3) == Interface_ParamEnum)
    {
      const char* text = theData->ParamCValue(theNum, 9);
      if (strcmp(text, ".BIDIRECTIONAL."))
        aTZ = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY."))
        aTZ = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY."))
        aTZ = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED."))
        aTZ = StepKinematics_adNotActuated;
      else
        theArch->AddFail("Parameter #3 (t_z) has not allowed value");
    }
    else
      theArch->AddFail("Parameter #3 (t_z) is not enumeration");
  }
  else
  {
    hasTZ = false;
  }

  StepKinematics_ActuatedDirection aRX   = StepKinematics_adNotActuated;
  bool                             hasRX = true;
  if (theData->IsParamDefined(theNum, 4))
  {
    if (theData->ParamType(theNum, 4) == Interface_ParamEnum)
    {
      const char* text = theData->ParamCValue(theNum, 10);
      if (strcmp(text, ".BIDIRECTIONAL."))
        aRX = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY."))
        aRX = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY."))
        aRX = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED."))
        aRX = StepKinematics_adNotActuated;
      else
        theArch->AddFail("Parameter #14 (r_x) has not allowed value");
    }
    else
      theArch->AddFail("Parameter #14 (r_x) is not enumeration");
  }
  else
  {
    hasRX = false;
  }

  StepKinematics_ActuatedDirection aRY   = StepKinematics_adNotActuated;
  bool                             hasRY = true;
  if (theData->IsParamDefined(theNum, 5))
  {
    if (theData->ParamType(theNum, 5) == Interface_ParamEnum)
    {
      const char* text = theData->ParamCValue(theNum, 11);
      if (strcmp(text, ".BIDIRECTIONAL."))
        aRY = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY."))
        aRY = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY."))
        aRY = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED."))
        aRY = StepKinematics_adNotActuated;
      else
        theArch->AddFail("Parameter #5 (r_y) has not allowed value");
    }
    else
      theArch->AddFail("Parameter #5 (r_y) is not enumeration");
  }
  else
  {
    hasRY = false;
  }

  StepKinematics_ActuatedDirection aRZ   = StepKinematics_adNotActuated;
  bool                             hasRZ = true;
  if (theData->IsParamDefined(theNum, 6))
  {
    if (theData->ParamType(theNum, 6) == Interface_ParamEnum)
    {
      const char* text = theData->ParamCValue(theNum, 12);
      if (strcmp(text, ".BIDIRECTIONAL."))
        aRZ = StepKinematics_adBidirectional;
      else if (strcmp(text, ".POSITIVE_ONLY."))
        aRZ = StepKinematics_adPositiveOnly;
      else if (strcmp(text, ".NEGATIVE_ONLY."))
        aRZ = StepKinematics_adNegativeOnly;
      else if (strcmp(text, ".NOT_ACTUATED."))
        aRZ = StepKinematics_adNotActuated;
      else
        theArch->AddFail("Parameter #6 (r_z) has not allowed value");
    }
    else
      theArch->AddFail("Parameter #6 (r_z) is not enumeration");
  }
  else
  {
    hasRZ = false;
  }

  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  theData->NamedForComplex("GEOMETRIC_REPRESENTATION_ITEM", "GMRPIT", num0, theNum, theArch);
  // geometricrepresentation_item is non-defined

  theData->NamedForComplex("ITEM_DEFINED_TRANSFORMATION", "ITDFTR", num0, theNum, theArch);
  // Own fields of item_defined_transformation

  occ::handle<TCollection_HAsciiString> aItemDefinedTransformation_Name;
  if (strcmp(theData->ParamCValue(theNum, 1), "*"))
    theData->ReadString(theNum,
                        1,
                        "item_defined_transformation.name",
                        theArch,
                        aItemDefinedTransformation_Name);

  occ::handle<TCollection_HAsciiString> aItemDefinedTransformation_Description;
  bool                                  hasItemDefinedTransformation_Description = true;
  if (theData->IsParamDefined(theNum, 2))
  {
    theData->ReadString(theNum,
                        2,
                        "item_defined_transformation.description",
                        theArch,
                        aItemDefinedTransformation_Description);
  }
  else
  {
    hasItemDefinedTransformation_Description = false;
    aItemDefinedTransformation_Description.Nullify();
  }

  occ::handle<StepRepr_RepresentationItem> aItemDefinedTransformation_TransformItem1;
  theData->ReadEntity(theNum,
                      3,
                      "ITEM_DEFINED_TRANSFORMATION",
                      theArch,
                      STANDARD_TYPE(StepRepr_RepresentationItem),
                      aItemDefinedTransformation_TransformItem1);

  occ::handle<StepRepr_RepresentationItem> aItemDefinedTransformation_TransformItem2;
  theData->ReadEntity(theNum,
                      4,
                      "ITEM_DEFINED_TRANSFORMATION",
                      theArch,
                      STANDARD_TYPE(StepRepr_RepresentationItem),
                      aItemDefinedTransformation_TransformItem2);

  theData->NamedForComplex("KINEMATIC_PAIR", "KNMPR", num0, theNum, theArch);

  // Inherited fields of KinematicPair

  occ::handle<StepKinematics_KinematicJoint> aKinematicPair_Joint;
  theData->ReadEntity(theNum,
                      1,
                      "kinematic_pair.joint",
                      theArch,
                      STANDARD_TYPE(StepKinematics_KinematicJoint),
                      aKinematicPair_Joint);

  // Select pair supertype
  theNum                                                   = theData->NextForComplex(theNum);
  const char*                               aPairSuperType = theData->CType(theNum);
  occ::handle<StepKinematics_KinematicPair> aKinematicPair;
  if (!strcmp(aPairSuperType, "LOW_ORDER_KINEMATIC_PAIR"))
  {
    // Own fields of low_order_kinematic_pair
    bool aLowOrderKinematicPair_TX = true;
    bool aLowOrderKinematicPair_TY = true;
    bool aLowOrderKinematicPair_TZ = true;
    bool aLowOrderKinematicPair_RX = true;
    bool aLowOrderKinematicPair_RY = true;
    bool aLowOrderKinematicPair_RZ = true;
    if (theData->CheckNbParams(theNum, 6, theArch, "LOW_ORDER_KINEMATIC_PAIR"))
    {
      if (strcmp(theData->ParamCValue(theNum, 1), "*"))
        theData->ReadBoolean(theNum,
                             1,
                             "low_order_kinematic_pair.t_x",
                             theArch,
                             aLowOrderKinematicPair_TX);
      if (strcmp(theData->ParamCValue(theNum, 2), "*"))
        theData->ReadBoolean(theNum,
                             2,
                             "low_order_kinematic_pair.t_y",
                             theArch,
                             aLowOrderKinematicPair_TY);
      if (strcmp(theData->ParamCValue(theNum, 3), "*"))
        theData->ReadBoolean(theNum,
                             3,
                             "low_order_kinematic_pair.t_z",
                             theArch,
                             aLowOrderKinematicPair_TZ);
      if (strcmp(theData->ParamCValue(theNum, 4), "*"))
        theData->ReadBoolean(theNum,
                             4,
                             "low_order_kinematic_pair.r_x",
                             theArch,
                             aLowOrderKinematicPair_RX);
      if (strcmp(theData->ParamCValue(theNum, 5), "*"))
        theData->ReadBoolean(theNum,
                             5,
                             "low_order_kinematic_pair.r_y",
                             theArch,
                             aLowOrderKinematicPair_RY);
      if (strcmp(theData->ParamCValue(theNum, 6), "*"))
        theData->ReadBoolean(theNum,
                             6,
                             "low_order_kinematic_pair.r_z",
                             theArch,
                             aLowOrderKinematicPair_RZ);
    }
    else
      theArch->AddFail("The supertype of kinematic pair is not supported");
    aKinematicPair = new StepKinematics_KinematicPair;
    aKinematicPair->Init(aRepresentationItem_Name,
                         aItemDefinedTransformation_Name,
                         hasItemDefinedTransformation_Description,
                         aItemDefinedTransformation_Description,
                         aItemDefinedTransformation_TransformItem1,
                         aItemDefinedTransformation_TransformItem2,
                         aKinematicPair_Joint);
    theNum                = theData->NextForComplex(theNum);
    const char* aPairtype = theData->CType(theNum);
    if (!strcmp(aPairtype, "REVOLUTE_PAIR"))
    {
      // Own fields of revolute_pair is non-defined

      if (theData->NamedForComplex("REVOLUTE_PAIR_WITH_RANGE", "RPWR", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 2, theArch, "REVOLUTE_PAIR_WITH_RANGE"))
        {
          // Own fields of revolute_pair_with_range
          aKinematicPair = new StepKinematics_RevolutePairWithRange;
          double aLowerLimitActualRotation;
          bool   hasLowerLimitActualRotation = true;
          if (theData->IsParamDefined(theNum, 1))
          {
            theData->ReadReal(theNum,
                              1,
                              "lower_limit_actual_rotation",
                              theArch,
                              aLowerLimitActualRotation);
          }
          else
          {
            hasLowerLimitActualRotation = false;
            aLowerLimitActualRotation   = 0;
          }

          double aUpperLimitActualRotation;
          bool   hasUpperLimitActualRotation = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum,
                              2,
                              "upper_limit_actual_rotation",
                              theArch,
                              aUpperLimitActualRotation);
          }
          else
          {
            hasUpperLimitActualRotation = false;
            aUpperLimitActualRotation   = 0;
          }
          occ::handle<StepKinematics_RevolutePairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_RevolutePairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           aUpperLimitActualRotation);
        }
        else // revolute_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_RevolutePair;
          occ::handle<StepKinematics_RevolutePair> aLocalPair =
            occ::down_cast<StepKinematics_RevolutePair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           aLowOrderKinematicPair_RZ);
        }
      }
    }
    else if (!strcmp(aPairtype, "CYLINDRICAL_PAIR"))
    {
      // Own fields of cylindrical_pair is non-defined

      if (theData->NamedForComplex("CYLINDRICAL_PAIR_WITH_RANGE", "CPW0", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 4, theArch, "CYLINDRICAL_PAIR_WITH_RANGE"))
        {
          // Own fields of cylindrical_pair_with_range
          aKinematicPair = new StepKinematics_CylindricalPairWithRange;

          double aLowerLimitActualTranslation;
          bool   hasLowerLimitActualTranslation = true;
          if (theData->IsParamDefined(theNum, 1))
          {
            theData->ReadReal(theNum,
                              1,
                              "lower_limit_actual_translation",
                              theArch,
                              aLowerLimitActualTranslation);
          }
          else
          {
            hasLowerLimitActualTranslation = false;
            aLowerLimitActualTranslation   = 0;
          }

          double aUpperLimitActualTranslation;
          bool   hasUpperLimitActualTranslation = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum,
                              2,
                              "upper_limit_actual_translation",
                              theArch,
                              aUpperLimitActualTranslation);
          }
          else
          {
            hasUpperLimitActualTranslation = false;
            aUpperLimitActualTranslation   = 0;
          }

          double aLowerLimitActualRotation;
          bool   hasLowerLimitActualRotation = true;
          if (theData->IsParamDefined(theNum, 3))
          {
            theData->ReadReal(theNum,
                              3,
                              "lower_limit_actual_rotation",
                              theArch,
                              aLowerLimitActualRotation);
          }
          else
          {
            hasLowerLimitActualRotation = false;
            aLowerLimitActualRotation   = 0;
          }

          double aUpperLimitActualRotation;
          bool   hasUpperLimitActualRotation = true;
          if (theData->IsParamDefined(theNum, 4))
          {
            theData->ReadReal(theNum,
                              4,
                              "upper_limit_actual_rotation",
                              theArch,
                              aUpperLimitActualRotation);
          }
          else
          {
            hasUpperLimitActualRotation = false;
            aUpperLimitActualRotation   = 0;
          }
          occ::handle<StepKinematics_CylindricalPairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_CylindricalPairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           hasLowerLimitActualTranslation,
                           aLowerLimitActualTranslation,
                           hasUpperLimitActualTranslation,
                           aUpperLimitActualTranslation,
                           hasLowerLimitActualRotation,
                           aLowerLimitActualRotation,
                           hasUpperLimitActualRotation,
                           aUpperLimitActualRotation);
        }
        else // cylindrical_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_CylindricalPair;
          occ::handle<StepKinematics_CylindricalPair> aLocalPair =
            occ::down_cast<StepKinematics_CylindricalPair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           aLowOrderKinematicPair_RZ);
        }
      }
    }
    else if (!strcmp(aPairtype, "SPHERICAL_PAIR"))
    {
      // Own fields of cylindrical_pair is non-defined

      if (theData->NamedForComplex("SPHERICAL_PAIR_WITH_RANGE", "SPW0", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 6, theArch, "SPHERICAL_PAIR_WITH_RANGE"))
        {
          // Own fields of spherical_pair_with_range
          aKinematicPair = new StepKinematics_SphericalPairWithRange;

          double aLowerLimitYaw;
          bool   hasLowerLimitYaw = true;
          if (theData->IsParamDefined(theNum, 1))
          {
            theData->ReadReal(theNum, 1, "lower_limit_yaw", theArch, aLowerLimitYaw);
          }
          else
          {
            hasLowerLimitYaw = false;
            aLowerLimitYaw   = 0;
          }

          double aUpperLimitYaw;
          bool   hasUpperLimitYaw = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum, 2, "upper_limit_yaw", theArch, aUpperLimitYaw);
          }
          else
          {
            hasUpperLimitYaw = false;
            aUpperLimitYaw   = 0;
          }

          double aLowerLimitPitch;
          bool   hasLowerLimitPitch = true;
          if (theData->IsParamDefined(theNum, 3))
          {
            theData->ReadReal(theNum, 3, "lower_limit_pitch", theArch, aLowerLimitPitch);
          }
          else
          {
            hasLowerLimitPitch = false;
            aLowerLimitPitch   = 0;
          }

          double aUpperLimitPitch;
          bool   hasUpperLimitPitch = true;
          if (theData->IsParamDefined(theNum, 4))
          {
            theData->ReadReal(theNum, 4, "upper_limit_pitch", theArch, aUpperLimitPitch);
          }
          else
          {
            hasUpperLimitPitch = false;
            aUpperLimitPitch   = 0;
          }

          double aLowerLimitRoll;
          bool   hasLowerLimitRoll = true;
          if (theData->IsParamDefined(theNum, 5))
          {
            theData->ReadReal(theNum, 5, "lower_limit_roll", theArch, aLowerLimitRoll);
          }
          else
          {
            hasLowerLimitRoll = false;
            aLowerLimitRoll   = 0;
          }

          double aUpperLimitRoll;
          bool   hasUpperLimitRoll = true;
          if (theData->IsParamDefined(theNum, 6))
          {
            theData->ReadReal(theNum, 6, "upper_limit_roll", theArch, aUpperLimitRoll);
          }
          else
          {
            hasUpperLimitRoll = false;
            aUpperLimitRoll   = 0;
          }
          occ::handle<StepKinematics_SphericalPairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_SphericalPairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           hasLowerLimitYaw,
                           aLowerLimitYaw,
                           hasUpperLimitYaw,
                           aUpperLimitYaw,
                           hasLowerLimitPitch,
                           aLowerLimitPitch,
                           hasUpperLimitPitch,
                           aUpperLimitPitch,
                           hasLowerLimitRoll,
                           aLowerLimitRoll,
                           hasUpperLimitRoll,
                           aUpperLimitRoll);
        }
        else // spherical_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_SphericalPair;
          occ::handle<StepKinematics_SphericalPair> aLocalPair =
            occ::down_cast<StepKinematics_SphericalPair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           aLowOrderKinematicPair_RZ);
        }
      }
    }
    else if (!strcmp(aPairtype, "SPHERICAL_PAIR_WITH_PIN"))
    {
      // Own fields of spherical_pair_with_pin is non-defined

      if (theData
            ->NamedForComplex("SPHERICAL_PAIR_WITH_PIN_AND_RANGE", "SPWPAR", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 4, theArch, "SPHERICAL_PAIR_WITH_PIN_AND_RANGE"))
        {
          // Own fields of spherical_pair_with_pin_and_range
          aKinematicPair = new StepKinematics_SphericalPairWithPinAndRange;

          double aLowerLimitYaw;
          bool   hasLowerLimitYaw = true;
          if (theData->IsParamDefined(theNum, 1))
          {
            theData->ReadReal(theNum, 1, "lower_limit_yaw", theArch, aLowerLimitYaw);
          }
          else
          {
            hasLowerLimitYaw = false;
            aLowerLimitYaw   = 0;
          }

          double aUpperLimitYaw;
          bool   hasUpperLimitYaw = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum, 2, "upper_limit_yaw", theArch, aUpperLimitYaw);
          }
          else
          {
            hasUpperLimitYaw = false;
            aUpperLimitYaw   = 0;
          }

          double aLowerLimitRoll;
          bool   hasLowerLimitRoll = true;
          if (theData->IsParamDefined(theNum, 3))
          {
            theData->ReadReal(theNum, 3, "lower_limit_roll", theArch, aLowerLimitRoll);
          }
          else
          {
            hasLowerLimitRoll = false;
            aLowerLimitRoll   = 0;
          }

          double aUpperLimitRoll;
          bool   hasUpperLimitRoll = true;
          if (theData->IsParamDefined(theNum, 4))
          {
            theData->ReadReal(theNum, 4, "upper_limit_roll", theArch, aUpperLimitRoll);
          }
          else
          {
            hasUpperLimitRoll = false;
            aUpperLimitRoll   = 0;
          }
          occ::handle<StepKinematics_SphericalPairWithPinAndRange> aLocalPair =
            occ::down_cast<StepKinematics_SphericalPairWithPinAndRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           hasLowerLimitYaw,
                           aLowerLimitYaw,
                           hasUpperLimitYaw,
                           aUpperLimitYaw,
                           hasLowerLimitRoll,
                           aLowerLimitRoll,
                           hasUpperLimitRoll,
                           aUpperLimitRoll);
        }
        else // spherical_pair_with_pin_and_range hasn't params
        {
          aKinematicPair = new StepKinematics_SphericalPairWithPin;
          occ::handle<StepKinematics_SphericalPairWithPin> aLocalPair =
            occ::down_cast<StepKinematics_SphericalPairWithPin>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           aLowOrderKinematicPair_RZ);
        }
      }
      else
        theArch->AddFail("The type of kinematic pair with range is not supported");
    }
    else if (!strcmp(aPairtype, "PRISMATIC_PAIR"))
    {
      // Own fields of prismatic_pair is non-defined

      if (theData->NamedForComplex("PRISMATIC_PAIR_WITH_RANGE", "PPW0", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 2, theArch, "PRISMATIC_PAIR_WITH_RANGE"))
        {
          aKinematicPair = new StepKinematics_PrismaticPairWithRange;
          // Own fields of prismatic_pair_with_range

          double aLowerLimitActualTranslation;
          bool   hasLowerLimitActualTranslation = true;
          if (theData->IsParamDefined(theNum, 1))
          {
            theData->ReadReal(theNum,
                              1,
                              "lower_limit_actual_translation",
                              theArch,
                              aLowerLimitActualTranslation);
          }
          else
          {
            hasLowerLimitActualTranslation = false;
            aLowerLimitActualTranslation   = 0;
          }

          double aUpperLimitActualTranslation;
          bool   hasUpperLimitActualTranslation = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum,
                              2,
                              "upper_limit_actual_translation",
                              theArch,
                              aUpperLimitActualTranslation);
          }
          else
          {
            hasUpperLimitActualTranslation = false;
            aUpperLimitActualTranslation   = 0;
          }
          occ::handle<StepKinematics_PrismaticPairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_PrismaticPairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           hasLowerLimitActualTranslation,
                           aLowerLimitActualTranslation,
                           hasUpperLimitActualTranslation,
                           aUpperLimitActualTranslation);
        }
        else // prismatic_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_PrismaticPair;
          occ::handle<StepKinematics_PrismaticPair> aLocalPair =
            occ::down_cast<StepKinematics_PrismaticPair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           aLowOrderKinematicPair_RZ);
        }
      }
    }
    else if (!strcmp(aPairtype, "UNIVERSAL_PAIR"))
    {
      // Own fields of universal_pair

      aKinematicPair = new StepKinematics_UniversalPair;
      double aInputSkewAngle;
      bool   hasInputSkewAngle = true;
      if (theData->IsParamDefined(theNum, 1))
      {
        theData->ReadReal(theNum, 1, "input_skew_angle", theArch, aInputSkewAngle);
      }
      else
      {
        hasInputSkewAngle = false;
        aInputSkewAngle   = 0;
      }

      if (theData->NamedForComplex("UNIVERSAL_PAIR_WITH_RANGE", "UPWR", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 4, theArch, "UNIVERSAL_PAIR_WITH_RANGE"))
        {
          // Own fields of universal_pair_with_range

          double aLowerLimitFirstRotation;
          bool   hasLowerLimitFirstRotation = true;
          if (theData->IsParamDefined(theNum, 1))
          {
            theData->ReadReal(theNum,
                              1,
                              "lower_limit_first_rotation",
                              theArch,
                              aLowerLimitFirstRotation);
          }
          else
          {
            hasLowerLimitFirstRotation = false;
            aLowerLimitFirstRotation   = 0;
          }

          double aUpperLimitFirstRotation;
          bool   hasUpperLimitFirstRotation = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum,
                              2,
                              "upper_limit_first_rotation",
                              theArch,
                              aUpperLimitFirstRotation);
          }
          else
          {
            hasUpperLimitFirstRotation = false;
            aUpperLimitFirstRotation   = 0;
          }

          double aLowerLimitSecondRotation;
          bool   hasLowerLimitSecondRotation = true;
          if (theData->IsParamDefined(theNum, 3))
          {
            theData->ReadReal(theNum,
                              3,
                              "lower_limit_second_rotation",
                              theArch,
                              aLowerLimitSecondRotation);
          }
          else
          {
            hasLowerLimitSecondRotation = false;
            aLowerLimitSecondRotation   = 0;
          }

          double aUpperLimitSecondRotation;
          bool   hasUpperLimitSecondRotation = true;
          if (theData->IsParamDefined(theNum, 4))
          {
            theData->ReadReal(theNum,
                              4,
                              "upper_limit_second_rotation",
                              theArch,
                              aUpperLimitSecondRotation);
          }
          else
          {
            hasUpperLimitSecondRotation = false;
            aUpperLimitSecondRotation   = 0;
          }
          aKinematicPair = new StepKinematics_UniversalPairWithRange;
          occ::handle<StepKinematics_UniversalPairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_UniversalPairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           hasInputSkewAngle,
                           aInputSkewAngle,
                           hasLowerLimitFirstRotation,
                           aLowerLimitFirstRotation,
                           hasUpperLimitFirstRotation,
                           aUpperLimitFirstRotation,
                           hasLowerLimitSecondRotation,
                           aLowerLimitSecondRotation,
                           hasUpperLimitSecondRotation,
                           aUpperLimitSecondRotation);
        }
        else // universal_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_UniversalPair;
          occ::handle<StepKinematics_UniversalPair> aLocalPair =
            occ::down_cast<StepKinematics_UniversalPair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           hasInputSkewAngle,
                           aInputSkewAngle);
        }
      }
    }
    else if (!strcmp(aPairtype, "PLANAR_PAIR"))
    {
      // Own fields of planar_pair is non-defined

      if (theData->NamedForComplex("PLANAR_PAIR_WITH_RANGE", "PPWR", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 6, theArch, "UNIVERSAL_PAIR_WITH_RANGE"))
        {
          // Own fields of universal_pair_with_range

          aKinematicPair = new StepKinematics_PlanarPairWithRange;
          double aLowerLimitActualRotation;
          bool   hasLowerLimitActualRotation = true;
          if (theData->IsParamDefined(theNum, 1))
          {
            theData->ReadReal(theNum,
                              1,
                              "lower_limit_actual_rotation",
                              theArch,
                              aLowerLimitActualRotation);
          }
          else
          {
            hasLowerLimitActualRotation = false;
            aLowerLimitActualRotation   = 0;
          }

          double aUpperLimitActualRotation;
          bool   hasUpperLimitActualRotation = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum,
                              2,
                              "upper_limit_actual_rotation",
                              theArch,
                              aUpperLimitActualRotation);
          }
          else
          {
            hasUpperLimitActualRotation = false;
            aUpperLimitActualRotation   = 0;
          }

          double aLowerLimitActualTranslationX;
          bool   hasLowerLimitActualTranslationX = true;
          if (theData->IsParamDefined(theNum, 3))
          {
            theData->ReadReal(theNum,
                              3,
                              "lower_limit_actual_translation_x",
                              theArch,
                              aLowerLimitActualTranslationX);
          }
          else
          {
            hasLowerLimitActualTranslationX = false;
            aLowerLimitActualTranslationX   = 0;
          }

          double aUpperLimitActualTranslationX;
          bool   hasUpperLimitActualTranslationX = true;
          if (theData->IsParamDefined(theNum, 4))
          {
            theData->ReadReal(theNum,
                              4,
                              "upper_limit_actual_translation_x",
                              theArch,
                              aUpperLimitActualTranslationX);
          }
          else
          {
            hasUpperLimitActualTranslationX = false;
            aUpperLimitActualTranslationX   = 0;
          }

          double aLowerLimitActualTranslationY;
          bool   hasLowerLimitActualTranslationY = true;
          if (theData->IsParamDefined(theNum, 5))
          {
            theData->ReadReal(theNum,
                              5,
                              "lower_limit_actual_translation_y",
                              theArch,
                              aLowerLimitActualTranslationY);
          }
          else
          {
            hasLowerLimitActualTranslationY = false;
            aLowerLimitActualTranslationY   = 0;
          }

          double aUpperLimitActualTranslationY;
          bool   hasUpperLimitActualTranslationY = true;
          if (theData->IsParamDefined(theNum, 6))
          {
            theData->ReadReal(theNum,
                              6,
                              "upper_limit_actual_translation_y",
                              theArch,
                              aUpperLimitActualTranslationY);
          }
          else
          {
            hasUpperLimitActualTranslationY = false;
            aUpperLimitActualTranslationY   = 0;
          }
          occ::handle<StepKinematics_PlanarPairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_PlanarPairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
        else // universal_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_PlanarPair;
          occ::handle<StepKinematics_PlanarPair> aLocalPair =
            occ::down_cast<StepKinematics_PlanarPair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           aLowOrderKinematicPair_RZ);
        }
      }
    }
  }
  else if (!strcmp(aPairSuperType, "LOW_ORDER_KINEMATIC_PAIR_WITH_MOTION_COUPLING"))
  {
    theNum                = theData->NextForComplex(theNum);
    const char* aPairtype = theData->CType(theNum);
    if (!strcmp(aPairtype, "GEAR_PAIR"))
    {

      // Inherited fields of GearPair

      double aGearPair_RadiusFirstLink;
      theData->ReadReal(theNum,
                        1,
                        "gear_pair.radius_first_link",
                        theArch,
                        aGearPair_RadiusFirstLink);

      double aGearPair_RadiusSecondLink;
      theData->ReadReal(theNum,
                        2,
                        "gear_pair.radius_second_link",
                        theArch,
                        aGearPair_RadiusSecondLink);

      double aGearPair_Bevel;
      theData->ReadReal(theNum, 3, "gear_pair.bevel", theArch, aGearPair_Bevel);

      double aGearPair_HelicalAngle;
      theData->ReadReal(theNum, 4, "gear_pair.helical_angle", theArch, aGearPair_HelicalAngle);

      double aGearPair_GearRatio;
      theData->ReadReal(theNum, 5, "gear_pair.gear_ratio", theArch, aGearPair_GearRatio);

      if (theData->NamedForComplex("GEAR_PAIR_WITH_RANGE", "GPWR", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 2, theArch, "GEAR_PAIR_WITH_RANGE"))
        {
          // Own fields of GearPairWithRange

          aKinematicPair = new StepKinematics_GearPairWithRange;

          double aLowerLimitActualRotation1;
          bool   hasLowerLimitActualRotation1 = true;
          if (theData->IsParamDefined(theNum, 1))
          {
            theData->ReadReal(theNum,
                              1,
                              "lower_limit_actual_rotation1",
                              theArch,
                              aLowerLimitActualRotation1);
          }
          else
          {
            hasLowerLimitActualRotation1 = false;
            aLowerLimitActualRotation1   = 0;
          }

          double aUpperLimitActualRotation1;
          bool   hasUpperLimitActualRotation1 = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum,
                              2,
                              "upper_limit_actual_rotation1",
                              theArch,
                              aUpperLimitActualRotation1);
          }
          else
          {
            hasUpperLimitActualRotation1 = false;
            aUpperLimitActualRotation1   = 0;
          }
          occ::handle<StepKinematics_GearPairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_GearPairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
        else // StepKinematics_GearPairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_GearPair;
          occ::handle<StepKinematics_GearPair> aLocalPair =
            occ::down_cast<StepKinematics_GearPair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
                           aGearPair_GearRatio);
        }
      }
    }
    else if (!strcmp(aPairtype, "RACK_AND_PINION_PAIR"))
    {

      // Inherited fields of RackAndPinionPair
      double aRackAndPinionPair_PinionRadius;
      theData->ReadReal(theNum,
                        1,
                        "rack_and_pinion_pair.pinion_radius",
                        theArch,
                        aRackAndPinionPair_PinionRadius);

      if (theData
            ->NamedForComplex("RACK_AND_PINION_PAIR_WITH_RANGE", "RAPPWR", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 2, theArch, "RACK_AND_PINION_PAIR_WITH_RANGE"))
        {
          // Own fields of GearPairWithRange

          aKinematicPair = new StepKinematics_RackAndPinionPairWithRange;

          double aLowerLimitRackDisplacement;
          bool   hasLowerLimitRackDisplacement = true;
          if (theData->IsParamDefined(theNum, 1))
          {
            theData->ReadReal(theNum,
                              1,
                              "lower_limit_rack_displacement",
                              theArch,
                              aLowerLimitRackDisplacement);
          }
          else
          {
            hasLowerLimitRackDisplacement = false;
            aLowerLimitRackDisplacement   = 0;
          }

          double aUpperLimitRackDisplacement;
          bool   hasUpperLimitRackDisplacement = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum,
                              2,
                              "upper_limit_rack_displacement",
                              theArch,
                              aUpperLimitRackDisplacement);
          }
          else
          {
            hasUpperLimitRackDisplacement = false;
            aUpperLimitRackDisplacement   = 0;
          }

          occ::handle<StepKinematics_RackAndPinionPairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_RackAndPinionPairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
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
        else // StepKinematics_RackAndPinionPairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_RackAndPinionPair;
          occ::handle<StepKinematics_RackAndPinionPair> aLocalPair =
            occ::down_cast<StepKinematics_RackAndPinionPair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aRackAndPinionPair_PinionRadius);
        }
      }
    }
    else if (!strcmp(aPairtype, "SCREW_PAIR"))
    {
      // Inherited fields of ScrewPair

      double aScrewPair_Pitch;
      theData->ReadReal(theNum, 1, "screw_pair.pitch", theArch, aScrewPair_Pitch);

      if (theData->NamedForComplex("SCREW_PAIR_WITH_RANGE", "SPWR", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 2, theArch, "SCREW_PAIR_WITH_RANGE"))
        {
          // Own fields of ScrewPairWithRange

          aKinematicPair = new StepKinematics_ScrewPairWithRange;
          double aLowerLimitActualRotation;
          bool   hasLowerLimitActualRotation = true;
          if (theData->IsParamDefined(theNum, 1))
          {
            theData->ReadReal(theNum,
                              1,
                              "lower_limit_actual_rotation",
                              theArch,
                              aLowerLimitActualRotation);
          }
          else
          {
            hasLowerLimitActualRotation = false;
            aLowerLimitActualRotation   = 0;
          }

          double aUpperLimitActualRotation;
          bool   hasUpperLimitActualRotation = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum,
                              2,
                              "upper_limit_actual_rotation",
                              theArch,
                              aUpperLimitActualRotation);
          }
          else
          {
            hasUpperLimitActualRotation = false;
            aUpperLimitActualRotation   = 0;
          }

          occ::handle<StepKinematics_ScrewPairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_ScrewPairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aScrewPair_Pitch,
                           hasLowerLimitActualRotation,
                           aLowerLimitActualRotation,
                           hasUpperLimitActualRotation,
                           aUpperLimitActualRotation);
        }
        else // StepKinematics_ScrewPairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_ScrewPair;
          occ::handle<StepKinematics_ScrewPair> aLocalPair =
            occ::down_cast<StepKinematics_ScrewPair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aScrewPair_Pitch);
        }
      }
    }
  }
  else // "HIGH_ORDER_KINEMATIC_PAIR"
  {
    theNum                = theData->NextForComplex(theNum);
    const char* aPairtype = theData->CType(theNum);
    if (!strcmp(aPairtype, "POINT_ON_PLANAR_CURVE_PAIR"))
    {

      // Inherited fields of PointOnPlanarCurvePair

      occ::handle<StepGeom_Curve> aPointOnPlanarCurvePair_PairCurve;
      theData->ReadEntity(theNum,
                          1,
                          "point_on_planar_curve_pair.pair_curve",
                          theArch,
                          STANDARD_TYPE(StepGeom_Curve),
                          aPointOnPlanarCurvePair_PairCurve);

      bool aPointOnPlanarCurvePair_Orientation;
      theData->ReadBoolean(theNum,
                           2,
                           "point_on_planar_curve_pair.orientation",
                           theArch,
                           aPointOnPlanarCurvePair_Orientation);

      if (theData->NamedForComplex("POINT_ON_PLANAR_CURVE_PAIR_WITH_RANGE",
                                   "POPCPW",
                                   num0,
                                   theNum,
                                   theArch))
      {
        if (theData->CheckNbParams(theNum, 7, theArch, "POINT_ON_PLANAR_CURVE_PAIR_WITH_RANGE"))
        {
          // Own fields of PointOnPlanarCurvePairWithRange

          aKinematicPair = new StepKinematics_PointOnPlanarCurvePairWithRange;

          occ::handle<StepGeom_TrimmedCurve> aRangeOnPairCurve;
          theData->ReadEntity(theNum,
                              1,
                              "range_on_pair_curve",
                              theArch,
                              STANDARD_TYPE(StepGeom_TrimmedCurve),
                              aRangeOnPairCurve);

          double aLowerLimitYaw;
          bool   hasLowerLimitYaw = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum, 2, "lower_limit_yaw", theArch, aLowerLimitYaw);
          }
          else
          {
            hasLowerLimitYaw = false;
            aLowerLimitYaw   = 0;
          }

          double aUpperLimitYaw;
          bool   hasUpperLimitYaw = true;
          if (theData->IsParamDefined(theNum, 3))
          {
            theData->ReadReal(theNum, 3, "upper_limit_yaw", theArch, aUpperLimitYaw);
          }
          else
          {
            hasUpperLimitYaw = false;
            aUpperLimitYaw   = 0;
          }

          double aLowerLimitPitch;
          bool   hasLowerLimitPitch = true;
          if (theData->IsParamDefined(theNum, 4))
          {
            theData->ReadReal(theNum, 4, "lower_limit_pitch", theArch, aLowerLimitPitch);
          }
          else
          {
            hasLowerLimitPitch = false;
            aLowerLimitPitch   = 0;
          }

          double aUpperLimitPitch;
          bool   hasUpperLimitPitch = true;
          if (theData->IsParamDefined(theNum, 5))
          {
            theData->ReadReal(theNum, 5, "upper_limit_pitch", theArch, aUpperLimitPitch);
          }
          else
          {
            hasUpperLimitPitch = false;
            aUpperLimitPitch   = 0;
          }

          double aLowerLimitRoll;
          bool   hasLowerLimitRoll = true;
          if (theData->IsParamDefined(theNum, 6))
          {
            theData->ReadReal(theNum, 6, "lower_limit_roll", theArch, aLowerLimitRoll);
          }
          else
          {
            hasLowerLimitRoll = false;
            aLowerLimitRoll   = 0;
          }

          double aUpperLimitRoll;
          bool   hasUpperLimitRoll = true;
          if (theData->IsParamDefined(theNum, 7))
          {
            theData->ReadReal(theNum, 7, "upper_limit_roll", theArch, aUpperLimitRoll);
          }
          else
          {
            hasUpperLimitRoll = false;
            aUpperLimitRoll   = 0;
          }
          occ::handle<StepKinematics_PointOnPlanarCurvePairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_PointOnPlanarCurvePairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aPointOnPlanarCurvePair_PairCurve,
                           aPointOnPlanarCurvePair_Orientation,
                           aRangeOnPairCurve,
                           hasLowerLimitYaw,
                           aLowerLimitYaw,
                           hasUpperLimitYaw,
                           aUpperLimitYaw,
                           hasLowerLimitPitch,
                           aLowerLimitPitch,
                           hasUpperLimitPitch,
                           aUpperLimitPitch,
                           hasLowerLimitRoll,
                           aLowerLimitRoll,
                           hasUpperLimitRoll,
                           aUpperLimitRoll);
        }
        else // point_on_planar_curve_pair_with_range hasn't params
        {
          aKinematicPair = new StepKinematics_PointOnPlanarCurvePair;
          occ::handle<StepKinematics_PointOnPlanarCurvePair> aLocalPair =
            occ::down_cast<StepKinematics_PointOnPlanarCurvePair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aPointOnPlanarCurvePair_PairCurve,
                           aPointOnPlanarCurvePair_Orientation);
        }
      }
    }
    else if (!strcmp(aPairtype, "POINT_ON_SURFACE_PAIR"))
    {
      // Inherited fields of PointOnSurfacePair

      occ::handle<StepGeom_Surface> aPointOnSurfacePair_PairSurface;
      theData->ReadEntity(theNum,
                          1,
                          "point_on_surface_pair.pair_surface",
                          theArch,
                          STANDARD_TYPE(StepGeom_Surface),
                          aPointOnSurfacePair_PairSurface);

      if (theData
            ->NamedForComplex("POINT_ON_SURFACE_PAIR_WITH_RANGE ", "RAPPWR", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 2, theArch, "POINT_ON_SURFACE_PAIR_WITH_RANGE "))
        {
          // Own fields of PointOnSurfacePairWithRange

          aKinematicPair = new StepKinematics_PointOnSurfacePairWithRange;

          occ::handle<StepGeom_RectangularTrimmedSurface> aRangeOnPairSurface;
          theData->ReadEntity(theNum,
                              1,
                              "range_on_pair_surface",
                              theArch,
                              STANDARD_TYPE(StepGeom_RectangularTrimmedSurface),
                              aRangeOnPairSurface);

          double aLowerLimitYaw;
          bool   hasLowerLimitYaw = true;
          if (theData->IsParamDefined(theNum, 2))
          {
            theData->ReadReal(theNum, 2, "lower_limit_yaw", theArch, aLowerLimitYaw);
          }
          else
          {
            hasLowerLimitYaw = false;
            aLowerLimitYaw   = 0;
          }

          double aUpperLimitYaw;
          bool   hasUpperLimitYaw = true;
          if (theData->IsParamDefined(theNum, 3))
          {
            theData->ReadReal(theNum, 3, "upper_limit_yaw", theArch, aUpperLimitYaw);
          }
          else
          {
            hasUpperLimitYaw = false;
            aUpperLimitYaw   = 0;
          }

          double aLowerLimitPitch;
          bool   hasLowerLimitPitch = true;
          if (theData->IsParamDefined(theNum, 4))
          {
            theData->ReadReal(theNum, 4, "lower_limit_pitch", theArch, aLowerLimitPitch);
          }
          else
          {
            hasLowerLimitPitch = false;
            aLowerLimitPitch   = 0;
          }

          double aUpperLimitPitch;
          bool   hasUpperLimitPitch = true;
          if (theData->IsParamDefined(theNum, 5))
          {
            theData->ReadReal(theNum, 5, "upper_limit_pitch", theArch, aUpperLimitPitch);
          }
          else
          {
            hasUpperLimitPitch = false;
            aUpperLimitPitch   = 0;
          }

          double aLowerLimitRoll;
          bool   hasLowerLimitRoll = true;
          if (theData->IsParamDefined(theNum, 6))
          {
            theData->ReadReal(theNum, 6, "lower_limit_roll", theArch, aLowerLimitRoll);
          }
          else
          {
            hasLowerLimitRoll = false;
            aLowerLimitRoll   = 0;
          }

          double aUpperLimitRoll;
          bool   hasUpperLimitRoll = true;
          if (theData->IsParamDefined(theNum, 7))
          {
            theData->ReadReal(theNum, 7, "upper_limit_roll", theArch, aUpperLimitRoll);
          }
          else
          {
            hasUpperLimitRoll = false;
            aUpperLimitRoll   = 0;
          }
          occ::handle<StepKinematics_PointOnSurfacePairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_PointOnSurfacePairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aPointOnSurfacePair_PairSurface,
                           aRangeOnPairSurface,
                           hasLowerLimitYaw,
                           aLowerLimitYaw,
                           hasUpperLimitYaw,
                           aUpperLimitYaw,
                           hasLowerLimitPitch,
                           aLowerLimitPitch,
                           hasUpperLimitPitch,
                           aUpperLimitPitch,
                           hasLowerLimitRoll,
                           aLowerLimitRoll,
                           hasUpperLimitRoll,
                           aUpperLimitRoll);
        }
        else // StepKinematics_PointOnSurfacePairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_PointOnSurfacePair;
          occ::handle<StepKinematics_PointOnSurfacePair> aLocalPair =
            occ::down_cast<StepKinematics_PointOnSurfacePair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aPointOnSurfacePair_PairSurface);
        }
      }
    }
    else if (!strcmp(aPairtype, "PLANAR_CURVE_PAIR"))
    {
      // Inherited fields of PlanarCurvePair

      occ::handle<StepGeom_Curve> aPlanarCurvePair_Curve1;
      theData->ReadEntity(theNum,
                          1,
                          "planar_curve_pair.curve1",
                          theArch,
                          STANDARD_TYPE(StepGeom_Curve),
                          aPlanarCurvePair_Curve1);

      occ::handle<StepGeom_Curve> aPlanarCurvePair_Curve2;
      theData->ReadEntity(theNum,
                          2,
                          "planar_curve_pair.curve2",
                          theArch,
                          STANDARD_TYPE(StepGeom_Curve),
                          aPlanarCurvePair_Curve2);

      bool aPlanarCurvePair_Orientation;
      theData->ReadBoolean(theNum,
                           3,
                           "planar_curve_pair.orientation",
                           theArch,
                           aPlanarCurvePair_Orientation);

      if (theData->NamedForComplex("PLANAR_CURVE_PAIR_RANGE", "PCPR", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 2, theArch, "PLANAR_CURVE_PAIR_RANGE"))
        {
          // Own fields of PlanarCurvePairRange
          aKinematicPair = new StepKinematics_PlanarCurvePairRange;

          occ::handle<StepGeom_TrimmedCurve> aRangeOnCurve1;
          theData->ReadEntity(theNum,
                              1,
                              "range_on_curve1",
                              theArch,
                              STANDARD_TYPE(StepGeom_TrimmedCurve),
                              aRangeOnCurve1);

          occ::handle<StepGeom_TrimmedCurve> aRangeOnCurve2;
          theData->ReadEntity(theNum,
                              2,
                              "range_on_curve2",
                              theArch,
                              STANDARD_TYPE(StepGeom_TrimmedCurve),
                              aRangeOnCurve2);

          occ::handle<StepKinematics_PlanarCurvePairRange> aLocalPair =
            occ::down_cast<StepKinematics_PlanarCurvePairRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aPlanarCurvePair_Curve1,
                           aPlanarCurvePair_Curve2,
                           aPlanarCurvePair_Orientation,
                           aRangeOnCurve1,
                           aRangeOnCurve2);
        }
        else // StepKinematics_ScrewPairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_PlanarCurvePair;
          occ::handle<StepKinematics_PlanarCurvePair> aLocalPair =
            occ::down_cast<StepKinematics_PlanarCurvePair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aPlanarCurvePair_Curve1,
                           aPlanarCurvePair_Curve2,
                           aPlanarCurvePair_Orientation);
        }
      }
    }
    else if (!strcmp(aPairtype, "SURFACE_PAIR"))
    {
      // Inherited fields of SurfacePair

      occ::handle<StepGeom_Surface> aSurfacePair_Surface1;
      theData->ReadEntity(theNum,
                          1,
                          "surface_pair.surface1",
                          theArch,
                          STANDARD_TYPE(StepGeom_Surface),
                          aSurfacePair_Surface1);

      occ::handle<StepGeom_Surface> aSurfacePair_Surface2;
      theData->ReadEntity(theNum,
                          2,
                          "surface_pair.surface2",
                          theArch,
                          STANDARD_TYPE(StepGeom_Surface),
                          aSurfacePair_Surface2);

      bool aSurfacePair_Orientation;
      theData->ReadBoolean(theNum,
                           3,
                           "surface_pair.orientation",
                           theArch,
                           aSurfacePair_Orientation);

      if (theData->NamedForComplex("SURFACE_PAIR_WITH_RANGE", "SPW1", num0, theNum, theArch))
      {
        if (theData->CheckNbParams(theNum, 4, theArch, "SURFACE_PAIR_WITH_RANGE"))
        {
          aKinematicPair = new StepKinematics_SurfacePairWithRange;
          // Own fields of SurfacePairWithRange

          occ::handle<StepGeom_RectangularTrimmedSurface> aRangeOnSurface1;
          theData->ReadEntity(theNum,
                              1,
                              "range_on_surface1",
                              theArch,
                              STANDARD_TYPE(StepGeom_RectangularTrimmedSurface),
                              aRangeOnSurface1);

          occ::handle<StepGeom_RectangularTrimmedSurface> aRangeOnSurface2;
          theData->ReadEntity(theNum,
                              2,
                              "range_on_surface2",
                              theArch,
                              STANDARD_TYPE(StepGeom_RectangularTrimmedSurface),
                              aRangeOnSurface2);

          double aLowerLimitActualRotation;
          bool   hasLowerLimitActualRotation = true;
          if (theData->IsParamDefined(theNum, 3))
          {
            theData->ReadReal(theNum,
                              3,
                              "lower_limit_actual_rotation",
                              theArch,
                              aLowerLimitActualRotation);
          }
          else
          {
            hasLowerLimitActualRotation = false;
            aLowerLimitActualRotation   = 0;
          }

          double aUpperLimitActualRotation;
          bool   hasUpperLimitActualRotation = true;
          if (theData->IsParamDefined(theNum, 4))
          {
            theData->ReadReal(theNum,
                              4,
                              "upper_limit_actual_rotation",
                              theArch,
                              aUpperLimitActualRotation);
          }
          else
          {
            hasUpperLimitActualRotation = false;
            aUpperLimitActualRotation   = 0;
          }

          occ::handle<StepKinematics_SurfacePairWithRange> aLocalPair =
            occ::down_cast<StepKinematics_SurfacePairWithRange>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aSurfacePair_Surface1,
                           aSurfacePair_Surface2,
                           aSurfacePair_Orientation,
                           aRangeOnSurface1,
                           aRangeOnSurface2,
                           hasLowerLimitActualRotation,
                           aLowerLimitActualRotation,
                           hasUpperLimitActualRotation,
                           aUpperLimitActualRotation);
        }
        else // StepKinematics_SurfacePairWithRange hasn't params
        {
          aKinematicPair = new StepKinematics_SurfacePair;
          occ::handle<StepKinematics_SurfacePair> aLocalPair =
            occ::down_cast<StepKinematics_SurfacePair>(aKinematicPair);
          // Initialize kinematic_pair
          aLocalPair->Init(aRepresentationItem_Name,
                           aItemDefinedTransformation_Name,
                           hasItemDefinedTransformation_Description,
                           aItemDefinedTransformation_Description,
                           aItemDefinedTransformation_TransformItem1,
                           aItemDefinedTransformation_TransformItem2,
                           aKinematicPair_Joint,
                           aSurfacePair_Surface1,
                           aSurfacePair_Surface2,
                           aSurfacePair_Orientation);
        }
      }
    }
  }
  theData->NamedForComplex("REPRESENTATION_ITEM", "RPRITM", num0, theNum, theArch);

  // Own fields of representation_item
  theData->ReadString(theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);
  anActuatedKinematicPair->Init(aRepresentationItem_Name,
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

  // Initialize kinematic_pair
  aKinematicPair->SetName(aRepresentationItem_Name);
  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
               aItemDefinedTransformation_Name,
               hasItemDefinedTransformation_Description,
               aItemDefinedTransformation_Description,
               aItemDefinedTransformation_TransformItem1,
               aItemDefinedTransformation_TransformItem2,
               aKinematicPair_Joint,
               anActuatedKinematicPair,
               aKinematicPair);
}

//=================================================================================================

void RWStepKinematics_RWActuatedKinPairAndOrderKinPair::WriteStep(
  StepData_StepWriter&                                              theSW,
  const occ::handle<StepKinematics_ActuatedKinPairAndOrderKinPair>& theEnt) const
{
  // --- Instance of plex component BoundedCurve ---

  theSW.StartEntity("(ACTUATED_KINEMATIC_PAIR");

  if (theEnt->GetActuatedKinematicPair()->HasTX())
  {
    switch (theEnt->GetActuatedKinematicPair()->TX())
    {
      case StepKinematics_adBidirectional:
        theSW.SendEnum(".BIDIRECTIONAL.");
        break;
      case StepKinematics_adPositiveOnly:
        theSW.SendEnum(".POSITIVE_ONLY.");
        break;
      case StepKinematics_adNegativeOnly:
        theSW.SendEnum(".NEGATIVE_ONLY.");
        break;
      case StepKinematics_adNotActuated:
        theSW.SendEnum(".NOT_ACTUATED.");
        break;
    }
  }
  else
    theSW.SendUndef();

  if (theEnt->GetActuatedKinematicPair()->HasTY())
  {
    switch (theEnt->GetActuatedKinematicPair()->TY())
    {
      case StepKinematics_adBidirectional:
        theSW.SendEnum(".BIDIRECTIONAL.");
        break;
      case StepKinematics_adPositiveOnly:
        theSW.SendEnum(".POSITIVE_ONLY.");
        break;
      case StepKinematics_adNegativeOnly:
        theSW.SendEnum(".NEGATIVE_ONLY.");
        break;
      case StepKinematics_adNotActuated:
        theSW.SendEnum(".NOT_ACTUATED.");
        break;
    }
  }
  else
    theSW.SendUndef();

  if (theEnt->GetActuatedKinematicPair()->HasTZ())
  {
    switch (theEnt->GetActuatedKinematicPair()->TZ())
    {
      case StepKinematics_adBidirectional:
        theSW.SendEnum(".BIDIRECTIONAL.");
        break;
      case StepKinematics_adPositiveOnly:
        theSW.SendEnum(".POSITIVE_ONLY.");
        break;
      case StepKinematics_adNegativeOnly:
        theSW.SendEnum(".NEGATIVE_ONLY.");
        break;
      case StepKinematics_adNotActuated:
        theSW.SendEnum(".NOT_ACTUATED.");
        break;
    }
  }
  else
    theSW.SendUndef();

  if (theEnt->GetActuatedKinematicPair()->HasRX())
  {
    switch (theEnt->GetActuatedKinematicPair()->RX())
    {
      case StepKinematics_adBidirectional:
        theSW.SendEnum(".BIDIRECTIONAL.");
        break;
      case StepKinematics_adPositiveOnly:
        theSW.SendEnum(".POSITIVE_ONLY.");
        break;
      case StepKinematics_adNegativeOnly:
        theSW.SendEnum(".NEGATIVE_ONLY.");
        break;
      case StepKinematics_adNotActuated:
        theSW.SendEnum(".NOT_ACTUATED.");
        break;
    }
  }
  else
    theSW.SendUndef();

  if (theEnt->GetActuatedKinematicPair()->HasRY())
  {
    switch (theEnt->GetActuatedKinematicPair()->RY())
    {
      case StepKinematics_adBidirectional:
        theSW.SendEnum(".BIDIRECTIONAL.");
        break;
      case StepKinematics_adPositiveOnly:
        theSW.SendEnum(".POSITIVE_ONLY.");
        break;
      case StepKinematics_adNegativeOnly:
        theSW.SendEnum(".NEGATIVE_ONLY.");
        break;
      case StepKinematics_adNotActuated:
        theSW.SendEnum(".NOT_ACTUATED.");
        break;
    }
  }
  else
    theSW.SendUndef();

  if (theEnt->GetActuatedKinematicPair()->HasRZ())
  {
    switch (theEnt->GetActuatedKinematicPair()->RZ())
    {
      case StepKinematics_adBidirectional:
        theSW.SendEnum(".BIDIRECTIONAL.");
        break;
      case StepKinematics_adPositiveOnly:
        theSW.SendEnum(".POSITIVE_ONLY.");
        break;
      case StepKinematics_adNegativeOnly:
        theSW.SendEnum(".NEGATIVE_ONLY.");
        break;
      case StepKinematics_adNotActuated:
        theSW.SendEnum(".NOT_ACTUATED.");
        break;
    }
  }
  else
    theSW.SendUndef();
  theSW.StartEntity("(GEOMETRIC_REPRESENTATION_ITEM");

  theSW.StartEntity("(ITEM_DEFINED_TRANSFORMATION");
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

  theSW.StartEntity("(KINEMATIC_PAIR");

  // Own fields of KinematicPair

  theSW.Send(theEnt->Joint());
  if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_LowOrderKinematicPair)))
  {
    occ::handle<StepKinematics_LowOrderKinematicPair> aLowOrderKinematicPair =
      occ::down_cast<StepKinematics_LowOrderKinematicPair>(theEnt->GetOrderKinematicPair());
    theSW.StartEntity("(LOW_ORDER_KINEMATIC_PAIR");

    theSW.SendBoolean(aLowOrderKinematicPair->TX());

    theSW.SendBoolean(aLowOrderKinematicPair->TY());

    theSW.SendBoolean(aLowOrderKinematicPair->TZ());

    theSW.SendBoolean(aLowOrderKinematicPair->RX());

    theSW.SendBoolean(aLowOrderKinematicPair->RY());

    theSW.SendBoolean(aLowOrderKinematicPair->RZ());

    if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_RevolutePair)))
    {
      theSW.StartEntity("(REVOLUTE_PAIR");
      theSW.StartEntity("(REVOLUTE_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_RevolutePairWithRange)))
      {
        occ::handle<StepKinematics_RevolutePairWithRange> aRevolutePairWithRange =
          occ::down_cast<StepKinematics_RevolutePairWithRange>(theEnt->GetOrderKinematicPair());
        // Own fields of RevolutePairWithRange

        if (aRevolutePairWithRange->HasLowerLimitActualRotation())
        {
          theSW.Send(aRevolutePairWithRange->LowerLimitActualRotation());
        }
        else
          theSW.SendUndef();

        if (aRevolutePairWithRange->HasUpperLimitActualRotation())
        {
          theSW.Send(aRevolutePairWithRange->UpperLimitActualRotation());
        }
        else
          theSW.SendUndef();
      }
    }
    else if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_CylindricalPair)))
    {
      theSW.StartEntity("(CYLINDRICAL_PAIR");
      theSW.StartEntity("(CYLINDRICAL_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_CylindricalPairWithRange)))
      {
        occ::handle<StepKinematics_CylindricalPairWithRange> aCylindricalPairWithRange =
          occ::down_cast<StepKinematics_CylindricalPairWithRange>(theEnt->GetOrderKinematicPair());
        // Own fields of CylindricalPairWithRange

        if (aCylindricalPairWithRange->HasLowerLimitActualTranslation())
        {
          theSW.Send(aCylindricalPairWithRange->LowerLimitActualTranslation());
        }
        else
          theSW.SendUndef();

        if (aCylindricalPairWithRange->HasUpperLimitActualTranslation())
        {
          theSW.Send(aCylindricalPairWithRange->UpperLimitActualTranslation());
        }
        else
          theSW.SendUndef();

        if (aCylindricalPairWithRange->HasLowerLimitActualRotation())
        {
          theSW.Send(aCylindricalPairWithRange->LowerLimitActualRotation());
        }
        else
          theSW.SendUndef();

        if (aCylindricalPairWithRange->HasUpperLimitActualRotation())
        {
          theSW.Send(aCylindricalPairWithRange->UpperLimitActualRotation());
        }
        else
          theSW.SendUndef();
      }
    }
    else if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_SphericalPair)))
    {
      theSW.StartEntity("(SPHERICAL_PAIR");
      theSW.StartEntity("(SPHERICAL_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_SphericalPairWithRange)))
      {
        occ::handle<StepKinematics_SphericalPairWithRange> aSphericalPairWithRange =
          occ::down_cast<StepKinematics_SphericalPairWithRange>(theEnt->GetOrderKinematicPair());
        // Own fields of SphericalPairWithRange

        if (aSphericalPairWithRange->HasLowerLimitYaw())
        {
          theSW.Send(aSphericalPairWithRange->LowerLimitYaw());
        }
        else
          theSW.SendUndef();

        if (aSphericalPairWithRange->HasUpperLimitYaw())
        {
          theSW.Send(aSphericalPairWithRange->UpperLimitYaw());
        }
        else
          theSW.SendUndef();

        if (aSphericalPairWithRange->HasLowerLimitPitch())
        {
          theSW.Send(aSphericalPairWithRange->LowerLimitPitch());
        }
        else
          theSW.SendUndef();

        if (aSphericalPairWithRange->HasUpperLimitPitch())
        {
          theSW.Send(aSphericalPairWithRange->UpperLimitPitch());
        }
        else
          theSW.SendUndef();

        if (aSphericalPairWithRange->HasLowerLimitRoll())
        {
          theSW.Send(aSphericalPairWithRange->LowerLimitRoll());
        }
        else
          theSW.SendUndef();

        if (aSphericalPairWithRange->HasUpperLimitRoll())
        {
          theSW.Send(aSphericalPairWithRange->UpperLimitRoll());
        }
        else
          theSW.SendUndef();
      }
    }
    else if (theEnt->GetOrderKinematicPair()->IsKind(
               STANDARD_TYPE(StepKinematics_SphericalPairWithPin)))
    {
      theSW.StartEntity("(SPHERICAL_PAIR_WITH_PIN");
      theSW.StartEntity("(SPHERICAL_PAIR_WITH_PIN_AND_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_SphericalPairWithPinAndRange)))
      {
        occ::handle<StepKinematics_SphericalPairWithPinAndRange> aSphericalPairWithPinAndRange =
          occ::down_cast<StepKinematics_SphericalPairWithPinAndRange>(
            theEnt->GetOrderKinematicPair());
        // Own fields of SphericalPairWithPinAndRange

        if (aSphericalPairWithPinAndRange->HasLowerLimitYaw())
        {
          theSW.Send(aSphericalPairWithPinAndRange->LowerLimitYaw());
        }
        else
          theSW.SendUndef();

        if (aSphericalPairWithPinAndRange->HasUpperLimitYaw())
        {
          theSW.Send(aSphericalPairWithPinAndRange->UpperLimitYaw());
        }
        else
          theSW.SendUndef();

        if (aSphericalPairWithPinAndRange->HasLowerLimitRoll())
        {
          theSW.Send(aSphericalPairWithPinAndRange->LowerLimitRoll());
        }
        else
          theSW.SendUndef();

        if (aSphericalPairWithPinAndRange->HasUpperLimitRoll())
        {
          theSW.Send(aSphericalPairWithPinAndRange->UpperLimitRoll());
        }
        else
          theSW.SendUndef();
      }
    }
    else if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_PrismaticPair)))
    {
      theSW.StartEntity("(PRISMATIC_PAIR");
      theSW.StartEntity("(PRISMATIC_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_PrismaticPairWithRange)))
      {
        occ::handle<StepKinematics_PrismaticPairWithRange> aPrismaticPairWithRange =
          occ::down_cast<StepKinematics_PrismaticPairWithRange>(theEnt->GetOrderKinematicPair());
        // Own fields of PrismaticPairWithRange

        if (aPrismaticPairWithRange->HasLowerLimitActualTranslation())
        {
          theSW.Send(aPrismaticPairWithRange->LowerLimitActualTranslation());
        }
        else
          theSW.SendUndef();

        if (aPrismaticPairWithRange->HasUpperLimitActualTranslation())
        {
          theSW.Send(aPrismaticPairWithRange->UpperLimitActualTranslation());
        }
        else
          theSW.SendUndef();
      }
    }
    else if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_UniversalPair)))
    {
      theSW.StartEntity("(UNIVERSAL_PAIR");
      occ::handle<StepKinematics_UniversalPair> anUniversalPair =
        occ::down_cast<StepKinematics_UniversalPair>(theEnt->GetOrderKinematicPair());
      // Own fields of UniversalPair

      if (anUniversalPair->HasInputSkewAngle())
      {
        theSW.Send(anUniversalPair->InputSkewAngle());
      }
      else
        theSW.SendUndef();

      theSW.StartEntity("(PRISMATIC_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_PrismaticPairWithRange)))
      {
        occ::handle<StepKinematics_UniversalPairWithRange> anUniversalPairWithRange =
          occ::down_cast<StepKinematics_UniversalPairWithRange>(theEnt->GetOrderKinematicPair());
        // Own fields of UniversalPairWithRange

        if (anUniversalPairWithRange->HasLowerLimitFirstRotation())
        {
          theSW.Send(anUniversalPairWithRange->LowerLimitFirstRotation());
        }
        else
          theSW.SendUndef();

        if (anUniversalPairWithRange->HasUpperLimitFirstRotation())
        {
          theSW.Send(anUniversalPairWithRange->UpperLimitFirstRotation());
        }
        else
          theSW.SendUndef();

        if (anUniversalPairWithRange->HasLowerLimitSecondRotation())
        {
          theSW.Send(anUniversalPairWithRange->LowerLimitSecondRotation());
        }
        else
          theSW.SendUndef();

        if (anUniversalPairWithRange->HasUpperLimitSecondRotation())
        {
          theSW.Send(anUniversalPairWithRange->UpperLimitSecondRotation());
        }
        else
          theSW.SendUndef();
      }
    }
  }
  else if (theEnt->GetOrderKinematicPair()->IsKind(
             STANDARD_TYPE(StepKinematics_LowOrderKinematicPairWithMotionCoupling)))
  {
    if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_GearPair)))
    {
      theSW.StartEntity("(GEAR_PAIR");
      occ::handle<StepKinematics_GearPair> aGearPair =
        occ::down_cast<StepKinematics_GearPair>(theEnt->GetOrderKinematicPair());
      // Own fields of GearPair

      theSW.Send(aGearPair->RadiusFirstLink());

      theSW.Send(aGearPair->RadiusSecondLink());

      theSW.Send(aGearPair->Bevel());

      theSW.Send(aGearPair->HelicalAngle());

      theSW.Send(aGearPair->GearRatio());

      theSW.StartEntity("(PRISMATIC_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_GearPairWithRange)))
      {
        occ::handle<StepKinematics_GearPairWithRange> aGearPairWithRange =
          occ::down_cast<StepKinematics_GearPairWithRange>(theEnt->GetOrderKinematicPair());
        // Own fields of GearPairWithRange

        if (aGearPairWithRange->HasLowerLimitActualRotation1())
        {
          theSW.Send(aGearPairWithRange->LowerLimitActualRotation1());
        }
        else
          theSW.SendUndef();

        if (aGearPairWithRange->HasUpperLimitActualRotation1())
        {
          theSW.Send(aGearPairWithRange->UpperLimitActualRotation1());
        }
        else
          theSW.SendUndef();
      }
    }
    else if (theEnt->GetOrderKinematicPair()->IsKind(
               STANDARD_TYPE(StepKinematics_RackAndPinionPair)))
    {
      theSW.StartEntity("RACK_AND_PINION_PAIR");
      occ::handle<StepKinematics_RackAndPinionPair> aRackAndPinionPair =
        occ::down_cast<StepKinematics_RackAndPinionPair>(theEnt->GetOrderKinematicPair());
      // Own fields of RackAndPinionPair

      theSW.Send(aRackAndPinionPair->PinionRadius());

      theSW.StartEntity("(RACK_AND_PINION_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_RackAndPinionPairWithRange)))
      {
        occ::handle<StepKinematics_RackAndPinionPairWithRange> aRackAndPinionPairWithRange =
          occ::down_cast<StepKinematics_RackAndPinionPairWithRange>(
            theEnt->GetOrderKinematicPair());
        // Own fields of aRackAndPinionPairWithRange

        if (aRackAndPinionPairWithRange->HasLowerLimitRackDisplacement())
        {
          theSW.Send(aRackAndPinionPairWithRange->LowerLimitRackDisplacement());
        }
        else
          theSW.SendUndef();

        if (aRackAndPinionPairWithRange->HasUpperLimitRackDisplacement())
        {
          theSW.Send(aRackAndPinionPairWithRange->UpperLimitRackDisplacement());
        }
        else
          theSW.SendUndef();
      }
    }
    else if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_ScrewPair)))
    {
      theSW.StartEntity("SCREW_PAIR");
      occ::handle<StepKinematics_ScrewPair> aScrewPair =
        occ::down_cast<StepKinematics_ScrewPair>(theEnt->GetOrderKinematicPair());
      // Own fields of ScrewPair

      theSW.Send(aScrewPair->Pitch());

      theSW.StartEntity("(SCREW_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_ScrewPairWithRange)))
      {
        occ::handle<StepKinematics_ScrewPairWithRange> aScrewPairWithRange =
          occ::down_cast<StepKinematics_ScrewPairWithRange>(theEnt->GetOrderKinematicPair());
        // Own fields of aRackAndPinionPairWithRange

        if (aScrewPairWithRange->HasLowerLimitActualRotation())
        {
          theSW.Send(aScrewPairWithRange->LowerLimitActualRotation());
        }
        else
          theSW.SendUndef();

        if (aScrewPairWithRange->HasUpperLimitActualRotation())
        {
          theSW.Send(aScrewPairWithRange->UpperLimitActualRotation());
        }
        else
          theSW.SendUndef();
      }
    }
  }
  else if (theEnt->GetOrderKinematicPair()->IsKind(
             STANDARD_TYPE(StepKinematics_HighOrderKinematicPair)))
  {
    if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_ScrewPair)))
    {
      theSW.StartEntity("POINT_ON_PLANAR_CURVE_PAIR");
      occ::handle<StepKinematics_PointOnPlanarCurvePair> aPointOnPlanarCurvePair =
        occ::down_cast<StepKinematics_PointOnPlanarCurvePair>(theEnt->GetOrderKinematicPair());
      // Own fields of PointOnPlanarCurvePair

      theSW.Send(aPointOnPlanarCurvePair->PairCurve());

      theSW.SendBoolean(aPointOnPlanarCurvePair->Orientation());

      theSW.StartEntity("(POINT_ON_PLANAR_CURVE_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_PointOnPlanarCurvePairWithRange)))
      {
        occ::handle<StepKinematics_PointOnPlanarCurvePairWithRange>
          aPointOnPlanarCurvePairWithRange =
            occ::down_cast<StepKinematics_PointOnPlanarCurvePairWithRange>(
              theEnt->GetOrderKinematicPair());
        // Own fields of PointOnPlanarCurvePairWithRange

        theSW.Send(aPointOnPlanarCurvePairWithRange->RangeOnPairCurve());

        if (aPointOnPlanarCurvePairWithRange->HasLowerLimitYaw())
        {
          theSW.Send(aPointOnPlanarCurvePairWithRange->LowerLimitYaw());
        }
        else
          theSW.SendUndef();

        if (aPointOnPlanarCurvePairWithRange->HasUpperLimitYaw())
        {
          theSW.Send(aPointOnPlanarCurvePairWithRange->UpperLimitYaw());
        }
        else
          theSW.SendUndef();

        if (aPointOnPlanarCurvePairWithRange->HasLowerLimitPitch())
        {
          theSW.Send(aPointOnPlanarCurvePairWithRange->LowerLimitPitch());
        }
        else
          theSW.SendUndef();

        if (aPointOnPlanarCurvePairWithRange->HasUpperLimitPitch())
        {
          theSW.Send(aPointOnPlanarCurvePairWithRange->UpperLimitPitch());
        }
        else
          theSW.SendUndef();

        if (aPointOnPlanarCurvePairWithRange->HasLowerLimitRoll())
        {
          theSW.Send(aPointOnPlanarCurvePairWithRange->LowerLimitRoll());
        }
        else
          theSW.SendUndef();

        if (aPointOnPlanarCurvePairWithRange->HasUpperLimitRoll())
        {
          theSW.Send(aPointOnPlanarCurvePairWithRange->UpperLimitRoll());
        }
        else
          theSW.SendUndef();
      }
    }
    else if (theEnt->GetOrderKinematicPair()->IsKind(
               STANDARD_TYPE(StepKinematics_PointOnSurfacePair)))
    {
      theSW.StartEntity("POINT_ON_SURFACE_PAIR");
      occ::handle<StepKinematics_PointOnSurfacePair> aPointOnSurfacePair =
        occ::down_cast<StepKinematics_PointOnSurfacePair>(theEnt->GetOrderKinematicPair());
      // Own fields of PointOnSurfacePair

      theSW.Send(aPointOnSurfacePair->PairSurface());

      theSW.StartEntity("(SCREW_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_PointOnSurfacePairWithRange)))
      {
        occ::handle<StepKinematics_PointOnSurfacePairWithRange> aPointOnSurfacePairWithRange =
          occ::down_cast<StepKinematics_PointOnSurfacePairWithRange>(
            theEnt->GetOrderKinematicPair());
        // Own fields of PointOnSurfacePairWithRange

        theSW.Send(aPointOnSurfacePairWithRange->RangeOnPairSurface());

        if (aPointOnSurfacePairWithRange->HasLowerLimitYaw())
        {
          theSW.Send(aPointOnSurfacePairWithRange->LowerLimitYaw());
        }
        else
          theSW.SendUndef();

        if (aPointOnSurfacePairWithRange->HasUpperLimitYaw())
        {
          theSW.Send(aPointOnSurfacePairWithRange->UpperLimitYaw());
        }
        else
          theSW.SendUndef();

        if (aPointOnSurfacePairWithRange->HasLowerLimitPitch())
        {
          theSW.Send(aPointOnSurfacePairWithRange->LowerLimitPitch());
        }
        else
          theSW.SendUndef();

        if (aPointOnSurfacePairWithRange->HasUpperLimitPitch())
        {
          theSW.Send(aPointOnSurfacePairWithRange->UpperLimitPitch());
        }
        else
          theSW.SendUndef();

        if (aPointOnSurfacePairWithRange->HasLowerLimitRoll())
        {
          theSW.Send(aPointOnSurfacePairWithRange->LowerLimitRoll());
        }
        else
          theSW.SendUndef();

        if (aPointOnSurfacePairWithRange->HasUpperLimitRoll())
        {
          theSW.Send(aPointOnSurfacePairWithRange->UpperLimitRoll());
        }
        else
          theSW.SendUndef();
      }
    }
    else if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_PlanarCurvePair)))
    {
      theSW.StartEntity("PLANAR_CURVE_PAIR");
      occ::handle<StepKinematics_PlanarCurvePair> aPlanarCurvePair =
        occ::down_cast<StepKinematics_PlanarCurvePair>(theEnt->GetOrderKinematicPair());
      // Own fields of PlanarCurvePair

      theSW.Send(aPlanarCurvePair->Curve1());

      theSW.Send(aPlanarCurvePair->Curve2());

      theSW.StartEntity("(PLANAR_CURVE_PAIR_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_PlanarCurvePairRange)))
      {
        occ::handle<StepKinematics_PlanarCurvePairRange> aPlanarCurvePairRange =
          occ::down_cast<StepKinematics_PlanarCurvePairRange>(theEnt->GetOrderKinematicPair());
        // Own fields of PlanarCurvePairRange

        theSW.Send(aPlanarCurvePairRange->RangeOnCurve1());

        theSW.Send(aPlanarCurvePairRange->RangeOnCurve2());
      }
    }
    else if (theEnt->GetOrderKinematicPair()->IsKind(STANDARD_TYPE(StepKinematics_SurfacePair)))
    {
      theSW.StartEntity("SURFACE_PAIR");
      occ::handle<StepKinematics_SurfacePair> aSurfacePair =
        occ::down_cast<StepKinematics_SurfacePair>(theEnt->GetOrderKinematicPair());
      // Own fields of SurfacePair

      theSW.Send(aSurfacePair->Surface1());

      theSW.Send(aSurfacePair->Surface2());

      theSW.SendBoolean(aSurfacePair->Orientation());

      theSW.StartEntity("(SURFACE_PAIR_WITH_RANGE");
      if (theEnt->GetOrderKinematicPair()->IsKind(
            STANDARD_TYPE(StepKinematics_SurfacePairWithRange)))
      {
        occ::handle<StepKinematics_SurfacePairWithRange> aSurfacePairWithRange =
          occ::down_cast<StepKinematics_SurfacePairWithRange>(theEnt->GetOrderKinematicPair());
        // Own fields of SurfacePairWithRange

        theSW.Send(aSurfacePairWithRange->RangeOnSurface1());

        theSW.Send(aSurfacePairWithRange->RangeOnSurface2());

        if (aSurfacePairWithRange->HasLowerLimitActualRotation())
        {
          theSW.Send(aSurfacePairWithRange->LowerLimitActualRotation());
        }
        else
          theSW.SendUndef();

        if (aSurfacePairWithRange->HasUpperLimitActualRotation())
        {
          theSW.Send(aSurfacePairWithRange->UpperLimitActualRotation());
        }
        else
          theSW.SendUndef();
      }
    }
  }

  // Own fields of RepresentationItem
  theSW.StartEntity("REPRESENTATION_ITEM");
  theSW.Send(theEnt->Name());
}

//=================================================================================================

void RWStepKinematics_RWActuatedKinPairAndOrderKinPair::Share(
  const occ::handle<StepKinematics_ActuatedKinPairAndOrderKinPair>& theEnt,
  Interface_EntityIterator&                                         iter) const
{
  iter.AddItem(theEnt->GetOrderKinematicPair());
  iter.AddItem(theEnt->GetOrderKinematicPair()->Joint());
  iter.AddItem(theEnt->GetActuatedKinematicPair());
  iter.AddItem(theEnt->GetOrderKinematicPair()->ItemDefinedTransformation());
}
