// Created on: 2003-02-04
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

#include <StepFEA_ElementGeometricRelationship.ixx>

//=======================================================================
//function : StepFEA_ElementGeometricRelationship
//purpose  : 
//=======================================================================

StepFEA_ElementGeometricRelationship::StepFEA_ElementGeometricRelationship ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_ElementGeometricRelationship::Init (const StepFEA_ElementOrElementGroup &aElementRef,
                                                 const Handle(StepElement_AnalysisItemWithinRepresentation) &aItem,
                                                 const StepElement_ElementAspect &aAspect)
{

  theElementRef = aElementRef;

  theItem = aItem;

  theAspect = aAspect;
}

//=======================================================================
//function : ElementRef
//purpose  : 
//=======================================================================

StepFEA_ElementOrElementGroup StepFEA_ElementGeometricRelationship::ElementRef () const
{
  return theElementRef;
}

//=======================================================================
//function : SetElementRef
//purpose  : 
//=======================================================================

void StepFEA_ElementGeometricRelationship::SetElementRef (const StepFEA_ElementOrElementGroup &aElementRef)
{
  theElementRef = aElementRef;
}

//=======================================================================
//function : Item
//purpose  : 
//=======================================================================

Handle(StepElement_AnalysisItemWithinRepresentation) StepFEA_ElementGeometricRelationship::Item () const
{
  return theItem;
}

//=======================================================================
//function : SetItem
//purpose  : 
//=======================================================================

void StepFEA_ElementGeometricRelationship::SetItem (const Handle(StepElement_AnalysisItemWithinRepresentation) &aItem)
{
  theItem = aItem;
}

//=======================================================================
//function : Aspect
//purpose  : 
//=======================================================================

StepElement_ElementAspect StepFEA_ElementGeometricRelationship::Aspect () const
{
  return theAspect;
}

//=======================================================================
//function : SetAspect
//purpose  : 
//=======================================================================

void StepFEA_ElementGeometricRelationship::SetAspect (const StepElement_ElementAspect &aAspect)
{
  theAspect = aAspect;
}
