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

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Bnd_Sphere.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomAbs_IsoType.hxx>
#include <GeomGridEval_Surface.hxx>
#include <Extrema_ExtFlag.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_Handle.hxx>
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
#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>

// IMPLEMENT_HARRAY1(Extrema_HArray1OfSphere)
class Bnd_SphereUBTreeSelector : public NCollection_UBTree<int, Bnd_Sphere>::Selector
{
public:
  Bnd_SphereUBTreeSelector(const occ::handle<NCollection_HArray1<Bnd_Sphere>>& theSphereArray,
                           Bnd_Sphere&                                         theSol)
      : myXYZ(0, 0, 0),
        mySphereArray(theSphereArray),
        mySol(theSol)
  {
  }

  void DefineCheckPoint(const gp_Pnt& theXYZ) { myXYZ = theXYZ; }

  Bnd_Sphere& Sphere() const { return mySol; }

  bool Reject(const Bnd_Sphere& theBnd) const override = 0;

  bool Accept(const int& theObj) override = 0;

protected:
  gp_Pnt                                              myXYZ;
  const occ::handle<NCollection_HArray1<Bnd_Sphere>>& mySphereArray;
  Bnd_Sphere&                                         mySol;

private:
  void operator=(const Bnd_SphereUBTreeSelector&) = delete;
};

class Bnd_SphereUBTreeSelectorMin : public Bnd_SphereUBTreeSelector
{
public:
  Bnd_SphereUBTreeSelectorMin(const occ::handle<NCollection_HArray1<Bnd_Sphere>>& theSphereArray,
                              Bnd_Sphere&                                         theSol)
      : Bnd_SphereUBTreeSelector(theSphereArray, theSol),
        myMinDist(RealLast())
  {
  }

  void SetMinDist(const double theMinDist) { myMinDist = theMinDist; }

  double MinDist() const { return myMinDist; }

  bool Reject(const Bnd_Sphere& theBnd) const override
  {
    Bnd_SphereUBTreeSelectorMin* me = const_cast<Bnd_SphereUBTreeSelectorMin*>(this);
    // myMinDist is decreased each time a nearer object is found
    return theBnd.IsOut(myXYZ.XYZ(), me->myMinDist);
  }

  bool Accept(const int&) override;

private:
  double myMinDist;
};

bool Bnd_SphereUBTreeSelectorMin::Accept(const int& theInd)
{
  const Bnd_Sphere& aSph = mySphereArray->Value(theInd);
  double            aCurDist;

  //    if ( (aCurDist = aSph.SquareDistance(myXYZ.XYZ())) < mySol.SquareDistance(myXYZ.XYZ()) )
  if ((aCurDist = aSph.Distance(myXYZ.XYZ())) < mySol.Distance(myXYZ.XYZ()))
  {
    mySol = aSph;
    if (aCurDist < myMinDist)
      myMinDist = aCurDist;

    return true;
  }

  return false;
}

class Bnd_SphereUBTreeSelectorMax : public Bnd_SphereUBTreeSelector
{
public:
  Bnd_SphereUBTreeSelectorMax(const occ::handle<NCollection_HArray1<Bnd_Sphere>>& theSphereArray,
                              Bnd_Sphere&                                         theSol)
      : Bnd_SphereUBTreeSelector(theSphereArray, theSol),
        myMaxDist(0)
  {
  }

  void SetMaxDist(const double theMaxDist) { myMaxDist = theMaxDist; }

  double MaxDist() const { return myMaxDist; }

  bool Reject(const Bnd_Sphere& theBnd) const override
  {
    Bnd_SphereUBTreeSelectorMax* me = const_cast<Bnd_SphereUBTreeSelectorMax*>(this);
    // myMaxDist is decreased each time a nearer object is found
    return theBnd.IsOut(myXYZ.XYZ(), me->myMaxDist);
  }

  bool Accept(const int&) override;

private:
  double myMaxDist;
};

bool Bnd_SphereUBTreeSelectorMax::Accept(const int& theInd)
{
  const Bnd_Sphere& aSph = mySphereArray->Value(theInd);
  double            aCurDist;

  //    if ( (aCurDist = aSph.SquareDistance(myXYZ.XYZ())) > mySol.SquareDistance(myXYZ.XYZ()) )
  if ((aCurDist = aSph.Distance(myXYZ.XYZ())) > mySol.Distance(myXYZ.XYZ()))
  {
    mySol = aSph;
    if (aCurDist > myMaxDist)
      myMaxDist = aCurDist;

    return true;
  }

  return false;
}

//=============================================================================

