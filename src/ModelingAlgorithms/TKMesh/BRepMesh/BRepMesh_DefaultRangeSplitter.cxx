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

#include <GeomAdaptor_Curve.hxx>
#include <BRep_Tool.hxx>

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
  myRangeU.first  = Min(thePoint.X(), myRangeU.first);
  myRangeU.second = Max(thePoint.X(), myRangeU.second);
  myRangeV.first  = Min(thePoint.Y(), myRangeV.first);
  myRangeV.second = Max(thePoint.Y(), myRangeV.second);
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

  const Standard_Real aLengthU = computeLengthU();
  const Standard_Real aLengthV = computeLengthV();
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
  myTolerance.first                 = Max(Min(aDeflectionUV, aResU), 1e-7 * aDiffU);
  myTolerance.second                = Max(Min(aDeflectionUV, aResV), 1e-7 * aDiffV);
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

Standard_Real BRepMesh_DefaultRangeSplitter::computeLengthU()
{
  Standard_Real longu = 0.0;
  gp_Pnt        P11, P12, P21, P22, P31, P32;

  Standard_Real    du     = 0.05 * (myRangeU.second - myRangeU.first);
  Standard_Real    dfvave = 0.5 * (myRangeV.second + myRangeV.first);
  Standard_Real    dfucur;
  Standard_Integer i1;

  const Handle(BRepAdaptor_Surface)& gFace = GetSurface();
  gFace->D0(myRangeU.first, myRangeV.first, P11);
  gFace->D0(myRangeU.first, dfvave, P21);
  gFace->D0(myRangeU.first, myRangeV.second, P31);
  for (i1 = 1, dfucur = myRangeU.first + du; i1 <= 20; i1++, dfucur += du)
  {
    gFace->D0(dfucur, myRangeV.first, P12);
    gFace->D0(dfucur, dfvave, P22);
    gFace->D0(dfucur, myRangeV.second, P32);
    longu += (P11.Distance(P12) + P21.Distance(P22) + P31.Distance(P32));
    P11 = P12;
    P21 = P22;
    P31 = P32;
  }

  return longu / 3.;
}

//=================================================================================================

Standard_Real BRepMesh_DefaultRangeSplitter::computeLengthV()
{
  Standard_Real longv = 0.0;
  gp_Pnt        P11, P12, P21, P22, P31, P32;

  Standard_Real    dv     = 0.05 * (myRangeV.second - myRangeV.first);
  Standard_Real    dfuave = 0.5 * (myRangeU.second + myRangeU.first);
  Standard_Real    dfvcur;
  Standard_Integer i1;

  const Handle(BRepAdaptor_Surface)& gFace = GetSurface();
  gFace->D0(myRangeU.first, myRangeV.first, P11);
  gFace->D0(dfuave, myRangeV.first, P21);
  gFace->D0(myRangeU.second, myRangeV.first, P31);
  for (i1 = 1, dfvcur = myRangeV.first + dv; i1 <= 20; i1++, dfvcur += dv)
  {
    gFace->D0(myRangeU.first, dfvcur, P12);
    gFace->D0(dfuave, dfvcur, P22);
    gFace->D0(myRangeU.second, dfvcur, P32);
    longv += (P11.Distance(P12) + P21.Distance(P22) + P31.Distance(P32));
    P11 = P12;
    P21 = P22;
    P31 = P32;
  }

  return longv / 3.;
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
