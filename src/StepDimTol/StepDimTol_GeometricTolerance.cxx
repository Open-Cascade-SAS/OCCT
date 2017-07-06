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

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_GeometricTolerance,Standard_Transient)

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

void StepDimTol_GeometricTolerance::Init (const Handle(TCollection_HAsciiString) &theName,
                                          const Handle(TCollection_HAsciiString) &theDescription,
                                          const Handle(StepBasic_MeasureWithUnit) &theMagnitude,
                                          const StepDimTol_GeometricToleranceTarget &theTolerancedShapeAspect)
{

  myName = theName;

  myDescription = theDescription;

  myMagnitude = theMagnitude;

  myTolerancedShapeAspect = theTolerancedShapeAspect;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::Init (const Handle(TCollection_HAsciiString) &theName,
                                          const Handle(TCollection_HAsciiString) &theDescription,
                                          const Handle(StepBasic_MeasureWithUnit) &theMagnitude,
                                          const Handle(StepRepr_ShapeAspect) &theTolerancedShapeAspect)
{

  myName = theName;

  myDescription = theDescription;

  myMagnitude = theMagnitude;

  myTolerancedShapeAspect.SetValue(theTolerancedShapeAspect);
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_GeometricTolerance::Name () const
{
  return myName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetName (const Handle(TCollection_HAsciiString) &theName)
{
  myName = theName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_GeometricTolerance::Description () const
{
  return myDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetDescription (const Handle(TCollection_HAsciiString) &theDescription)
{
  myDescription = theDescription;
}

//=======================================================================
//function : Magnitude
//purpose  : 
//=======================================================================

Handle(StepBasic_MeasureWithUnit) StepDimTol_GeometricTolerance::Magnitude () const
{
  return myMagnitude;
}

//=======================================================================
//function : SetMagnitude
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetMagnitude (const Handle(StepBasic_MeasureWithUnit) &theMagnitude)
{
  myMagnitude = theMagnitude;
}

//=======================================================================
//function : TolerancedShapeAspect
//purpose  : 
//=======================================================================

StepDimTol_GeometricToleranceTarget StepDimTol_GeometricTolerance::TolerancedShapeAspect () const
{
  return myTolerancedShapeAspect;
}

//=======================================================================
//function : SetTolerancedShapeAspect
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetTolerancedShapeAspect (const Handle(StepRepr_ShapeAspect) &theTolerancedShapeAspect)
{
  myTolerancedShapeAspect.SetValue(theTolerancedShapeAspect);
}

//=======================================================================
//function : SetTolerancedShapeAspect
//purpose  : 
//=======================================================================

void StepDimTol_GeometricTolerance::SetTolerancedShapeAspect (const StepDimTol_GeometricToleranceTarget &theTolerancedShapeAspect)
{
  myTolerancedShapeAspect = theTolerancedShapeAspect;
}
