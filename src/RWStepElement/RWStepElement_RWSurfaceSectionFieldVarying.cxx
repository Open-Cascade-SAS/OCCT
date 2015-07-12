// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepElement_RWSurfaceSectionFieldVarying.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_HArray1OfSurfaceSection.hxx>
#include <StepElement_SurfaceSection.hxx>
#include <StepElement_SurfaceSectionFieldVarying.hxx>

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
