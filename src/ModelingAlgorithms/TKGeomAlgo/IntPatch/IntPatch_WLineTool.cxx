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

#include <IntPatch_WLineTool.hxx>

#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <Bnd_Range.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <IntPatch_SpecialPoints.hxx>
#include <NCollection_IncAllocator.hxx>
#include <TopAbs_State.hxx>

// It is pure empirical value.
const double IntPatch_WLineTool::myMaxConcatAngle = M_PI / 6;

// Bit-mask is used for information about
// the operation made in
// IntPatch_WLineTool::ExtendTwoWLines(...) method.
enum
{
  IntPatchWT_EnAll         = 0x00,
  IntPatchWT_DisLastLast   = 0x01,
  IntPatchWT_DisLastFirst  = 0x02,
  IntPatchWT_DisFirstLast  = 0x04,
  IntPatchWT_DisFirstFirst = 0x08
};

enum IntPatchWT_WLsConnectionType
{
  IntPatchWT_NotConnected,
  IntPatchWT_Singular,
  IntPatchWT_Common,
  IntPatchWT_ReqExtend
};

//=======================================================================
// function : MinMax
// purpose  : Replaces theParMIN = MIN(theParMIN, theParMAX),
//                    theParMAX = MAX(theParMIN, theParMAX).
//
//           Static subfunction in IsSeamOrBound.
//=======================================================================
static inline void MinMax(double& theParMIN, double& theParMAX)
{
  if (theParMIN > theParMAX)
  {
    const double aTmp = theParMAX;
    theParMAX         = theParMIN;
    theParMIN         = aTmp;
  }
}

//=========================================================================
// function : FillPointsHash
// purpose  : Fill points hash by input data.
//            Static subfunction in ComputePurgedWLine.
//=========================================================================
static void FillPointsHash(const occ::handle<IntPatch_WLine>& theWLine,
                           NCollection_Array1<int>&           thePointsHash)
{
  // 1 - Delete point.
  // 0 - Store point.
  // -1 - Vertex point (not delete).
  int i, v;

  for (i = 1; i <= theWLine->NbPnts(); i++)
    thePointsHash.SetValue(i, 0);

  for (v = 1; v <= theWLine->NbVertex(); v++)
  {
    IntPatch_Point aVertex      = theWLine->Vertex(v);
    int            avertexindex = (int)aVertex.ParameterOnLine();
    thePointsHash.SetValue(avertexindex, -1);
  }
}

//=========================================================================
// function : MakeNewWLine
// purpose  : Makes new walking line according to the points hash
//            Static subfunction in ComputePurgedWLine and DeleteOuter.
//=========================================================================
static occ::handle<IntPatch_WLine> MakeNewWLine(const occ::handle<IntPatch_WLine>& theWLine,
                                                NCollection_Array1<int>&           thePointsHash,
                                                const bool                         theIsOuter)
{
  int i;

  occ::handle<IntSurf_LineOn2S> aPurgedLineOn2S = new IntSurf_LineOn2S();
  occ::handle<IntPatch_WLine>   aLocalWLine     = new IntPatch_WLine(aPurgedLineOn2S, false);
  aLocalWLine->SetCreatingWayInfo(theWLine->GetCreatingWay());
  int    anOldLineIdx = 1, aVertexIdx = 1, anIndexPrev = -1, anIdxOld = -1;
  gp_Pnt aPPrev, aPOld;
  for (i = 1; i <= thePointsHash.Upper(); i++)
  {
    if (thePointsHash(i) == 0)
    {
      // Point has to be added

      const gp_Pnt aP          = theWLine->Point(i).Value();
      const double aSqDistPrev = aPPrev.SquareDistance(aPOld);
      const double aSqDist     = aPPrev.SquareDistance(aP);

      const double aRatio = (aSqDistPrev < gp::Resolution()) ? 0.0 : 9.0 * aSqDist / aSqDistPrev;

      if (theIsOuter || (aRatio < gp::Resolution()) || ((1.0 < aRatio) && (aRatio < 81.0))
          || (i - anIndexPrev <= 1) || (i - anIdxOld <= 1))
      {
        // difference in distances is satisfactory
        // (1/9 < aSqDist/aSqDistPrev < 9)

        // Store this point.
        aPurgedLineOn2S->Add(theWLine->Point(i));
        anOldLineIdx++;
        aPOld       = aPPrev;
        aPPrev      = aP;
        anIdxOld    = anIndexPrev;
        anIndexPrev = i;
      }
      else if (aSqDist >= aSqDistPrev * 9.0)
      {
        // current segment is much more longer
        // (aSqDist/aSqDistPrev >= 9)

        i                = (i + anIndexPrev) / 2;
        thePointsHash(i) = 0;
        i--;
      }
      else
      {
        // previous segment is much more longer
        //(aSqDist/aSqDistPrev <= 1/9)

        if (anIndexPrev - anIdxOld > 1)
        {
          // Delete aPPrev from WL
          aPurgedLineOn2S->RemovePoint(aPurgedLineOn2S->NbPoints());
          anOldLineIdx--;

          // Insert point between aPOld and aPPrev
          i                = (anIdxOld + anIndexPrev) / 2;
          thePointsHash(i) = 0;

          aPPrev      = aPOld;
          anIndexPrev = anIdxOld;
        }
        else
        {
          aPOld    = aPPrev;
          anIdxOld = anIndexPrev;
        }

        // Next iterations will start from this inserted point.
        i--;
      }
    }
    else if (thePointsHash(i) == -1)
    {
      // Add vertex.
      IntPatch_Point aVertex = theWLine->Vertex(aVertexIdx++);
      aVertex.SetParameter(anOldLineIdx++);
      aLocalWLine->AddVertex(aVertex);
      aPurgedLineOn2S->Add(theWLine->Point(i));
      aPPrev = aPOld = theWLine->Point(i).Value();
      anIndexPrev = anIdxOld = i;
    }

    // Other points will be rejected by purger.
  }

  return aLocalWLine;
}

//=========================================================================
// function : MovePoint
// purpose  : Move point into surface param space. No interpolation used
//            because walking algorithm should care for closeness to the param space.
//            Static subfunction in ComputePurgedWLine.
//=========================================================================
static void MovePoint(const occ::handle<Adaptor3d_Surface>& theS1, double& U1, double& V1)
{
  if (U1 < theS1->FirstUParameter())
    U1 = theS1->FirstUParameter();

  if (U1 > theS1->LastUParameter())
    U1 = theS1->LastUParameter();

  if (V1 < theS1->FirstVParameter())
    V1 = theS1->FirstVParameter();

  if (V1 > theS1->LastVParameter())
    V1 = theS1->LastVParameter();
}

//=========================================================================
// function : DeleteOuterPoints
// purpose  : Check and delete out of bounds points on walking line.
//            Static subfunction in ComputePurgedWLine.
//=========================================================================
static occ::handle<IntPatch_WLine> DeleteOuterPoints(
  const occ::handle<IntPatch_WLine>&      theWLine,
  const occ::handle<Adaptor3d_Surface>&   theS1,
  const occ::handle<Adaptor3d_Surface>&   theS2,
  const occ::handle<Adaptor3d_TopolTool>& theDom1,
  const occ::handle<Adaptor3d_TopolTool>& theDom2)
{
  int i;

  NCollection_Array1<int> aDelOuterPointsHash(1, theWLine->NbPnts());
  FillPointsHash(theWLine, aDelOuterPointsHash);

  if (theS1->IsUPeriodic() || theS1->IsVPeriodic() || theS2->IsUPeriodic() || theS2->IsVPeriodic())
    return theWLine;

  gp_Pnt2d aPntOnF1, aPntOnF2;
  double   aX1, aY1, aX2, aY2;

  // Iterate over points in walking line and delete which are out of bounds.
  // Forward.
  bool isAllDeleted  = true;
  bool aChangedFirst = false;
  int  aFirstGeomIdx = 1;
  for (i = 1; i <= theWLine->NbPnts(); i++)
  {
    theWLine->Point(i).Parameters(aX1, aY1, aX2, aY2);
    aPntOnF1.SetCoord(aX1, aY1);
    aPntOnF2.SetCoord(aX2, aY2);

    TopAbs_State aState1 = theDom1->Classify(aPntOnF1, Precision::Confusion());
    TopAbs_State aState2 = theDom2->Classify(aPntOnF2, Precision::Confusion());

    if (aState1 == TopAbs_OUT || aState2 == TopAbs_OUT)
    {
      aDelOuterPointsHash(i) = 1;
      aChangedFirst          = true;
    }
    else
    {
      isAllDeleted = false;

      aFirstGeomIdx = std::max(i - 1, 1);
      if (aDelOuterPointsHash(i) == -1)
        aFirstGeomIdx = i; // Use data what lies in (i) point / vertex.

      aDelOuterPointsHash(i) = -1;
      break;
    }
  }

  if (isAllDeleted)
  {
    // ALL points are out of bounds:
    // case boolean bcut_complex F5 and similar.
    return theWLine;
  }

  // Backward.
  bool aChangedLast = false;
  int  aLastGeomIdx = theWLine->NbPnts();
  for (i = theWLine->NbPnts(); i >= 1; i--)
  {
    theWLine->Point(i).Parameters(aX1, aY1, aX2, aY2);
    aPntOnF1.SetCoord(aX1, aY1);
    aPntOnF2.SetCoord(aX2, aY2);

    TopAbs_State aState1 = theDom1->Classify(aPntOnF1, Precision::Confusion());
    TopAbs_State aState2 = theDom2->Classify(aPntOnF2, Precision::Confusion());

    if (aState1 == TopAbs_OUT || aState2 == TopAbs_OUT)
    {
      aDelOuterPointsHash(i) = 1;
      aChangedLast           = true; // Move vertex to first good point
    }
    else
    {
      aLastGeomIdx = std::min(i + 1, theWLine->NbPnts());
      if (aDelOuterPointsHash(i) == -1)
        aLastGeomIdx = i; // Use data what lies in (i) point / vertex.

      aDelOuterPointsHash(i) = -1;
      break;
    }
  }

  if (!aChangedFirst && !aChangedLast)
  {
    // Nothing is done, return input.
    return theWLine;
  }

  // Build new line and modify geometry of necessary vertices.
  occ::handle<IntPatch_WLine> aLocalWLine = MakeNewWLine(theWLine, aDelOuterPointsHash, true);

  if (aChangedFirst)
  {
    // Vertex geometry.
    IntPatch_Point aVertex = aLocalWLine->Vertex(1);
    aVertex.SetValue(theWLine->Point(aFirstGeomIdx).Value());
    double aU1, aU2, aV1, aV2;
    theWLine->Point(aFirstGeomIdx).Parameters(aU1, aV1, aU2, aV2);
    MovePoint(theS1, aU1, aV1);
    MovePoint(theS2, aU2, aV2);
    aVertex.SetParameters(aU1, aV1, aU2, aV2);
    aLocalWLine->Replace(1, aVertex);
    // Change point in walking line.
    aLocalWLine->SetPoint(1, aVertex);
  }

  if (aChangedLast)
  {
    // Vertex geometry.
    IntPatch_Point aVertex = aLocalWLine->Vertex(aLocalWLine->NbVertex());
    aVertex.SetValue(theWLine->Point(aLastGeomIdx).Value());
    double aU1, aU2, aV1, aV2;
    theWLine->Point(aLastGeomIdx).Parameters(aU1, aV1, aU2, aV2);
    MovePoint(theS1, aU1, aV1);
    MovePoint(theS2, aU2, aV2);
    aVertex.SetParameters(aU1, aV1, aU2, aV2);
    aLocalWLine->Replace(aLocalWLine->NbVertex(), aVertex);
    // Change point in walking line.
    aLocalWLine->SetPoint(aLocalWLine->NbPnts(), aVertex);
  }

  return aLocalWLine;
}