/*-----------------------------------------------------------------------------
Function:
   Find all extremum distances between point P and surface
  S using sampling (NbU,NbV).

Method:
   The algorithm bases on the hypothesis that sampling is precise enough,
  if there exist N extreme distances between the point and the surface,
  so there also exist N extrema between the point and the grid.
  So, the algorithm consists in starting from extrema of the grid to find the
  extrema of the surface.
  The extrema are calculated by the algorithm math_FunctionSetRoot with the
  following arguments:
  - F: Extrema_FuncExtPS created from P and S,
  - UV: math_Vector the components which of are parameters of the extremum on the
    grid,
  - Tol: std::min(TolU,TolV), (Prov.:math_FunctionSetRoot does not authorize a vector)
  - UVinf: math_Vector the components which of are lower limits of u and v,
  - UVsup: math_Vector the components which of are upper limits of u and v.

Processing:
  a- Creation of the table of distances (TbDist(0,NbU+1,0,NbV+1)):
     The table is expanded at will; lines 0 and NbU+1 and
     columns 0 and NbV+1 are initialized at RealFirst() or RealLast()
     to simplify the tests carried out at stage b
     (there is no need to test if the point is on border of the grid).
  b- Calculation of extrema:
     First the minimums and then the maximums are found. These 2 procedured
     pass in a similar way:
  b.a- Initialization:
      - 'borders' of table  TbDist (RealLast() in case of minimums
        and  RealLast() in case of maximums),
      - table TbSel(0,NbU+1,0,NbV+1) of selection of points for
        calculation of local extremum (0). When a point will selected,
    it will not be selectable, as well as the adjacent points
    (8 at least). The corresponding addresses will be set to 1.
  b.b- Calculation of minimums (or maximums):
       All distances from table TbDist are parsed in a loop:
      - search minimum (or maximum) in the grid,
      - calculate extremum on the surface,
      - update table TbSel.
-----------------------------------------------------------------------------*/

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
  myDone = false;
  myInit = false;
  myFlag = Extrema_ExtFlag_MINMAX;
  myAlgo = Extrema_ExtAlgo_Grad;
}

//=================================================================================================

Extrema_GenExtPS::~Extrema_GenExtPS() = default;

//=================================================================================================

Extrema_GenExtPS::Extrema_GenExtPS(const gp_Pnt&            P,
                                   const Adaptor3d_Surface& S,
                                   const int                NbU,
                                   const int                NbV,
                                   const double             TolU,
                                   const double             TolV,
                                   const Extrema_ExtFlag    F,
                                   const Extrema_ExtAlgo    A)
    : myF(P, S),
      myFlag(F),
      myAlgo(A)
{
  Initialize(S, NbU, NbV, TolU, TolV);
  Perform(P);
}

Extrema_GenExtPS::Extrema_GenExtPS(const gp_Pnt&            P,
                                   const Adaptor3d_Surface& S,
                                   const int                NbU,
                                   const int                NbV,
                                   const double             Umin,
                                   const double             Usup,
                                   const double             Vmin,
                                   const double             Vsup,
                                   const double             TolU,
                                   const double             TolV,
                                   const Extrema_ExtFlag    F,
                                   const Extrema_ExtAlgo    A)
    : myF(P, S),
      myFlag(F),
      myAlgo(A)
{
  Initialize(S, NbU, NbV, Umin, Usup, Vmin, Vsup, TolU, TolV);
  Perform(P);
}

void Extrema_GenExtPS::Initialize(const Adaptor3d_Surface& S,
                                  const int                NbU,
                                  const int                NbV,
                                  const double             TolU,
                                  const double             TolV)
{
  myumin = S.FirstUParameter();
  myusup = S.LastUParameter();
  myvmin = S.FirstVParameter();
  myvsup = S.LastVParameter();
  Initialize(S, NbU, NbV, myumin, myusup, myvmin, myvsup, TolU, TolV);
}

void Extrema_GenExtPS::Initialize(const Adaptor3d_Surface& S,
                                  const int                NbU,
                                  const int                NbV,
                                  const double             Umin,
                                  const double             Usup,
                                  const double             Vmin,
                                  const double             Vsup,
                                  const double             TolU,
                                  const double             TolV)
{
  myS       = &S;
  myusample = NbU;
  myvsample = NbV;
  mytolu    = TolU;
  mytolv    = TolV;
  myumin    = Umin;
  myusup    = Usup;
  myvmin    = Vmin;
  myvsup    = Vsup;

  if ((myusample < 2) || (myvsample < 2))
  {
    throw Standard_OutOfRange();
  }

  myF.Initialize(S);

  mySphereUBTree.Nullify();
  myUParams.Nullify();
  myVParams.Nullify();
  myInit = false;
}

