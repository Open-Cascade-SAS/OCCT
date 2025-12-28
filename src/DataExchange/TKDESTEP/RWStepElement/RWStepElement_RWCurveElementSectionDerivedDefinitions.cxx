// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include "RWStepElement_RWCurveElementSectionDerivedDefinitions.pxx"
#include <Standard_Real.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_CurveElementSectionDerivedDefinitions.hxx>
#include <StepElement_MeasureOrUnspecifiedValue.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepElement_MeasureOrUnspecifiedValue.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

RWStepElement_RWCurveElementSectionDerivedDefinitions::
  RWStepElement_RWCurveElementSectionDerivedDefinitions()
{
}

//=================================================================================================

void RWStepElement_RWCurveElementSectionDerivedDefinitions::ReadStep(
  const occ::handle<StepData_StepReaderData>&                           data,
  const int                                           num,
  occ::handle<Interface_Check>&                                         ach,
  const occ::handle<StepElement_CurveElementSectionDerivedDefinitions>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 12, ach, "curve_element_section_derived_definitions"))
    return;

  // Inherited fields of CurveElementSectionDefinition

  occ::handle<TCollection_HAsciiString> aCurveElementSectionDefinition_Description;
  data->ReadString(num,
                   1,
                   "curve_element_section_definition.description",
                   ach,
                   aCurveElementSectionDefinition_Description);

  double aCurveElementSectionDefinition_SectionAngle;
  data->ReadReal(num,
                 2,
                 "curve_element_section_definition.section_angle",
                 ach,
                 aCurveElementSectionDefinition_SectionAngle);

  // Own fields of CurveElementSectionDerivedDefinitions

  double aCrossSectionalArea;
  data->ReadReal(num, 3, "cross_sectional_area", ach, aCrossSectionalArea);

  occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>> aShearArea;
  int                                       sub4 = 0;
  if (data->ReadSubList(num, 4, "shear_area", ach, sub4))
  {
    int nb0  = data->NbParams(sub4);
    aShearArea            = new NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>(1, nb0);
    int num2 = sub4;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      StepElement_MeasureOrUnspecifiedValue anIt0;
      data->ReadEntity(num2, i0, "measure_or_unspecified_value", ach, anIt0);
      aShearArea->SetValue(i0, anIt0);
    }
  }

  occ::handle<NCollection_HArray1<double>> aSecondMomentOfArea;
  int              sub5 = 0;
  if (data->ReadSubList(num, 5, "second_moment_of_area", ach, sub5))
  {
    int nb0  = data->NbParams(sub5);
    aSecondMomentOfArea   = new NCollection_HArray1<double>(1, nb0);
    int num2 = sub5;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      double anIt0;
      data->ReadReal(num2, i0, "real", ach, anIt0);
      aSecondMomentOfArea->SetValue(i0, anIt0);
    }
  }

  double aTorsionalConstant;
  data->ReadReal(num, 6, "torsional_constant", ach, aTorsionalConstant);

  StepElement_MeasureOrUnspecifiedValue aWarpingConstant;
  data->ReadEntity(num, 7, "warping_constant", ach, aWarpingConstant);

  occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>> aLocationOfCentroid;
  int                                       sub8 = 0;
  if (data->ReadSubList(num, 8, "location_of_centroid", ach, sub8))
  {
    int nb0  = data->NbParams(sub8);
    aLocationOfCentroid   = new NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>(1, nb0);
    int num2 = sub8;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      StepElement_MeasureOrUnspecifiedValue anIt0;
      data->ReadEntity(num2, i0, "measure_or_unspecified_value", ach, anIt0);
      aLocationOfCentroid->SetValue(i0, anIt0);
    }
  }

  occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>> aLocationOfShearCentre;
  int                                       sub9 = 0;
  if (data->ReadSubList(num, 9, "location_of_shear_centre", ach, sub9))
  {
    int nb0   = data->NbParams(sub9);
    aLocationOfShearCentre = new NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>(1, nb0);
    int num2  = sub9;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      StepElement_MeasureOrUnspecifiedValue anIt0;
      data->ReadEntity(num2, i0, "measure_or_unspecified_value", ach, anIt0);
      aLocationOfShearCentre->SetValue(i0, anIt0);
    }
  }

  occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>> aLocationOfNonStructuralMass;
  int                                       sub10 = 0;
  if (data->ReadSubList(num, 10, "location_of_non_structural_mass", ach, sub10))
  {
    int nb0         = data->NbParams(sub10);
    aLocationOfNonStructuralMass = new NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>(1, nb0);
    int num2        = sub10;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      StepElement_MeasureOrUnspecifiedValue anIt0;
      data->ReadEntity(num2, i0, "measure_or_unspecified_value", ach, anIt0);
      aLocationOfNonStructuralMass->SetValue(i0, anIt0);
    }
  }

  StepElement_MeasureOrUnspecifiedValue aNonStructuralMass;
  data->ReadEntity(num, 11, "non_structural_mass", ach, aNonStructuralMass);

  StepElement_MeasureOrUnspecifiedValue aPolarMoment;
  data->ReadEntity(num, 12, "polar_moment", ach, aPolarMoment);

  // Initialize entity
  ent->Init(aCurveElementSectionDefinition_Description,
            aCurveElementSectionDefinition_SectionAngle,
            aCrossSectionalArea,
            aShearArea,
            aSecondMomentOfArea,
            aTorsionalConstant,
            aWarpingConstant,
            aLocationOfCentroid,
            aLocationOfShearCentre,
            aLocationOfNonStructuralMass,
            aNonStructuralMass,
            aPolarMoment);
}

