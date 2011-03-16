// File:	RWStepElement_RWSurfaceElementProperty.cxx
// Created:	Thu Dec 12 17:29:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepElement_RWSurfaceElementProperty.ixx>

//=======================================================================
//function : RWStepElement_RWSurfaceElementProperty
//purpose  : 
//=======================================================================

RWStepElement_RWSurfaceElementProperty::RWStepElement_RWSurfaceElementProperty ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceElementProperty::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                       const Standard_Integer num,
                                                       Handle(Interface_Check)& ach,
                                                       const Handle(StepElement_SurfaceElementProperty) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"surface_element_property") ) return;

  // Own fields of SurfaceElementProperty

  Handle(TCollection_HAsciiString) aPropertyId;
  data->ReadString (num, 1, "property_id", ach, aPropertyId);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  Handle(StepElement_SurfaceSectionField) aSection;
  data->ReadEntity (num, 3, "section", ach, STANDARD_TYPE(StepElement_SurfaceSectionField), aSection);

  // Initialize entity
  ent->Init(aPropertyId,
            aDescription,
            aSection);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceElementProperty::WriteStep (StepData_StepWriter& SW,
                                                        const Handle(StepElement_SurfaceElementProperty) &ent) const
{

  // Own fields of SurfaceElementProperty

  SW.Send (ent->PropertyId());

  SW.Send (ent->Description());

  SW.Send (ent->Section());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceElementProperty::Share (const Handle(StepElement_SurfaceElementProperty) &ent,
                                                    Interface_EntityIterator& iter) const
{

  // Own fields of SurfaceElementProperty

  iter.AddItem (ent->Section());
}
