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

#include <RWStepFEA_RWElementGeometricRelationship.ixx>

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
