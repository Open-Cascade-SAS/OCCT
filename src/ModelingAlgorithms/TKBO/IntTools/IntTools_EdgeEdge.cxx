// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>
#include <ElCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_OffsetCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_Tools.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRepExtrema_DistShapeShape.hxx>

static void   BndBuildBox(const BRepAdaptor_Curve& theBAC,
                          const double             aT1,
                          const double             aT2,
                          const double             theTol,
                          Bnd_Box&                 theBox);
static double PointBoxDistance(const Bnd_Box& aB, const gp_Pnt& aP);
static int    SplitRangeOnSegments(const double                          aT1,
                                   const double                          aT2,
                                   const double                          theResolution,
                                   const int                             theNbSeg,
                                   NCollection_Sequence<IntTools_Range>& theSegments);
static int    DistPC(const double                   aT1,
                     const occ::handle<Geom_Curve>& theC1,
                     const double                   theCriteria,
                     GeomAPI_ProjectPointOnCurve&   theProjector,
                     double&                        aD,
                     double&                        aT2,
                     const int                      iC = 1);
static int    DistPC(const double                   aT1,
                     const occ::handle<Geom_Curve>& theC1,
                     const double                   theCriteria,
                     GeomAPI_ProjectPointOnCurve&   theProjector,
                     double&                        aD,
                     double&                        aT2,
                     double&                        aDmax,
                     double&                        aT1max,
                     double&                        aT2max,
                     const int                      iC = 1);
static int    FindDistPC(const double                   aT1A,
                         const double                   aT1B,
                         const occ::handle<Geom_Curve>& theC1,
                         const double                   theCriteria,
                         const double                   theEps,
                         GeomAPI_ProjectPointOnCurve&   theProjector,
                         double&                        aDmax,
                         double&                        aT1max,
                         double&                        aT2max,
                         const bool                     bMaxDist = true);
static double ResolutionCoeff(const BRepAdaptor_Curve& theBAC, const IntTools_Range& theRange);
static double Resolution(const occ::handle<Geom_Curve>& theCurve,
                         const GeomAbs_CurveType        theCurveType,
                         const double                   theResCoeff,
                         const double                   theR3D);
static double CurveDeflection(const BRepAdaptor_Curve& theBAC, const IntTools_Range& theRange);
static bool   IsClosed(const occ::handle<Geom_Curve>& theCurve,
                       const double                   aT1,
                       const double                   aT2,
                       const double                   theTol,
                       const double                   theRes);
static int    TypeToInteger(const GeomAbs_CurveType theCType);

//=================================================================================================

void IntTools_EdgeEdge::Prepare()
{
  GeomAbs_CurveType aCT1, aCT2;
  int               iCT1, iCT2;
  //
  myCurve1.Initialize(myEdge1);
  myCurve2.Initialize(myEdge2);
  //
  if (myRange1.First() == 0. && myRange1.Last() == 0.)
  {
    myRange1.SetFirst(myCurve1.FirstParameter());
    myRange1.SetLast(myCurve1.LastParameter());
  }
  //
  if (myRange2.First() == 0. && myRange2.Last() == 0.)
  {
    myRange2.SetFirst(myCurve2.FirstParameter());
    myRange2.SetLast(myCurve2.LastParameter());
  }
  //
  aCT1 = myCurve1.GetType();
  aCT2 = myCurve2.GetType();
  //
  iCT1 = TypeToInteger(aCT1);
  iCT2 = TypeToInteger(aCT2);
  //
  if (iCT1 == iCT2)
  {
    if (iCT1 != 0)
    {
      // compute deflection
      double aC1, aC2;
      //
      aC2 = CurveDeflection(myCurve2, myRange2);
      aC1 = (aC2 > Precision::Confusion()) ? CurveDeflection(myCurve1, myRange1) : 1.;
      //
      if (aC1 < aC2)
      {
        --iCT1;
      }
    }
  }
  //
  if (iCT1 < iCT2)
  {
    TopoDS_Edge tmpE = myEdge1;
    myEdge1          = myEdge2;
    myEdge2          = tmpE;
    //
    BRepAdaptor_Curve tmpC = myCurve1;
    myCurve1               = myCurve2;
    myCurve2               = tmpC;
    //
    IntTools_Range tmpR = myRange1;
    myRange1            = myRange2;
    myRange2            = tmpR;
    //
    mySwap = true;
  }
  //
  // Use a minimum tolerance that accounts for coordinate precision issues in tangent geometries.
  // For mm-scale CAD models, coordinate precision can differ by 2-3e-6 between shapes that
  // are geometrically "at the same position".
  double aTolAdd = std::max(myFuzzyValue / 2., 2.0 * Precision::Approximation());
  myTol1         = myCurve1.Tolerance() + aTolAdd;
  myTol2         = myCurve2.Tolerance() + aTolAdd;
  myTol          = myTol1 + myTol2;
  //
  if (iCT1 != 0 || iCT2 != 0)
  {
    double f, l, aTM;
    //
    myGeom1 = BRep_Tool::Curve(myEdge1, f, l);
    myGeom2 = BRep_Tool::Curve(myEdge2, f, l);
    //
    myResCoeff1 = ResolutionCoeff(myCurve1, myRange1);
    myResCoeff2 = ResolutionCoeff(myCurve2, myRange2);
    //
    myRes1 = Resolution(myCurve1.Curve().Curve(), myCurve1.GetType(), myResCoeff1, myTol1);
    myRes2 = Resolution(myCurve2.Curve().Curve(), myCurve2.GetType(), myResCoeff2, myTol2);
    //
    myPTol1 = 5.e-13;
    aTM     = std::max(fabs(myRange1.First()), fabs(myRange1.Last()));
    if (aTM > 999.)
    {
      myPTol1 = 5.e-16 * aTM;
    }
    //
    myPTol2 = 5.e-13;
    aTM     = std::max(fabs(myRange2.First()), fabs(myRange2.Last()));
    if (aTM > 999.)
    {
      myPTol2 = 5.e-16 * aTM;
    }
  }
}

//=================================================================================================

