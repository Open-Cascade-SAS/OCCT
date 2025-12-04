// Created on: 1995-07-18
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593

#include <Extrema_GenExtPS.hxx>

#include <BSplSLib_GridEvaluator.hxx>
#include <BVH_Tools.hxx>
#include <BVH_Traverse.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Extrema_POnSurf.hxx>
#include <Extrema_POnSurfParams.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <gp_Pnt.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array2OfInteger.hxx>

//! BVH traverser for finding the closest point to a given point.
//! Uses BVH_Tools::PointBoxSquareDistance for efficient pruning.
class Extrema_PSBVHMinDistanceSelector
    : public BVH_Traverse<Standard_Real,
                          3,
                          BVH_BoxSet<Standard_Real, 3, Extrema_GenExtPS::UVIndex>,
                          Standard_Real>
{
public:
  Extrema_PSBVHMinDistanceSelector(const Extrema_Array2OfPOnSurfParams& thePoints)
      : myPoints(&thePoints),
        myMinDistSq(std::numeric_limits<Standard_Real>::max()),
        myClosestU(0),
        myClosestV(0)
  {
  }

  void SetQueryPoint(const gp_Pnt& thePoint)
  {
    myQueryPoint = BVH_Vec3d(thePoint.X(), thePoint.Y(), thePoint.Z());
  }

  void Reset()
  {
    myMinDistSq = std::numeric_limits<Standard_Real>::max();
    myClosestU  = 0;
    myClosestV  = 0;
  }

  Standard_Real MinSquareDistance() const { return myMinDistSq; }

  Standard_Integer ClosestU() const { return myClosestU; }

  Standard_Integer ClosestV() const { return myClosestV; }

  //! Reject nodes whose bounding box is farther than current minimum
  virtual Standard_Boolean RejectNode(const BVH_Vec3d& theCornerMin,
                                      const BVH_Vec3d& theCornerMax,
                                      Standard_Real&   theMetric) const Standard_OVERRIDE
  {
    theMetric =
      BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance(myQueryPoint, theCornerMin, theCornerMax);
    return theMetric >= myMinDistSq;
  }

  //! Accept a leaf element and update minimum if closer
  virtual Standard_Boolean Accept(const Standard_Integer theIndex,
                                  const Standard_Real&) Standard_OVERRIDE
  {
    const Extrema_GenExtPS::UVIndex& anIdx = this->myBVHSet->Element(theIndex);
    const gp_Pnt&                    aPnt  = myPoints->Value(anIdx.first, anIdx.second).Value();
    const Standard_Real aDistSq = (aPnt.X() - myQueryPoint.x()) * (aPnt.X() - myQueryPoint.x())
                                  + (aPnt.Y() - myQueryPoint.y()) * (aPnt.Y() - myQueryPoint.y())
                                  + (aPnt.Z() - myQueryPoint.z()) * (aPnt.Z() - myQueryPoint.z());
    if (aDistSq < myMinDistSq)
    {
      myMinDistSq = aDistSq;
      myClosestU  = anIdx.first;
      myClosestV  = anIdx.second;
    }
    return Standard_True;
  }

  virtual Standard_Boolean IsMetricBetter(const Standard_Real& theLeft,
                                          const Standard_Real& theRight) const Standard_OVERRIDE
  {
    return theLeft < theRight;
  }

  virtual Standard_Boolean RejectMetric(const Standard_Real& theMetric) const Standard_OVERRIDE
  {
    return theMetric >= myMinDistSq;
  }

private:
  const Extrema_Array2OfPOnSurfParams* myPoints;
  BVH_Vec3d                            myQueryPoint;
  mutable Standard_Real                myMinDistSq;
  mutable Standard_Integer             myClosestU;
  mutable Standard_Integer             myClosestV;
};

//! BVH traverser for finding the farthest point from a given point.
class Extrema_PSBVHMaxDistanceSelector
    : public BVH_Traverse<Standard_Real,
                          3,
                          BVH_BoxSet<Standard_Real, 3, Extrema_GenExtPS::UVIndex>,
                          Standard_Real>
{
public:
  Extrema_PSBVHMaxDistanceSelector(const Extrema_Array2OfPOnSurfParams& thePoints)
      : myPoints(&thePoints),
        myMaxDistSq(0.0),
        myFarthestU(0),
        myFarthestV(0)
  {
  }

  void SetQueryPoint(const gp_Pnt& thePoint)
  {
    myQueryPoint = BVH_Vec3d(thePoint.X(), thePoint.Y(), thePoint.Z());
  }

  void Reset()
  {
    myMaxDistSq = 0.0;
    myFarthestU = 0;
    myFarthestV = 0;
  }

  Standard_Real MaxSquareDistance() const { return myMaxDistSq; }

  Standard_Integer FarthestU() const { return myFarthestU; }

  Standard_Integer FarthestV() const { return myFarthestV; }

  //! Compute maximum possible distance to box (distance to farthest corner)
  static Standard_Real MaxPointBoxSquareDistance(const BVH_Vec3d& thePoint,
                                                 const BVH_Vec3d& theCornerMin,
                                                 const BVH_Vec3d& theCornerMax)
  {
    Standard_Real aDistSq = 0.0;
    for (int i = 0; i < 3; ++i)
    {
      const Standard_Real aDist =
        std::max(std::abs(thePoint[i] - theCornerMin[i]), std::abs(thePoint[i] - theCornerMax[i]));
      aDistSq += aDist * aDist;
    }
    return aDistSq;
  }

  //! Reject nodes whose maximum possible distance is less than current maximum
  virtual Standard_Boolean RejectNode(const BVH_Vec3d& theCornerMin,
                                      const BVH_Vec3d& theCornerMax,
                                      Standard_Real&   theMetric) const Standard_OVERRIDE
  {
    theMetric = MaxPointBoxSquareDistance(myQueryPoint, theCornerMin, theCornerMax);
    return theMetric <= myMaxDistSq;
  }

  //! Accept a leaf element and update maximum if farther
  virtual Standard_Boolean Accept(const Standard_Integer theIndex,
                                  const Standard_Real&) Standard_OVERRIDE
  {
    const Extrema_GenExtPS::UVIndex& anIdx = this->myBVHSet->Element(theIndex);
    const gp_Pnt&                    aPnt  = myPoints->Value(anIdx.first, anIdx.second).Value();
    const Standard_Real aDistSq = (aPnt.X() - myQueryPoint.x()) * (aPnt.X() - myQueryPoint.x())
                                  + (aPnt.Y() - myQueryPoint.y()) * (aPnt.Y() - myQueryPoint.y())
                                  + (aPnt.Z() - myQueryPoint.z()) * (aPnt.Z() - myQueryPoint.z());
    if (aDistSq > myMaxDistSq)
    {
      myMaxDistSq = aDistSq;
      myFarthestU = anIdx.first;
      myFarthestV = anIdx.second;
    }
    return Standard_True;
  }

  //! For max distance, larger metrics are better
  virtual Standard_Boolean IsMetricBetter(const Standard_Real& theLeft,
                                          const Standard_Real& theRight) const Standard_OVERRIDE
  {
    return theLeft > theRight;
  }

  virtual Standard_Boolean RejectMetric(const Standard_Real& theMetric) const Standard_OVERRIDE
  {
    return theMetric <= myMaxDistSq;
  }

private:
  const Extrema_Array2OfPOnSurfParams* myPoints;
  BVH_Vec3d                            myQueryPoint;
  mutable Standard_Real                myMaxDistSq;
  mutable Standard_Integer             myFarthestU;
  mutable Standard_Integer             myFarthestV;
};

