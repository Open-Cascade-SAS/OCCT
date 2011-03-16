// File:	RWStepElement_RWSurfaceSectionFieldVarying.cxx
// Created:	Thu Dec 12 17:29:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