void IntTools_EdgeEdge::Perform()
{
  // 1. Check data
  CheckData();
  if (myErrorStatus)
  {
    return;
  }
  //
  // 2. Prepare Data
  Prepare();
  //
  // 3.1. Check Line/Line case
  if (myCurve1.GetType() == GeomAbs_Line && myCurve2.GetType() == GeomAbs_Line)
  {
    ComputeLineLine();
    return;
  }
  //
  if (myQuickCoincidenceCheck)
  {
    if (IsCoincident())
    {
      double aT11, aT12, aT21, aT22;
      //
      myRange1.Range(aT11, aT12);
      myRange2.Range(aT21, aT22);
      AddSolution(aT11, aT12, aT21, aT22, TopAbs_EDGE);
      return;
    }
  }
  //
  if ((myCurve1.GetType() <= GeomAbs_Parabola && myCurve2.GetType() <= GeomAbs_Parabola)
      && (myCurve1.GetType() == GeomAbs_Line || myCurve2.GetType() == GeomAbs_Line))
  {
    // Improvement of performance for cases of searching common parts between line
    // and analytical curve. This code allows to define that edges have no
    // common parts more fast, then regular algorithm (FindSolution(...))
    // Check minimal distance between edges
    BRepExtrema_DistShapeShape aMinDist(myEdge1, myEdge2, Extrema_ExtFlag_MIN);
    if (aMinDist.IsDone())
    {
      if (aMinDist.Value() > 1.1 * myTol)
      {
        return;
      }
    }
  }

  NCollection_Sequence<IntTools_Range> aRanges1, aRanges2;
  //
  // 3.2. Find ranges containing solutions
  bool bSplit2;
  FindSolutions(aRanges1, aRanges2, bSplit2);
  //
  // 4. Merge solutions and save common parts
  MergeSolutions(aRanges1, aRanges2, bSplit2);
}

//=================================================================================================

bool IntTools_EdgeEdge::IsCoincident()
{
  int                         i, iCnt, aNbSeg, aNbP2;
  double                      dT, aT1, aCoeff, aTresh, aD;
  double                      aT11, aT12, aT21, aT22;
  GeomAPI_ProjectPointOnCurve aProjPC;
  gp_Pnt                      aP1;
  //
  aTresh = 0.5;
  aNbSeg = 23;
  myRange1.Range(aT11, aT12);
  myRange2.Range(aT21, aT22);
  //
  aProjPC.Init(myGeom2, aT21, aT22);
  //
  dT = (aT12 - aT11) / aNbSeg;
  //
  iCnt = 0;
  for (i = 0; i <= aNbSeg; ++i)
  {
    aT1 = aT11 + i * dT;
    myGeom1->D0(aT1, aP1);
    //
    aProjPC.Perform(aP1);
    aNbP2 = aProjPC.NbPoints();
    if (!aNbP2)
    {
      continue;
    }
    //
    aD = aProjPC.LowerDistance();
    if (aD < myTol)
    {
      ++iCnt;
    }
  }
  //
  aCoeff = (double)iCnt / ((double)aNbSeg + 1);
  return aCoeff > aTresh;
}

//=================================================================================================

void IntTools_EdgeEdge::FindSolutions(NCollection_Sequence<IntTools_Range>& theRanges1,
                                      NCollection_Sequence<IntTools_Range>& theRanges2,
                                      bool&                                 bSplit2)
{
  bool    bIsClosed2;
  double  aT11, aT12, aT21, aT22;
  Bnd_Box aB1, aB2;
  //
  bSplit2 = false;
  myRange1.Range(aT11, aT12);
  myRange2.Range(aT21, aT22);
  //
  bIsClosed2 = IsClosed(myGeom2, aT21, aT22, myTol2, myRes2);
  //
  if (bIsClosed2)
  {
    BndBuildBox(myCurve1, aT11, aT12, myTol1, aB1);
    //
    gp_Pnt aP  = myGeom2->Value(aT21);
    bIsClosed2 = !aB1.IsOut(aP);
  }
  //
  if (!bIsClosed2)
  {
    BndBuildBox(myCurve1, aT11, aT12, myTol1, aB1);
    BndBuildBox(myCurve2, aT21, aT22, myTol2, aB2);
    FindSolutions(myRange1, aB1, myRange2, aB2, theRanges1, theRanges2);
    return;
  }
  //
  if (!CheckCoincidence(aT11, aT12, aT21, aT22, myTol, myRes1))
  {
    theRanges1.Append(myRange1);
    theRanges2.Append(myRange2);
    return;
  }
  //
  int                                  i, j, aNb1, aNb2;
  NCollection_Sequence<IntTools_Range> aSegments1, aSegments2;
  //
  aNb1 = IsClosed(myGeom1, aT11, aT12, myTol1, myRes1) ? 2 : 1;
  aNb2 = 2;
  //
  aNb1 = SplitRangeOnSegments(aT11, aT12, myRes1, aNb1, aSegments1);
  aNb2 = SplitRangeOnSegments(aT21, aT22, myRes2, aNb2, aSegments2);
  //
  for (i = 1; i <= aNb1; ++i)
  {
    const IntTools_Range& aR1 = aSegments1(i);
    BndBuildBox(myCurve1, aR1.First(), aR1.Last(), myTol1, aB1);
    for (j = 1; j <= aNb2; ++j)
    {
      const IntTools_Range& aR2 = aSegments2(j);
      BndBuildBox(myCurve2, aR2.First(), aR2.Last(), myTol2, aB2);
      FindSolutions(aR1, aB1, aR2, aB2, theRanges1, theRanges2);
    }
  }
  //
  bSplit2 = aNb2 > 1;
}

//=================================================================================================

