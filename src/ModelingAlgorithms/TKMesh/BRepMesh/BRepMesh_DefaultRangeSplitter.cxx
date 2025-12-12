// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_DefaultRangeSplitter.hxx>

#include <BRep_Tool.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomGridEval_Surface.hxx>
#include <TColStd_Array1OfReal.hxx>

//=================================================================================================

void BRepMesh_DefaultRangeSplitter::Reset(const IMeshData::IFaceHandle& theDFace,
                                          const IMeshTools_Parameters& /*theParameters*/)
{
  myDFace        = theDFace;
  myRangeU.first = myRangeV.first = 1.e100;
  myRangeU.second = myRangeV.second = -1.e100;
  myDelta.first = myDelta.second = 1.;
  myTolerance.first = myTolerance.second = Precision::Confusion();
}

//=================================================================================================

void BRepMesh_DefaultRangeSplitter::AddPoint(const gp_Pnt2d& thePoint)
{
  myRangeU.first  = std::min(thePoint.X(), myRangeU.first);
  myRangeU.second = std::max(thePoint.X(), myRangeU.second);
  myRangeV.first  = std::min(thePoint.Y(), myRangeV.first);
  myRangeV.second = std::max(thePoint.Y(), myRangeV.second);
}

//=================================================================================================

void BRepMesh_DefaultRangeSplitter::AdjustRange()
{
  const Handle(BRepAdaptor_Surface)& aSurface = GetSurface();
  updateRange(aSurface->FirstUParameter(),
              aSurface->LastUParameter(),
              aSurface->IsUPeriodic(),
              myRangeU.first,
              myRangeU.second);

  if (myRangeU.second < myRangeU.first)
  {
    myIsValid = Standard_False;
    return;
  }

  updateRange(aSurface->FirstVParameter(),
              aSurface->LastVParameter(),
              aSurface->IsVPeriodic(),
              myRangeV.first,
              myRangeV.second);

  if (myRangeV.second < myRangeV.first)
  {
    myIsValid = Standard_False;
    return;
  }

  Standard_Real aLengthU = 0.0, aLengthV = 0.0;
  computeLengths(aLengthU, aLengthV);
  myIsValid = aLengthU > Precision::PConfusion() && aLengthV > Precision::PConfusion();

  if (myIsValid)
  {
    computeTolerance(aLengthU, aLengthV);
    computeDelta(aLengthU, aLengthV);
  }
}

//=================================================================================================

Standard_Boolean BRepMesh_DefaultRangeSplitter::IsValid()
{
  return myIsValid;
}

//=================================================================================================

gp_Pnt2d BRepMesh_DefaultRangeSplitter::Scale(const gp_Pnt2d&        thePoint,
                                              const Standard_Boolean isToFaceBasis) const
{
  return isToFaceBasis ? gp_Pnt2d((thePoint.X() - myRangeU.first) / myDelta.first,
                                  (thePoint.Y() - myRangeV.first) / myDelta.second)
                       : gp_Pnt2d(thePoint.X() * myDelta.first + myRangeU.first,
                                  thePoint.Y() * myDelta.second + myRangeV.first);
}

//=================================================================================================

Handle(IMeshData::ListOfPnt2d) BRepMesh_DefaultRangeSplitter::GenerateSurfaceNodes(
  const IMeshTools_Parameters& /*theParameters*/) const
{
  return Handle(IMeshData::ListOfPnt2d)();
}

//=================================================================================================

void BRepMesh_DefaultRangeSplitter::computeTolerance(const Standard_Real /*theLenU*/,
                                                     const Standard_Real /*theLenV*/)
{
  const Standard_Real aDiffU = myRangeU.second - myRangeU.first;
  const Standard_Real aDiffV = myRangeV.second - myRangeV.first;

  // Slightly increase exact resolution so to cover links with approximate
  // length equal to resolution itself on sub-resolution differences.
  const Standard_Real      aTolerance = BRep_Tool::Tolerance(myDFace->GetFace());
  const Adaptor3d_Surface& aSurface   = GetSurface()->Surface();
  const Standard_Real      aResU      = aSurface.UResolution(aTolerance) * 1.1;
  const Standard_Real      aResV      = aSurface.VResolution(aTolerance) * 1.1;

  const Standard_Real aDeflectionUV = 1.e-05;
  myTolerance.first                 = std::max(std::min(aDeflectionUV, aResU), 1e-7 * aDiffU);
  myTolerance.second                = std::max(std::min(aDeflectionUV, aResV), 1e-7 * aDiffV);
}

