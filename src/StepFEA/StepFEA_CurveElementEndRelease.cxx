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
#include <StepFEA_CurveElementEndCoordinateSystem.hxx>
#include <StepFEA_CurveElementEndRelease.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_CurveElementEndRelease,Standard_Transient)

//=======================================================================
//function : StepFEA_CurveElementEndRelease
//purpose  : 
//=======================================================================
StepFEA_CurveElementEndRelease::StepFEA_CurveElementEndRelease ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_CurveElementEndRelease::Init (const StepFEA_CurveElementEndCoordinateSystem &aCoordinateSystem,
                                           const Handle(StepElement_HArray1OfCurveElementEndReleasePacket) &aReleases)
{

  theCoordinateSystem = aCoordinateSystem;

  theReleases = aReleases;
}

//=======================================================================
//function : CoordinateSystem
//purpose  : 
//=======================================================================

StepFEA_CurveElementEndCoordinateSystem StepFEA_CurveElementEndRelease::CoordinateSystem () const
{
  return theCoordinateSystem;
}

//=======================================================================
//function : SetCoordinateSystem
//purpose  : 
//=======================================================================

void StepFEA_CurveElementEndRelease::SetCoordinateSystem (const StepFEA_CurveElementEndCoordinateSystem &aCoordinateSystem)
{
  theCoordinateSystem = aCoordinateSystem;
}

//=======================================================================
//function : Releases
//purpose  : 
//=======================================================================

Handle(StepElement_HArray1OfCurveElementEndReleasePacket) StepFEA_CurveElementEndRelease::Releases () const
{
  return theReleases;
}

//=======================================================================
//function : SetReleases
//purpose  : 
//=======================================================================

void StepFEA_CurveElementEndRelease::SetReleases (const Handle(StepElement_HArray1OfCurveElementEndReleasePacket) &aReleases)
{
  theReleases = aReleases;
}
