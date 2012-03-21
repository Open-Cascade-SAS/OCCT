// Created on: 2003-06-04
// Created by: Galina KULIKOVA
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

#include <RWStepDimTol_RWGeometricToleranceRelationship.ixx>

//=======================================================================
//function : RWStepDimTol_RWGeometricToleranceRelationship
//purpose  : 
//=======================================================================

RWStepDimTol_RWGeometricToleranceRelationship::RWStepDimTol_RWGeometricToleranceRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                              const Standard_Integer num,
                                                              Handle(Interface_Check)& ach,
                                                              const Handle(StepDimTol_GeometricToleranceRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"geometric_tolerance_relationship") ) return;

  // Own fields of GeometricToleranceRelationship

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  Handle(StepDimTol_GeometricTolerance) aRelatingGeometricTolerance;
  data->ReadEntity (num, 3, "relating_geometric_tolerance", ach, STANDARD_TYPE(StepDimTol_GeometricTolerance), aRelatingGeometricTolerance);

  Handle(StepDimTol_GeometricTolerance) aRelatedGeometricTolerance;
  data->ReadEntity (num, 4, "related_geometric_tolerance", ach, STANDARD_TYPE(StepDimTol_GeometricTolerance), aRelatedGeometricTolerance);

  // Initialize entity
  ent->Init(aName,
            aDescription,
            aRelatingGeometricTolerance,
            aRelatedGeometricTolerance);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceRelationship::WriteStep (StepData_StepWriter& SW,
                                                               const Handle(StepDimTol_GeometricToleranceRelationship) &ent) const
{

  // Own fields of GeometricToleranceRelationship

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->RelatingGeometricTolerance());

  SW.Send (ent->RelatedGeometricTolerance());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceRelationship::Share (const Handle(StepDimTol_GeometricToleranceRelationship) &ent,
                                                           Interface_EntityIterator& iter) const
{

  // Own fields of GeometricToleranceRelationship

  iter.AddItem (ent->RelatingGeometricTolerance());

  iter.AddItem (ent->RelatedGeometricTolerance());
}