inline static void fillParams(const NCollection_Array1<double>&         theKnots,
                              int                                       theDegree,
                              double                                    theParMin,
                              double                                    theParMax,
                              occ::handle<NCollection_HArray1<double>>& theParams,
                              int                                       theSample)
{
  NCollection_Vector<double> aParams;
  int                        i        = 1;
  double                     aPrevPar = theParMin;
  aParams.Append(aPrevPar);
  // calculation the array of parametric points depending on the knots array variation and degree of
  // given surface
  for (; i < theKnots.Length() && theKnots(i) < (theParMax - Precision::PConfusion()); i++)
  {
    if (theKnots(i + 1) < theParMin + Precision::PConfusion())
      continue;

    double aStep = (theKnots(i + 1) - theKnots(i)) / std::max(theDegree, 2);
    int    k     = 1;
    for (; k <= theDegree; k++)
    {
      double aPar = theKnots(i) + k * aStep;
      if (aPar > theParMax - Precision::PConfusion())
        break;
      if (aPar > aPrevPar + Precision::PConfusion())
      {
        aParams.Append(aPar);
        aPrevPar = aPar;
      }
    }
  }
  aParams.Append(theParMax);
  int nbPar = aParams.Length();
  // in case of an insufficient number of points the grid will be built later
  if (nbPar < theSample)
    return;
  theParams = new NCollection_HArray1<double>(1, nbPar);
  for (i = 0; i < nbPar; i++)
    theParams->SetValue(i + 1, aParams(i));
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
    occ::handle<Geom_BSplineSurface> aBspl = theSurf.BSpline();
    if (!aBspl.IsNull())
    {
      const NCollection_Array1<double>& aUKnots = aBspl->UKnots();
      const NCollection_Array1<double>& aVKnots = aBspl->VKnots();
      fillParams(aUKnots, aBspl->UDegree(), myumin, myusup, myUParams, myusample);
      fillParams(aVKnots, aBspl->VDegree(), myvmin, myvsup, myVParams, myvsample);
    }
  }
  // calculation parametric points for Bezier surfaces
  else if (theSurf.GetType() == GeomAbs_BezierSurface)
  {
    occ::handle<Geom_BezierSurface> aBezier = theSurf.Bezier();
    if (aBezier.IsNull())
      return;

    NCollection_Array1<double> aUKnots(1, 2);
    NCollection_Array1<double> aVKnots(1, 2);
    aBezier->Bounds(aUKnots(1), aUKnots(2), aVKnots(1), aVKnots(2));
    fillParams(aUKnots, aBezier->UDegree(), myumin, myusup, myUParams, myusample);
    fillParams(aVKnots, aBezier->VDegree(), myvmin, myvsup, myVParams, myvsample);
  }
  // creation points for surfaces based on BSpline or Bezier curves
  else if (theSurf.GetType() == GeomAbs_SurfaceOfRevolution
           || theSurf.GetType() == GeomAbs_SurfaceOfExtrusion)
  {
    occ::handle<NCollection_HArray1<double>> anArrKnots;
    int                                      aDegree = 0;
    if (theSurf.BasisCurve()->GetType() == GeomAbs_BSplineCurve)
    {
      occ::handle<Geom_BSplineCurve> aBspl = theSurf.BasisCurve()->BSpline();
      if (!aBspl.IsNull())
      {
        anArrKnots = new NCollection_HArray1<double>(aBspl->Knots());
        aDegree    = aBspl->Degree();
      }
    }
    if (theSurf.BasisCurve()->GetType() == GeomAbs_BezierCurve)
    {
      occ::handle<Geom_BezierCurve> aBez = theSurf.BasisCurve()->Bezier();
      if (!aBez.IsNull())
      {
        anArrKnots = new NCollection_HArray1<double>(1, 2);
        anArrKnots->SetValue(1, aBez->FirstParameter());
        anArrKnots->SetValue(2, aBez->LastParameter());
        aDegree = aBez->Degree();
      }
    }
    if (anArrKnots.IsNull())
      return;
    if (theSurf.GetType() == GeomAbs_SurfaceOfRevolution)
      fillParams(anArrKnots->Array1(), aDegree, myvmin, myvsup, myVParams, myvsample);
    else
      fillParams(anArrKnots->Array1(), aDegree, myumin, myusup, myUParams, myusample);
  }
  // update the number of points in sample
  if (!myUParams.IsNull())
    myusample = myUParams->Length();
  if (!myVParams.IsNull())
    myvsample = myVParams->Length();
}

/*
 * This function computes the point on surface parameters on edge.
 * if it coincides with theParam0 or theParam1, it is returned.
 */
