// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

#include <RWStepElement_RWCurveElementSectionDerivedDefinitions.ixx>
#include <StepElement_HArray1OfMeasureOrUnspecifiedValue.hxx>
#include <StepElement_MeasureOrUnspecifiedValue.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Real.hxx>
#include <StepElement_HArray1OfMeasureOrUnspecifiedValue.hxx>
#include <StepElement_MeasureOrUnspecifiedValue.hxx>
#include <StepElement_HArray1OfMeasureOrUnspecifiedValue.hxx>
#include <StepElement_MeasureOrUnspecifiedValue.hxx>
#include <StepElement_HArray1OfMeasureOrUnspecifiedValue.hxx>
#include <StepElement_MeasureOrUnspecifiedValue.hxx>

//=======================================================================
//function : RWStepElement_RWCurveElementSectionDerivedDefinitions
//purpose  : 
//=======================================================================

RWStepElement_RWCurveElementSectionDerivedDefinitions::RWStepElement_RWCurveElementSectionDerivedDefinitions ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementSectionDerivedDefinitions::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                      const Standard_Integer num,
                                                                      Handle(Interface_Check)& ach,
                                                                      const Handle(StepElement_CurveElementSectionDerivedDefinitions) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,12,ach,"curve_element_section_derived_definitions") ) return;

  // Inherited fields of CurveElementSectionDefinition

  Handle(TCollection_HAsciiString) aCurveElementSectionDefinition_Description;
  data->ReadString (num, 1, "curve_element_section_definition.description", ach, aCurveElementSectionDefinition_Description);

  Standard_Real aCurveElementSectionDefinition_SectionAngle;
  data->ReadReal (num, 2, "curve_element_section_definition.section_angle", ach, aCurveElementSectionDefinition_SectionAngle);

  // Own fields of CurveElementSectionDerivedDefinitions

  Standard_Real aCrossSectionalArea;
  data->ReadReal (num, 3, "cross_sectional_area", ach, aCrossSectionalArea);

  Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) aShearArea;
  Standard_Integer sub4 = 0;
  if ( data->ReadSubList (num, 4, "shear_area", ach, sub4) ) {
    Standard_Integer nb0 = data->NbParams(sub4);
    aShearArea = new StepElement_HArray1OfMeasureOrUnspecifiedValue (1, nb0);
    Standard_Integer num2 = sub4;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepElement_MeasureOrUnspecifiedValue anIt0;
      data->ReadEntity (num2, i0, "measure_or_unspecified_value", ach, anIt0);
      aShearArea->SetValue(i0, anIt0);
    }
  }

  Handle(TColStd_HArray1OfReal) aSecondMomentOfArea;
  Standard_Integer sub5 = 0;
  if ( data->ReadSubList (num, 5, "second_moment_of_area", ach, sub5) ) {
    Standard_Integer nb0 = data->NbParams(sub5);
    aSecondMomentOfArea = new TColStd_HArray1OfReal (1, nb0);
    Standard_Integer num2 = sub5;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Standard_Real anIt0;
      data->ReadReal (num2, i0, "real", ach, anIt0);
      aSecondMomentOfArea->SetValue(i0, anIt0);
    }
  }

  Standard_Real aTorsionalConstant;
  data->ReadReal (num, 6, "torsional_constant", ach, aTorsionalConstant);

  StepElement_MeasureOrUnspecifiedValue aWarpingConstant;
  data->ReadEntity (num, 7, "warping_constant", ach, aWarpingConstant);

  Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) aLocationOfCentroid;
  Standard_Integer sub8 = 0;
  if ( data->ReadSubList (num, 8, "location_of_centroid", ach, sub8) ) {
    Standard_Integer nb0 = data->NbParams(sub8);
    aLocationOfCentroid = new StepElement_HArray1OfMeasureOrUnspecifiedValue (1, nb0);
    Standard_Integer num2 = sub8;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepElement_MeasureOrUnspecifiedValue anIt0;
      data->ReadEntity (num2, i0, "measure_or_unspecified_value", ach, anIt0);
      aLocationOfCentroid->SetValue(i0, anIt0);
    }
  }

  Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) aLocationOfShearCentre;
  Standard_Integer sub9 = 0;
  if ( data->ReadSubList (num, 9, "location_of_shear_centre", ach, sub9) ) {
    Standard_Integer nb0 = data->NbParams(sub9);
    aLocationOfShearCentre = new StepElement_HArray1OfMeasureOrUnspecifiedValue (1, nb0);
    Standard_Integer num2 = sub9;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepElement_MeasureOrUnspecifiedValue anIt0;
      data->ReadEntity (num2, i0, "measure_or_unspecified_value", ach, anIt0);
      aLocationOfShearCentre->SetValue(i0, anIt0);
    }
  }

  Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) aLocationOfNonStructuralMass;
  Standard_Integer sub10 = 0;
  if ( data->ReadSubList (num, 10, "location_of_non_structural_mass", ach, sub10) ) {
    Standard_Integer nb0 = data->NbParams(sub10);
    aLocationOfNonStructuralMass = new StepElement_HArray1OfMeasureOrUnspecifiedValue (1, nb0);
    Standard_Integer num2 = sub10;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepElement_MeasureOrUnspecifiedValue anIt0;
      data->ReadEntity (num2, i0, "measure_or_unspecified_value", ach, anIt0);
      aLocationOfNonStructuralMass->SetValue(i0, anIt0);
    }
  }

  StepElement_MeasureOrUnspecifiedValue aNonStructuralMass;
  data->ReadEntity (num, 11, "non_structural_mass", ach, aNonStructuralMass);

  StepElement_MeasureOrUnspecifiedValue aPolarMoment;
  data->ReadEntity (num, 12, "polar_moment", ach, aPolarMoment);

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

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementSectionDerivedDefinitions::WriteStep (StepData_StepWriter& SW,
                                                                       const Handle(StepElement_CurveElementSectionDerivedDefinitions) &ent) const
{

  // Inherited fields of CurveElementSectionDefinition

  SW.Send (ent->StepElement_CurveElementSectionDefinition::Description());

  SW.Send (ent->StepElement_CurveElementSectionDefinition::SectionAngle());

  // Own fields of CurveElementSectionDerivedDefinitions

  SW.Send (ent->CrossSectionalArea());

  SW.OpenSub();
  for (Standard_Integer i3=1; i3 <= ent->ShearArea()->Length(); i3++ ) {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->ShearArea()->Value(i3);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();

  SW.OpenSub();
  for (Standard_Integer i4=1; i4 <= ent->SecondMomentOfArea()->Length(); i4++ ) {
    Standard_Real Var0 = ent->SecondMomentOfArea()->Value(i4);
    SW.Send (Var0);
  }
  SW.CloseSub();

  SW.Send (ent->TorsionalConstant());

  SW.Send (ent->WarpingConstant().Value());

  SW.OpenSub();
  for (Standard_Integer i7=1; i7 <= ent->LocationOfCentroid()->Length(); i7++ ) {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfCentroid()->Value(i7);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();

  SW.OpenSub();
  for (Standard_Integer i8=1; i8 <= ent->LocationOfShearCentre()->Length(); i8++ ) {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfShearCentre()->Value(i8);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();

  SW.OpenSub();
  for (Standard_Integer i9=1; i9 <= ent->LocationOfNonStructuralMass()->Length(); i9++ ) {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfNonStructuralMass()->Value(i9);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();

  SW.Send (ent->NonStructuralMass().Value());

  SW.Send (ent->PolarMoment().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementSectionDerivedDefinitions::Share (const Handle(StepElement_CurveElementSectionDerivedDefinitions) &ent,
                                                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of CurveElementSectionDefinition

  // Own fields of CurveElementSectionDerivedDefinitions

  for (Standard_Integer i1=1; i1 <= ent->ShearArea()->Length(); i1++ ) {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->ShearArea()->Value(i1);
    iter.AddItem (Var0.Value());
  }

  iter.AddItem (ent->WarpingConstant().Value());

  for (Standard_Integer i3=1; i3 <= ent->LocationOfCentroid()->Length(); i3++ ) {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfCentroid()->Value(i3);
    iter.AddItem (Var0.Value());
  }

  for (Standard_Integer i4=1; i4 <= ent->LocationOfShearCentre()->Length(); i4++ ) {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfShearCentre()->Value(i4);
    iter.AddItem (Var0.Value());
  }

  for (Standard_Integer i5=1; i5 <= ent->LocationOfNonStructuralMass()->Length(); i5++ ) {
    StepElement_MeasureOrUnspecifiedValue Var0 = ent->LocationOfNonStructuralMass()->Value(i5);
    iter.AddItem (Var0.Value());
  }

  iter.AddItem (ent->NonStructuralMass().Value());

  iter.AddItem (ent->PolarMoment().Value());
}