//==================================================================================================

Extrema_GenExtPS::Extrema_GenExtPS()
    : myumin(0.0),
      myusup(0.0),
      myvmin(0.0),
      myvsup(0.0),
      myusample(0),
      myvsample(0),
      mytolu(0.0),
      mytolv(0.0),
      myS(nullptr)
{
  myDone = Standard_False;
  myInit = Standard_False;
  myFlag = Extrema_ExtFlag_MINMAX;
  myAlgo = Extrema_ExtAlgo_Grad;
}

//==================================================================================================

Extrema_GenExtPS::~Extrema_GenExtPS()
{
  //
}

//==================================================================================================

Extrema_GenExtPS::Extrema_GenExtPS(const gp_Pnt&            theP,
                                   const Adaptor3d_Surface& theS,
                                   const Standard_Integer   theNbU,
                                   const Standard_Integer   theNbV,
                                   const Standard_Real      theTolU,
                                   const Standard_Real      theTolV,
                                   const Extrema_ExtFlag    theF,
                                   const Extrema_ExtAlgo    theA)
    : myF(theP, theS),
      myFlag(theF),
      myAlgo(theA)
{
  Initialize(theS, theNbU, theNbV, theTolU, theTolV);
  Perform(theP);
}

Extrema_GenExtPS::Extrema_GenExtPS(const gp_Pnt&            theP,
                                   const Adaptor3d_Surface& theS,
                                   const Standard_Integer   theNbU,
                                   const Standard_Integer   theNbV,
                                   const Standard_Real      theUMin,
                                   const Standard_Real      theUSup,
                                   const Standard_Real      theVMin,
                                   const Standard_Real      theVSup,
                                   const Standard_Real      theTolU,
                                   const Standard_Real      theTolV,
                                   const Extrema_ExtFlag    theF,
                                   const Extrema_ExtAlgo    theA)
    : myF(theP, theS),
      myFlag(theF),
      myAlgo(theA)
{
  Initialize(theS, theNbU, theNbV, theUMin, theUSup, theVMin, theVSup, theTolU, theTolV);
  Perform(theP);
}

void Extrema_GenExtPS::Initialize(const Adaptor3d_Surface& theS,
                                  const Standard_Integer   theNbU,
                                  const Standard_Integer   theNbV,
                                  const Standard_Real      theTolU,
                                  const Standard_Real      theTolV)
{
  myumin = theS.FirstUParameter();
  myusup = theS.LastUParameter();
  myvmin = theS.FirstVParameter();
  myvsup = theS.LastVParameter();
  Initialize(theS, theNbU, theNbV, myumin, myusup, myvmin, myvsup, theTolU, theTolV);
}

void Extrema_GenExtPS::Initialize(const Adaptor3d_Surface& theS,
                                  const Standard_Integer   theNbU,
                                  const Standard_Integer   theNbV,
                                  const Standard_Real      theUMin,
                                  const Standard_Real      theUSup,
                                  const Standard_Real      theVMin,
                                  const Standard_Real      theVSup,
                                  const Standard_Real      theTolU,
                                  const Standard_Real      theTolV)
{
  myS       = &theS;
  myusample = theNbU;
  myvsample = theNbV;
  mytolu    = theTolU;
  mytolv    = theTolV;
  myumin    = theUMin;
  myusup    = theUSup;
  myvmin    = theVMin;
  myvsup    = theVSup;

  if ((myusample < 2) || (myvsample < 2))
  {
    throw Standard_OutOfRange();
  }

  myF.Initialize(theS);

  myBVHSet.Clear();
  myUParams.Nullify();
  myVParams.Nullify();
  myInit = Standard_False;
}

inline static void fillParams(const TColStd_Array1OfReal&    theKnots,
                              Standard_Integer               theDegree,
                              Standard_Real                  theParMin,
                              Standard_Real                  theParMax,
                              Handle(TColStd_HArray1OfReal)& theParams,
                              Standard_Integer               theSample)
{
  NCollection_Vector<Standard_Real> aParams;
  Standard_Integer                  anI      = 1;
  Standard_Real                     aPrevPar = theParMin;
  aParams.Append(aPrevPar);
  // calculation the array of parametric points depending on the knots array variation and degree of
  // given surface
  for (; anI < theKnots.Length() && theKnots(anI) < (theParMax - Precision::PConfusion()); anI++)
  {
    if (theKnots(anI + 1) < theParMin + Precision::PConfusion())
    {
      continue;
    }

    Standard_Real aStep = (theKnots(anI + 1) - theKnots(anI)) / std::max(theDegree, 2);
    for (Standard_Integer aK = 1; aK <= theDegree; aK++)
    {
      Standard_Real aPar = theKnots(anI) + aK * aStep;
      if (aPar > theParMax - Precision::PConfusion())
      {
        break;
      }
      if (aPar > aPrevPar + Precision::PConfusion())
      {
        aParams.Append(aPar);
        aPrevPar = aPar;
      }
    }
  }
  aParams.Append(theParMax);
  Standard_Integer aNbPar = aParams.Length();
  // in case of an insufficient number of points the grid will be built later
  if (aNbPar < theSample)
  {
    return;
  }
  theParams = new TColStd_HArray1OfReal(1, aNbPar);
  for (anI = 0; anI < aNbPar; anI++)
  {
    theParams->SetValue(anI + 1, aParams(anI));
  }
}

