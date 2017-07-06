// Created on: 2003-01-22
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

#include <Standard_Type.hxx>
#include <StepElement_AnalysisItemWithinRepresentation.hxx>
#include <StepElement_SurfaceSection.hxx>
#include <StepFEA_FeaSurfaceSectionGeometricRelationship.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_FeaSurfaceSectionGeometricRelationship,Standard_Transient)

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
