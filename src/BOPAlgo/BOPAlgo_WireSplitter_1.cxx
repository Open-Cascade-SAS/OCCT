// Created by: Peter KURNEV
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <BOPAlgo_WireSplitter.ixx>

#include <Precision.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_SequenceOfShape.hxx>
#include <BOPCol_SequenceOfPnt2d.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
//
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <TopLoc_Location.hxx>
#include <BRep_Builder.hxx>
#include <BOPCol_SequenceOfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
//

static
  Standard_Real Angle (const gp_Dir2d& aDir2D);

static
  Standard_Real Angle2D (const TopoDS_Vertex& aV,
                         const TopoDS_Edge& anEdge,
                         const TopoDS_Face& myFace,
                         const GeomAdaptor_Surface& aGAS,
                         const Standard_Boolean aFlag);

static
  void GetNextVertex(const TopoDS_Vertex& aV,
                     const TopoDS_Edge& aE,
                     TopoDS_Vertex& aV1);

static
  Standard_Real AngleIn(const TopoDS_Edge& aEIn,
                        const BOPAlgo_ListOfEdgeInfo& aLEInfo);

static
  Standard_Integer NbWaysOut(const BOPAlgo_ListOfEdgeInfo& aLEInfo);

static
  gp_Pnt2d Coord2dVf (const TopoDS_Edge& aE,
                      const TopoDS_Face& aF);

static
  gp_Pnt2d Coord2d (const TopoDS_Vertex& aV1,
                    const TopoDS_Edge& aE1,
                    const TopoDS_Face& aF);


static
  Standard_Real ClockWiseAngle(const Standard_Real aAngleIn,
                               const Standard_Real aAngleOut);

static 
  void Path (const GeomAdaptor_Surface& aGAS,
             const TopoDS_Face& myFace,
             const TopoDS_Vertex& aVa,
             const TopoDS_Edge& aEOuta,
             BOPAlgo_EdgeInfo& anEdgeInfo,
             BOPCol_SequenceOfShape& aLS,
             BOPCol_SequenceOfShape& aVertVa,
             BOPCol_SequenceOfPnt2d& aCoordVa,
             BOPTools_ConnexityBlock& aCB,
             BOPAlgo_IndexedDataMapOfShapeListOfEdgeInfo& mySmartMap);
             
static
  Standard_Real Angle2D (const TopoDS_Vertex& aV,
                         const TopoDS_Edge& anEdge,
                         const TopoDS_Face& myFace,
                         const GeomAdaptor_Surface& aGAS,
                         const Standard_Boolean aFlag);
static
  Standard_Real Angle (const gp_Dir2d& aDir2D);

static
  Standard_Real Tolerance2D (const TopoDS_Vertex& aV,
                             const GeomAdaptor_Surface& aGAS);

static
  void BuildPCurveForPlane (const BOPCol_ListOfShape myEdges,
                            const TopoDS_Face& myFace);

static
  Standard_Real UTolerance2D (const TopoDS_Vertex& aV,
                              const GeomAdaptor_Surface& aGAS);
static
  Standard_Real VTolerance2D (const TopoDS_Vertex& aV,
                              const GeomAdaptor_Surface& aGAS);

static
  Standard_Boolean RecomputeAngles(const BOPAlgo_ListOfEdgeInfo& aLEInfo, 
                                   const TopoDS_Face&            theFace, 
                                   const gp_Pnt2d&               thePb, 
                                   const TopoDS_Vertex&          theVb,
                                   const GeomAdaptor_Surface&    theGAS,
                                   const TopoDS_Edge&            theEOuta, 
                                   const Standard_Boolean&       bHasClosed,
                                   const Standard_Real&          theTol2D,
                                   BOPCol_SequenceOfReal&        theRecomputedAngles);

