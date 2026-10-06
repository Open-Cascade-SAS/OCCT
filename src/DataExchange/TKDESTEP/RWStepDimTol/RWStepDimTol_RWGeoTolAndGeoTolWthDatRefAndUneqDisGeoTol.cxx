// Created on: 2015-08-11
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
#include "RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.pxx"
#include "RWStepDimTol_RWGeometricToleranceWithModifiers.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.hxx>
#include <StepDimTol_DatumSystemOrReference.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepDimTol_UnequallyDisposedGeometricTolerance.hxx>

namespace
{
// Names follow StepDimTol_GeometricToleranceType and are alphabetically ordered.
static constexpr const char* THE_TOLERANCE_NAMES[] = {"ANGULARITY_TOLERANCE",
                                                      "CIRCULAR_RUNOUT_TOLERANCE",
                                                      "COAXIALITY_TOLERANCE",
                                                      "CONCENTRICITY_TOLERANCE",
                                                      "CYLINDRICITY_TOLERANCE",
                                                      "FLATNESS_TOLERANCE",
                                                      "LINE_PROFILE_TOLERANCE",
                                                      "PARALLELISM_TOLERANCE",
                                                      "PERPENDICULARITY_TOLERANCE",
                                                      "POSITION_TOLERANCE",
                                                      "ROUNDNESS_TOLERANCE",
                                                      "STRAIGHTNESS_TOLERANCE",
                                                      "SURFACE_PROFILE_TOLERANCE",
                                                      "SYMMETRY_TOLERANCE",
                                                      "TOTAL_RUNOUT_TOLERANCE"};
} // namespace

//=================================================================================================

RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::
  RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol() = default;

