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

#include <Adaptor3d_Surface.hxx>
#include <BSplSLib_GridEvaluator.hxx>
#include <BVH_BinnedBuilder.hxx>
#include <BVH_BoxSet.hxx>
#include <BVH_Tools.hxx>
#include <BVH_Traverse.hxx>
#include <Extrema_GenExtSS.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <math_BFGS.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
#include <math_Vector.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//! This class represents distance objective function for surface / surface.
class Extrema_FuncDistSS : public math_MultipleVarFunctionWithGradient
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Extrema_FuncDistSS(const Adaptor3d_Surface& S1, const Adaptor3d_Surface& S2)
      : myS1(&S1),
        myS2(&S2)
  {
  }

  Standard_EXPORT Standard_Integer NbVariables() const { return 4; }

  Standard_EXPORT virtual Standard_Boolean Value(const math_Vector& X, Standard_Real& F)
  {
    F = myS1->Value(X(1), X(2)).SquareDistance(myS2->Value(X(3), X(4)));
    return true;
  }

  Standard_EXPORT Standard_Boolean Gradient(const math_Vector& X, math_Vector& G)
  {
    gp_Pnt P1, P2;
    gp_Vec Du1s1, Dv1s1;
    gp_Vec Du2s2, Dv2s2;
    myS1->D1(X(1), X(2), P1, Du1s1, Dv1s1);
    myS2->D1(X(3), X(4), P2, Du2s2, Dv2s2);

    gp_Vec P1P2(P2, P1);

    G(1) = P1P2.Dot(Du1s1);
    G(2) = P1P2.Dot(Dv1s1);
    G(3) = -P1P2.Dot(Du2s2);
    G(4) = -P1P2.Dot(Dv2s2);

    return true;
  }

  Standard_EXPORT virtual Standard_Boolean Values(const math_Vector& X,
                                                  Standard_Real&     F,
                                                  math_Vector&       G)
  {
    F = myS1->Value(X(1), X(2)).SquareDistance(myS2->Value(X(3), X(4)));

    gp_Pnt P1, P2;
    gp_Vec Du1s1, Dv1s1;
    gp_Vec Du2s2, Dv2s2;
    myS1->D1(X(1), X(2), P1, Du1s1, Dv1s1);
    myS2->D1(X(3), X(4), P2, Du2s2, Dv2s2);

    gp_Vec P1P2(P2, P1);

    G(1) = P1P2.Dot(Du1s1);
    G(2) = P1P2.Dot(Dv1s1);
    G(3) = -P1P2.Dot(Du2s2);
    G(4) = -P1P2.Dot(Dv2s2);

    return true;
  }

protected:
private:
  const Adaptor3d_Surface* myS1;
  const Adaptor3d_Surface* myS2;
};

//! BVH traverser for finding the closest point to a given point.
//! Uses BVH_Tools::PointBoxSquareDistance for efficient pruning.
class Extrema_BVHMinDistanceSelector
    : public BVH_Traverse<Standard_Real,
                          3,
                          BVH_BoxSet<Standard_Real, 3, Extrema_GenExtSS::UVIndex>,
                          Standard_Real>
{
public:
  Extrema_BVHMinDistanceSelector(const Handle(TColgp_HArray2OfPnt)& thePoints)
      : myPoints(thePoints),
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
    const Extrema_GenExtSS::UVIndex& anIdx = this->myBVHSet->Element(theIndex);
    const gp_Pnt&                    aPnt  = myPoints->Value(anIdx.first, anIdx.second);
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
  Handle(TColgp_HArray2OfPnt) myPoints;
  BVH_Vec3d                   myQueryPoint;
  mutable Standard_Real       myMinDistSq;
  mutable Standard_Integer    myClosestU;
  mutable Standard_Integer    myClosestV;
};