//=======================================================================
//function : SplitBlock
//purpose  : 
//=======================================================================
  void BOPAlgo_WireSplitter::SplitBlock(BOPTools_ConnexityBlock& aCB)
{
  Standard_Boolean bNothingToDo;
  Standard_Integer aIx, aNb, i, aCntIn, aCntOut;
  Standard_Real aAngle;
  TopAbs_Orientation aOr;
  TopoDS_Iterator aItS;
  TopoDS_Vertex aVV;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPAlgo_ListIteratorOfListOfEdgeInfo aItLEI;
  //
  BOPAlgo_IndexedDataMapOfShapeListOfEdgeInfo mySmartMap(100, myAllocator);
  //
  const TopoDS_Face& myFace=myWES->Face();
  const BOPCol_ListOfShape& myEdges=aCB.Shapes();
  //
  // 1.Filling mySmartMap
  BuildPCurveForPlane(myEdges, myFace);
  aIt.Initialize(myEdges);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)&aIt.Value());
    if (!BOPTools_AlgoTools2D::HasCurveOnSurface (aE, myFace)) {
      continue;
    }
    //
    aItS.Initialize(aE);
    for(; aItS.More(); aItS.Next()) {
      const TopoDS_Shape& aV=aItS.Value();
      aIx=mySmartMap.FindIndex(aV);
      if (!aIx) {
        BOPAlgo_ListOfEdgeInfo aLEIx(myAllocator);
        aIx=mySmartMap.Add(aV, aLEIx);
      }
      //
      BOPAlgo_ListOfEdgeInfo& aLEI=mySmartMap(aIx);
      //
      BOPAlgo_EdgeInfo aEI;
      //
      aEI.SetEdge(aE);
      aOr=aV.Orientation();
      if (aOr==TopAbs_FORWARD) {
        aEI.SetInFlag(Standard_False);
      }
      else if (aOr==TopAbs_REVERSED) {
        aEI.SetInFlag(Standard_True);
      }
      aLEI.Append(aEI);
    }
  }
  //
  aNb=mySmartMap.Extent();
  //
  bNothingToDo=Standard_True;
  for (i=1; i<=aNb; i++) {
    aCntIn=0;
    aCntOut=0;
    const BOPAlgo_ListOfEdgeInfo& aLEInfo= mySmartMap(i);
    BOPAlgo_ListIteratorOfListOfEdgeInfo anIt(aLEInfo);
    for (; anIt.More(); anIt.Next()) {
      const BOPAlgo_EdgeInfo& aEI=anIt.Value();
      if (aEI.IsIn()) {
        aCntIn++;
      }
      else {
        aCntOut++;
      }
    }
    if (aCntIn!=1 || aCntOut!=1) {
      bNothingToDo=Standard_False;
      break;
    }
  }
  //
  // Each vertex has one edge In and one - Out. Good. But it is not enought
  // to consider that nothing to do with this. We must check edges on TShape
  // coinsidence. If there are such edges there is something to do with.
  if (bNothingToDo) {
    Standard_Integer aNbE, aNbMapEE;
    Standard_Boolean bFlag;
    //
    BOPCol_IndexedDataMapOfShapeListOfShape aMapEE(100, myAllocator);
    aNbE=myEdges.Extent();
    //
    aIt.Initialize(myEdges);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE = aIt.Value();
      if (!aMapEE.Contains(aE)) {
        BOPCol_ListOfShape aLEx(myAllocator);
        aLEx.Append(aE);
        aMapEE.Add(aE, aLEx);
      }
      else {
        BOPCol_ListOfShape& aLEx=aMapEE.ChangeFromKey(aE);
        aLEx.Append(aE);
      }
    }
    //
    bFlag=Standard_True;
    aNbMapEE=aMapEE.Extent();
    for (i=1; i<=aNbMapEE; ++i) {
      const BOPCol_ListOfShape& aLEx=aMapEE(i);
      aNbE=aLEx.Extent();
      if (aNbE==1) {// usual case
        continue;
      }
      else if (aNbE==2){
        const TopoDS_Shape& aE1=aLEx.First();
        const TopoDS_Shape& aE2=aLEx.Last();
        if (aE1.IsSame(aE2)) {
          bFlag=Standard_False;
          break;
        }
      }
      else {
        bFlag=Standard_False;
        break;
      }
    }
    bNothingToDo=bNothingToDo && bFlag;
  } // if (bNothingToDo) {
  if (bNothingToDo) {
    TopoDS_Wire aW;
    //
    BOPCol_ListOfShape& aLECB=aCB.ChangeShapes();
    BOPAlgo_WireSplitter::MakeWire(aLECB, aW);
    BOPCol_ListOfShape& aLoops=aCB.ChangeLoops();
    aLoops.Append(aW);
    //
    myErrorStatus=0;
    return;
  }
  //
  // 3. Angles in mySmartMap
  BRepAdaptor_Surface aBAS(myFace);
  const GeomAdaptor_Surface& aGAS=aBAS.Surface();
  //
  for (i=1; i<=aNb; i++) {
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&mySmartMap.FindKey(i))); 
    const BOPAlgo_ListOfEdgeInfo& aLEI= mySmartMap(i);

    aItLEI.Initialize(aLEI);
    for (; aItLEI.More(); aItLEI.Next()) {
      BOPAlgo_EdgeInfo& aEI=aItLEI.ChangeValue();
      const TopoDS_Edge& aE=aEI.Edge();
      //
      aVV=aV;
      if (aEI.IsIn()) {
        aVV.Orientation(TopAbs_REVERSED);
        aAngle=Angle2D (aVV, aE, myFace, aGAS, Standard_True);
      }
      else { // OUT
        aVV.Orientation(TopAbs_FORWARD);
        aAngle=Angle2D (aVV, aE, myFace, aGAS, Standard_False);
      }
      aEI.SetAngle(aAngle);
    }
  }// for (i=1; i<=aNb; i++) {
  //
  // 4. Do
  //
  Standard_Boolean bIsOut, bIsNotPassed;
  BOPCol_SequenceOfShape aLS, aVertVa;
  BOPCol_SequenceOfPnt2d aCoordVa;
  //
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Vertex& aVa=(*(TopoDS_Vertex *)(&mySmartMap.FindKey(i))); 
    const BOPAlgo_ListOfEdgeInfo& aLEI=mySmartMap(i);
    aItLEI.Initialize(aLEI);
    for (; aItLEI.More(); aItLEI.Next()) {
      BOPAlgo_EdgeInfo& aEI=aItLEI.ChangeValue();
      const TopoDS_Edge& aEOuta=aEI.Edge();
      //
      bIsOut=!aEI.IsIn();
      bIsNotPassed=!aEI.Passed();
      if (bIsOut && bIsNotPassed) {
        //
        aLS.Clear();
        aVertVa.Clear();
        aCoordVa.Clear();
        //
        Path(aGAS, myFace, aVa, aEOuta, aEI, aLS, 
             aVertVa, aCoordVa, aCB, mySmartMap);
      }
    }
  }// for (i=1; i<=aNb; ++i) {
}
//=======================================================================
// function: Path
// purpose: 
//=======================================================================
void Path (const GeomAdaptor_Surface& aGAS,
           const TopoDS_Face& myFace,
           const TopoDS_Vertex& aVa,
           const TopoDS_Edge& aEOuta,
           BOPAlgo_EdgeInfo& anEdgeInfo,
           BOPCol_SequenceOfShape& aLS,
           BOPCol_SequenceOfShape& aVertVa,
           BOPCol_SequenceOfPnt2d& aCoordVa,
           BOPTools_ConnexityBlock& aCB,
           BOPAlgo_IndexedDataMapOfShapeListOfEdgeInfo& mySmartMap)
     