void IntTools_EdgeEdge::FindSolutions(const IntTools_Range&                 theR1,
                                      const Bnd_Box&                        theBox1,
                                      const IntTools_Range&                 theR2,
                                      const Bnd_Box&                        theBox2,
                                      NCollection_Sequence<IntTools_Range>& theRanges1,
                                      NCollection_Sequence<IntTools_Range>& theRanges2)
{
  bool    bOut, bStop, bThin;
  double  aT11, aT12, aT21, aT22;
  double  aTB11, aTB12, aTB21, aTB22;
  double  aSmallStep1, aSmallStep2;
  int     iCom;
  Bnd_Box aB1, aB2;
  //
  theR1.Range(aT11, aT12);
  theR2.Range(aT21, aT22);
  //
  aB1 = theBox1;
  aB2 = theBox2;
  //
  bThin = false;
  bStop = false;
  iCom  = 1;
  //
  do
  {
    aTB11 = aT11;
    aTB12 = aT12;
    aTB21 = aT21;
    aTB22 = aT22;
    //
    // 1. Find parameters of the second edge in the box of first one
    bOut = aB1.IsOut(aB2);
    if (bOut)
    {
      break;
    }
    //
    bThin =
      ((aT12 - aT11) < myRes1) || (aB1.IsXThin(myTol) && aB1.IsYThin(myTol) && aB1.IsZThin(myTol));
    //
    bOut = !FindParameters(myCurve2,
                           aTB21,
                           aTB22,
                           myTol2,
                           myRes2,
                           myPTol2,
                           myResCoeff2,
                           aB1,
                           aT21,
                           aT22);
    if (bOut || bThin)
    {
      break;
    }
    //
    // 2. Build box for second edge and find parameters
    //   of the first one in that box
    BndBuildBox(myCurve2, aT21, aT22, myTol2, aB2);
    bOut = aB1.IsOut(aB2);
    if (bOut)
    {
      break;
    }
    //
    bThin =
      ((aT22 - aT21) < myRes2) || (aB2.IsXThin(myTol) && aB2.IsYThin(myTol) && aB2.IsZThin(myTol));
    //
    bOut = !FindParameters(myCurve1,
                           aTB11,
                           aTB12,
                           myTol1,
                           myRes1,
                           myPTol1,
                           myResCoeff1,
                           aB2,
                           aT11,
                           aT12);
    //
    if (bOut || bThin)
    {
      break;
    }
    //
    // 3. Check if it makes sense to continue
    aSmallStep1 = (aTB12 - aTB11) / 250.;
    aSmallStep2 = (aTB22 - aTB21) / 250.;
    //
    if (aSmallStep1 < myRes1)
    {
      aSmallStep1 = myRes1;
    }
    if (aSmallStep2 < myRes2)
    {
      aSmallStep2 = myRes2;
    }
    //
    if (((aT11 - aTB11) < aSmallStep1) && ((aTB12 - aT12) < aSmallStep1)
        && ((aT21 - aTB21) < aSmallStep2) && ((aTB22 - aT22) < aSmallStep2))
    {
      bStop = true;
    }
    else
      BndBuildBox(myCurve1, aT11, aT12, myTol1, aB1);

  } while (!bStop);
  //
  if (bOut)
  {
    // no intersection;
    return;
  }
  //
  if (!bThin)
  {
    // check curves for coincidence on the ranges
    iCom = CheckCoincidence(aT11, aT12, aT21, aT22, myTol, myRes1);
    if (!iCom)
    {
      bThin = true;
    }
  }
  //
  if (bThin)
  {
    if (iCom != 0)
    {
      // check intermediate points
      bool                        bSol;
      double                      aT1;
      gp_Pnt                      aP1;
      GeomAPI_ProjectPointOnCurve aProjPC;
      //
      aT1 = (aT11 + aT12) * .5;
      myGeom1->D0(aT1, aP1);
      //
      aProjPC.Init(myGeom2, aT21, aT22);
      aProjPC.Perform(aP1);
      //
      if (aProjPC.NbPoints())
      {
        bSol = aProjPC.LowerDistance() <= myTol;
      }
      else
      {
        double aT2;
        gp_Pnt aP2;
        //
        aT2 = (aT21 + aT22) * .5;
        myGeom2->D0(aT2, aP2);
        //
        bSol = aP1.IsEqual(aP2, myTol);
      }
      //
      if (!bSol)
      {
        return;
      }
    }
    // add common part
    IntTools_Range aR1(aT11, aT12), aR2(aT21, aT22);
    //
    theRanges1.Append(aR1);
    theRanges2.Append(aR2);
    return;
  }
  //
  if (!IsIntersection(aT11, aT12, aT21, aT22))
  {
    return;
  }
  //
  // split ranges on segments and repeat
  int                                  i, aNb1;
  NCollection_Sequence<IntTools_Range> aSegments1;
  //
  // Build box for first curve to compare
  // the boxes of the splits with this one
  BndBuildBox(myCurve1, aT11, aT12, myTol1, aB1);
  const double aB1SqExtent = aB1.SquareExtent();
  //
  IntTools_Range aR2(aT21, aT22);
  BndBuildBox(myCurve2, aT21, aT22, myTol2, aB2);
  //
  aNb1 = SplitRangeOnSegments(aT11, aT12, myRes1, 3, aSegments1);
  for (i = 1; i <= aNb1; ++i)
  {
    const IntTools_Range& aR1 = aSegments1(i);
    BndBuildBox(myCurve1, aR1.First(), aR1.Last(), myTol1, aB1);
    if (!aB1.IsOut(aB2) && (aNb1 == 1 || aB1.SquareExtent() < aB1SqExtent))
      FindSolutions(aR1, aB1, aR2, aB2, theRanges1, theRanges2);
  }
}

//=================================================================================================

bool IntTools_EdgeEdge::FindParameters(const BRepAdaptor_Curve& theBAC,
                                       const double             aT1,
                                       const double             aT2,
                                       const double             theTol,
                                       const double             theRes,
                                       const double             thePTol,
                                       const double             theResCoeff,
                                       const Bnd_Box&           theCBox,
                                       double&                  aTB1,
                                       double&                  aTB2)
{
  bool    bRet;
  int     aC, i;
  double  aCf, aDiff, aDt, aT, aTB, aTOut, aTIn;
  double  aDist, aDistP;
  gp_Pnt  aP;
  Bnd_Box aCBx;
  //
  bRet = false;
  aCf  = 0.6180339887498948482045868343656; // =0.5*(1.+sqrt(5.))/2.;
  aCBx = theCBox;
  aCBx.SetGap(aCBx.GetGap() + theTol);
  //
  const occ::handle<Geom_Curve>& aCurve     = theBAC.Curve().Curve();
  const GeomAbs_CurveType        aCurveType = theBAC.GetType();
  double                         aMaxDt     = (aT2 - aT1) * 0.01;
  //
  for (i = 0; i < 2; ++i)
  {
    aTB      = !i ? aT1 : aT2;
    aT       = !i ? aT2 : aTB1;
    aC       = !i ? 1 : -1;
    aDt      = theRes;
    aDistP   = 0.;
    bRet     = false;
    double k = 1;
    // looking for the point on the edge which is in the box;
    while (aC * (aT - aTB) >= 0)
    {
      theBAC.D0(aTB, aP);
      aDist = PointBoxDistance(theCBox, aP);
      if (aDist > theTol)
      {
        if (aDistP > 0.)
        {
          bool toGrow = false;
          if (std::abs(aDistP - aDist) / aDistP < 0.1)
          {
            aDt = Resolution(aCurve, aCurveType, theResCoeff, k * aDist);
            if (aDt < aMaxDt)
            {
              toGrow = true;
              k *= 2;
            }
          }
          if (!toGrow)
          {
            k   = 1;
            aDt = Resolution(aCurve, aCurveType, theResCoeff, aDist);
          }
        }
        aTB += aC * aDt;
      }
      else
      {
        bRet = true;
        break;
      }
      aDistP = aDist;
    }
    //
    if (!bRet)
    {
      if (!i)
      {
        // edge is out of the box;
        return bRet;
      }
      else
      {
        bRet = !bRet;
        aTB  = aTB1;
        aDt  = aT2 - aTB1;
      }
    }
    //
    aT = !i ? aT1 : aT2;
    if (aTB != aT)
    {
      // one point IN, one point OUT; looking for the bounding point;
      aTIn  = aTB;
      aTOut = aTB - aC * aDt;
      aDiff = aTIn - aTOut;
      while (fabs(aDiff) > thePTol)
      {
        aTB = aTOut + aDiff * aCf;
        theBAC.D0(aTB, aP);
        if (aCBx.IsOut(aP))
        {
          aTOut = aTB;
        }
        else
        {
          aTIn = aTB;
        }
        aDiff = aTIn - aTOut;
      }
    }
    if (!i)
    {
      aTB1 = aTB;
    }
    else
    {
      aTB2 = aTB;
    }
  }
  return bRet;
}

