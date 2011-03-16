// File:	RWStepFEA_RWElementGeometricRelationship.cxx
// Created:	Tue Feb  4 10:39:08 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWElementGeometricRelationship.ixx>

//=======================================================================
//function : RWStepFEA_RWElementGeometricRelationship
//purpose  : 
//=======================================================================

RWStepFEA_RWElementGeometricRelationship::RWStepFEA_RWElementGeometricRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWElementGeometricRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                         const Standard_Integer num,
                                                         Handle(Interface_Check)& ach,
                                                         const Handle(StepFEA_ElementGeometricRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"element_geometric_relationship") ) return;

  // Own fields of ElementGeometricRelationship

  StepFEA_ElementOrElementGroup aElementRef;
  data->ReadEntity (num, 1, "element_ref", ach, aElementRef);

  Handle(StepElement_AnalysisItemWithinRepresentation) aItem;
  data->ReadEntity (num, 2, "item", ach, STANDARD_TYPE(StepElement_AnalysisItemWithinRepresentation), aItem);

  StepElement_ElementAspect aAspect;
  data->ReadEntity (num, 3, "aspect", ach, aAspect);

  // Initialize entity
  ent->Init(aElementRef,
            aItem,
            aAspect);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWElementGeometricRelationship::WriteStep (StepData_StepWriter& SW,
                                                          const Handle(StepFEA_ElementGeometricRelationship) &ent) const
{

  // Own fields of ElementGeometricRelationship

  SW.Send (ent->ElementRef().Value());

  SW.Send (ent->Item());

  SW.Send (ent->Aspect().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWElementGeometricRelationship::Share (const Handle(StepFEA_ElementGeometricRelationship) &ent,
                                                      Interface_EntityIterator& iter) const
{

  // Own fields of ElementGeometricRelationship

  iter.AddItem (ent->ElementRef().Value());

  iter.AddItem (ent->Item());

  iter.AddItem (ent->Aspect().Value());
}
