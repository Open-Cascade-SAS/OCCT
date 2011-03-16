// File:	StepAP203_CcDesignDateAndTimeAssignment.cxx
// Created:	Fri Nov 26 16:26:32 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_CcDesignDateAndTimeAssignment.ixx>

//=======================================================================
//function : StepAP203_CcDesignDateAndTimeAssignment
//purpose  : 
//=======================================================================

StepAP203_CcDesignDateAndTimeAssignment::StepAP203_CcDesignDateAndTimeAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_CcDesignDateAndTimeAssignment::Init (const Handle(StepBasic_DateAndTime) &aDateAndTimeAssignment_AssignedDateAndTime,
                                                    const Handle(StepBasic_DateTimeRole) &aDateAndTimeAssignment_Role,
                                                    const Handle(StepAP203_HArray1OfDateTimeItem) &aItems)
{
  StepBasic_DateAndTimeAssignment::Init(aDateAndTimeAssignment_AssignedDateAndTime,
                                        aDateAndTimeAssignment_Role);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfDateTimeItem) StepAP203_CcDesignDateAndTimeAssignment::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_CcDesignDateAndTimeAssignment::SetItems (const Handle(StepAP203_HArray1OfDateTimeItem) &aItems)
{
  theItems = aItems;
}