const Extrema_POnSurfParams& Extrema_GenExtPS::ComputeEdgeParameters(
  const bool                   IsUEdge,
  const Extrema_POnSurfParams& theParam0,
  const Extrema_POnSurfParams& theParam1,
  const gp_Pnt&                thePoint,
  const double                 theDiffTol)
{
  const double aSqrDist01 = theParam0.Value().SquareDistance(theParam1.Value());

  if (aSqrDist01 <= theDiffTol)
  {
    // The points are confused. Get the first point and change its type.
    return theParam0;
  }
  else
  {
    const double aDiffDist = std::abs(theParam0.GetSqrDistance() - theParam1.GetSqrDistance());

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
      gp_XYZ aPoP(thePoint.XYZ().Subtracted(theParam0.Value().XYZ()));
      gp_XYZ aPoP1(theParam1.Value().XYZ().Subtracted(theParam0.Value().XYZ()));
      double aRatio = aPoP.Dot(aPoP1) / aSqrDist01;
      double aU[2];
      double aV[2];

      theParam0.Parameter(aU[0], aV[0]);
      theParam1.Parameter(aU[1], aV[1]);

      double aUPar = aU[0];
      double aVPar = aV[0];

      if (IsUEdge)
      {
        aUPar += aRatio * (aU[1] - aU[0]);
      }
      else
      {
        aVPar += aRatio * (aV[1] - aV[0]);
      }

      myGridParam.SetParameters(aUPar, aVPar, myS->Value(aUPar, aVPar));
      int anIndices[2];

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
  // if grid was already built skip its creation
  if (!myInit)
  {
    // build parametric grid in case of a complex surface geometry (BSpline and Bezier surfaces)
    GetGridPoints(*myS);

    // build grid in other cases
    if (myUParams.IsNull())
    {
      double PasU = myusup - myumin;
      double U0   = PasU / myusample / 100.;
      PasU        = (PasU - U0) / (myusample - 1);
      U0          = U0 / 2. + myumin;
      myUParams   = new NCollection_HArray1<double>(1, myusample);
      double U    = U0;
      for (int NoU = 1; NoU <= myusample; NoU++, U += PasU)
        myUParams->SetValue(NoU, U);
    }

    if (myVParams.IsNull())
    {
      double PasV = myvsup - myvmin;
      double V0   = PasV / myvsample / 100.;
      PasV        = (PasV - V0) / (myvsample - 1);
      V0          = V0 / 2. + myvmin;

      myVParams = new NCollection_HArray1<double>(1, myvsample);
      double V  = V0;
      for (int NoV = 1; NoV <= myvsample; NoV++, V += PasV)
        myVParams->SetValue(NoV, V);
    }

    // If flag was changed and extrema not reinitialized Extrema would fail
    myPoints.Resize(0, myusample + 1, 0, myvsample + 1, false);

    GeomGridEval_Surface aGridEval;
    aGridEval.Initialize(*myS);

    NCollection_Array2<gp_Pnt> aGridPoints =
      aGridEval.EvaluateGrid(myUParams->Array1(), myVParams->Array1());

    if (aGridPoints.IsEmpty())
    {
      return;
    }

    for (int NoU = 1; NoU <= myusample; NoU++)
    {
      for (int NoV = 1; NoV <= myvsample; NoV++)
      {
        const gp_Pnt&         aP1 = aGridPoints.Value(NoU, NoV);
        Extrema_POnSurfParams aParam(myUParams->Value(NoU), myVParams->Value(NoV), aP1);
        aParam.SetElementType(Extrema_Node);
        aParam.SetIndices(NoU, NoV);
        myPoints.SetValue(NoU, NoV, aParam);
      }
    }

    myFacePntParams.Resize(0, myusample, 0, myvsample, false);
    myUEdgePntParams.Resize(1, myusample - 1, 1, myvsample, false);
    myVEdgePntParams.Resize(1, myusample, 1, myvsample - 1, false);

    // Fill boundary with negative square distance.
    // It is used for computation of Maximum.
    for (int NoV = 0; NoV <= myvsample + 1; NoV++)
    {
      myPoints.ChangeValue(0, NoV).SetSqrDistance(-1.);
      myPoints.ChangeValue(myusample + 1, NoV).SetSqrDistance(-1.);
    }

    for (int NoU = 1; NoU <= myusample; NoU++)
    {
      myPoints.ChangeValue(NoU, 0).SetSqrDistance(-1.);
      myPoints.ChangeValue(NoU, myvsample + 1).SetSqrDistance(-1.);
    }

    myInit = true;
  }

  // Compute distances to mesh.
  // Step 1. Compute distances to nodes.
  for (int NoU = 1; NoU <= myusample; NoU++)
  {
    for (int NoV = 1; NoV <= myvsample; NoV++)
    {
      Extrema_POnSurfParams& aParam = myPoints.ChangeValue(NoU, NoV);

      aParam.SetSqrDistance(thePoint.SquareDistance(aParam.Value()));
    }
  }

  // For search of minimum compute distances to mesh.
  if (myFlag == Extrema_ExtFlag_MIN || myFlag == Extrema_ExtFlag_MINMAX)
  {
    // This is the tolerance of difference of squared values.
    // No need to set it too small.
    const double aDiffTol = mytolu + mytolv;

    // Step 2. Compute distances to edges.
    // Assume UEdge(i, j) = { Point(i, j); Point(i + 1, j    ) }
    // Assume VEdge(i, j) = { Point(i, j); Point(i,     j + 1) }
    for (int NoU = 1; NoU <= myusample; NoU++)
    {
      for (int NoV = 1; NoV <= myvsample; NoV++)
      {
        const Extrema_POnSurfParams& aParam0 = myPoints.Value(NoU, NoV);

        if (NoU < myusample)
        {
          // Compute parameters to UEdge.
          const Extrema_POnSurfParams& aParam1 = myPoints.Value(NoU + 1, NoV);
          const Extrema_POnSurfParams& anEdgeParam =
            ComputeEdgeParameters(true, aParam0, aParam1, thePoint, aDiffTol);

          myUEdgePntParams.SetValue(NoU, NoV, anEdgeParam);
        }

        if (NoV < myvsample)
        {
          // Compute parameters to VEdge.
          const Extrema_POnSurfParams& aParam1 = myPoints.Value(NoU, NoV + 1);
          const Extrema_POnSurfParams& anEdgeParam =
            ComputeEdgeParameters(false, aParam0, aParam1, thePoint, aDiffTol);

          myVEdgePntParams.SetValue(NoU, NoV, anEdgeParam);
        }
      }
    }

    // Step 3. Compute distances to faces.
    // Assume myFacePntParams(i, j) =
    // { Point(i, j); Point(i + 1, j); Point(i + 1, j + 1); Point(i, j + 1) }
    //   Or
    // { UEdge(i, j); VEdge(i + 1, j); UEdge(i, j + 1); VEdge(i, j) }
    double aSqrDist01;
    double aDiffDist;
    bool   isOut;

    for (int NoU = 1; NoU < myusample; NoU++)
    {
      for (int NoV = 1; NoV < myvsample; NoV++)
      {
        const Extrema_POnSurfParams& aUE0 = myUEdgePntParams.Value(NoU, NoV);
        const Extrema_POnSurfParams& aUE1 = myUEdgePntParams.Value(NoU, NoV + 1);
        const Extrema_POnSurfParams& aVE0 = myVEdgePntParams.Value(NoU, NoV);
        const Extrema_POnSurfParams& aVE1 = myVEdgePntParams.Value(NoU + 1, NoV);

        aSqrDist01 = aUE0.Value().SquareDistance(aUE1.Value());
        aDiffDist  = std::abs(aUE0.GetSqrDistance() - aUE1.GetSqrDistance());
        isOut      = false;

        if (aDiffDist >= aSqrDist01 - aDiffTol)
        {
          // The projection is outside the face.
          isOut = true;
        }
        else
        {
          aSqrDist01 = aVE0.Value().SquareDistance(aVE1.Value());
          aDiffDist  = std::abs(aVE0.GetSqrDistance() - aVE1.GetSqrDistance());

          if (aDiffDist >= aSqrDist01 - aDiffTol)
          {
            // The projection is outside the face.
            isOut = true;
          }
        }

        if (isOut)
        {
          // Get the closest point on an edge.
          const Extrema_POnSurfParams& aUEMin =
            aUE0.GetSqrDistance() < aUE1.GetSqrDistance() ? aUE0 : aUE1;
          const Extrema_POnSurfParams& aVEMin =
            aVE0.GetSqrDistance() < aVE1.GetSqrDistance() ? aVE0 : aVE1;
          const Extrema_POnSurfParams& aEMin =
            aUEMin.GetSqrDistance() < aVEMin.GetSqrDistance() ? aUEMin : aVEMin;

          myFacePntParams.SetValue(NoU, NoV, aEMin);
        }
        else
        {
          // Find closest point inside the face.
          double aU[2];
          double aV[2];
          double aUPar;
          double aVPar;

          // Compute U parameter.
          aUE0.Parameter(aU[0], aV[0]);
          aUE1.Parameter(aU[1], aV[1]);
          aUPar = 0.5 * (aU[0] + aU[1]);
          // Compute V parameter.
          aVE0.Parameter(aU[0], aV[0]);
          aVE1.Parameter(aU[1], aV[1]);
          aVPar = 0.5 * (aV[0] + aV[1]);

          Extrema_POnSurfParams aParam(aUPar, aVPar, myS->Value(aUPar, aVPar));

          aParam.SetElementType(Extrema_Face);
          aParam.SetSqrDistance(thePoint.SquareDistance(aParam.Value()));
          aParam.SetIndices(NoU, NoV);
          myFacePntParams.SetValue(NoU, NoV, aParam);
        }
      }
    }

    // Fill boundary with RealLast square distance.
    for (int NoV = 0; NoV <= myvsample; NoV++)
    {
      myFacePntParams.ChangeValue(0, NoV).SetSqrDistance(RealLast());
      myFacePntParams.ChangeValue(myusample, NoV).SetSqrDistance(RealLast());
    }

    for (int NoU = 1; NoU < myusample; NoU++)
    {
      myFacePntParams.ChangeValue(NoU, 0).SetSqrDistance(RealLast());
      myFacePntParams.ChangeValue(NoU, myvsample).SetSqrDistance(RealLast());
    }
  }
}

static double LengthOfIso(const Adaptor3d_Surface& theS,
                          const GeomAbs_IsoType    theIso,
                          const double             thePar1,
                          const double             thePar2,
                          const int                theNbPnts,
                          const double             thePar)
{
  double aLen = 0.;
  int    i;
  double dPar = (thePar2 - thePar1) / (theNbPnts - 1);
  gp_Pnt aP1, aP2;
  double aPar = thePar1 + dPar;
  if (theIso == GeomAbs_IsoU)
  {
    aP1 = theS.Value(thePar, thePar1);
  }
  else
  {
    aP1 = theS.Value(thePar1, thePar);
  }

  for (i = 2; i <= theNbPnts; ++i)
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
    aPar += dPar;
  }
  return aLen;
}

