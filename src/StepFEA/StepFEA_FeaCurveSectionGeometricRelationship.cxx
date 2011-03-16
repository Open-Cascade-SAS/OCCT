// File:	StepFEA_FeaCurveSectionGeometricRelationship.cxx
// Created:	Wed Jan 22 17:31:43 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaCurveSectionGeometricRelationship.ixx>

//=======================================================================
//function : StepFEA_FeaCurveSectionGeometricRelationship
//purpose  : 
//=======================================================================

StepFEA_FeaCurveSectionGeometricRelationship::StepFEA_FeaCurveSectionGeometricRelationship ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaCurveSectionGeometricRelationship::Init (const Handle(StepElement_CurveElementSectionDefinition) &aSectionRef,
                                                         const Handle(StepElement_AnalysisItemWithinRepresentation) &aItem)
{

  theSectionRef = aSectionRef;

  theItem = aItem;
}

//=======================================================================
//function : SectionRef
//purpose  : 
//=======================================================================

Handle(StepElement_CurveElementSectionDefinition) StepFEA_FeaCurveSectionGeometricRelationship::SectionRef () const
{
  return theSectionRef;
}

//=======================================================================
//function : SetSectionRef
//purpose  : 
//=======================================================================

void StepFEA_FeaCurveSectionGeometricRelationship::SetSectionRef (const Handle(StepElement_CurveElementSectionDefinition) &aSectionRef)
{
  theSectionRef = aSectionRef;
}

//=======================================================================
//function : Item
//purpose  : 
//=======================================================================

Handle(StepElement_AnalysisItemWithinRepresentation) StepFEA_FeaCurveSectionGeometricRelationship::Item () const
{
  return theItem;
}

//=======================================================================
//function : SetItem
//purpose  : 
//=======================================================================

void StepFEA_FeaCurveSectionGeometricRelationship::SetItem (const Handle(StepElement_AnalysisItemWithinRepresentation) &aItem)
{
  theItem = aItem;
}