//=================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::ReadStep(
  const occ::handle<StepData_StepReaderData>&                             data,
  const int                                                               num0,
  occ::handle<Interface_Check>&                                           ach,
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol>& ent) const
{
  int num = 0; // num0;
  data->NamedForComplex("GEOMETRIC_TOLERANCE", "GMTTLR", num0, num, ach);
  if (!data->CheckNbParams(num, 4, ach, "geometric_tolerance"))
  {
    return;
  }
  // Own fields of GeometricTolerance
  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);
  occ::handle<TCollection_HAsciiString> aDescription;
  data->ReadString(num, 2, "description", ach, aDescription);
  occ::handle<Standard_Transient> aMagnitude;
  data->ReadEntity(num, 3, "magnitude", ach, STANDARD_TYPE(Standard_Transient), aMagnitude);
  StepDimTol_GeometricToleranceTarget aTolerancedShapeAspect;
  data->ReadEntity(num, 4, "toleranced_shape_aspect", ach, aTolerancedShapeAspect);

  NCollection_Sequence<TCollection_AsciiString> aTypes;
  data->ComplexType(num0, aTypes);
  const auto hasComponent = [&aTypes](const char* theName, const char* theShortName = "") {
    for (const auto& aName : aTypes)
    {
      if (aName.IsEqual(theName) || aName.IsEqual(theShortName))
      {
        return true;
      }
    }
    return false;
  };
  occ::handle<StepDimTol_GeometricToleranceWithDatumReference> aGTWDR;
  if (hasComponent("GEOMETRIC_TOLERANCE_WITH_DATUM_REFERENCE", "GTWDR"))
  {
    data->NamedForComplex("GEOMETRIC_TOLERANCE_WITH_DATUM_REFERENCE", "GTWDR", num0, num, ach);
    // Own fields of GeometricToleranceWithDatumReference
    occ::handle<NCollection_HArray1<StepDimTol_DatumSystemOrReference>> aDatumSystem;
    int                                                                 sub5 = 0;
    if (data->ReadSubList(num, 1, "datum_system", ach, sub5))
    {
      int nb0      = data->NbParams(sub5);
      aDatumSystem = new NCollection_HArray1<StepDimTol_DatumSystemOrReference>(1, nb0);
      int num2     = sub5;
      for (int i0 = 1; i0 <= nb0; i0++)
      {
        StepDimTol_DatumSystemOrReference anIt0;
        data->ReadEntity(num2, i0, "datum_system_or_reference", ach, anIt0);
        aDatumSystem->SetValue(i0, anIt0);
      }
    }
    // Initialize entity
    aGTWDR = new StepDimTol_GeometricToleranceWithDatumReference;
    aGTWDR->SetDatumSystem(aDatumSystem);
  }
  occ::handle<StepBasic_LengthMeasureWithUnit> aMaxTolerance;
  if (hasComponent("GEOMETRIC_TOLERANCE_WITH_MAXIMUM_TOLERANCE"))
  {
    data->NamedForComplex("GEOMETRIC_TOLERANCE_WITH_MAXIMUM_TOLERANCE", num0, num, ach);
    data->ReadEntity(num,
                     1,
                     "maximum_upper_tolerance",
                     ach,
                     STANDARD_TYPE(StepBasic_LengthMeasureWithUnit),
                     aMaxTolerance);
  }
  occ::handle<StepDimTol_GeometricToleranceWithModifiers> aWithModifiers;
  if (hasComponent("GEOMETRIC_TOLERANCE_WITH_MODIFIERS"))
  {
    data->NamedForComplex("GEOMETRIC_TOLERANCE_WITH_MODIFIERS", num0, num, ach);
    int aSubList = 0;
    if (data->ReadSubList(num, 1, "modifiers", ach, aSubList))
    {
      const int aCount = data->NbParams(aSubList);
      const occ::handle<NCollection_HArray1<StepDimTol_GeometricToleranceModifier>> aModifiers =
        new NCollection_HArray1<StepDimTol_GeometricToleranceModifier>(1, aCount);
      for (int anIndex = 1; anIndex <= aCount; ++anIndex)
      {
        const char*                           aModifier = data->ParamCValue(aSubList, anIndex);
        StepDimTol_GeometricToleranceModifier aValue    = StepDimTol_GTMMaximumMaterialRequirement;
        if (data->ParamType(aSubList, anIndex) != Interface_ParamEnum
            || !RWStepDimTol_RWGeometricToleranceWithModifiers::GetModifier(aModifier, aValue))
        {
          ach->AddFail("Unknown geometric tolerance modifier");
        }
        aModifiers->SetValue(anIndex, aValue);
      }
      aWithModifiers = new StepDimTol_GeometricToleranceWithModifiers();
      aWithModifiers->SetModifiers(aModifiers);
    }
  }
  data->NamedForComplex("UNEQUALLY_DISPOSED_GEOMETRIC_TOLERANCE", num0, num, ach);
  occ::handle<StepBasic_LengthMeasureWithUnit> aDisplacement;
  data->ReadEntity(num,
                   1,
                   "displacement",
                   ach,
                   STANDARD_TYPE(StepBasic_LengthMeasureWithUnit),
                   aDisplacement);
  // Initialize entity
  occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance> anUDGT =
    new StepDimTol_UnequallyDisposedGeometricTolerance;
  anUDGT->SetDisplacement(aDisplacement);

  // Choose the concrete tolerance constituent, independent of optional components.
  StepDimTol_GeometricToleranceType aType   = StepDimTol_GTTPositionTolerance;
  bool                              hasType = false;
  for (int anIndex = StepDimTol_GTTAngularityTolerance;
       anIndex <= StepDimTol_GTTTotalRunoutTolerance;
       ++anIndex)
  {
    if (hasComponent(THE_TOLERANCE_NAMES[anIndex]))
    {
      aType   = static_cast<StepDimTol_GeometricToleranceType>(anIndex);
      hasType = true;
      break;
    }
  }
  if (!hasType)
  {
    ach->AddFail("The type of geometric tolerance is not supported");
  }

  // Initialize entity
  ent->Init(aName, aDescription, aMagnitude, aTolerancedShapeAspect, aGTWDR, aType, anUDGT);
  ent->SetGeometricToleranceWithModifiers(aWithModifiers);
  ent->SetMaxTolerance(aMaxTolerance);
}

