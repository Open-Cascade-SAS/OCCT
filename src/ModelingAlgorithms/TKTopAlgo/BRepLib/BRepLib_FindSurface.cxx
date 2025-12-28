// Created on: 1994-07-22
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <math_Jacobi.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_Vector.hxx>

//=================================================================================================

static double Controle(const NCollection_Sequence<gp_Pnt>& thePoints,
                              const occ::handle<Geom_Plane>&   thePlane)
{
  double    dfMaxDist = 0.;
  double    a, b, c, d, dist;
  int ii;
  thePlane->Coefficients(a, b, c, d);
  for (ii = 1; ii <= thePoints.Length(); ii++)
  {
    const gp_XYZ& xyz = thePoints(ii).XYZ();
    dist              = std::abs(a * xyz.X() + b * xyz.Y() + c * xyz.Z() + d);
    if (dist > dfMaxDist)
      dfMaxDist = dist;
  }

  return dfMaxDist;
}

//=======================================================================
// function : Is2DConnected
// purpose  : Return true if the last vertex of theEdge1 coincides with
//           the first vertex of theEdge2 in parametric space of theFace
//=======================================================================
inline static bool Is2DConnected(const TopoDS_Edge&          theEdge1,
                                             const TopoDS_Edge&          theEdge2,
                                             const occ::handle<Geom_Surface>& theSurface,
                                             const TopLoc_Location&      theLocation)
{
  double f, l;
  // TopLoc_Location aLoc;
  occ::handle<Geom2d_Curve> aCurve;
  gp_Pnt2d             p1, p2;

  // get 2D points
  aCurve = BRep_Tool::CurveOnSurface(theEdge1, theSurface, theLocation, f, l);
  p1     = aCurve->Value(theEdge1.Orientation() == TopAbs_FORWARD ? l : f);
  aCurve = BRep_Tool::CurveOnSurface(theEdge2, theSurface, theLocation, f, l);
  p2     = aCurve->Value(theEdge2.Orientation() == TopAbs_FORWARD ? f : l);

  // compare 2D points
  GeomAdaptor_Surface aSurface(theSurface);
  TopoDS_Vertex       aV    = TopExp::FirstVertex(theEdge2, true);
  double       tol3D = BRep_Tool::Tolerance(aV);
  double       tol2D = aSurface.UResolution(tol3D) + aSurface.VResolution(tol3D);
  double       dist2 = p1.SquareDistance(p2);
  return dist2 < tol2D * tol2D;
}

//=======================================================================
// function : Is2DClosed
// purpose  : Return true if edges of theShape form a closed wire in
//           parametric space of theSurface
//=======================================================================

static bool Is2DClosed(const TopoDS_Shape&         theShape,
                                   const occ::handle<Geom_Surface>& theSurface,
                                   const TopLoc_Location&      theLocation)
{
  try
  {
    OCC_CATCH_SIGNALS
    // get a wire theShape
    TopExp_Explorer aWireExp(theShape, TopAbs_WIRE);
    if (!aWireExp.More())
    {
      return false;
    }
    TopoDS_Wire aWire = TopoDS::Wire(aWireExp.Current());
    // a tmp face
    TopoDS_Face aTmpFace = BRepLib_MakeFace(theSurface, Precision::PConfusion());

    // check topological closeness using wire explorer, if the wire is not closed
    // the 1st and the last vertices of wire are different
    BRepTools_WireExplorer aWireExplorer(aWire, aTmpFace);
    if (!aWireExplorer.More())
    {
      return false;
    }
    // remember the 1st and the last edges of aWire
    TopoDS_Edge aFisrtEdge = aWireExplorer.Current(), aLastEdge = aFisrtEdge;
    // check if edges connected topologically (that is assured by BRepTools_WireExplorer)
    // are connected in 2D
    TopoDS_Edge aPrevEdge = aFisrtEdge;
    for (aWireExplorer.Next(); aWireExplorer.More(); aWireExplorer.Next())
    {
      aLastEdge = aWireExplorer.Current();
      if (!Is2DConnected(aPrevEdge, aLastEdge, theSurface, theLocation))
      {
        return false;
      }
      aPrevEdge = aLastEdge;
    }
    // wire is closed if ( 1st vertex of aFisrtEdge ) ==
    // ( last vertex of aLastEdge ) in 2D
    TopoDS_Vertex aV1 = TopExp::FirstVertex(aFisrtEdge, true);
    TopoDS_Vertex aV2 = TopExp::LastVertex(aLastEdge, true);
    return (aV1.IsSame(aV2) && Is2DConnected(aLastEdge, aFisrtEdge, theSurface, theLocation));
  }
  catch (Standard_Failure const&)
  {
    return false;
  }
}

