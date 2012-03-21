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

#include <RWStepElement_RWSurfaceSection.ixx>

//=======================================================================
//function : RWStepElement_RWSurfaceSection
//purpose  : 
//=======================================================================

RWStepElement_RWSurfaceSection::RWStepElement_RWSurfaceSection ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSection::ReadStep (const Handle(StepData_StepReaderData)& data,
                                               const Standard_Integer num,
                                               Handle(Interface_Check)& ach,
                                               const Handle(StepElement_SurfaceSection) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"surface_section") ) return;

  // Own fields of SurfaceSection

  StepElement_MeasureOrUnspecifiedValue aOffset;
  data->ReadEntity (num, 1, "offset", ach, aOffset);

  StepElement_MeasureOrUnspecifiedValue aNonStructuralMass;
  data->ReadEntity (num, 2, "non_structural_mass", ach, aNonStructuralMass);

  StepElement_MeasureOrUnspecifiedValue aNonStructuralMassOffset;
  data->ReadEntity (num, 3, "non_structural_mass_offset", ach, aNonStructuralMassOffset);

  // Initialize entity
  ent->Init(aOffset,
            aNonStructuralMass,
            aNonStructuralMassOffset);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSection::WriteStep (StepData_StepWriter& SW,
                                                const Handle(StepElement_SurfaceSection) &ent) const
{

  // Own fields of SurfaceSection

  SW.Send (ent->Offset().Value());

  SW.Send (ent->NonStructuralMass().Value());

  SW.Send (ent->NonStructuralMassOffset().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSection::Share (const Handle(StepElement_SurfaceSection) &ent,
                                            Interface_EntityIterator& iter) const
{

  // Own fields of SurfaceSection
/*  CKY 17JUN04. Content is made of REAL and ENUM. No entity !
  iter.AddItem (ent->Offset().Value());

  iter.AddItem (ent->NonStructuralMass().Value());

  iter.AddItem (ent->NonStructuralMassOffset().Value());
*/
}