//! Helper function to build grid points for BSpline surface using optimized evaluator.
//! This avoids repeated span lookups (LocateParameter) by pre-computing span indices.
//! @param theSurf       the BSpline surface
//! @param theUParams    output array of U parameters
//! @param theVParams    output array of V parameters
//! @param thePoints     output 2D array of surface points with parameters
//! @param theUMin       minimum U parameter
//! @param theUMax       maximum U parameter
//! @param theVMin       minimum V parameter
//! @param theVMax       maximum V parameter
//! @param theNbU        number of U samples (may be updated)
//! @param theNbV        number of V samples (may be updated)
//! @return true if optimization was applied, false otherwise
static Standard_Boolean buildBSplineGrid(const Handle(Geom_BSplineSurface)& theSurf,
                                         Handle(TColStd_HArray1OfReal)&     theUParams,
                                         Handle(TColStd_HArray1OfReal)&     theVParams,
                                         Extrema_Array2OfPOnSurfParams&     thePoints,
                                         const Standard_Real                theUMin,
                                         const Standard_Real                theUMax,
                                         const Standard_Real                theVMin,
                                         const Standard_Real                theVMax,
                                         Standard_Integer&                  theNbU,
                                         Standard_Integer&                  theNbV)
{
  if (theSurf.IsNull())
  {
    return Standard_False;
  }

  // Initialize the grid evaluator with BSpline surface data
  BSplSLib_GridEvaluator anEval;
  // Weights() returns a pointer (nullptr for non-rational surfaces)
  const TColStd_Array2OfReal* aWeights = theSurf->Weights();
  anEval.Initialize(theSurf->UDegree(),
                    theSurf->VDegree(),
                    theSurf->Poles(),
                    aWeights,
                    theSurf->UKnotSequence(),
                    theSurf->VKnotSequence(),
                    theSurf->IsURational(),
                    theSurf->IsVRational(),
                    theSurf->IsUPeriodic(),
                    theSurf->IsVPeriodic());

  // Use uniform sampling with EXACTLY the same grid size as original code
  // This ensures consistent behavior while benefiting from pre-computed span indices
  anEval.PrepareUParams(theUMin, theUMax, theNbU);
  anEval.PrepareVParams(theVMin, theVMax, theNbV);

  // Use actual parameter count from evaluator (may differ from requested)
  const Standard_Integer aNbU = anEval.NbUParams();
  const Standard_Integer aNbV = anEval.NbVParams();

  if (aNbU < 2 || aNbV < 2)
  {
    return Standard_False;
  }

  // Update output counts
  theNbU = aNbU;
  theNbV = aNbV;

  // Create parameter arrays
  theUParams = new TColStd_HArray1OfReal(1, aNbU);
  theVParams = new TColStd_HArray1OfReal(1, aNbV);

  for (Standard_Integer anI = 1; anI <= aNbU; ++anI)
  {
    theUParams->SetValue(anI, anEval.UParam(anI));
  }
  for (Standard_Integer anI = 1; anI <= aNbV; ++anI)
  {
    theVParams->SetValue(anI, anEval.VParam(anI));
  }

  // Resize the points array
  thePoints.Resize(0, aNbU + 1, 0, aNbV + 1, false);

  // Evaluate all grid points using the optimized evaluator (no binary search per point)
  for (Standard_Integer aNoU = 1; aNoU <= aNbU; ++aNoU)
  {
    for (Standard_Integer aNoV = 1; aNoV <= aNbV; ++aNoV)
    {
      gp_Pnt aPnt;
      anEval.D0(aNoU, aNoV, aPnt);

      Extrema_POnSurfParams aParam(theUParams->Value(aNoU), theVParams->Value(aNoV), aPnt);
      aParam.SetElementType(Extrema_Node);
      aParam.SetIndices(aNoU, aNoV);
      thePoints.SetValue(aNoU, aNoV, aParam);
    }
  }

  return Standard_True;
}

void Extrema_GenExtPS::GetGridPoints(const Adaptor3d_Surface& theSurf)
{
  // creation parametric points for BSpline and Bezier surfaces
  // with taking into account of Degree and NbKnots of BSpline or Bezier geometry
  if (theSurf.GetType() == GeomAbs_OffsetSurface)
  {
    GetGridPoints(*theSurf.BasisSurface());
  }
  // parametric points for BSpline surfaces
  else if (theSurf.GetType() == GeomAbs_BSplineSurface)
  {
    Handle(Geom_BSplineSurface) aBspl = theSurf.BSpline();
    if (!aBspl.IsNull())
    {
      TColStd_Array1OfReal aUKnots(1, aBspl->NbUKnots());
      aBspl->UKnots(aUKnots);
      TColStd_Array1OfReal aVKnots(1, aBspl->NbVKnots());
      aBspl->VKnots(aVKnots);
      fillParams(aUKnots, aBspl->UDegree(), myumin, myusup, myUParams, myusample);
      fillParams(aVKnots, aBspl->VDegree(), myvmin, myvsup, myVParams, myvsample);
    }
  }
  // calculation parametric points for Bezier surfaces
  else if (theSurf.GetType() == GeomAbs_BezierSurface)
  {
    Handle(Geom_BezierSurface) aBezier = theSurf.Bezier();
    if (aBezier.IsNull())
    {
      return;
    }

    TColStd_Array1OfReal aUKnots(1, 2);
    TColStd_Array1OfReal aVKnots(1, 2);
    aBezier->Bounds(aUKnots(1), aUKnots(2), aVKnots(1), aVKnots(2));
    fillParams(aUKnots, aBezier->UDegree(), myumin, myusup, myUParams, myusample);
    fillParams(aVKnots, aBezier->VDegree(), myvmin, myvsup, myVParams, myvsample);
  }
  // creation points for surfaces based on BSpline or Bezier curves
  else if (theSurf.GetType() == GeomAbs_SurfaceOfRevolution
           || theSurf.GetType() == GeomAbs_SurfaceOfExtrusion)
  {
    Handle(TColStd_HArray1OfReal) anArrKnots;
    Standard_Integer              aDegree = 0;
    if (theSurf.BasisCurve()->GetType() == GeomAbs_BSplineCurve)
    {
      Handle(Geom_BSplineCurve) aBspl = theSurf.BasisCurve()->BSpline();
      if (!aBspl.IsNull())
      {
        anArrKnots = new TColStd_HArray1OfReal(1, aBspl->NbKnots());
        aBspl->Knots(anArrKnots->ChangeArray1());
        aDegree = aBspl->Degree();
      }
    }
    if (theSurf.BasisCurve()->GetType() == GeomAbs_BezierCurve)
    {
      Handle(Geom_BezierCurve) aBez = theSurf.BasisCurve()->Bezier();
      if (!aBez.IsNull())
      {
        anArrKnots = new TColStd_HArray1OfReal(1, 2);
        anArrKnots->SetValue(1, aBez->FirstParameter());
        anArrKnots->SetValue(2, aBez->LastParameter());
        aDegree = aBez->Degree();
      }
    }
    if (anArrKnots.IsNull())
    {
      return;
    }
    if (theSurf.GetType() == GeomAbs_SurfaceOfRevolution)
    {
      fillParams(anArrKnots->Array1(), aDegree, myvmin, myvsup, myVParams, myvsample);
    }
    else
    {
      fillParams(anArrKnots->Array1(), aDegree, myumin, myusup, myUParams, myusample);
    }
  }
  // update the number of points in sample
  if (!myUParams.IsNull())
  {
    myusample = myUParams->Length();
  }
  if (!myVParams.IsNull())
  {
    myvsample = myVParams->Length();
  }
}

