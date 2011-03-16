// File:	StepFEA_FeaSurfaceSectionGeometricRelationship.cxx
// Created:	Wed Jan 22 17:31:43 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaSurfaceSectionGeometricRelationship.ixx>

//=======================================================================
//function : StepFEA_FeaSurfaceSectionGeometricRelationship
//purpose  : 
//=======================================================================

StepFEA_FeaSurfaceSectionGeometricRelationship::StepFEA_FeaSurfaceSectionGeometricRelationship ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaSurfaceSectionGeometricRelationship::Init (const Handle(StepElement_SurfaceSection) &aSectionRef,
                                                           const Handle(StepElement_AnalysisItemWithinRepresentation) &aItem)
{

  theSectionRef = aSectionRef;

  theItem = aItem;
}

//=======================================================================
//function : SectionRef
//purpose  : 
//=======================================================================

Handle(StepElement_SurfaceSection) StepFEA_FeaSurfaceSectionGeometricRelationship::SectionRef () const
{
  return theSectionRef;
}

//=======================================================================
//function : SetSectionRef
//purpose  : 
//=======================================================================

void StepFEA_FeaSurfaceSectionGeometricRelationship::SetSectionRef (const Handle(StepElement_SurfaceSection) &aSectionRef)
{
  theSectionRef = aSectionRef;
}

//=======================================================================
//function : Item
//purpose  : 
//=======================================================================

Handle(StepElement_AnalysisItemWithinRepresentation) StepFEA_FeaSurfaceSectionGeometricRelationship::Item () const
{
  return theItem;
}

//=======================================================================
//function : SetItem
//purpose  : 
//=======================================================================

void StepFEA_FeaSurfaceSectionGeometricRelationship::SetItem (const Handle(StepElement_AnalysisItemWithinRepresentation) &aItem)
{
  theItem = aItem;
}