{
  Standard_Integer i, j, aNb, aNbj;
  Standard_Real aTol, anAngleIn, anAngleOut, anAngle, aMinAngle;
  Standard_Real aTol2D, aTol2D2;
  Standard_Real aTol2, aD2;
  Standard_Boolean anIsSameV2d, anIsSameV, anIsFound, anIsOut, anIsNotPassed;
  TopoDS_Vertex aVb;
  TopoDS_Edge aEOutb;
  BOPAlgo_ListIteratorOfListOfEdgeInfo anIt;
  //
  aTol=1.e-7;
  //
  // append block
  //
  // Do not escape through edge from which you enter 
  aNb=aLS.Length();
  if (aNb==1) {
    const TopoDS_Shape& anEPrev=aLS(aNb);
    if (anEPrev.IsSame(aEOuta)) {
      return;
    }
  }
  //
  anEdgeInfo.SetPassed(Standard_True);
  aLS.Append(aEOuta);
  aVertVa.Append(aVa);
  
  TopoDS_Vertex pVa=aVa;
  pVa.Orientation(TopAbs_FORWARD);
  gp_Pnt2d aPa=Coord2d(pVa, aEOuta, myFace);
  aCoordVa.Append(aPa);
  
  GetNextVertex (pVa, aEOuta, aVb);

  gp_Pnt2d aPb=Coord2d(aVb, aEOuta, myFace);

  const BOPAlgo_ListOfEdgeInfo& aLEInfoVb=mySmartMap.FindFromKey(aVb);
  //
  aTol=2.*Tolerance2D(aVb, aGAS);
  aTol2=10.*aTol*aTol;

  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(aEOuta, aV1, aV2);
  Standard_Boolean bIsClosedEdge = aV1.IsNull() || aV2.IsNull() || aV1.IsSame(aV2);
  Standard_Boolean bIsDegenerated = BRep_Tool::Degenerated(aEOuta);
  Standard_Boolean bIsSeam = BRep_Tool::IsClosed(aEOuta, myFace);

  anIt.Initialize(aLEInfoVb);
  for (; anIt.More(); anIt.Next()) {
    const BOPAlgo_EdgeInfo& anEI = anIt.Value();
    const TopoDS_Edge& aE = anEI.Edge();
    bIsDegenerated = bIsDegenerated || BRep_Tool::Degenerated(aE);
    bIsSeam = bIsSeam || BRep_Tool::IsClosed(aE, myFace);
    aV1.Nullify();
    aV2.Nullify();
    TopExp::Vertices(aE, aV1, aV2);
    bIsClosedEdge = bIsClosedEdge || aV1.IsNull() || aV2.IsNull() || aV1.IsSame(aV2);
  }
  //
  aNb=aLS.Length();
  if (aNb>0) {
    //
    BOPCol_ListOfShape aBuf;
    //
    for (i=aNb; i>0; --i) {
      const TopoDS_Shape& aVPrev=aVertVa(i);
      const gp_Pnt2d& aPaPrev=aCoordVa(i);
      const TopoDS_Shape& aEPrev=aLS(i);

      aBuf.Append(aEPrev);

      anIsSameV=aVPrev.IsSame(aVb);
      anIsSameV2d=Standard_False;

      if (anIsSameV) {
        anIsSameV2d = Standard_True;
        //
        aD2=aPaPrev.SquareDistance(aPb);
        anIsSameV2d =aD2<aTol2;
        if(anIsSameV2d && 
           (bIsDegenerated || bIsSeam || bIsClosedEdge)) {
          Standard_Real udist = fabs(aPaPrev.X() - aPb.X());
          Standard_Real vdist = fabs(aPaPrev.Y() - aPb.Y());
          Standard_Real aTolU = 2. * UTolerance2D(aVb, aGAS);
          Standard_Real aTolV = 2. * VTolerance2D(aVb, aGAS);
          //
          if((udist > aTolU) ||
             (vdist > aTolV)) {
            anIsSameV2d = Standard_False;
          }
        }
      }//if (anIsSameV) {
      //
      if (anIsSameV && anIsSameV2d) {
        Standard_Integer iPriz;
        iPriz=1;
        if (aBuf.Extent()==2) {
          if(aBuf.First().IsSame(aBuf.Last())) {
            iPriz=0;
          }
        }
        if (iPriz) {
          TopoDS_Wire aW;
          BOPAlgo_WireSplitter::MakeWire(aBuf, aW);
          aCB.ChangeLoops().Append(aW);
        }
        //
        aNbj=i-1;
        if (aNbj<1) {
          //
          aLS.Clear();
          aVertVa.Clear();
          aCoordVa.Clear();
          //
          return;
        }
        //
        BOPCol_SequenceOfShape aLSt, aVertVat;
        BOPCol_SequenceOfPnt2d aCoordVat;
        //
        aVb=(*(TopoDS_Vertex *)(&aVertVa(i))); 
        //
        for (j=1; j<=aNbj; ++j) {
          aLSt.Append(aLS(j));
          aVertVat.Append(aVertVa(j));
          aCoordVat.Append(aCoordVa(j));
        }
        //
        aLS.Clear();
        aVertVa.Clear();
        aCoordVa.Clear();
        
        aLS=aLSt;
        aVertVa=aVertVat;
        aCoordVa=aCoordVat;
        //
        break;
      }
    }
  }
  //
  aTol2D=2.*Tolerance2D(aVb, aGAS);
  aTol2D2=1000.*aTol2D*aTol2D;//100.*aTol2D*aTol2D;
  //
  // anAngleIn in Vb from edge aEOuta
  const BOPAlgo_ListOfEdgeInfo& aLEInfo=mySmartMap.FindFromKey(aVb);
  //
  anAngleIn=AngleIn(aEOuta, aLEInfo);
  BOPCol_SequenceOfReal aRecomputedAngles;

  Standard_Boolean bRecomputeAngle = 
    RecomputeAngles(aLEInfo, myFace, aPb, aVb, aGAS, aEOuta, 
                    (bIsDegenerated || bIsSeam || bIsClosedEdge),
                    aTol2D, aRecomputedAngles);

  //
  // aEOutb
  BOPAlgo_EdgeInfo *pEdgeInfo=NULL;
  //
  aMinAngle=100.;
  anIsFound=Standard_False;
  Standard_Integer aCurIndexE = 0;
  anIt.Initialize(aLEInfo);
  for (; anIt.More(); anIt.Next()) {
    BOPAlgo_EdgeInfo& anEI=anIt.ChangeValue();
    const TopoDS_Edge& aE=anEI.Edge();
    anIsOut=!anEI.IsIn();
    anIsNotPassed=!anEI.Passed();
    
    if (anIsOut && anIsNotPassed) {
      aCurIndexE++;
      //
      // Is there one way to go out of the vertex 
      // we have to use it only.
      Standard_Integer iCnt;
      iCnt=NbWaysOut (aLEInfo);
      //
      if (!iCnt) {
        // no way to go . (Error)
        return ;
      }
      //
      if (iCnt==1) {
        // the one and only way to go out .
        pEdgeInfo=&anEI;
        anIsFound=Standard_True;
        break;
      }
      //
      // Look for minimal angle and make the choice.
      gp_Pnt2d aP2Dx;
      //
      aP2Dx=Coord2dVf(aE, myFace);
      //
      aD2=aP2Dx.SquareDistance(aPb);
      if (aD2 > aTol2D2){
        continue;
      }
      //
      //
      anAngleOut=anEI.Angle();
      //
      if(bRecomputeAngle) {
        if(aCurIndexE <= aRecomputedAngles.Length()) {
          anAngleOut = aRecomputedAngles.Value(aCurIndexE);
        }
      }

      anAngle=ClockWiseAngle(anAngleIn, anAngleOut);
      if (anAngle < aMinAngle) {
        aMinAngle=anAngle;
        pEdgeInfo=&anEI;
        anIsFound=Standard_True;
      }
    }
  } // for (; anIt.More(); anIt.Next()) 
  //
  if (!anIsFound) {
    // no way to go . (Error)
    return;
  }
  
  aEOutb=pEdgeInfo->Edge();
  //
  Path (aGAS, myFace, aVb, aEOutb, *pEdgeInfo, aLS, 
        aVertVa, aCoordVa, aCB, mySmartMap);
}
//=======================================================================
// function:  ClockWiseAngle
// purpose:
//=======================================================================
 Standard_Real ClockWiseAngle(const Standard_Real aAngleIn,
                              const Standard_Real aAngleOut)
{
  Standard_Real aTwoPi=M_PI+M_PI;
  Standard_Real dA, A1, A2, AIn, AOut ;

  AIn=aAngleIn;
  AOut=aAngleOut;
  if (AIn >= aTwoPi) {
    AIn=AIn-aTwoPi;
  }
  
  if (AOut >= aTwoPi) {
    AOut=AOut-aTwoPi;
  }

  A1=AIn+M_PI;
  
  if (A1 >= aTwoPi) {
    A1=A1-aTwoPi;
  }
  
  A2=AOut;
  
  dA=A1-A2;
  if (dA <= 0.) {
    dA=aTwoPi+dA;
  }
  //xx
  //else if (dA <= 1.e-15) {
  else if (dA <= 1.e-14) {
    dA=aTwoPi;
  }
  return dA;
}
//=======================================================================
// function:  Coord2d
// purpose:
//=======================================================================
 gp_Pnt2d Coord2d (const TopoDS_Vertex& aV1,
                   const TopoDS_Edge& aE1,
                   const TopoDS_Face& aF)
{
  Standard_Real aT, aFirst, aLast;
  Handle(Geom2d_Curve) aC2D;
  gp_Pnt2d aP2D1;
  //
  aT=BRep_Tool::Parameter (aV1, aE1, aF);
  aC2D=BRep_Tool::CurveOnSurface(aE1, aF, aFirst, aLast);
  aC2D->D0 (aT, aP2D1);
  //
  return aP2D1;
}
//=======================================================================
// function:  Coord2dVf
// purpose:
//=======================================================================
 gp_Pnt2d Coord2dVf (const TopoDS_Edge& aE,
                     const TopoDS_Face& aF)
{
  Standard_Real aCoord=99.;
  gp_Pnt2d aP2D1(aCoord, aCoord);
  TopoDS_Iterator aIt;
  //
  aIt.Initialize(aE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aVx=aIt.Value();
    if (aVx.Orientation()==TopAbs_FORWARD) {
      
      const TopoDS_Vertex& aVxx=(*(TopoDS_Vertex *)(&aVx));// TopoDS::Vertex(aVx);
      aP2D1=Coord2d(aVxx, aE, aF);
      return aP2D1;
    }
  }
  return aP2D1;
}

