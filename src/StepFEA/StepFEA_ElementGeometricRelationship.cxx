// File:	StepFEA_ElementGeometricRelationship.cxx
// Created:	Tue Feb  4 10:39:08 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