//! BVH traverser for finding the farthest point from a given point.
class Extrema_BVHMaxDistanceSelector
    : public BVH_Traverse<Standard_Real,
                          3,
                          BVH_BoxSet<Standard_Real, 3, Extrema_GenExtSS::UVIndex>,
                          Standard_Real>
{
public:
  Extrema_BVHMaxDistanceSelector(const Handle(TColgp_HArray2OfPnt)& thePoints)
      : myPoints(thePoints),
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
    const Extrema_GenExtSS::UVIndex& anIdx = this->myBVHSet->Element(theIndex);
    const gp_Pnt&                    aPnt  = myPoints->Value(anIdx.first, anIdx.second);
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
  Handle(TColgp_HArray2OfPnt) myPoints;
  BVH_Vec3d                   myQueryPoint;
  mutable Standard_Real       myMaxDistSq;
  mutable Standard_Integer    myFarthestU;
  mutable Standard_Integer    myFarthestV;
};

//! Helper function to build grid for BSpline surface using optimized evaluator.
//! @param theSurf    BSpline surface
//! @param thePoints  output array of grid points
//! @param theUMin    minimum U parameter
//! @param theUMax    maximum U parameter
//! @param theVMin    minimum V parameter
//! @param theVMax    maximum V parameter
//! @param theNbU     number of U samples (updated on output)
//! @param theNbV     number of V samples (updated on output)
//! @return true if grid was built successfully
static Standard_Boolean buildBSplineGridSS(const Handle(Geom_BSplineSurface)& theSurf,
                                           Handle(TColgp_HArray2OfPnt)&       thePoints,
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
  anEval.Initialize(theSurf->UDegree(),
                    theSurf->VDegree(),
                    theSurf->Poles(),
                    theSurf->Weights(),
                    theSurf->UKnotSequence(),
                    theSurf->VKnotSequence(),
                    theSurf->IsURational(),
                    theSurf->IsVRational(),
                    theSurf->IsUPeriodic(),
                    theSurf->IsVPeriodic());

  if (!anEval.IsInitialized())
  {
    return Standard_False;
  }

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

  // Allocate points array (0-based to match original code)
  thePoints = new TColgp_HArray2OfPnt(0, aNbU + 1, 0, aNbV + 1);

  // Evaluate grid points using pre-computed span indices
  for (Standard_Integer iu = 1; iu <= aNbU; ++iu)
  {
    for (Standard_Integer iv = 1; iv <= aNbV; ++iv)
    {
      thePoints->SetValue(iu, iv, anEval.Value(iu, iv));
    }
  }

  return Standard_True;
}

//=================================================================================================

Extrema_GenExtSS::Extrema_GenExtSS()
    : myu1min(0.0),
      myu1sup(0.0),
      myv1min(0.0),
      myv1sup(0.0),
      myu2min(0.0),
      myu2sup(0.0),
      myv2min(0.0),
      myv2sup(0.0),
      myusample(0),
      myvsample(0),
      mytol1(0.0),
      mytol2(0.0),
      myS2(NULL)
{
  myDone = Standard_False;
  myInit = Standard_False;
}

//=================================================================================================

Extrema_GenExtSS::~Extrema_GenExtSS()
{
  //
}

//=================================================================================================

Extrema_GenExtSS::Extrema_GenExtSS(const Adaptor3d_Surface& S1,
                                   const Adaptor3d_Surface& S2,
                                   const Standard_Integer   NbU,
                                   const Standard_Integer   NbV,
                                   const Standard_Real      Tol1,
                                   const Standard_Real      Tol2)
    : myF(S1, S2)
{
  Initialize(S2, NbU, NbV, Tol2);
  Perform(S1, Tol1);
}

//=================================================================================================

Extrema_GenExtSS::Extrema_GenExtSS(const Adaptor3d_Surface& S1,
                                   const Adaptor3d_Surface& S2,
                                   const Standard_Integer   NbU,
                                   const Standard_Integer   NbV,
                                   const Standard_Real      U1min,
                                   const Standard_Real      U1sup,
                                   const Standard_Real      V1min,
                                   const Standard_Real      V1sup,
                                   const Standard_Real      U2min,
                                   const Standard_Real      U2sup,
                                   const Standard_Real      V2min,
                                   const Standard_Real      V2sup,
                                   const Standard_Real      Tol1,
                                   const Standard_Real      Tol2)
    : myF(S1, S2)
{
  Initialize(S2, NbU, NbV, U2min, U2sup, V2min, V2sup, Tol2);
  Perform(S1, U1min, U1sup, V1min, V1sup, Tol1);
}

//=================================================================================================

