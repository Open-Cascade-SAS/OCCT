// File:	StepAP203_StartWork.cxx
// Created:	Fri Nov 26 16:26:40 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_StartWork.ixx>

//=======================================================================
//function : StepAP203_StartWork
//purpose  : 
//=======================================================================

StepAP203_StartWork::StepAP203_StartWork ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_StartWork::Init (const Handle(StepBasic_Action) &aActionAssignment_AssignedAction,
                                const Handle(StepAP203_HArray1OfWorkItem) &aItems)
{
  StepBasic_ActionAssignment::Init(aActionAssignment_AssignedAction);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfWorkItem) StepAP203_StartWork::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_StartWork::SetItems (const Handle(StepAP203_HArray1OfWorkItem) &aItems)
{
  theItems = aItems;
}