//=================================================================================================

void IntTools_EdgeEdge::MergeSolutions(const NCollection_Sequence<IntTools_Range>& theRanges1,
                                       const NCollection_Sequence<IntTools_Range>& theRanges2,
                                       const bool                                  bSplit2)
{
  int aNbCP = theRanges1.Length();
  if (aNbCP == 0)
  {
    return;
  }
  //
  IntTools_Range       aRi1, aRi2, aRj1, aRj2;
  bool                 bCond;
  int                  i, j;
  TopAbs_ShapeEnum     aType;
  double               aT11, aT12, aT21, aT22;
  double               aTi11, aTi12, aTi21, aTi22;
  double               aTj11, aTj12, aTj21, aTj22;
  double               aRes1, aRes2, dTR1, dTR2;
  NCollection_Map<int> aMI;
  //
  aRes1 = Resolution(myCurve1.Curve().Curve(), myCurve1.GetType(), myResCoeff1, myTol);
  aRes2 = Resolution(myCurve2.Curve().Curve(), myCurve2.GetType(), myResCoeff2, myTol);
  //
  myRange1.Range(aT11, aT12);
  myRange2.Range(aT21, aT22);
  dTR1  = 20 * aRes1;
  dTR2  = 20 * aRes2;
  aType = TopAbs_VERTEX;
  //
  for (i = 1; i <= aNbCP;)
  {
    if (aMI.Contains(i))
    {
      ++i;
      continue;
    }
    //
    aRi1 = theRanges1(i);
    aRi2 = theRanges2(i);
    //
    aRi1.Range(aTi11, aTi12);
    aRi2.Range(aTi21, aTi22);
    //
    aMI.Add(i);
    //
    for (j = i + 1; j <= aNbCP; ++j)
    {
      if (aMI.Contains(j))
      {
        continue;
      }
      //
      aRj1 = theRanges1(j);
      aRj2 = theRanges2(j);
      //
      aRj1.Range(aTj11, aTj12);
      aRj2.Range(aTj21, aTj22);
      //
      bCond = (fabs(aTi12 - aTj11) < dTR1) || (aTj11 > aTi11 && aTj11 < aTi12)
              || (aTi11 > aTj11 && aTi11 < aTj12) || (bSplit2 && (fabs(aTj12 - aTi11) < dTR1));
      if (bCond && bSplit2)
      {
        bCond = (fabs((std::max(aTi22, aTj22) - std::min(aTi21, aTj21))
                      - ((aTi22 - aTi21) + (aTj22 - aTj21)))
                 < dTR2)
                || (aTj21 > aTi21 && aTj21 < aTi22) || (aTi21 > aTj21 && aTi21 < aTj22);
      }
      //
      if (bCond)
      {
        aTi11 = std::min(aTi11, aTj11);
        aTi12 = std::max(aTi12, aTj12);
        aTi21 = std::min(aTi21, aTj21);
        aTi22 = std::max(aTi22, aTj22);
        aMI.Add(j);
      }
      else if (!bSplit2)
      {
        i = j;
        break;
      }
    }
    //
    if (((fabs(aT11 - aTi11) < myRes1) && (fabs(aT12 - aTi12) < myRes1))
        || ((fabs(aT21 - aTi21) < myRes2) && (fabs(aT22 - aTi22) < myRes2)))
    {
      aType = TopAbs_EDGE;
      myCommonParts.Clear();
    }
    //
    AddSolution(aTi11, aTi12, aTi21, aTi22, aType);
    if (aType == TopAbs_EDGE)
    {
      break;
    }
    //
    if (bSplit2)
    {
      ++i;
    }
  }
}

//=================================================================================================

void IntTools_EdgeEdge::AddSolution(const double           aT11,
                                    const double           aT12,
                                    const double           aT21,
                                    const double           aT22,
                                    const TopAbs_ShapeEnum theType)
{
  IntTools_CommonPrt aCPart;
  //
  aCPart.SetType(theType);
  if (!mySwap)
  {
    aCPart.SetEdge1(myEdge1);
    aCPart.SetEdge2(myEdge2);
    aCPart.SetRange1(aT11, aT12);
    aCPart.AppendRange2(aT21, aT22);
  }
  else
  {
    aCPart.SetEdge1(myEdge2);
    aCPart.SetEdge2(myEdge1);
    aCPart.SetRange1(aT21, aT22);
    aCPart.AppendRange2(aT11, aT12);
  }
  //
  if (theType == TopAbs_VERTEX)
  {
    double aT1, aT2;
    //
    FindBestSolution(aT11, aT12, aT21, aT22, aT1, aT2);
    //
    if (!mySwap)
    {
      aCPart.SetVertexParameter1(aT1);
      aCPart.SetVertexParameter2(aT2);
    }
    else
    {
      aCPart.SetVertexParameter1(aT2);
      aCPart.SetVertexParameter2(aT1);
    }
  }
  myCommonParts.Append(aCPart);
}

//=================================================================================================