//=========================================================================
// function : IsInsideIn2d
// purpose  : Check if aNextPnt lies inside of tube build on aBasePnt and aBaseVec.
//            In 2d space. Static subfunction in DeleteByTube.
//=========================================================================
static bool IsInsideIn2d(const gp_Pnt2d& aBasePnt,
                         const gp_Vec2d& aBaseVec,
                         const gp_Pnt2d& aNextPnt,
                         const double    aSquareMaxDist)
{
  gp_Vec2d aVec2d(aBasePnt, aNextPnt);

  // d*d = (basevec^(nextpnt-basepnt))**2 / basevec**2
  double aCross      = aVec2d.Crossed(aBaseVec);
  double aSquareDist = aCross * aCross / aBaseVec.SquareMagnitude();

  return (aSquareDist <= aSquareMaxDist);
}

//=========================================================================
// function : IsInsideIn3d
// purpose  : Check if aNextPnt lies inside of tube build on aBasePnt and aBaseVec.
//            In 3d space. Static subfunction in DeleteByTube.
//=========================================================================
static bool IsInsideIn3d(const gp_Pnt& aBasePnt,
                         const gp_Vec& aBaseVec,
                         const gp_Pnt& aNextPnt,
                         const double  aSquareMaxDist)
{
  gp_Vec aVec(aBasePnt, aNextPnt);

  // d*d = (basevec^(nextpnt-basepnt))**2 / basevec**2
  double aSquareDist = aVec.CrossSquareMagnitude(aBaseVec) / aBaseVec.SquareMagnitude();

  return (aSquareDist <= aSquareMaxDist);
}

static const int aMinNbBadDistr  = 15;
static const int aNbSingleBezier = 30;

//=========================================================================
// function : IsSurfPlaneLike
// purpose  : Define is surface plane like or not.
//            Static subfunction in DeleteByTube.
//=========================================================================
static bool IsSurfPlaneLike(const occ::handle<Adaptor3d_Surface>& theS)
{
  if (theS->GetType() == GeomAbs_Plane)
  {
    return true;
  }

  if (theS->GetType() == GeomAbs_BSplineSurface)
  {
    if (theS->UDegree() == 1 && theS->VDegree() == 1)
    {
      return true;
    }
  }

  return false;
}

//=========================================================================
// function : DeleteByTube
// purpose  : Check and delete points using tube criteria.
//            Static subfunction in ComputePurgedWLine.
//=========================================================================

static occ::handle<IntPatch_WLine> DeleteByTube(const occ::handle<IntPatch_WLine>&    theWLine,
                                                const occ::handle<Adaptor3d_Surface>& theS1,
                                                const occ::handle<Adaptor3d_Surface>& theS2)
{
  // III: Check points for tube criteria:
  // Workaround to handle case of small amount points after purge.
  // Test "boolean boptuc_complex B5" and similar.
  int aNbPnt = 0, i;

  if (theWLine->NbPnts() <= 2)
    return theWLine;

  NCollection_Array1<int> aNewPointsHash(1, theWLine->NbPnts());
  FillPointsHash(theWLine, aNewPointsHash);

  // Initial computations.
  double UonS1[3], VonS1[3], UonS2[3], VonS2[3];
  theWLine->Point(1).ParametersOnS1(UonS1[0], VonS1[0]);
  theWLine->Point(2).ParametersOnS1(UonS1[1], VonS1[1]);
  theWLine->Point(1).ParametersOnS2(UonS2[0], VonS2[0]);
  theWLine->Point(2).ParametersOnS2(UonS2[1], VonS2[1]);

  gp_Pnt2d aBase2dPnt1(UonS1[0], VonS1[0]);
  gp_Pnt2d aBase2dPnt2(UonS2[0], VonS2[0]);
  gp_Vec2d aBase2dVec1(UonS1[1] - UonS1[0], VonS1[1] - VonS1[0]);
  gp_Vec2d aBase2dVec2(UonS2[1] - UonS2[0], VonS2[1] - VonS2[0]);
  gp_Pnt   aBase3dPnt = theWLine->Point(1).Value();
  gp_Vec   aBase3dVec(theWLine->Point(1).Value(), theWLine->Point(2).Value());
  double   aPrevStep = aBase3dVec.SquareMagnitude();

  // Choose base tolerance and scale it to pipe algorithm.
  constexpr double aBaseTolerance = Precision::Approximation();
  double           aResS1Tol =
    std::min(theS1->UResolution(aBaseTolerance), theS1->VResolution(aBaseTolerance));
  double aResS2Tol =
    std::min(theS2->UResolution(aBaseTolerance), theS2->VResolution(aBaseTolerance));
  double aTol1  = aResS1Tol * aResS1Tol;
  double aTol2  = aResS2Tol * aResS2Tol;
  double aTol3d = aBaseTolerance * aBaseTolerance;

  const double aLimitCoeff  = 0.99 * 0.99;
  const double aMaxSqrRatio = 15. * 15.;
  bool         isPlanePlane = IsSurfPlaneLike(theS1) && IsSurfPlaneLike(theS2);
  for (i = 3; i <= theWLine->NbPnts(); i++)
  {
    bool isDeleteState = false;

    theWLine->Point(i).ParametersOnS1(UonS1[2], VonS1[2]);
    theWLine->Point(i).ParametersOnS2(UonS2[2], VonS2[2]);
    gp_Pnt2d      aPnt2dOnS1(UonS1[2], VonS1[2]);
    gp_Pnt2d      aPnt2dOnS2(UonS2[2], VonS2[2]);
    const gp_Pnt& aPnt3d = theWLine->Point(i).Value();

    if (aNewPointsHash(i - 1) != -1 && IsInsideIn2d(aBase2dPnt1, aBase2dVec1, aPnt2dOnS1, aTol1)
        && IsInsideIn2d(aBase2dPnt2, aBase2dVec2, aPnt2dOnS2, aTol2)
        && IsInsideIn3d(aBase3dPnt, aBase3dVec, aPnt3d, aTol3d))
    {
      // Handle possible uneven parametrization on one of 2d subspaces.
      // Delete point only when expected lengths are close to each other (aLimitCoeff).
      // Example:
      // c2d1 - line
      // c3d - line
      // c2d2 - geometrically line, but have uneven parametrization -> c2d2 is bspline.
      gp_XY aPntOnS1[2] = {gp_XY(UonS1[1] - UonS1[0], VonS1[1] - VonS1[0]),
                           gp_XY(UonS1[2] - UonS1[1], VonS1[2] - VonS1[1])};
      gp_XY aPntOnS2[2] = {gp_XY(UonS2[1] - UonS2[0], VonS2[1] - VonS2[0]),
                           gp_XY(UonS2[2] - UonS2[1], VonS2[2] - VonS2[1])};

      double aStepOnS1 = aPntOnS1[0].SquareModulus() / aPntOnS1[1].SquareModulus();
      double aStepOnS2 = aPntOnS2[0].SquareModulus() / aPntOnS2[1].SquareModulus();

      // Check very rare case when wline fluctuates nearly one point and some of them may be equal.
      // Middle point will be deleted when such situation occurs.
      // bugs moddata_2 bug469.
      if (std::min(aStepOnS1, aStepOnS2) >= aLimitCoeff * std::max(aStepOnS1, aStepOnS2))
      {
        // Set hash flag to "Delete" state.
        double aCurrStep = aBase3dPnt.SquareDistance(aPnt3d);
        double aSqrRatio = 0.;
        if (!isPlanePlane)
        {
          aSqrRatio = aPrevStep / aCurrStep;
          if (aSqrRatio < 1.)
          {
            aSqrRatio = 1. / aSqrRatio;
          }
        }
        if (aSqrRatio < aMaxSqrRatio)
        {
          isDeleteState = true;
          aNewPointsHash.SetValue(i - 1, 1);

          // Change middle point.
          UonS1[1] = UonS1[2];
          UonS2[1] = UonS2[2];
          VonS1[1] = VonS1[2];
          VonS2[1] = VonS2[2];
        }
      }
    }

    if (!isDeleteState)
    {
      // Compute new pipe parameters.
      UonS1[0] = UonS1[1];
      VonS1[0] = VonS1[1];
      UonS2[0] = UonS2[1];
      VonS2[0] = VonS2[1];

      UonS1[1] = UonS1[2];
      VonS1[1] = VonS1[2];
      UonS2[1] = UonS2[2];
      VonS2[1] = VonS2[2];

      aBase2dPnt1.SetCoord(UonS1[0], VonS1[0]);
      aBase2dPnt2.SetCoord(UonS2[0], VonS2[0]);
      aBase2dVec1.SetCoord(UonS1[1] - UonS1[0], VonS1[1] - VonS1[0]);
      aBase2dVec2.SetCoord(UonS2[1] - UonS2[0], VonS2[1] - VonS2[0]);
      aBase3dPnt = theWLine->Point(i - 1).Value();
      aBase3dVec = gp_Vec(theWLine->Point(i - 1).Value(), theWLine->Point(i).Value());

      aPrevStep = aBase3dVec.SquareMagnitude();

      aNbPnt++;
    }
  }

  // Workaround to handle case of small amount of points after purge.
  // Test "boolean boptuc_complex B5" and similar.
  // This is possible since there are at least two points.
  if (aNewPointsHash(1) == -1 && aNewPointsHash(2) == -1 && aNbPnt <= 3)
  {
    // Delete first.
    aNewPointsHash(1) = 1;
  }
  if (aNewPointsHash(theWLine->NbPnts() - 1) == -1 && aNewPointsHash(theWLine->NbPnts()) == -1
      && aNbPnt <= 3)
  {
    // Delete last.
    aNewPointsHash(theWLine->NbPnts()) = 1;
  }

  // Purgre when too small amount of points left.
  if (aNbPnt <= 2)
  {
    for (i = aNewPointsHash.Lower(); i <= aNewPointsHash.Upper(); i++)
    {
      if (aNewPointsHash(i) != -1)
      {
        aNewPointsHash(i) = 1;
      }
    }
  }

  // Handle possible bad distribution of points,
  // which are will converted into one single bezier curve (less than 30 points).
  // Make distribution more even:
  // max step will be nearly to 0.1 of param distance.
  if (aNbPnt + 2 > aMinNbBadDistr && aNbPnt + 2 < aNbSingleBezier)
  {
    for (int anIdx = 1; anIdx <= 8; anIdx++)
    {
      int aHashIdx = int(anIdx * theWLine->NbPnts() / 9);

      // Vertex must be stored as VERTEX (HASH = -1)
      if (aNewPointsHash(aHashIdx) != -1)
        aNewPointsHash(aHashIdx) = 0;
    }
  }

  return MakeNewWLine(theWLine, aNewPointsHash, false);
}

