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

#include <RWStepDimTol_RWGeometricToleranceWithDatumReference.ixx>
#include <StepDimTol_HArray1OfDatumReference.hxx>
#include <StepDimTol_DatumReference.hxx>

//=======================================================================
//function : RWStepDimTol_RWGeometricToleranceWithDatumReference
//purpose  : 
//=======================================================================

RWStepDimTol_RWGeometricToleranceWithDatumReference::RWStepDimTol_RWGeometricToleranceWithDatumReference ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceWithDatumReference::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                    const Standard_Integer num,
                                                                    Handle(Interface_Check)& ach,
                                                                    const Handle(StepDimTol_GeometricToleranceWithDatumReference) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"geometric_tolerance_with_datum_reference") ) return;

  // Inherited fields of GeometricTolerance

  Handle(TCollection_HAsciiString) aGeometricTolerance_Name;
  data->ReadString (num, 1, "geometric_tolerance.name", ach, aGeometricTolerance_Name);

  Handle(TCollection_HAsciiString) aGeometricTolerance_Description;
  data->ReadString (num, 2, "geometric_tolerance.description", ach, aGeometricTolerance_Description);

  Handle(StepBasic_MeasureWithUnit) aGeometricTolerance_Magnitude;
  data->ReadEntity (num, 3, "geometric_tolerance.magnitude", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aGeometricTolerance_Magnitude);

  Handle(StepRepr_ShapeAspect) aGeometricTolerance_TolerancedShapeAspect;
  data->ReadEntity (num, 4, "geometric_tolerance.toleranced_shape_aspect", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aGeometricTolerance_TolerancedShapeAspect);

  // Own fields of GeometricToleranceWithDatumReference

  Handle(StepDimTol_HArray1OfDatumReference) aDatumSystem;
  Standard_Integer sub5 = 0;
  if ( data->ReadSubList (num, 5, "datum_system", ach, sub5) ) {
    Standard_Integer nb0 = data->NbParams(sub5);
    aDatumSystem = new StepDimTol_HArray1OfDatumReference (1, nb0);
    Standard_Integer num2 = sub5;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepDimTol_DatumReference) anIt0;
      data->ReadEntity (num2, i0, "datum_reference", ach, STANDARD_TYPE(StepDimTol_DatumReference), anIt0);
      aDatumSystem->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aGeometricTolerance_Name,
            aGeometricTolerance_Description,
            aGeometricTolerance_Magnitude,
            aGeometricTolerance_TolerancedShapeAspect,
            aDatumSystem);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceWithDatumReference::WriteStep (StepData_StepWriter& SW,
                                                                     const Handle(StepDimTol_GeometricToleranceWithDatumReference) &ent) const
{

  // Inherited fields of GeometricTolerance

  SW.Send (ent->StepDimTol_GeometricTolerance::Name());

  SW.Send (ent->StepDimTol_GeometricTolerance::Description());

  SW.Send (ent->StepDimTol_GeometricTolerance::Magnitude());

  SW.Send (ent->StepDimTol_GeometricTolerance::TolerancedShapeAspect());

  // Own fields of GeometricToleranceWithDatumReference

  SW.OpenSub();
  for (Standard_Integer i4=1; i4 <= ent->DatumSystem()->Length(); i4++ ) {
    Handle(StepDimTol_DatumReference) Var0 = ent->DatumSystem()->Value(i4);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceWithDatumReference::Share (const Handle(StepDimTol_GeometricToleranceWithDatumReference) &ent,
                                                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of GeometricTolerance

  iter.AddItem (ent->StepDimTol_GeometricTolerance::Magnitude());

  iter.AddItem (ent->StepDimTol_GeometricTolerance::TolerancedShapeAspect());

  // Own fields of GeometricToleranceWithDatumReference

  for (Standard_Integer i3=1; i3 <= ent->DatumSystem()->Length(); i3++ ) {
    Handle(StepDimTol_DatumReference) Var0 = ent->DatumSystem()->Value(i3);
    iter.AddItem (Var0);
  }
}
