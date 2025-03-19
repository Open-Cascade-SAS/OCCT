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

#include "RWStepKinematics_RWPointOnPlanarCurvePairWithRange.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_PointOnPlanarCurvePairWithRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <Standard_Real.hxx>

//=================================================================================================

RWStepKinematics_RWPointOnPlanarCurvePairWithRange::
  RWStepKinematics_RWPointOnPlanarCurvePairWithRange()
{
}

//=================================================================================================

void RWStepKinematics_RWPointOnPlanarCurvePairWithRange::ReadStep(
  const Handle(StepData_StepReaderData)&                        theData,
  const Standard_Integer                                        theNum,
  Handle(Interface_Check)&                                      theArch,
  const Handle(StepKinematics_PointOnPlanarCurvePairWithRange)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 15, theArch, "point_on_planar_curve_pair_with_range"))
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

  // Inherited fields of PointOnPlanarCurvePair

  Handle(StepGeom_Curve) aPointOnPlanarCurvePair_PairCurve;
  theData->ReadEntity(theNum,
                      7,
                      "point_on_planar_curve_pair.pair_curve",
                      theArch,
                      STANDARD_TYPE(StepGeom_Curve),
                      aPointOnPlanarCurvePair_PairCurve);

  Standard_Boolean aPointOnPlanarCurvePair_Orientation;
  theData->ReadBoolean(theNum,
                       8,
                       "point_on_planar_curve_pair.orientation",
                       theArch,
                       aPointOnPlanarCurvePair_Orientation);

  // Own fields of PointOnPlanarCurvePairWithRange

  Handle(StepGeom_TrimmedCurve) aRangeOnPairCurve;
  theData->ReadEntity(theNum,
                      9,
                      "range_on_pair_curve",
                      theArch,
                      STANDARD_TYPE(StepGeom_TrimmedCurve),
                      aRangeOnPairCurve);

  Standard_Real    aLowerLimitYaw;
  Standard_Boolean hasLowerLimitYaw = Standard_True;
  if (theData->IsParamDefined(theNum, 10))
  {
    theData->ReadReal(theNum, 10, "lower_limit_yaw", theArch, aLowerLimitYaw);
  }
  else
  {
    hasLowerLimitYaw = Standard_False;
    aLowerLimitYaw   = 0;
  }

  Standard_Real    aUpperLimitYaw;
  Standard_Boolean hasUpperLimitYaw = Standard_True;
  if (theData->IsParamDefined(theNum, 11))
  {
    theData->ReadReal(theNum, 11, "upper_limit_yaw", theArch, aUpperLimitYaw);
  }
  else
  {
    hasUpperLimitYaw = Standard_False;
    aUpperLimitYaw   = 0;
  }

  Standard_Real    aLowerLimitPitch;
  Standard_Boolean hasLowerLimitPitch = Standard_True;
  if (theData->IsParamDefined(theNum, 12))
  {
    theData->ReadReal(theNum, 12, "lower_limit_pitch", theArch, aLowerLimitPitch);
  }
  else
  {
    hasLowerLimitPitch = Standard_False;
    aLowerLimitPitch   = 0;
  }

  Standard_Real    aUpperLimitPitch;
  Standard_Boolean hasUpperLimitPitch = Standard_True;
  if (theData->IsParamDefined(theNum, 13))
  {
    theData->ReadReal(theNum, 13, "upper_limit_pitch", theArch, aUpperLimitPitch);
  }
  else
  {
    hasUpperLimitPitch = Standard_False;
    aUpperLimitPitch   = 0;
  }

  Standard_Real    aLowerLimitRoll;
  Standard_Boolean hasLowerLimitRoll = Standard_True;
  if (theData->IsParamDefined(theNum, 14))
  {
    theData->ReadReal(theNum, 14, "lower_limit_roll", theArch, aLowerLimitRoll);
  }
  else
  {
    hasLowerLimitRoll = Standard_False;
    aLowerLimitRoll   = 0;
  }

  Standard_Real    aUpperLimitRoll;
  Standard_Boolean hasUpperLimitRoll = Standard_True;
  if (theData->IsParamDefined(theNum, 15))
  {
    theData->ReadReal(theNum, 15, "upper_limit_roll", theArch, aUpperLimitRoll);
  }
  else
  {
    hasUpperLimitRoll = Standard_False;
    aUpperLimitRoll   = 0;
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
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

//=================================================================================================

void RWStepKinematics_RWPointOnPlanarCurvePairWithRange::WriteStep(
  StepData_StepWriter&                                          theSW,
  const Handle(StepKinematics_PointOnPlanarCurvePairWithRange)& theEnt) const
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

  // Own fields of PointOnPlanarCurvePair

  theSW.Send(theEnt->PairCurve());

  theSW.SendBoolean(theEnt->Orientation());

  // Own fields of PointOnPlanarCurvePairWithRange

  theSW.Send(theEnt->RangeOnPairCurve());

  if (theEnt->HasLowerLimitYaw())
  {
    theSW.Send(theEnt->LowerLimitYaw());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasUpperLimitYaw())
  {
    theSW.Send(theEnt->UpperLimitYaw());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasLowerLimitPitch())
  {
    theSW.Send(theEnt->LowerLimitPitch());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasUpperLimitPitch())
  {
    theSW.Send(theEnt->UpperLimitPitch());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasLowerLimitRoll())
  {
    theSW.Send(theEnt->LowerLimitRoll());
  }
  else
    theSW.SendUndef();

  if (theEnt->HasUpperLimitRoll())
  {
    theSW.Send(theEnt->UpperLimitRoll());
  }
  else
    theSW.SendUndef();
}

//=================================================================================================

void RWStepKinematics_RWPointOnPlanarCurvePairWithRange::Share(
  const Handle(StepKinematics_PointOnPlanarCurvePairWithRange)& theEnt,
  Interface_EntityIterator&                                     iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem(theEnt->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem(theEnt->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem(theEnt->StepKinematics_KinematicPair::Joint());

  // Inherited fields of PointOnPlanarCurvePair

  iter.AddItem(theEnt->StepKinematics_PointOnPlanarCurvePair::PairCurve());

  // Own fields of PointOnPlanarCurvePairWithRange

  iter.AddItem(theEnt->RangeOnPairCurve());
}
