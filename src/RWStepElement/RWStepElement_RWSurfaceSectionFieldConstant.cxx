// File:	RWStepElement_RWSurfaceSectionFieldConstant.cxx
// Created:	Thu Dec 12 17:29:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepElement_RWSurfaceSectionFieldConstant.ixx>

//=======================================================================
//function : RWStepElement_RWSurfaceSectionFieldConstant
//purpose  : 
//=======================================================================

RWStepElement_RWSurfaceSectionFieldConstant::RWStepElement_RWSurfaceSectionFieldConstant ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionFieldConstant::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                            const Standard_Integer num,
                                                            Handle(Interface_Check)& ach,
                                                            const Handle(StepElement_SurfaceSectionFieldConstant) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"surface_section_field_constant") ) return;

  // Own fields of SurfaceSectionFieldConstant

  Handle(StepElement_SurfaceSection) aDefinition;
  data->ReadEntity (num, 1, "definition", ach, STANDARD_TYPE(StepElement_SurfaceSection), aDefinition);

  // Initialize entity
  ent->Init(aDefinition);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionFieldConstant::WriteStep (StepData_StepWriter& SW,
                                                             const Handle(StepElement_SurfaceSectionFieldConstant) &ent) const
{

  // Own fields of SurfaceSectionFieldConstant

  SW.Send (ent->Definition());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionFieldConstant::Share (const Handle(StepElement_SurfaceSectionFieldConstant) &ent,
                                                         Interface_EntityIterator& iter) const
{

  // Own fields of SurfaceSectionFieldConstant

  iter.AddItem (ent->Definition());
}