static void CorrectNbSamples(const Adaptor3d_Surface& theS,
                             const double             theU1,
                             const double             theU2,
                             int&                     theNbU,
                             const double             theV1,
                             const double             theV2,
                             int&                     theNbV)
{
  double aMinLen = 1.e-3;
  int    nbp     = std::min(23, theNbV);
  double aLenU1  = LengthOfIso(theS, GeomAbs_IsoU, theV1, theV2, nbp, theU1);
  if (aLenU1 <= aMinLen)
  {
    double aL = LengthOfIso(theS, GeomAbs_IsoU, theV1, theV2, nbp, .7 * theU1 + 0.3 * theU2);
    aLenU1    = std::max(aL, aLenU1);
  }
  double aLenU2 = LengthOfIso(theS, GeomAbs_IsoU, theV1, theV2, nbp, theU2);
  if (aLenU2 <= aMinLen)
  {
    double aL = LengthOfIso(theS, GeomAbs_IsoU, theV1, theV2, nbp, .3 * theU1 + 0.7 * theU2);
    aLenU2    = std::max(aL, aLenU2);
  }
  nbp           = std::min(23, theNbV);
  double aLenV1 = LengthOfIso(theS, GeomAbs_IsoV, theU1, theU2, nbp, theV1);
  if (aLenV1 <= aMinLen)
  {
    double aL = LengthOfIso(theS, GeomAbs_IsoV, theU1, theU2, nbp, .7 * theV1 + 0.3 * theV2);
    aLenV1    = std::max(aL, aLenV1);
  }
  double aLenV2 = LengthOfIso(theS, GeomAbs_IsoV, theU1, theU2, nbp, theV2);
  if (aLenV2 <= aMinLen)
  {
    double aL = LengthOfIso(theS, GeomAbs_IsoV, theU1, theU2, nbp, .3 * theV1 + 0.7 * theV2);
    aLenV2    = std::max(aL, aLenV2);
  }
  //
  double aStepV1 = aLenU1 / theNbV;
  double aStepV2 = aLenU2 / theNbV;
  double aStepU1 = aLenV1 / theNbU;
  double aStepU2 = aLenV2 / theNbU;

  double aMaxStepV = std::max(aStepV1, aStepV2);
  double aMaxStepU = std::max(aStepU1, aStepU2);
  //
  double aRatio = aMaxStepV / aMaxStepU;
  if (aRatio > 10.)
  {
    int aMult = RealToInt(std::log(aRatio));
    if (aMult > 1)
      theNbV *= aMult;
  }
  else if (aRatio < 0.1)
  {
    int aMult = RealToInt(-std::log(aRatio));
    if (aMult > 1)
      theNbV *= aMult;
  }
}