//=================================================================================================

void RWStepElement_RWCurveElementSectionDerivedDefinitions::WriteStep(
  StepData_StepWriter&                                             SW,
  const occ::handle<StepElement_CurveElementSectionDerivedDefinitions>& ent) const
{

  // Inherited fields of CurveElementSectionDefinition

  SW.Send(ent->StepElement_CurveElementSectionDefinition::Description());

  SW.Send(ent->StepElement_CurveElementSectionDefinition::SectionAngle());

  // Own fields of CurveElementSectionDerivedDefinitions

  SW.Send(ent->CrossSectionalArea());

  SW.OpenSub();
  for (int i3 = 1; i3 <= ent->ShearArea()->Length(); i3++)
  {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->ShearArea()->Value(i3);
    SW.Send(Var0.Value());
  }
  SW.CloseSub();

  SW.OpenSub();
  for (int i4 = 1; i4 <= ent->SecondMomentOfArea()->Length(); i4++)
  {
    double Var0 = ent->SecondMomentOfArea()->Value(i4);
    SW.Send(Var0);
  }
  SW.CloseSub();

  SW.Send(ent->TorsionalConstant());

  SW.Send(ent->WarpingConstant().Value());

  SW.OpenSub();
  for (int i7 = 1; i7 <= ent->LocationOfCentroid()->Length(); i7++)
  {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfCentroid()->Value(i7);
    SW.Send(Var0.Value());
  }
  SW.CloseSub();

  SW.OpenSub();
  for (int i8 = 1; i8 <= ent->LocationOfShearCentre()->Length(); i8++)
  {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfShearCentre()->Value(i8);
    SW.Send(Var0.Value());
  }
  SW.CloseSub();

  SW.OpenSub();
  for (int i9 = 1; i9 <= ent->LocationOfNonStructuralMass()->Length(); i9++)
  {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfNonStructuralMass()->Value(i9);
    SW.Send(Var0.Value());
  }
  SW.CloseSub();

  SW.Send(ent->NonStructuralMass().Value());

  SW.Send(ent->PolarMoment().Value());
}

//=================================================================================================

void RWStepElement_RWCurveElementSectionDerivedDefinitions::Share(
  const occ::handle<StepElement_CurveElementSectionDerivedDefinitions>& ent,
  Interface_EntityIterator&                                        iter) const
{

  // Inherited fields of CurveElementSectionDefinition

  // Own fields of CurveElementSectionDerivedDefinitions

  for (int i1 = 1; i1 <= ent->ShearArea()->Length(); i1++)
  {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->ShearArea()->Value(i1);
    iter.AddItem(Var0.Value());
  }

  iter.AddItem(ent->WarpingConstant().Value());

  for (int i3 = 1; i3 <= ent->LocationOfCentroid()->Length(); i3++)
  {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfCentroid()->Value(i3);
    iter.AddItem(Var0.Value());
  }

  for (int i4 = 1; i4 <= ent->LocationOfShearCentre()->Length(); i4++)
  {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfShearCentre()->Value(i4);
    iter.AddItem(Var0.Value());
  }

  for (int i5 = 1; i5 <= ent->LocationOfNonStructuralMass()->Length(); i5++)
  {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfNonStructuralMass()->Value(i5);
    iter.AddItem(Var0.Value());
  }

  iter.AddItem(ent->NonStructuralMass().Value());

  iter.AddItem(ent->PolarMoment().Value());
}
