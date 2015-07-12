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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepFEA_RWFeaCurveSectionGeometricRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_AnalysisItemWithinRepresentation.hxx>
#include <StepElement_CurveElementSectionDefinition.hxx>
#include <StepFEA_FeaCurveSectionGeometricRelationship.hxx>

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
