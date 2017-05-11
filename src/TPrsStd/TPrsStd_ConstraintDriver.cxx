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
#include <AIS_Relation.hxx>
#include <Standard_GUID.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Position.hxx>
#include <TDF_Label.hxx>
#include <TPrsStd_ConstraintDriver.hxx>
#include <TPrsStd_ConstraintTools.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TPrsStd_ConstraintDriver,TPrsStd_Driver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
TPrsStd_ConstraintDriver::TPrsStd_ConstraintDriver()
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_ConstraintDriver::Update (const TDF_Label& aLabel,
						  Handle(AIS_InteractiveObject)& anAISObject) 
{
  Handle(TDataXtd_Constraint) apConstraint;
  if( !aLabel.FindAttribute(TDataXtd_Constraint::GetID(), apConstraint) ) {
   return Standard_False;
  }
   
  if (!anAISObject.IsNull() && anAISObject->HasInteractiveContext()) {
    if (!apConstraint->Verified()) {
      TPrsStd_ConstraintTools::UpdateOnlyValue(apConstraint,anAISObject);
      Quantity_Color aColor;
      anAISObject->Color (aColor);
      if (aColor.Name() != Quantity_NOC_RED)
      {
        anAISObject->SetColor(Quantity_NOC_RED);
      }
      return Standard_True;
    }
  }

  Handle(AIS_InteractiveObject) anAIS = anAISObject;

  // recuperation 
  TDataXtd_ConstraintEnum thetype = apConstraint->GetType();
  
  switch (thetype)  {
  case TDataXtd_DISTANCE:
    {
      TPrsStd_ConstraintTools::ComputeDistance(apConstraint,anAIS);
      break;
    }
  case TDataXtd_PARALLEL:
    {
      TPrsStd_ConstraintTools::ComputeParallel(apConstraint,anAIS);
      break;
    }
  case TDataXtd_PERPENDICULAR:
    {
      TPrsStd_ConstraintTools::ComputePerpendicular(apConstraint,anAIS);
      break;
    }
  case TDataXtd_CONCENTRIC:
    {
      TPrsStd_ConstraintTools::ComputeConcentric(apConstraint,anAIS);
      break;
    }
  case TDataXtd_SYMMETRY:
    {
      TPrsStd_ConstraintTools::ComputeSymmetry(apConstraint,anAIS);
      break;
    }
  case TDataXtd_MIDPOINT:
    {
      TPrsStd_ConstraintTools::ComputeMidPoint(apConstraint,anAIS);
      break;
    }
  case TDataXtd_TANGENT:
    {
      TPrsStd_ConstraintTools::ComputeTangent(apConstraint,anAIS);
      break;
    }
  case TDataXtd_ANGLE:
    {
      TPrsStd_ConstraintTools::ComputeAngle(apConstraint,anAIS);
      break;
    }
  case TDataXtd_RADIUS:
    {
      TPrsStd_ConstraintTools::ComputeRadius(apConstraint,anAIS);
      break;
    }
  case TDataXtd_MINOR_RADIUS:
    {
      TPrsStd_ConstraintTools::ComputeMinRadius(apConstraint,anAIS);
      break;
    }
  case TDataXtd_MAJOR_RADIUS:
    {
      TPrsStd_ConstraintTools::ComputeMaxRadius(apConstraint,anAIS);
      break; 
    }
  case TDataXtd_DIAMETER:
    {
      TPrsStd_ConstraintTools::ComputeDiameter(apConstraint,anAIS);
      break;
    }
  case TDataXtd_FIX:
    {
      TPrsStd_ConstraintTools::ComputeFix(apConstraint,anAIS);
      break;
    } 
  case TDataXtd_OFFSET:
    {
      TPrsStd_ConstraintTools::ComputeOffset(apConstraint,anAIS);
      break;
    }
  case TDataXtd_COINCIDENT:
    {
      TPrsStd_ConstraintTools::ComputeCoincident(apConstraint,anAIS); 
      break;
    }
  case TDataXtd_ROUND:
    {
      TPrsStd_ConstraintTools::ComputeRound(apConstraint,anAIS); 
      break;
    }

  case TDataXtd_MATE:
  case TDataXtd_ALIGN_FACES:
  case TDataXtd_ALIGN_AXES:
  case TDataXtd_AXES_ANGLE:
    {
      TPrsStd_ConstraintTools::ComputePlacement(apConstraint,anAIS);
      break;
    }
  case TDataXtd_EQUAL_DISTANCE :
    {
      TPrsStd_ConstraintTools::ComputeEqualDistance(apConstraint,anAIS);
      break;
    }
  case  TDataXtd_EQUAL_RADIUS:
    {
      TPrsStd_ConstraintTools::ComputeEqualRadius(apConstraint,anAIS);
      break;
    }
  default:
    {
      TPrsStd_ConstraintTools::ComputeOthers(apConstraint,anAIS);
      break;
    }
  }
  if (anAIS.IsNull()) return Standard_False;
 
  anAIS->ResetTransformation();
  anAIS->SetToUpdate();
  anAIS->UpdateSelection();
  
  anAISObject = anAIS;
  
  Handle(TDataXtd_Position) Position;
  if (aLabel.FindAttribute(TDataXtd_Position::GetID(),Position)) {
    Handle(AIS_Relation)::DownCast(anAISObject)->SetPosition(Position->GetPosition());
  }

  if (anAISObject->HasInteractiveContext()) {
    Quantity_Color originColor;
    anAISObject->Color (originColor);
    if (!apConstraint->Verified()) {
      if (originColor.Name() != Quantity_NOC_RED)
	anAISObject->SetColor(Quantity_NOC_RED);
    }
    else if (apConstraint->IsDimension() && apConstraint->GetValue()->IsCaptured()) {
      if (originColor.Name() != Quantity_NOC_PURPLE)
	anAISObject->SetColor(Quantity_NOC_PURPLE);
    }
    else if (!apConstraint->IsPlanar() && (originColor.Name() != Quantity_NOC_YELLOW))
      anAISObject->SetColor(Quantity_NOC_YELLOW);
  }
  else {
    if (!apConstraint->Verified()) {
      anAISObject->SetColor(Quantity_NOC_RED);
    }
    else if (apConstraint->IsDimension() && apConstraint->GetValue()->IsCaptured()) {
      anAISObject->SetColor(Quantity_NOC_PURPLE);
    }
    else if (!apConstraint->IsPlanar()) anAISObject->SetColor(Quantity_NOC_YELLOW);
  }
  return Standard_True;
}

