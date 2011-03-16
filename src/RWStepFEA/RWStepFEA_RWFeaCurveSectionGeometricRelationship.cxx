// File:	RWStepFEA_RWFeaCurveSectionGeometricRelationship.cxx
// Created:	Wed Jan 22 17:31:43 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaCurveSectionGeometricRelationship.ixx>

//=======================================================================
//function : RWStepFEA_RWFeaCurveSectionGeometricRelationship
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaCurveSectionGeometricRelationship::RWStepFEA_RWFeaCurveSectionGeometricRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaCurveSectionGeometricRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                 const Standard_Integer num,
                                                                 Handle(Interface_Check)& ach,
                                                                 const Handle(StepFEA_FeaCurveSectionGeometricRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"fea_curve_section_geometric_relationship") ) return;

  // Own fields of FeaCurveSectionGeometricRelationship

  Handle(StepElement_CurveElementSectionDefinition) aSectionRef;
  data->ReadEntity (num, 1, "section_ref", ach, STANDARD_TYPE(StepElement_CurveElementSectionDefinition), aSectionRef);

  Handle(StepElement_AnalysisItemWithinRepresentation) aItem;
  data->ReadEntity (num, 2, "item", ach, STANDARD_TYPE(StepElement_AnalysisItemWithinRepresentation), aItem);

  // Initialize entity
  ent->Init(aSectionRef,
            aItem);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaCurveSectionGeometricRelationship::WriteStep (StepData_StepWriter& SW,
                                                                  const Handle(StepFEA_FeaCurveSectionGeometricRelationship) &ent) const
{

  // Own fields of FeaCurveSectionGeometricRelationship

  SW.Send (ent->SectionRef());

  SW.Send (ent->Item());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaCurveSectionGeometricRelationship::Share (const Handle(StepFEA_FeaCurveSectionGeometricRelationship) &ent,
                                                              Interface_EntityIterator& iter) const
{

  // Own fields of FeaCurveSectionGeometricRelationship

  iter.AddItem (ent->SectionRef());

  iter.AddItem (ent->Item());
}
