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

#include <RWStepElement_RWSurfaceSectionField.ixx>

//=======================================================================
//function : RWStepElement_RWSurfaceSectionField
//purpose  : 
//=======================================================================

RWStepElement_RWSurfaceSectionField::RWStepElement_RWSurfaceSectionField ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionField::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepElement_SurfaceSectionField) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,0,ach,"surface_section_field") ) return;

  // Initialize entity
//  ent->Init();
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionField::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepElement_SurfaceSectionField) &ent) const
{
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionField::Share (const Handle(StepElement_SurfaceSectionField) &ent,
                                                 Interface_EntityIterator& iter) const
{
}