//==================================================================================================

void Extrema_GenExtPS::computeGridParameters(const Standard_Real    theMin,
                                             const Standard_Real    theMax,
                                             const Standard_Integer theNbSamples,
                                             Standard_Real&         theStep,
                                             Standard_Real&         theStart)
{
  Standard_Real aRange  = theMax - theMin;
  Standard_Real aOffset = aRange / theNbSamples / 100.;
  theStep               = (aRange - aOffset) / (theNbSamples - 1);
  theStart              = aOffset / 2. + theMin;
}

//==================================================================================================

void Extrema_GenExtPS::populateParamArray(const Handle(TColStd_HArray1OfReal)& theParams,
                                          const Standard_Real                  theStart,
                                          const Standard_Real                  theStep,
                                          const Standard_Integer               theCount)
{
  Standard_Real aParam = theStart;
  for (Standard_Integer anI = 1; anI <= theCount; ++anI, aParam += theStep)
  {
    theParams->SetValue(anI, aParam);
  }
}

//==================================================================================================

void Extrema_GenExtPS::addGridPoint(const Standard_Integer theNoU,
                                    const Standard_Integer theNoV,
                                    const gp_Pnt&          thePnt,
                                    const Standard_Boolean theAddToBVH)
{
  Extrema_POnSurfParams aParam(myUParams->Value(theNoU), myVParams->Value(theNoV), thePnt);
  aParam.SetElementType(Extrema_Node);
  aParam.SetIndices(theNoU, theNoV);
  myPoints.SetValue(theNoU, theNoV, aParam);

  if (theAddToBVH)
  {
    BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(thePnt.X(), thePnt.Y(), thePnt.Z()),
                                   BVH_Vec3d(thePnt.X(), thePnt.Y(), thePnt.Z()));
    myBVHSet.Add(std::make_pair(theNoU, theNoV), aBox);
  }
}

//==================================================================================================

void Extrema_GenExtPS::initializeGrid()
{
  Standard_Boolean anIsGridBuilt = Standard_False;

  // Try optimized BSpline grid evaluation (avoids repeated span lookups)
  if (myS->GetType() == GeomAbs_BSplineSurface)
  {
    const Handle(Geom_BSplineSurface) aBspl = myS->BSpline();
    anIsGridBuilt                           = buildBSplineGrid(aBspl,
                                     myUParams,
                                     myVParams,
                                     myPoints,
                                     myumin,
                                     myusup,
                                     myvmin,
                                     myvsup,
                                     myusample,
                                     myvsample);
  }

  if (!anIsGridBuilt)
  {
    // Fallback to standard grid building for non-BSpline or failed BSpline cases

    // Build parametric grid in case of a complex surface geometry (BSpline and Bezier surfaces)
    GetGridPoints(*myS);

    // Build grid in other cases
    if (myUParams.IsNull())
    {
      Standard_Real aPasU, aU0;
      computeGridParameters(myumin, myusup, myusample, aPasU, aU0);
      myUParams = new TColStd_HArray1OfReal(1, myusample);
      populateParamArray(myUParams, aU0, aPasU, myusample);
    }

    if (myVParams.IsNull())
    {
      Standard_Real aPasV, aV0;
      computeGridParameters(myvmin, myvsup, myvsample, aPasV, aV0);
      myVParams = new TColStd_HArray1OfReal(1, myvsample);
      populateParamArray(myVParams, aV0, aPasV, myvsample);
    }

    // If flag was changed and extrema not reinitialized Extrema would fail
    myPoints.Resize(0, myusample + 1, 0, myvsample + 1, false);

    for (Standard_Integer aNoU = 1; aNoU <= myusample; aNoU++)
    {
      for (Standard_Integer aNoV = 1; aNoV <= myvsample; aNoV++)
      {
        const gp_Pnt aPnt = myS->Value(myUParams->Value(aNoU), myVParams->Value(aNoV));
        addGridPoint(aNoU, aNoV, aPnt, Standard_False);
      }
    }
  }

  // Allocate edge/face parameter arrays only when MIN is needed (lazy allocation)
  if (myFlag == Extrema_ExtFlag_MIN || myFlag == Extrema_ExtFlag_MINMAX)
  {
    myFacePntParams.Resize(0, myusample, 0, myvsample, false);
    myUEdgePntParams.Resize(1, myusample - 1, 1, myvsample, false);
    myVEdgePntParams.Resize(1, myusample, 1, myvsample - 1, false);
  }

  // Fill boundary with negative square distance.
  // It is used for computation of Maximum.
  if (myFlag == Extrema_ExtFlag_MAX || myFlag == Extrema_ExtFlag_MINMAX)
  {
    for (Standard_Integer aNoV = 0; aNoV <= myvsample + 1; aNoV++)
    {
      myPoints.ChangeValue(0, aNoV).SetSqrDistance(-1.);
      myPoints.ChangeValue(myusample + 1, aNoV).SetSqrDistance(-1.);
    }

    for (Standard_Integer aNoU = 1; aNoU <= myusample; aNoU++)
    {
      myPoints.ChangeValue(aNoU, 0).SetSqrDistance(-1.);
      myPoints.ChangeValue(aNoU, myvsample + 1).SetSqrDistance(-1.);
    }
  }

  myInit = Standard_True;
}

//==================================================================================================

void Extrema_GenExtPS::computeNodeDistances(const gp_Pnt& thePoint)
{
  for (Standard_Integer aNoU = 1; aNoU <= myusample; aNoU++)
  {
    for (Standard_Integer aNoV = 1; aNoV <= myvsample; aNoV++)
    {
      Extrema_POnSurfParams& aParam = myPoints.ChangeValue(aNoU, aNoV);
      aParam.SetSqrDistance(thePoint.SquareDistance(aParam.Value()));
    }
  }
}

//==================================================================================================

