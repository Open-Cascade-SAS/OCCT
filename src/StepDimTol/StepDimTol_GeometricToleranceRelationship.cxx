// File:	StepDimTol_GeometricToleranceRelationship.cxx
// Created:	Wed Jun  4 13:34:35 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepDimTol_GeometricToleranceRelationship.ixx>

//=======================================================================
//function : StepDimTol_GeometricToleranceRelationship
//purpose  : 
//=======================================================================

StepDimTol_GeometricToleranceRelationship::StepDimTol_GeometricToleranceRelationship ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceRelationship::Init (const Handle(TCollection_HAsciiString) &aName,
                                                      const Handle(TCollection_HAsciiString) &aDescription,
                                                      const Handle(StepDimTol_GeometricTolerance) &aRelatingGeometricTolerance,
                                                      const Handle(StepDimTol_GeometricTolerance) &aRelatedGeometricTolerance)
{

  theName = aName;

  theDescription = aDescription;

  theRelatingGeometricTolerance = aRelatingGeometricTolerance;

  theRelatedGeometricTolerance = aRelatedGeometricTolerance;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_GeometricToleranceRelationship::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceRelationship::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_GeometricToleranceRelationship::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceRelationship::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : RelatingGeometricTolerance
//purpose  : 
//=======================================================================

Handle(StepDimTol_GeometricTolerance) StepDimTol_GeometricToleranceRelationship::RelatingGeometricTolerance () const
{
  return theRelatingGeometricTolerance;
}

//=======================================================================
//function : SetRelatingGeometricTolerance
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceRelationship::SetRelatingGeometricTolerance (const Handle(StepDimTol_GeometricTolerance) &aRelatingGeometricTolerance)
{
  theRelatingGeometricTolerance = aRelatingGeometricTolerance;
}

//=======================================================================
//function : RelatedGeometricTolerance
//purpose  : 
//=======================================================================

Handle(StepDimTol_GeometricTolerance) StepDimTol_GeometricToleranceRelationship::RelatedGeometricTolerance () const
{
  return theRelatedGeometricTolerance;
}

//=======================================================================
//function : SetRelatedGeometricTolerance
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceRelationship::SetRelatedGeometricTolerance (const Handle(StepDimTol_GeometricTolerance) &aRelatedGeometricTolerance)
{
  theRelatedGeometricTolerance = aRelatedGeometricTolerance;
}