//=================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::WriteStep(
  StepData_StepWriter&                                                    SW,
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol>& ent) const
{
  const StepDimTol_GeometricToleranceType aType = ent->GetToleranceType();
  // STEP complex constituents must be written in alphabetical order.
  if (aType < StepDimTol_GTTLineProfileTolerance)
  {
    SW.StartEntity(THE_TOLERANCE_NAMES[aType]);
  }

  SW.StartEntity("GEOMETRIC_TOLERANCE");
  SW.Send(ent->Name());
  SW.Send(ent->Description());
  SW.Send(ent->Magnitude());
  SW.Send(ent->TolerancedShapeAspect().Value());
  if (!ent->GetGeometricToleranceWithDatumReference().IsNull())
  {
    SW.StartEntity("GEOMETRIC_TOLERANCE_WITH_DATUM_REFERENCE");
    SW.OpenSub();
    for (int i4 = 1;
         i4 <= ent->GetGeometricToleranceWithDatumReference()->DatumSystemAP242()->Length();
         i4++)
    {
      StepDimTol_DatumSystemOrReference Var0 =
        ent->GetGeometricToleranceWithDatumReference()->DatumSystemAP242()->Value(i4);
      SW.Send(Var0.Value());
    }
    SW.CloseSub();
  }
  if (!ent->GetMaxTolerance().IsNull())
  {
    SW.StartEntity("GEOMETRIC_TOLERANCE_WITH_MAXIMUM_TOLERANCE");
    SW.Send(ent->GetMaxTolerance());
  }
  if (!ent->GetGeometricToleranceWithModifiers().IsNull())
  {
    SW.StartEntity("GEOMETRIC_TOLERANCE_WITH_MODIFIERS");
    SW.OpenSub();
    const auto aModifiers = ent->GetGeometricToleranceWithModifiers()->Modifiers();
    if (!aModifiers.IsNull())
    {
      for (const auto aModifier : *aModifiers)
      {
        const char* aName =
          RWStepDimTol_RWGeometricToleranceWithModifiers::GetModifierName(aModifier);
        if (aName != nullptr)
        {
          SW.SendEnum(aName);
        }
        else
        {
          SW.SendUndef();
        }
      }
    }
    SW.CloseSub();
  }

  if (aType >= StepDimTol_GTTLineProfileTolerance)
  {
    SW.StartEntity(THE_TOLERANCE_NAMES[aType]);
  }

  SW.StartEntity("UNEQUALLY_DISPOSED_GEOMETRIC_TOLERANCE");
  SW.Send(ent->GetUnequallyDisposedGeometricTolerance()->Displacement());
}

//=================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::Share(
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol>& ent,
  Interface_EntityIterator&                                               iter) const
{
  // Own fields of GeometricTolerance
  iter.AddItem(ent->Magnitude());
  iter.AddItem(ent->TolerancedShapeAspect().Value());
  if (!ent->GetUnequallyDisposedGeometricTolerance().IsNull())
  {
    iter.AddItem(ent->GetUnequallyDisposedGeometricTolerance()->Displacement());
  }
  iter.AddItem(ent->GetMaxTolerance());
  if (!ent->GetGeometricToleranceWithDatumReference().IsNull())
  {
    // Own fields of GeometricToleranceWithDatumReference
    for (int i3 = 1;
         i3 <= ent->GetGeometricToleranceWithDatumReference()->DatumSystemAP242()->Length();
         i3++)
    {
      StepDimTol_DatumSystemOrReference Var0 =
        ent->GetGeometricToleranceWithDatumReference()->DatumSystemAP242()->Value(i3);
      iter.AddItem(Var0.Value());
    }
  }
}
