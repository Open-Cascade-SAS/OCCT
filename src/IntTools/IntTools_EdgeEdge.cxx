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
#include <BOPCol_MapOfInteger.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <ElCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_OffsetCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_Tools.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>

static 
  void BndBuildBox(const BRepAdaptor_Curve& theBAC,
                   const Standard_Real aT1,
                   const Standard_Real aT2,
                   const Standard_Real theTol,
                   Bnd_Box& theBox);
static
  Standard_Real PointBoxDistance(const Bnd_Box& aB,
                                 const gp_Pnt& aP);
static 
  Standard_Integer SplitRangeOnSegments(const Standard_Real aT1, 
                                        const Standard_Real aT2,
                                        const Standard_Real theResolution,
                                        const Standard_Integer theNbSeg,
                                        IntTools_SequenceOfRanges& theSegments);
static
 Standard_Integer DistPC(const Standard_Real aT1, 
                         const Handle(Geom_Curve)& theC1,
                         const Standard_Real theCriteria, 
                         GeomAPI_ProjectPointOnCurve& theProjector,
                         Standard_Real& aD, 
                         Standard_Real& aT2,
                         const Standard_Integer iC = 1);
static
 Standard_Integer DistPC(const Standard_Real aT1, 
                         const Handle(Geom_Curve)& theC1,
                         const Standard_Real theCriteria,
                         GeomAPI_ProjectPointOnCurve& theProjector, 
                         Standard_Real& aD, 
                         Standard_Real& aT2,
                         Standard_Real& aDmax,
                         Standard_Real& aT1max,
                         Standard_Real& aT2max,
                         const Standard_Integer iC = 1);
static
  Standard_Integer FindDistPC(const Standard_Real aT1A, 
                              const Standard_Real aT1B,
                              const Handle(Geom_Curve)& theC1,
                              const Standard_Real theCriteria,
                              const Standard_Real theEps,
                              GeomAPI_ProjectPointOnCurve& theProjector,
                              Standard_Real& aDmax, 
                              Standard_Real& aT1max,
                              Standard_Real& aT2max,
                              const Standard_Boolean bMaxDist = Standard_True);
static
  Standard_Real ResolutionCoeff(const BRepAdaptor_Curve& theBAC,
                                const IntTools_Range& theRange);
static
  Standard_Real Resolution(const Handle(Geom_Curve)& theCurve,
                           const GeomAbs_CurveType theCurveType,
                           const Standard_Real theResCoeff,
                           const Standard_Real theR3D);
static
  Standard_Real CurveDeflection(const BRepAdaptor_Curve& theBAC,
                                const IntTools_Range& theRange);
static 
  Standard_Boolean IsClosed(const Handle(Geom_Curve)& theCurve,
                            const Standard_Real aT1,
                            const Standard_Real aT2,
                            const Standard_Real theTol,
                            const Standard_Real theRes);
static 
  Standard_Integer TypeToInteger(const GeomAbs_CurveType theCType);

