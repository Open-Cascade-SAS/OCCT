// File:	StepFEA_CurveElementEndOffset.cxx
// Created:	Thu Dec 12 17:51:03 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_CurveElementEndOffset.ixx>

//=======================================================================
//function : StepFEA_CurveElementEndOffset
//purpose  : 
//=======================================================================

StepFEA_CurveElementEndOffset::StepFEA_CurveElementEndOffset ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_CurveElementEndOffset::Init (const StepFEA_CurveElementEndCoordinateSystem &aCoordinateSystem,
                                          const Handle(TColStd_HArray1OfReal) &aOffsetVector)
{

  theCoordinateSystem = aCoordinateSystem;

  theOffsetVector = aOffsetVector;
}

//=======================================================================
//function : CoordinateSystem
//purpose  : 
//=======================================================================

StepFEA_CurveElementEndCoordinateSystem StepFEA_CurveElementEndOffset::CoordinateSystem () const
{
  return theCoordinateSystem;
}

//=======================================================================
//function : SetCoordinateSystem
//purpose  : 
//=======================================================================

void StepFEA_CurveElementEndOffset::SetCoordinateSystem (const StepFEA_CurveElementEndCoordinateSystem &aCoordinateSystem)
{
  theCoordinateSystem = aCoordinateSystem;
}

//=======================================================================
//function : OffsetVector
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_CurveElementEndOffset::OffsetVector () const
{
  return theOffsetVector;
}

//=======================================================================
//function : SetOffsetVector
//purpose  : 
//=======================================================================

void StepFEA_CurveElementEndOffset::SetOffsetVector (const Handle(TColStd_HArray1OfReal) &aOffsetVector)
{
  theOffsetVector = aOffsetVector;
}
