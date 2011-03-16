// File:	StepElement_CurveElementSectionDerivedDefinitions.cxx
// Created:	Thu Dec 12 17:29:00 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