void IntTools_EdgeEdge::FindBestSolution(const double aT11,
                                         const double aT12,
                                         const double aT21,
                                         const double aT22,
                                         double&      aT1,
                                         double&      aT2)
{
  int                                  i, aNbS, iErr;
  double                               aDMin, aD, aRes1, aSolCriteria, aTouchCriteria;
  double                               aT1A, aT1B, aT1Min, aT2Min;
  GeomAPI_ProjectPointOnCurve          aProjPC;
  NCollection_Sequence<IntTools_Range> aRanges;
  //
  aDMin              = Precision::Infinite();
  aSolCriteria       = 5.e-16;
  aTouchCriteria     = 5.e-13;
  bool bTouch        = false;
  bool bTouchConfirm = false;
  //
  aRes1 = Resolution(myCurve1.Curve().Curve(), myCurve1.GetType(), myResCoeff1, myTol);
  aNbS  = 10;
  aNbS  = SplitRangeOnSegments(aT11, aT12, 3 * aRes1, aNbS, aRanges);
  //
  aProjPC.Init(myGeom2, aT21, aT22);
  //
  double aT11Touch = aT11, aT12Touch = aT12;
  double aT21Touch = aT21, aT22Touch = aT22;
  bool   isSolFound = false;
  for (i = 1; i <= aNbS; ++i)
  {
    const IntTools_Range& aR1 = aRanges(i);
    aR1.Range(aT1A, aT1B);
    //
    aD = myTol;
    iErr =
      FindDistPC(aT1A, aT1B, myGeom1, aSolCriteria, myPTol1, aProjPC, aD, aT1Min, aT2Min, false);
    if (iErr != 1)
    {
      if (aD < aDMin)
      {
        aT1        = aT1Min;
        aT2        = aT2Min;
        aDMin      = aD;
        isSolFound = true;
      }
      //
      if (aD < aTouchCriteria)
      {
        if (bTouch)
        {
          aT12Touch     = aT1Min;
          aT22Touch     = aT2Min;
          bTouchConfirm = true;
        }
        else
        {
          aT11Touch = aT1Min;
          aT21Touch = aT2Min;
          bTouch    = true;
        }
      }
    }
  }
  if (!isSolFound || bTouchConfirm)
  {
    aT1  = (aT11Touch + aT12Touch) * 0.5;
    iErr = DistPC(aT1, myGeom1, aSolCriteria, aProjPC, aD, aT2, -1);
    if (iErr == 1)
    {
      aT2 = (aT21Touch + aT22Touch) * 0.5;
    }
  }
}

//=================================================================================================

void IntTools_EdgeEdge::ComputeLineLine()
{
  double aTol = myTol * myTol;

  gp_Lin aL1 = myCurve1.Line();
  gp_Lin aL2 = myCurve2.Line();

  gp_Dir aD1 = aL1.Direction();
  gp_Dir aD2 = aL2.Direction();

  double anAngle    = aD1.Angle(aD2);
  bool   IsCoincide = anAngle < Precision::Angular();
  if (IsCoincide)
  {
    if (aL1.SquareDistance(aL2.Location()) > aTol)
      return;
  }

  double aT11, aT12, aT21, aT22;
  myRange1.Range(aT11, aT12);
  myRange2.Range(aT21, aT22);

  gp_Pnt aP11 = ElCLib::Value(aT11, aL1);
  gp_Pnt aP12 = ElCLib::Value(aT12, aL1);

  if (!IsCoincide)
  {
    gp_Pnt O2(aL2.Location());
    if (!Precision::IsInfinite(aT21) && !Precision::IsInfinite(aT22))
      O2 = ElCLib::Value((aT21 + aT22) / 2., aL2);

    gp_Vec aVec1 = gp_Vec(O2, aP11).Crossed(aD2);
    gp_Vec aVec2 = gp_Vec(O2, aP12).Crossed(aD2);

    double aSqDist1 = aVec1.SquareMagnitude();
    double aSqDist2 = aVec2.SquareMagnitude();

    IsCoincide = (aSqDist1 <= aTol && aSqDist2 <= aTol);

    if (!IsCoincide && aVec1.Dot(aVec2) > 0)
      // the lines do not intersect
      return;
  }

  IntTools_CommonPrt aCommonPrt;
  aCommonPrt.SetEdge1(myEdge1);
  aCommonPrt.SetEdge2(myEdge2);

  if (IsCoincide)
  {
    double t21 = ElCLib::Parameter(aL2, aP11);
    double t22 = ElCLib::Parameter(aL2, aP12);

    if ((t21 > aT22 && t22 > aT22) || (t21 < aT21 && t22 < aT21))
      // projections are out of range
      return;

    if (t21 > t22)
      std::swap(t21, t22);

    if (t21 >= aT21)
    {
      if (t22 <= aT22)
      {
        aCommonPrt.SetRange1(aT11, aT12);
        aCommonPrt.SetAllNullFlag(true);
        aCommonPrt.AppendRange2(t21, t22);
      }
      else
      {
        aCommonPrt.SetRange1(aT11, aT12 - (t22 - aT22));
        aCommonPrt.AppendRange2(t21, aT22);
      }
    }
    else
    {
      aCommonPrt.SetRange1(aT11 + (aT21 - t21), aT12);
      aCommonPrt.AppendRange2(aT21, t22);
    }
    aCommonPrt.SetType(TopAbs_EDGE);
    myCommonParts.Append(aCommonPrt);
    return;
  }

  gp_Vec O1O2(aL1.Location(), aL2.Location());
  gp_XYZ aCross  = aD1.XYZ().Crossed(aD2.XYZ());
  double aDistLL = O1O2.Dot(gp_Vec(aCross.Normalized()));
  if (std::abs(aDistLL) > myTol)
    return;

  {
    // Fast check that no intersection needs to be added
    for (TopoDS_Iterator it1(myEdge1); it1.More(); it1.Next())
    {
      for (TopoDS_Iterator it2(myEdge2); it2.More(); it2.Next())
      {
        if (it1.Value().IsSame(it2.Value()))
          return;
      }
    }
  }

  double aSqSin = aCross.SquareModulus();
  double aT2    = (aD1.XYZ() * (O1O2.Dot(aD1)) - (O1O2.XYZ())).Dot(aD2.XYZ());
  aT2 /= aSqSin;

  if (aT2 < aT21 || aT2 > aT22)
    // out of range
    return;

  gp_Pnt aP2 = ElCLib::Value(aT2, aL2);
  double aT1 = gp_Vec(aL1.Location(), aP2).Dot(aD1);

  if (aT1 < aT11 || aT1 > aT12)
    // out of range
    return;

  gp_Pnt aP1   = ElCLib::Value(aT1, aL1);
  double aDist = aP1.SquareDistance(aP2);

  if (aDist > aTol)
    // no intersection
    return;

  // compute correct range on the edges
  double aDt1 = IntTools_Tools::ComputeIntRange(myTol1, myTol2, anAngle);
  double aDt2 = IntTools_Tools::ComputeIntRange(myTol2, myTol1, anAngle);

  aCommonPrt.SetRange1(aT1 - aDt1, aT1 + aDt1);
  aCommonPrt.AppendRange2(aT2 - aDt2, aT2 + aDt2);
  aCommonPrt.SetType(TopAbs_VERTEX);
  aCommonPrt.SetVertexParameter1(aT1);
  aCommonPrt.SetVertexParameter2(aT2);
  myCommonParts.Append(aCommonPrt);
}