//=================================================================================================

void BRepMesh_DefaultRangeSplitter::computeDelta(const Standard_Real theLengthU,
                                                 const Standard_Real theLengthV)
{
  const Standard_Real aDiffU = myRangeU.second - myRangeU.first;
  const Standard_Real aDiffV = myRangeV.second - myRangeV.first;

  myDelta.first  = aDiffU / (theLengthU < myTolerance.first ? 1. : theLengthU);
  myDelta.second = aDiffV / (theLengthV < myTolerance.second ? 1. : theLengthV);
}

//=================================================================================================

void BRepMesh_DefaultRangeSplitter::computeLengths(Standard_Real& theLengthU,
                                                   Standard_Real& theLengthV)
{
  // Use batch grid evaluation for optimized surface point computation.
  // Grid is 21x21 points covering the UV range uniformly.
  constexpr int THE_NB_PARAMS = 21;
  constexpr int THE_MID_INDEX = 11; // Middle index (1-based)

  const double aDeltaU = (myRangeU.second - myRangeU.first) / (THE_NB_PARAMS - 1);
  const double aDeltaV = (myRangeV.second - myRangeV.first) / (THE_NB_PARAMS - 1);

  TColStd_Array1OfReal aUParams(1, THE_NB_PARAMS);
  TColStd_Array1OfReal aVParams(1, THE_NB_PARAMS);

  for (int i = 1; i <= THE_NB_PARAMS; ++i)
  {
    aUParams.SetValue(i, myRangeU.first + (i - 1) * aDeltaU);
    aVParams.SetValue(i, myRangeV.first + (i - 1) * aDeltaV);
  }
  // Ensure exact boundary values
  aUParams.SetValue(THE_NB_PARAMS, myRangeU.second);
  aVParams.SetValue(THE_NB_PARAMS, myRangeV.second);

  GeomGridEval_Surface anEvaluator;
  anEvaluator.Initialize(*GetSurface());
  anEvaluator.SetUVParams(aUParams, aVParams);

  const NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();

  // Compute length along U at 3 V values: VFirst (row 1), VMid (row 11), VLast (row 21)
  theLengthU = 0.0;
  for (int aVIdx : {1, THE_MID_INDEX, THE_NB_PARAMS})
  {
    for (int aUIdx = 1; aUIdx < THE_NB_PARAMS; ++aUIdx)
    {
      theLengthU += aGrid.Value(aUIdx, aVIdx).Distance(aGrid.Value(aUIdx + 1, aVIdx));
    }
  }
  theLengthU /= 3.0;

  // Compute length along V at 3 U values: UFirst (col 1), UMid (col 11), ULast (col 21)
  theLengthV = 0.0;
  for (int aUIdx : {1, THE_MID_INDEX, THE_NB_PARAMS})
  {
    for (int aVIdx = 1; aVIdx < THE_NB_PARAMS; ++aVIdx)
    {
      theLengthV += aGrid.Value(aUIdx, aVIdx).Distance(aGrid.Value(aUIdx, aVIdx + 1));
    }
  }
  theLengthV /= 3.0;
}

//=================================================================================================

void BRepMesh_DefaultRangeSplitter::updateRange(const Standard_Real    theGeomFirst,
                                                const Standard_Real    theGeomLast,
                                                const Standard_Boolean isPeriodic,
                                                Standard_Real&         theDiscreteFirst,
                                                Standard_Real&         theDiscreteLast)
{
  if (theDiscreteFirst < theGeomFirst || theDiscreteLast > theGeomLast)
  {
    if (isPeriodic)
    {
      if ((theDiscreteLast - theDiscreteFirst) > (theGeomLast - theGeomFirst))
      {
        theDiscreteLast = theDiscreteFirst + (theGeomLast - theGeomFirst);
      }
    }
    else
    {
      if ((theDiscreteFirst < theGeomLast) && (theDiscreteLast > theGeomFirst))
      {
        // Protection against the faces whose pcurve is out of the surface's domain
        //(see issue #23675 and test cases "bugs iges buc60820*")

        if (theGeomFirst > theDiscreteFirst)
        {
          theDiscreteFirst = theGeomFirst;
        }

        if (theGeomLast < theDiscreteLast)
        {
          theDiscreteLast = theGeomLast;
        }
      }
    }
  }
}
