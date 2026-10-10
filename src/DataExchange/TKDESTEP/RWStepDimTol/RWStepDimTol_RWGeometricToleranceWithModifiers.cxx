// Created on: 2015-07-07
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

#include "RWStepDimTol_RWGeometricToleranceWithModifiers.pxx"

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_GeometricToleranceWithModifiers.hxx>
#include <StepDimTol_GeometricToleranceModifier.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

RWStepDimTol_RWGeometricToleranceWithModifiers::RWStepDimTol_RWGeometricToleranceWithModifiers() =
  default;

//=================================================================================================

void RWStepDimTol_RWGeometricToleranceWithModifiers::ReadStep(
  const occ::handle<StepData_StepReaderData>&                    data,
  const int                                                      num,
  occ::handle<Interface_Check>&                                  ach,
  const occ::handle<StepDimTol_GeometricToleranceWithModifiers>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 5, ach, "geometric_tolerance_with_modifiers"))
  {
    return;
  }

  // inherited fields from GeometricTolerance

  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "geometric_tolerance.name", ach, aName);

  occ::handle<TCollection_HAsciiString> aDescription;
  data->ReadString(num, 2, "geometric_tolerance.description", ach, aDescription);

  occ::handle<Standard_Transient> aMagnitude;
  data->ReadEntity(num,
                   3,
                   "geometric_tolerance.magnitude",
                   ach,
                   STANDARD_TYPE(Standard_Transient),
                   aMagnitude);

  StepDimTol_GeometricToleranceTarget aTolerancedShapeAspect;
  data->ReadEntity(num,
                   4,
                   "geometric_tolerance.toleranced_shape_aspect",
                   ach,
                   aTolerancedShapeAspect);

  // own fields of GeometricToleranceWithModifiers
  occ::handle<NCollection_HArray1<StepDimTol_GeometricToleranceModifier>> aModifiers;
  int                                                                     sub5 = 0;
  if (data->ReadSubList(num, 5, "modifiers", ach, sub5))
  {
    int nb0    = data->NbParams(sub5);
    aModifiers = new NCollection_HArray1<StepDimTol_GeometricToleranceModifier>(1, nb0);
    int num2   = sub5;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      StepDimTol_GeometricToleranceModifier anIt0 = StepDimTol_GTMMaximumMaterialRequirement;
      if (data->ParamType(num2, i0) == Interface_ParamEnum)
      {
        const char* text = data->ParamCValue(num2, i0);
        if (!GetModifier(text, anIt0))
        {
          ach->AddFail("Parameter #5 (modifiers) has not allowed value");
        }
      }
      else
      {
        ach->AddFail("Parameter #5 (modifier) is not set of enumerations");
      }
      aModifiers->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aName, aDescription, aMagnitude, aTolerancedShapeAspect, aModifiers);
}

//=================================================================================================

void RWStepDimTol_RWGeometricToleranceWithModifiers::WriteStep(
  StepData_StepWriter&                                           SW,
  const occ::handle<StepDimTol_GeometricToleranceWithModifiers>& ent) const
{

  // inherited fields from GeometricTolerance

  SW.Send(ent->Name());

  SW.Send(ent->Description());

  SW.Send(ent->Magnitude());

  SW.Send(ent->TolerancedShapeAspect().Value());

  // own fields of GeometricToleranceWithModifiers

  SW.OpenSub();
  for (int i = 1; i <= ent->NbModifiers(); i++)
  {
    const char* aName = GetModifierName(ent->ModifierValue(i));
    if (aName != nullptr)
    {
      SW.SendEnum(aName);
    }
    else
    {
      SW.SendUndef();
    }
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepDimTol_RWGeometricToleranceWithModifiers::Share(
  const occ::handle<StepDimTol_GeometricToleranceWithModifiers>& ent,
  Interface_EntityIterator&                                      iter) const
{

  // inherited fields from GeometricTolerance

  iter.AddItem(ent->Magnitude());

  iter.AddItem(ent->TolerancedShapeAspect().Value());
}

//==================================================================================================

bool RWStepDimTol_RWGeometricToleranceWithModifiers::GetModifier(
  const char*                            theName,
  StepDimTol_GeometricToleranceModifier& theModifier)
{
  for (int anIndex = StepDimTol_GTMAnyCrossSection; anIndex <= StepDimTol_GTMTangentPlane;
       ++anIndex)
  {
    const StepDimTol_GeometricToleranceModifier aModifier =
      static_cast<StepDimTol_GeometricToleranceModifier>(anIndex);
    if (strcmp(theName, GetModifierName(aModifier)) == 0)
    {
      theModifier = aModifier;
      return true;
    }
  }
  return false;
}

//==================================================================================================

const char* RWStepDimTol_RWGeometricToleranceWithModifiers::GetModifierName(
  const StepDimTol_GeometricToleranceModifier theModifier)
{
  // Names follow the contiguous StepDimTol_GeometricToleranceModifier enumeration.
  static constexpr const char* THE_NAMES[] = {".ANY_CROSS_SECTION.",
                                              ".COMMON_ZONE.",
                                              ".EACH_RADIAL_ELEMENT.",
                                              ".FREE_STATE.",
                                              ".LEAST_MATERIAL_REQUIREMENT.",
                                              ".LINE_ELEMENT.",
                                              ".MAJOR_DIAMETER.",
                                              ".MAXIMUM_MATERIAL_REQUIREMENT.",
                                              ".MINOR_DIAMETER.",
                                              ".NOT_CONVEX.",
                                              ".PITCH_DIAMETER.",
                                              ".RECIPROCITY_REQUIREMENT.",
                                              ".SEPARATE_REQUIREMENT.",
                                              ".STATISTICAL_TOLERANCE.",
                                              ".TANGENT_PLANE."};
  return theModifier >= StepDimTol_GTMAnyCrossSection && theModifier <= StepDimTol_GTMTangentPlane
           ? THE_NAMES[theModifier]
           : nullptr;
}
