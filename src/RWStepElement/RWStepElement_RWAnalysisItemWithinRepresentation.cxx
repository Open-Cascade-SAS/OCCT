// File:	RWStepElement_RWAnalysisItemWithinRepresentation.cxx
// Created:	Thu Dec 12 17:28:59 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepElement_RWAnalysisItemWithinRepresentation.ixx>

//=======================================================================
//function : RWStepElement_RWAnalysisItemWithinRepresentation
//purpose  : 
//=======================================================================

RWStepElement_RWAnalysisItemWithinRepresentation::RWStepElement_RWAnalysisItemWithinRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWAnalysisItemWithinRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                 const Standard_Integer num,
                                                                 Handle(Interface_Check)& ach,
                                                                 const Handle(StepElement_AnalysisItemWithinRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"analysis_item_within_representation") ) return;

  // Own fields of AnalysisItemWithinRepresentation

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  Handle(StepRepr_RepresentationItem) aItem;
  data->ReadEntity (num, 3, "item", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aItem);

  Handle(StepRepr_Representation) aRep;
  data->ReadEntity (num, 4, "rep", ach, STANDARD_TYPE(StepRepr_Representation), aRep);

  // Initialize entity
  ent->Init(aName,
            aDescription,
            aItem,
            aRep);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWAnalysisItemWithinRepresentation::WriteStep (StepData_StepWriter& SW,
                                                                  const Handle(StepElement_AnalysisItemWithinRepresentation) &ent) const
{

  // Own fields of AnalysisItemWithinRepresentation

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->Item());

  SW.Send (ent->Rep());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWAnalysisItemWithinRepresentation::Share (const Handle(StepElement_AnalysisItemWithinRepresentation) &ent,
                                                              Interface_EntityIterator& iter) const
{

  // Own fields of AnalysisItemWithinRepresentation

  iter.AddItem (ent->Item());

  iter.AddItem (ent->Rep());
}
