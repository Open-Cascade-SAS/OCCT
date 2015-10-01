// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_DatumReference.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <StepDimTol_HArray1OfDatumSystemOrReference.hxx>
#include <StepDimTol_LimitCondition.hxx>
#include <StepDimTol_ModifiedGeometricTolerance.hxx>

//=======================================================================
//function : RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol
//purpose  : 
//=======================================================================
RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol()
{
}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num0, Handle(Interface_Check)& ach,
   const Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol)& ent) const
{
  Standard_Integer num = 0;//num0;
  data->NamedForComplex("GEOMETRIC_TOLERANCE","GMTTLR",num0,num,ach);
  if (!data->CheckNbParams(num,4,ach,"geometric_tolerance")) return;
  // Own fields of GeometricTolerance
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);
  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);
  Handle(StepBasic_MeasureWithUnit) aMagnitude;
  data->ReadEntity (num, 3, "magnitude", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aMagnitude);
  StepDimTol_GeometricToleranceTarget aTolerancedShapeAspect;
  data->ReadEntity (num, 4, "toleranced_shape_aspect", ach, aTolerancedShapeAspect);

  data->NamedForComplex("GEOMETRIC_TOLERANCE_WITH_DATUM_REFERENCE","GTWDR",num0,num,ach);
  // Own fields of GeometricToleranceWithDatumReference
  Handle(StepDimTol_HArray1OfDatumSystemOrReference) aDatumSystem;
  Standard_Integer sub5 = 0;
  if ( data->ReadSubList (num, 1, "datum_system", ach, sub5) ) {
    Standard_Integer nb0 = data->NbParams(sub5);
    aDatumSystem = new StepDimTol_HArray1OfDatumSystemOrReference (1, nb0);
    Standard_Integer num2 = sub5;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepDimTol_DatumSystemOrReference anIt0;
      data->ReadEntity (num2, i0, "datum_system_or_reference", ach, anIt0);
      aDatumSystem->SetValue(i0, anIt0);
    }
  }
  // Initialize entity
  Handle(StepDimTol_GeometricToleranceWithDatumReference) GTWDR =
    new StepDimTol_GeometricToleranceWithDatumReference;
  GTWDR->SetDatumSystem(aDatumSystem);

  data->NamedForComplex("MODIFIED_GEOMETRIC_TOLERANCE","MDGMTL",num0,num,ach);
  // Own fields of ModifiedGeometricTolerance
  StepDimTol_LimitCondition aModifier = StepDimTol_MaximumMaterialCondition;
  if (data->ParamType (num, 1) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 1);
    if      (strcmp(text, ".MAXIMUM_MATERIAL_CONDITION.")==0) aModifier = StepDimTol_MaximumMaterialCondition;
    else if (strcmp(text, ".LEAST_MATERIAL_CONDITION.")==0) aModifier = StepDimTol_LeastMaterialCondition;
    else if (strcmp(text, ".REGARDLESS_OF_FEATURE_SIZE.")==0) aModifier = StepDimTol_RegardlessOfFeatureSize;
    else ach->AddFail("Parameter in MODIFIED_GEOMETRIC_TOLERANCE has not allowed value");
  }
  else ach->AddFail("Parameter in MODIFIED_GEOMETRIC_TOLERANCE is not enumeration");
  Handle(StepDimTol_ModifiedGeometricTolerance) MGT = new StepDimTol_ModifiedGeometricTolerance;
  MGT->SetModifier(aModifier);

  // Initialize entity
  ent->Init(aName, aDescription, aMagnitude, aTolerancedShapeAspect, GTWDR, MGT);

}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol)& ent) const
{
  SW.StartEntity("GEOMETRIC_TOLERANCE");
  SW.Send(ent->Name());
  SW.Send(ent->Description());
  SW.Send(ent->Magnitude());
  SW.Send(ent->TolerancedShapeAspect().Value());
  SW.StartEntity("GEOMETRIC_TOLERANCE_WITH_DATUM_REFERENCE");
  SW.OpenSub();
  for(Standard_Integer i4=1; i4<=ent->GetGeometricToleranceWithDatumReference()->DatumSystemAP242()->Length(); i4++) {
    StepDimTol_DatumSystemOrReference Var0 =
      ent->GetGeometricToleranceWithDatumReference()->DatumSystemAP242()->Value(i4);
    SW.Send(Var0.Value());
  }
  SW.CloseSub();
  SW.StartEntity("MODIFIED_GEOMETRIC_TOLERANCE");
  switch (ent->GetModifiedGeometricTolerance()->Modifier()) {
    case StepDimTol_MaximumMaterialCondition: SW.SendEnum(".MAXIMUM_MATERIAL_CONDITION."); break;
    case StepDimTol_LeastMaterialCondition: SW.SendEnum(".LEAST_MATERIAL_CONDITION."); break;
    case StepDimTol_RegardlessOfFeatureSize: SW.SendEnum(".REGARDLESS_OF_FEATURE_SIZE."); break;
  }
  SW.StartEntity("POSITION_TOLERANCE");
}


//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::Share
  (const Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol)& ent,
   Interface_EntityIterator& iter) const
{
  // Own fields of GeometricTolerance
  iter.AddItem (ent->Magnitude());
  iter.AddItem (ent->TolerancedShapeAspect().Value());
  // Own fields of GeometricToleranceWithDatumReference
  for (Standard_Integer i3=1; i3<=ent->GetGeometricToleranceWithDatumReference()->DatumSystemAP242()->Length(); i3++ ) {
    StepDimTol_DatumSystemOrReference Var0 = ent->GetGeometricToleranceWithDatumReference()->DatumSystemAP242()->Value(i3);
    iter.AddItem (Var0.Value());
  }
}
