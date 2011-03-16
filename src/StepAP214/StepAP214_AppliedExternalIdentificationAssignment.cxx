// File:	StepAP214_AppliedExternalIdentificationAssignment.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepAP214_AppliedExternalIdentificationAssignment.ixx>

//=======================================================================
//function : StepAP214_AppliedExternalIdentificationAssignment
//purpose  : 
//=======================================================================

StepAP214_AppliedExternalIdentificationAssignment::StepAP214_AppliedExternalIdentificationAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP214_AppliedExternalIdentificationAssignment::Init (const Handle(TCollection_HAsciiString) &aIdentificationAssignment_AssignedId,
                                                              const Handle(StepBasic_IdentificationRole) &aIdentificationAssignment_Role,
                                                              const Handle(StepBasic_ExternalSource) &aExternalIdentificationAssignment_Source,
                                                              const Handle(StepAP214_HArray1OfExternalIdentificationItem) &aItems)
{
  StepBasic_ExternalIdentificationAssignment::Init(aIdentificationAssignment_AssignedId,
                                                   aIdentificationAssignment_Role,
                                                   aExternalIdentificationAssignment_Source);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP214_HArray1OfExternalIdentificationItem) StepAP214_AppliedExternalIdentificationAssignment::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP214_AppliedExternalIdentificationAssignment::SetItems (const Handle(StepAP214_HArray1OfExternalIdentificationItem) &aItems)
{
  theItems = aItems;
}
