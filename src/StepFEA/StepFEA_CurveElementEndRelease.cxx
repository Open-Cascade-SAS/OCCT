// File:	StepFEA_CurveElementEndRelease.cxx
// Created:	Thu Dec 12 17:51:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_CurveElementEndRelease.ixx>

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
