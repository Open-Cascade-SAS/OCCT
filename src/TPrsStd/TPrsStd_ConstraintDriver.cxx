// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <TPrsStd_ConstraintDriver.ixx>

#include <TDF_Label.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TPrsStd_ConstraintTools.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_GUID.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd_Position.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Relation.hxx>

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
      if (anAISObject->Color() != Quantity_NOC_RED) anAISObject->SetColor(Quantity_NOC_RED);
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
 
  anAIS->ResetLocation();
  anAIS->SetToUpdate();
  anAIS->UpdateSelection();
  
  anAISObject = anAIS;
  
  Handle(TDataXtd_Position) Position;
  if (aLabel.FindAttribute(TDataXtd_Position::GetID(),Position)) {
    Handle(AIS_Relation)::DownCast(anAISObject)->SetPosition(Position->GetPosition());
  }

  if (anAISObject->HasInteractiveContext()) {
    Quantity_NameOfColor originColor = anAISObject->Color();
    if (!apConstraint->Verified()) {
      if (originColor != Quantity_NOC_RED)
	anAISObject->SetColor(Quantity_NOC_RED);
    }
    else if (apConstraint->IsDimension() && apConstraint->GetValue()->IsCaptured()) {
      if (originColor != Quantity_NOC_PURPLE)
	anAISObject->SetColor(Quantity_NOC_PURPLE);
    }
    else if (!apConstraint->IsPlanar() && (originColor != Quantity_NOC_YELLOW)) 
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