//=================================================================================================

bool IntTools_EdgeEdge::IsIntersection(const double aT11,
                                       const double aT12,
                                       const double aT21,
                                       const double aT22)
{
  bool   bRet;
  gp_Pnt aP11, aP12, aP21, aP22;
  gp_Vec aV11, aV12, aV21, aV22;
  double aD11_21, aD11_22, aD12_21, aD12_22, aCriteria, aCoef;
  bool   bSmall_11_21, bSmall_11_22, bSmall_12_21, bSmall_12_22;
  //
  bRet  = true;
  aCoef = 1.e+5;
  if (((aT12 - aT11) > aCoef * myRes1) && ((aT22 - aT21) > aCoef * myRes2))
  {
    aCoef = 5000;
  }
  else
  {
    double aTRMin = std::min((aT12 - aT11) / myRes1, (aT22 - aT21) / myRes2);
    aCoef         = aTRMin / 100.;
    if (aCoef < 1.)
    {
      aCoef = 1.;
    }
  }
  aCriteria = aCoef * myTol;
  aCriteria *= aCriteria;
  //
  myGeom1->D1(aT11, aP11, aV11);
  myGeom1->D1(aT12, aP12, aV12);
  myGeom2->D1(aT21, aP21, aV21);
  myGeom2->D1(aT22, aP22, aV22);
  //
  aD11_21 = aP11.SquareDistance(aP21);
  aD11_22 = aP11.SquareDistance(aP22);
  aD12_21 = aP12.SquareDistance(aP21);
  aD12_22 = aP12.SquareDistance(aP22);
  //
  bSmall_11_21 = aD11_21 < aCriteria;
  bSmall_11_22 = aD11_22 < aCriteria;
  bSmall_12_21 = aD12_21 < aCriteria;
  bSmall_12_22 = aD12_22 < aCriteria;
  //
  if ((bSmall_11_21 && bSmall_12_22) || (bSmall_11_22 && bSmall_12_21))
  {
    if (aCoef == 1.)
    {
      return bRet;
    }
    //
    double anAngleCriteria;
    double anAngle1 = 0.0, anAngle2 = 0.0;
    //
    anAngleCriteria = 5.e-3;
    if (aV11.SquareMagnitude() > Precision::SquareConfusion()
        && aV12.SquareMagnitude() > Precision::SquareConfusion()
        && aV21.SquareMagnitude() > Precision::SquareConfusion()
        && aV22.SquareMagnitude() > Precision::SquareConfusion())
    {
      if (bSmall_11_21 && bSmall_12_22)
      {
        anAngle1 = aV11.Angle(aV21);
        anAngle2 = aV12.Angle(aV22);
      }
      else
      {
        anAngle1 = aV11.Angle(aV22);
        anAngle2 = aV12.Angle(aV21);
      }
    }
    //
    if (((anAngle1 < anAngleCriteria) || ((M_PI - anAngle1) < anAngleCriteria))
        || ((anAngle2 < anAngleCriteria) || ((M_PI - anAngle2) < anAngleCriteria)))
    {
      GeomAPI_ProjectPointOnCurve aProjPC;
      int                         iErr;
      double                      aD, aT1Min, aT2Min;
      //
      aD = Precision::Infinite();
      aProjPC.Init(myGeom2, aT21, aT22);
      iErr = FindDistPC(aT11, aT12, myGeom1, myTol, myRes1, aProjPC, aD, aT1Min, aT2Min, false);
      bRet = (iErr == 2);
    }
  }
  return bRet;
}

//=================================================================================================

int IntTools_EdgeEdge::CheckCoincidence(const double aT11,
                                        const double aT12,
                                        const double aT21,
                                        const double aT22,
                                        const double theCriteria,
                                        const double theCurveRes1)
{
  int                                  iErr, aNb, aNb1, i;
  double                               aT1A, aT1B, aT1max, aT2max, aDmax;
  GeomAPI_ProjectPointOnCurve          aProjPC;
  NCollection_Sequence<IntTools_Range> aRanges;
  //
  iErr  = 0;
  aDmax = -1.;
  aProjPC.Init(myGeom2, aT21, aT22);
  //
  // 1. Express evaluation
  aNb  = 10; // Number of intervals on the curve #1
  aNb1 = SplitRangeOnSegments(aT11, aT12, theCurveRes1, aNb, aRanges);
  for (i = 1; i < aNb1; ++i)
  {
    const IntTools_Range& aR1 = aRanges(i);
    aR1.Range(aT1A, aT1B);
    //
    iErr = DistPC(aT1B, myGeom1, theCriteria, aProjPC, aDmax, aT2max);
    if (iErr)
    {
      return iErr;
    }
  }
  //
  // if the ranges in aRanges are less than theCurveRes1,
  // there is no need to do step 2 (deep evaluation)
  if (aNb1 < aNb)
  {
    return iErr;
  }
  //
  // 2. Deep evaluation
  for (i = 2; i < aNb1; ++i)
  {
    const IntTools_Range& aR1 = aRanges(i);
    aR1.Range(aT1A, aT1B);
    //
    iErr =
      FindDistPC(aT1A, aT1B, myGeom1, theCriteria, theCurveRes1, aProjPC, aDmax, aT1max, aT2max);
    if (iErr)
    {
      return iErr;
    }
  }
  // Possible values:
  // iErr == 0 - the patches are coincided
  // iErr == 1 - a point from aC1 can not be projected on aC2
  // iErr == 2 - the distance is too big
  return iErr;
}

//=================================================================================================