//=======================================================================
// function : IsSeamOrBound
// purpose  : Returns TRUE if segment [thePtf, thePtl] intersects "seam-edge"
//            (if it exist) or surface boundaries and both thePtf and thePtl do
//            not match "seam-edge" or boundaries.
//           Point thePtmid lies in this segment (in both 3D and 2D-space).
//           If thePtmid match "seam-edge" or boundaries strictly
//            (without any tolerance) then the function will return TRUE.
//            See comments in function body for detail information.
//
//          Arrays theArrPeriods, theFBound and theLBound must be filled
//            as follows:
//          [0] - U-parameter of 1st surface;
//          [1] - V-parameter of 1st surface;
//          [2] - U-parameter of 2nd surface;
//          [3] - V-parameter of 2nd surface.
//=======================================================================
static bool IsSeamOrBound(const IntSurf_PntOn2S& thePtf,
                          const IntSurf_PntOn2S& thePtl,
                          const IntSurf_PntOn2S& thePtmid,
                          const double           theArrPeriods[4],
                          const double           theFBound[4],
                          const double           theLBound[4])
{
  double aParF[4] = {0.0, 0.0, 0.0, 0.0};
  double aParL[4] = {0.0, 0.0, 0.0, 0.0};
  thePtf.Parameters(aParF[0], aParF[1], aParF[2], aParF[3]);
  thePtl.Parameters(aParL[0], aParL[1], aParL[2], aParL[3]);

  Bnd_Range aBndR[4];

  for (int i = 0; i < 4; i++)
  {
    aBndR[i].Add(aParF[i]);
    aBndR[i].Add(aParL[i]);

    if (aBndR[i].IsIntersected(theFBound[i], theArrPeriods[i]) == Bnd_Range::IntersectStatus_In)
      return true;

    if (aBndR[i].IsIntersected(theLBound[i], theArrPeriods[i]) == Bnd_Range::IntersectStatus_In)
      return true;
  }

  for (int i = 0; i < 4; i++)
  {
    if (theArrPeriods[i] == 0.0)
    {
      // Strictly equal
      continue;
    }

    const double aDelta = std::abs(aParL[i] - aParF[i]);
    if (2.0 * aDelta > theArrPeriods[i])
    {
      // Most likely, seam is intersected.
      return true;
    }

    if (aBndR[i].IsIntersected(0.0, theArrPeriods[i]) == Bnd_Range::IntersectStatus_In)
      return true;
  }

  // The segment [thePtf, thePtl] does not intersect the boundaries and
  // the seam-edge of the surfaces.
  // Nevertheless, following situation is possible:

  //              seam or
  //               bound
  //                 |
  //    thePtf  *    |
  //                 |
  //                 * thePtmid
  //      thePtl  *  |
  //                 |

  // This case must be processed, too.

  double aMPar[4] = {0.0, 0.0, 0.0, 0.0};
  thePtmid.Parameters(aMPar[0], aMPar[1], aMPar[2], aMPar[3]);

  for (int i = 0; i < 4; i++)
  {
    const Bnd_Range aBR(aMPar[i], aMPar[i]);
    if (aBR.IsIntersected(theFBound[i], theArrPeriods[i]))
      return true;

    if (aBR.IsIntersected(theLBound[i], theArrPeriods[i]))
      return true;

    if (aBR.IsIntersected(0.0, theArrPeriods[i]))
      return true;
  }

  return false;
}

//=======================================================================
// function : IsIntersectionPoint
// purpose  : Returns True if thePmid is intersection point
//            between theS1 and theS2 with given tolerance.
//           In this case, parameters of thePmid on every quadric
//            will be recomputed and returned.
//=======================================================================
static bool IsIntersectionPoint(const gp_Pnt&                         thePmid,
                                const occ::handle<Adaptor3d_Surface>& theS1,
                                const occ::handle<Adaptor3d_Surface>& theS2,
                                const IntSurf_PntOn2S&                theRefPt,
                                const double                          theTol,
                                const double* const                   theArrPeriods,
                                IntSurf_PntOn2S&                      theNewPt)
{
  double aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;

  switch (theS1->GetType())
  {
    case GeomAbs_Plane:
      ElSLib::Parameters(theS1->Plane(), thePmid, aU1, aV1);
      break;

    case GeomAbs_Cylinder:
      ElSLib::Parameters(theS1->Cylinder(), thePmid, aU1, aV1);
      break;

    case GeomAbs_Sphere:
      ElSLib::Parameters(theS1->Sphere(), thePmid, aU1, aV1);
      break;

    case GeomAbs_Cone:
      ElSLib::Parameters(theS1->Cone(), thePmid, aU1, aV1);
      break;

    case GeomAbs_Torus:
      ElSLib::Parameters(theS1->Torus(), thePmid, aU1, aV1);
      break;

    default:
      return false;
  }

  switch (theS2->GetType())
  {
    case GeomAbs_Plane:
      ElSLib::Parameters(theS2->Plane(), thePmid, aU2, aV2);
      break;

    case GeomAbs_Cylinder:
      ElSLib::Parameters(theS2->Cylinder(), thePmid, aU2, aV2);
      break;

    case GeomAbs_Sphere:
      ElSLib::Parameters(theS2->Sphere(), thePmid, aU2, aV2);
      break;

    case GeomAbs_Cone:
      ElSLib::Parameters(theS2->Cone(), thePmid, aU2, aV2);
      break;

    case GeomAbs_Torus:
      ElSLib::Parameters(theS2->Torus(), thePmid, aU2, aV2);
      break;

    default:
      return false;
  }

  theNewPt.SetValue(thePmid, aU1, aV1, aU2, aV2);

  IntPatch_SpecialPoints::AdjustPointAndVertex(theRefPt, theArrPeriods, theNewPt);

  const gp_Pnt aP1(theS1->Value(aU1, aV1));
  const gp_Pnt aP2(theS2->Value(aU2, aV2));

  return (aP1.SquareDistance(aP2) <= theTol * theTol);
}

//=======================================================================
// function : ExtendFirst
// purpose  : Adds thePOn2S to the begin of theWline
//=======================================================================
static void ExtendFirst(const occ::handle<IntPatch_WLine>& theWline,
                        const IntSurf_PntOn2S&             theAddedPt)
{
  double aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  theAddedPt.Parameters(aU1, aV1, aU2, aV2);

  if (theAddedPt.IsSame(theWline->Point(1), Precision::Confusion()))
  {
    theWline->Curve()->Value(1, theAddedPt);
    for (int i = 1; i <= theWline->NbVertex(); i++)
    {
      IntPatch_Point& aVert = theWline->ChangeVertex(i);
      if (aVert.ParameterOnLine() != 1)
        break;

      aVert.SetParameters(aU1, aV1, aU2, aV2);
      aVert.SetValue(theAddedPt.Value());
    }

    return;
  }

  theWline->Curve()->InsertBefore(1, theAddedPt);

  for (int i = 1; i <= theWline->NbVertex(); i++)
  {
    IntPatch_Point& aVert = theWline->ChangeVertex(i);

    if (aVert.ParameterOnLine() == 1)
    {
      aVert.SetParameters(aU1, aV1, aU2, aV2);
      aVert.SetValue(theAddedPt.Value());
    }
    else
    {
      aVert.SetParameter(aVert.ParameterOnLine() + 1);
    }
  }
}

