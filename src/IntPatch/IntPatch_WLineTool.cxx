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

#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>

//=======================================================================
//function : MinMax
//purpose  : Replaces theParMIN = MIN(theParMIN, theParMAX),
//                    theParMAX = MAX(theParMIN, theParMAX).
//
//           Static subfunction in IsSeamOrBound.
//=======================================================================
static inline void MinMax(Standard_Real& theParMIN, Standard_Real& theParMAX)
{
  if(theParMIN > theParMAX)
  {
    const Standard_Real aTmp = theParMAX;
    theParMAX = theParMIN;
    theParMIN = aTmp;
  }
}

//=========================================================================
// function : FillPointsHash
// purpose  : Fill points hash by input data.
//            Static subfunction in ComputePurgedWLine.
//=========================================================================
static void FillPointsHash(const Handle(IntPatch_WLine)         &theWLine,
                           NCollection_Array1<Standard_Integer> &thePointsHash)
{
  // 1 - Delete point.
  // 0 - Store point.
  // -1 - Vertex point (not delete).
  Standard_Integer i, v;

  for(i = 1; i <= theWLine->NbPnts(); i++)
    thePointsHash.SetValue(i, 0);

  for(v = 1; v <= theWLine->NbVertex(); v++) 
  {
    IntPatch_Point aVertex = theWLine->Vertex(v);
    Standard_Integer avertexindex = (Standard_Integer)aVertex.ParameterOnLine();
    thePointsHash.SetValue(avertexindex, -1);
  }
}

//=========================================================================
// function : MakeNewWLine
// purpose  : Makes new walking line according to the points hash
//            Static subfunction in ComputePurgedWLine and DeleteOuter.
//=========================================================================
static Handle(IntPatch_WLine) MakeNewWLine(const Handle(IntPatch_WLine)         &theWLine,
                                           const NCollection_Array1<Standard_Integer> &thePointsHash)
{
  Standard_Integer i;

  Handle(IntSurf_LineOn2S) aPurgedLineOn2S = new IntSurf_LineOn2S();
  Handle(IntPatch_WLine) aLocalWLine = new IntPatch_WLine(aPurgedLineOn2S, Standard_False);
  Standard_Integer anOldLineIdx = 1, aVertexIdx = 1;
  for(i = 1; i <= thePointsHash.Upper(); i++)
  {
    if (thePointsHash(i) == 0)
    {
      // Store this point.
      aPurgedLineOn2S->Add(theWLine->Point(i));
      anOldLineIdx++;
    }
    else if (thePointsHash(i) == -1)
    {
      // Add vertex.
      IntPatch_Point aVertex = theWLine->Vertex(aVertexIdx++);
      aVertex.SetParameter(anOldLineIdx++);
      aLocalWLine->AddVertex(aVertex);
      aPurgedLineOn2S->Add(theWLine->Point(i));
    }
  }

  return aLocalWLine;
}

