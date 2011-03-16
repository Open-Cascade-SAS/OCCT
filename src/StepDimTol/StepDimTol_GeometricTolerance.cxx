// File:	StepDimTol_GeometricTolerance.cxx
// Created:	Wed Jun  4 13:34:35 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