//=======================================================================
// function : ExtendLast
// purpose  : Adds thePOn2S to the end of theWline
//=======================================================================
static void ExtendLast(const occ::handle<IntPatch_WLine>& theWline,
                       const IntSurf_PntOn2S&             theAddedPt)
{
  double aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  theAddedPt.Parameters(aU1, aV1, aU2, aV2);

  const int aNbPnts = theWline->NbPnts();
  if (theAddedPt.IsSame(theWline->Point(aNbPnts), Precision::Confusion()))
  {
    theWline->Curve()->Value(aNbPnts, theAddedPt);
  }
  else
  {
    theWline->Curve()->Add(theAddedPt);
  }

  for (int i = theWline->NbVertex(); i >= 1; i--)
  {
    IntPatch_Point& aVert = theWline->ChangeVertex(i);
    if (aVert.ParameterOnLine() != aNbPnts)
      break;

    aVert.SetParameters(aU1, aV1, aU2, aV2);
    aVert.SetValue(theAddedPt.Value());
    aVert.SetParameter(theWline->NbPnts());
  }
}

//=========================================================================
// function: IsOutOfDomain
// purpose : Checks, if 2D-representation of thePOn2S is in surfaces domain,
//            defined by bounding-boxes theBoxS1 and theBoxS2
//=========================================================================
static bool IsOutOfDomain(const Bnd_Box2d&       theBoxS1,
                          const Bnd_Box2d&       theBoxS2,
                          const IntSurf_PntOn2S& thePOn2S,
                          const double* const    theArrPeriods)
{
  double aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  double aU1min = 0.0, aU1max = 0.0, aV1min = 0.0, aV1max = 0.0;
  double aU2min = 0.0, aU2max = 0.0, aV2min = 0.0, aV2max = 0.0;

  thePOn2S.Parameters(aU1, aV1, aU2, aV2);

  theBoxS1.Get(aU1min, aV1min, aU1max, aV1max);
  theBoxS2.Get(aU2min, aV2min, aU2max, aV2max);

  aU1 = ElCLib::InPeriod(aU1, aU1min, aU1min + theArrPeriods[0]);
  aV1 = ElCLib::InPeriod(aV1, aV1min, aV1min + theArrPeriods[1]);
  aU2 = ElCLib::InPeriod(aU2, aU2min, aU2min + theArrPeriods[2]);
  aV2 = ElCLib::InPeriod(aV2, aV2min, aV2min + theArrPeriods[3]);

  return (theBoxS1.IsOut(gp_Pnt2d(aU1, aV1)) || theBoxS2.IsOut(gp_Pnt2d(aU2, aV2)));
}

//=======================================================================
// function : CheckArgumentsToExtend
// purpose  : Check if extending is possible
//            (see IntPatch_WLineTool::ExtendTwoWLines)
//=======================================================================
static IntPatchWT_WLsConnectionType CheckArgumentsToExtend(
  const occ::handle<Adaptor3d_Surface>& theS1,
  const occ::handle<Adaptor3d_Surface>& theS2,
  const IntSurf_PntOn2S&                thePtWL1,
  const IntSurf_PntOn2S&                thePtWL2,
  IntSurf_PntOn2S&                      theNewPoint,
  const gp_Vec&                         theVec1,
  const gp_Vec&                         theVec2,
  const gp_Vec&                         theVec3,
  const Bnd_Box2d&                      theBoxS1,
  const Bnd_Box2d&                      theBoxS2,
  const double                          theToler3D,
  const double* const                   theArrPeriods)
{
  const double                 aSqToler = theToler3D * theToler3D;
  IntPatchWT_WLsConnectionType aRetVal  = IntPatchWT_NotConnected;
  if (theVec3.SquareMagnitude() <= aSqToler)
  {
    if ((theVec1.Angle(theVec2) > IntPatch_WLineTool::myMaxConcatAngle))
    {
      return aRetVal;
    }
    else
    {
      aRetVal = IntPatchWT_Common;
    }
  }
  else if ((theVec1.Angle(theVec2) > IntPatch_WLineTool::myMaxConcatAngle)
           || (theVec1.Angle(theVec3) > IntPatch_WLineTool::myMaxConcatAngle)
           || (theVec2.Angle(theVec3) > IntPatch_WLineTool::myMaxConcatAngle))
  {
    return aRetVal;
  }

  const gp_Pnt aPmid(0.5 * (thePtWL1.Value().XYZ() + thePtWL2.Value().XYZ()));

  double aNewPar[4] = {0.0, 0.0, 0.0, 0.0};

  // Left-bottom corner
  double aParLBC[4] = {0.0, 0.0, 0.0, 0.0};
  theBoxS1.Get(aParLBC[0], aParLBC[1], aNewPar[0], aNewPar[0]);
  theBoxS2.Get(aParLBC[2], aParLBC[3], aNewPar[0], aNewPar[0]);

  if (!IsIntersectionPoint(aPmid, theS1, theS2, thePtWL1, theToler3D, theArrPeriods, theNewPoint))
  {
    return IntPatchWT_NotConnected;
  }

  if (IsOutOfDomain(theBoxS1, theBoxS2, theNewPoint, theArrPeriods))
  {
    return IntPatchWT_NotConnected;
  }

  double aParWL1[4] = {0.0, 0.0, 0.0, 0.0}, aParWL2[4] = {0.0, 0.0, 0.0, 0.0};

  thePtWL1.Parameters(aParWL1[0], aParWL1[1], aParWL1[2], aParWL1[3]);
  thePtWL2.Parameters(aParWL2[0], aParWL2[1], aParWL2[2], aParWL2[3]);
  theNewPoint.Parameters(aNewPar[0], aNewPar[1], aNewPar[2], aNewPar[3]);

  Bnd_Range aR1, aR2;

  bool isOnBoundary = false;
  for (int i = 0; i < 4; i++)
  {
    if (theArrPeriods[i] == 0.0)
    {
      // Strictly equal
      continue;
    }

    aR1.SetVoid();
    aR1.Add(aParWL1[i]);
    aR1.Add(aParWL2[i]);

    if (aR1.IsIntersected(aParLBC[i], theArrPeriods[i]))
    {
      // Check, if we intersect surface boundary when we will extend Wline1 or Wline2
      // to theNewPoint
      MinMax(aParWL1[i], aParWL2[i]);
      if (aNewPar[i] > aParWL2[i])
      {
        // Source situation:
        //
        //---*---------------*------------*-----
        //  aParWL1[i]   aParWL2[i]    aNewPar[i]
        //
        // After possible adjusting:
        //
        //---*---------------*------------*-----
        //  aParWL1[i]   aNewPar[i]    aParWL2[i]
        //
        // Now we will be able to extend every WLine to
        // aNewPar[i] to make them close to each other.
        // However, it is necessary to add check if we
        // intersect boundary.
        const double aPar =
          aParWL1[i] + theArrPeriods[i] * std::ceil((aNewPar[i] - aParWL1[i]) / theArrPeriods[i]);
        aParWL1[i] = aParWL2[i];
        aParWL2[i] = aPar;
      }
      else if (aNewPar[i] < aParWL1[i])
      {
        // See comments to main "if".
        // Source situation:
        //
        //---*---------------*------------*-----
        //  aNewPar[i]    aParWL1[i]   aParWL2[i]
        //
        // After possible adjusting:
        //
        //---*---------------*------------*-----
        //  aParWL1[i]   aNewPar[i]    aParWL2[i]

        const double aPar =
          aParWL2[i] - theArrPeriods[i] * std::ceil((aParWL2[i] - aNewPar[i]) / theArrPeriods[i]);
        aParWL2[i] = aParWL1[i];
        aParWL1[i] = aPar;
      }

      aR1.SetVoid();
      aR2.SetVoid();
      aR1.Add(aParWL1[i]);
      aR1.Add(aNewPar[i]);
      aR2.Add(aNewPar[i]);
      aR2.Add(aParWL2[i]);

      if (aR1.IsIntersected(aParLBC[i], theArrPeriods[i])
          || aR2.IsIntersected(aParLBC[i], theArrPeriods[i]))
      {
        return IntPatchWT_NotConnected;
      }
      else
      {
        isOnBoundary = true;
      }
    }
  }

  if (isOnBoundary)
  {
    return IntPatchWT_Singular;
  }

  if (aRetVal == IntPatchWT_Common)
  {
    return IntPatchWT_Common;
  }

  return IntPatchWT_ReqExtend;
}

//=======================================================================
// function : CheckArgumentsToJoin
// purpose  : Check if joining is possible
//            (see IntPatch_WLineTool::JoinWLines(...))
//=======================================================================
bool CheckArgumentsToJoin(const occ::handle<Adaptor3d_Surface>& theS1,
                          const occ::handle<Adaptor3d_Surface>& theS2,
                          const IntSurf_PntOn2S&                thePnt,
                          const gp_Pnt&                         theP1,
                          const gp_Pnt&                         theP2,
                          const gp_Pnt&                         theP3,
                          const double                          theMinRad)
{
  const double aRad = IntPatch_PointLine::CurvatureRadiusOfIntersLine(theS1, theS2, thePnt);

  if (aRad > theMinRad)
  {
    return true;
  }
  else if (aRad > 0.0)
  {
    return false;
  }

  // Curvature radius cannot be computed.
  // Check smoothness of polygon.

  //                  theP2
  //                    *
  //                    |
  //                    |
  //       *            o         *
  //      theP1         O       theP3

  // Joining is enabled if two conditions are satisfied together:
  //   1. Angle (theP1, theP2, theP3) is quite big;
  //   2. Modulus of perpendicular (O->theP2) to the segment (theP1->theP3)
  //   is less than 0.01*<modulus of this segment>.

  const gp_Vec aV12f(theP1, theP2), aV12l(theP2, theP3);

  if (aV12f.Angle(aV12l) > IntPatch_WLineTool::myMaxConcatAngle)
    return false;

  const gp_Vec aV13(theP1, theP3);
  const double aSq13 = aV13.SquareMagnitude();

  return (aV12f.CrossSquareMagnitude(aV13) < 1.0e-4 * aSq13 * aSq13);
}

