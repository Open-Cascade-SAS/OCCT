// Created on: 2017-03-24
// Created by: Mikhail Sazonov
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <BRepLib.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
// function: findNearestValidPoint
// purpose : Starting from the appointed end of the curve, find the nearest
//           point on the curve that is an intersection with the sphere with
//           center theVertPnt and radius theTol.
//=======================================================================
static Standard_Boolean findNearestValidPoint(
  const Adaptor3d_Curve& theCurve,
  const Standard_Real theFirst, const Standard_Real theLast,
  const Standard_Boolean isFirst,
  const gp_Pnt& theVertPnt,
  const Standard_Real theTol,
  const Standard_Real theEps,
  Standard_Real& thePar)
{
  // 1. Check that the needed end is inside the sphere

  Standard_Real aStartU = theFirst;
  Standard_Real anEndU = theLast;
  if (!isFirst)
    std::swap(aStartU, anEndU);
  gp_Pnt aP = theCurve.Value(aStartU);
  const Standard_Real aSqTol = theTol * theTol;
  if (aP.SquareDistance(theVertPnt) > aSqTol)
    // the vertex does not cover the corresponding to this vertex end of the curve
    return Standard_False;

  // 2. Find a nearest point that is outside

  // stepping along the curve by theTol till go out
  //
  // the general step is computed using general curve resolution
  Standard_Real aStep = theCurve.Resolution(theTol) * 1.01;
  // aD1Mag is a threshold to consider local derivative magnitude too small
  // and to accelerate going out of sphere
  // (inverse of resolution is the maximal derivative);
  // this is actual for bezier and b-spline types only
  Standard_Real aD1Mag = 0.;
  GeomAbs_CurveType aType = theCurve.GetType();
  if (aType == GeomAbs_OffsetCurve)
  {
    Handle(Geom_OffsetCurve) anOffsetCurve = theCurve.OffsetCurve();
    Handle(Geom_Curve) aBaseCurve = anOffsetCurve->BasisCurve();
    aType = GeomAdaptor_Curve(aBaseCurve).GetType();
  }
  if (aType == GeomAbs_BezierCurve || aType == GeomAbs_BSplineCurve)
  {
    aD1Mag = 1. / theCurve.Resolution(1.) * 0.01;
    aD1Mag *= aD1Mag;
  }
  if (!isFirst)
    aStep = -aStep;
  Standard_Boolean isOut = Standard_False;
  Standard_Real anUIn = aStartU;
  Standard_Real anUOut = anUIn;
  while (!isOut)
  {
    anUIn = anUOut;
    anUOut += aStep;
    if ((isFirst && anUOut > anEndU) || (!isFirst && anUOut < anEndU))
    {
      // step is too big and we go out of bounds,
      // check if the opposite bound is outside
      aP = theCurve.Value(anEndU);
      isOut = (aP.SquareDistance(theVertPnt) > aSqTol);
      if (!isOut)
        // all range is inside sphere
        return Standard_False;
      anUOut = anEndU;
      break;
    }
    if (aD1Mag > 0.)
    {
      Standard_Real aStepLocal = aStep;
      for (;;)
      {
        // cycle to go out of local singularity
        gp_Vec aD1;
        theCurve.D1(anUOut, aP, aD1);
        if (aD1.SquareMagnitude() < aD1Mag)
        {
          aStepLocal *= 2.;
          anUOut += aStepLocal;
          if ((isFirst && anUOut < anEndU) || (!isFirst && anUOut > anEndU))
            // still in range
            continue;
          // went out of range, so check if the end point has out state
          anUOut = anEndU;
          aP = theCurve.Value(anUOut);
          isOut = (aP.SquareDistance(theVertPnt) > aSqTol);
          if (!isOut)
            // all range is inside sphere
            return Standard_False;
        }
        break;
      }
    }
    else
    {
      aP = theCurve.Value(anUOut);
    }
    if (!isOut)
      isOut = (aP.SquareDistance(theVertPnt) > aSqTol);
  }

  // 3. Precise solution with binary search

  Standard_Real aDelta = Abs(anUOut - anUIn);
  while (aDelta > theEps)
  {
    Standard_Real aMidU = (anUIn + anUOut) * 0.5;
    aP = theCurve.Value(aMidU);
    isOut = (aP.SquareDistance(theVertPnt) > aSqTol);
    if (isOut)
      anUOut = aMidU;
    else
      anUIn = aMidU;
    aDelta = Abs(anUOut - anUIn);
  }
  thePar = (anUIn + anUOut) * 0.5;
  return Standard_True;
}

