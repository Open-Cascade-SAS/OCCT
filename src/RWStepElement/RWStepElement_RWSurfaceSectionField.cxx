// File:	RWStepElement_RWSurfaceSectionField.cxx
// Created:	Thu Dec 12 17:29:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