//=========================================================================
// function : MovePoint
// purpose  : Move point into surface param space. No interpolation used 
//            because walking algorithm should care for closeness to the param space.
//            Static subfunction in ComputePurgedWLine.
//=========================================================================
static void MovePoint(const Handle(Adaptor3d_HSurface)   &theS1,
                      Standard_Real &U1, Standard_Real &V1)
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
static Handle(IntPatch_WLine)
  DeleteOuterPoints(const Handle(IntPatch_WLine)       &theWLine,
                    const Handle(Adaptor3d_HSurface)   &theS1,
                    const Handle(Adaptor3d_HSurface)   &theS2,
                    const Handle(Adaptor3d_TopolTool)  &theDom1,
                    const Handle(Adaptor3d_TopolTool)  &theDom2)
{
  Standard_Integer i;

  NCollection_Array1<Standard_Integer> aDelOuterPointsHash(1, theWLine->NbPnts());
  FillPointsHash(theWLine, aDelOuterPointsHash);

  if (theS1->IsUPeriodic() || theS1->IsVPeriodic() ||
      theS2->IsUPeriodic() || theS2->IsVPeriodic() )
      return theWLine;

  gp_Pnt2d aPntOnF1, aPntOnF2;
  Standard_Real aX1, aY1, aX2, aY2;

  // Iterate over points in walking line and delete which are out of bounds.
  // Forward.
  Standard_Boolean isAllDeleted = Standard_True;
  Standard_Boolean aChangedFirst = Standard_False;
  Standard_Integer aFirstGeomIdx = 1;
  for(i = 1; i <= theWLine->NbPnts(); i++)
  {
    theWLine->Point(i).Parameters(aX1, aY1, aX2, aY2);
    aPntOnF1.SetCoord(aX1, aY1);
    aPntOnF2.SetCoord(aX2, aY2);

    TopAbs_State aState1 = theDom1->Classify(aPntOnF1, Precision::Confusion());
    TopAbs_State aState2 = theDom2->Classify(aPntOnF2, Precision::Confusion());

    if (aState1 == TopAbs_OUT ||
        aState2 == TopAbs_OUT )
    {
      aDelOuterPointsHash(i) = 1;
      aChangedFirst = Standard_True;
    }
    else
    {
      isAllDeleted = Standard_False;

      aFirstGeomIdx = Max (i - 1, 1);
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
  Standard_Boolean aChangedLast = Standard_False;
  Standard_Integer aLastGeomIdx = theWLine->NbPnts();
  for(i = theWLine->NbPnts(); i >= 1; i--)
  {
    theWLine->Point(i).Parameters(aX1, aY1, aX2, aY2);
    aPntOnF1.SetCoord(aX1, aY1);
    aPntOnF2.SetCoord(aX2, aY2);

    TopAbs_State aState1 = theDom1->Classify(aPntOnF1, Precision::Confusion());
    TopAbs_State aState2 = theDom2->Classify(aPntOnF2, Precision::Confusion());

    if (aState1 == TopAbs_OUT ||
        aState2 == TopAbs_OUT )
    {
      aDelOuterPointsHash(i) = 1;
      aChangedLast = Standard_True; // Move vertex to first good point
    }
    else
    {
      aLastGeomIdx = Min (i + 1, theWLine->NbPnts());
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

  // Build new line and modify geometry of necessary vertexes.
  Handle(IntPatch_WLine) aLocalWLine = MakeNewWLine(theWLine, aDelOuterPointsHash);

  if (aChangedFirst)
  {
    // Vertex geometry.
    IntPatch_Point aVertex = aLocalWLine->Vertex(1);
    aVertex.SetValue(theWLine->Point(aFirstGeomIdx).Value());
    Standard_Real aU1, aU2, aV1, aV2;
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
    Standard_Real aU1, aU2, aV1, aV2;
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
static Standard_Boolean IsInsideIn2d(const gp_Pnt2d& aBasePnt,
                                     const gp_Vec2d& aBaseVec,
                                     const gp_Pnt2d& aNextPnt,
                                     const Standard_Real aSquareMaxDist)
{
  gp_Vec2d aVec2d(aBasePnt, aNextPnt);

  //d*d = (basevec^(nextpnt-basepnt))**2 / basevec**2
  Standard_Real aCross = aVec2d.Crossed(aBaseVec);
  Standard_Real aSquareDist = aCross * aCross
                            / aBaseVec.SquareMagnitude();

  return (aSquareDist <= aSquareMaxDist);
}

//=========================================================================
// function : IsInsideIn3d
// purpose  : Check if aNextPnt lies inside of tube build on aBasePnt and aBaseVec.
//            In 3d space. Static subfunction in DeleteByTube.
//=========================================================================
static Standard_Boolean IsInsideIn3d(const gp_Pnt& aBasePnt,
                                     const gp_Vec& aBaseVec,
                                     const gp_Pnt& aNextPnt,
                                     const Standard_Real aSquareMaxDist)
{
  gp_Vec aVec(aBasePnt, aNextPnt);

  //d*d = (basevec^(nextpnt-basepnt))**2 / basevec**2
  Standard_Real aSquareDist = aVec.CrossSquareMagnitude(aBaseVec)
                            / aBaseVec.SquareMagnitude();

  return (aSquareDist <= aSquareMaxDist);
}

static const Standard_Integer aMinNbBadDistr = 15;
static const Standard_Integer aNbSingleBezier = 30;

//=========================================================================
// function : DeleteByTube
// purpose  : Check and delete points using tube criteria.
//            Static subfunction in ComputePurgedWLine.
//=========================================================================
static Handle(IntPatch_WLine)
  DeleteByTube(const Handle(IntPatch_WLine)       &theWLine,
               const Handle(Adaptor3d_HSurface)   &theS1,
               const Handle(Adaptor3d_HSurface)   &theS2)
{
  // III: Check points for tube criteria:
  // Workaround to handle case of small amount points after purge.
  // Test "boolean boptuc_complex B5" and similar.
  Standard_Integer aNbPnt = 0 , i;

  if (theWLine->NbPnts() <= 2)
    return theWLine;

  NCollection_Array1<Standard_Integer> aNewPointsHash(1, theWLine->NbPnts());
  FillPointsHash(theWLine, aNewPointsHash);
  
  // Inital computations.
  Standard_Real UonS1[3], VonS1[3], UonS2[3], VonS2[3];
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

  // Choose base tolerance and scale it to pipe algorithm.
  const Standard_Real aBaseTolerance = Precision::Approximation();
  Standard_Real aResS1Tol = Min(theS1->UResolution(aBaseTolerance),
                                theS1->VResolution(aBaseTolerance));
  Standard_Real aResS2Tol = Min(theS2->UResolution(aBaseTolerance),
                                theS2->VResolution(aBaseTolerance));
  Standard_Real aTol1 = aResS1Tol * aResS1Tol;
  Standard_Real aTol2 = aResS2Tol * aResS2Tol;
  Standard_Real aTol3d = aBaseTolerance * aBaseTolerance;

  const Standard_Real aLimitCoeff = 0.99 * 0.99;
  for(i = 3; i <= theWLine->NbPnts(); i++)
  {
    Standard_Boolean isDeleteState = Standard_False;

    theWLine->Point(i).ParametersOnS1(UonS1[2], VonS1[2]);
    theWLine->Point(i).ParametersOnS2(UonS2[2], VonS2[2]);
    gp_Pnt2d aPnt2dOnS1(UonS1[2], VonS1[2]);
    gp_Pnt2d aPnt2dOnS2(UonS2[2], VonS2[2]);
    const gp_Pnt& aPnt3d = theWLine->Point(i).Value();

    if (aNewPointsHash(i - 1) != - 1 &&
        IsInsideIn2d(aBase2dPnt1, aBase2dVec1, aPnt2dOnS1, aTol1) &&
        IsInsideIn2d(aBase2dPnt2, aBase2dVec2, aPnt2dOnS2, aTol2) &&
        IsInsideIn3d(aBase3dPnt, aBase3dVec, aPnt3d, aTol3d) )
    {
      // Handle possible uneven parametrization on one of 2d subspaces.
      // Delete point only when expected lengths are close to each other (aLimitCoeff).
      // Example:
      // c2d1 - line
      // c3d - line
      // c2d2 - geometrically line, but have uneven parametrization -> c2d2 is bspline.
      gp_XY aPntOnS1[2]= { gp_XY(UonS1[1] - UonS1[0], VonS1[1] - VonS1[0])
                         , gp_XY(UonS1[2] - UonS1[1], VonS1[2] - VonS1[1])};
      gp_XY aPntOnS2[2]= { gp_XY(UonS2[1] - UonS2[0], VonS2[1] - VonS2[0])
                         , gp_XY(UonS2[2] - UonS2[1], VonS2[2] - VonS2[1])};

      Standard_Real aStepOnS1 = aPntOnS1[0].SquareModulus() / aPntOnS1[1].SquareModulus();
      Standard_Real aStepOnS2 = aPntOnS2[0].SquareModulus() / aPntOnS2[1].SquareModulus();

      Standard_Real aStepCoeff = Min(aStepOnS1, aStepOnS2) / Max(aStepOnS1, aStepOnS2);

      if (aStepCoeff > aLimitCoeff)
      {
        // Set hash flag to "Delete" state.
        isDeleteState = Standard_True;
        aNewPointsHash.SetValue(i - 1, 1);

        // Change middle point.
        UonS1[1] = UonS1[2];
        UonS2[1] = UonS2[2];
        VonS1[1] = VonS1[2];
        VonS2[1] = VonS2[2];
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

      aNbPnt++;
    }
  }

  // Workaround to handle case of small amount of points after purge.
  // Test "boolean boptuc_complex B5" and similar.
  // This is possible since there are at least two points.
  if (aNewPointsHash(1) == -1 &&
      aNewPointsHash(2) == -1 &&
      aNbPnt <= 3)
  {
    // Delete first.
    aNewPointsHash(1) = 1;
  }
  if (aNewPointsHash(theWLine->NbPnts() - 1) == -1 &&
      aNewPointsHash(theWLine->NbPnts()    ) == -1 &&
      aNbPnt <= 3)
  {
    // Delete last.
    aNewPointsHash(theWLine->NbPnts()) = 1;
  }

  // Purgre when too small amount of points left.
  if (aNbPnt <= 2)
  {
    for(i = aNewPointsHash.Lower(); i <= aNewPointsHash.Upper(); i++)
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
  if (aNbPnt + 2 > aMinNbBadDistr &&
      aNbPnt + 2 < aNbSingleBezier )
  {
    for(Standard_Integer anIdx = 1; anIdx <= 8; anIdx++)
    {
      Standard_Integer aHashIdx = 
        Standard_Integer(anIdx * theWLine->NbPnts() / 9);

      //Vertex must be stored as VERTEX (HASH = -1)
      if (aNewPointsHash(aHashIdx) != -1)
        aNewPointsHash(aHashIdx) = 0;
    }
  }

  return MakeNewWLine(theWLine, aNewPointsHash);
}

//=======================================================================
//function : IsSeam
//purpose  : Returns:
//            0 - if interval [theU1, theU2] does not intersect the "seam-edge"
//                or if "seam-edge" do not exist;
//            1 - if interval (theU1, theU2) intersect the "seam-edge".
//            2 - if theU1 or/and theU2 lie ON the "seam-edge"
//
//ATTENTION!!!
//  If (theU1 == theU2) then this function will return only both 0 or 2.
//
//           Static subfunction in IsSeamOrBound.
//=======================================================================
static Standard_Integer IsSeam( const Standard_Real theU1,
                                const Standard_Real theU2,
                                const Standard_Real thePeriod)
{
  if(IsEqual(thePeriod, 0.0))
    return 0;

  //If interval [theU1, theU2] intersect seam-edge then there exists an integer
  //number N such as 
  //    (theU1 <= T*N <= theU2) <=> (theU1/T <= N <= theU2/T),
  //where T is the period.
  //I.e. the inerval [theU1/T, theU2/T] must contain at least one
  //integer number. In this case, Floor(theU1/T) and Floor(theU2/T)
  //return different values or theU1/T is strictly integer number.
  //Examples:
  //  1. theU1/T==2.8, theU2/T==3.5 => Floor(theU1/T) == 2, Floor(theU2/T) == 3.
  //  2. theU1/T==2.0, theU2/T==2.6 => Floor(theU1/T) == Floor(theU2/T) == 2.

  const Standard_Real aVal1 = theU1/thePeriod,
                      aVal2 = theU2/thePeriod;
  const Standard_Integer aPar1 = static_cast<Standard_Integer>(Floor(aVal1));
  const Standard_Integer aPar2 = static_cast<Standard_Integer>(Floor(aVal2));
  if(aPar1 != aPar2)
  {//Interval (theU1, theU2] intersects seam-edge
    if(IsEqual(aVal2, static_cast<Standard_Real>(aPar2)))
    {//aVal2 is an integer number => theU2 lies ON the "seam-edge"
      return 2;
    }

    return 1;
  }

  //Here, aPar1 == aPar2. 

  if(IsEqual(aVal1, static_cast<Standard_Real>(aPar1)))
  {//aVal1 is an integer number => theU1 lies ON the "seam-edge"
    return 2;
  }

  //If aVal2 is a true integer number then always (aPar1 != aPar2).

  return 0;
}

//=======================================================================
//function : IsSeamOrBound
//purpose  : Returns TRUE if segment [thePtf, thePtl] intersects "seam-edge"
//            (if it exist) or surface boundaries and both thePtf and thePtl do
//            not match "seam-edge" or boundaries.
//           Point thePtmid lies in this segment (in both 3D and 2D-space).
//           If thePtmid match "seam-edge" or boundaries strictly 
//            (without any tolerance) then the function will return TRUE.
//            See comments in function body for detail information.
//=======================================================================
static Standard_Boolean IsSeamOrBound(const IntSurf_PntOn2S& thePtf,
                                      const IntSurf_PntOn2S& thePtl,
                                      const IntSurf_PntOn2S& thePtmid,
                                      const Standard_Real theU1Period,
                                      const Standard_Real theU2Period,
                                      const Standard_Real theV1Period,
                                      const Standard_Real theV2Period,
                                      const Standard_Real theUfSurf1,
                                      const Standard_Real theUlSurf1,
                                      const Standard_Real theVfSurf1,
                                      const Standard_Real theVlSurf1,
                                      const Standard_Real theUfSurf2,
                                      const Standard_Real theUlSurf2,
                                      const Standard_Real theVfSurf2,
                                      const Standard_Real theVlSurf2)
{
  Standard_Real aU11 = 0.0, aU12 = 0.0, aV11 = 0.0, aV12 = 0.0;
  Standard_Real aU21 = 0.0, aU22 = 0.0, aV21 = 0.0, aV22 = 0.0;
  thePtf.Parameters(aU11, aV11, aU12, aV12);
  thePtl.Parameters(aU21, aV21, aU22, aV22);

  MinMax(aU11, aU21);
  MinMax(aV11, aV21);
  MinMax(aU12, aU22);
  MinMax(aV12, aV22);

  if((aU11 - theUfSurf1)*(aU21 - theUfSurf1) < 0.0)
  {//Interval [aU11, aU21] intersects theUfSurf1
    return Standard_True;
  }

  if((aU11 - theUlSurf1)*(aU21 - theUlSurf1) < 0.0)
  {//Interval [aU11, aU21] intersects theUlSurf1
    return Standard_True;
  }

  if((aV11 - theVfSurf1)*(aV21 - theVfSurf1) < 0.0)
  {//Interval [aV11, aV21] intersects theVfSurf1
    return Standard_True;
  }

  if((aV11 - theVlSurf1)*(aV21 - theVlSurf1) < 0.0)
  {//Interval [aV11, aV21] intersects theVlSurf1
    return Standard_True;
  }

  if((aU12 - theUfSurf2)*(aU22 - theUfSurf2) < 0.0)
  {//Interval [aU12, aU22] intersects theUfSurf2
    return Standard_True;
  }

  if((aU12 - theUlSurf2)*(aU22 - theUlSurf2) < 0.0)
  {//Interval [aU12, aU22] intersects theUlSurf2
    return Standard_True;
  }

  if((aV12 - theVfSurf2)*(aV22 - theVfSurf2) < 0.0)
  {//Interval [aV12, aV22] intersects theVfSurf2
    return Standard_True;
  }

  if((aV12 - theVlSurf2)*(aV22 - theVlSurf2) < 0.0)
  {//Interval [aV12, aV22] intersects theVlSurf2
    return Standard_True;
  }

  if(IsSeam(aU11, aU21, theU1Period))
    return Standard_True;

  if(IsSeam(aV11, aV21, theV1Period))
    return Standard_True;

  if(IsSeam(aU12, aU22, theU2Period))
    return Standard_True;

  if(IsSeam(aV12, aV22, theV2Period))
    return Standard_True;

  /*
    The segment [thePtf, thePtl] does not intersect the boundaries and
    the seam-edge of the surfaces.
    Nevertheless, following situation is possible:

                  seam or
                   bound
                     |
        thePtf  *    |
                     |
                     * thePtmid
          thePtl  *  |
                     |

    This case must be processed, too.
  */

  Standard_Real aU1 = 0.0, aU2 = 0.0, aV1 = 0.0, aV2 = 0.0;
  thePtmid.Parameters(aU1, aV1, aU2, aV2);

  if(IsEqual(aU1, theUfSurf1) || IsEqual(aU1, theUlSurf1))
    return Standard_True;

  if(IsEqual(aU2, theUfSurf2) || IsEqual(aU2, theUlSurf2))
    return Standard_True;

  if(IsEqual(aV1, theVfSurf1) || IsEqual(aV1, theVlSurf1))
    return Standard_True;

  if(IsEqual(aV2, theVfSurf2) || IsEqual(aV2, theVlSurf2))
    return Standard_True;

  if(IsSeam(aU1, aU1, theU1Period))
    return Standard_True;

  if(IsSeam(aU2, aU2, theU2Period))
    return Standard_True;

  if(IsSeam(aV1, aV1, theV1Period))
    return Standard_True;

  if(IsSeam(aV2, aV2, theV2Period))
    return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : AbjustPeriodicToPrevPoint
//purpose  : Returns theCurrentParam in order to the distance betwen 
//            theRefParam and theCurrentParam is less than 0.5*thePeriod.
//=======================================================================
static void AbjustPeriodicToPrevPoint(const Standard_Real theRefParam,
                                      const Standard_Real thePeriod,
                                      Standard_Real& theCurrentParam)
{
  if(thePeriod == 0.0)
    return;

  Standard_Real aDeltaPar = 2.0*(theRefParam - theCurrentParam);
  const Standard_Real anIncr = Sign(thePeriod, aDeltaPar);
  while(Abs(aDeltaPar) > thePeriod)
  {
    theCurrentParam += anIncr;
    aDeltaPar = 2.0*(theRefParam-theCurrentParam);
  }
}

//=======================================================================
//function : IsIntersectionPoint
//purpose  : Returns True if thePmid is intersection point
//            between theS1 and theS2 with given tolerance.
//           In this case, parameters of thePmid on every quadric
//            will be recomputed and returned.
//=======================================================================
static Standard_Boolean IsIntersectionPoint(const gp_Pnt& thePmid,
                                            const IntSurf_Quadric& theS1,
                                            const IntSurf_Quadric& theS2,
                                            const IntSurf_PntOn2S& theRefPt,
                                            const Standard_Real theTol,
                                            const Standard_Real theU1Period,
                                            const Standard_Real theU2Period,
                                            const Standard_Real theV1Period,
                                            const Standard_Real theV2Period,
                                            Standard_Real &theU1,
                                            Standard_Real &theV1,
                                            Standard_Real &theU2,
                                            Standard_Real &theV2)
{
  Standard_Real aU1Ref = 0.0, aV1Ref = 0.0, aU2Ref = 0.0, aV2Ref = 0.0;
  theRefPt.Parameters(aU1Ref, aV1Ref, aU2Ref, aV2Ref);
  theS1.Parameters(thePmid, theU1, theV1);
  theS2.Parameters(thePmid, theU2, theV2);

  AbjustPeriodicToPrevPoint(aU1Ref, theU1Period, theU1);
  AbjustPeriodicToPrevPoint(aV1Ref, theV1Period, theV1);
  AbjustPeriodicToPrevPoint(aU2Ref, theU2Period, theU2);
  AbjustPeriodicToPrevPoint(aV2Ref, theV2Period, theV2);

  const gp_Pnt aP1(theS1.Value(theU1, theV1));
  const gp_Pnt aP2(theS2.Value(theU2, theV2));

  return (aP1.SquareDistance(aP2) <= theTol*theTol);
}

//=======================================================================
//function : ExtendFirst
//purpose  : Adds thePOn2S to the begin of theWline
//=======================================================================
static void ExtendFirst(const Handle(IntPatch_WLine)& theWline,
                        const IntSurf_PntOn2S &thePOn2S)
{
  theWline->Curve()->InsertBefore(1, thePOn2S);

  IntPatch_Point &aVert = theWline->ChangeVertex(1);

  Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  thePOn2S.Parameters(aU1, aV1, aU2, aV2);

  aVert.SetParameters(aU1, aV1, aU2, aV2);
  aVert.SetValue(thePOn2S.Value());

  for(Standard_Integer i = 2; i <= theWline->NbVertex(); i++)
  {
    IntPatch_Point &aV = theWline->ChangeVertex(i);
    aV.SetParameter(aV.ParameterOnLine()+1);
  }
}

//=======================================================================
//function : ExtendLast
//purpose  : Adds thePOn2S to the end of theWline
//=======================================================================
static void ExtendLast(const Handle(IntPatch_WLine)& theWline,
                       const IntSurf_PntOn2S &thePOn2S)
{
  theWline->Curve()->Add(thePOn2S);

  IntPatch_Point &aVert = theWline->ChangeVertex(theWline->NbVertex());

  Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  thePOn2S.Parameters(aU1, aV1, aU2, aV2);

  aVert.SetParameters(aU1, aV1, aU2, aV2);
  aVert.SetValue(thePOn2S.Value());
  aVert.SetParameter(theWline->NbPnts());
}

//=========================================================================
// function : ComputePurgedWLine
// purpose  :
//=========================================================================
Handle(IntPatch_WLine) IntPatch_WLineTool::
  ComputePurgedWLine(const Handle(IntPatch_WLine)       &theWLine,
                     const Handle(Adaptor3d_HSurface)   &theS1,
                     const Handle(Adaptor3d_HSurface)   &theS2,
                     const Handle(Adaptor3d_TopolTool)  &theDom1,
                     const Handle(Adaptor3d_TopolTool)  &theDom2)
{
  Standard_Integer i, k, v, nb, nbvtx;
  Handle(IntPatch_WLine) aResult;
  nbvtx = theWLine->NbVertex();
  nb = theWLine->NbPnts();
  if (nb==2)
  {
    const IntSurf_PntOn2S& p1 = theWLine->Point(1);
    const IntSurf_PntOn2S& p2 = theWLine->Point(2);
    if(p1.Value().IsEqual(p2.Value(), gp::Resolution()))
      return aResult;
  }

  Handle(IntPatch_WLine) aLocalWLine;
  Handle(IntPatch_WLine) aTmpWLine = theWLine;
  Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
  aLocalWLine = new IntPatch_WLine(aLineOn2S, Standard_False);
  for(i = 1; i <= nb; i++)
    aLineOn2S->Add(theWLine->Point(i));

  for(v = 1; v <= nbvtx; v++)
    aLocalWLine->AddVertex(theWLine->Vertex(v));

  // I: Delete equal points
  for(i = 1; i <= aLineOn2S->NbPoints(); i++)
  {
    Standard_Integer aStartIndex = i + 1;
    Standard_Integer anEndIndex = i + 5;
    nb = aLineOn2S->NbPoints();
    anEndIndex = (anEndIndex > nb) ? nb : anEndIndex;

    if((aStartIndex > nb) || (anEndIndex <= 1))
      continue;

    k = aStartIndex;

    while(k <= anEndIndex)
    {
      if(i != k)
      {
        IntSurf_PntOn2S p1 = aLineOn2S->Value(i);
        IntSurf_PntOn2S p2 = aLineOn2S->Value(k);
        
        Standard_Real UV[8];
        p1.Parameters(UV[0], UV[1], UV[2], UV[3]);
        p2.Parameters(UV[4], UV[5], UV[6], UV[7]);

        Standard_Real aMax = Abs(UV[0]);
        for(Standard_Integer anIdx = 1; anIdx < 8; anIdx++)
        {
          if (aMax < Abs(UV[anIdx]))
            aMax = Abs(UV[anIdx]);
        }

        if(p1.Value().IsEqual(p2.Value(), gp::Resolution()) ||
           Abs(UV[0] - UV[4]) + Abs(UV[1] - UV[5]) < 1.0e-16 * aMax ||
           Abs(UV[2] - UV[6]) + Abs(UV[3] - UV[7]) < 1.0e-16 * aMax )
        {
          aTmpWLine = aLocalWLine;
          aLocalWLine = new IntPatch_WLine(aLineOn2S, Standard_False);
          
          for(v = 1; v <= aTmpWLine->NbVertex(); v++)
          {
            IntPatch_Point aVertex = aTmpWLine->Vertex(v);
            Standard_Integer avertexindex = (Standard_Integer)aVertex.ParameterOnLine();

            if(avertexindex >= k)
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
  if (theS1->UContinuity() == GeomAbs_C0 ||
      theS1->VContinuity() == GeomAbs_C0 ||
      theS2->UContinuity() == GeomAbs_C0 ||
      theS2->VContinuity() == GeomAbs_C0 ||
      nb < aNbSingleBezier)
  {
    return aLocalWLine;
  }

  // II: Delete out of borders points.
  Handle(IntPatch_WLine) aLocalWLineOuter = 
    DeleteOuterPoints(aLocalWLine, theS1, theS2, theDom1, theDom2);

  // III: Delete points by tube criteria.
  Handle(IntPatch_WLine) aLocalWLineTube = 
    DeleteByTube(aLocalWLineOuter, theS1, theS2);

  if(aLocalWLineTube->NbPnts() > 1)
  {
    aResult = aLocalWLineTube;
  }
  return aResult;
}


//=======================================================================
//function : JoinWLines
//purpose  :
//=======================================================================
void IntPatch_WLineTool::JoinWLines(IntPatch_SequenceOfLine& theSlin,
                                    IntPatch_SequenceOfPoint& theSPnt,
                                    const Standard_Real theTol3D,
                                    const Standard_Real theU1Period,
                                    const Standard_Real theU2Period,
                                    const Standard_Real theV1Period,
                                    const Standard_Real theV2Period,
                                    const Standard_Real theUfSurf1,
                                    const Standard_Real theUlSurf1,
                                    const Standard_Real theVfSurf1,
                                    const Standard_Real theVlSurf1,
                                    const Standard_Real theUfSurf2,
                                    const Standard_Real theUlSurf2,
                                    const Standard_Real theVfSurf2,
                                    const Standard_Real theVlSurf2)
{
  if(theSlin.Length() == 0)
    return;

  for(Standard_Integer aNumOfLine1 = 1; aNumOfLine1 <= theSlin.Length(); aNumOfLine1++)
  {
    Handle(IntPatch_WLine) aWLine1 (Handle(IntPatch_WLine)::DownCast(theSlin.Value(aNumOfLine1)));

    if(aWLine1.IsNull())
    {//We must have failed to join not-point-lines
      continue;
    }

    const Standard_Integer aNbPntsWL1 = aWLine1->NbPnts();
    const IntSurf_PntOn2S& aPntFW1 = aWLine1->Point(1);
    const IntSurf_PntOn2S& aPntLW1 = aWLine1->Point(aNbPntsWL1);

    for(Standard_Integer aNPt = 1; aNPt <= theSPnt.Length(); aNPt++)
    {
      const IntSurf_PntOn2S aPntCur = theSPnt.Value(aNPt).PntOn2S();

      if( aPntCur.IsSame(aPntFW1, Precision::Confusion()) ||
        aPntCur.IsSame(aPntLW1, Precision::Confusion()))
      {
        theSPnt.Remove(aNPt);
        aNPt--;
      }
    }

    Standard_Boolean hasBeenRemoved = Standard_False;
    for(Standard_Integer aNumOfLine2 = aNumOfLine1 + 1; aNumOfLine2 <= theSlin.Length(); aNumOfLine2++)
    {
      Handle(IntPatch_WLine) aWLine2 (Handle(IntPatch_WLine)::DownCast(theSlin.Value(aNumOfLine2)));

      if(aWLine2.IsNull())
        continue;

      const Standard_Integer aNbPntsWL2 = aWLine2->NbPnts();

      const IntSurf_PntOn2S& aPntFWL1 = aWLine1->Point(1);
      const IntSurf_PntOn2S& aPntLWL1 = aWLine1->Point(aNbPntsWL1);

      const IntSurf_PntOn2S& aPntFWL2 = aWLine2->Point(1);
      const IntSurf_PntOn2S& aPntLWL2 = aWLine2->Point(aNbPntsWL2);

      if(aPntFWL1.IsSame(aPntFWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(2);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(2);
        if(!IsSeamOrBound(aPt1, aPt2, aPntFWL1, theU1Period, theU2Period,
                          theV1Period, theV2Period, theUfSurf1, theUlSurf1,
                          theVfSurf1, theVlSurf1, theUfSurf2, theUlSurf2,
                          theVfSurf2, theVlSurf2))
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = 1; aNPt <= aNbPntsWL2; aNPt++)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->InsertBefore(1, aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }

      if(aPntFWL1.IsSame(aPntLWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(2);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(aNbPntsWL2-1);
        if(!IsSeamOrBound(aPt1, aPt2, aPntFWL1, theU1Period, theU2Period,
                          theV1Period, theV2Period, theUfSurf1, theUlSurf1,
                          theVfSurf1, theVlSurf1, theUfSurf2, theUlSurf2,
                          theVfSurf2, theVlSurf2))
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = aNbPntsWL2; aNPt >= 1; aNPt--)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->InsertBefore(1, aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }

      if(aPntLWL1.IsSame(aPntFWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(aNbPntsWL1-1);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(2);
        if(!IsSeamOrBound(aPt1, aPt2, aPntLWL1, theU1Period, theU2Period,
                          theV1Period, theV2Period, theUfSurf1, theUlSurf1,
                          theVfSurf1, theVlSurf1, theUfSurf2, theUlSurf2,
                          theVfSurf2, theVlSurf2))
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = 1; aNPt <= aNbPntsWL2; aNPt++)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->Add(aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }

      if(aPntLWL1.IsSame(aPntLWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(aNbPntsWL1-1);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(aNbPntsWL2-1);
        if(!IsSeamOrBound(aPt1, aPt2, aPntLWL1, theU1Period, theU2Period,
                          theV1Period, theV2Period, theUfSurf1, theUlSurf1,
                          theVfSurf1, theVlSurf1, theUfSurf2, theUlSurf2,
                          theVfSurf2, theVlSurf2))
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = aNbPntsWL2; aNPt >= 1; aNPt--)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->Add(aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }
    }

    if(hasBeenRemoved)
      aNumOfLine1--;
  }
}

//=======================================================================
//function : ExtendTwoWlinesToEachOther
//purpose  : 
//=======================================================================
void IntPatch_WLineTool::ExtendTwoWlinesToEachOther(IntPatch_SequenceOfLine& theSlin,
                                                    const IntSurf_Quadric& theS1,
                                                    const IntSurf_Quadric& theS2,
                                                    const Standard_Real theToler3D,
                                                    const Standard_Real theU1Period,
                                                    const Standard_Real theU2Period,
                                                    const Standard_Real theV1Period,
                                                    const Standard_Real theV2Period)
{
  if(theSlin.Length() < 2)
    return;

  const Standard_Real aMaxAngle = M_PI/6; //30 degree
  const Standard_Real aSqToler = theToler3D*theToler3D;
  Standard_Real aU1=0.0, aV1=0.0, aU2=0.0, aV2=0.0;
  gp_Pnt aPmid;
  gp_Vec aVec1, aVec2, aVec3;

  for(Standard_Integer aNumOfLine1 = 1; aNumOfLine1 <= theSlin.Length(); aNumOfLine1++)
  {
    Handle(IntPatch_WLine) aWLine1 (Handle(IntPatch_WLine)::
                                    DownCast(theSlin.Value(aNumOfLine1)));

    if(aWLine1.IsNull())
    {//We must have failed to join not-point-lines
      continue;
    }
    
    const Standard_Integer aNbPntsWL1 = aWLine1->NbPnts();

    for(Standard_Integer aNumOfLine2 = aNumOfLine1 + 1;
        aNumOfLine2 <= theSlin.Length(); aNumOfLine2++)
    {
      Handle(IntPatch_WLine) aWLine2 (Handle(IntPatch_WLine)::
                                    DownCast(theSlin.Value(aNumOfLine2)));

      if(aWLine2.IsNull())
        continue;

      //Enable/Disable of some ckeck. Bit-mask is used for it.
      //E.g. if 1st point of aWLine1 matches with
      //1st point of aWLine2 then we do not need in check
      //1st point of aWLine1 and last point of aWLine2 etc.
      enum
      {
        IntPatchWT_EnAll = 0x00,
        IntPatchWT_DisLastLast = 0x01,
        IntPatchWT_DisLastFirst = 0x02,
        IntPatchWT_DisFirstLast = 0x04,
        IntPatchWT_DisFirstFirst = 0x08
      };

      unsigned int aCheckResult = IntPatchWT_EnAll;

      const Standard_Integer aNbPntsWL2 = aWLine2->NbPnts();

      const IntSurf_PntOn2S& aPntFWL1 = aWLine1->Point(1);
      const IntSurf_PntOn2S& aPntFp1WL1 = aWLine1->Point(2);

      const IntSurf_PntOn2S& aPntLWL1 = aWLine1->Point(aNbPntsWL1);
      const IntSurf_PntOn2S& aPntLm1WL1 = aWLine1->Point(aNbPntsWL1-1);
      
      const IntSurf_PntOn2S& aPntFWL2 = aWLine2->Point(1);
      const IntSurf_PntOn2S& aPntFp1WL2 = aWLine2->Point(2);

      const IntSurf_PntOn2S& aPntLWL2 = aWLine2->Point(aNbPntsWL2);
      const IntSurf_PntOn2S& aPntLm1WL2 = aWLine2->Point(aNbPntsWL2-1);
      
      //if(!(aCheckResult & IntPatchWT_DisFirstFirst))
      {// First/First
        aVec1.SetXYZ(aPntFp1WL1.Value().XYZ() - aPntFWL1.Value().XYZ());
        aVec2.SetXYZ(aPntFWL2.Value().XYZ() - aPntFp1WL2.Value().XYZ());
        aVec3.SetXYZ(aPntFWL1.Value().XYZ() - aPntFWL2.Value().XYZ());

        if(aVec3.SquareMagnitude() > aSqToler)
        {
          if( (aVec1.Angle(aVec2) < aMaxAngle) &&
              (aVec1.Angle(aVec3) < aMaxAngle) &&
              (aVec2.Angle(aVec3) < aMaxAngle))
          {
            aPmid.SetXYZ(0.5*(aPntFWL1.Value().XYZ()+aPntFWL2.Value().XYZ()));
            if(IsIntersectionPoint(aPmid, theS1, theS2, aPntFWL1, theToler3D,
                                   theU1Period, theU2Period, theV1Period, theV2Period,
                                   aU1, aV1, aU2, aV2))
            {
              IntSurf_PntOn2S aPOn2S;
              aPOn2S.SetValue(aPmid, aU1, aV1, aU2, aV2);
              
              Standard_Real aU11 = 0.0, aV11 = 0.0, aU21 = 0.0, aV21 = 0.0,
                            aU12 = 0.0, aV12 = 0.0, aU22 = 0.0, aV22 = 0.0;
              aPntFWL1.Parameters(aU11, aV11, aU21, aV21);
              aPntFWL2.Parameters(aU12, aV12, aU22, aV22);

              if(!IsSeam(aU11, aU12, theU1Period) &&
                 !IsSeam(aV11, aV12, theV1Period) &&
                 !IsSeam(aU21, aU22, theU2Period) &&
                 !IsSeam(aV21, aV22, theV2Period))
              {
                aCheckResult |= (IntPatchWT_DisFirstLast |
                                 IntPatchWT_DisLastFirst);

                if(!aPOn2S.IsSame(aPntFWL1, Precision::Confusion()))
                {
                  ExtendFirst(aWLine1, aPOn2S);
                }
                else
                {
                  aWLine1->Curve()->Value(1, aPOn2S);
                }

                if(!aPOn2S.IsSame(aPntFWL2, Precision::Confusion()))
                {
                  ExtendFirst(aWLine2, aPOn2S);
                }
                else
                {
                  aWLine2->Curve()->Value(1, aPOn2S);
                }
              }
            }
          }
        }//if(aVec3.SquareMagnitude() > aSqToler) cond.
      }//if(!(aCheckResult & 0x08)) cond.

      if(!(aCheckResult & IntPatchWT_DisFirstLast))
      {// First/Last
        aVec1.SetXYZ(aPntFp1WL1.Value().XYZ() - aPntFWL1.Value().XYZ());
        aVec2.SetXYZ(aPntLWL2.Value().XYZ() - aPntLm1WL2.Value().XYZ());
        aVec3.SetXYZ(aPntFWL1.Value().XYZ() - aPntLWL2.Value().XYZ());

        if(aVec3.SquareMagnitude() > aSqToler)
        {
          if((aVec1.Angle(aVec2) < aMaxAngle) &&
             (aVec1.Angle(aVec3) < aMaxAngle) &&
             (aVec2.Angle(aVec3) < aMaxAngle))
          {
            aPmid.SetXYZ(0.5*(aPntFWL1.Value().XYZ()+aPntLWL2.Value().XYZ()));
            if(IsIntersectionPoint(aPmid, theS1, theS2, aPntFWL1, theToler3D,
                                   theU1Period, theU2Period, theV1Period, theV2Period,
                                   aU1, aV1, aU2, aV2))
            {
              IntSurf_PntOn2S aPOn2S;
              aPOn2S.SetValue(aPmid, aU1, aV1, aU2, aV2);

              Standard_Real aU11 = 0.0, aV11 = 0.0, aU21 = 0.0, aV21 = 0.0,
                            aU12 = 0.0, aV12 = 0.0, aU22 = 0.0, aV22 = 0.0;
              aPntFWL1.Parameters(aU11, aV11, aU21, aV21);
              aPntLWL2.Parameters(aU12, aV12, aU22, aV22);

              if(!IsSeam(aU11, aU12, theU1Period) &&
                 !IsSeam(aV11, aV12, theV1Period) &&
                 !IsSeam(aU21, aU22, theU2Period) &&
                 !IsSeam(aV21, aV22, theV2Period))
              {
                aCheckResult |= IntPatchWT_DisLastLast;

                if(!aPOn2S.IsSame(aPntFWL1, Precision::Confusion()))
                {
                  ExtendFirst(aWLine1, aPOn2S);
                }
                else
                {
                  aWLine1->Curve()->Value(1, aPOn2S);
                }

                if(!aPOn2S.IsSame(aPntLWL2, Precision::Confusion()))
                {
                  ExtendLast(aWLine2, aPOn2S);
                }
                else
                {
                  aWLine2->Curve()->Value(aWLine2->NbPnts(), aPOn2S);
                }
              }
            }
          }
        }//if(aVec3.SquareMagnitude() > aSqToler) cond.
      }//if(!(aCheckResult & 0x04)) cond.

      if(!(aCheckResult & IntPatchWT_DisLastFirst))
      {// Last/First
        aVec1.SetXYZ(aPntLWL1.Value().XYZ() - aPntLm1WL1.Value().XYZ());
        aVec2.SetXYZ(aPntFp1WL2.Value().XYZ() - aPntFWL2.Value().XYZ());
        aVec3.SetXYZ(aPntFWL2.Value().XYZ() - aPntLWL1.Value().XYZ());

        if(aVec3.SquareMagnitude() > aSqToler)
        {
          if((aVec1.Angle(aVec2) < aMaxAngle) &&
             (aVec1.Angle(aVec3) < aMaxAngle) &&
             (aVec2.Angle(aVec3) < aMaxAngle))
          {
            aPmid.SetXYZ(0.5*(aPntLWL1.Value().XYZ()+aPntFWL2.Value().XYZ()));
            if(IsIntersectionPoint(aPmid, theS1, theS2, aPntLWL1, theToler3D,
                                   theU1Period, theU2Period, theV1Period, theV2Period,
                                   aU1, aV1, aU2, aV2))
            {
              IntSurf_PntOn2S aPOn2S;
              aPOn2S.SetValue(aPmid, aU1, aV1, aU2, aV2);

              Standard_Real aU11 = 0.0, aV11 = 0.0, aU21 = 0.0, aV21 = 0.0,
                            aU12 = 0.0, aV12 = 0.0, aU22 = 0.0, aV22 = 0.0;
              aPntLWL1.Parameters(aU11, aV11, aU21, aV21);
              aPntFWL2.Parameters(aU12, aV12, aU22, aV22);

              if(!IsSeam(aU11, aU12, theU1Period) &&
                 !IsSeam(aV11, aV12, theV1Period) &&
                 !IsSeam(aU21, aU22, theU2Period) &&
                 !IsSeam(aV21, aV22, theV2Period))
              {
                aCheckResult |= IntPatchWT_DisLastLast;

                if(!aPOn2S.IsSame(aPntLWL1, Precision::Confusion()))
                {
                  ExtendLast(aWLine1, aPOn2S);
                }
                else
                {
                  aWLine1->Curve()->Value(aWLine1->NbPnts(), aPOn2S);
                }

                if(!aPOn2S.IsSame(aPntFWL2, Precision::Confusion()))
                {
                  ExtendFirst(aWLine2, aPOn2S);
                }
                else
                {
                  aWLine2->Curve()->Value(1, aPOn2S);
                }
              }
            }
          }
        }//if(aVec3.SquareMagnitude() > aSqToler) cond.
      }//if(!(aCheckResult & 0x02)) cond.

      if(!(aCheckResult & IntPatchWT_DisLastLast))
      {// Last/Last
        aVec1.SetXYZ(aPntLWL1.Value().XYZ() - aPntLm1WL1.Value().XYZ());
        aVec2.SetXYZ(aPntLm1WL2.Value().XYZ() - aPntLWL2.Value().XYZ());
        aVec3.SetXYZ(aPntLWL2.Value().XYZ() - aPntLWL1.Value().XYZ());

        if(aVec3.SquareMagnitude() > aSqToler)
        {
          if((aVec1.Angle(aVec2) < aMaxAngle) &&
             (aVec1.Angle(aVec3) < aMaxAngle) &&
             (aVec2.Angle(aVec3) < aMaxAngle))
          {
            aPmid.SetXYZ(0.5*(aPntLWL1.Value().XYZ()+aPntLWL2.Value().XYZ()));
            if(IsIntersectionPoint(aPmid, theS1, theS2, aPntLWL1, theToler3D,
                                   theU1Period, theU2Period, theV1Period, theV2Period,
                                   aU1, aV1, aU2, aV2))
            {
              IntSurf_PntOn2S aPOn2S;
              aPOn2S.SetValue(aPmid, aU1, aV1, aU2, aV2);

              Standard_Real aU11 = 0.0, aV11 = 0.0, aU21 = 0.0, aV21 = 0.0,
                            aU12 = 0.0, aV12 = 0.0, aU22 = 0.0, aV22 = 0.0;
              aPntLWL1.Parameters(aU11, aV11, aU21, aV21);
              aPntLWL2.Parameters(aU12, aV12, aU22, aV22);

              if(!IsSeam(aU11, aU12, theU1Period) &&
                 !IsSeam(aV11, aV12, theV1Period) &&
                 !IsSeam(aU21, aU22, theU2Period) &&
                 !IsSeam(aV21, aV22, theV2Period))
              {
                if(!aPOn2S.IsSame(aPntLWL1, Precision::Confusion()))
                {
                  ExtendLast(aWLine1, aPOn2S);
                }
                else
                {
                  aWLine1->Curve()->Value(aWLine1->NbPnts(), aPOn2S);
                }

                if(!aPOn2S.IsSame(aPntLWL2, Precision::Confusion()))
                {
                  ExtendLast(aWLine2, aPOn2S);
                }
                else
                {
                  aWLine2->Curve()->Value(aWLine2->NbPnts(), aPOn2S);
                }
              }
            }
          }
        }//if(aVec3.SquareMagnitude() > aSqToler) cond.
      }//if(!(aCheckResult & 0x01)) cond.
    }
  }
}
