// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

//  Modified by skv - Thu Jul 13 17:42:58 2006 OCC12627
//  Total rewriting of the method Segment; add the method OtherSegment.

#include <BRep_Tool.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <NCollection_DataMap.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>

static const double Probing_Start = 0.123;
static const double Probing_End   = 0.7;
static const double Probing_Step  = 0.2111;

namespace
{

constexpr size_t THE_MIN_EDGES_FOR_BOUNDING_BOX = 10;

void cacheGeometry(BRepClass_Edge& theEdge)
{
  double                           aFirst = 0.0;
  double                           aLast  = 0.0;
  const occ::handle<Geom2d_Curve>& aCurve =
    BRep_Tool::CurveOnSurface(theEdge.Edge(), theEdge.Face(), aFirst, aLast);
  if (aCurve.IsNull())
  {
    return;
  }

  theEdge.SetCurve(aCurve, aFirst, aLast, Bnd_Box2d());
}

} // namespace

//=================================================================================================

BRepClass_FaceExplorer::BRepClass_FaceExplorer(const TopoDS_Face& F)
    : myFace(F),
      myCurrentWire(0),
      myCurrentEdge(0),
      myCurrentEdgeEnd(0),
      myCurEdgeInd(0),
      myCurEdgePar(Probing_Start),
      myMaxTolerance(0.1),
      myUseBndBox(false),
      myUMin(Precision::Infinite()),
      myUMax(-Precision::Infinite()),
      myVMin(Precision::Infinite()),
      myVMax(-Precision::Infinite())