void Extrema_GenExtPS::computeEdgeDistances(const gp_Pnt& thePoint, const Standard_Real theDiffTol)
{
  for (Standard_Integer aNoU = 1; aNoU <= myusample; aNoU++)
  {
    for (Standard_Integer aNoV = 1; aNoV <= myvsample; aNoV++)
    {
      const Extrema_POnSurfParams& aParam0 = myPoints.Value(aNoU, aNoV);

      if (aNoU < myusample)
      {
        // Compute parameters to UEdge.
        const Extrema_POnSurfParams& aParam1 = myPoints.Value(aNoU + 1, aNoV);
        const Extrema_POnSurfParams& anEdgeParam =
          ComputeEdgeParameters(Standard_True, aParam0, aParam1, thePoint, theDiffTol);

        myUEdgePntParams.SetValue(aNoU, aNoV, anEdgeParam);
      }

      if (aNoV < myvsample)
      {
        // Compute parameters to VEdge.
        const Extrema_POnSurfParams& aParam1 = myPoints.Value(aNoU, aNoV + 1);
        const Extrema_POnSurfParams& anEdgeParam =
          ComputeEdgeParameters(Standard_False, aParam0, aParam1, thePoint, theDiffTol);

        myVEdgePntParams.SetValue(aNoU, aNoV, anEdgeParam);
      }
    }
  }
}

//==================================================================================================

void Extrema_GenExtPS::computeFaceParameter(const Standard_Integer theNoU,
                                            const Standard_Integer theNoV,
                                            const gp_Pnt&          thePoint,
                                            const Standard_Real    theDiffTol)
{
  const Extrema_POnSurfParams& aUE0 = myUEdgePntParams.Value(theNoU, theNoV);
  const Extrema_POnSurfParams& aUE1 = myUEdgePntParams.Value(theNoU, theNoV + 1);
  const Extrema_POnSurfParams& aVE0 = myVEdgePntParams.Value(theNoU, theNoV);
  const Extrema_POnSurfParams& aVE1 = myVEdgePntParams.Value(theNoU + 1, theNoV);

  Standard_Real    aSqrDist01 = aUE0.Value().SquareDistance(aUE1.Value());
  Standard_Real    aDiffDist  = std::abs(aUE0.GetSqrDistance() - aUE1.GetSqrDistance());
  Standard_Boolean anIsOut    = Standard_False;

  if (aDiffDist >= aSqrDist01 - theDiffTol)
  {
    // The projection is outside the face.
    anIsOut = Standard_True;
  }
  else
  {
    aSqrDist01 = aVE0.Value().SquareDistance(aVE1.Value());
    aDiffDist  = std::abs(aVE0.GetSqrDistance() - aVE1.GetSqrDistance());

    if (aDiffDist >= aSqrDist01 - theDiffTol)
    {
      // The projection is outside the face.
      anIsOut = Standard_True;
    }
  }

  if (anIsOut)
  {
    // Get the closest point on an edge.
    const Extrema_POnSurfParams& aUEMin =
      aUE0.GetSqrDistance() < aUE1.GetSqrDistance() ? aUE0 : aUE1;
    const Extrema_POnSurfParams& aVEMin =
      aVE0.GetSqrDistance() < aVE1.GetSqrDistance() ? aVE0 : aVE1;
    const Extrema_POnSurfParams& aEMin =
      aUEMin.GetSqrDistance() < aVEMin.GetSqrDistance() ? aUEMin : aVEMin;

    myFacePntParams.SetValue(theNoU, theNoV, aEMin);
  }
  else
  {
    // Find closest point inside the face.
    Standard_Real aU[2];
    Standard_Real aV[2];

    // Compute U parameter.
    aUE0.Parameter(aU[0], aV[0]);
    aUE1.Parameter(aU[1], aV[1]);
    const Standard_Real aUPar = 0.5 * (aU[0] + aU[1]);

    // Compute V parameter.
    aVE0.Parameter(aU[0], aV[0]);
    aVE1.Parameter(aU[1], aV[1]);
    const Standard_Real aVPar = 0.5 * (aV[0] + aV[1]);

    Extrema_POnSurfParams aParam(aUPar, aVPar, myS->Value(aUPar, aVPar));

    aParam.SetElementType(Extrema_Face);
    aParam.SetSqrDistance(thePoint.SquareDistance(aParam.Value()));
    aParam.SetIndices(theNoU, theNoV);
    myFacePntParams.SetValue(theNoU, theNoV, aParam);
  }
}

//==================================================================================================

void Extrema_GenExtPS::computeFaceDistances(const gp_Pnt& thePoint, const Standard_Real theDiffTol)
{
  for (Standard_Integer aNoU = 1; aNoU < myusample; aNoU++)
  {
    for (Standard_Integer aNoV = 1; aNoV < myvsample; aNoV++)
    {
      computeFaceParameter(aNoU, aNoV, thePoint, theDiffTol);
    }
  }

  // Fill boundary with RealLast square distance.
  for (Standard_Integer aNoV = 0; aNoV <= myvsample; aNoV++)
  {
    myFacePntParams.ChangeValue(0, aNoV).SetSqrDistance(RealLast());
    myFacePntParams.ChangeValue(myusample, aNoV).SetSqrDistance(RealLast());
  }

  for (Standard_Integer aNoU = 1; aNoU < myusample; aNoU++)
  {
    myFacePntParams.ChangeValue(aNoU, 0).SetSqrDistance(RealLast());
    myFacePntParams.ChangeValue(aNoU, myvsample).SetSqrDistance(RealLast());
  }
}

//==================================================================================================

const Extrema_POnSurfParams& Extrema_GenExtPS::ComputeEdgeParameters(
  const Standard_Boolean       IsUEdge,
  const Extrema_POnSurfParams& theParam0,
  const Extrema_POnSurfParams& theParam1,
  const gp_Pnt&                thePoint,
  const Standard_Real          theDiffTol)
{
  const Standard_Real aSqrDist01 = theParam0.Value().SquareDistance(theParam1.Value());

  if (aSqrDist01 <= theDiffTol)
  {
    // The points are confused. Get the first point and change its type.
    return theParam0;
  }
  else
  {
    const Standard_Real aDiffDist =
      std::abs(theParam0.GetSqrDistance() - theParam1.GetSqrDistance());

    if (aDiffDist >= aSqrDist01 - theDiffTol)
    {
      // The shortest distance is one of the nodes.
      if (theParam0.GetSqrDistance() > theParam1.GetSqrDistance())
      {
        // The shortest distance is the point 1.
        return theParam1;
      }
      else
      {
        // The shortest distance is the point 0.
        return theParam0;
      }
    }
    else
    {
      // The shortest distance is inside the edge.
      gp_XYZ        aPoP(thePoint.XYZ().Subtracted(theParam0.Value().XYZ()));
      gp_XYZ        aPoP1(theParam1.Value().XYZ().Subtracted(theParam0.Value().XYZ()));
      Standard_Real aRatio = aPoP.Dot(aPoP1) / aSqrDist01;
      Standard_Real aU[2];
      Standard_Real aV[2];

      theParam0.Parameter(aU[0], aV[0]);
      theParam1.Parameter(aU[1], aV[1]);

      Standard_Real aUPar = aU[0];
      Standard_Real aVPar = aV[0];

      if (IsUEdge)
      {
        aUPar += aRatio * (aU[1] - aU[0]);
      }
      else
      {
        aVPar += aRatio * (aV[1] - aV[0]);
      }

      myGridParam.SetParameters(aUPar, aVPar, myS->Value(aUPar, aVPar));
      Standard_Integer anIndices[2];

      theParam0.GetIndices(anIndices[0], anIndices[1]);
      myGridParam.SetElementType(IsUEdge ? Extrema_UIsoEdge : Extrema_VIsoEdge);
      myGridParam.SetSqrDistance(thePoint.SquareDistance(myGridParam.Value()));
      myGridParam.SetIndices(anIndices[0], anIndices[1]);
      return myGridParam;
    }
  }
}

