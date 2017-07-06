// Created on: 2003-06-04
// Created by: Galina KULIKOVA
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
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_GeometricToleranceRelationship,Standard_Transient)

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

void StepDimTol_GeometricToleranceRelationship::Init (const Handle(TCollection_HAsciiString) &theName,
                                                      const Handle(TCollection_HAsciiString) &theDescription,
                                                      const Handle(StepDimTol_GeometricTolerance) &theRelatingGeometricTolerance,
                                                      const Handle(StepDimTol_GeometricTolerance) &theRelatedGeometricTolerance)
{

  myName = theName;

  myDescription = theDescription;

  myRelatingGeometricTolerance = theRelatingGeometricTolerance;

  myRelatedGeometricTolerance = theRelatedGeometricTolerance;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_GeometricToleranceRelationship::Name () const
{
  return myName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceRelationship::SetName (const Handle(TCollection_HAsciiString) &theName)
{
  myName = theName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_GeometricToleranceRelationship::Description () const
{
  return myDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceRelationship::SetDescription (const Handle(TCollection_HAsciiString) &theDescription)
{
  myDescription = theDescription;
}

//=======================================================================
//function : RelatingGeometricTolerance
//purpose  : 
//=======================================================================

Handle(StepDimTol_GeometricTolerance) StepDimTol_GeometricToleranceRelationship::RelatingGeometricTolerance () const
{
  return myRelatingGeometricTolerance;
}

//=======================================================================
//function : SetRelatingGeometricTolerance
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceRelationship::SetRelatingGeometricTolerance (const Handle(StepDimTol_GeometricTolerance) &theRelatingGeometricTolerance)
{
  myRelatingGeometricTolerance = theRelatingGeometricTolerance;
}

//=======================================================================
//function : RelatedGeometricTolerance
//purpose  : 
//=======================================================================

Handle(StepDimTol_GeometricTolerance) StepDimTol_GeometricToleranceRelationship::RelatedGeometricTolerance () const
{
  return myRelatedGeometricTolerance;
}

//=======================================================================
//function : SetRelatedGeometricTolerance
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceRelationship::SetRelatedGeometricTolerance (const Handle(StepDimTol_GeometricTolerance) &theRelatedGeometricTolerance)
{
  myRelatedGeometricTolerance = theRelatedGeometricTolerance;
}