{
  myFace.Orientation(TopAbs_FORWARD);

  using EdgeIndices = NCollection_LinearVector<uint32_t>;
  NCollection_DataMap<TopoDS_Shape, EdgeIndices, TopTools_ShapeMapHasher> anEdgeIndices;

  for (TopExp_Explorer aWireExp(myFace, TopAbs_WIRE); aWireExp.More(); aWireExp.Next())
  {
    WireData aWire;
    aWire.FirstEdge = static_cast<uint32_t>(myEdges.Size());

    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      aMapVE;
    TopExp::MapShapesAndAncestors(aWireExp.Current(), TopAbs_VERTEX, TopAbs_EDGE, aMapVE);
    for (TopExp_Explorer anEdgeExp(aWireExp.Current(), TopAbs_EDGE); anEdgeExp.More();
         anEdgeExp.Next())
    {
      BRepClass_Edge anEdge(TopoDS::Edge(anEdgeExp.Current()), myFace);
      anEdge.SetNextEdge(aMapVE);
      cacheGeometry(anEdge);
      myEdges.Append(std::move(anEdge));
      const uint32_t anEdgeIndex = static_cast<uint32_t>(myEdges.Size() - 1);
      EdgeIndices*   anIndices   = anEdgeIndices.ChangeSeek(myEdges[anEdgeIndex].Edge());
      if (anIndices == nullptr)
      {
        EdgeIndices aNewIndices;
        aNewIndices.Append(anEdgeIndex);
        anEdgeIndices.Bind(myEdges[anEdgeIndex].Edge(), std::move(aNewIndices));
      }
      else
      {
        anIndices->Append(anEdgeIndex);
      }
      ++aWire.NbEdges;
    }
    myWires.Append(aWire);
  }

  for (TopExp_Explorer anEdgeExp(myFace, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    const TopoDS_Edge& anEdge      = TopoDS::Edge(anEdgeExp.Current());
    const EdgeIndices* anIndices   = anEdgeIndices.Seek(anEdge);
    uint32_t           anEdgeIndex = 0;
    bool               isFound     = false;
    if (anIndices != nullptr)
    {
      for (const uint32_t aCandidateIndex : *anIndices)
      {
        if (myEdges[aCandidateIndex].Edge().IsEqual(anEdge))
        {
          anEdgeIndex = aCandidateIndex;
          isFound     = true;
          break;
        }
      }
    }
    if (!isFound)
    {
      BRepClass_Edge anEdgeData(anEdge, myFace);
      cacheGeometry(anEdgeData);
      myEdges.Append(std::move(anEdgeData));
      anEdgeIndex = static_cast<uint32_t>(myEdges.Size() - 1);
    }
    myProbeEdges.Append(anEdgeIndex);
  }
}

//=================================================================================================

void BRepClass_FaceExplorer::ComputeFaceBounds()
{
  TopLoc_Location                  aLocation;
  const occ::handle<Geom_Surface>& aSurface = BRep_Tool::Surface(myFace, aLocation);
  aSurface->Bounds(myUMin, myUMax, myVMin, myVMax);
  if (Precision::IsInfinite(myUMin) || Precision::IsInfinite(myUMax)
      || Precision::IsInfinite(myVMin) || Precision::IsInfinite(myVMax))
  {
    BRepTools::UVBounds(myFace, myUMin, myUMax, myVMin, myVMax);
  }
}

//=================================================================================================

bool BRepClass_FaceExplorer::CheckPoint(gp_Pnt2d& thePoint)
{
  if (myUMin > myUMax)
  {
    ComputeFaceBounds();
  }

  if (Precision::IsInfinite(myUMin) || Precision::IsInfinite(myUMax)
      || Precision::IsInfinite(myVMin) || Precision::IsInfinite(myVMax))
  {
    return true;
  }

  gp_Pnt2d aCenterPnt((myUMin + myUMax) / 2, (myVMin + myVMax) / 2);
  double   aDistance = aCenterPnt.Distance(thePoint);
  if (Precision::IsInfinite(aDistance))
  {
    thePoint.SetCoord(myUMin - (myUMax - myUMin), myVMin - (myVMax - myVMin));
    return false;
  }
  else
  {
    double anEpsilon = Epsilon(aDistance);
    if (anEpsilon > std::max(myUMax - myUMin, myVMax - myVMin))
    {
      gp_Vec2d aLinVec(aCenterPnt, thePoint);
      gp_Dir2d aLinDir(aLinVec);
      thePoint = aCenterPnt.XY() + aLinDir.XY() * (2. * anEpsilon);
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool BRepClass_FaceExplorer::Reject(const gp_Pnt2d&) const
{
  return false;
}

//=================================================================================================

bool BRepClass_FaceExplorer::Segment(const gp_Pnt2d& P, gp_Lin2d& L, double& Par)
{
  myCurEdgeInd = 0;
  myCurEdgePar = Probing_Start;

  return OtherSegment(P, L, Par);
}

//=================================================================================================

bool BRepClass_FaceExplorer::OtherSegment(const gp_Pnt2d& P, gp_Lin2d& L, double& Par)
{
  constexpr double aTolParConf2 = Precision::PConfusion() * Precision::PConfusion();
  gp_Pnt2d         aPOnC;
  while (myCurEdgeInd < myProbeEdges.Size())
  {
    const BRepClass_Edge&    anEdgeData    = myEdges[myProbeEdges[myCurEdgeInd]];
    const TopoDS_Edge&       anEdge        = anEdgeData.Edge();
    const TopAbs_Orientation anOrientation = anEdge.Orientation();

    if (anOrientation == TopAbs_FORWARD || anOrientation == TopAbs_REVERSED)
    {
      const occ::handle<Geom2d_Curve>& aC2d  = anEdgeData.Curve();
      double                           aFPar = anEdgeData.FirstParameter();
      double                           aLPar = anEdgeData.LastParameter();

      if (!aC2d.IsNull())
      {
        // Treatment of infinite cases.
        if (Precision::IsNegativeInfinite(aFPar))
        {
          if (Precision::IsPositiveInfinite(aLPar))
          {
            aFPar = -1.;
            aLPar = 1.;
          }
          else
          {
            aFPar = aLPar - 1.;
          }
        }
        else if (Precision::IsPositiveInfinite(aLPar))
        {
          aLPar = aFPar + 1.;
        }

        for (; myCurEdgePar < Probing_End; myCurEdgePar += Probing_Step)
        {
          const double aParamIn = myCurEdgePar * aFPar + (1. - myCurEdgePar) * aLPar;

          gp_Vec2d aTanVec;
          aC2d->D1(aParamIn, aPOnC, aTanVec);
          Par = aPOnC.SquareDistance(P);

          if (Par > aTolParConf2)
          {
            gp_Vec2d aLinVec(P, aPOnC);
            gp_Dir2d aLinDir(aLinVec);

            double aTanMod = aTanVec.SquareMagnitude();
            if (aTanMod < aTolParConf2)
            {
              continue;
            }
            aTanVec /= std::sqrt(aTanMod);
            double       aSinA        = aTanVec.Crossed(aLinDir.XY());
            const double SmallAngle   = 0.001;
            bool         isSmallAngle = false;
            if (std::abs(aSinA) < SmallAngle)
            {
              isSmallAngle = true;
              // The line from the input point P to the current point on edge
              // is tangent to the edge curve. This condition is bad for classification.
              // Therefore try to go to another point in the hope that there will be
              // no tangent. If there tangent is preserved then leave the last point in
              // order to get this edge chance to participate in classification.
              if (myCurEdgePar + Probing_Step < Probing_End)
              {
                continue;
              }
            }

            L = gp_Lin2d(P, aLinDir);

            // Check if ends of a curve lie on a line.
            aC2d->D0(aFPar, aPOnC);
            gp_Pnt2d aFPOnC = aPOnC;
            if (L.SquareDistance(aPOnC) > aTolParConf2)
            {
              aC2d->D0(aLPar, aPOnC);
              if (L.SquareDistance(aPOnC) > aTolParConf2)
              {

                if (isSmallAngle)
                {
                  // Try to find minimal distance between curve and line

                  Geom2dAPI_ProjectPointOnCurve aProj;
                  aProj.Init(P, aC2d, aFPar, aLPar);
                  if (aProj.NbPoints() > 0)
                  {
                    gp_Pnt2d aLPOnC   = aPOnC;
                    double   aFDist   = P.SquareDistance(aFPOnC);
                    double   aLDist   = P.SquareDistance(aLPOnC);
                    double   aMinDist = aProj.LowerDistance();
                    aMinDist *= aMinDist;
                    aPOnC = aProj.NearestPoint();
                    if (aMinDist > aFDist)
                    {
                      aMinDist = aFDist;
                      aPOnC    = aFPOnC;
                    }
                    //
                    if (aMinDist > aLDist)
                    {
                      aMinDist = aLDist;
                      aPOnC    = aLPOnC;
                    }
                    //
                    if (aMinDist < Par)
                    {
                      Par = aMinDist;
                      if (Par < aTolParConf2)
                      {
                        continue;
                      }
                      aLinVec.SetXY((aPOnC.XY() - P.XY()));
                      aLinDir.SetXY(aLinVec.XY());
                      L = gp_Lin2d(P, aLinDir);
                    }
                  }
                }
                myCurEdgePar += Probing_Step;
                if (myCurEdgePar >= Probing_End)
                {
                  ++myCurEdgeInd;
                  myCurEdgePar = Probing_Start;
                }

                Par = std::sqrt(Par);
                return true;
              }
            }
          }
        }
      } // if (!aC2d.IsNull()) {
    } // if (anOrientation == TopAbs_FORWARD ...

    // This curve is not valid for line construction. Go to another edge.
    ++myCurEdgeInd;
    myCurEdgePar = Probing_Start;
  }

  // nothing found, return an horizontal line
  Par = RealLast();
  L   = gp_Lin2d(P, gp_Dir2d(gp_Dir2d::D::X));

  return false;
}

//=================================================================================================

void BRepClass_FaceExplorer::InitWires()
{
  myCurrentWire = 0;
}

//=================================================================================================

bool BRepClass_FaceExplorer::RejectWire(const gp_Lin2d& theLine, const double theParameter) const
{
  (void)theLine;
  (void)theParameter;
  return false;
}

//=================================================================================================

void BRepClass_FaceExplorer::InitEdges()
{
  const WireData& aWire = myWires[myCurrentWire];
  myCurrentEdge         = aWire.FirstEdge;
  myCurrentEdgeEnd      = aWire.FirstEdge + aWire.NbEdges;
}

//=================================================================================================

bool BRepClass_FaceExplorer::RejectEdge(const gp_Lin2d& theLine, const double theParameter) const
{
  (void)theLine;
  (void)theParameter;
  return false;
}

//=================================================================================================

void BRepClass_FaceExplorer::SetUseBndBox(const bool theValue)
{
  if (!theValue || myUseBndBox)
  {
    myUseBndBox = theValue;
    return;
  }

  try
  {
    OCC_CATCH_SIGNALS
    for (const WireData& aWire : myWires)
    {
      for (uint32_t anEdgeIndex = aWire.FirstEdge; anEdgeIndex < aWire.FirstEdge + aWire.NbEdges;
           ++anEdgeIndex)
      {
        BRepClass_Edge& anEdge = myEdges[anEdgeIndex];
        if (!anEdge.Curve().IsNull() && anEdge.BoundingBox().IsVoid())
        {
          Bnd_Box2d aBox;
          BndLib_Add2dCurve::Add(anEdge.Curve(),
                                 anEdge.FirstParameter(),
                                 anEdge.LastParameter(),
                                 0.0,
                                 aBox);
          anEdge.SetCurve(anEdge.Curve(), anEdge.FirstParameter(), anEdge.LastParameter(), aBox);
        }
      }
    }
  }
  catch (const Standard_Failure&)
  {
    myUseBndBox = false;
    return;
  }
  myUseBndBox = true;
}

//=================================================================================================

bool BRepClass_FaceExplorer::ShouldUseBndBox() const
{
  if (myEdges.Size() <= THE_MIN_EDGES_FOR_BOUNDING_BOX)
  {
    return false;
  }

  size_t aNbSplineEdges = 0;
  for (const BRepClass_Edge& anEdge : myEdges)
  {
    if (anEdge.Curve().IsNull())
    {
      continue;
    }
    const GeomAbs_CurveType aType = Geom2dAdaptor_Curve(anEdge.Curve()).GetType();
    if (aType == GeomAbs_BSplineCurve || aType == GeomAbs_BezierCurve
        || aType == GeomAbs_OffsetCurve)
    {
      ++aNbSplineEdges;
    }
  }
  return aNbSplineEdges * 2 >= myEdges.Size();
}

//=================================================================================================

void BRepClass_FaceExplorer::CurrentEdge(BRepClass_Edge& E, TopAbs_Orientation& Or) const
{
  E  = myEdges[myCurrentEdge];
  Or = E.Edge().Orientation();
  E.SetMaxTolerance(myMaxTolerance);
  E.SetUseBndBox(myUseBndBox);
}
