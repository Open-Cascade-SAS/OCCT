// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <PrsDim_Relation.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Position.hxx>
#include <TDF_Label.hxx>
#include <TPrsStd_ConstraintDriver.hxx>
#include <TPrsStd_ConstraintTools.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TPrsStd_ConstraintDriver, TPrsStd_Driver)

//=================================================================================================

TPrsStd_ConstraintDriver::TPrsStd_ConstraintDriver() = default;

//=================================================================================================

bool TPrsStd_ConstraintDriver::Update(const TDF_Label&                    aLabel,
                                      occ::handle<AIS_InteractiveObject>& anAISObject)
{
  occ::handle<TDataXtd_Constraint> apConstraint;
  if (!aLabel.FindAttribute(TDataXtd_Constraint::GetID(), apConstraint))
  {
    return false;
  }

  if (!anAISObject.IsNull() && anAISObject->HasInteractiveContext())
  {
    if (!apConstraint->Verified())
    {
      TPrsStd_ConstraintTools::UpdateOnlyValue(apConstraint, anAISObject);
      Quantity_Color aColor;
      anAISObject->Color(aColor);
      if (aColor.Name() != Quantity_NOC_RED)
      {
        anAISObject->SetColor(Quantity_NOC_RED);
      }
      return true;
    }
  }

  occ::handle<AIS_InteractiveObject> anAIS = anAISObject;

  // recuperation
  TDataXtd_ConstraintEnum thetype = apConstraint->GetType();

  switch (thetype)
  {
    case TDataXtd_DISTANCE: {
      anAIS = TPrsStd_ConstraintTools::ComputeDistance(apConstraint);
      break;
    }
    case TDataXtd_PARALLEL: {
      anAIS = TPrsStd_ConstraintTools::ComputeParallel(apConstraint);
      break;
    }
    case TDataXtd_PERPENDICULAR: {
      anAIS = TPrsStd_ConstraintTools::ComputePerpendicular(apConstraint);
      break;
    }
    case TDataXtd_CONCENTRIC: {
      anAIS = TPrsStd_ConstraintTools::ComputeConcentric(apConstraint);
      break;
    }
    case TDataXtd_SYMMETRY: {
      anAIS = TPrsStd_ConstraintTools::ComputeSymmetry(apConstraint);
      break;
    }
    case TDataXtd_MIDPOINT: {
      anAIS = TPrsStd_ConstraintTools::ComputeMidPoint(apConstraint);
      break;
    }
    case TDataXtd_TANGENT: {
      anAIS = TPrsStd_ConstraintTools::ComputeTangent(apConstraint);
      break;
    }
    case TDataXtd_ANGLE: {
      anAIS = TPrsStd_ConstraintTools::ComputeAngle(apConstraint);
      break;
    }
    case TDataXtd_RADIUS: {
      anAIS = TPrsStd_ConstraintTools::ComputeRadius(apConstraint);
      break;
    }
    case TDataXtd_MINOR_RADIUS: {
      anAIS = TPrsStd_ConstraintTools::ComputeMinRadius(apConstraint);
      break;
    }
    case TDataXtd_MAJOR_RADIUS: {
      anAIS = TPrsStd_ConstraintTools::ComputeMaxRadius(apConstraint);
      break;
    }
    case TDataXtd_DIAMETER: {
      anAIS = TPrsStd_ConstraintTools::ComputeDiameter(apConstraint);
      break;
    }
    case TDataXtd_FIX: {
      anAIS = TPrsStd_ConstraintTools::ComputeFix(apConstraint);
      break;
    }
    case TDataXtd_OFFSET: {
      anAIS = TPrsStd_ConstraintTools::ComputeOffset(apConstraint);
      break;
    }
    case TDataXtd_COINCIDENT: {
      anAIS = TPrsStd_ConstraintTools::ComputeCoincident(apConstraint);
      break;
    }
    case TDataXtd_ROUND: {
      anAIS = TPrsStd_ConstraintTools::ComputeRound(apConstraint);
      break;
    }

    case TDataXtd_MATE:
    case TDataXtd_ALIGN_FACES:
    case TDataXtd_ALIGN_AXES:
    case TDataXtd_AXES_ANGLE: {
      anAIS = TPrsStd_ConstraintTools::ComputePlacement(apConstraint);
      break;
    }
    case TDataXtd_EQUAL_DISTANCE: {
      anAIS = TPrsStd_ConstraintTools::ComputeEqualDistance(apConstraint);
      break;
    }
    case TDataXtd_EQUAL_RADIUS: {
      anAIS = TPrsStd_ConstraintTools::ComputeEqualRadius(apConstraint);
      break;
    }
    default: {
      anAIS = TPrsStd_ConstraintTools::ComputeOthers(apConstraint);
      break;
    }
  }
  if (anAIS.IsNull())
    return false;

  anAIS->ResetTransformation();
  anAIS->SetToUpdate();
  anAIS->UpdateSelection();

  anAISObject = anAIS;

  occ::handle<TDataXtd_Position> Position;
  if (aLabel.FindAttribute(TDataXtd_Position::GetID(), Position))
  {
    occ::down_cast<PrsDim_Relation>(anAISObject)->SetPosition(Position->GetPosition());
  }

  if (anAISObject->HasInteractiveContext())
  {
    Quantity_Color originColor;
    anAISObject->Color(originColor);
    if (!apConstraint->Verified())
    {
      if (originColor.Name() != Quantity_NOC_RED)
        anAISObject->SetColor(Quantity_NOC_RED);
    }
    else if (apConstraint->IsDimension() && apConstraint->GetValue()->IsCaptured())
    {
      if (originColor.Name() != Quantity_NOC_PURPLE)
        anAISObject->SetColor(Quantity_NOC_PURPLE);
    }
    else if (!apConstraint->IsPlanar() && (originColor.Name() != Quantity_NOC_YELLOW))
      anAISObject->SetColor(Quantity_NOC_YELLOW);
  }
  else
  {
    if (!apConstraint->Verified())
    {
      anAISObject->SetColor(Quantity_NOC_RED);
    }
    else if (apConstraint->IsDimension() && apConstraint->GetValue()->IsCaptured())
    {
      anAISObject->SetColor(Quantity_NOC_PURPLE);
    }
    else if (!apConstraint->IsPlanar())
      anAISObject->SetColor(Quantity_NOC_YELLOW);
  }
  return true;
}
