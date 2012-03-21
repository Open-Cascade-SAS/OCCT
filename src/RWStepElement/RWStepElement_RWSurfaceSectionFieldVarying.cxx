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

#include <RWStepElement_RWSurfaceSectionFieldVarying.ixx>
#include <StepElement_HArray1OfSurfaceSection.hxx>
#include <StepElement_SurfaceSection.hxx>

//=======================================================================
//function : RWStepElement_RWSurfaceSectionFieldVarying
//purpose  : 
//=======================================================================

RWStepElement_RWSurfaceSectionFieldVarying::RWStepElement_RWSurfaceSectionFieldVarying ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionFieldVarying::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                           const Standard_Integer num,
                                                           Handle(Interface_Check)& ach,
                                                           const Handle(StepElement_SurfaceSectionFieldVarying) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"surface_section_field_varying") ) return;

  // Own fields of SurfaceSectionFieldVarying

  Handle(StepElement_HArray1OfSurfaceSection) aDefinitions;
  Standard_Integer sub1 = 0;
  if ( data->ReadSubList (num, 1, "definitions", ach, sub1) ) {
    Standard_Integer nb0 = data->NbParams(sub1);
    aDefinitions = new StepElement_HArray1OfSurfaceSection (1, nb0);
    Standard_Integer num2 = sub1;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepElement_SurfaceSection) anIt0;
      data->ReadEntity (num2, i0, "surface_section", ach, STANDARD_TYPE(StepElement_SurfaceSection), anIt0);
      aDefinitions->SetValue(i0, anIt0);
    }
  }

  Standard_Boolean aAdditionalNodeValues;
  data->ReadBoolean (num, 2, "additional_node_values", ach, aAdditionalNodeValues);

  // Initialize entity
  ent->Init(aDefinitions,
            aAdditionalNodeValues);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionFieldVarying::WriteStep (StepData_StepWriter& SW,
                                                            const Handle(StepElement_SurfaceSectionFieldVarying) &ent) const
{

  // Own fields of SurfaceSectionFieldVarying

  SW.OpenSub();
  for (Standard_Integer i0=1; i0 <= ent->Definitions()->Length(); i0++ ) {
    Handle(StepElement_SurfaceSection) Var0 = ent->Definitions()->Value(i0);
    SW.Send (Var0);
  }
  SW.CloseSub();

  SW.SendBoolean (ent->AdditionalNodeValues());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionFieldVarying::Share (const Handle(StepElement_SurfaceSectionFieldVarying) &ent,
                                                        Interface_EntityIterator& iter) const
{

  // Own fields of SurfaceSectionFieldVarying

  for (Standard_Integer i1=1; i1 <= ent->Definitions()->Length(); i1++ ) {
    Handle(StepElement_SurfaceSection) Var0 = ent->Definitions()->Value(i1);
    iter.AddItem (Var0);
  }
}
