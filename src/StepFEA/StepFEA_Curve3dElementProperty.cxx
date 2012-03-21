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
