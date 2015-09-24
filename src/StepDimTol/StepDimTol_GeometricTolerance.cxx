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
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <TCollection_HAsciiString.hxx>

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
                                          const StepDimTol_GeometricToleranceTarget &aTolerancedShapeAspect)
{

  theName = aName;

  theDescription = aDescription;

  theMagnitude = aMagnitude;

  theTolerancedShapeAspect = aTolerancedShapeAspect;
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

  theTolerancedShapeAspect.SetValue(aTolerancedShapeAspect);
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

StepDimTol_GeometricToleranceTarget StepDimTol_GeometricTolerance::TolerancedShapeAspect () const
{
  return theTolerancedShapeAspect;
}

//=======================================================================
//function : SetTolerancedShapeAspect
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetTolerancedShapeAspect (const Handle(StepRepr_ShapeAspect) &aTolerancedShapeAspect)
{
  theTolerancedShapeAspect.SetValue(aTolerancedShapeAspect);
}

//=======================================================================
//function : SetTolerancedShapeAspect
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetTolerancedShapeAspect (const StepDimTol_GeometricToleranceTarget &aTolerancedShapeAspect)
{
  theTolerancedShapeAspect = aTolerancedShapeAspect;
}
