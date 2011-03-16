// File:	StepAP203_Change.cxx
// Created:	Fri Nov 26 16:26:35 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_Change.ixx>

//=======================================================================
//function : StepAP203_Change
//purpose  : 
//=======================================================================

StepAP203_Change::StepAP203_Change ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_Change::Init (const Handle(StepBasic_Action) &aActionAssignment_AssignedAction,
                             const Handle(StepAP203_HArray1OfWorkItem) &aItems)
{
  StepBasic_ActionAssignment::Init(aActionAssignment_AssignedAction);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfWorkItem) StepAP203_Change::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_Change::SetItems (const Handle(StepAP203_HArray1OfWorkItem) &aItems)
{
  theItems = aItems;
}