int FindDistPC(const double                   aT1A,
               const double                   aT1B,
               const occ::handle<Geom_Curve>& theC1,
               const double                   theCriteria,
               const double                   theEps,
               GeomAPI_ProjectPointOnCurve&   theProjPC,
               double&                        aDmax,
               double&                        aT1max,
               double&                        aT2max,
               const bool                     bMaxDist)
{
  int    iErr, iC;
  double aGS, aXP, aA, aB, aXL, aYP, aYL, aT2P, aT2L;
  //
  iC     = bMaxDist ? 1 : -1;
  iErr   = 0;
  aT1max = aT2max = 0.; // silence GCC warning
  //
  aGS = 0.6180339887498948482045868343656; // =0.5*(1.+sqrt(5.))-1.;
  aA  = aT1A;
  aB  = aT1B;
  //
  // check bounds
  iErr = DistPC(aA, theC1, theCriteria, theProjPC, aYP, aT2P, aDmax, aT1max, aT2max, iC);
  if (iErr == 2)
  {
    return iErr;
  }
  //
  iErr = DistPC(aB, theC1, theCriteria, theProjPC, aYL, aT2L, aDmax, aT1max, aT2max, iC);
  if (iErr == 2)
  {
    return iErr;
  }
  //
  aXP = aA + (aB - aA) * aGS;
  aXL = aB - (aB - aA) * aGS;
  //
  iErr = DistPC(aXP, theC1, theCriteria, theProjPC, aYP, aT2P, aDmax, aT1max, aT2max, iC);
  if (iErr)
  {
    return iErr;
  }
  //
  iErr = DistPC(aXL, theC1, theCriteria, theProjPC, aYL, aT2L, aDmax, aT1max, aT2max, iC);
  if (iErr)
  {
    return iErr;
  }
  //
  double anEps = std::max(theEps, Epsilon(std::max(std::abs(aA), std::abs(aB))) * 10.);
  for (;;)
  {
    if (iC * (aYP - aYL) > 0)
    {
      aA   = aXL;
      aXL  = aXP;
      aYL  = aYP;
      aXP  = aA + (aB - aA) * aGS;
      iErr = DistPC(aXP, theC1, theCriteria, theProjPC, aYP, aT2P, aDmax, aT1max, aT2max, iC);
    }
    else
    {
      aB   = aXP;
      aXP  = aXL;
      aYP  = aYL;
      aXL  = aB - (aB - aA) * aGS;
      iErr = DistPC(aXL, theC1, theCriteria, theProjPC, aYL, aT2L, aDmax, aT1max, aT2max, iC);
    }
    //
    if (iErr)
    {
      if ((iErr == 2) && !bMaxDist)
      {
        aXP = (aA + aB) * 0.5;
        DistPC(aXP, theC1, theCriteria, theProjPC, aYP, aT2P, aDmax, aT1max, aT2max, iC);
      }
      return iErr;
    }
    //
    if ((aB - aA) < anEps)
    {
      break;
    }
  } // for (;;) {
  //
  return iErr;
}

//=================================================================================================

int DistPC(const double                   aT1,
           const occ::handle<Geom_Curve>& theC1,
           const double                   theCriteria,
           GeomAPI_ProjectPointOnCurve&   theProjPC,
           double&                        aD,
           double&                        aT2,
           double&                        aDmax,
           double&                        aT1max,
           double&                        aT2max,
           const int                      iC)
{
  int iErr;
  //
  iErr = DistPC(aT1, theC1, theCriteria, theProjPC, aD, aT2, iC);
  if (iErr == 1)
  {
    return iErr;
  }
  //
  if (iC * (aD - aDmax) > 0)
  {
    aDmax  = aD;
    aT1max = aT1;
    aT2max = aT2;
  }
  //
  return iErr;
}

//=================================================================================================

int DistPC(const double                   aT1,
           const occ::handle<Geom_Curve>& theC1,
           const double                   theCriteria,
           GeomAPI_ProjectPointOnCurve&   theProjPC,
           double&                        aD,
           double&                        aT2,
           const int                      iC)
{
  int    iErr, aNbP2;
  gp_Pnt aP1;
  //
  iErr = 0;
  theC1->D0(aT1, aP1);
  //
  theProjPC.Perform(aP1);
  aNbP2 = theProjPC.NbPoints();
  if (!aNbP2)
  {
    iErr = 1; // the point from aC1 can not be projected on aC2
    return iErr;
  }
  //
  aD  = theProjPC.LowerDistance();
  aT2 = theProjPC.LowerDistanceParameter();
  if (iC * (aD - theCriteria) > 0)
  {
    iErr = 2; // the distance is too big or small
  }
  //
  return iErr;
}

//=================================================================================================

int SplitRangeOnSegments(const double                          aT1,
                         const double                          aT2,
                         const double                          theResolution,
                         const int                             theNbSeg,
                         NCollection_Sequence<IntTools_Range>& theSegments)
{
  double aDiff = aT2 - aT1;
  if (aDiff < theResolution || theNbSeg == 1)
  {
    theSegments.Append(IntTools_Range(aT1, aT2));
    return 1;
  }
  //
  double aDt, aT1x, aT2x, aSeg;
  int    aNbSegments, i;
  //
  aNbSegments = theNbSeg;
  aDt         = aDiff / aNbSegments;
  if (aDt < theResolution)
  {
    aSeg        = aDiff / theResolution;
    aNbSegments = int(aSeg) + 1;
    aDt         = aDiff / aNbSegments;
  }
  //
  aT1x = aT1;
  for (i = 1; i < aNbSegments; ++i)
  {
    aT2x = aT1x + aDt;
    //
    IntTools_Range aR(aT1x, aT2x);
    theSegments.Append(aR);
    //
    aT1x = aT2x;
  }
  //
  IntTools_Range aR(aT1x, aT2);
  theSegments.Append(aR);
  //
  return aNbSegments;
}

//=================================================================================================

void BndBuildBox(const BRepAdaptor_Curve& theBAC,
                 const double             aT1,
                 const double             aT2,
                 const double             theTol,
                 Bnd_Box&                 theBox)
{
  Bnd_Box aB;
  BndLib_Add3dCurve::Add(theBAC, aT1, aT2, theTol, aB);
  theBox = aB;
}

//=================================================================================================

double PointBoxDistance(const Bnd_Box& aB, const gp_Pnt& aP)
{
  double aPCoord[3];
  double aBMinCoord[3], aBMaxCoord[3];
  double aDist, aR1, aR2;
  int    i;
  //
  aP.Coord(aPCoord[0], aPCoord[1], aPCoord[2]);
  aB.Get(aBMinCoord[0], aBMinCoord[1], aBMinCoord[2], aBMaxCoord[0], aBMaxCoord[1], aBMaxCoord[2]);
  //
  aDist = 0.;
  for (i = 0; i < 3; ++i)
  {
    aR1 = aBMinCoord[i] - aPCoord[i];
    if (aR1 > 0.)
    {
      aDist += aR1 * aR1;
      continue;
    }
    //
    aR2 = aPCoord[i] - aBMaxCoord[i];
    if (aR2 > 0.)
    {
      aDist += aR2 * aR2;
    }
  }
  //
  aDist = std::sqrt(aDist);
  return aDist;
}

