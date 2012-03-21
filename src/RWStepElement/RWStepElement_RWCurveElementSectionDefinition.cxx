// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <RWStepElement_RWCurveElementSectionDefinition.ixx>

//=======================================================================
//function : RWStepElement_RWCurveElementSectionDefinition
//purpose  : 
//=======================================================================

RWStepElement_RWCurveElementSectionDefinition::RWStepElement_RWCurveElementSectionDefinition ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementSectionDefinition::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                              const Standard_Integer num,
                                                              Handle(Interface_Check)& ach,
                                                              const Handle(StepElement_CurveElementSectionDefinition) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"curve_element_section_definition") ) return;

  // Own fields of CurveElementSectionDefinition

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 1, "description", ach, aDescription);

  Standard_Real aSectionAngle;
  data->ReadReal (num, 2, "section_angle", ach, aSectionAngle);

  // Initialize entity
  ent->Init(aDescription,
            aSectionAngle);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementSectionDefinition::WriteStep (StepData_StepWriter& SW,
                                                               const Handle(StepElement_CurveElementSectionDefinition) &ent) const
{

  // Own fields of CurveElementSectionDefinition

  SW.Send (ent->Description());

  SW.Send (ent->SectionAngle());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementSectionDefinition::Share (const Handle(StepElement_CurveElementSectionDefinition) &ent,
                                                           Interface_EntityIterator& iter) const
{

  // Own fields of CurveElementSectionDefinition
}
