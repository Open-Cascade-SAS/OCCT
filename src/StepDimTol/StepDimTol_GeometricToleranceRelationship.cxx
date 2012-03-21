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
