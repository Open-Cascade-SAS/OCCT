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
#include <BRep_Builder.hxx>
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
static bool findNearestValidPoint(const Adaptor3d_Curve& theCurve,
                                              const double    theFirst,
                                              const double    theLast,
                                              const bool isFirst,
                                              const gp_Pnt&          theVertPnt,
                                              const double    theTol,
                                              const double    theEps,
                                              double&         thePar)
{
  // 1. Check that the needed end is inside the sphere

  double aStartU = theFirst;
  double anEndU  = theLast;
  if (!isFirst)
    std::swap(aStartU, anEndU);
  gp_Pnt              aP     = theCurve.Value(aStartU);
  const double aSqTol = theTol * theTol;
  if (aP.SquareDistance(theVertPnt) > aSqTol)
    // the vertex does not cover the corresponding to this vertex end of the curve
    return false;

  // 2. Find a nearest point that is outside

  // stepping along the curve by theTol till go out
  //
  // the general step is computed using general curve resolution
  double aStep = theCurve.Resolution(theTol) * 1.01;
  if (aStep < theEps)
    aStep = theEps;
  // aD1Mag is a threshold to consider local derivative magnitude too small
  // and to accelerate going out of sphere
  // (inverse of resolution is the maximal derivative);
  // this is actual for bezier and b-spline types only
  double     aD1Mag = 0.;
  GeomAbs_CurveType aType  = theCurve.GetType();
  if (aType == GeomAbs_OffsetCurve)
  {
    occ::handle<Geom_OffsetCurve> anOffsetCurve = theCurve.OffsetCurve();
    occ::handle<Geom_Curve>       aBaseCurve    = anOffsetCurve->BasisCurve();
    aType                                  = GeomAdaptor_Curve(aBaseCurve).GetType();
  }
  if (aType == GeomAbs_BezierCurve || aType == GeomAbs_BSplineCurve)
  {
    aD1Mag = 1. / theCurve.Resolution(1.) * 0.01;
    aD1Mag *= aD1Mag;
  }
  if (!isFirst)
    aStep = -aStep;
  bool isOut  = false;
  double    anUIn  = aStartU;
  double    anUOut = anUIn;
  while (!isOut)
  {
    anUIn = anUOut;
    anUOut += aStep;
    if ((isFirst && anUOut > anEndU) || (!isFirst && anUOut < anEndU))
    {
      // step is too big and we go out of bounds,
      // check if the opposite bound is outside
      aP    = theCurve.Value(anEndU);
      isOut = (aP.SquareDistance(theVertPnt) > aSqTol);
      if (!isOut)
        // all range is inside sphere
        return false;
      anUOut = anEndU;
      break;
    }
    if (aD1Mag > 0.)
    {
      double aStepLocal = aStep;
      for (;;)
      {
        // cycle to go out of local singularity
        gp_Vec aD1;
        theCurve.D1(anUOut, aP, aD1);
        isOut = (aP.SquareDistance(theVertPnt) > aSqTol);
        if (!isOut && aD1.SquareMagnitude() < aD1Mag)
        {
          aStepLocal *= 2.;
          anUOut += aStepLocal;
          if ((isFirst && anUOut < anEndU) || (!isFirst && anUOut > anEndU))
            // still in range
            continue;
          // went out of range, so check if the end point has out state
          anUOut = anEndU;
          aP     = theCurve.Value(anUOut);
          isOut  = (aP.SquareDistance(theVertPnt) > aSqTol);
          if (!isOut)
            // all range is inside sphere
            return false;
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

  double aDelta = std::abs(anUOut - anUIn);
  while (aDelta > theEps)
  {
    double aMidU = (anUIn + anUOut) * 0.5;
    aP                  = theCurve.Value(aMidU);
    isOut               = (aP.SquareDistance(theVertPnt) > aSqTol);
    if (isOut)
      anUOut = aMidU;
    else
      anUIn = aMidU;
    aDelta = std::abs(anUOut - anUIn);
  }
  thePar = (anUIn + anUOut) * 0.5;
  return true;
}

//=================================================================================================

bool BRepLib::FindValidRange(const Adaptor3d_Curve& theCurve,
                                         const double    theTolE,
                                         const double    theParV1,
                                         const gp_Pnt&          thePntV1,
                                         const double    theTolV1,
                                         const double    theParV2,
                                         const gp_Pnt&          thePntV2,
                                         const double    theTolV2,
                                         double&         theFirst,
                                         double&         theLast)
{
  if (theParV2 - theParV1 < Precision::PConfusion())
    return false;

  bool isInfParV1 = Precision::IsInfinite(theParV1),
                   isInfParV2 = Precision::IsInfinite(theParV2);

  double aMaxPar = 0.0;
  if (!isInfParV1)
    aMaxPar = std::abs(theParV1);
  if (!isInfParV2)
    aMaxPar = std::max(aMaxPar, std::abs(theParV2));

  double anEps = std::max(std::max(theCurve.Resolution(theTolE) * 0.1, Epsilon(aMaxPar)),
                                 Precision::PConfusion());

  if (isInfParV1)
    theFirst = theParV1;
  else
  {
    if (!findNearestValidPoint(theCurve,
                               theParV1,
                               theParV2,
                               true,
                               thePntV1,
                               theTolV1,
                               anEps,
                               theFirst))
      return false;
    if (theParV2 - theFirst < anEps)
      return false;
  }

  if (isInfParV2)
    theLast = theParV2;
  else
  {
    if (!findNearestValidPoint(theCurve,
                               theParV1,
                               theParV2,
                               false,
                               thePntV2,
                               theTolV2,
                               anEps,
                               theLast))
      return false;
    if (theLast - theParV1 < anEps)
      return false;
  }

  // check found parameters
  if (theFirst > theLast)
  {
    // overlapping, not valid range
    return false;
  }

  return true;
}

//=================================================================================================

bool BRepLib::FindValidRange(const TopoDS_Edge& theEdge,
                                         double&     theFirst,
                                         double&     theLast)
{
  TopLoc_Location aLoc;
  double   f, l;
  if (BRep_Tool::Curve(theEdge, aLoc, f, l).IsNull())
    return false;
  BRepAdaptor_Curve anAC(theEdge);
  double     aParV[2] = {anAC.FirstParameter(), anAC.LastParameter()};
  if (aParV[1] - aParV[0] < Precision::PConfusion())
    return false;

  // get vertices
  TopoDS_Vertex aV[2];
  TopExp::Vertices(theEdge, aV[0], aV[1]);

  double aTolE = BRep_Tool::Tolerance(theEdge);
  // to have correspondence with intersection precision
  // the tolerances of vertices are increased on Precision::Confusion()
  double aTolV[2] = {Precision::Confusion(), Precision::Confusion()};
  gp_Pnt        aPntV[2];
  for (int i = 0; i < 2; i++)
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
  return FindValidRange(anAC,
                        aTolE,
                        aParV[0],
                        aPntV[0],
                        aTolV[0],
                        aParV[1],
                        aPntV[1],
                        aTolV[1],
                        theFirst,
                        theLast);
}

//=================================================================================================

void BRepLib::BuildPCurveForEdgeOnPlane(const TopoDS_Edge& aE, const TopoDS_Face& aF)
{
  bool     bToUpdate;
  double        aTolE;
  occ::handle<Geom2d_Curve> aC2D;
  BRep_Builder         aBB;
  //
  BuildPCurveForEdgeOnPlane(aE, aF, aC2D, bToUpdate);
  if (bToUpdate)
  {
    aTolE = BRep_Tool::Tolerance(aE);
    aBB.UpdateEdge(aE, aC2D, aF, aTolE);
  }
}

//=================================================================================================

void BRepLib::BuildPCurveForEdgeOnPlane(const TopoDS_Edge&    aE,
                                        const TopoDS_Face&    aF,
                                        occ::handle<Geom2d_Curve>& aC2D,
                                        bool&     bToUpdate)
{
  double    aT1, aT2;
  bool isStored;
  aC2D      = BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2, &isStored);
  bToUpdate = !isStored && !aC2D.IsNull();
}