void Extrema_GenExtPS::BuildGrid(const gp_Pnt& thePoint)
{
  // Initialize grid structure on first call
  if (!myInit)
  {
    initializeGrid();
  }

  // Step 1. Compute distances to nodes.
  computeNodeDistances(thePoint);

  // For search of minimum compute distances to edges and faces.
  if (myFlag == Extrema_ExtFlag_MIN || myFlag == Extrema_ExtFlag_MINMAX)
  {
    const Standard_Real aDiffTol = mytolu + mytolv;

    // Step 2. Compute distances to edges.
    computeEdgeDistances(thePoint, aDiffTol);

    // Step 3. Compute distances to faces.
    computeFaceDistances(thePoint, aDiffTol);
  }
}

static Standard_Real LengthOfIso(const Adaptor3d_Surface& theS,
                                 const GeomAbs_IsoType    theIso,
                                 const Standard_Real      thePar1,
                                 const Standard_Real      thePar2,
                                 const Standard_Integer   theNbPnts,
                                 const Standard_Real      thePar)
{
  Standard_Real aLen  = 0.;
  Standard_Real aDPar = (thePar2 - thePar1) / (theNbPnts - 1);
  gp_Pnt        aP1, aP2;
  Standard_Real aPar = thePar1 + aDPar;
  if (theIso == GeomAbs_IsoU)
  {
    aP1 = theS.Value(thePar, thePar1);
  }
  else
  {
    aP1 = theS.Value(thePar1, thePar);
  }

  for (Standard_Integer anI = 2; anI <= theNbPnts; ++anI)
  {
    if (theIso == GeomAbs_IsoU)
    {
      aP2 = theS.Value(thePar, aPar);
    }
    else
    {
      aP2 = theS.Value(aPar, thePar);
    }
    aLen += aP1.Distance(aP2);
    aP1 = aP2;
    aPar += aDPar;
  }
  return aLen;
}

static void CorrectNbSamples(const Adaptor3d_Surface& theS,
                             const Standard_Real      theU1,
                             const Standard_Real      theU2,
                             Standard_Integer&        theNbU,
                             const Standard_Real      theV1,
                             const Standard_Real      theV2,
                             Standard_Integer&        theNbV)
{
  Standard_Real    aMinLen = 1.e-3;
  Standard_Integer aNbP    = std::min(23, theNbV);
  Standard_Real    aLenU1  = LengthOfIso(theS, GeomAbs_IsoU, theV1, theV2, aNbP, theU1);
  if (aLenU1 <= aMinLen)
  {
    Standard_Real aL =
      LengthOfIso(theS, GeomAbs_IsoU, theV1, theV2, aNbP, .7 * theU1 + 0.3 * theU2);
    aLenU1 = std::max(aL, aLenU1);
  }
  Standard_Real aLenU2 = LengthOfIso(theS, GeomAbs_IsoU, theV1, theV2, aNbP, theU2);
  if (aLenU2 <= aMinLen)
  {
    Standard_Real aL =
      LengthOfIso(theS, GeomAbs_IsoU, theV1, theV2, aNbP, .3 * theU1 + 0.7 * theU2);
    aLenU2 = std::max(aL, aLenU2);
  }
  aNbP                 = std::min(23, theNbV);
  Standard_Real aLenV1 = LengthOfIso(theS, GeomAbs_IsoV, theU1, theU2, aNbP, theV1);
  if (aLenV1 <= aMinLen)
  {
    Standard_Real aL =
      LengthOfIso(theS, GeomAbs_IsoV, theU1, theU2, aNbP, .7 * theV1 + 0.3 * theV2);
    aLenV1 = std::max(aL, aLenV1);
  }
  Standard_Real aLenV2 = LengthOfIso(theS, GeomAbs_IsoV, theU1, theU2, aNbP, theV2);
  if (aLenV2 <= aMinLen)
  {
    Standard_Real aL =
      LengthOfIso(theS, GeomAbs_IsoV, theU1, theU2, aNbP, .3 * theV1 + 0.7 * theV2);
    aLenV2 = std::max(aL, aLenV2);
  }
  //
  Standard_Real aStepV1 = aLenU1 / theNbV;
  Standard_Real aStepV2 = aLenU2 / theNbV;
  Standard_Real aStepU1 = aLenV1 / theNbU;
  Standard_Real aStepU2 = aLenV2 / theNbU;

  Standard_Real aMaxStepV = std::max(aStepV1, aStepV2);
  Standard_Real aMaxStepU = std::max(aStepU1, aStepU2);
  //
  Standard_Real aRatio = aMaxStepV / aMaxStepU;
  if (aRatio > 10.)
  {
    Standard_Integer aMult = RealToInt(std::log(aRatio));
    if (aMult > 1)
    {
      theNbV *= aMult;
    }
  }
  else if (aRatio < 0.1)
  {
    Standard_Integer aMult = RealToInt(-std::log(aRatio));
    if (aMult > 1)
    {
      theNbV *= aMult;
    }
  }
}

