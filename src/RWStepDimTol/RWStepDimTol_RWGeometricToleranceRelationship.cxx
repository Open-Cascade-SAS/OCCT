// File:	RWStepDimTol_RWGeometricToleranceRelationship.cxx
// Created:	Wed Jun  4 13:34:35 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