void Extrema_GenExtSS::Initialize(const Adaptor3d_Surface& S2,
                                  const Standard_Integer   NbU,
                                  const Standard_Integer   NbV,
                                  const Standard_Real      Tol2)
{
  myu2min = S2.FirstUParameter();
  myu2sup = S2.LastUParameter();
  myv2min = S2.FirstVParameter();
  myv2sup = S2.LastVParameter();
  Initialize(S2, NbU, NbV, myu2min, myu2sup, myv2min, myv2sup, Tol2);
}

//=================================================================================================

void Extrema_GenExtSS::Initialize(const Adaptor3d_Surface& S2,
                                  const Standard_Integer   NbU,
                                  const Standard_Integer   NbV,
                                  const Standard_Real      U2min,
                                  const Standard_Real      U2sup,
                                  const Standard_Real      V2min,
                                  const Standard_Real      V2sup,
                                  const Standard_Real      Tol2)
{
  myS2      = &S2;
  myusample = NbU;
  myvsample = NbV;
  myu2min   = U2min;
  myu2sup   = U2sup;
  myv2min   = V2min;
  myv2sup   = V2sup;
  mytol2    = Tol2;

  // Try optimized path for BSpline surfaces
  Standard_Boolean isGridBuilt = Standard_False;
  if (S2.GetType() == GeomAbs_BSplineSurface)
  {
    Handle(Geom_BSplineSurface) aBspl = S2.BSpline();
    Standard_Integer            aNbU  = myusample;
    Standard_Integer            aNbV  = myvsample;
    isGridBuilt =
      buildBSplineGridSS(aBspl, mypoints2, myu2min, myu2sup, myv2min, myv2sup, aNbU, aNbV);
    if (isGridBuilt)
    {
      myusample = aNbU;
      myvsample = aNbV;
    }
  }

  // Allocate points1 array with same dimensions
  mypoints1 = new TColgp_HArray2OfPnt(0, myusample + 1, 0, myvsample + 1);

  // Fall back to standard evaluation if optimization was not applied
  if (!isGridBuilt)
  {
    mypoints2 = new TColgp_HArray2OfPnt(0, myusample + 1, 0, myvsample + 1);

    // Parametrage de l echantillon sur S2
    Standard_Real PasU = myu2sup - myu2min;
    Standard_Real PasV = myv2sup - myv2min;
    Standard_Real U0   = PasU / myusample / 100.;
    Standard_Real V0   = PasV / myvsample / 100.;
    gp_Pnt        P1;
    PasU = (PasU - U0) / (myusample - 1);
    PasV = (PasV - V0) / (myvsample - 1);
    U0   = myu2min + U0 / 2.;
    V0   = myv2min + V0 / 2.;

    // Calcul des distances
    Standard_Integer NoU, NoV;
    Standard_Real    U, V;
    for (NoU = 1, U = U0; NoU <= myusample; NoU++, U += PasU)
    {
      for (NoV = 1, V = V0; NoV <= myvsample; NoV++, V += PasV)
      {
        P1 = myS2->Value(U, V);
        mypoints2->SetValue(NoU, NoV, P1);
      }
    }
  }

  // Build BVH for S2 points for efficient spatial queries during Perform()
  myBVHSet2.Clear();
  myBVHSet2.SetSize(static_cast<Standard_Size>(myusample * myvsample));

  for (Standard_Integer iu = 1; iu <= myusample; ++iu)
  {
    for (Standard_Integer iv = 1; iv <= myvsample; ++iv)
    {
      const gp_Pnt&             aPnt = mypoints2->Value(iu, iv);
      BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(aPnt.X(), aPnt.Y(), aPnt.Z()),
                                     BVH_Vec3d(aPnt.X(), aPnt.Y(), aPnt.Z()));
      myBVHSet2.Add(std::make_pair(iu, iv), aBox);
    }
  }
  myBVHSet2.Build();

  myInit = Standard_True;
}

//=================================================================================================

void Extrema_GenExtSS::Perform(const Adaptor3d_Surface& S1, const Standard_Real Tol1)
{
  myu1min = S1.FirstUParameter();
  myu1sup = S1.LastUParameter();
  myv1min = S1.FirstVParameter();
  myv1sup = S1.LastVParameter();
  Perform(S1, myu1min, myu1sup, myv1min, myv1sup, Tol1);
}