void Extrema_GenExtPS::BuildBVH()
{
  // if BVH already exists, assume it is already correctly filled
  if (myBVHSet.Size() > 0)
  {
    return;
  }

  Standard_Boolean            anIsBSpline = (myS->GetType() == GeomAbs_BSplineSurface);
  Handle(Geom_BSplineSurface) aBspl;

  if (anIsBSpline)
  {
    aBspl                    = myS->BSpline();
    Standard_Integer aUValue = aBspl->UDegree() * aBspl->NbUKnots();
    Standard_Integer aVValue = aBspl->VDegree() * aBspl->NbVKnots();
    // 300 is value, which is used for singular points (see Extrema_ExtPS.cxx::Initialize(...))
    if (aUValue > myusample)
    {
      myusample = std::min(aUValue, 300);
    }
    if (aVValue > myvsample)
    {
      myvsample = std::min(aVValue, 300);
    }
  }

  CorrectNbSamples(*myS, myumin, myusup, myusample, myvmin, myvsup, myvsample);

  Standard_Real aPasU, aU0;
  Standard_Real aPasV, aV0;
  computeGridParameters(myumin, myusup, myusample, aPasU, aU0);
  computeGridParameters(myvmin, myvsup, myvsample, aPasV, aV0);

  // Try optimized BSpline evaluation (pre-computed span indices avoid repeated binary search)
  if (anIsBSpline && !aBspl.IsNull())
  {
    BSplSLib_GridEvaluator      anEval;
    const TColStd_Array2OfReal* aWeights = aBspl->Weights();
    anEval.Initialize(aBspl->UDegree(),
                      aBspl->VDegree(),
                      aBspl->Poles(),
                      aWeights,
                      aBspl->UKnotSequence(),
                      aBspl->VKnotSequence(),
                      aBspl->IsURational(),
                      aBspl->IsVRational(),
                      aBspl->IsUPeriodic(),
                      aBspl->IsVPeriodic());

    if (anEval.IsInitialized())
    {
      anEval.PrepareUParams(aU0, aU0 + aPasU * (myusample - 1), myusample);
      anEval.PrepareVParams(aV0, aV0 + aPasV * (myvsample - 1), myvsample);

      // Use actual parameter count from evaluator (may differ from requested)
      const Standard_Integer aNbU = anEval.NbUParams();
      const Standard_Integer aNbV = anEval.NbVParams();

      if (aNbU >= 2 && aNbV >= 2)
      {
        // Update sample counts with actual values
        myusample = aNbU;
        myvsample = aNbV;

        // Allocate arrays with actual sizes
        myUParams = new TColStd_HArray1OfReal(1, aNbU);
        myVParams = new TColStd_HArray1OfReal(1, aNbV);
        myPoints.Resize(0, aNbU + 1, 0, aNbV + 1, false);
        myBVHSet.SetSize(static_cast<Standard_Size>(aNbU * aNbV));

        // Fill parameter arrays from evaluator
        for (Standard_Integer anI = 1; anI <= aNbU; ++anI)
        {
          myUParams->SetValue(anI, anEval.UParam(anI));
        }
        for (Standard_Integer anI = 1; anI <= aNbV; ++anI)
        {
          myVParams->SetValue(anI, anEval.VParam(anI));
        }

        // Evaluate grid points and build BVH
        for (Standard_Integer aNoU = 1; aNoU <= aNbU; ++aNoU)
        {
          for (Standard_Integer aNoV = 1; aNoV <= aNbV; ++aNoV)
          {
            gp_Pnt aPnt;
            anEval.D0(aNoU, aNoV, aPnt);
            addGridPoint(aNoU, aNoV, aPnt, Standard_True);
          }
        }
        myBVHSet.Build();
        return;
      }
    }
  }

  // Fallback: standard evaluation for non-BSpline surfaces or failed BSpline initialization
  // Allocate arrays with requested sizes
  myUParams = new TColStd_HArray1OfReal(1, myusample);
  myVParams = new TColStd_HArray1OfReal(1, myvsample);
  myPoints.Resize(0, myusample + 1, 0, myvsample + 1, false);
  myBVHSet.SetSize(static_cast<Standard_Size>(myusample * myvsample));

  populateParamArray(myUParams, aU0, aPasU, myusample);
  populateParamArray(myVParams, aV0, aPasV, myvsample);

  for (Standard_Integer aNoU = 1; aNoU <= myusample; ++aNoU)
  {
    for (Standard_Integer aNoV = 1; aNoV <= myvsample; ++aNoV)
    {
      gp_Pnt aPnt = myS->Value(myUParams->Value(aNoU), myVParams->Value(aNoV));
      addGridPoint(aNoU, aNoV, aPnt, Standard_True);
    }
  }
  myBVHSet.Build();
}

void Extrema_GenExtPS::FindSolution(const gp_Pnt& /*theP*/, const Extrema_POnSurfParams& theParams)
{
  math_Vector aTol(1, 2);
  aTol(1) = mytolu;
  aTol(2) = mytolv;

  math_Vector aUV(1, 2);
  theParams.Parameter(aUV(1), aUV(2));

  math_Vector aUVInf(1, 2);
  math_Vector aUVSup(1, 2);
  aUVInf(1) = myumin;
  aUVInf(2) = myvmin;
  aUVSup(1) = myusup;
  aUVSup(2) = myvsup;

  math_FunctionSetRoot aRoot(myF, aTol);
  aRoot.Perform(myF, aUV, aUVInf, aUVSup);

  myDone = Standard_True;
}

void Extrema_GenExtPS::SetFlag(const Extrema_ExtFlag theF)
{
  myFlag = theF;
}

void Extrema_GenExtPS::SetAlgo(const Extrema_ExtAlgo theA)
{
  if (myAlgo != theA)
  {
    myInit = Standard_False;
  }
  myAlgo = theA;
}

