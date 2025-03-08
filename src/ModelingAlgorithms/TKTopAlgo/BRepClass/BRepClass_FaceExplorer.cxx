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
#include <BRepClass_Edge.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>

static const Standard_Real Probing_Start = 0.123;
static const Standard_Real Probing_End   = 0.7;
static const Standard_Real Probing_Step  = 0.2111;

//=================================================================================================

BRepClass_FaceExplorer::BRepClass_FaceExplorer(const TopoDS_Face& F)
    : myFace(F),
      myCurEdgeInd(1),
      myCurEdgePar(Probing_Start),
      myMaxTolerance(0.1),
      myUseBndBox(Standard_False),
      myUMin(Precision::Infinite()),
      myUMax(-Precision::Infinite()),
      myVMin(Precision::Infinite()),
      myVMax(-Precision::Infinite())

{
  myFace.Orientation(TopAbs_FORWARD);
}

//=================================================================================================

void BRepClass_FaceExplorer::ComputeFaceBounds()
{
  TopLoc_Location             aLocation;
  const Handle(Geom_Surface)& aSurface = BRep_Tool::Surface(myFace, aLocation);
  aSurface->Bounds(myUMin, myUMax, myVMin, myVMax);
  if (Precision::IsInfinite(myUMin) || Precision::IsInfinite(myUMax)
      || Precision::IsInfinite(myVMin) || Precision::IsInfinite(myVMax))
  {
    BRepTools::UVBounds(myFace, myUMin, myUMax, myVMin, myVMax);
  }
}

//=================================================================================================

