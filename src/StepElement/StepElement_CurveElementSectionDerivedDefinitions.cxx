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

#include <StepElement_CurveElementSectionDerivedDefinitions.ixx>

//=======================================================================
//function : StepElement_CurveElementSectionDerivedDefinitions
//purpose  : 
//=======================================================================

StepElement_CurveElementSectionDerivedDefinitions::StepElement_CurveElementSectionDerivedDefinitions ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::Init (const Handle(TCollection_HAsciiString) &aCurveElementSectionDefinition_Description,
                                                              const Standard_Real aCurveElementSectionDefinition_SectionAngle,
                                                              const Standard_Real aCrossSectionalArea,
                                                              const Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) &aShearArea,
                                                              const Handle(TColStd_HArray1OfReal) &aSecondMomentOfArea,
                                                              const Standard_Real aTorsionalConstant,
                                                              const StepElement_MeasureOrUnspecifiedValue &aWarpingConstant,
                                                              const Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) &aLocationOfCentroid,
                                                              const Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) &aLocationOfShearCentre,
                                                              const Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) &aLocationOfNonStructuralMass,
                                                              const StepElement_MeasureOrUnspecifiedValue &aNonStructuralMass,
                                                              const StepElement_MeasureOrUnspecifiedValue &aPolarMoment)
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

//=======================================================================
//function : CrossSectionalArea
//purpose  : 
//=======================================================================

Standard_Real StepElement_CurveElementSectionDerivedDefinitions::CrossSectionalArea () const
{
  return theCrossSectionalArea;
}

//=======================================================================
//function : SetCrossSectionalArea
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetCrossSectionalArea (const Standard_Real aCrossSectionalArea)
{
  theCrossSectionalArea = aCrossSectionalArea;
}

//=======================================================================
//function : ShearArea
//purpose  : 
//=======================================================================

Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) StepElement_CurveElementSectionDerivedDefinitions::ShearArea () const
{
  return theShearArea;
}

//=======================================================================
//function : SetShearArea
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetShearArea (const Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) &aShearArea)
{
  theShearArea = aShearArea;
}

//=======================================================================
//function : SecondMomentOfArea
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepElement_CurveElementSectionDerivedDefinitions::SecondMomentOfArea () const
{
  return theSecondMomentOfArea;
}

//=======================================================================
//function : SetSecondMomentOfArea
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetSecondMomentOfArea (const Handle(TColStd_HArray1OfReal) &aSecondMomentOfArea)
{
  theSecondMomentOfArea = aSecondMomentOfArea;
}

//=======================================================================
//function : TorsionalConstant
//purpose  : 
//=======================================================================

Standard_Real StepElement_CurveElementSectionDerivedDefinitions::TorsionalConstant () const
{
  return theTorsionalConstant;
}

//=======================================================================
//function : SetTorsionalConstant
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetTorsionalConstant (const Standard_Real aTorsionalConstant)
{
  theTorsionalConstant = aTorsionalConstant;
}

//=======================================================================
//function : WarpingConstant
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_CurveElementSectionDerivedDefinitions::WarpingConstant () const
{
  return theWarpingConstant;
}

//=======================================================================
//function : SetWarpingConstant
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetWarpingConstant (const StepElement_MeasureOrUnspecifiedValue &aWarpingConstant)
{
  theWarpingConstant = aWarpingConstant;
}

//=======================================================================
//function : LocationOfCentroid
//purpose  : 
//=======================================================================

Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) StepElement_CurveElementSectionDerivedDefinitions::LocationOfCentroid () const
{
  return theLocationOfCentroid;
}

//=======================================================================
//function : SetLocationOfCentroid
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetLocationOfCentroid (const Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) &aLocationOfCentroid)
{
  theLocationOfCentroid = aLocationOfCentroid;
}

//=======================================================================
//function : LocationOfShearCentre
//purpose  : 
//=======================================================================

Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) StepElement_CurveElementSectionDerivedDefinitions::LocationOfShearCentre () const
{
  return theLocationOfShearCentre;
}

//=======================================================================
//function : SetLocationOfShearCentre
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetLocationOfShearCentre (const Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) &aLocationOfShearCentre)
{
  theLocationOfShearCentre = aLocationOfShearCentre;
}

//=======================================================================
//function : LocationOfNonStructuralMass
//purpose  : 
//=======================================================================

Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) StepElement_CurveElementSectionDerivedDefinitions::LocationOfNonStructuralMass () const
{
  return theLocationOfNonStructuralMass;
}

//=======================================================================
//function : SetLocationOfNonStructuralMass
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetLocationOfNonStructuralMass (const Handle(StepElement_HArray1OfMeasureOrUnspecifiedValue) &aLocationOfNonStructuralMass)
{
  theLocationOfNonStructuralMass = aLocationOfNonStructuralMass;
}

//=======================================================================
//function : NonStructuralMass
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_CurveElementSectionDerivedDefinitions::NonStructuralMass () const
{
  return theNonStructuralMass;
}

//=======================================================================
//function : SetNonStructuralMass
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetNonStructuralMass (const StepElement_MeasureOrUnspecifiedValue &aNonStructuralMass)
{
  theNonStructuralMass = aNonStructuralMass;
}

//=======================================================================
//function : PolarMoment
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_CurveElementSectionDerivedDefinitions::PolarMoment () const
{
  return thePolarMoment;
}

//=======================================================================
//function : SetPolarMoment
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDerivedDefinitions::SetPolarMoment (const StepElement_MeasureOrUnspecifiedValue &aPolarMoment)
{
  thePolarMoment = aPolarMoment;
}