//=================================================================================================

BRepLib_FindSurface::BRepLib_FindSurface()
    : myTolerance(0.0),
      myTolReached(0.0),
      isExisted(false)
{
}

//=================================================================================================

BRepLib_FindSurface::BRepLib_FindSurface(const TopoDS_Shape&    S,
                                         const double    Tol,
                                         const bool OnlyPlane,
                                         const bool OnlyClosed)
{
  Init(S, Tol, OnlyPlane, OnlyClosed);
}

namespace
{
static void fillParams(const NCollection_Array1<double>&        theKnots,
                       int                   theDegree,
                       double                      theParMin,
                       double                      theParMax,
                       NCollection_Vector<double>& theParams)
{
  double aPrevPar = theParMin;
  theParams.Append(aPrevPar);

  int aNbP = std::max(theDegree, 1);

  for (int i = 1;
       (i < theKnots.Length()) && (theKnots(i) < (theParMax - Precision::PConfusion()));
       ++i)
  {
    if (theKnots(i + 1) < theParMin + Precision::PConfusion())
      continue;

    double aStep = (theKnots(i + 1) - theKnots(i)) / aNbP;
    for (int k = 1; k <= aNbP; ++k)
    {
      double aPar = theKnots(i) + k * aStep;
      if (aPar > theParMax - Precision::PConfusion())
        break;

      if (aPar > aPrevPar + Precision::PConfusion())
      {
        theParams.Append(aPar);
        aPrevPar = aPar;
      }
    }
  }
  theParams.Append(theParMax);
}

static void fillPoints(const BRepAdaptor_Curve&                 theCurve,
                       const NCollection_Vector<double>& theParams,
                       NCollection_Sequence<gp_Pnt>&                    thePoints,
                       NCollection_Sequence<double>&                  theWeights)
{
  double aDistPrev = 0., aDistNext;
  gp_Pnt        aPPrev(theCurve.Value(theParams(0))), aPNext;

  for (int iP = 1; iP <= theParams.Length(); ++iP)
  {
    if (iP < theParams.Length())
    {
      double aParam = theParams(iP);
      aPNext               = theCurve.Value(aParam);
      aDistNext            = aPPrev.Distance(aPNext);
    }
    else
      aDistNext = 0.0;

    thePoints.Append(aPPrev);
    theWeights.Append(aDistPrev + aDistNext);
    aDistPrev = aDistNext;
    aPPrev    = aPNext;
  }
}

} // namespace

//=================================================================================================

