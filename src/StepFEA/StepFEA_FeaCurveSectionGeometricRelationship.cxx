// Created on: 2003-01-22
// Created by: data exchange team
// Copyright (c) 2003-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

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