//=======================================================================
// function : ExtendTwoWLFirstFirst
// purpose  : Performs extending theWLine1 and theWLine2 through their
//            respecting start point.
//=======================================================================
static void ExtendTwoWLFirstFirst(const occ::handle<Adaptor3d_Surface>& theS1,
                                  const occ::handle<Adaptor3d_Surface>& theS2,
                                  const occ::handle<IntPatch_WLine>&    theWLine1,
                                  const occ::handle<IntPatch_WLine>&    theWLine2,
                                  const IntSurf_PntOn2S&                thePtWL1,
                                  const IntSurf_PntOn2S&                thePtWL2,
                                  const gp_Vec&                         theVec1,
                                  const gp_Vec&                         theVec2,
                                  const gp_Vec&                         theVec3,
                                  const Bnd_Box2d&                      theBoxS1,
                                  const Bnd_Box2d&                      theBoxS2,
                                  const double                          theToler3D,
                                  const double* const                   theArrPeriods,
                                  unsigned int&                         theCheckResult,
                                  bool&                                 theHasBeenJoined)
{
  IntSurf_PntOn2S                    aPOn2S;
  const IntPatchWT_WLsConnectionType aCheckRes = CheckArgumentsToExtend(theS1,
                                                                        theS2,
                                                                        thePtWL1,
                                                                        thePtWL2,
                                                                        aPOn2S,
                                                                        theVec1,
                                                                        theVec2,
                                                                        theVec3,
                                                                        theBoxS1,
                                                                        theBoxS2,
                                                                        theToler3D,
                                                                        theArrPeriods);

  if (aCheckRes != IntPatchWT_NotConnected)
    theCheckResult |= (IntPatchWT_DisFirstLast | IntPatchWT_DisLastFirst);
  else
    return;

  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL1, theArrPeriods, aPOn2S);
  ExtendFirst(theWLine1, aPOn2S);
  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL2, theArrPeriods, aPOn2S);
  ExtendFirst(theWLine2, aPOn2S);

  if (theHasBeenJoined || (aCheckRes == IntPatchWT_Singular))
    return;

  double aPrm = theWLine1->Vertex(1).ParameterOnLine();
  while (theWLine1->Vertex(1).ParameterOnLine() == aPrm)
    theWLine1->RemoveVertex(1);

  aPrm = theWLine2->Vertex(1).ParameterOnLine();
  while (theWLine2->Vertex(1).ParameterOnLine() == aPrm)
    theWLine2->RemoveVertex(1);

  const int aNbPts = theWLine2->NbPnts();
  for (int aNPt = 2; aNPt <= aNbPts; aNPt++)
  {
    const IntSurf_PntOn2S& aPt = theWLine2->Point(aNPt);
    theWLine1->Curve()->InsertBefore(1, aPt);
  }

  for (int aNVtx = 1; aNVtx <= theWLine1->NbVertex(); aNVtx++)
  {
    IntPatch_Point& aVert     = theWLine1->ChangeVertex(aNVtx);
    const double    aCurParam = aVert.ParameterOnLine();
    aVert.SetParameter(aNbPts + aCurParam - 1);
  }

  for (int aNVtx = 1; aNVtx <= theWLine2->NbVertex(); aNVtx++)
  {
    IntPatch_Point& aVert     = theWLine2->ChangeVertex(aNVtx);
    const double    aCurParam = aVert.ParameterOnLine();
    aVert.SetParameter(aNbPts - aCurParam + 1);
    theWLine1->AddVertex(aVert, true);
  }

  theHasBeenJoined = true;
}

//=======================================================================
// function : ExtendTwoWLFirstLast
// purpose  : Performs extending theWLine1 through its start point and theWLine2
//            through its end point.
//=======================================================================
static void ExtendTwoWLFirstLast(const occ::handle<Adaptor3d_Surface>& theS1,
                                 const occ::handle<Adaptor3d_Surface>& theS2,
                                 const occ::handle<IntPatch_WLine>&    theWLine1,
                                 const occ::handle<IntPatch_WLine>&    theWLine2,
                                 const IntSurf_PntOn2S&                thePtWL1,
                                 const IntSurf_PntOn2S&                thePtWL2,
                                 const gp_Vec&                         theVec1,
                                 const gp_Vec&                         theVec2,
                                 const gp_Vec&                         theVec3,
                                 const Bnd_Box2d&                      theBoxS1,
                                 const Bnd_Box2d&                      theBoxS2,
                                 const double                          theToler3D,
                                 const double* const                   theArrPeriods,
                                 unsigned int&                         theCheckResult,
                                 bool&                                 theHasBeenJoined)
{
  IntSurf_PntOn2S                    aPOn2S;
  const IntPatchWT_WLsConnectionType aCheckRes = CheckArgumentsToExtend(theS1,
                                                                        theS2,
                                                                        thePtWL1,
                                                                        thePtWL2,
                                                                        aPOn2S,
                                                                        theVec1,
                                                                        theVec2,
                                                                        theVec3,
                                                                        theBoxS1,
                                                                        theBoxS2,
                                                                        theToler3D,
                                                                        theArrPeriods);

  if (aCheckRes != IntPatchWT_NotConnected)
    theCheckResult |= IntPatchWT_DisLastLast;
  else
    return;

  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL1, theArrPeriods, aPOn2S);
  ExtendFirst(theWLine1, aPOn2S);
  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL2, theArrPeriods, aPOn2S);
  ExtendLast(theWLine2, aPOn2S);

  if (theHasBeenJoined || (aCheckRes == IntPatchWT_Singular))
    return;

  double aPrm = theWLine1->Vertex(1).ParameterOnLine();
  while (theWLine1->Vertex(1).ParameterOnLine() == aPrm)
    theWLine1->RemoveVertex(1);

  aPrm = theWLine2->Vertex(theWLine2->NbVertex()).ParameterOnLine();
  while (theWLine2->Vertex(theWLine2->NbVertex()).ParameterOnLine() == aPrm)
    theWLine2->RemoveVertex(theWLine2->NbVertex());

  const int aNbPts = theWLine2->NbPnts();
  for (int aNPt = aNbPts - 1; aNPt >= 1; aNPt--)
  {
    const IntSurf_PntOn2S& aPt = theWLine2->Point(aNPt);
    theWLine1->Curve()->InsertBefore(1, aPt);
  }

  for (int aNVtx = 1; aNVtx <= theWLine1->NbVertex(); aNVtx++)
  {
    IntPatch_Point& aVert     = theWLine1->ChangeVertex(aNVtx);
    const double    aCurParam = aVert.ParameterOnLine();
    aVert.SetParameter(aNbPts + aCurParam - 1);
  }

  for (int aNVtx = theWLine2->NbVertex(); aNVtx >= 1; aNVtx--)
  {
    const IntPatch_Point& aVert = theWLine2->Vertex(aNVtx);
    theWLine1->AddVertex(aVert, true);
  }

  theHasBeenJoined = true;
}

//=======================================================================
// function : ExtendTwoWLLastFirst
// purpose  : Performs extending theWLine1 through its end point and theWLine2
//            through its start point.
//=======================================================================
static void ExtendTwoWLLastFirst(const occ::handle<Adaptor3d_Surface>& theS1,
                                 const occ::handle<Adaptor3d_Surface>& theS2,
                                 const occ::handle<IntPatch_WLine>&    theWLine1,
                                 const occ::handle<IntPatch_WLine>&    theWLine2,
                                 const IntSurf_PntOn2S&                thePtWL1,
                                 const IntSurf_PntOn2S&                thePtWL2,
                                 const gp_Vec&                         theVec1,
                                 const gp_Vec&                         theVec2,
                                 const gp_Vec&                         theVec3,
                                 const Bnd_Box2d&                      theBoxS1,
                                 const Bnd_Box2d&                      theBoxS2,
                                 const double                          theToler3D,
                                 const double* const                   theArrPeriods,
                                 unsigned int&                         theCheckResult,
                                 bool&                                 theHasBeenJoined)
{
  IntSurf_PntOn2S                    aPOn2S;
  const IntPatchWT_WLsConnectionType aCheckRes = CheckArgumentsToExtend(theS1,
                                                                        theS2,
                                                                        thePtWL1,
                                                                        thePtWL2,
                                                                        aPOn2S,
                                                                        theVec1,
                                                                        theVec2,
                                                                        theVec3,
                                                                        theBoxS1,
                                                                        theBoxS2,
                                                                        theToler3D,
                                                                        theArrPeriods);

  if (aCheckRes != IntPatchWT_NotConnected)
    theCheckResult |= IntPatchWT_DisLastLast;
  else
    return;

  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL1, theArrPeriods, aPOn2S);
  ExtendLast(theWLine1, aPOn2S);
  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL2, theArrPeriods, aPOn2S);
  ExtendFirst(theWLine2, aPOn2S);

  if (theHasBeenJoined || (aCheckRes == IntPatchWT_Singular))
  {
    return;
  }

  double aPrm = theWLine1->Vertex(theWLine1->NbVertex()).ParameterOnLine();
  while (theWLine1->Vertex(theWLine1->NbVertex()).ParameterOnLine() == aPrm)
    theWLine1->RemoveVertex(theWLine1->NbVertex());

  aPrm = theWLine2->Vertex(1).ParameterOnLine();
  while (theWLine2->Vertex(1).ParameterOnLine() == aPrm)
    theWLine2->RemoveVertex(1);

  const int aNbPts = theWLine1->NbPnts();
  for (int aNPt = 2; aNPt <= theWLine2->NbPnts(); aNPt++)
  {
    const IntSurf_PntOn2S& aPt = theWLine2->Point(aNPt);
    theWLine1->Curve()->Add(aPt);
  }

  for (int aNVtx = 1; aNVtx <= theWLine2->NbVertex(); aNVtx++)
  {
    IntPatch_Point& aVert     = theWLine2->ChangeVertex(aNVtx);
    const double    aCurParam = aVert.ParameterOnLine();
    aVert.SetParameter(aNbPts + aCurParam - 1);
    theWLine1->AddVertex(aVert, false);
  }

  theHasBeenJoined = true;
}

