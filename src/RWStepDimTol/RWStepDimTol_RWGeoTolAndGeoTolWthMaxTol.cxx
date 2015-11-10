// Created on: 2015-08-10
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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
#include <RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepDimTol_GeometricToleranceType.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthMaxTol.hxx>
#include <StepDimTol_GeometricToleranceWithModifiers.hxx>

//=======================================================================
//function : RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol
//purpose  : 
//=======================================================================
RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol::RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol()
{
}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num0, Handle(Interface_Check)& ach,
   const Handle(StepDimTol_GeoTolAndGeoTolWthMaxTol)& ent) const
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
  
  data->NamedForComplex("GEOMETRIC_TOLERANCE_WITH_MAXIMUM_TOLERANCE",num0,num,ach);
  Handle(StepBasic_LengthMeasureWithUnit) aMaxTol;
  data->ReadEntity (num, 1, "maximum_upper_tolerance", ach, STANDARD_TYPE(StepBasic_LengthMeasureWithUnit), aMaxTol);

  data->NamedForComplex("GEOMETRIC_TOLERANCE_WITH_MODIFIERS",num0,num,ach);
  // Own fields of ModifiedGeometricTolerance
  Handle(StepDimTol_HArray1OfGeometricToleranceModifier) aModifiers;
  Standard_Integer sub = 0;
  if ( data->ReadSubList (num, 1, "modifiers", ach, sub) ) {
    Standard_Integer nb0 = data->NbParams(sub);
    aModifiers = new StepDimTol_HArray1OfGeometricToleranceModifier (1, nb0);
    Standard_Integer num2 = sub;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepDimTol_GeometricToleranceModifier anIt0 = StepDimTol_GTMMaximumMaterialRequirement;
      if (data->ParamType (num2, i0) == Interface_ParamEnum) {
        Standard_CString text = data->ParamCValue(num2, i0);
        if      (strcmp(text, ".ANY_CROSS_SECTION.")==0) anIt0 = StepDimTol_GTMAnyCrossSection;
        else if (strcmp(text, ".COMMON_ZONE.")==0) anIt0 = StepDimTol_GTMCommonZone;
        else if (strcmp(text, ".EACH_RADIAL_ELEMENT.")==0) anIt0 = StepDimTol_GTMEachRadialElement;
        else if (strcmp(text, ".FREE_STATE.")==0) anIt0 = StepDimTol_GTMFreeState;
        else if (strcmp(text, ".LEAST_MATERIAL_REQUIREMENT.")==0) anIt0 = StepDimTol_GTMLeastMaterialRequirement;
        else if (strcmp(text, ".LINE_ELEMENT.")==0) anIt0 = StepDimTol_GTMLineElement;
        else if (strcmp(text, ".MAJOR_DIAMETER.")==0) anIt0 = StepDimTol_GTMMajorDiameter;
        else if (strcmp(text, ".MAXIMUM_MATERIAL_REQUIREMENT.")==0) anIt0 = StepDimTol_GTMMaximumMaterialRequirement;
        else if (strcmp(text, ".MINOR_DIAMETER.")==0) anIt0 = StepDimTol_GTMMinorDiameter;
        else if (strcmp(text, ".NOT_CONVEX.")==0) anIt0 = StepDimTol_GTMNotConvex;
        else if (strcmp(text, ".PITCH_DIAMETER.")==0) anIt0 = StepDimTol_GTMPitchDiameter;
        else if (strcmp(text, ".RECIPROCITY_REQUIREMENT.")==0) anIt0 = StepDimTol_GTMReciprocityRequirement;
        else if (strcmp(text, ".SEPARATE_REQUIREMENT.")==0) anIt0 = StepDimTol_GTMSeparateRequirement;
        else if (strcmp(text, ".STATISTICAL_TOLERANCE.")==0) anIt0 = StepDimTol_GTMStatisticalTolerance;
        else if (strcmp(text, ".TANGENT_PLANE.")==0) anIt0 = StepDimTol_GTMTangentPlane;
        else ach->AddFail("Parameter #5 (modifiers) has not allowed value");
      }
      else ach->AddFail("Parameter #5 (modifier) is not set of enumerations");
      aModifiers->SetValue(i0, anIt0);
    }
  }
  Handle(StepDimTol_GeometricToleranceWithModifiers) aGTWM = new StepDimTol_GeometricToleranceWithModifiers;
  aGTWM->SetModifiers(aModifiers);

  //Choose type of geometric tolerance
  TColStd_SequenceOfAsciiString aTypes;
  data->ComplexType(num0, aTypes);
  Standard_CString aFirst = aTypes.First().ToCString();
  Standard_CString aLast = aTypes.Last().ToCString();
  StepDimTol_GeometricToleranceType aType = StepDimTol_GTTPositionTolerance;
  if (strcmp(aFirst, "ANGULARITY_TOLERANCE") == 0) aType = StepDimTol_GTTAngularityTolerance;
  else if (strcmp(aFirst, "CIRCULAR_RUNOUT_TOLERANCE") == 0) aType = StepDimTol_GTTCircularRunoutTolerance;
  else if (strcmp(aFirst, "COAXIALITY_TOLERANCE") == 0) aType = StepDimTol_GTTCoaxialityTolerance;
  else if (strcmp(aFirst, "CONCENTRICITY_TOLERANCE") == 0) aType = StepDimTol_GTTConcentricityTolerance;
  else if (strcmp(aFirst, "CYLINDRICITY_TOLERANCE") == 0) aType = StepDimTol_GTTCylindricityTolerance;
  else if (strcmp(aFirst, "FLATNESS_TOLERANCE") == 0) aType = StepDimTol_GTTFlatnessTolerance;
  else if (strcmp(aLast, "LINE_PROFILE_TOLERANCE") == 0) aType = StepDimTol_GTTLineProfileTolerance;
  else if (strcmp(aLast, "PARALLELISM_TOLERANCE") == 0) aType = StepDimTol_GTTParallelismTolerance;
  else if (strcmp(aLast, "PERPENDICULARITY_TOLERANCE") == 0) aType = StepDimTol_GTTPerpendicularityTolerance;
  else if (strcmp(aLast, "POSITION_TOLERANCE") == 0) aType = StepDimTol_GTTPositionTolerance;
  else if (strcmp(aLast, "ROUNDNESS_TOLERANCE") == 0) aType = StepDimTol_GTTRoundnessTolerance;
  else if (strcmp(aLast, "STRAIGHTNESS_TOLERANCE") == 0) aType = StepDimTol_GTTStraightnessTolerance;
  else if (strcmp(aLast, "SURFACE_PROFILE_TOLERANCE") == 0) aType = StepDimTol_GTTSurfaceProfileTolerance;
  else if (strcmp(aLast, "SYMMETRY_TOLERANCE") == 0) aType = StepDimTol_GTTSymmetryTolerance;
  else if (strcmp(aLast, "TOTAL_RUNOUT_TOLERANCE") == 0) aType = StepDimTol_GTTTotalRunoutTolerance;
  else ach->AddFail("The type of geometric tolerance is not supported");

  // Initialize entity
  ent->Init(aName, aDescription, aMagnitude, aTolerancedShapeAspect, aGTWM, aMaxTol, aType);

}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepDimTol_GeoTolAndGeoTolWthMaxTol)& ent) const
{
  StepDimTol_GeometricToleranceType aType = ent->GetToleranceType();
  if (aType == StepDimTol_GTTAngularityTolerance)
    SW.StartEntity("ANGULARITY_TOLERANCE");
  else if (aType == StepDimTol_GTTCircularRunoutTolerance)
    SW.StartEntity("CIRCULAR_RUNOUT_TOLERANCE");
  else if (aType == StepDimTol_GTTCoaxialityTolerance)
    SW.StartEntity("COAXIALITY_TOLERANCE");
  else if (aType == StepDimTol_GTTConcentricityTolerance)
    SW.StartEntity("CONCENTRICITY_TOLERANCE");
  else if (aType == StepDimTol_GTTCylindricityTolerance)
    SW.StartEntity("CYLINDRICITY_TOLERANCE");
  else if (aType == StepDimTol_GTTFlatnessTolerance)
    SW.StartEntity("FLATNESS_TOLERANCE");

  SW.StartEntity("GEOMETRIC_TOLERANCE");
  SW.Send(ent->Name());
  SW.Send(ent->Description());
  SW.Send(ent->Magnitude());
  SW.Send(ent->TolerancedShapeAspect().Value());
  SW.StartEntity("GEOMETRIC_TOLERANCE_WITH_MAXIMUM_TOLERANCE");
  SW.Send(ent->GetMaxTolerance());
  SW.StartEntity("GEOMETRIC_TOLERANCE_WITH_MODIFIERS");
  SW.OpenSub();
  Handle(StepDimTol_GeometricToleranceWithModifiers) aGTWM = ent->GetGeometricToleranceWithModifiers();
  for (Standard_Integer i = 1;  i <= aGTWM->NbModifiers();  i++) {
    switch (aGTWM->ModifierValue(i)) {
      case StepDimTol_GTMAnyCrossSection: SW.SendEnum (".ANY_CROSS_SECTION."); break;
      case StepDimTol_GTMCommonZone: SW.SendEnum (".COMMON_ZONE."); break;
      case StepDimTol_GTMEachRadialElement: SW.SendEnum (".EACH_RADIAL_ELEMENT."); break;
      case StepDimTol_GTMFreeState: SW.SendEnum (".FREE_STATE."); break;
      case StepDimTol_GTMLeastMaterialRequirement: SW.SendEnum (".LEAST_MATERIAL_REQUIREMENT."); break;
      case StepDimTol_GTMLineElement: SW.SendEnum (".LINE_ELEMENT."); break;
      case StepDimTol_GTMMajorDiameter: SW.SendEnum (".MAJOR_DIAMETER."); break;
      case StepDimTol_GTMMaximumMaterialRequirement: SW.SendEnum (".MAXIMUM_MATERIAL_REQUIREMENT."); break;
      case StepDimTol_GTMMinorDiameter: SW.SendEnum (".MINOR_DIAMETER."); break;
      case StepDimTol_GTMNotConvex: SW.SendEnum (".NOT_CONVEX."); break;
      case StepDimTol_GTMPitchDiameter: SW.SendEnum (".PITCH_DIAMETER."); break;
      case StepDimTol_GTMReciprocityRequirement: SW.SendEnum (".RECIPROCITY_REQUIREMENT."); break;
      case StepDimTol_GTMSeparateRequirement: SW.SendEnum (".SEPARATE_REQUIREMENT."); break;
      case StepDimTol_GTMStatisticalTolerance: SW.SendEnum (".STATISTICAL_TOLERANCE."); break;
      case StepDimTol_GTMTangentPlane: SW.SendEnum (".TANGENT_PLANE."); break;
    }
  }
  SW.CloseSub();
  if (aType == StepDimTol_GTTLineProfileTolerance)
    SW.StartEntity("LINE_PROFILE_TOLERANCE");
  else if (aType == StepDimTol_GTTParallelismTolerance)
    SW.StartEntity("PARALLELISM_TOLERANCE");
  else if (aType == StepDimTol_GTTPerpendicularityTolerance)
    SW.StartEntity("PERPENDICULARITY_TOLERANCE");
  else if (aType == StepDimTol_GTTPositionTolerance)
    SW.StartEntity("POSITION_TOLERANCE");
  else if (aType == StepDimTol_GTTRoundnessTolerance)
    SW.StartEntity("ROUNDNESS_TOLERANCE");
  else if (aType == StepDimTol_GTTStraightnessTolerance)
    SW.StartEntity("STRAIGHTNESS_TOLERANCE");
  else if (aType == StepDimTol_GTTSurfaceProfileTolerance)
    SW.StartEntity("SURFACE_PROFILE_TOLERANCE");
  else if (aType == StepDimTol_GTTSymmetryTolerance)
    SW.StartEntity("SYMMETRY_TOLERANCE");
  else if (aType == StepDimTol_GTTTotalRunoutTolerance)
    SW.StartEntity("TOTAL_RUNOUT_TOLERANCE");
}


//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol::Share
  (const Handle(StepDimTol_GeoTolAndGeoTolWthMaxTol)& ent,
   Interface_EntityIterator& iter) const
{
  // Own fields of GeometricTolerance
  iter.AddItem (ent->Magnitude());
  iter.AddItem (ent->TolerancedShapeAspect().Value());
}
