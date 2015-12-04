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
#include <StepDimTol_ModifiedGeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_ModifiedGeometricTolerance,StepDimTol_GeometricTolerance)

//=======================================================================
//function : StepDimTol_ModifiedGeometricTolerance
//purpose  : 
//=======================================================================
StepDimTol_ModifiedGeometricTolerance::StepDimTol_ModifiedGeometricTolerance ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_ModifiedGeometricTolerance::Init (const Handle(TCollection_HAsciiString) &theGeometricTolerance_Name,
                                                  const Handle(TCollection_HAsciiString) &theGeometricTolerance_Description,
                                                  const Handle(StepBasic_MeasureWithUnit) &theGeometricTolerance_Magnitude,
                                                  const Handle(StepRepr_ShapeAspect) &theGeometricTolerance_TolerancedShapeAspect,
                                                  const StepDimTol_LimitCondition theModifier)
{
  StepDimTol_GeometricTolerance::Init(theGeometricTolerance_Name,
                                      theGeometricTolerance_Description,
                                      theGeometricTolerance_Magnitude,
                                      theGeometricTolerance_TolerancedShapeAspect);

  myModifier = theModifier;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_ModifiedGeometricTolerance::Init (const Handle(TCollection_HAsciiString) &theGeometricTolerance_Name,
                                                  const Handle(TCollection_HAsciiString) &theGeometricTolerance_Description,
                                                  const Handle(StepBasic_MeasureWithUnit) &theGeometricTolerance_Magnitude,
                                                  const StepDimTol_GeometricToleranceTarget &theGeometricTolerance_TolerancedShapeAspect,
                                                  const StepDimTol_LimitCondition theModifier)
{
  StepDimTol_GeometricTolerance::Init(theGeometricTolerance_Name,
                                      theGeometricTolerance_Description,
                                      theGeometricTolerance_Magnitude,
                                      theGeometricTolerance_TolerancedShapeAspect);

  myModifier = theModifier;
}

//=======================================================================
//function : Modifier
//purpose  : 
//=======================================================================

StepDimTol_LimitCondition StepDimTol_ModifiedGeometricTolerance::Modifier () const
{
  return myModifier;
}

//=======================================================================
//function : SetModifier
//purpose  : 
//=======================================================================

void StepDimTol_ModifiedGeometricTolerance::SetModifier (const StepDimTol_LimitCondition theModifier)
{
  myModifier = theModifier;
}