//=================================================================================================

static void ExtendTwoWLLastLast(const occ::handle<Adaptor3d_Surface>& theS1,
                                const occ::handle<Adaptor3d_Surface>& theS2,
                                const occ::handle<IntPatch_WLine>&    theWLine1,
                                const occ::handle<IntPatch_WLine>&    theWLine2,
                                const IntSurf_PntOn2S&                thePtWL1,
                                const IntSurf_PntOn2S&                thePtWL2,
                                const gp_Vec&                         theVec1,
                                const gp_Vec&                         theVec2,
                                const gp_Vec&                         theVec3,
                                const Bnd_Box2d&                      theBoxS1,
                                const Bnd_Box2d&                      theBoxS2,
                                const double                          theToler3D,
                                const double* const                   theArrPeriods,
                                unsigned int&                         theCheckResult,
                                bool&                                 theHasBeenJoined)
{
  IntSurf_PntOn2S                    aPOn2S;
  const IntPatchWT_WLsConnectionType aCheckRes = CheckArgumentsToExtend(theS1,
                                                                        theS2,
                                                                        thePtWL1,
                                                                        thePtWL2,
                                                                        aPOn2S,
                                                                        theVec1,
                                                                        theVec2,
                                                                        theVec3,
                                                                        theBoxS1,
                                                                        theBoxS2,
                                                                        theToler3D,
                                                                        theArrPeriods);

  if (aCheckRes != IntPatchWT_NotConnected)
    theCheckResult |= IntPatchWT_DisLastLast;
  else
    return;

  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL1, theArrPeriods, aPOn2S);
  ExtendLast(theWLine1, aPOn2S);
  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL2, theArrPeriods, aPOn2S);
  ExtendLast(theWLine2, aPOn2S);

  if (theHasBeenJoined || (aCheckRes == IntPatchWT_Singular))
    return;

  double aPrm = theWLine1->Vertex(theWLine1->NbVertex()).ParameterOnLine();
  while (theWLine1->Vertex(theWLine1->NbVertex()).ParameterOnLine() == aPrm)
    theWLine1->RemoveVertex(theWLine1->NbVertex());

  aPrm = theWLine2->Vertex(theWLine2->NbVertex()).ParameterOnLine();
  while (theWLine2->Vertex(theWLine2->NbVertex()).ParameterOnLine() == aPrm)
    theWLine2->RemoveVertex(theWLine2->NbVertex());

  const int aNbPts = theWLine1->NbPnts() + theWLine2->NbPnts();
  for (int aNPt = theWLine2->NbPnts() - 1; aNPt >= 1; aNPt--)
  {
    const IntSurf_PntOn2S& aPt = theWLine2->Point(aNPt);
    theWLine1->Curve()->Add(aPt);
  }

  for (int aNVtx = theWLine2->NbVertex(); aNVtx >= 1; aNVtx--)
  {
    IntPatch_Point& aVert     = theWLine2->ChangeVertex(aNVtx);
    const double    aCurParam = aVert.ParameterOnLine();
    aVert.SetParameter(aNbPts - aCurParam);
    theWLine1->AddVertex(aVert, false);
  }

  theHasBeenJoined = true;
}

//=================================================================================================

occ::handle<IntPatch_WLine> IntPatch_WLineTool::ComputePurgedWLine(
  const occ::handle<IntPatch_WLine>&      theWLine,
  const occ::handle<Adaptor3d_Surface>&   theS1,
  const occ::handle<Adaptor3d_Surface>&   theS2,
  const occ::handle<Adaptor3d_TopolTool>& theDom1,
  const occ::handle<Adaptor3d_TopolTool>& theDom2)
{
  int                         i, k, v, nb, nbvtx;
  occ::handle<IntPatch_WLine> aResult;
  nbvtx = theWLine->NbVertex();
  nb    = theWLine->NbPnts();
  if (nb == 2)
  {
    const IntSurf_PntOn2S& p1 = theWLine->Point(1);
    const IntSurf_PntOn2S& p2 = theWLine->Point(2);
    if (p1.Value().IsEqual(p2.Value(), gp::Resolution()))
      return aResult;
  }

  occ::handle<IntPatch_WLine>   aLocalWLine;
  occ::handle<IntPatch_WLine>   aTmpWLine = theWLine;
  occ::handle<IntSurf_LineOn2S> aLineOn2S = new IntSurf_LineOn2S();
  aLocalWLine                             = new IntPatch_WLine(aLineOn2S, false);
  aLocalWLine->SetCreatingWayInfo(theWLine->GetCreatingWay());
  for (i = 1; i <= nb; i++)
    aLineOn2S->Add(theWLine->Point(i));

  for (v = 1; v <= nbvtx; v++)
    aLocalWLine->AddVertex(theWLine->Vertex(v));

  // I: Delete equal points
  for (i = 1; i <= aLineOn2S->NbPoints(); i++)
  {
    int aStartIndex = i + 1;
    int anEndIndex  = i + 5;
    nb              = aLineOn2S->NbPoints();
    anEndIndex      = (anEndIndex > nb) ? nb : anEndIndex;

    if ((aStartIndex > nb) || (anEndIndex <= 1))
      continue;

    k = aStartIndex;

    while (k <= anEndIndex)
    {
      if (i != k)
      {
        IntSurf_PntOn2S p1 = aLineOn2S->Value(i);
        IntSurf_PntOn2S p2 = aLineOn2S->Value(k);

        double UV[8];
        p1.Parameters(UV[0], UV[1], UV[2], UV[3]);
        p2.Parameters(UV[4], UV[5], UV[6], UV[7]);

        double aMax = std::abs(UV[0]);
        for (int anIdx = 1; anIdx < 8; anIdx++)
        {
          if (aMax < std::abs(UV[anIdx]))
            aMax = std::abs(UV[anIdx]);
        }

        if (p1.Value().IsEqual(p2.Value(), gp::Resolution())
            || std::abs(UV[0] - UV[4]) + std::abs(UV[1] - UV[5]) < 1.0e-16 * aMax
            || std::abs(UV[2] - UV[6]) + std::abs(UV[3] - UV[7]) < 1.0e-16 * aMax)
        {
          aTmpWLine   = aLocalWLine;
          aLocalWLine = new IntPatch_WLine(aLineOn2S, false);
          aLocalWLine->SetCreatingWayInfo(theWLine->GetCreatingWay());

          for (v = 1; v <= aTmpWLine->NbVertex(); v++)
          {
            IntPatch_Point aVertex      = aTmpWLine->Vertex(v);
            int            avertexindex = (int)aVertex.ParameterOnLine();

            if (avertexindex >= k)
            {
              aVertex.SetParameter(aVertex.ParameterOnLine() - 1.);
            }
            aLocalWLine->AddVertex(aVertex);
          }
          aLineOn2S->RemovePoint(k);
          anEndIndex--;
          continue;
        }
      }
      k++;
    }
  }

  if (aLineOn2S->NbPoints() <= 2)
  {
    if (aLineOn2S->NbPoints() == 2)
      return aLocalWLine;
    else
      return aResult;
  }

  // Avoid purge in case of C0 continuity:
  // Intersection approximator may produce invalid curve after purge, example:
  // bugs modalg_5 bug24731.
  // Do not run purger when base number of points is too small.
  if (theS1->UContinuity() == GeomAbs_C0 || theS1->VContinuity() == GeomAbs_C0
      || theS2->UContinuity() == GeomAbs_C0 || theS2->VContinuity() == GeomAbs_C0
      || nb < aNbSingleBezier)
  {
    return aLocalWLine;
  }

  // II: Delete out of borders points.
  aLocalWLine = DeleteOuterPoints(aLocalWLine, theS1, theS2, theDom1, theDom2);

  // III: Delete points by tube criteria.
  occ::handle<IntPatch_WLine> aLocalWLineTube = DeleteByTube(aLocalWLine, theS1, theS2);

  if (aLocalWLineTube->NbPnts() > 1)
  {
    aResult = aLocalWLineTube;
  }
  return aResult;
}

//=================================================================================================

