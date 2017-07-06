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

#include <Standard_Type.hxx>
#include <StepFEA_Curve3dElementProperty.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_Curve3dElementProperty,Standard_Transient)

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