//=======================================================================
// function: FindValidRange
// purpose : 
//=======================================================================
Standard_Boolean BRepLib::FindValidRange
  (const Adaptor3d_Curve& theCurve, const Standard_Real theTolE,
   const Standard_Real theParV1, const gp_Pnt& thePntV1, const Standard_Real theTolV1,
   const Standard_Real theParV2, const gp_Pnt& thePntV2, const Standard_Real theTolV2,
   Standard_Real& theFirst, Standard_Real& theLast)
{
  if (theParV2 - theParV1 < Precision::PConfusion())
    return Standard_False;
  
  Standard_Real anEps = Max(theCurve.Resolution(theTolE) * 0.1, Precision::PConfusion());

  if (Precision::IsInfinite(theParV1))
    theFirst = theParV1;
  else
  {
    if (!findNearestValidPoint(theCurve, theParV1, theParV2, Standard_True,
                               thePntV1, theTolV1, anEps, theFirst))
      return Standard_False;
    if (theParV2 - theFirst < anEps)
      return Standard_False;
  }

  if (Precision::IsInfinite(theParV2))
    theLast = theParV2;
  else
  {
    if (!findNearestValidPoint(theCurve, theParV1, theParV2, Standard_False,
                               thePntV2, theTolV2, anEps, theLast))
      return Standard_False;
    if (theLast - theParV1 < anEps)
      return Standard_False;
  }

  // check found parameters
  if (theFirst > theLast)
  {
    // overlapping, not valid range
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
// function: FindValidRange
// purpose : 
//=======================================================================
Standard_Boolean BRepLib::FindValidRange
  (const TopoDS_Edge& theEdge, Standard_Real& theFirst, Standard_Real& theLast)
{
  TopLoc_Location aLoc;
  Standard_Real f, l;
  if (BRep_Tool::Curve(theEdge, aLoc, f, l).IsNull())
    return Standard_False;
  BRepAdaptor_Curve anAC(theEdge);
  Standard_Real aParV[2] = { anAC.FirstParameter(), anAC.LastParameter() };
  if (aParV[1] - aParV[0] < Precision::PConfusion())
    return Standard_False;

  // get vertices
  TopoDS_Vertex aV[2];
  TopExp::Vertices(theEdge, aV[0], aV[1]);

  Standard_Real aTolE = BRep_Tool::Tolerance(theEdge);
  // to have correspondence with intersection precision
  // the tolerances of vertices are increased on Precision::Confusion()
  Standard_Real aTolV[2] = { Precision::Confusion(), Precision::Confusion() };
  gp_Pnt aPntV[2];
  for (Standard_Integer i = 0; i < 2; i++)
  {
    if (!aV[i].IsNull())
    {
      aTolV[i] += BRep_Tool::Tolerance(aV[i]);
      aPntV[i] = BRep_Tool::Pnt(aV[i]);
    }
    else if (!Precision::IsInfinite(aParV[i]))
    {
      aTolV[i] += aTolE;
      aPntV[i] = anAC.Value(aParV[i]);
    }
  }
  return FindValidRange(anAC, aTolE, 
                        aParV[0], aPntV[0], aTolV[0],
                        aParV[1], aPntV[1], aTolV[1],
                        theFirst, theLast);
}