void Extrema_GenExtPS::Perform(const gp_Pnt& theP)
{
  myDone = Standard_False;
  myF.SetPoint(theP);

  if (myAlgo == Extrema_ExtAlgo_Grad)
  {
    BuildGrid(theP);
    Standard_Integer aNoU = 0;
    Standard_Integer aNoV = 0;

    if (myFlag == Extrema_ExtFlag_MIN || myFlag == Extrema_ExtFlag_MINMAX)
    {
      Extrema_ElementType anElemType;
      Standard_Integer    anIdxU  = 0;
      Standard_Integer    anIdxV  = 0;
      Standard_Integer    anIdxU2 = 0;
      Standard_Integer    anIdxV2 = 0;
      Standard_Boolean    anIsMin = Standard_False;

      for (aNoU = 1; aNoU < myusample; aNoU++)
      {
        for (aNoV = 1; aNoV < myvsample; aNoV++)
        {
          const Extrema_POnSurfParams& aParam = myFacePntParams.Value(aNoU, aNoV);

          anIsMin    = Standard_False;
          anElemType = aParam.GetElementType();

          if (anElemType == Extrema_Face)
          {
            anIsMin = Standard_True;
          }
          else
          {
            // Check if it is a boundary edge or corner vertex.
            aParam.GetIndices(anIdxU, anIdxV);

            if (anElemType == Extrema_UIsoEdge)
            {
              anIsMin = (anIdxV == 1 || anIdxV == myvsample);
            }
            else if (anElemType == Extrema_VIsoEdge)
            {
              anIsMin = (anIdxU == 1 || anIdxU == myusample);
            }
            else if (anElemType == Extrema_Node)
            {
              anIsMin =
                (anIdxU == 1 || anIdxU == myusample) && (anIdxV == 1 || anIdxV == myvsample);
            }

            if (!anIsMin)
            {
              // This is a middle element.
              if (anElemType == Extrema_UIsoEdge
                  || (anElemType == Extrema_Node && (anIdxU == 1 || anIdxU == myusample)))
              {
                // Check the down face.
                const Extrema_POnSurfParams& aDownParam = myFacePntParams.Value(aNoU, aNoV - 1);

                if (aDownParam.GetElementType() == anElemType)
                {
                  aDownParam.GetIndices(anIdxU2, anIdxV2);
                  anIsMin = (anIdxU == anIdxU2 && anIdxV == anIdxV2);
                }
              }
              else if (anElemType == Extrema_VIsoEdge
                       || (anElemType == Extrema_Node && (anIdxV == 1 || anIdxV == myvsample)))
              {
                // Check the right face.
                const Extrema_POnSurfParams& aRightParam = myFacePntParams.Value(aNoU - 1, aNoV);

                if (aRightParam.GetElementType() == anElemType)
                {
                  aRightParam.GetIndices(anIdxU2, anIdxV2);
                  anIsMin = (anIdxU == anIdxU2 && anIdxV == anIdxV2);
                }
              }
              else if (anIdxU == aNoU && anIdxV == aNoV)
              {
                // Check the lower-left node. For this purpose it is necessary
                // to check lower-left, lower and left faces.
                anIsMin = Standard_True;

                const Extrema_POnSurfParams* anOtherParam[3] = {
                  &myFacePntParams.Value(aNoU, aNoV - 1),     // Down
                  &myFacePntParams.Value(aNoU - 1, aNoV - 1), // Lower-left
                  &myFacePntParams.Value(aNoU - 1, aNoV)};    // Left

                for (Standard_Integer anIdx = 0; anIdx < 3 && anIsMin; anIdx++)
                {
                  if (anOtherParam[anIdx]->GetElementType() == Extrema_Node)
                  {
                    anOtherParam[anIdx]->GetIndices(anIdxU2, anIdxV2);
                    anIsMin = (anIdxU == anIdxU2 && anIdxV == anIdxV2);
                  }
                  else
                  {
                    anIsMin = Standard_False;
                  }
                }
              }
            }
          }

          if (anIsMin)
          {
            FindSolution(theP, aParam);
          }
        }
      }
    }

    if (myFlag == Extrema_ExtFlag_MAX || myFlag == Extrema_ExtFlag_MINMAX)
    {
      Standard_Real aDist = 0.0;

      for (aNoU = 1; aNoU <= myusample; aNoU++)
      {
        for (aNoV = 1; aNoV <= myvsample; aNoV++)
        {
          const Extrema_POnSurfParams& aParamMain = myPoints.Value(aNoU, aNoV);
          const Extrema_POnSurfParams& aParam1    = myPoints.Value(aNoU - 1, aNoV - 1);
          const Extrema_POnSurfParams& aParam2    = myPoints.Value(aNoU - 1, aNoV);
          const Extrema_POnSurfParams& aParam3    = myPoints.Value(aNoU - 1, aNoV + 1);
          const Extrema_POnSurfParams& aParam4    = myPoints.Value(aNoU, aNoV - 1);
          const Extrema_POnSurfParams& aParam5    = myPoints.Value(aNoU, aNoV + 1);
          const Extrema_POnSurfParams& aParam6    = myPoints.Value(aNoU + 1, aNoV - 1);
          const Extrema_POnSurfParams& aParam7    = myPoints.Value(aNoU + 1, aNoV);
          const Extrema_POnSurfParams& aParam8    = myPoints.Value(aNoU + 1, aNoV + 1);

          aDist = aParamMain.GetSqrDistance();

          if ((aParam1.GetSqrDistance() <= aDist) && (aParam2.GetSqrDistance() <= aDist)
              && (aParam3.GetSqrDistance() <= aDist) && (aParam4.GetSqrDistance() <= aDist)
              && (aParam5.GetSqrDistance() <= aDist) && (aParam6.GetSqrDistance() <= aDist)
              && (aParam7.GetSqrDistance() <= aDist) && (aParam8.GetSqrDistance() <= aDist))
          {
            // Find maximum.
            FindSolution(theP, myPoints.Value(aNoU, aNoV));
          }
        }
      }
    }
  }
  else
  {
    BuildBVH();

    // Create BVH traversers
    Extrema_PSBVHMinDistanceSelector aMinSelector(myPoints);
    Extrema_PSBVHMaxDistanceSelector aMaxSelector(myPoints);
    aMinSelector.SetBVHSet(&myBVHSet);
    aMaxSelector.SetBVHSet(&myBVHSet);

    if (myFlag == Extrema_ExtFlag_MIN || myFlag == Extrema_ExtFlag_MINMAX)
    {
      aMinSelector.SetQueryPoint(theP);
      aMinSelector.Select();

      if (aMinSelector.ClosestU() > 0 && aMinSelector.ClosestV() > 0)
      {
        Standard_Real         aU = myUParams->Value(aMinSelector.ClosestU());
        Standard_Real         aV = myVParams->Value(aMinSelector.ClosestV());
        Extrema_POnSurfParams aParams(aU, aV, myS->Value(aU, aV));
        aParams.SetSqrDistance(theP.SquareDistance(aParams.Value()));
        aParams.SetIndices(aMinSelector.ClosestU(), aMinSelector.ClosestV());
        FindSolution(theP, aParams);
      }
    }
    if (myFlag == Extrema_ExtFlag_MAX || myFlag == Extrema_ExtFlag_MINMAX)
    {
      aMaxSelector.SetQueryPoint(theP);
      aMaxSelector.Select();

      if (aMaxSelector.FarthestU() > 0 && aMaxSelector.FarthestV() > 0)
      {
        Standard_Real         aU = myUParams->Value(aMaxSelector.FarthestU());
        Standard_Real         aV = myVParams->Value(aMaxSelector.FarthestV());
        Extrema_POnSurfParams aParams(aU, aV, myS->Value(aU, aV));
        aParams.SetSqrDistance(theP.SquareDistance(aParams.Value()));
        aParams.SetIndices(aMaxSelector.FarthestU(), aMaxSelector.FarthestV());
        FindSolution(theP, aParams);
      }
    }
  }
}

//==================================================================================================

Standard_Boolean Extrema_GenExtPS::IsDone() const
{
  return myDone;
}

//==================================================================================================

Standard_Integer Extrema_GenExtPS::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myF.NbExt();
}

//==================================================================================================

Standard_Real Extrema_GenExtPS::SquareDistance(const Standard_Integer theN) const
{
  if ((theN < 1) || (theN > NbExt()))
  {
    throw Standard_OutOfRange();
  }

  return myF.SquareDistance(theN);
}

//==================================================================================================

const Extrema_POnSurf& Extrema_GenExtPS::Point(const Standard_Integer theN) const
{
  if ((theN < 1) || (theN > NbExt()))
  {
    throw Standard_OutOfRange();
  }

  return myF.Point(theN);
}

//==================================================================================================
