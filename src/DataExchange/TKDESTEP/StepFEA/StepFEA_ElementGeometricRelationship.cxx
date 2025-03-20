// Created on: 2003-02-04
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepElement_AnalysisItemWithinRepresentation.hxx>
#include <StepFEA_ElementGeometricRelationship.hxx>
#include <StepFEA_ElementOrElementGroup.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_ElementGeometricRelationship, Standard_Transient)

//=================================================================================================

StepFEA_ElementGeometricRelationship::StepFEA_ElementGeometricRelationship() {}

//=================================================================================================

void StepFEA_ElementGeometricRelationship::Init(
  const StepFEA_ElementOrElementGroup&                        aElementRef,
  const Handle(StepElement_AnalysisItemWithinRepresentation)& aItem,
  const StepElement_ElementAspect&                            aAspect)
{

  theElementRef = aElementRef;

  theItem = aItem;

  theAspect = aAspect;
}

//=================================================================================================

StepFEA_ElementOrElementGroup StepFEA_ElementGeometricRelationship::ElementRef() const
{
  return theElementRef;
}

//=================================================================================================

void StepFEA_ElementGeometricRelationship::SetElementRef(
  const StepFEA_ElementOrElementGroup& aElementRef)
{
  theElementRef = aElementRef;
}

//=================================================================================================

Handle(StepElement_AnalysisItemWithinRepresentation) StepFEA_ElementGeometricRelationship::Item()
  const
{
  return theItem;
}

//=================================================================================================

void StepFEA_ElementGeometricRelationship::SetItem(
  const Handle(StepElement_AnalysisItemWithinRepresentation)& aItem)
{
  theItem = aItem;
}

//=================================================================================================

StepElement_ElementAspect StepFEA_ElementGeometricRelationship::Aspect() const
{
  return theAspect;
}

//=================================================================================================

void StepFEA_ElementGeometricRelationship::SetAspect(const StepElement_ElementAspect& aAspect)
{
  theAspect = aAspect;
}