void BRepLib_FindSurface::Init(const TopoDS_Shape&    S,
                               const double    Tol,
                               const bool OnlyPlane,
                               const bool OnlyClosed)
{
  myTolerance  = Tol;
  myTolReached = 0.;
  isExisted    = false;
  myLocation.Identity();
  mySurface.Nullify();

  // compute the tolerance
  TopExp_Explorer ex;

  for (ex.Init(S, TopAbs_EDGE); ex.More(); ex.Next())
  {
    double t = BRep_Tool::Tolerance(TopoDS::Edge(ex.Current()));
    if (t > myTolerance)
      myTolerance = t;
  }

  // search an existing surface
  ex.Init(S, TopAbs_EDGE);
  if (!ex.More())
    return; // no edges ....

  TopoDS_Edge          E = TopoDS::Edge(ex.Current());
  double        f, l, ff, ll;
  occ::handle<Geom2d_Curve> PC, aPPC;
  occ::handle<Geom_Surface> SS;
  TopLoc_Location      L;
  int     i = 0, j;

  // iterate on the surfaces of the first edge
  for (;;)
  {
    i++;
    BRep_Tool::CurveOnSurface(E, PC, mySurface, myLocation, f, l, i);
    if (mySurface.IsNull())
    {
      break;
    }
    // check the other edges
    for (ex.Init(S, TopAbs_EDGE); ex.More(); ex.Next())
    {
      if (!E.IsSame(ex.Current()))
      {
        j = 0;
        for (;;)
        {
          j++;
          BRep_Tool::CurveOnSurface(TopoDS::Edge(ex.Current()), aPPC, SS, L, ff, ll, j);
          if (SS.IsNull())
          {
            break;
          }
          if ((SS == mySurface) && (L.IsEqual(myLocation)))
          {
            break;
          }
          SS.Nullify();
        }

        if (SS.IsNull())
        {
          mySurface.Nullify();
          break;
        }
      }
    }

    // if OnlyPlane, eval if mySurface is a plane.
    if (OnlyPlane && !mySurface.IsNull())
    {
      if (mySurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
        mySurface = occ::down_cast<Geom_RectangularTrimmedSurface>(mySurface)->BasisSurface();
      mySurface = occ::down_cast<Geom_Plane>(mySurface);
    }

    if (!mySurface.IsNull())
      // if S is e.g. the bottom face of a cylinder, mySurface can be the
      // lateral (cylindrical) face of the cylinder; reject an improper mySurface
      if (!OnlyClosed || Is2DClosed(S, mySurface, myLocation))
        break;
  }

  if (!mySurface.IsNull())
  {
    isExisted = true;
    return;
  }
  //
  // no existing surface, search a plane
  // 07/02/02 akm vvv : (OCC157) changed algorithm
  //                    1. Collect the points along all edges of the shape
  //                       For each point calculate the WEIGHT = sum of
  //                       distances from neighboring points (_only_ same edge)
  //                    2. Minimizing the weighed sum of squared deviations
  //                       compute coefficients of the sought plane.

  NCollection_Sequence<gp_Pnt>   aPoints;
  NCollection_Sequence<double> aWeight;

  // ======================= Step #1
  for (ex.Init(S, TopAbs_EDGE); ex.More(); ex.Next())
  {
    BRepAdaptor_Curve c(TopoDS::Edge(ex.Current()));

    double dfUf = c.FirstParameter();
    double dfUl = c.LastParameter();
    if (IsEqual(dfUf, dfUl))
    {
      // Degenerate
      continue;
    }
    int iNbPoints = 0;

    // Fill the parameters of the sampling points
    NCollection_Vector<double> aParams;
    switch (c.GetType())
    {
      case GeomAbs_BezierCurve: {
        occ::handle<Geom_BezierCurve> GC = c.Bezier();
        NCollection_Array1<double>     aKnots(1, 2);
        aKnots.SetValue(1, GC->FirstParameter());
        aKnots.SetValue(2, GC->LastParameter());

        fillParams(aKnots, GC->Degree(), dfUf, dfUl, aParams);
        break;
      }
      case GeomAbs_BSplineCurve: {
        occ::handle<Geom_BSplineCurve> GC = c.BSpline();
        fillParams(GC->Knots(), GC->Degree(), dfUf, dfUl, aParams);
        break;
      }
      case GeomAbs_Line: {
        // Two points on a straight segment
        aParams.Append(dfUf);
        aParams.Append(dfUl);
        break;
      }
      case GeomAbs_Circle:
      case GeomAbs_Ellipse:
      case GeomAbs_Hyperbola:
      case GeomAbs_Parabola:
        // Four points on other analytical curves
        iNbPoints = 4;
        [[fallthrough]];
      default: {
        // Put some points on other curves
        if (iNbPoints == 0)
          iNbPoints = 15 + c.NbIntervals(GeomAbs_C3);

        NCollection_Array1<double> aBounds(1, 2);
        aBounds.SetValue(1, dfUf);
        aBounds.SetValue(2, dfUl);

        fillParams(aBounds, iNbPoints - 1, dfUf, dfUl, aParams);
      }
    }

    // Add the points with weights to the sequences
    fillPoints(c, aParams, aPoints, aWeight);
  }

  if (aPoints.Length() < 3)
  {
    return;
  }

  // ======================= Step #2
  myLocation.Identity();
  int iPoint;
  math_Matrix      aMat(1, 3, 1, 3, 0.);
  math_Vector      aVec(1, 3, 0.);
  // Find the barycenter and normalize weights
  double dfMaxWeight = 0.;
  gp_XYZ        aBaryCenter(0., 0., 0.);
  double dfSumWeight = 0.;
  for (iPoint = 1; iPoint <= aPoints.Length(); iPoint++)
  {
    double dfW = aWeight(iPoint);
    aBaryCenter += dfW * aPoints(iPoint).XYZ();
    dfSumWeight += dfW;
    if (dfW > dfMaxWeight)
    {
      dfMaxWeight = dfW;
    }
  }
  aBaryCenter /= dfSumWeight;

  // Fill the matrix and the right vector
  for (iPoint = 1; iPoint <= aPoints.Length(); iPoint++)
  {
    gp_XYZ        p = aPoints(iPoint).XYZ() - aBaryCenter;
    double w = aWeight(iPoint) / dfMaxWeight;
    aMat(1, 1) += w * p.X() * p.X();
    aMat(1, 2) += w * p.X() * p.Y();
    aMat(1, 3) += w * p.X() * p.Z();
    //
    aMat(2, 2) += w * p.Y() * p.Y();
    aMat(2, 3) += w * p.Y() * p.Z();
    //
    aMat(3, 3) += w * p.Z() * p.Z();
  }
  aMat(2, 1) = aMat(1, 2);
  aMat(3, 1) = aMat(1, 3);
  aMat(3, 2) = aMat(2, 3);
  //
  math_Jacobi      anEignval(aMat);
  math_Vector      anEVals(1, 3);
  bool isSolved = anEignval.IsDone();
  int isol     = 0;
  if (isSolved)
  {
    anEVals = anEignval.Values();
    // We need vector with eigenvalue ~ 0.
    double anEMin = RealLast();
    double anEMax = -anEMin;
    for (i = 1; i <= 3; ++i)
    {
      double anE = std::abs(anEVals(i));
      if (anEMin > anE)
      {
        anEMin = anE;
        isol   = i;
      }
      if (anEMax < anE)
      {
        anEMax = anE;
      }
    }

    if (isol == 0)
    {
      isSolved = false;
    }
    else
    {
      double eps = Epsilon(anEMax);
      if (anEMin <= eps)
      {
        anEignval.Vector(isol, aVec);
      }
      else
      {
        // try using vector product of other axes
        int ind[2] = {0, 0};
        for (i = 1; i <= 3; ++i)
        {
          if (i == isol)
          {
            continue;
          }
          if (ind[0] == 0)
          {
            ind[0] = i;
            continue;
          }
          if (ind[1] == 0)
          {
            ind[1] = i;
            continue;
          }
        }
        math_Vector aVec1(1, 3, 0.), aVec2(1, 3, 0.);
        anEignval.Vector(ind[0], aVec1);
        anEignval.Vector(ind[1], aVec2);
        gp_Vec aV1(aVec1(1), aVec1(2), aVec1(3));
        gp_Vec aV2(aVec2(1), aVec2(2), aVec2(3));
        gp_Vec aN = aV1 ^ aV2;
        aVec(1)   = aN.X();
        aVec(2)   = aN.Y();
        aVec(3)   = aN.Z();
      }
      if (aVec.Norm2() < gp::Resolution())
      {
        isSolved = false;
      }
    }
  }

  if (!isSolved)
    return;
  // Removing very small values
  double aMaxV = std::max(std::abs(aVec(1)), std::max(std::abs(aVec(2)), std::abs(aVec(3))));
  double eps   = Epsilon(aMaxV);
  for (i = 1; i <= 3; ++i)
  {
    if (std::abs(aVec(i)) <= eps)
      aVec(i) = 0.;
  }
  gp_Vec             aN(aVec(1), aVec(2), aVec(3));
  occ::handle<Geom_Plane> aPlane     = new Geom_Plane(aBaryCenter, aN);
  myTolReached                  = Controle(aPoints, aPlane);
  const double aWeakness = 5.0;
  if (myTolReached <= myTolerance || (Tol < 0 && myTolReached < myTolerance * aWeakness))
  {
    mySurface = aPlane;
    // If S is wire, try to orient surface according to orientation of wire.
    if (S.ShapeType() == TopAbs_WIRE && S.Closed())
    {
      TopoDS_Wire  aW       = TopoDS::Wire(S);
      TopoDS_Face  aTmpFace = BRepLib_MakeFace(mySurface, Precision::Confusion());
      BRep_Builder BB;
      BB.Add(aTmpFace, aW);
      BRepTopAdaptor_FClass2d FClass(aTmpFace, 0.);
      if (FClass.PerformInfinitePoint() == TopAbs_IN)
      {
        gp_Dir aNorm = aPlane->Position().Direction();
        aNorm.Reverse();
        mySurface = new Geom_Plane(aPlane->Position().Location(), aNorm);
      }
    }
  }
}

//=================================================================================================

bool BRepLib_FindSurface::Found() const
{
  return !mySurface.IsNull();
}

//=================================================================================================

occ::handle<Geom_Surface> BRepLib_FindSurface::Surface() const
{
  return mySurface;
}

//=================================================================================================

double BRepLib_FindSurface::Tolerance() const
{
  return myTolerance;
}

//=================================================================================================

double BRepLib_FindSurface::ToleranceReached() const
{
  return myTolReached;
}

//=================================================================================================

bool BRepLib_FindSurface::Existed() const
{
  return isExisted;
}

//=================================================================================================

TopLoc_Location BRepLib_FindSurface::Location() const
{
  return myLocation;
}