//=================================================================================================

int TypeToInteger(const GeomAbs_CurveType theCType)
{
  int iRet;
  //
  switch (theCType)
  {
    case GeomAbs_Line:
      iRet = 0;
      break;
    case GeomAbs_Hyperbola:
    case GeomAbs_Parabola:
      iRet = 1;
      break;
    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
      iRet = 2;
      break;
    case GeomAbs_BezierCurve:
    case GeomAbs_BSplineCurve:
      iRet = 3;
      break;
    default:
      iRet = 4;
      break;
  }
  return iRet;
}

//=================================================================================================

double ResolutionCoeff(const BRepAdaptor_Curve& theBAC, const IntTools_Range& theRange)
{
  double aResCoeff = 0.;
  //
  const occ::handle<Geom_Curve>& aCurve     = theBAC.Curve().Curve();
  const GeomAbs_CurveType        aCurveType = theBAC.GetType();
  //
  switch (aCurveType)
  {
    case GeomAbs_Circle:
      aResCoeff = 1. / (2 * occ::down_cast<Geom_Circle>(aCurve)->Circ().Radius());
      break;
    case GeomAbs_Ellipse:
      aResCoeff = 1. / occ::down_cast<Geom_Ellipse>(aCurve)->MajorRadius();
      break;
    case GeomAbs_OffsetCurve: {
      const occ::handle<Geom_OffsetCurve>& anOffsetCurve = occ::down_cast<Geom_OffsetCurve>(aCurve);
      const occ::handle<Geom_Curve>&       aBasisCurve   = anOffsetCurve->BasisCurve();
      GeomAdaptor_Curve                    aGBasisCurve(aBasisCurve);
      const GeomAbs_CurveType              aBCType = aGBasisCurve.GetType();
      if (aBCType == GeomAbs_Line)
      {
        break;
      }
      else if (aBCType == GeomAbs_Circle)
      {
        aResCoeff = 1. / (2 * (anOffsetCurve->Offset() + aGBasisCurve.Circle().Radius()));
        break;
      }
      else if (aBCType == GeomAbs_Ellipse)
      {
        aResCoeff = 1. / (anOffsetCurve->Offset() + aGBasisCurve.Ellipse().MajorRadius());
        break;
      }
    }
      [[fallthrough]];
    case GeomAbs_Hyperbola:
    case GeomAbs_Parabola:
    case GeomAbs_OtherCurve: {
      double k, kMin, aDist, aDt, aT1, aT2, aT;
      int    aNbP, i;
      gp_Pnt aP1, aP2;
      //
      aNbP = 30;
      theRange.Range(aT1, aT2);
      aDt  = (aT2 - aT1) / aNbP;
      aT   = aT1;
      kMin = 10.;
      //
      theBAC.D0(aT1, aP1);
      for (i = 1; i <= aNbP; ++i)
      {
        aT += aDt;
        theBAC.D0(aT, aP2);
        aDist = aP1.Distance(aP2);
        k     = aDt / aDist;
        if (k < kMin)
        {
          kMin = k;
        }
        aP1 = aP2;
      }
      //
      aResCoeff = kMin;
      break;
    }
    default:
      break;
  }
  //
  return aResCoeff;
}

//=================================================================================================

double Resolution(const occ::handle<Geom_Curve>& theCurve,
                  const GeomAbs_CurveType        theCurveType,
                  const double                   theResCoeff,
                  const double                   theR3D)
{
  double aRes;
  //
  switch (theCurveType)
  {
    case GeomAbs_Line:
      aRes = theR3D;
      break;
    case GeomAbs_Circle: {
      double aDt = theResCoeff * theR3D;
      aRes       = (aDt <= 1.) ? 2 * std::asin(aDt) : 2 * M_PI;
      break;
    }
    case GeomAbs_BezierCurve:
      occ::down_cast<Geom_BezierCurve>(theCurve)->Resolution(theR3D, aRes);
      break;
    case GeomAbs_BSplineCurve:
      occ::down_cast<Geom_BSplineCurve>(theCurve)->Resolution(theR3D, aRes);
      break;
    case GeomAbs_OffsetCurve: {
      const occ::handle<Geom_Curve>& aBasisCurve =
        occ::down_cast<Geom_OffsetCurve>(theCurve)->BasisCurve();
      const GeomAbs_CurveType aBCType = GeomAdaptor_Curve(aBasisCurve).GetType();
      if (aBCType == GeomAbs_Line)
      {
        aRes = theR3D;
        break;
      }
      else if (aBCType == GeomAbs_Circle)
      {
        double aDt = theResCoeff * theR3D;
        aRes       = (aDt <= 1.) ? 2 * std::asin(aDt) : 2 * M_PI;
        break;
      }
    }
      [[fallthrough]];
    default:
      aRes = theResCoeff * theR3D;
      break;
  }
  //
  return aRes;
}

//=================================================================================================

double CurveDeflection(const BRepAdaptor_Curve& theBAC, const IntTools_Range& theRange)
{
  double aDt, aT, aT1, aT2, aDefl;
  int    i, aNbP;
  gp_Vec aV1, aV2;
  gp_Pnt aP;
  //
  aDefl = 0;
  aNbP  = 10;
  theRange.Range(aT1, aT2);
  aDt = (aT2 - aT1) / aNbP;
  aT  = aT1;
  //
  theBAC.D1(aT1, aP, aV1);
  for (i = 1; i <= aNbP; ++i)
  {
    aT += aDt;
    theBAC.D1(aT, aP, aV2);
    if (aV1.Magnitude() > gp::Resolution() && aV2.Magnitude() > gp::Resolution())
    {
      gp_Dir aD1(aV1), aD2(aV2);
      aDefl += aD1.Angle(aD2);
    }
    aV1 = aV2;
  }
  //
  return aDefl;
}

//=================================================================================================

bool IsClosed(const occ::handle<Geom_Curve>& theCurve,
              const double                   aT1,
              const double                   aT2,
              const double                   theTol,
              const double                   theRes)
{
  if (std::abs(aT1 - aT2) < theRes)
  {
    return false;
  }

  gp_Pnt aP1, aP2;
  theCurve->D0(aT1, aP1);
  theCurve->D0(aT2, aP2);
  //
  double aD = aP1.Distance(aP2);
  return aD < theTol;
}