Standard_Boolean BRepClass_FaceExplorer::CheckPoint(gp_Pnt2d& thePoint)
{
  if (myUMin > myUMax)
  {
    ComputeFaceBounds();
  }

  if (Precision::IsInfinite(myUMin) || Precision::IsInfinite(myUMax)
      || Precision::IsInfinite(myVMin) || Precision::IsInfinite(myVMax))
  {
    return Standard_True;
  }

  gp_Pnt2d      aCenterPnt((myUMin + myUMax) / 2, (myVMin + myVMax) / 2);
  Standard_Real aDistance = aCenterPnt.Distance(thePoint);
  if (Precision::IsInfinite(aDistance))
  {
    thePoint.SetCoord(myUMin - (myUMax - myUMin), myVMin - (myVMax - myVMin));
    return Standard_False;
  }
  else
  {
    Standard_Real anEpsilon = Epsilon(aDistance);
    if (anEpsilon > Max(myUMax - myUMin, myVMax - myVMin))
    {
      gp_Vec2d aLinVec(aCenterPnt, thePoint);
      gp_Dir2d aLinDir(aLinVec);
      thePoint = aCenterPnt.XY() + aLinDir.XY() * (2. * anEpsilon);
      return Standard_False;
    }
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean BRepClass_FaceExplorer::Reject(const gp_Pnt2d&) const
{
  return Standard_False;
}

//=================================================================================================

Standard_Boolean BRepClass_FaceExplorer::Segment(const gp_Pnt2d& P, gp_Lin2d& L, Standard_Real& Par)
{
  myCurEdgeInd = 1;
  myCurEdgePar = Probing_Start;

  return OtherSegment(P, L, Par);
}

//=================================================================================================

Standard_Boolean BRepClass_FaceExplorer::OtherSegment(const gp_Pnt2d& P,
                                                      gp_Lin2d&       L,
                                                      Standard_Real&  Par)
{
  TopExp_Explorer         anExpF(myFace, TopAbs_EDGE);
  Standard_Integer        i;
  Standard_Real           aFPar;
  Standard_Real           aLPar;
  Handle(Geom2d_Curve)    aC2d;
  constexpr Standard_Real aTolParConf2 = Precision::PConfusion() * Precision::PConfusion();
  gp_Pnt2d                aPOnC;
  Standard_Real           aParamIn;

  for (i = 1; anExpF.More(); anExpF.Next(), i++)
  {
    if (i != myCurEdgeInd)
      continue;

    const TopoDS_Shape&      aLocalShape   = anExpF.Current();
    const TopAbs_Orientation anOrientation = aLocalShape.Orientation();

    if (anOrientation == TopAbs_FORWARD || anOrientation == TopAbs_REVERSED)
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(aLocalShape);

      aC2d = BRep_Tool::CurveOnSurface(anEdge, myFace, aFPar, aLPar);

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
          aLPar = aFPar + 1.;

        for (; myCurEdgePar < Probing_End; myCurEdgePar += Probing_Step)
        {
          aParamIn = myCurEdgePar * aFPar + (1. - myCurEdgePar) * aLPar;

          gp_Vec2d aTanVec;
          aC2d->D1(aParamIn, aPOnC, aTanVec);
          Par = aPOnC.SquareDistance(P);

          if (Par > aTolParConf2)
          {
            gp_Vec2d aLinVec(P, aPOnC);
            gp_Dir2d aLinDir(aLinVec);

            Standard_Real aTanMod = aTanVec.SquareMagnitude();
            if (aTanMod < aTolParConf2)
              continue;
            aTanVec /= Sqrt(aTanMod);
            Standard_Real       aSinA        = aTanVec.Crossed(aLinDir.XY());
            const Standard_Real SmallAngle   = 0.001;
            Standard_Boolean    isSmallAngle = Standard_False;
            if (Abs(aSinA) < SmallAngle)
            {
              isSmallAngle = Standard_True;
              // The line from the input point P to the current point on edge
              // is tangent to the edge curve. This condition is bad for classification.
              // Therefore try to go to another point in the hope that there will be
              // no tangent. If there tangent is preserved then leave the last point in
              // order to get this edge chance to participate in classification.
              if (myCurEdgePar + Probing_Step < Probing_End)
                continue;
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
                    gp_Pnt2d      aLPOnC   = aPOnC;
                    Standard_Real aFDist   = P.SquareDistance(aFPOnC);
                    Standard_Real aLDist   = P.SquareDistance(aLPOnC);
                    Standard_Real aMinDist = aProj.LowerDistance();
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
                  myCurEdgeInd++;
                  myCurEdgePar = Probing_Start;
                }

                Par = Sqrt(Par);
                return Standard_True;
              }
            }
          }
        }
      } // if (!aC2d.IsNull()) {
    } // if (anOrientation == TopAbs_FORWARD ...

    // This curve is not valid for line construction. Go to another edge.
    myCurEdgeInd++;
    myCurEdgePar = Probing_Start;
  }

  // nothing found, return an horizontal line
  Par = RealLast();
  L   = gp_Lin2d(P, gp_Dir2d(1, 0));

  return Standard_False;
}

//=================================================================================================

void BRepClass_FaceExplorer::InitWires()
{
  myWExplorer.Init(myFace, TopAbs_WIRE);
}

//=================================================================================================

Standard_Boolean BRepClass_FaceExplorer::RejectWire(const gp_Lin2d&, const Standard_Real) const
{
  return Standard_False;
}

//=================================================================================================

void BRepClass_FaceExplorer::InitEdges()
{
  myEExplorer.Init(myWExplorer.Current(), TopAbs_EDGE);
  myMapVE.Clear();
  TopExp::MapShapesAndAncestors(myWExplorer.Current(), TopAbs_VERTEX, TopAbs_EDGE, myMapVE);
}

//=================================================================================================

Standard_Boolean BRepClass_FaceExplorer::RejectEdge(const gp_Lin2d&, const Standard_Real) const
{
  return Standard_False;
}

//=================================================================================================

void BRepClass_FaceExplorer::CurrentEdge(BRepClass_Edge& E, TopAbs_Orientation& Or) const
{
  E.Edge() = TopoDS::Edge(myEExplorer.Current());
  E.Face() = myFace;
  Or       = E.Edge().Orientation();
  E.SetNextEdge(myMapVE);
  E.SetMaxTolerance(myMaxTolerance);
  E.SetUseBndBox(myUseBndBox);
}
