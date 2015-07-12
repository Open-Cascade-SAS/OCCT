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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepFEA_RWElementGeometricRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_AnalysisItemWithinRepresentation.hxx>
#include <StepFEA_ElementGeometricRelationship.hxx>

//=======================================================================
//function : RWStepFEA_RWElementGeometricRelationship
//purpose  : 
//=======================================================================
RWStepFEA_RWElementGeometricRelationship::RWStepFEA_RWElementGeometricRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWElementGeometricRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                         const Standard_Integer num,
                                                         Handle(Interface_Check)& ach,
                                                         const Handle(StepFEA_ElementGeometricRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"element_geometric_relationship") ) return;

  // Own fields of ElementGeometricRelationship

  StepFEA_ElementOrElementGroup aElementRef;
  data->ReadEntity (num, 1, "element_ref", ach, aElementRef);

  Handle(StepElement_AnalysisItemWithinRepresentation) aItem;
  data->ReadEntity (num, 2, "item", ach, STANDARD_TYPE(StepElement_AnalysisItemWithinRepresentation), aItem);

  StepElement_ElementAspect aAspect;
  data->ReadEntity (num, 3, "aspect", ach, aAspect);

  // Initialize entity
  ent->Init(aElementRef,
            aItem,
            aAspect);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWElementGeometricRelationship::WriteStep (StepData_StepWriter& SW,
                                                          const Handle(StepFEA_ElementGeometricRelationship) &ent) const
{

  // Own fields of ElementGeometricRelationship

  SW.Send (ent->ElementRef().Value());

  SW.Send (ent->Item());

  SW.Send (ent->Aspect().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWElementGeometricRelationship::Share (const Handle(StepFEA_ElementGeometricRelationship) &ent,
                                                      Interface_EntityIterator& iter) const
{

  // Own fields of ElementGeometricRelationship

  iter.AddItem (ent->ElementRef().Value());

  iter.AddItem (ent->Item());

  iter.AddItem (ent->Aspect().Value());
}
