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

#include <IntTools_EdgeEdge.ixx> 

#include <gp_Dir.hxx>
#include <gp_Lin.hxx>

#include <ElCLib.hxx>

#include <TopoDS_Iterator.hxx>

#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <IntTools_Tools.hxx>
#include <IntTools_CommonPrt.hxx>


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
  void SplitRangeOnSegments(const Standard_Real aT1, 
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
      Standard_Integer i;
      Standard_Real aDt, aT, aT1, aT2;
      gp_Vec aV1, aV2;
      gp_Pnt aP;
      //
      Standard_Real aC1(10.), aC2(10.);
      for (i = 0; i < 2; ++i) {
        Standard_Real &aC = !i ? aC2 : aC1;
        aC = 0;
        IntTools_Range aR = !i ? myRange2 : myRange1;
        const BRepAdaptor_Curve& aBAC = !i ? myCurve2 : myCurve1;
        aR.Range(aT1, aT2);
        aDt = (aT2 - aT1) / 10.;
        aT = aT1;
        aBAC.D1(aT, aP, aV1);
        while (aT < aT2) {
          aT += aDt;
          aBAC.D1(aT, aP, aV2);
          if (aV1.Magnitude() > gp::Resolution() &&
              aV2.Magnitude() > gp::Resolution()) {
            gp_Dir aD1(aV1), aD2(aV2);
            aC += aD1.Angle(aD2);
          }
          aV1 = aV2;
        }
        //
        if (aC < Precision::Confusion()) {
          break;
        }
      }
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
  myTol1 = myCurve1.Tolerance();
  myTol2 = myCurve2.Tolerance();
  myTol = myTol1 + myTol2;
  //
  myRes1 = myCurve1.Resolution(myTol);
  myRes2 = myCurve2.Resolution(myTol);
  //
  if (iCT1 != 0 || iCT2 != 0) {
    Standard_Real f, l, aTM;
    //
    myGeom1 = BRep_Tool::Curve(myEdge1, f, l);
    myGeom2 = BRep_Tool::Curve(myEdge2, f, l);
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
  IntTools_SequenceOfRanges aRanges1, aRanges2;
  //
  //3.2. Find ranges containig solutions
  FindSolutions(myRange1, myRange2, aRanges1, aRanges2);
  //
  //4. Merge solutions and save common parts
  MergeSolutions(aRanges1, aRanges2);
}

//=======================================================================
//function : FindSolutions
//purpose  : 
//=======================================================================
void IntTools_EdgeEdge::FindSolutions(const IntTools_Range& theR1,
                                      const IntTools_Range& theR2,
                                      IntTools_SequenceOfRanges& theRanges1, 
                                      IntTools_SequenceOfRanges& theRanges2)
{
  Standard_Boolean bOut, bStop, bThin;
  Standard_Real aT11, aT12, aT21, aT22;
  Standard_Real aTB11, aTB12, aTB21, aTB22;
  Standard_Real aTol, aSmallStep1, aSmallStep2;
  Standard_Integer iCom;
  Bnd_Box aB1, aB2;
  //
  theR1.Range(aT11, aT12);
  theR2.Range(aT21, aT22);
  //
  BndBuildBox(myCurve1, aT11, aT12, myTol1, aB1);
  BndBuildBox(myCurve2, aT21, aT22, myTol2, aB2);
  if (aB1.IsOut(aB2)) {
    //no intersection;
    return;
  }
  //
  bOut  = Standard_False;
  bThin = Standard_False;
  bStop = Standard_False;
  aTol  = 2*myTol;
  iCom  = 1;
  //
  do {
    bOut = !FindParameters(myCurve2, aT21, aT22, myRes2, myPTol2, aB1, aTB21, aTB22);
    if (bOut) {
      break;
    }
    //
    bThin = (aTB22 - aTB21) < myRes2;
    if (bThin) {
      bOut = !FindParameters(myCurve1, aT11, aT12, myRes1, myPTol1, aB1, aTB11, aTB12);
      break;
    }
    //
    BndBuildBox(myCurve2, aTB21, aTB22, myTol2, aB2);
    //
    bOut = !FindParameters(myCurve1, aT11, aT12, myRes1, myPTol1, aB2, aTB11, aTB12);
    if (bOut) {
      break;
    }
    //
    bThin = ((aTB12 - aTB11) < myRes1) ||
      (aB2.IsXThin(aTol) && aB2.IsYThin(aTol) && aB2.IsZThin(aTol));
    //
    if (!bThin) {
      aSmallStep1 = (aT12 - aT11) / 250.;
      aSmallStep2 = (aT22 - aT21) / 250.;
      //
      if (aSmallStep1 < myRes1) {
        aSmallStep1 = myRes1;
      }
      if (aSmallStep2 < myRes2) {
        aSmallStep2 = myRes2;
      }
      //
      if (((aTB11 - aT11) < aSmallStep1) && ((aT12 - aTB12) < aSmallStep1) &&
          ((aTB21 - aT21) < aSmallStep2) && ((aT22 - aTB22) < aSmallStep2)) {
        bStop = Standard_True;
      } else {
        BndBuildBox(myCurve1, aTB11, aTB12, myTol1, aB1);
        bOut = aB1.IsOut(aB2);
        if (bOut) {
          break;
        }
      }
    }
    //
    aT11 = aTB11;
    aT12 = aTB12;
    aT21 = aTB21;
    aT22 = aTB22;
  } while (!bThin && !bStop);
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
      Standard_Real aT1, aT2, aDist;
      gp_Pnt aP1, aP2;
      //
      aT1 = IntTools_Tools::IntermediatePoint(aT11, aT12);
      aT2 = IntTools_Tools::IntermediatePoint(aT21, aT22);
      //
      myGeom1->D0(aT1, aP1);
      myGeom2->D0(aT2, aP2);
      //
      aDist = aP1.Distance(aP2);
      if (aDist > myTol) {
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
  //
  SplitRangeOnSegments(aT11, aT12, myRes1, 3, aSegments1);
  aNb1 = aSegments1.Length();
  for (i = 1; i <= aNb1; ++i) {
    const IntTools_Range& aR1 = aSegments1(i);
    FindSolutions(aR1, aR2, theRanges1, theRanges2);
  }
}

//=======================================================================
//function : FindParameters
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeEdge::FindParameters(const BRepAdaptor_Curve& theBAC,
                                                   const Standard_Real aT1, 
                                                   const Standard_Real aT2, 
                                                   const Standard_Real theRes,
                                                   const Standard_Real thePTol,
                                                   const Bnd_Box& theCBox,
                                                   Standard_Real& aTB1, 
                                                   Standard_Real& aTB2)
{
  Standard_Boolean bRet;
  Standard_Integer aC, i, k;
  Standard_Real aCf, aDiff, aDt, aT, aTB, aTOut, aTIn;
  Standard_Real aDist, aDistP, aDistTol, aTol;
  gp_Pnt aP;
  Bnd_Box aCBx;
  //
  bRet = Standard_False;
  aCf = 0.6180339887498948482045868343656;// =0.5*(1.+sqrt(5.))/2.;
  aDt = theRes;
  aTol = theBAC.Tolerance();
  aDistP = 0.;
  aDistTol = Precision::PConfusion();
  aCBx = theCBox;
  aCBx.Enlarge(aTol);
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
      if (aDist > aTol) {
        if (fabs(aDist - aDistP) < aDistTol) {
          aDt = theBAC.Resolution((++k)*aDist);
        } else {
          k = 0;
          aDt = theBAC.Resolution(aDist);
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
                                       const IntTools_SequenceOfRanges& theRanges2)
{
  IntTools_Range aRi1, aRi2, aRj1, aRj2;
  Standard_Integer aNbCP, i, j;
  TopAbs_ShapeEnum aType;
  Standard_Real aTi11, aTi12, aTi21, aTi22,
                aTj11, aTj12, aTj21, aTj22;
  //
  aNbCP = theRanges1.Length();
  aType = TopAbs_VERTEX;
  //
  for (i = 1; i <= aNbCP; ) {
    aRi1 = theRanges1(i);
    aRi2 = theRanges2(i);
    //
    aRi1.Range(aTi11, aTi12);
    aRi2.Range(aTi21, aTi22);
    //
    for (j = i+1; j <= aNbCP; ++j) {
      aRj1 = theRanges1(j);
      aRj2 = theRanges2(j);
      //
      aRj1.Range(aTj11, aTj12);
      aRj2.Range(aTj21, aTj22);
      if (fabs(aTi12 - aTj11) < 10*myRes1 ||
          fabs(aTi22 - aTj21) < 10*myRes2) {
        aTi11 = Min(aTi11, aTj11);
        aTi12 = Max(aTi12, aTj12);
        aTi21 = Min(aTi21, aTj21);
        aTi22 = Max(aTi22, aTj22);
      } else {
        break;
      }
    }
    i = j;
    //
    if (aTi11 == myRange1.First() && aTi12 == myRange1.Last() &&
        aTi21 == myRange2.First() && aTi22 == myRange2.Last()) {
      aType = TopAbs_EDGE;
    }
    //
    AddSolution(aTi11, aTi12, aTi21, aTi22, aType);
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
  Standard_Real aDMin, aD, aCrit;
  Standard_Real aT1x, aT2x, aT1p, aT2p;
  GeomAPI_ProjectPointOnCurve aProj;
  IntTools_SequenceOfRanges aSeg1;
  //
  aT1 = IntTools_Tools::IntermediatePoint(aT11, aT12);
  aT2 = IntTools_Tools::IntermediatePoint(aT21, aT22);
  //
  aDMin = 100.;
  aD = 100.;
  aCrit = 0.;//1.e-16;
  //
  aNbS = 10;
  SplitRangeOnSegments(aT11, aT12, 3*myRes1, aNbS, aSeg1);
  aNbS = aSeg1.Length();
  //
  aProj.Init(myGeom2, aT21, aT22);
  for (i = 1; i <= aNbS; ++i) {
    const IntTools_Range& aR1 = aSeg1(i);
    aR1.Range(aT1x, aT2x);
    //
    iErr = FindDistPC(aT1x, aT2x, myGeom1, aCrit, myPTol1,
                      aProj, aD, aT1p, aT2p, Standard_False);
    if (iErr != 1 && aD < aDMin) {
      aT1 = aT1p;
      aT2 = aT2p;
      aDMin = aD;
      if (aDMin <= aCrit) {
        break;
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
  aCommonPrt.SetRange1(aT1 - myTol, aT1 + myTol);
  aCommonPrt.AppendRange2(aT2 - myTol, aT2 + myTol);
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
      GeomAPI_ProjectPointOnCurve aProj;
      Standard_Integer iErr;
      Standard_Real aD, aT1p, aT2p;
      //
      aD = 100.;
      aProj.Init(myGeom2, aT21, aT22);
      iErr = FindDistPC(aT11, aT12, myGeom1, myTol, myRes1, aProj, aD, aT1p, aT2p, Standard_False);
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
  IntTools_SequenceOfRanges aSeg1;
  //
  iErr  = 0;
  aDmax = -1.;
  aProjPC.Init(myGeom2, aT21, aT22);
  //
  // 1. Express evaluation
  aNb = 10; // Number of intervals on the curve #1
  SplitRangeOnSegments(aT11, aT12, theCurveRes1, aNb, aSeg1);
  aNb1 = aSeg1.Length();
  for (i = 1; i < aNb1; ++i) {
    const IntTools_Range& aR1 = aSeg1(i);
    aR1.Range(aT1A, aT1B);
    //
    iErr = DistPC(aT1B, myGeom1, theCriteria, aProjPC, aDmax, aT2max);
    if (iErr) {
      return iErr;
    }
  }
  //
  // if the ranges in aSeg1 are less than theCurveRes1,
  // there is no need to do step 2 (deep evaluation)
  if (aNb1 < aNb) {
    return iErr;
  }
  //
  // 2. Deep evaluation
  for (i = 2; i < aNb1; ++i) {
    const IntTools_Range& aR1 = aSeg1(i);
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
  //
  aGS = 0.6180339887498948482045868343656;// =0.5*(1.+sqrt(5.))-1.;
  aA = aT1A;
  aB = aT1B;
  //
  // check bounds
  iErr = DistPC(aA, theC1, theCriteria, theProjPC, aYP, aT2P, aDmax, aT1max, aT2max, iC);
  if (iErr == 2) {
    return iErr;
  }
  //
  iErr = DistPC(aB, theC1, theCriteria, theProjPC, aYL, aT2L, aDmax, aT1max, aT2max, iC);
  if (iErr == 2) {
    return iErr;
  }
  //
  aXP = aA + (aB - aA)*aGS;
  aXL = aB - (aB - aA)*aGS;
  //
  iErr = DistPC(aXP, theC1, theCriteria, theProjPC, aYP, aT2P, aDmax, aT1max, aT2max, iC);
  if (iErr) {
    return iErr;
  }
  //
  iErr = DistPC(aXL, theC1, theCriteria, theProjPC, aYL, aT2L, aDmax, aT1max, aT2max, iC);
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
      iErr = DistPC(aXP, theC1, theCriteria, theProjPC, aYP, aT2P, aDmax, aT1max, aT2max, iC);
      if (iErr) {
        return iErr;
      }
    }
    else {
      aB = aXP;
      aXP = aXL;
      aYP = aYL;
      aXL = aB - (aB - aA)*aGS;
      iErr = DistPC(aXL, theC1, theCriteria, theProjPC, aYL, aT2L, aDmax, aT1max, aT2max, iC);
      if (iErr) {
        return iErr;
      }
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
  if (iErr) {
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
void SplitRangeOnSegments(const Standard_Real aT1, 
                          const Standard_Real aT2,
                          const Standard_Real theResolution,
                          const Standard_Integer theNbSeg,
                          IntTools_SequenceOfRanges& theSegments)
{
  Standard_Real aDiff = aT2 - aT1;
  if (aDiff < theResolution) {
    theSegments.Append(IntTools_Range(aT1, aT2));
    return;
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
  case GeomAbs_Circle:
    iRet=1;
    break;
  case GeomAbs_Ellipse:
  case GeomAbs_Hyperbola:
  case GeomAbs_Parabola:
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