void IntPatch_WLineTool::JoinWLines(NCollection_Sequence<occ::handle<IntPatch_Line>>& theSlin,
                                    NCollection_Sequence<IntPatch_Point>&             theSPnt,
                                    const occ::handle<Adaptor3d_Surface>&             theS1,
                                    const occ::handle<Adaptor3d_Surface>&             theS2,
                                    const double                                      theTol3D)
{
  if (theSlin.Length() == 0)
    return;

  // For two cylindrical surfaces only
  const double aMinRad = 1.0e-3 * std::min(theS1->Cylinder().Radius(), theS2->Cylinder().Radius());

  const double anArrPeriods[4] = {theS1->IsUPeriodic() ? theS1->UPeriod() : 0.0,
                                  theS1->IsVPeriodic() ? theS1->VPeriod() : 0.0,
                                  theS2->IsUPeriodic() ? theS2->UPeriod() : 0.0,
                                  theS2->IsVPeriodic() ? theS2->VPeriod() : 0.0};

  const double anArrFBonds[4] = {theS1->FirstUParameter(),
                                 theS1->FirstVParameter(),
                                 theS2->FirstUParameter(),
                                 theS2->FirstVParameter()};
  const double anArrLBonds[4] = {theS1->LastUParameter(),
                                 theS1->LastVParameter(),
                                 theS2->LastUParameter(),
                                 theS2->LastVParameter()};

  occ::handle<NCollection_IncAllocator> anAlloc = new NCollection_IncAllocator();

  for (int aN1 = 1; aN1 <= theSlin.Length(); aN1++)
  {
    occ::handle<IntPatch_WLine> aWLine1(occ::down_cast<IntPatch_WLine>(theSlin.Value(aN1)));

    if (aWLine1.IsNull())
    { // We must have failed to join not-point-lines
      continue;
    }

    const int              aNbPntsWL1 = aWLine1->NbPnts();
    const IntSurf_PntOn2S& aPntFWL1   = aWLine1->Point(1);
    const IntSurf_PntOn2S& aPntLWL1   = aWLine1->Point(aNbPntsWL1);

    for (int aNPt = 1; aNPt <= theSPnt.Length(); aNPt++)
    {
      const IntSurf_PntOn2S aPntCur = theSPnt.Value(aNPt).PntOn2S();

      if (aPntCur.IsSame(aPntFWL1, Precision::Confusion())
          || aPntCur.IsSame(aPntLWL1, Precision::Confusion()))
      {
        theSPnt.Remove(aNPt);
        aNPt--;
      }
    }

    anAlloc->Reset(false);
    NCollection_List<int> aListFC(anAlloc), aListLC(anAlloc);

    bool isFirstConnected = false, isLastConnected = false;

    for (int aN2 = 1; aN2 <= theSlin.Length(); aN2++)
    {
      if (aN2 == aN1)
        continue;

      occ::handle<IntPatch_WLine> aWLine2(occ::down_cast<IntPatch_WLine>(theSlin.Value(aN2)));

      if (aWLine2.IsNull())
        continue;

      isFirstConnected = isLastConnected = false;

      const int aNbPntsWL2 = aWLine2->NbPnts();

      const IntSurf_PntOn2S& aPntFWL2 = aWLine2->Point(1);
      const IntSurf_PntOn2S& aPntLWL2 = aWLine2->Point(aNbPntsWL2);

      double aSqDistF = aPntFWL1.Value().SquareDistance(aPntFWL2.Value());
      double aSqDistL = aPntFWL1.Value().SquareDistance(aPntLWL2.Value());

      const double aSqMinFDist = std::min(aSqDistF, aSqDistL);
      if (aSqMinFDist < Precision::SquareConfusion())
      {
        const bool             isFM = (aSqDistF < aSqDistL);
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(2);
        const IntSurf_PntOn2S& aPt2 = isFM ? aWLine2->Point(2) : aWLine2->Point(aNbPntsWL2 - 1);
        if (!IsSeamOrBound(aPt1, aPt2, aPntFWL1, anArrPeriods, anArrFBonds, anArrLBonds))
        {
          isFirstConnected = true;
        }
      }

      aSqDistF = aPntLWL1.Value().SquareDistance(aPntFWL2.Value());
      aSqDistL = aPntLWL1.Value().SquareDistance(aPntLWL2.Value());

      const double aSqMinLDist = std::min(aSqDistF, aSqDistL);
      if (aSqMinLDist < Precision::SquareConfusion())
      {
        const bool             isFM = (aSqDistF < aSqDistL);
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(aNbPntsWL1 - 1);
        const IntSurf_PntOn2S& aPt2 = isFM ? aWLine2->Point(2) : aWLine2->Point(aNbPntsWL2 - 1);
        if (!IsSeamOrBound(aPt1, aPt2, aPntLWL1, anArrPeriods, anArrFBonds, anArrLBonds))
        {
          isLastConnected = true;
        }
      }

      if (isFirstConnected && isLastConnected)
      {
        if (aSqMinFDist < aSqMinLDist)
        {
          aListFC.Append(aN2);
        }
        else
        {
          aListLC.Append(aN2);
        }
      }
      else if (isFirstConnected)
      {
        aListFC.Append(aN2);
      }
      else if (isLastConnected)
      {
        aListLC.Append(aN2);
      }
    }

    isFirstConnected = (aListFC.Extent() == 1);
    isLastConnected  = (aListLC.Extent() == 1);

    if (!(isFirstConnected || isLastConnected))
    {
      continue;
    }

    const int                   anIndexWL2 = isFirstConnected ? aListFC.First() : aListLC.First();
    occ::handle<IntPatch_WLine> aWLine2(occ::down_cast<IntPatch_WLine>(theSlin.Value(anIndexWL2)));
    const int                   aNbPntsWL2 = aWLine2->NbPnts();
    const IntSurf_PntOn2S&      aPntFWL2   = aWLine2->Point(1);
    const IntSurf_PntOn2S&      aPntLWL2   = aWLine2->Point(aNbPntsWL2);

    if (isFirstConnected)
    {
      const double aSqDistF = aPntFWL1.Value().SquareDistance(aPntFWL2.Value());
      const double aSqDistL = aPntFWL1.Value().SquareDistance(aPntLWL2.Value());
      const bool   isFM     = (aSqDistF < aSqDistL);

      const IntSurf_PntOn2S& aPt1 = aWLine1->Point(2);
      const IntSurf_PntOn2S& aPt2 = isFM ? aWLine2->Point(2) : aWLine2->Point(aNbPntsWL2 - 1);

      if (!CheckArgumentsToJoin(theS1,
                                theS2,
                                aPntFWL1,
                                aPt1.Value(),
                                aPntFWL1.Value(),
                                aPt2.Value(),
                                aMinRad))
      {
        continue;
      }

      aWLine1->ClearVertexes();

      if (isFM)
      {
        // First-First-connection
        for (int aNPt = 1; aNPt <= aNbPntsWL2; aNPt++)
        {
          const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
          aWLine1->Curve()->InsertBefore(1, aPt);
        }
      }
      else
      {
        // First-Last-connection
        for (int aNPt = aNbPntsWL2; aNPt >= 1; aNPt--)
        {
          const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
          aWLine1->Curve()->InsertBefore(1, aPt);
        }
      }
    }
    else // if (isLastConnected)
    {
      const double aSqDistF = aPntLWL1.Value().SquareDistance(aPntFWL2.Value());
      const double aSqDistL = aPntLWL1.Value().SquareDistance(aPntLWL2.Value());

      const bool             isFM = (aSqDistF < aSqDistL);
      const IntSurf_PntOn2S& aPt1 = aWLine1->Point(aNbPntsWL1 - 1);
      const IntSurf_PntOn2S& aPt2 = isFM ? aWLine2->Point(2) : aWLine2->Point(aNbPntsWL2 - 1);

      if (!CheckArgumentsToJoin(theS1,
                                theS2,
                                aPntLWL1,
                                aPt1.Value(),
                                aPntLWL1.Value(),
                                aPt2.Value(),
                                aMinRad))
      {
        continue;
      }

      aWLine1->ClearVertexes();

      if (isFM)
      {
        // Last-First connection
        for (int aNPt = 1; aNPt <= aNbPntsWL2; aNPt++)
        {
          const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
          aWLine1->Curve()->Add(aPt);
        }
      }
      else
      {
        // Last-Last connection
        for (int aNPt = aNbPntsWL2; aNPt >= 1; aNPt--)
        {
          const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
          aWLine1->Curve()->Add(aPt);
        }
      }
    }

    aWLine1->ComputeVertexParameters(theTol3D);
    theSlin.Remove(anIndexWL2);
    aN1--;
  }
}

//=======================================================================
// function : IsNeedSkipWL
// purpose  : Detect is WLine need to skip.
//=======================================================================
static bool IsNeedSkipWL(const occ::handle<IntPatch_WLine>& theWL,
                         const Bnd_Box2d&                   theBoxS1,
                         const Bnd_Box2d&                   theBoxS2,
                         const double* const                theArrPeriods)
{
  double aFirstp, aLastp;
  int    aNbVtx = theWL->NbVertex();

  for (int i = 1; i < aNbVtx; i++)
  {
    aFirstp = theWL->Vertex(i).ParameterOnLine();
    aLastp  = theWL->Vertex(i + 1).ParameterOnLine();

    double                 aU1, aV1, aU2, aV2;
    const int              pmid  = (int)((aFirstp + aLastp) / 2);
    const IntSurf_PntOn2S& aPmid = theWL->Point(pmid);
    aPmid.Parameters(aU1, aV1, aU2, aV2);

    if (IsOutOfDomain(theBoxS1, theBoxS2, aPmid, theArrPeriods))
    {
      return true;
    }
  }

  return false;
}