//=================================================================================================

void Extrema_GenExtSS::Perform(const Adaptor3d_Surface& S1,
                               const Standard_Real      U1min,
                               const Standard_Real      U1sup,
                               const Standard_Real      V1min,
                               const Standard_Real      V1sup,
                               const Standard_Real      Tol1)
{
  myF.Initialize(S1, *myS2);
  myu1min = U1min;
  myu1sup = U1sup;
  myv1min = V1min;
  myv1sup = V1sup;
  mytol1  = Tol1;

  Standard_Real    U1, V1;
  Standard_Integer NoU1, NoV1;
  gp_Pnt           P1;

  // Try optimized path for S1 if it's a BSpline surface
  Standard_Boolean isS1GridBuilt = Standard_False;
  if (S1.GetType() == GeomAbs_BSplineSurface)
  {
    Handle(Geom_BSplineSurface) aBspl = S1.BSpline();
    Standard_Integer            aNbU  = myusample;
    Standard_Integer            aNbV  = myvsample;
    isS1GridBuilt =
      buildBSplineGridSS(aBspl, mypoints1, myu1min, myu1sup, myv1min, myv1sup, aNbU, aNbV);
    // Note: We don't update myusample/myvsample here as they were set by Initialize
  }

  // Parametrage de l echantillon sur S1
  Standard_Real PasU1 = myu1sup - myu1min;
  Standard_Real PasV1 = myv1sup - myv1min;
  Standard_Real U10   = PasU1 / myusample / 100.;
  Standard_Real V10   = PasV1 / myvsample / 100.;
  PasU1               = (PasU1 - U10) / (myusample - 1);
  PasV1               = (PasV1 - V10) / (myvsample - 1);
  U10                 = myu1min + U10 / 2.;
  V10                 = myv1min + V10 / 2.;

  Standard_Real PasU2 = myu2sup - myu2min;
  Standard_Real PasV2 = myv2sup - myv2min;
  Standard_Real U20   = PasU2 / myusample / 100.;
  Standard_Real V20   = PasV2 / myvsample / 100.;
  PasU2               = (PasU2 - U20) / (myusample - 1);
  PasV2               = (PasV2 - V20) / (myvsample - 1);
  U20                 = myu2min + U20 / 2.;
  V20                 = myv2min + V20 / 2.;

  // Fall back to standard evaluation for S1 if optimization was not applied
  if (!isS1GridBuilt)
  {
    for (NoU1 = 1, U1 = U10; NoU1 <= myusample; NoU1++, U1 += PasU1)
    {
      for (NoV1 = 1, V1 = V10; NoV1 <= myvsample; NoV1++, V1 += PasV1)
      {
        P1 = S1.Value(U1, V1);
        mypoints1->SetValue(NoU1, NoV1, P1);
      }
    }
  }

  /*
  b- Calcul des minima:
     -----------------
     b.a) Initialisations:
  */

  math_Vector Tol(1, 4);
  Tol(1) = mytol1;
  Tol(2) = mytol1;
  Tol(3) = mytol2;
  Tol(4) = mytol2;
  math_Vector UV(1, 4), UVinf(1, 4), UVsup(1, 4);
  UVinf(1) = myu1min;
  UVinf(2) = myv1min;
  UVinf(3) = myu2min;
  UVinf(4) = myv2min;
  UVsup(1) = myu1sup;
  UVsup(2) = myv1sup;
  UVsup(3) = myu2sup;
  UVsup(4) = myv2sup;

  Standard_Real distmin = RealLast(), distmax = 0.0;

  Standard_Integer N1Umin = 0, N1Vmin = 0, N2Umin = 0, N2Vmin = 0;
  Standard_Integer N1Umax = 0, N1Vmax = 0, N2Umax = 0, N2Vmax = 0;

  // Create BVH traversers using pre-built BVH from Initialize()
  Extrema_BVHMinDistanceSelector aMinSelector(mypoints2);
  Extrema_BVHMaxDistanceSelector aMaxSelector(mypoints2);
  aMinSelector.SetBVHSet(&myBVHSet2);
  aMaxSelector.SetBVHSet(&myBVHSet2);

  // Find grid points with minimum and maximum distance using BVH traversal
  // Complexity: O(N² log N) instead of O(N⁴)
  for (NoU1 = 1; NoU1 <= myusample; ++NoU1)
  {
    for (NoV1 = 1; NoV1 <= myvsample; ++NoV1)
    {
      const gp_Pnt& aP1 = mypoints1->Value(NoU1, NoV1);

      // Find closest point on S2 using BVH
      // Reset() is essential to clear stale pruning bounds from previous queries
      aMinSelector.SetQueryPoint(aP1);
      aMinSelector.Reset();
      aMinSelector.Select();
      if (aMinSelector.MinSquareDistance() < distmin)
      {
        distmin = aMinSelector.MinSquareDistance();
        N1Umin  = NoU1;
        N1Vmin  = NoV1;
        N2Umin  = aMinSelector.ClosestU();
        N2Vmin  = aMinSelector.ClosestV();
      }

      // Find farthest point on S2 using BVH
      // Reset() is essential to clear stale pruning bounds from previous queries
      aMaxSelector.SetQueryPoint(aP1);
      aMaxSelector.Reset();
      aMaxSelector.Select();
      if (aMaxSelector.MaxSquareDistance() > distmax)
      {
        distmax = aMaxSelector.MaxSquareDistance();
        N1Umax  = NoU1;
        N1Vmax  = NoV1;
        N2Umax  = aMaxSelector.FarthestU();
        N2Vmax  = aMaxSelector.FarthestV();
      }
    }
  }

  UV(1) = U10 + (N1Umin - 1) * PasU1;
  UV(2) = V10 + (N1Vmin - 1) * PasV1;
  UV(3) = U20 + (N2Umin - 1) * PasU2;
  UV(4) = V20 + (N2Vmin - 1) * PasV2;

  Extrema_FuncDistSS aGFSS(S1, *myS2);
  math_BFGS          aBFGSSolver(4);
  aBFGSSolver.Perform(aGFSS, UV);
  if (aBFGSSolver.IsDone())
  {
    aBFGSSolver.Location(UV);

    //  Store result in myF.
    myF.Value(UV, UV);
    myF.GetStateNumber();
  }
  else
  {
    // If optimum is not computed successfully then compute by old approach.

    // Restore initial point.
    UV(1) = U10 + (N1Umin - 1) * PasU1;
    UV(2) = V10 + (N1Vmin - 1) * PasV1;
    UV(3) = U20 + (N2Umin - 1) * PasU2;
    UV(4) = V20 + (N2Vmin - 1) * PasV2;

    math_FunctionSetRoot SR1(myF, Tol);
    SR1.Perform(myF, UV, UVinf, UVsup);
  }

  // math_FunctionSetRoot SR1(myF, Tol);
  // SR1.Perform(myF, UV, UVinf, UVsup);

  UV(1) = U10 + (N1Umax - 1) * PasU1;
  UV(2) = V10 + (N1Vmax - 1) * PasV1;
  UV(3) = U20 + (N2Umax - 1) * PasU2;
  UV(4) = V20 + (N2Vmax - 1) * PasV2;

  // It is impossible to compute max distance in the same manner,
  // since for the distance functional for max have bad definition.
  // So, for max computation old approach is used.
  math_FunctionSetRoot SR2(myF, Tol);
  SR2.Perform(myF, UV, UVinf, UVsup);

  myDone = Standard_True;
}

//=================================================================================================

Standard_Boolean Extrema_GenExtSS::IsDone() const
{
  return myDone;
}

//=================================================================================================

Standard_Integer Extrema_GenExtSS::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myF.NbExt();
}

//=================================================================================================

Standard_Real Extrema_GenExtSS::SquareDistance(const Standard_Integer N) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  return myF.SquareDistance(N);
}

//=================================================================================================

const Extrema_POnSurf& Extrema_GenExtSS::PointOnS1(const Standard_Integer N) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  return myF.PointOnS1(N);
}

//=================================================================================================

const Extrema_POnSurf& Extrema_GenExtSS::PointOnS2(const Standard_Integer N) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  return myF.PointOnS2(N);
}
