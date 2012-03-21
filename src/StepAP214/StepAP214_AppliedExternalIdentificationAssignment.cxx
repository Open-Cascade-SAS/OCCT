// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

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