//=======================================================================
// function : ExtendTwoWLines
// purpose  : Performs extending theWLine1 and theWLine2 through their
//            respecting end point.
//=======================================================================
void IntPatch_WLineTool::ExtendTwoWLines(NCollection_Sequence<occ::handle<IntPatch_Line>>& theSlin,
                                         const occ::handle<Adaptor3d_Surface>&             theS1,
                                         const occ::handle<Adaptor3d_Surface>&             theS2,
                                         const double                    theToler3D,
                                         const double* const             theArrPeriods,
                                         const Bnd_Box2d&                theBoxS1,
                                         const Bnd_Box2d&                theBoxS2,
                                         const NCollection_List<gp_Pnt>& theListOfCriticalPoints)
{
  if (theSlin.Length() < 2)
    return;

  gp_Vec aVec1, aVec2, aVec3;

  unsigned int hasBeenJoinedCounter = 0;

  for (int aNumOfLine1 = 1; aNumOfLine1 <= theSlin.Length(); aNumOfLine1++)
  {
    if (hasBeenJoinedCounter > 0)
    {
      aNumOfLine1--;
    }

    hasBeenJoinedCounter = 0;

    occ::handle<IntPatch_WLine> aWLine1(occ::down_cast<IntPatch_WLine>(theSlin.Value(aNumOfLine1)));

    if (aWLine1.IsNull())
    { // We must have failed to join not-point-lines
      continue;
    }

    const int aNbPntsWL1 = aWLine1->NbPnts();

    if (aWLine1->Vertex(1).ParameterOnLine() != 1)
      continue;

    if (aWLine1->Vertex(aWLine1->NbVertex()).ParameterOnLine() != aWLine1->NbPnts())
      continue;

    const IntSurf_PntOn2S& aPntFWL1   = aWLine1->Point(1);
    const IntSurf_PntOn2S& aPntFp1WL1 = aWLine1->Point(2);

    const IntSurf_PntOn2S& aPntLWL1   = aWLine1->Point(aNbPntsWL1);
    const IntSurf_PntOn2S& aPntLm1WL1 = aWLine1->Point(aNbPntsWL1 - 1);

    if (IsNeedSkipWL(aWLine1, theBoxS1, theBoxS2, theArrPeriods))
    {
      continue;
    }

    // Enable/Disable of some check. Bit-mask is used for it.
    // E.g. if 1st point of aWLine1 matches with
    // 1st point of aWLine2 then we do not need in check
    // 1st point of aWLine1 and last point of aWLine2 etc.
    unsigned int aCheckResult = IntPatchWT_EnAll;

    // If aWLine1 is already connected with another Wline then
    // there is no point in extending.
    for (int aNumOfLine2 = aNumOfLine1 + 1; aNumOfLine2 <= theSlin.Length(); aNumOfLine2++)
    {
      occ::handle<IntPatch_WLine> aWLine2(
        occ::down_cast<IntPatch_WLine>(theSlin.Value(aNumOfLine2)));

      if (aWLine2.IsNull())
        continue;

      const IntSurf_PntOn2S& aPntFWL2 = aWLine2->Point(1);
      const IntSurf_PntOn2S& aPntLWL2 = aWLine2->Point(aWLine2->NbPnts());

      if (!(aPntFWL1.IsSame(aPntFWL2, theToler3D, Precision::PConfusion()))
          && !(aPntFWL1.IsSame(aPntLWL2, theToler3D, Precision::PConfusion())))
      {
        if (aPntFWL1.IsSame(aPntFWL2, theToler3D) || aPntFWL1.IsSame(aPntLWL2, theToler3D))
        {
          aCheckResult |= IntPatchWT_DisFirstFirst | IntPatchWT_DisFirstLast;
        }
      }

      if (!(aPntLWL1.IsSame(aPntFWL2, theToler3D, Precision::PConfusion()))
          && !(aPntLWL1.IsSame(aPntLWL2, theToler3D, Precision::PConfusion())))
      {
        if (aPntLWL1.IsSame(aPntFWL2, theToler3D) || aPntLWL1.IsSame(aPntLWL2, theToler3D))
        {
          aCheckResult |= IntPatchWT_DisLastFirst | IntPatchWT_DisLastLast;
        }
      }

      if (!theListOfCriticalPoints.IsEmpty())
      {
        for (NCollection_List<gp_Pnt>::Iterator anItr(theListOfCriticalPoints); anItr.More();
             anItr.Next())
        {
          const gp_Pnt& aPt = anItr.Value();
          if (!(aCheckResult & (IntPatchWT_DisFirstFirst | IntPatchWT_DisFirstLast)))
          {
            if (aPt.SquareDistance(aPntFWL1.Value()) < Precision::Confusion())
            {
              aCheckResult |= IntPatchWT_DisFirstFirst | IntPatchWT_DisFirstLast;
            }
          }

          if (!(aCheckResult & (IntPatchWT_DisLastFirst | IntPatchWT_DisLastLast)))
          {
            if (aPt.SquareDistance(aPntLWL1.Value()) < Precision::Confusion())
            {
              aCheckResult |= IntPatchWT_DisLastFirst | IntPatchWT_DisLastLast;
            }
          }

          if (!(aCheckResult & (IntPatchWT_DisFirstFirst | IntPatchWT_DisLastFirst)))
          {
            if (aPt.SquareDistance(aPntFWL2.Value()) < Precision::Confusion())
            {
              aCheckResult |= IntPatchWT_DisFirstFirst | IntPatchWT_DisLastFirst;
            }
          }

          if (!(aCheckResult & (IntPatchWT_DisFirstLast | IntPatchWT_DisLastLast)))
          {
            if (aPt.SquareDistance(aPntLWL2.Value()) < Precision::Confusion())
            {
              aCheckResult |= IntPatchWT_DisFirstLast | IntPatchWT_DisLastLast;
            }
          }
        }
      }
    }

    if (aCheckResult
        == (IntPatchWT_DisFirstFirst | IntPatchWT_DisFirstLast | IntPatchWT_DisLastFirst
            | IntPatchWT_DisLastLast))
      continue;

    for (int aNumOfLine2 = aNumOfLine1 + 1; aNumOfLine2 <= theSlin.Length(); aNumOfLine2++)
    {
      occ::handle<IntPatch_WLine> aWLine2(
        occ::down_cast<IntPatch_WLine>(theSlin.Value(aNumOfLine2)));

      if (aWLine2.IsNull())
        continue;

      if (aWLine2->Vertex(1).ParameterOnLine() != 1)
        continue;

      if (aWLine2->Vertex(aWLine2->NbVertex()).ParameterOnLine() != aWLine2->NbPnts())
        continue;

      bool hasBeenJoined = false;

      const int aNbPntsWL2 = aWLine2->NbPnts();

      const IntSurf_PntOn2S& aPntFWL2   = aWLine2->Point(1);
      const IntSurf_PntOn2S& aPntFp1WL2 = aWLine2->Point(2);

      const IntSurf_PntOn2S& aPntLWL2   = aWLine2->Point(aNbPntsWL2);
      const IntSurf_PntOn2S& aPntLm1WL2 = aWLine2->Point(aNbPntsWL2 - 1);

      if (IsNeedSkipWL(aWLine2, theBoxS1, theBoxS2, theArrPeriods))
      {
        continue;
      }

      if (!(aCheckResult & IntPatchWT_DisFirstFirst))
      { // First/First
        aVec1.SetXYZ(aPntFp1WL1.Value().XYZ() - aPntFWL1.Value().XYZ());
        aVec2.SetXYZ(aPntFWL2.Value().XYZ() - aPntFp1WL2.Value().XYZ());
        aVec3.SetXYZ(aPntFWL1.Value().XYZ() - aPntFWL2.Value().XYZ());

        ExtendTwoWLFirstFirst(theS1,
                              theS2,
                              aWLine1,
                              aWLine2,
                              aPntFWL1,
                              aPntFWL2,
                              aVec1,
                              aVec2,
                              aVec3,
                              theBoxS1,
                              theBoxS2,
                              theToler3D,
                              theArrPeriods,
                              aCheckResult,
                              hasBeenJoined);
      }

      if (!(aCheckResult & IntPatchWT_DisFirstLast))
      { // First/Last
        aVec1.SetXYZ(aPntFp1WL1.Value().XYZ() - aPntFWL1.Value().XYZ());
        aVec2.SetXYZ(aPntLWL2.Value().XYZ() - aPntLm1WL2.Value().XYZ());
        aVec3.SetXYZ(aPntFWL1.Value().XYZ() - aPntLWL2.Value().XYZ());

        ExtendTwoWLFirstLast(theS1,
                             theS2,
                             aWLine1,
                             aWLine2,
                             aPntFWL1,
                             aPntLWL2,
                             aVec1,
                             aVec2,
                             aVec3,
                             theBoxS1,
                             theBoxS2,
                             theToler3D,
                             theArrPeriods,
                             aCheckResult,
                             hasBeenJoined);
      }

      if (!(aCheckResult & IntPatchWT_DisLastFirst))
      { // Last/First
        aVec1.SetXYZ(aPntLWL1.Value().XYZ() - aPntLm1WL1.Value().XYZ());
        aVec2.SetXYZ(aPntFp1WL2.Value().XYZ() - aPntFWL2.Value().XYZ());
        aVec3.SetXYZ(aPntFWL2.Value().XYZ() - aPntLWL1.Value().XYZ());

        ExtendTwoWLLastFirst(theS1,
                             theS2,
                             aWLine1,
                             aWLine2,
                             aPntLWL1,
                             aPntFWL2,
                             aVec1,
                             aVec2,
                             aVec3,
                             theBoxS1,
                             theBoxS2,
                             theToler3D,
                             theArrPeriods,
                             aCheckResult,
                             hasBeenJoined);
      }

      if (!(aCheckResult & IntPatchWT_DisLastLast))
      { // Last/Last
        aVec1.SetXYZ(aPntLWL1.Value().XYZ() - aPntLm1WL1.Value().XYZ());
        aVec2.SetXYZ(aPntLm1WL2.Value().XYZ() - aPntLWL2.Value().XYZ());
        aVec3.SetXYZ(aPntLWL2.Value().XYZ() - aPntLWL1.Value().XYZ());

        ExtendTwoWLLastLast(theS1,
                            theS2,
                            aWLine1,
                            aWLine2,
                            aPntLWL1,
                            aPntLWL2,
                            aVec1,
                            aVec2,
                            aVec3,
                            theBoxS1,
                            theBoxS2,
                            theToler3D,
                            theArrPeriods,
                            aCheckResult,
                            hasBeenJoined);
      }

      if (hasBeenJoined)
      {
        hasBeenJoinedCounter++;
        theSlin.Remove(aNumOfLine2);
        aNumOfLine2--;
      }
    }
  }
}
