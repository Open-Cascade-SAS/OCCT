// File:	RWStepFEA_RWFeaSurfaceSectionGeometricRelationship.cxx
// Created:	Wed Jan 22 17:31:43 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaSurfaceSectionGeometricRelationship.ixx>

//=======================================================================
//function : RWStepFEA_RWFeaSurfaceSectionGeometricRelationship
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaSurfaceSectionGeometricRelationship::RWStepFEA_RWFeaSurfaceSectionGeometricRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaSurfaceSectionGeometricRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                   const Standard_Integer num,
                                                                   Handle(Interface_Check)& ach,
                                                                   const Handle(StepFEA_FeaSurfaceSectionGeometricRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"fea_surface_section_geometric_relationship") ) return;

  // Own fields of FeaSurfaceSectionGeometricRelationship

  Handle(StepElement_SurfaceSection) aSectionRef;
  data->ReadEntity (num, 1, "section_ref", ach, STANDARD_TYPE(StepElement_SurfaceSection), aSectionRef);

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

void RWStepFEA_RWFeaSurfaceSectionGeometricRelationship::WriteStep (StepData_StepWriter& SW,
                                                                    const Handle(StepFEA_FeaSurfaceSectionGeometricRelationship) &ent) const
{

  // Own fields of FeaSurfaceSectionGeometricRelationship

  SW.Send (ent->SectionRef());

  SW.Send (ent->Item());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaSurfaceSectionGeometricRelationship::Share (const Handle(StepFEA_FeaSurfaceSectionGeometricRelationship) &ent,
                                                                Interface_EntityIterator& iter) const
{

  // Own fields of FeaSurfaceSectionGeometricRelationship

  iter.AddItem (ent->SectionRef());

  iter.AddItem (ent->Item());
}
