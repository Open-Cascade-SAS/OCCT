// File:	StepAP203_CcDesignPersonAndOrganizationAssignment.cxx
// Created:	Fri Nov 26 16:26:32 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_CcDesignPersonAndOrganizationAssignment.ixx>

//=======================================================================
//function : StepAP203_CcDesignPersonAndOrganizationAssignment
//purpose  : 
//=======================================================================

StepAP203_CcDesignPersonAndOrganizationAssignment::StepAP203_CcDesignPersonAndOrganizationAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_CcDesignPersonAndOrganizationAssignment::Init (const Handle(StepBasic_PersonAndOrganization) &aPersonAndOrganizationAssignment_AssignedPersonAndOrganization,
                                                              const Handle(StepBasic_PersonAndOrganizationRole) &aPersonAndOrganizationAssignment_Role,
                                                              const Handle(StepAP203_HArray1OfPersonOrganizationItem) &aItems)
{
  StepBasic_PersonAndOrganizationAssignment::Init(aPersonAndOrganizationAssignment_AssignedPersonAndOrganization,
                                                  aPersonAndOrganizationAssignment_Role);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfPersonOrganizationItem) StepAP203_CcDesignPersonAndOrganizationAssignment::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_CcDesignPersonAndOrganizationAssignment::SetItems (const Handle(StepAP203_HArray1OfPersonOrganizationItem) &aItems)
{
  theItems = aItems;
}