//=======================================================================
// function: NbWaysOut
// purpose: 
//=======================================================================
Standard_Integer NbWaysOut(const BOPAlgo_ListOfEdgeInfo& aLEInfo)
{
  Standard_Boolean bIsOut, bIsNotPassed;
  Standard_Integer iCnt=0;
  BOPAlgo_ListIteratorOfListOfEdgeInfo anIt;
  //
  anIt.Initialize(aLEInfo);
  for (; anIt.More(); anIt.Next()) {
    const BOPAlgo_EdgeInfo& anEI=anIt.Value();
    //
    bIsOut=!anEI.IsIn();
    bIsNotPassed=!anEI.Passed();
    if (bIsOut && bIsNotPassed) {
      iCnt++;
    }
  }
  return iCnt;
}

//=======================================================================
// function:  AngleIn
// purpose:
//=======================================================================
 Standard_Real AngleIn(const TopoDS_Edge& aEIn,
                       const BOPAlgo_ListOfEdgeInfo& aLEInfo)
{
  Standard_Real anAngleIn;
  Standard_Boolean anIsIn;
  BOPAlgo_ListIteratorOfListOfEdgeInfo anIt;

  anIt.Initialize(aLEInfo);
  for (; anIt.More(); anIt.Next()) {
    const BOPAlgo_EdgeInfo& anEdgeInfo=anIt.Value();
    const TopoDS_Edge& aE=anEdgeInfo.Edge();
    anIsIn=anEdgeInfo.IsIn();
    //
    if (anIsIn && aE==aEIn) {
      anAngleIn=anEdgeInfo.Angle();
      return anAngleIn;
    }
  }
  anAngleIn=0.;
  return anAngleIn;
}
//=======================================================================
// function: GetNextVertex
// purpose: 
//=======================================================================
 void GetNextVertex(const TopoDS_Vertex& aV,
                    const TopoDS_Edge& aE,
                    TopoDS_Vertex& aV1)
{
  TopoDS_Iterator aIt;
  //
  aIt.Initialize(aE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aVx=aIt.Value();
    if (!aVx.IsEqual(aV)) {
      aV1=(*(TopoDS_Vertex *)(&aVx)); 
      return ;
    }
  }
  aV1=aV;
}
//=======================================================================
// function: Angle2D
// purpose: 
//=======================================================================
  Standard_Real Angle2D (const TopoDS_Vertex& aV,
                         const TopoDS_Edge& anEdge,
                         const TopoDS_Face& myFace,
                         const GeomAdaptor_Surface& aGAS,
                         const Standard_Boolean aFlag)
{
  Standard_Real aFirst, aLast, aToler, dt, aTV, aTV1, anAngle, aTX;
  gp_Pnt2d aPV, aPV1;
  gp_Vec2d aV2D;
  Handle(Geom2d_Curve) aC2D;
  //
  aTV=BRep_Tool::Parameter (aV, anEdge, myFace);
  if (Precision::IsInfinite(aTV)) {
    return 0.;
  }
  //
  BOPTools_AlgoTools2D::CurveOnSurface (anEdge, myFace, aC2D, 
                                    aFirst, aLast, aToler);
  dt=2.*Tolerance2D(aV, aGAS);
  //
  //for case chl/927/r9
  aTX=0.05*(aLast - aFirst);//aTX=0.25*(aLast - aFirst);
  if (aTX < 5.e-5) {
    aTX = 5.e-5;
  }
  if(dt > aTX) {
    // to save direction of the curve as much as it possible
    // in the case of big tolerances
    dt = aTX; 
  }
  //
  if (fabs (aTV-aFirst) < fabs(aTV - aLast)) {
    aTV1=aTV + dt;
  }
  else {
    aTV1=aTV - dt;
  }
  //
  aC2D->D0 (aTV1, aPV1);
  aC2D->D0 (aTV, aPV);
  //
  if (aFlag) {//IN
    gp_Vec2d aV2DIn(aPV1, aPV);
    aV2D=aV2DIn;
  }
  else {
    gp_Vec2d aV2DOut(aPV, aPV1);
    aV2D=aV2DOut;
  }
  //
  gp_Dir2d aDir2D(aV2D);
  anAngle=Angle(aDir2D);
  //
  return anAngle;
}
//=======================================================================
// function: Angle
// purpose: 
//=======================================================================
Standard_Real Angle (const gp_Dir2d& aDir2D)
{
  gp_Dir2d aRefDir(1., 0.);
  Standard_Real anAngle;
  
  anAngle = aRefDir.Angle(aDir2D);
  if (anAngle < 0.)
    anAngle += M_PI + M_PI;
  return anAngle;
}
//=======================================================================
// function:  Tolerance2D
// purpose:
//=======================================================================
 Standard_Real Tolerance2D (const TopoDS_Vertex& aV,
                            const GeomAdaptor_Surface& aGAS)	     
{
  Standard_Real aTol2D, anUr, aVr, aTolV3D;
  GeomAbs_SurfaceType aType;
  //
  aType=aGAS.GetType();
  aTolV3D=BRep_Tool::Tolerance(aV);

  anUr=aGAS.UResolution(aTolV3D);
  aVr =aGAS.VResolution(aTolV3D);
  aTol2D=(aVr>anUr) ? aVr : anUr;
  //
  if (aTol2D < aTolV3D) {
    aTol2D=aTolV3D;
  }
  if (aType==GeomAbs_BSplineSurface) {
    aTol2D=1.1*aTol2D;
  }
  //
  return aTol2D;
}
//=======================================================================
// function: BuildPCurvesForPlane
// purpose: 
//=======================================================================
  void BuildPCurveForPlane (const BOPCol_ListOfShape myEdges,
                            const TopoDS_Face& myFace)
{
  TopLoc_Location aLoc;
  Handle(Geom2d_Curve) aC2D;
  Handle(Geom_Plane) aGP;
  Handle(Geom_RectangularTrimmedSurface) aGRTS;
  //
  const Handle(Geom_Surface)& aS = BRep_Tool::Surface(myFace, aLoc);
  aGRTS=Handle(Geom_RectangularTrimmedSurface)::DownCast(aS);
  if(!aGRTS.IsNull()){
    aGP=Handle(Geom_Plane)::DownCast(aGRTS->BasisSurface());
    }    
  else {
    aGP=Handle(Geom_Plane)::DownCast(aS);
  }
  //
  if (aGP.IsNull()) {
    return;
  }
  //
  Standard_Real aTolE;
  BOPCol_ListIteratorOfListOfShape aIt;
  BRep_Builder aBB;
  //
  aIt.Initialize(myEdges);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)&aIt.Value());
    BOPTools_AlgoTools2D::CurveOnSurface(aE, myFace, aC2D, aTolE);
    aBB.UpdateEdge(aE, aC2D, myFace, aTolE);
  }
}
//=======================================================================
//function : UTolerance2D
//purpose  : 
//=======================================================================
Standard_Real UTolerance2D (const TopoDS_Vertex& aV,
                            const GeomAdaptor_Surface& aGAS)
{
  const Standard_Real aTolV3D = BRep_Tool::Tolerance(aV);
  const Standard_Real anUr = aGAS.UResolution(aTolV3D);
  //
  return anUr;
}