void Extrema_GenExtPS::BuildTree()
{
  // if tree already exists, assume it is already correctly filled
  if (!mySphereUBTree.IsNull())
    return;

  if (myS->GetType() == GeomAbs_BSplineSurface)
  {
    occ::handle<Geom_BSplineSurface> aBspl   = myS->BSpline();
    int                              aUValue = aBspl->UDegree() * aBspl->NbUKnots();
    int                              aVValue = aBspl->VDegree() * aBspl->NbVKnots();
    // 300 is value, which is used for singular points (see Extrema_ExtPS.cxx::Initialize(...))
    if (aUValue > myusample)
      myusample = std::min(aUValue, 300);
    if (aVValue > myvsample)
      myvsample = std::min(aVValue, 300);
  }
  //
  CorrectNbSamples(*myS, myumin, myusup, myusample, myvmin, myvsup, myvsample);
  //
  double PasU = myusup - myumin;
  double PasV = myvsup - myvmin;
  double U0   = PasU / myusample / 100.;
  double V0   = PasV / myvsample / 100.;
  PasU        = (PasU - U0) / (myusample - 1);
  PasV        = (PasV - V0) / (myvsample - 1);
  U0          = U0 / 2. + myumin;
  V0          = V0 / 2. + myvmin;

  // build grid of parametric points
  myUParams = new NCollection_HArray1<double>(1, myusample);
  myVParams = new NCollection_HArray1<double>(1, myvsample);
  int    NoU, NoV;
  double U = U0, V = V0;
  for (NoU = 1; NoU <= myusample; NoU++, U += PasU)
    myUParams->SetValue(NoU, U);
  for (NoV = 1; NoV <= myvsample; NoV++, V += PasV)
    myVParams->SetValue(NoV, V);

  // Build UB-tree with surface points for fast proximity search.
  // Use optimized grid evaluator with span-based caching for B-spline surfaces.
  mySphereUBTree = new NCollection_UBTree<int, Bnd_Sphere>;
  NCollection_UBTreeFiller<int, Bnd_Sphere> aFiller(*mySphereUBTree);
  int                                       i = 0;

  mySphereArray = new NCollection_HArray1<Bnd_Sphere>(0, myusample * myvsample);

  // Use unified grid evaluator for all surface types (optimized for BSpline, Bezier, etc.)
  GeomGridEval_Surface aGridEval;
  aGridEval.Initialize(*myS);

  const NCollection_Array2<gp_Pnt> aGridPoints =
    aGridEval.EvaluateGrid(myUParams->Array1(), myVParams->Array1());

  for (NoU = 1; NoU <= myusample; NoU++)
  {
    for (NoV = 1; NoV <= myvsample; NoV++)
    {
      const gp_Pnt& aP1 = aGridPoints.Value(NoU, NoV);
      Bnd_Sphere    aSph(aP1.XYZ(), 0 /*mytolu < mytolv ? mytolu : mytolv*/, NoU, NoV);
      aFiller.Add(i, aSph);
      mySphereArray->SetValue(i, aSph);
      i++;
    }
  }
  aFiller.Fill();
}

