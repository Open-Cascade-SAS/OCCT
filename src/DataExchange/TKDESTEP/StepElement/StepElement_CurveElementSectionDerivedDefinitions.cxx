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

#include <StepElement_CurveElementSectionDerivedDefinitions.hxx>
#include <StepElement_MeasureOrUnspecifiedValue.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_CurveElementSectionDerivedDefinitions,
                           StepElement_CurveElementSectionDefinition)

//=================================================================================================

StepElement_CurveElementSectionDerivedDefinitions::
  StepElement_CurveElementSectionDerivedDefinitions()
{
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::Init(
  const occ::handle<TCollection_HAsciiString>& aCurveElementSectionDefinition_Description,
  const double                                 aCurveElementSectionDefinition_SectionAngle,
  const double                                 aCrossSectionalArea,
  const occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>& aShearArea,
  const occ::handle<NCollection_HArray1<double>>& aSecondMomentOfArea,
  const double                                    aTorsionalConstant,
  const StepElement_MeasureOrUnspecifiedValue&    aWarpingConstant,
  const occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>&
    aLocationOfCentroid,
  const occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>&
    aLocationOfShearCentre,
  const occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>&
                                               aLocationOfNonStructuralMass,
  const StepElement_MeasureOrUnspecifiedValue& aNonStructuralMass,
  const StepElement_MeasureOrUnspecifiedValue& aPolarMoment)
{
  StepElement_CurveElementSectionDefinition::Init(aCurveElementSectionDefinition_Description,
                                                  aCurveElementSectionDefinition_SectionAngle);

  theCrossSectionalArea = aCrossSectionalArea;

  theShearArea = aShearArea;

  theSecondMomentOfArea = aSecondMomentOfArea;

  theTorsionalConstant = aTorsionalConstant;

  theWarpingConstant = aWarpingConstant;

  theLocationOfCentroid = aLocationOfCentroid;

  theLocationOfShearCentre = aLocationOfShearCentre;

  theLocationOfNonStructuralMass = aLocationOfNonStructuralMass;

  theNonStructuralMass = aNonStructuralMass;

  thePolarMoment = aPolarMoment;
}

//=================================================================================================

double StepElement_CurveElementSectionDerivedDefinitions::CrossSectionalArea() const
{
  return theCrossSectionalArea;
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetCrossSectionalArea(
  const double aCrossSectionalArea)
{
  theCrossSectionalArea = aCrossSectionalArea;
}

//=================================================================================================

occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>
  StepElement_CurveElementSectionDerivedDefinitions::ShearArea() const
{
  return theShearArea;
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetShearArea(
  const occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>& aShearArea)
{
  theShearArea = aShearArea;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> StepElement_CurveElementSectionDerivedDefinitions::
  SecondMomentOfArea() const
{
  return theSecondMomentOfArea;
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetSecondMomentOfArea(
  const occ::handle<NCollection_HArray1<double>>& aSecondMomentOfArea)
{
  theSecondMomentOfArea = aSecondMomentOfArea;
}

//=================================================================================================

double StepElement_CurveElementSectionDerivedDefinitions::TorsionalConstant() const
{
  return theTorsionalConstant;
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetTorsionalConstant(
  const double aTorsionalConstant)
{
  theTorsionalConstant = aTorsionalConstant;
}

//=================================================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_CurveElementSectionDerivedDefinitions::
  WarpingConstant() const
{
  return theWarpingConstant;
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetWarpingConstant(
  const StepElement_MeasureOrUnspecifiedValue& aWarpingConstant)
{
  theWarpingConstant = aWarpingConstant;
}

//=================================================================================================

occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>
  StepElement_CurveElementSectionDerivedDefinitions::LocationOfCentroid() const
{
  return theLocationOfCentroid;
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetLocationOfCentroid(
  const occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>&
    aLocationOfCentroid)
{
  theLocationOfCentroid = aLocationOfCentroid;
}

//=================================================================================================

occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>
  StepElement_CurveElementSectionDerivedDefinitions::LocationOfShearCentre() const
{
  return theLocationOfShearCentre;
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetLocationOfShearCentre(
  const occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>&
    aLocationOfShearCentre)
{
  theLocationOfShearCentre = aLocationOfShearCentre;
}

//=================================================================================================

occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>
  StepElement_CurveElementSectionDerivedDefinitions::LocationOfNonStructuralMass() const
{
  return theLocationOfNonStructuralMass;
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetLocationOfNonStructuralMass(
  const occ::handle<NCollection_HArray1<StepElement_MeasureOrUnspecifiedValue>>&
    aLocationOfNonStructuralMass)
{
  theLocationOfNonStructuralMass = aLocationOfNonStructuralMass;
}

//=================================================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_CurveElementSectionDerivedDefinitions::
  NonStructuralMass() const
{
  return theNonStructuralMass;
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetNonStructuralMass(
  const StepElement_MeasureOrUnspecifiedValue& aNonStructuralMass)
{
  theNonStructuralMass = aNonStructuralMass;
}

//=================================================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_CurveElementSectionDerivedDefinitions::
  PolarMoment() const
{
  return thePolarMoment;
}

//=================================================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetPolarMoment(
  const StepElement_MeasureOrUnspecifiedValue& aPolarMoment)
{
  thePolarMoment = aPolarMoment;
}