//=======================================================================
//function : VTolerance2D
//purpose  : 
//=======================================================================
Standard_Real VTolerance2D (const TopoDS_Vertex& aV,
                            const GeomAdaptor_Surface& aGAS)
{
  const Standard_Real aTolV3D = BRep_Tool::Tolerance(aV);
  const Standard_Real anVr = aGAS.VResolution(aTolV3D);
  //
  return anVr;
}

//=======================================================================
// function: RecomputeAngles
// purpose: 
//=======================================================================
Standard_Boolean RecomputeAngles(const BOPAlgo_ListOfEdgeInfo& aLEInfo, 
                                 const TopoDS_Face&            theFace, 
                                 const gp_Pnt2d&               thePb, 
                                 const TopoDS_Vertex&          theVb,
                                 const GeomAdaptor_Surface&    theGAS,
                                 const TopoDS_Edge&            theEOuta, 
                                 const Standard_Boolean&       bIsClosed,
                                 const Standard_Real&          theTol2D,
                                 BOPCol_SequenceOfReal&        theRecomputedAngles)
{
  Standard_Boolean bRecomputeAngle = Standard_False;
  BOPAlgo_ListIteratorOfListOfEdgeInfo anIt;
  anIt.Initialize(aLEInfo);

  for (; anIt.More(); anIt.Next()) {
    const BOPAlgo_EdgeInfo& anEI=anIt.Value();
    const TopoDS_Edge& aE=anEI.Edge();
    Standard_Boolean anIsOut=!anEI.IsIn();
    Standard_Boolean anIsNotPassed=!anEI.Passed();
    
    if (anIsOut && anIsNotPassed) {
      theRecomputedAngles.Append(anEI.Angle());
      Standard_Integer acurindex = theRecomputedAngles.Length();

      Standard_Boolean bRecomputeAngleLocal = Standard_False;
      TopExp_Explorer anExp1(aE, TopAbs_VERTEX);

      for(; anExp1.More(); anExp1.Next()) {
        TopExp_Explorer anExp2(theEOuta, TopAbs_VERTEX);
        Standard_Boolean existsInEdge = Standard_False;
        
        for(; anExp2.More(); anExp2.Next()) {
          if(anExp1.Current().IsSame(anExp2.Current())) {
            existsInEdge = Standard_True;
            break;
          }
        }
        
        if(!existsInEdge) {
          bRecomputeAngleLocal = Standard_False;
          break;
        }
        bRecomputeAngleLocal = Standard_True;
      }
      bRecomputeAngle = bRecomputeAngle || bRecomputeAngleLocal;

      if(!bRecomputeAngle) {
        BOPAlgo_ListIteratorOfListOfEdgeInfo anIt2(aLEInfo);
        
        for(; anIt2.More(); anIt2.Next()) {
          const BOPAlgo_EdgeInfo& anEI2=anIt2.Value();
          const TopoDS_Edge& aE2=anEI2.Edge();
          
          if(aE2.IsSame(aE))
            continue;
          Standard_Boolean anIsOut2=!anEI2.IsIn();
          Standard_Boolean anIsNotPassed2=!anEI2.Passed();
          
          if (anIsOut2 && anIsNotPassed2) {
            anExp1.Init(aE, TopAbs_VERTEX);
            
            for(; anExp1.More(); anExp1.Next()) {
              TopExp_Explorer anExp2(aE2, TopAbs_VERTEX);
              Standard_Boolean existsInEdge = Standard_False;
              
              for(; anExp2.More(); anExp2.Next()) {
                if(anExp1.Current().IsSame(anExp2.Current())) {
                  existsInEdge = Standard_True;
                  break;
                }
              }
              
              if(!existsInEdge) {
                bRecomputeAngleLocal = Standard_False;
                break;
              }
              bRecomputeAngleLocal = Standard_True;
            }
            bRecomputeAngle = bRecomputeAngle || bRecomputeAngleLocal;
          }
        }
      }
      
      bRecomputeAngle = bRecomputeAngle || bRecomputeAngleLocal;

      if(bRecomputeAngle) {
        gp_Pnt2d aP2Dx;
        //
        aP2Dx=Coord2dVf(aE, theFace);
        Standard_Real aD = aP2Dx.Distance(thePb);
        
        TopoDS_Vertex aVf;
        TopExp_Explorer anExp(aE, TopAbs_VERTEX);
        
        for (; anExp.More(); anExp.Next()) {
          const TopoDS_Vertex& aVx=*(TopoDS_Vertex*)(&anExp.Current());
          if (aVx.Orientation()==TopAbs_FORWARD) {
            aVf = aVx;
          }
        }
        Standard_Boolean bIgnore = Standard_False;
        
        if(bIsClosed || aVf.IsNull() || !aVf.IsSame(theVb)) {
          bIgnore = (aD > theTol2D);
        }
        
        if(!bIgnore && (theTol2D > M_PI)) {
          Standard_Real udist = fabs(aP2Dx.X() - thePb.X());
          Standard_Real vdist = fabs(aP2Dx.Y() - thePb.Y());
          Standard_Real aTolU = 2. * UTolerance2D(theVb, theGAS);
          Standard_Real aTolV = 2. * VTolerance2D(theVb, theGAS);
          
          if((udist > aTolU) ||
             (vdist > aTolV)) {
            bIgnore = Standard_True;
          }
        }
        
        if((aD > Precision::Confusion()) && !bIgnore) {
          Standard_Real f1, l1;
          Handle(Geom2d_Curve) ac1 = BRep_Tool::CurveOnSurface(aE, theFace, f1, l1);
          
          Standard_Real aTV1 = BRep_Tool::Parameter (aVf, aE, theFace);
          Standard_Real aTV12 = 0.;
          Standard_Real dt1 = (l1 - f1) * 0.5;
          
          if (fabs (aTV1-f1) < fabs(aTV1 - l1)) {
            aTV12 = aTV1 + dt1;
          }
          else {
            aTV12 = aTV1 - dt1;
          }
          
          gp_Pnt2d aPointNew = ac1->Value(aTV12);
          gp_Vec2d aV2DOut(thePb, aPointNew);
          
          gp_Dir2d aDir2D(aV2DOut);
          Standard_Real anAngleOut = Angle(aDir2D);
          theRecomputedAngles.ChangeValue(acurindex) = anAngleOut;
        }
      }
    }
  }
  return bRecomputeAngle;
}