void Extrema_GenExtPS::FindSolution(const gp_Pnt& /*P*/, const Extrema_POnSurfParams& theParams)
{
  math_Vector Tol(1, 2);
  Tol(1) = mytolu;
  Tol(2) = mytolv;

  math_Vector UV(1, 2);
  theParams.Parameter(UV(1), UV(2));

  math_Vector UVinf(1, 2), UVsup(1, 2);
  UVinf(1) = myumin;
  UVinf(2) = myvmin;
  UVsup(1) = myusup;
  UVsup(2) = myvsup;

  math_FunctionSetRoot S(myF, Tol);
  S.Perform(myF, UV, UVinf, UVsup);

  myDone = true;
}

void Extrema_GenExtPS::SetFlag(const Extrema_ExtFlag F)
{
  myFlag = F;
}

void Extrema_GenExtPS::SetAlgo(const Extrema_ExtAlgo A)
{
  if (myAlgo != A)
    myInit = false;
  myAlgo = A;
}

void Extrema_GenExtPS::Perform(const gp_Pnt& P)
{
  myDone = false;
  myF.SetPoint(P);

  if (myAlgo == Extrema_ExtAlgo_Grad)
  {
    BuildGrid(P);
    int NoU, NoV;

    if (myFlag == Extrema_ExtFlag_MIN || myFlag == Extrema_ExtFlag_MINMAX)
    {
      Extrema_ElementType anElemType;
      int                 iU;
      int                 iV;
      int                 iU2;
      int                 iV2;
      bool                isMin;
      int                 i;

      for (NoU = 1; NoU < myusample; NoU++)
      {
        for (NoV = 1; NoV < myvsample; NoV++)
        {
          const Extrema_POnSurfParams& aParam = myFacePntParams.Value(NoU, NoV);

          isMin      = false;
          anElemType = aParam.GetElementType();

          if (anElemType == Extrema_Face)
          {
            isMin = true;
          }
          else
          {
            // Check if it is a boundary edge or corner vertex.
            aParam.GetIndices(iU, iV);

            if (anElemType == Extrema_UIsoEdge)
            {
              isMin = (iV == 1 || iV == myvsample);
            }
            else if (anElemType == Extrema_VIsoEdge)
            {
              isMin = (iU == 1 || iU == myusample);
            }
            else if (anElemType == Extrema_Node)
            {
              isMin = (iU == 1 || iU == myusample) && (iV == 1 || iV == myvsample);
            }

            if (!isMin)
            {
              // This is a middle element.
              if (anElemType == Extrema_UIsoEdge
                  || (anElemType == Extrema_Node && (iU == 1 || iU == myusample)))
              {
                // Check the down face.
                const Extrema_POnSurfParams& aDownParam = myFacePntParams.Value(NoU, NoV - 1);

                if (aDownParam.GetElementType() == anElemType)
                {
                  aDownParam.GetIndices(iU2, iV2);
                  isMin = (iU == iU2 && iV == iV2);
                }
              }
              else if (anElemType == Extrema_VIsoEdge
                       || (anElemType == Extrema_Node && (iV == 1 || iV == myvsample)))
              {
                // Check the right face.
                const Extrema_POnSurfParams& aRightParam = myFacePntParams.Value(NoU - 1, NoV);

                if (aRightParam.GetElementType() == anElemType)
                {
                  aRightParam.GetIndices(iU2, iV2);
                  isMin = (iU == iU2 && iV == iV2);
                }
              }
              else if (iU == NoU && iV == NoV)
              {
                // Check the lower-left node. For this purpose it is necessary
                // to check lower-left, lower and left faces.
                isMin = true;

                const Extrema_POnSurfParams* anOtherParam[3] = {
                  &myFacePntParams.Value(NoU, NoV - 1),     // Down
                  &myFacePntParams.Value(NoU - 1, NoV - 1), // Lower-left
                  &myFacePntParams.Value(NoU - 1, NoV)};    // Left

                for (i = 0; i < 3 && isMin; i++)
                {
                  if (anOtherParam[i]->GetElementType() == Extrema_Node)
                  {
                    anOtherParam[i]->GetIndices(iU2, iV2);
                    isMin = (iU == iU2 && iV == iV2);
                  }
                  else
                  {
                    isMin = false;
                  }
                }
              }
            }
          }

          if (isMin)
          {
            FindSolution(P, aParam);
          }
        }
      }
    }

    if (myFlag == Extrema_ExtFlag_MAX || myFlag == Extrema_ExtFlag_MINMAX)
    {
      double Dist;

      for (NoU = 1; NoU <= myusample; NoU++)
      {
        for (NoV = 1; NoV <= myvsample; NoV++)
        {
          const Extrema_POnSurfParams& aParamMain = myPoints.Value(NoU, NoV);
          const Extrema_POnSurfParams& aParam1    = myPoints.Value(NoU - 1, NoV - 1);
          const Extrema_POnSurfParams& aParam2    = myPoints.Value(NoU - 1, NoV);
          const Extrema_POnSurfParams& aParam3    = myPoints.Value(NoU - 1, NoV + 1);
          const Extrema_POnSurfParams& aParam4    = myPoints.Value(NoU, NoV - 1);
          const Extrema_POnSurfParams& aParam5    = myPoints.Value(NoU, NoV + 1);
          const Extrema_POnSurfParams& aParam6    = myPoints.Value(NoU + 1, NoV - 1);
          const Extrema_POnSurfParams& aParam7    = myPoints.Value(NoU + 1, NoV);
          const Extrema_POnSurfParams& aParam8    = myPoints.Value(NoU + 1, NoV + 1);

          Dist = aParamMain.GetSqrDistance();

          if ((aParam1.GetSqrDistance() <= Dist) && (aParam2.GetSqrDistance() <= Dist)
              && (aParam3.GetSqrDistance() <= Dist) && (aParam4.GetSqrDistance() <= Dist)
              && (aParam5.GetSqrDistance() <= Dist) && (aParam6.GetSqrDistance() <= Dist)
              && (aParam7.GetSqrDistance() <= Dist) && (aParam8.GetSqrDistance() <= Dist))
          {
            // Find maximum.
            FindSolution(P, myPoints.Value(NoU, NoV));
          }
        }
      }
    }
  }
  else
  {
    BuildTree();
    if (myFlag == Extrema_ExtFlag_MIN || myFlag == Extrema_ExtFlag_MINMAX)
    {
      Bnd_Sphere                  aSol = mySphereArray->Value(0);
      Bnd_SphereUBTreeSelectorMin aSelector(mySphereArray, aSol);
      // aSelector.SetMaxDist( RealLast() );
      aSelector.DefineCheckPoint(P);
      mySphereUBTree->Select(aSelector);
      // TODO: check if no solution in binary tree
      Bnd_Sphere&           aSph = aSelector.Sphere();
      double                aU   = myUParams->Value(aSph.U());
      double                aV   = myVParams->Value(aSph.V());
      Extrema_POnSurfParams aParams(aU, aV, myS->Value(aU, aV));

      aParams.SetSqrDistance(P.SquareDistance(aParams.Value()));
      aParams.SetIndices(aSph.U(), aSph.V());
      FindSolution(P, aParams);
    }
    if (myFlag == Extrema_ExtFlag_MAX || myFlag == Extrema_ExtFlag_MINMAX)
    {
      Bnd_Sphere                  aSol = mySphereArray->Value(0);
      Bnd_SphereUBTreeSelectorMax aSelector(mySphereArray, aSol);
      // aSelector.SetMaxDist( RealLast() );
      aSelector.DefineCheckPoint(P);
      mySphereUBTree->Select(aSelector);
      // TODO: check if no solution in binary tree
      Bnd_Sphere&           aSph = aSelector.Sphere();
      double                aU   = myUParams->Value(aSph.U());
      double                aV   = myVParams->Value(aSph.V());
      Extrema_POnSurfParams aParams(aU, aV, myS->Value(aU, aV));
      aParams.SetSqrDistance(P.SquareDistance(aParams.Value()));
      aParams.SetIndices(aSph.U(), aSph.V());

      FindSolution(P, aParams);
    }
  }
}

//=============================================================================

bool Extrema_GenExtPS::IsDone() const
{
  return myDone;
}

//=============================================================================

int Extrema_GenExtPS::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myF.NbExt();
}

//=============================================================================

double Extrema_GenExtPS::SquareDistance(const int N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }

  return myF.SquareDistance(N);
}

//=============================================================================

const Extrema_POnSurf& Extrema_GenExtPS::Point(const int N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }

  return myF.Point(N);
}

//=============================================================================
