// File:	StepFEA_Curve3dElementProperty.cxx
// Created:	Thu Dec 12 17:51:03 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_Curve3dElementProperty.ixx>

//=======================================================================
//function : StepFEA_Curve3dElementProperty
//purpose  : 
//=======================================================================

StepFEA_Curve3dElementProperty::StepFEA_Curve3dElementProperty ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementProperty::Init (const Handle(TCollection_HAsciiString) &aPropertyId,
                                           const Handle(TCollection_HAsciiString) &aDescription,
                                           const Handle(StepFEA_HArray1OfCurveElementInterval) &aIntervalDefinitions,
                                           const Handle(StepFEA_HArray1OfCurveElementEndOffset) &aEndOffsets,
                                           const Handle(StepFEA_HArray1OfCurveElementEndRelease) &aEndReleases)
{

  thePropertyId = aPropertyId;

  theDescription = aDescription;

  theIntervalDefinitions = aIntervalDefinitions;

  theEndOffsets = aEndOffsets;

  theEndReleases = aEndReleases;
}

//=======================================================================
//function : PropertyId
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepFEA_Curve3dElementProperty::PropertyId () const
{
  return thePropertyId;
}

//=======================================================================
//function : SetPropertyId
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementProperty::SetPropertyId (const Handle(TCollection_HAsciiString) &aPropertyId)
{
  thePropertyId = aPropertyId;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepFEA_Curve3dElementProperty::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementProperty::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : IntervalDefinitions
//purpose  : 
//=======================================================================

Handle(StepFEA_HArray1OfCurveElementInterval) StepFEA_Curve3dElementProperty::IntervalDefinitions () const
{
  return theIntervalDefinitions;
}

//=======================================================================
//function : SetIntervalDefinitions
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementProperty::SetIntervalDefinitions (const Handle(StepFEA_HArray1OfCurveElementInterval) &aIntervalDefinitions)
{
  theIntervalDefinitions = aIntervalDefinitions;
}

//=======================================================================
//function : EndOffsets
//purpose  : 
//=======================================================================

Handle(StepFEA_HArray1OfCurveElementEndOffset) StepFEA_Curve3dElementProperty::EndOffsets () const
{
  return theEndOffsets;
}

//=======================================================================
//function : SetEndOffsets
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementProperty::SetEndOffsets (const Handle(StepFEA_HArray1OfCurveElementEndOffset) &aEndOffsets)
{
  theEndOffsets = aEndOffsets;
}

//=======================================================================
//function : EndReleases
//purpose  : 
//=======================================================================

Handle(StepFEA_HArray1OfCurveElementEndRelease) StepFEA_Curve3dElementProperty::EndReleases () const
{
  return theEndReleases;
}

//=======================================================================
//function : SetEndReleases
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementProperty::SetEndReleases (const Handle(StepFEA_HArray1OfCurveElementEndRelease) &aEndReleases)
{
  theEndReleases = aEndReleases;
}
