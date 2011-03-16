// File:	RWStepDimTol_RWGeometricTolerance.cxx
// Created:	Wed Jun  4 13:34:35 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepDimTol_RWGeometricTolerance.ixx>

//=======================================================================
//function : RWStepDimTol_RWGeometricTolerance
//purpose  : 
//=======================================================================

RWStepDimTol_RWGeometricTolerance::RWStepDimTol_RWGeometricTolerance ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricTolerance::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                  const Standard_Integer num,
                                                  Handle(Interface_Check)& ach,
                                                  const Handle(StepDimTol_GeometricTolerance) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"geometric_tolerance") ) return;

  // Own fields of GeometricTolerance

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  Handle(StepBasic_MeasureWithUnit) aMagnitude;
  data->ReadEntity (num, 3, "magnitude", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aMagnitude);

  Handle(StepRepr_ShapeAspect) aTolerancedShapeAspect;
  data->ReadEntity (num, 4, "toleranced_shape_aspect", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aTolerancedShapeAspect);

  // Initialize entity
  ent->Init(aName,
            aDescription,
            aMagnitude,
            aTolerancedShapeAspect);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricTolerance::WriteStep (StepData_StepWriter& SW,
                                                   const Handle(StepDimTol_GeometricTolerance) &ent) const
{

  // Own fields of GeometricTolerance

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->Magnitude());

  SW.Send (ent->TolerancedShapeAspect());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricTolerance::Share (const Handle(StepDimTol_GeometricTolerance) &ent,
                                               Interface_EntityIterator& iter) const
{

  // Own fields of GeometricTolerance

  iter.AddItem (ent->Magnitude());

  iter.AddItem (ent->TolerancedShapeAspect());
}