//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================
void IntTools_EdgeEdge::Prepare()
{
  GeomAbs_CurveType aCT1, aCT2;
  Standard_Integer iCT1, iCT2;
  //
  myCurve1.Initialize(myEdge1);
  myCurve2.Initialize(myEdge2);
  //
  if (myRange1.First() == 0. && myRange1.Last() == 0.) {
    myRange1.SetFirst(myCurve1.FirstParameter());
    myRange1.SetLast (myCurve1.LastParameter());
  }
  //
  if (myRange2.First() == 0. && myRange2.Last() == 0.) {
    myRange2.SetFirst(myCurve2.FirstParameter());
    myRange2.SetLast (myCurve2.LastParameter());
  }
  //
  aCT1 = myCurve1.GetType();
  aCT2 = myCurve2.GetType();
  //
  iCT1 = TypeToInteger(aCT1);
  iCT2 = TypeToInteger(aCT2);
  //
  if (iCT1 == iCT2) {
    if (iCT1 != 0) {
      //compute deflection
      Standard_Real aC1, aC2;
      //
      aC2 = CurveDeflection(myCurve2, myRange2);
      aC1 = (aC2 > Precision::Confusion()) ? 
        CurveDeflection(myCurve1, myRange1) : 1.;
      //
      if (aC1 < aC2) {
        --iCT1;
      }
    }
  }
  //
  if (iCT1 < iCT2) {
    TopoDS_Edge tmpE = myEdge1;
    myEdge1 = myEdge2;
    myEdge2 = tmpE;
    //
    BRepAdaptor_Curve tmpC = myCurve1;
    myCurve1 = myCurve2;
    myCurve2 = tmpC;
    //
    IntTools_Range tmpR = myRange1;
    myRange1 = myRange2;
    myRange2 = tmpR;
    //
    mySwap = Standard_True;
  }
  //
  Standard_Real aTolAdd = myFuzzyValue / 2.;
  myTol1 = myCurve1.Tolerance() + aTolAdd;
  myTol2 = myCurve2.Tolerance() + aTolAdd;
  myTol = myTol1 + myTol2;
  //
  if (iCT1 != 0 || iCT2 != 0) {
    Standard_Real f, l, aTM;
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
    aTM = Max(fabs(myRange1.First()), fabs(myRange1.Last()));
    if (aTM > 999.) {
      myPTol1 = 5.e-16 * aTM;
    }
    //
    myPTol2 = 5.e-13;
    aTM = Max(fabs(myRange2.First()), fabs(myRange2.Last()));
    if (aTM > 999.) {
      myPTol2 = 5.e-16 * aTM;
    }
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntTools_EdgeEdge::Perform()
{
  //1. Check data
  CheckData();
  if (myErrorStatus) {
    return;
  }
  //
  //2. Prepare Data
  Prepare();
  //
  //3.1. Check Line/Line case
  if (myCurve1.GetType() == GeomAbs_Line &&
      myCurve2.GetType() == GeomAbs_Line) {
    ComputeLineLine();
    return;
  }
  //
  if (myQuickCoincidenceCheck) {
    if (IsCoincident()) {
      Standard_Real aT11, aT12, aT21, aT22;
      //
      myRange1.Range(aT11, aT12);
      myRange2.Range(aT21, aT22);
      AddSolution(aT11, aT12, aT21, aT22, TopAbs_EDGE);
      return;
    }
  }
  //
  IntTools_SequenceOfRanges aRanges1, aRanges2;
  //
  //3.2. Find ranges containig solutions
  Standard_Boolean bSplit2;
  FindSolutions(aRanges1, aRanges2, bSplit2);
  //
  //4. Merge solutions and save common parts
  MergeSolutions(aRanges1, aRanges2, bSplit2);
}

//=======================================================================
//function :  IsCoincident
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeEdge::IsCoincident() 
{
  Standard_Integer i, iCnt, aNbSeg, aNbP2;
  Standard_Real dT, aT1, aCoeff, aTresh, aD;
  Standard_Real aT11, aT12, aT21, aT22;
  GeomAPI_ProjectPointOnCurve aProjPC;
  gp_Pnt aP1;
  //
  aTresh=0.5;
  aNbSeg=23;
  myRange1.Range(aT11, aT12);
  myRange2.Range(aT21, aT22);
  //
  aProjPC.Init(myGeom2, aT21, aT22);
  //
  dT=(aT12-aT11)/aNbSeg;
  //
  iCnt=0;
  for(i=0; i <= aNbSeg; ++i) {
    aT1 = aT11+i*dT;
    myGeom1->D0(aT1, aP1);
    //
    aProjPC.Perform(aP1);
    aNbP2=aProjPC.NbPoints();
    if (!aNbP2) {
      continue;
    }
    //
    aD=aProjPC.LowerDistance();
    if(aD < myTol) {
      ++iCnt; 
    }
  }
  //
  aCoeff=(Standard_Real)iCnt/((Standard_Real)aNbSeg+1);
  return aCoeff > aTresh;
}
//=======================================================================
//function : FindSolutions
//purpose  : 
//=======================================================================
void IntTools_EdgeEdge::FindSolutions(IntTools_SequenceOfRanges& theRanges1,
                                      IntTools_SequenceOfRanges& theRanges2,
                                      Standard_Boolean& bSplit2)
{
  Standard_Boolean bIsClosed2;
  Standard_Real aT11, aT12, aT21, aT22;
  Bnd_Box aB2;
  //
  bSplit2 = Standard_False;
  myRange1.Range(aT11, aT12);
  myRange2.Range(aT21, aT22);
  //
  bIsClosed2 = IsClosed(myGeom2, aT21, aT22, myTol2, myRes2);
  //
  if (bIsClosed2) {
    Bnd_Box aB1;
    BndBuildBox(myCurve1, aT11, aT12, myTol1, aB1);
    //
    gp_Pnt aP = myGeom2->Value(aT21);
    bIsClosed2 = !aB1.IsOut(aP);
  }
  //
  if (!bIsClosed2) {
    BndBuildBox(myCurve2, aT21, aT22, myTol2, aB2);
    FindSolutions(myRange1, myRange2, aB2, theRanges1, theRanges2);
    return;
  }
  //
  if (!CheckCoincidence(aT11, aT12, aT21, aT22, myTol, myRes1)) {
    theRanges1.Append(myRange1);
    theRanges2.Append(myRange2);
    return;
  }
  //
  Standard_Integer i, j, aNb1, aNb2;
  IntTools_SequenceOfRanges aSegments1, aSegments2;
  //
  aNb1 = IsClosed(myGeom1, aT11, aT12, myTol1, myRes1) ? 2 : 1;
  aNb2 = 2;
  //
  aNb1 = SplitRangeOnSegments(aT11, aT12, myRes1, aNb1, aSegments1);
  aNb2 = SplitRangeOnSegments(aT21, aT22, myRes2, aNb2, aSegments2);
  //
  for (i = 1; i <= aNb1; ++i) {
    const IntTools_Range& aR1 = aSegments1(i);
    for (j = 1; j <= aNb2; ++j) {
      const IntTools_Range& aR2 = aSegments2(j);
      BndBuildBox(myCurve2, aR2.First(), aR2.Last(), myTol2, aB2);
      FindSolutions(aR1, aR2, aB2, theRanges1, theRanges2);
    }
  }
  //
  bSplit2 = aNb2 > 1;
}

//=======================================================================
//function : FindSolutions
//purpose  : 
//=======================================================================
void IntTools_EdgeEdge::FindSolutions(const IntTools_Range& theR1,
                                      const IntTools_Range& theR2,
                                      const Bnd_Box& theBox2,
                                      IntTools_SequenceOfRanges& theRanges1,
                                      IntTools_SequenceOfRanges& theRanges2)
{
  Standard_Boolean bOut, bStop, bThin;
  Standard_Real aT11, aT12, aT21, aT22;
  Standard_Real aTB11, aTB12, aTB21, aTB22;
  Standard_Real aSmallStep1, aSmallStep2;
  Standard_Integer iCom;
  Bnd_Box aB1, aB2;
  //
  theR1.Range(aT11, aT12);
  theR2.Range(aT21, aT22);
  //
  aB2 = theBox2;
  //
  bThin = Standard_False;
  bStop = Standard_False;
  iCom  = 1;
  //
  do {
    aTB11 = aT11;
    aTB12 = aT12;
    aTB21 = aT21;
    aTB22 = aT22;
    //
    //1. Build box for first edge and find parameters 
    //   of the second one in that box
    BndBuildBox(myCurve1, aT11, aT12, myTol1, aB1);
    bOut = aB1.IsOut(aB2);
    if (bOut) {
      break;
    }
    //
    bThin = ((aT12 - aT11) < myRes1) ||
      (aB1.IsXThin(myTol) && aB1.IsYThin(myTol) && aB1.IsZThin(myTol));
    //
    bOut = !FindParameters(myCurve2, aTB21, aTB22, myTol2, myRes2, myPTol2, 
                           myResCoeff2, aB1, aT21, aT22);
    if (bOut || bThin) {
      break;
    }
    //
    //2. Build box for second edge and find parameters 
    //   of the first one in that box
    BndBuildBox(myCurve2, aT21, aT22, myTol2, aB2);
    bOut = aB1.IsOut(aB2);
    if (bOut) {
      break;
    }
    //
    bThin = ((aT22 - aT21) < myRes2) ||
      (aB2.IsXThin(myTol) && aB2.IsYThin(myTol) && aB2.IsZThin(myTol));
    //
    bOut = !FindParameters(myCurve1, aTB11, aTB12, myTol1, myRes1, myPTol1,
                           myResCoeff1, aB2, aT11, aT12);
    //
    if (bOut || bThin) {
      break;
    }
    //
    //3. Check if it makes sense to continue
    aSmallStep1 = (aTB12 - aTB11) / 250.;
    aSmallStep2 = (aTB22 - aTB21) / 250.;
    //
    if (aSmallStep1 < myRes1) {
      aSmallStep1 = myRes1;
    }
    if (aSmallStep2 < myRes2) {
      aSmallStep2 = myRes2;
    }
    //
    if (((aT11 - aTB11) < aSmallStep1) && ((aTB12 - aT12) < aSmallStep1) &&
        ((aT21 - aTB21) < aSmallStep2) && ((aTB22 - aT22) < aSmallStep2)) {
      bStop = Standard_True;
    }
    //
  } while (!bStop);
  //
  if (bOut) {
    //no intersection;
    return;
  }
  //
  if (!bThin) {
    //check curves for coincidence on the ranges
    iCom = CheckCoincidence(aT11, aT12, aT21, aT22, myTol, myRes1);
    if (!iCom) {
      bThin = Standard_True;
    }
  }
  //
  if (bThin) {
    if (iCom != 0) {
      //check intermediate points
      Standard_Boolean bSol;
      Standard_Real aT1;
      gp_Pnt aP1;
      GeomAPI_ProjectPointOnCurve aProjPC;
      //
      aT1 = (aT11 + aT12) * .5;
      myGeom1->D0(aT1, aP1);
      //
      aProjPC.Init(myGeom2, aT21, aT22);
      aProjPC.Perform(aP1);
      //
      if (aProjPC.NbPoints()) {
        bSol = aProjPC.LowerDistance() <= myTol;
      }
      else {
        Standard_Real aT2;
        gp_Pnt aP2;
        //
        aT2 = (aT21 + aT22) * .5;
        myGeom2->D0(aT2, aP2);
        //
        bSol = aP1.IsEqual(aP2, myTol);
      }
      //
      if (!bSol) {
        return;
      }
    }
    //add common part
    IntTools_Range aR1(aT11, aT12), aR2(aT21, aT22);
    //
    theRanges1.Append(aR1);
    theRanges2.Append(aR2);
    return;
  }
  //
  if (!IsIntersection(aT11, aT12, aT21, aT22)) {
    return;
  }
  //
  //split ranges on segments and repeat
  Standard_Integer i, aNb1;
  IntTools_SequenceOfRanges aSegments1;
  //
  IntTools_Range aR2(aT21, aT22);
  BndBuildBox(myCurve2, aT21, aT22, myTol2, aB2);
  //
  aNb1 = SplitRangeOnSegments(aT11, aT12, myRes1, 3, aSegments1);
  for (i = 1; i <= aNb1; ++i) {
    const IntTools_Range& aR1 = aSegments1(i);
    FindSolutions(aR1, aR2, aB2, theRanges1, theRanges2);
  }
}

//=======================================================================
//function : FindParameters
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeEdge::FindParameters(const BRepAdaptor_Curve& theBAC,
                                                   const Standard_Real aT1, 
                                                   const Standard_Real aT2,
                                                   const Standard_Real theTol,
                                                   const Standard_Real theRes,
                                                   const Standard_Real thePTol,
                                                   const Standard_Real theResCoeff,
                                                   const Bnd_Box& theCBox,
                                                   Standard_Real& aTB1, 
                                                   Standard_Real& aTB2)
{
  Standard_Boolean bRet;
  Standard_Integer aC, i, k;
  Standard_Real aCf, aDiff, aDt, aT, aTB, aTOut, aTIn;
  Standard_Real aDist, aDistP, aDistTol;
  gp_Pnt aP;
  Bnd_Box aCBx;
  //
  bRet = Standard_False;
  aCf = 0.6180339887498948482045868343656;// =0.5*(1.+sqrt(5.))/2.;
  aDt = theRes;
  aDistP = 0.;
  aDistTol = 1e-9;
  aCBx = theCBox;
  aCBx.Enlarge(theTol);
  //
  const Handle(Geom_Curve)& aCurve = theBAC.Curve().Curve();
  const GeomAbs_CurveType aCurveType = theBAC.GetType();
  //
  for (i = 0; i < 2; ++i) {
    aTB = !i ? aT1 : aT2;
    aT = !i ? aT2 : aTB1;
    aC = !i ? 1 : -1;
    bRet = Standard_False;
    k = 0;
    //looking for the point on the edge which is in the box;
    while (aC*(aT-aTB) >= 0) {
      theBAC.D0(aTB, aP);
      aDist = PointBoxDistance(theCBox, aP);
      if (aDist > theTol) {
        if (fabs(aDist - aDistP) < aDistTol) {
          aDt = Resolution(aCurve, aCurveType, theResCoeff, (++k)*aDist);
        } else {
          k = 0;
          aDt = Resolution(aCurve, aCurveType, theResCoeff, aDist);
        }
        aTB += aC*aDt;
      } else {
        bRet = Standard_True;
        break;
      }
      aDistP = aDist;
    }
    //
    if (!bRet) {
      if (!i) {
        //edge is out of the box;
        return bRet;
      } else {
        bRet = !bRet;
        aTB = aTB1;
        aDt = aT2 - aTB1;
      }
    }
    //
    aT = !i ? aT1 : aT2;
    if (aTB != aT) {
      //one point IN, one point OUT; looking for the bounding point;
      aTIn = aTB;
      aTOut = aTB - aC*aDt;
      aDiff = aTIn - aTOut;
      while (fabs(aDiff) > thePTol) {
        aTB = aTOut + aDiff*aCf;
        theBAC.D0(aTB, aP);
        if (aCBx.IsOut(aP)) {
          aTOut = aTB;
        } else {
          aTIn = aTB;
        }
        aDiff = aTIn - aTOut;
      }
    }
    if (!i) {
      aTB1 = aTB;
    } else {
      aTB2 = aTB;
    }
  }
  return bRet;
}

//=======================================================================
//function : MergeSolutions
//purpose  : 
//=======================================================================
void IntTools_EdgeEdge::MergeSolutions(const IntTools_SequenceOfRanges& theRanges1, 
                                       const IntTools_SequenceOfRanges& theRanges2,
                                       const Standard_Boolean bSplit2)
{
  Standard_Integer aNbCP = theRanges1.Length();
  if (aNbCP == 0) {
    return;
  }
  //
  IntTools_Range aRi1, aRi2, aRj1, aRj2;
  Standard_Boolean bCond;
  Standard_Integer i, j;
  TopAbs_ShapeEnum aType;
  Standard_Real aT11, aT12, aT21, aT22;
  Standard_Real aTi11, aTi12, aTi21, aTi22;
  Standard_Real aTj11, aTj12, aTj21, aTj22;
  Standard_Real aRes1, aRes2, dTR1, dTR2;
  BOPCol_MapOfInteger aMI;
  //
  aRes1 = Resolution(myCurve1.Curve().Curve(), 
                     myCurve1.GetType(), myResCoeff1, myTol);
  aRes2 = Resolution(myCurve2.Curve().Curve(), 
                     myCurve2.GetType(), myResCoeff2, myTol);
  //
  myRange1.Range(aT11, aT12);
  myRange2.Range(aT21, aT22);
  dTR1 = 20*aRes1;
  dTR2 = 20*aRes2;
  aType = TopAbs_VERTEX;
  //
  for (i = 1; i <= aNbCP;) {
    if (aMI.Contains(i)) {
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
    for (j = i+1; j <= aNbCP; ++j) {
      if (aMI.Contains(j)) {
        continue;
      }
      //
      aRj1 = theRanges1(j);
      aRj2 = theRanges2(j);
      //
      aRj1.Range(aTj11, aTj12);
      aRj2.Range(aTj21, aTj22);
      //
      bCond = (fabs(aTi12 - aTj11) < dTR1) ||
        (bSplit2 && (fabs(aTj12 - aTi11) < dTR1));
      if (bCond && bSplit2) {
        bCond = (fabs((Max(aTi22, aTj22) - Min(aTi21, aTj21)) - 
                      ((aTi22 - aTi21) + (aTj22 - aTj21))) < dTR2);
      }
      //
      if (bCond) {
        aTi11 = Min(aTi11, aTj11);
        aTi12 = Max(aTi12, aTj12);
        aTi21 = Min(aTi21, aTj21);
        aTi22 = Max(aTi22, aTj22);
        aMI.Add(j);
      }
      else if (!bSplit2) {
        i = j;
        break;
      }
    }
    //
    if (((fabs(aT11 - aTi11) < myRes1) && (fabs(aT12 - aTi12) < myRes1)) ||
        ((fabs(aT21 - aTi21) < myRes2) && (fabs(aT22 - aTi22) < myRes2))) {
      aType = TopAbs_EDGE;
      myCommonParts.Clear();
    }
    //
    AddSolution(aTi11, aTi12, aTi21, aTi22, aType);
    if (aType == TopAbs_EDGE) {
      break;
    }
    //
    if (bSplit2) {
      ++i;
    }
  }
}

//=======================================================================
//function : AddSolution
//purpose  : 
//=======================================================================
void IntTools_EdgeEdge::AddSolution(const Standard_Real aT11,
                                    const Standard_Real aT12,
                                    const Standard_Real aT21,
                                    const Standard_Real aT22,
                                    const TopAbs_ShapeEnum theType)
{
  IntTools_CommonPrt aCPart;
  //
  aCPart.SetType(theType);
  if (!mySwap) {
    aCPart.SetEdge1(myEdge1);
    aCPart.SetEdge2(myEdge2);
    aCPart.SetRange1(aT11, aT12);
    aCPart.AppendRange2(aT21, aT22);
  } else {
    aCPart.SetEdge1(myEdge2);
    aCPart.SetEdge2(myEdge1);
    aCPart.SetRange1(aT21, aT22);
    aCPart.AppendRange2(aT11, aT12);
  }
  //
  if (theType == TopAbs_VERTEX) {
    Standard_Real aT1, aT2;
    //
    FindBestSolution(aT11, aT12, aT21, aT22, aT1, aT2);
    //
    if (!mySwap) {
      aCPart.SetVertexParameter1(aT1);
      aCPart.SetVertexParameter2(aT2);
    } else {
      aCPart.SetVertexParameter1(aT2);
      aCPart.SetVertexParameter2(aT1);
    }
  }
  myCommonParts.Append(aCPart);
}

//=======================================================================
//function : FindBestSolution
//purpose  : 
//=======================================================================
void IntTools_EdgeEdge::FindBestSolution(const Standard_Real aT11,
                                         const Standard_Real aT12,
                                         const Standard_Real aT21,
                                         const Standard_Real aT22,
                                         Standard_Real& aT1,
                                         Standard_Real& aT2)
{
  Standard_Integer i, aNbS, iErr;
  Standard_Real aDMin, aD, aRes1, aSolCriteria, aTouchCriteria;
  Standard_Real aT1A, aT1B, aT1Min, aT2Min;
  Standard_Real aT1Im, aT2Im, aT1Touch;
  GeomAPI_ProjectPointOnCurve aProjPC;
  IntTools_SequenceOfRanges aRanges;
  Standard_Boolean bTouch;
  //
  aDMin = Precision::Infinite();
  aSolCriteria   = 5.e-16;
  aTouchCriteria = 5.e-13;
  bTouch = Standard_False;
  aT1Touch = aT11;
  //
  aRes1 = Resolution(myCurve1.Curve().Curve(), 
                     myCurve1.GetType(), myResCoeff1, myTol);
  aNbS = 10;
  aNbS = SplitRangeOnSegments(aT11, aT12, 3*aRes1, aNbS, aRanges);
  //
  aProjPC.Init(myGeom2, aT21, aT22);
  //
  aT1 = (aT11 + aT12) * 0.5;
  iErr = DistPC(aT1, myGeom1, aSolCriteria, aProjPC, aD, aT2, -1);
  if (iErr == 1) {
    aT2 = (aT21 + aT22) * 0.5;
  }
  //
  aT1Im = aT1;
  aT2Im = aT2;
  //
  for (i = 1; i <= aNbS; ++i) {
    const IntTools_Range& aR1 = aRanges(i);
    aR1.Range(aT1A, aT1B);
    //
    aD = myTol;
    iErr = FindDistPC(aT1A, aT1B, myGeom1, aSolCriteria, myPTol1,
                      aProjPC, aD, aT1Min, aT2Min, Standard_False);
    if (iErr != 1) {
      if (aD < aDMin) {
        aT1 = aT1Min;
        aT2 = aT2Min;
        aDMin = aD;
      }
      //
      if (aD < aTouchCriteria) {
        if (bTouch) {
          aT1A = (aT1Touch + aT1Min) * 0.5;
          iErr = DistPC(aT1A, myGeom1, aTouchCriteria, 
                        aProjPC, aD, aT2Min, -1);
          if (aD > aTouchCriteria) {
            aT1 = aT1Im;
            aT2 = aT2Im;
            break;
          }
        }
        else {
          aT1Touch = aT1Min;
          bTouch = Standard_True;
        }
      }
    }
  }
}

//=======================================================================
//function : ComputeLineLine
//purpose  : 
//=======================================================================
void IntTools_EdgeEdge::ComputeLineLine()
{
  Standard_Boolean IsParallel, IsCoincide;
  Standard_Real aSin, aCos, aAng, aTol;
  Standard_Real aT1, aT2, aT11, aT12, aT21, aT22;
  gp_Pnt aP11, aP12;
  gp_Lin aL1, aL2;
  gp_Dir aD1, aD2;
  IntTools_CommonPrt aCommonPrt;
  //
  IsParallel = Standard_False;
  IsCoincide = Standard_False;
  aTol = myTol*myTol;
  aL1 = myCurve1.Line();
  aL2 = myCurve2.Line();
  aD1 = aL1.Position().Direction();
  aD2 = aL2.Position().Direction();
  myRange1.Range(aT11, aT12);
  myRange2.Range(aT21, aT22);
  //
  aCommonPrt.SetEdge1(myEdge1);
  aCommonPrt.SetEdge2(myEdge2);
  //
  aCos = aD1.Dot(aD2);
  aAng = (aCos >= 0.) ? 2.*(1. - aCos) : 2.*(1. + aCos);
  //
  if(aAng <= Precision::Angular()) {
    IsParallel = Standard_True;
    if(aL1.SquareDistance(aL2.Location()) <= aTol) {
      IsCoincide = Standard_True;
      aP11 = ElCLib::Value(aT11, aL1);
      aP12 = ElCLib::Value(aT12, aL1);
    }
  }
  else {
    aP11 = ElCLib::Value(aT11, aL1);
    aP12 = ElCLib::Value(aT12, aL1);
    if(aL2.SquareDistance(aP11) <= aTol && aL2.SquareDistance(aP12) <= aTol) {
      IsCoincide = Standard_True;
    }
  }
  //
  if (IsCoincide) {
    Standard_Real t21, t22;
    //
    t21 = ElCLib::Parameter(aL2, aP11);
    t22 = ElCLib::Parameter(aL2, aP12);
    if((t21 > aT22 && t22 > aT22) || (t21 < aT21 && t22 < aT21)) {
      return;
    }
    //
    Standard_Real temp;
    if(t21 > t22) {
      temp = t21;
      t21 = t22;
      t22 = temp;
    }
    //
    if(t21 >= aT21) {
      if(t22 <= aT22) {
        aCommonPrt.SetRange1(aT11, aT12);
        aCommonPrt.SetAllNullFlag(Standard_True);
        aCommonPrt.AppendRange2(t21, t22);
      }
      else {
        aCommonPrt.SetRange1(aT11, aT12 - (t22 - aT22));
        aCommonPrt.AppendRange2(t21, aT22);
      }
    }
    else {
      aCommonPrt.SetRange1(aT11 + (aT21 - t21), aT12);
      aCommonPrt.AppendRange2(aT21, t22);
    }
    aCommonPrt.SetType(TopAbs_EDGE);  
    myCommonParts.Append(aCommonPrt);
    return;
  }
  //
  if (IsParallel) {
    return;
  }
  //
  {
    TopoDS_Iterator aIt1, aIt2;
    aIt1.Initialize(myEdge1);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aV1 = aIt1.Value();
      aIt2.Initialize(myEdge2);
      for (; aIt2.More(); aIt2.Next()) {
        const TopoDS_Shape& aV2 = aIt2.Value();
        if (aV2.IsSame(aV1)) {
          return;
        }
      }
    }
  }
  //
  aSin = 1. - aCos*aCos;
  gp_Pnt O1 = aL1.Location();
  gp_Pnt O2 = aL2.Location();
  gp_Vec O1O2 (O1, O2);
  //
  aT2 = (aD1.XYZ()*(O1O2.Dot(aD1))-(O1O2.XYZ())).Dot(aD2.XYZ());
  aT2 /= aSin;
  //
  if(aT2 < aT21 || aT2 > aT22) {
    return;
  }
  //
  gp_Pnt aP2(ElCLib::Value(aT2, aL2));
  aT1 = (gp_Vec(O1, aP2)).Dot(aD1);
  //
  if(aT1 < aT11 || aT1 > aT12) {
    return;
  }
  //
  gp_Pnt aP1(ElCLib::Value(aT1, aL1));
  Standard_Real aDist = aP1.SquareDistance(aP2);
  //
  if (aDist > aTol) {
    return;
  }
  //
  // compute correct range on the edges
  Standard_Real anAngle, aDt1, aDt2;
  //
  anAngle = aD1.Angle(aD2);
  //
  aDt1 = IntTools_Tools::ComputeIntRange(myTol1, myTol2, anAngle);
  aDt2 = IntTools_Tools::ComputeIntRange(myTol2, myTol1, anAngle);
  //
  aCommonPrt.SetRange1(aT1 - aDt1, aT1 + aDt1);
  aCommonPrt.AppendRange2(aT2 - aDt2, aT2 + aDt2);
  aCommonPrt.SetType(TopAbs_VERTEX);
  aCommonPrt.SetVertexParameter1(aT1);
  aCommonPrt.SetVertexParameter2(aT2);
  myCommonParts.Append(aCommonPrt);
}

//=======================================================================
//function : IsIntersection
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeEdge::IsIntersection(const Standard_Real aT11,
                                                   const Standard_Real aT12,
                                                   const Standard_Real aT21,
                                                   const Standard_Real aT22)
{
  Standard_Boolean bRet;
  gp_Pnt aP11, aP12, aP21, aP22;
  gp_Vec aV11, aV12, aV21, aV22;
  Standard_Real aD11_21, aD11_22, aD12_21, aD12_22, aCriteria, aCoef;
  Standard_Boolean bSmall_11_21, bSmall_11_22, bSmall_12_21, bSmall_12_22;
  //
  bRet = Standard_True;
  aCoef = 1.e+5;
  if (((aT12 - aT11) > aCoef*myRes1) && ((aT22 - aT21) > aCoef*myRes2)) {
    aCoef = 5000;
  } else {
    Standard_Real aTRMin = Min((aT12 - aT11)/myRes1, (aT22 - aT21)/myRes2);
    aCoef = aTRMin / 100.;
    if (aCoef < 1.) {
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
  if ((bSmall_11_21 && bSmall_12_22) ||
      (bSmall_11_22 && bSmall_12_21)) {
    if (aCoef == 1.) {
      return bRet;
    }
    //
    Standard_Real anAngleCriteria;
    Standard_Real anAngle1, anAngle2;
    //
    anAngleCriteria = 5.e-3;
    if (bSmall_11_21 && bSmall_12_22) {
      anAngle1 = aV11.Angle(aV21);
      anAngle2 = aV12.Angle(aV22);
    } else {
      anAngle1 = aV11.Angle(aV22);
      anAngle2 = aV12.Angle(aV21);
    }
    //
    if (((anAngle1 < anAngleCriteria) || ((M_PI - anAngle1) < anAngleCriteria)) ||
        ((anAngle2 < anAngleCriteria) || ((M_PI - anAngle2) < anAngleCriteria))) {
      GeomAPI_ProjectPointOnCurve aProjPC;
      Standard_Integer iErr;
      Standard_Real aD, aT1Min, aT2Min;
      //
      aD = Precision::Infinite();
      aProjPC.Init(myGeom2, aT21, aT22);
      iErr = FindDistPC(aT11, aT12, myGeom1, myTol, myRes1, 
                        aProjPC, aD, aT1Min, aT2Min, Standard_False);
      bRet = (iErr == 2);
    }
  }
  return bRet;
}

//=======================================================================
//function : CheckCoincidence
//purpose  : 
//=======================================================================
Standard_Integer IntTools_EdgeEdge::CheckCoincidence(const Standard_Real aT11,
                                                     const Standard_Real aT12,
                                                     const Standard_Real aT21,
                                                     const Standard_Real aT22,
                                                     const Standard_Real theCriteria,
                                                     const Standard_Real theCurveRes1)
{
  Standard_Integer iErr, aNb, aNb1, i;
  Standard_Real aT1A, aT1B, aT1max, aT2max, aDmax;
  GeomAPI_ProjectPointOnCurve aProjPC;
  IntTools_SequenceOfRanges aRanges;
  //
  iErr  = 0;
  aDmax = -1.;
  aProjPC.Init(myGeom2, aT21, aT22);
  //
  // 1. Express evaluation
  aNb = 10; // Number of intervals on the curve #1
  aNb1 = SplitRangeOnSegments(aT11, aT12, theCurveRes1, aNb, aRanges);
  for (i = 1; i < aNb1; ++i) {
    const IntTools_Range& aR1 = aRanges(i);
    aR1.Range(aT1A, aT1B);
    //
    iErr = DistPC(aT1B, myGeom1, theCriteria, aProjPC, aDmax, aT2max);
    if (iErr) {
      return iErr;
    }
  }
  //
  // if the ranges in aRanges are less than theCurveRes1,
  // there is no need to do step 2 (deep evaluation)
  if (aNb1 < aNb) {
    return iErr;
  }
  //
  // 2. Deep evaluation
  for (i = 2; i < aNb1; ++i) {
    const IntTools_Range& aR1 = aRanges(i);
    aR1.Range(aT1A, aT1B);
    //
    iErr = FindDistPC(aT1A, aT1B, myGeom1, theCriteria, theCurveRes1, 
                      aProjPC, aDmax, aT1max, aT2max);
    if (iErr) {
      return iErr;
    }
  }
  // Possible values:
  // iErr == 0 - the patches are coincided
  // iErr == 1 - a point from aC1 can not be projected on aC2
  // iErr == 2 - the distance is too big
  return iErr;
}

//=======================================================================
//function : FindDistPC
//purpose  : 
//=======================================================================
Standard_Integer FindDistPC(const Standard_Real aT1A, 
                            const Standard_Real aT1B,
                            const Handle(Geom_Curve)& theC1,
                            const Standard_Real theCriteria,
                            const Standard_Real theEps,
                            GeomAPI_ProjectPointOnCurve& theProjPC,
                            Standard_Real& aDmax, 
                            Standard_Real& aT1max,
                            Standard_Real& aT2max,
                            const Standard_Boolean bMaxDist) 
{
  Standard_Integer iErr, iC;
  Standard_Real aGS, aXP, aA, aB, aXL, aYP, aYL, aT2P, aT2L;
  //
  iC = bMaxDist ? 1 : -1;
  iErr = 0;
  aT1max = aT2max = 0.; // silence GCC warning
  //
  aGS = 0.6180339887498948482045868343656;// =0.5*(1.+sqrt(5.))-1.;
  aA = aT1A;
  aB = aT1B;
  //
  // check bounds
  iErr = DistPC(aA, theC1, theCriteria, theProjPC, 
                aYP, aT2P, aDmax, aT1max, aT2max, iC);
  if (iErr == 2) {
    return iErr;
  }
  //
  iErr = DistPC(aB, theC1, theCriteria, theProjPC, 
                aYL, aT2L, aDmax, aT1max, aT2max, iC);
  if (iErr == 2) {
    return iErr;
  }
  //
  aXP = aA + (aB - aA)*aGS;
  aXL = aB - (aB - aA)*aGS;
  //
  iErr = DistPC(aXP, theC1, theCriteria, theProjPC, 
                aYP, aT2P, aDmax, aT1max, aT2max, iC);
  if (iErr) {
    return iErr;
  }
  //
  iErr = DistPC(aXL, theC1, theCriteria, theProjPC, 
                aYL, aT2L, aDmax, aT1max, aT2max, iC);
  if (iErr) {
    return iErr;
  }
  //
  for (;;) {
    if (iC*(aYP - aYL) > 0) {
      aA = aXL;
      aXL = aXP;
      aYL = aYP;
      aXP = aA + (aB - aA)*aGS;
      iErr = DistPC(aXP, theC1, theCriteria, theProjPC, 
                    aYP, aT2P, aDmax, aT1max, aT2max, iC);
    }
    else {
      aB = aXP;
      aXP = aXL;
      aYP = aYL;
      aXL = aB - (aB - aA)*aGS;
      iErr = DistPC(aXL, theC1, theCriteria, theProjPC, 
                    aYL, aT2L, aDmax, aT1max, aT2max, iC);
    }
    //
    if (iErr) {
      if ((iErr == 2) && !bMaxDist) {
        aXP = (aA + aB) * 0.5;
        DistPC(aXP, theC1, theCriteria, theProjPC, 
               aYP, aT2P, aDmax, aT1max, aT2max, iC);
      }
      return iErr;
    }
    //
    if ((aB - aA) < theEps) {
      break;
    }
  }// for (;;) {
  //
  return iErr;
}
//=======================================================================
//function : DistPC
//purpose  : 
//=======================================================================
Standard_Integer DistPC(const Standard_Real aT1, 
                        const Handle(Geom_Curve)& theC1,
                        const Standard_Real theCriteria,
                        GeomAPI_ProjectPointOnCurve& theProjPC, 
                        Standard_Real& aD, 
                        Standard_Real& aT2,
                        Standard_Real& aDmax,
                        Standard_Real& aT1max,
                        Standard_Real& aT2max,
                        const Standard_Integer iC)
{
  Standard_Integer iErr;
  //
  iErr = DistPC(aT1, theC1, theCriteria, theProjPC, aD, aT2, iC);
  if (iErr == 1) {
    return iErr;
  }
  //
  if (iC*(aD - aDmax) > 0) {
    aDmax = aD;
    aT1max = aT1;
    aT2max = aT2;
  }
  //
  return iErr;
}
//=======================================================================
//function : DistPC
//purpose  : 
//=======================================================================
Standard_Integer DistPC(const Standard_Real aT1, 
                        const Handle(Geom_Curve)& theC1,
                        const Standard_Real theCriteria, 
                        GeomAPI_ProjectPointOnCurve& theProjPC,
                        Standard_Real& aD, 
                        Standard_Real& aT2,
                        const Standard_Integer iC) 
{
  Standard_Integer iErr, aNbP2;
  gp_Pnt aP1;
  //
  iErr = 0;
  theC1->D0(aT1, aP1);
  //
  theProjPC.Perform(aP1);
  aNbP2 = theProjPC.NbPoints();
  if (!aNbP2) {
    iErr = 1;// the point from aC1 can not be projected on aC2
    return iErr;
  }
  //
  aD  = theProjPC.LowerDistance();
  aT2 = theProjPC.LowerDistanceParameter();
  if (iC*(aD - theCriteria) > 0) {
    iErr = 2;// the distance is too big or small
  }
  //
  return iErr;
}

//=======================================================================
//function : SplitRangeOnSegments
//purpose  : 
//=======================================================================
Standard_Integer SplitRangeOnSegments(const Standard_Real aT1, 
                                      const Standard_Real aT2,
                                      const Standard_Real theResolution,
                                      const Standard_Integer theNbSeg,
                                      IntTools_SequenceOfRanges& theSegments)
{
  Standard_Real aDiff = aT2 - aT1;
  if (aDiff < theResolution || theNbSeg == 1) {
    theSegments.Append(IntTools_Range(aT1, aT2));
    return 1;
  }
  //
  Standard_Real aDt, aT1x, aT2x, aSeg;
  Standard_Integer aNbSegments, i;
  //
  aNbSegments = theNbSeg;
  aDt = aDiff / aNbSegments;
  if (aDt < theResolution) {
    aSeg = aDiff / theResolution;
    aNbSegments = Standard_Integer(aSeg) + 1;
    aDt = aDiff / aNbSegments;
  }
  //
  aT1x = aT1;
  for (i = 1; i < aNbSegments; ++i) {
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

//=======================================================================
//function : BndBuildBox
//purpose  : 
//=======================================================================
void BndBuildBox(const BRepAdaptor_Curve& theBAC,
                 const Standard_Real aT1,
                 const Standard_Real aT2,
                 const Standard_Real theTol,
                 Bnd_Box& theBox)
{
  Bnd_Box aB;
  BndLib_Add3dCurve::Add(theBAC, aT1, aT2, theTol, aB);
  theBox = aB;
}

//=======================================================================
//function : PointBoxDistance
//purpose  : 
//=======================================================================
Standard_Real PointBoxDistance(const Bnd_Box& aB,
                               const gp_Pnt& aP)
{
  Standard_Real aPCoord[3];
  Standard_Real aBMinCoord[3], aBMaxCoord[3];
  Standard_Real aDist, aR1, aR2;
  Standard_Integer i;
  //
  aP.Coord(aPCoord[0], aPCoord[1], aPCoord[2]);
  aB.Get(aBMinCoord[0], aBMinCoord[1], aBMinCoord[2], 
         aBMaxCoord[0], aBMaxCoord[1], aBMaxCoord[2]);
  //
  aDist = 0.;
  for (i = 0; i < 3; ++i) {
    aR1 = aBMinCoord[i] - aPCoord[i];
    if (aR1 > 0.) {
      aDist += aR1*aR1;
      continue;
    }
    //
    aR2 = aPCoord[i] - aBMaxCoord[i];
    if (aR2 > 0.) {
      aDist += aR2*aR2;
    }
  }
  //
  aDist = Sqrt(aDist);
  return aDist;
}

//=======================================================================
//function : TypeToInteger
//purpose  : 
//=======================================================================
Standard_Integer TypeToInteger(const GeomAbs_CurveType theCType)
{
  Standard_Integer iRet;
  //
  switch(theCType) {
  case GeomAbs_Line:
    iRet=0;
    break;
  case GeomAbs_Hyperbola:
  case GeomAbs_Parabola:
    iRet=1;
    break;
  case GeomAbs_Circle:
  case GeomAbs_Ellipse:
    iRet=2;
    break;
  case GeomAbs_BezierCurve:
  case GeomAbs_BSplineCurve:
    iRet=3;
    break;
  default:
    iRet=4;
    break;
  }
  return iRet;
}

//=======================================================================
//function : ResolutionCoeff
//purpose  : 
//=======================================================================
Standard_Real ResolutionCoeff(const BRepAdaptor_Curve& theBAC,
                              const IntTools_Range& theRange)
{
  Standard_Real aResCoeff = 0.;
  //
  const Handle(Geom_Curve)& aCurve = theBAC.Curve().Curve();
  const GeomAbs_CurveType aCurveType = theBAC.GetType();
  //
  switch (aCurveType) {
  case GeomAbs_Circle :
    aResCoeff = 1. / (2 * Handle(Geom_Circle)::DownCast (aCurve)->Circ().Radius());
    break;
  case GeomAbs_Ellipse :
    aResCoeff =  1. / Handle(Geom_Ellipse)::DownCast (aCurve)->MajorRadius();
    break;
  case GeomAbs_OffsetCurve : {
    const Handle(Geom_OffsetCurve)& anOffsetCurve = Handle(Geom_OffsetCurve)::DownCast(aCurve);
    const Handle(Geom_Curve)& aBasisCurve = anOffsetCurve->BasisCurve();
    GeomAdaptor_Curve aGBasisCurve(aBasisCurve);
    const GeomAbs_CurveType aBCType = aGBasisCurve.GetType();
    if (aBCType == GeomAbs_Line) {
      break;
    }
    else if (aBCType == GeomAbs_Circle) {
      aResCoeff = 1. / (2 * (anOffsetCurve->Offset() + aGBasisCurve.Circle().Radius()));
      break;
    }
    else if (aBCType == GeomAbs_Ellipse) {
      aResCoeff = 1. / (anOffsetCurve->Offset() + aGBasisCurve.Ellipse().MajorRadius());
      break;
    }
  }
  case GeomAbs_Hyperbola :
  case GeomAbs_Parabola : 
  case GeomAbs_OtherCurve :{
    Standard_Real k, kMin, aDist, aDt, aT1, aT2, aT;
    Standard_Integer aNbP, i;
    gp_Pnt aP1, aP2;
    //
    aNbP = 30;
    theRange.Range(aT1, aT2);
    aDt = (aT2 - aT1) / aNbP;
    aT = aT1;
    kMin = 10.;
    //
    theBAC.D0(aT1, aP1);
    for (i = 1; i <= aNbP; ++i) {
      aT += aDt;
      theBAC.D0(aT, aP2);
      aDist = aP1.Distance(aP2);
      k = aDt / aDist;
      if (k < kMin) {
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

//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================
Standard_Real Resolution(const Handle(Geom_Curve)& theCurve,
                         const GeomAbs_CurveType theCurveType,
                         const Standard_Real theResCoeff,
                         const Standard_Real theR3D)
{
  Standard_Real aRes;
  //
  switch (theCurveType) {
  case GeomAbs_Line :
    aRes = theR3D;
    break;
  case GeomAbs_Circle: {
    Standard_Real aDt = theResCoeff * theR3D;
    aRes = (aDt <= 1.) ? 2*ASin(aDt) : 2*M_PI;
    break;
  }
  case GeomAbs_BezierCurve:
    Handle(Geom_BezierCurve)::DownCast (theCurve)->Resolution(theR3D, aRes);
    break;
  case GeomAbs_BSplineCurve:
    Handle(Geom_BSplineCurve)::DownCast (theCurve)->Resolution(theR3D, aRes);
    break;
  case GeomAbs_OffsetCurve: {
    const Handle(Geom_Curve)& aBasisCurve = 
      Handle(Geom_OffsetCurve)::DownCast(theCurve)->BasisCurve();
    const GeomAbs_CurveType aBCType = GeomAdaptor_Curve(aBasisCurve).GetType();
    if (aBCType == GeomAbs_Line) {
      aRes = theR3D;
      break;
    }
    else if (aBCType == GeomAbs_Circle) {
      Standard_Real aDt = theResCoeff * theR3D;
      aRes = (aDt <= 1.) ? 2*ASin(aDt) : 2*M_PI;
      break;
    }
  }
  default:
    aRes = theResCoeff * theR3D;
    break;
  }
  //
  return aRes;
}

//=======================================================================
//function : CurveDeflection
//purpose  : 
//=======================================================================
Standard_Real CurveDeflection(const BRepAdaptor_Curve& theBAC,
                              const IntTools_Range& theRange)
{
  Standard_Real aDt, aT, aT1, aT2, aDefl;
  Standard_Integer i, aNbP;
  gp_Vec aV1, aV2;
  gp_Pnt aP;
  //
  aDefl = 0;
  aNbP = 10;
  theRange.Range(aT1, aT2);
  aDt = (aT2 - aT1) / aNbP;
  aT = aT1;
  //
  theBAC.D1(aT1, aP, aV1);
  for (i = 1; i <= aNbP; ++i) {
    aT += aDt;
    theBAC.D1(aT, aP, aV2);
    if (aV1.Magnitude() > gp::Resolution() &&
        aV2.Magnitude() > gp::Resolution()) {
      gp_Dir aD1(aV1), aD2(aV2);
      aDefl += aD1.Angle(aD2);
    }
    aV1 = aV2;
  }
  //
  return aDefl;
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================
Standard_Boolean IsClosed(const Handle(Geom_Curve)& theCurve,
                          const Standard_Real aT1,
                          const Standard_Real aT2,
                          const Standard_Real theTol,
                          const Standard_Real theRes)
{
  if (Abs(aT1 - aT2) < theRes)
  {
    return Standard_False;
  }

  gp_Pnt aP1, aP2;
  theCurve->D0(aT1, aP1);
  theCurve->D0(aT2, aP2);
  //
  Standard_Real aD = aP1.Distance(aP2);
  return aD < theTol;
}
