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

#include <StepDimTol_GeometricTolerance.ixx>

//=======================================================================
//function : StepDimTol_GeometricTolerance
//purpose  : 
//=======================================================================

StepDimTol_GeometricTolerance::StepDimTol_GeometricTolerance ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::Init (const Handle(TCollection_HAsciiString) &aName,
                                          const Handle(TCollection_HAsciiString) &aDescription,
                                          const Handle(StepBasic_MeasureWithUnit) &aMagnitude,
                                          const Handle(StepRepr_ShapeAspect) &aTolerancedShapeAspect)
{

  theName = aName;

  theDescription = aDescription;

  theMagnitude = aMagnitude;

  theTolerancedShapeAspect = aTolerancedShapeAspect;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_GeometricTolerance::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_GeometricTolerance::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : Magnitude
//purpose  : 
//=======================================================================

Handle(StepBasic_MeasureWithUnit) StepDimTol_GeometricTolerance::Magnitude () const
{
  return theMagnitude;
}

//=======================================================================
//function : SetMagnitude
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetMagnitude (const Handle(StepBasic_MeasureWithUnit) &aMagnitude)
{
  theMagnitude = aMagnitude;
}

//=======================================================================
//function : TolerancedShapeAspect
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepDimTol_GeometricTolerance::TolerancedShapeAspect () const
{
  return theTolerancedShapeAspect;
}

//=======================================================================
//function : SetTolerancedShapeAspect
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetTolerancedShapeAspect (const Handle(StepRepr_ShapeAspect) &aTolerancedShapeAspect)
{
  theTolerancedShapeAspect = aTolerancedShapeAspect;
}
