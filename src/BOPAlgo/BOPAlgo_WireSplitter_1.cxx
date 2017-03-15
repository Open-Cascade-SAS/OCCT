// Created by: Peter KURNEV
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

#include <BOPAlgo_WireEdgeSet.hxx>
#include <BOPAlgo_WireSplitter.hxx>
#include <BOPCol_IndexedDataMapOfShapeInteger.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_SequenceOfPnt2d.hxx>
#include <BOPCol_SequenceOfReal.hxx>
#include <BOPCol_SequenceOfShape.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
typedef NCollection_DataMap \
  <TopoDS_Shape, Standard_Boolean, TopTools_ShapeMapHasher> \
  BOPCol_DataMapOfShapeBoolean; 
//

static
  Standard_Real Angle (const gp_Dir2d& aDir2D);

static
  Standard_Real Angle2D (const TopoDS_Vertex& aV,
                         const TopoDS_Edge& anEdge,
                         const TopoDS_Face& myFace,
                         const GeomAdaptor_Surface& aGAS,
                         const Standard_Boolean aFlag,
                         const Handle(IntTools_Context)& theContext);

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
             BOPAlgo_IndexedDataMapOfShapeListOfEdgeInfo& mySmartMap,
             BOPCol_DataMapOfShapeBoolean aVertMap);

static
  Standard_Real Angle (const gp_Dir2d& aDir2D);

static
  Standard_Real Tolerance2D (const TopoDS_Vertex& aV,
                             const GeomAdaptor_Surface& aGAS);



static
  Standard_Real UTolerance2D (const TopoDS_Vertex& aV,
                              const GeomAdaptor_Surface& aGAS);
static
  Standard_Real VTolerance2D (const TopoDS_Vertex& aV,
                              const GeomAdaptor_Surface& aGAS);

static
  void RefineAngles(const TopoDS_Face& myFace,
                    const BOPCol_ListOfShape&,
                    BOPAlgo_IndexedDataMapOfShapeListOfEdgeInfo&,
                    const Handle(IntTools_Context)&);


static
  void RefineAngles(const TopoDS_Vertex& ,
                  const TopoDS_Face& ,
                  const BOPCol_MapOfShape& ,
                  BOPAlgo_ListOfEdgeInfo&,
                  const Handle(IntTools_Context)&);

static
  Standard_Boolean RefineAngle2D(const TopoDS_Vertex& ,
                                 const TopoDS_Edge& ,
                                 const TopoDS_Face& ,
                                 const Standard_Real ,
                                 const Standard_Real ,
                                 const Standard_Real, 
                                 Standard_Real& ,
                                 const Handle(IntTools_Context)& );

//=======================================================================
//function : SplitBlock
//purpose  : 
//=======================================================================
void BOPAlgo_WireSplitter::SplitBlock(const TopoDS_Face& myFace,
                                      BOPTools_ConnexityBlock& aCB,
                                      const Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bNothingToDo, bIsClosed, bIsIN;
  Standard_Integer aIx, aNb, i, aCntIn, aCntOut;
  Standard_Real aAngle;
  TopAbs_Orientation aOr;
  TopoDS_Iterator aItS;
  TopoDS_Vertex aVV;
  TopoDS_Shape aV1;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPAlgo_ListIteratorOfListOfEdgeInfo aItLEI;
  //
  BOPAlgo_IndexedDataMapOfShapeListOfEdgeInfo mySmartMap(100);
  BOPCol_DataMapOfShapeBoolean aVertMap;
  //
  const BOPCol_ListOfShape& myEdges=aCB.Shapes();
  //
  // 1.Filling mySmartMap
  aIt.Initialize(myEdges);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)&aIt.Value());
    if (!BOPTools_AlgoTools2D::HasCurveOnSurface (aE, myFace)) {
      continue;
    }
    //
    bIsClosed = BRep_Tool::Degenerated(aE) || 
                BRep_Tool::IsClosed(aE, myFace);
    //
    aItS.Initialize(aE);
    for(i = 0; aItS.More(); aItS.Next(), ++i) {
      const TopoDS_Shape& aV = aItS.Value();
      aIx = mySmartMap.FindIndex(aV);
      if (!aIx) {
        BOPAlgo_ListOfEdgeInfo aLEIx;
        aIx = mySmartMap.Add(aV, aLEIx);
      }
      //
      BOPAlgo_ListOfEdgeInfo& aLEI = mySmartMap(aIx);
      BOPAlgo_EdgeInfo aEI;
      //
      aEI.SetEdge(aE);
      aOr = aV.Orientation();
      bIsIN = (aOr == TopAbs_REVERSED);
      aEI.SetInFlag(bIsIN);
      aLEI.Append(aEI);
      //
      if (!i) {
        aV1 = aV;
      } else {
        bIsClosed = bIsClosed || aV1.IsSame(aV);
      }
      //
      if (aVertMap.IsBound(aV)) {
        if (bIsClosed) {
          aVertMap.ChangeFind(aV) = bIsClosed;
        }
      } else {
        aVertMap.Bind(aV, bIsClosed);
      }
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
    BOPCol_IndexedDataMapOfShapeListOfShape aMapEE(100);
    aNbE=myEdges.Extent();
    //
    aIt.Initialize(myEdges);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE = aIt.Value();
      if (!aMapEE.Contains(aE)) {
        BOPCol_ListOfShape aLEx;
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
    return;
  }
  //
  // 3. Angles in mySmartMap
  const BRepAdaptor_Surface& aBAS = theContext->SurfaceAdaptor(myFace);
  const GeomAdaptor_Surface& aGAS=aBAS.Surface();
  //
  for (i=1; i<=aNb; i++) {
    const TopoDS_Vertex& aV = (*(TopoDS_Vertex *)(&mySmartMap.FindKey(i))); 
    const BOPAlgo_ListOfEdgeInfo& aLEI= mySmartMap(i);
    aItLEI.Initialize(aLEI);
    for (; aItLEI.More(); aItLEI.Next()) {
      BOPAlgo_EdgeInfo& aEI=aItLEI.ChangeValue();
      const TopoDS_Edge& aE=aEI.Edge();
      //
      aVV = aV;
      bIsIN = aEI.IsIn();
      aOr = bIsIN ? TopAbs_REVERSED : TopAbs_FORWARD;
      aVV.Orientation(aOr);
      aAngle = Angle2D(aVV, aE, myFace, aGAS, bIsIN, theContext);
      aEI.SetAngle(aAngle);
    }
  }// for (i=1; i<=aNb; i++) {
  //
  //Theme: The treatment p-curves convergent in node.
  //The refining the angles of p-curves taking into account 
  //bounding curves if exist. 
  RefineAngles(myFace, myEdges, mySmartMap, theContext);
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
             aVertVa, aCoordVa, aCB, mySmartMap, aVertMap);
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
           const TopoDS_Vertex& aVFirst,
           const TopoDS_Edge& aEFirst,
           BOPAlgo_EdgeInfo& aEIFirst,
           BOPCol_SequenceOfShape& aLS,
           BOPCol_SequenceOfShape& aVertVa,
           BOPCol_SequenceOfPnt2d& aCoordVa,
           BOPTools_ConnexityBlock& aCB,
           BOPAlgo_IndexedDataMapOfShapeListOfEdgeInfo& mySmartMap,
           BOPCol_DataMapOfShapeBoolean aVertMap)
     
{
  Standard_Integer i, j, aNb, aNbj;
  Standard_Real anAngleIn, anAngleOut, anAngle, aMinAngle;
  Standard_Real aTol2D, aTol2D2, aD2, aTwoPI;
  Standard_Boolean anIsSameV2d, anIsSameV, anIsFound, anIsOut, anIsNotPassed;
  Standard_Boolean bIsClosed;
  TopoDS_Vertex aVa, aVb;
  TopoDS_Edge aEOuta;
  BOPAlgo_ListIteratorOfListOfEdgeInfo anIt;
  Standard_Real eps = Epsilon(1.);
  //
  aVa = aVFirst;
  aEOuta = aEFirst;
  BOPAlgo_EdgeInfo* anEdgeInfo = &aEIFirst;
  //
  aTwoPI = M_PI + M_PI;
  //
  // append block
  //
  for (;;) {
    // Do not escape through edge from which you enter 
    aNb=aLS.Length();
    if (aNb==1) {
      const TopoDS_Shape& anEPrev=aLS(aNb);
      if (anEPrev.IsSame(aEOuta)) {
        return;
      }
    }
    //
    anEdgeInfo->SetPassed(Standard_True);
    aLS.Append(aEOuta);
    aVertVa.Append(aVa);
    
    TopoDS_Vertex pVa=aVa;
    pVa.Orientation(TopAbs_FORWARD);
    gp_Pnt2d aPa=Coord2d(pVa, aEOuta, myFace);
    aCoordVa.Append(aPa);
    
    GetNextVertex (pVa, aEOuta, aVb);
    
    gp_Pnt2d aPb=Coord2d(aVb, aEOuta, myFace);
    
    const BOPAlgo_ListOfEdgeInfo& aLEInfo=mySmartMap.FindFromKey(aVb);
    //
    aTol2D = 2.*Tolerance2D(aVb, aGAS);
    aTol2D2 = aTol2D * aTol2D;
    //
    bIsClosed = aVertMap.Find(aVb);
    {
      BOPCol_ListOfShape aBuf;
      Standard_Boolean bHasEdge = Standard_False;
      //
      aNb = aLS.Length();
      for (i = aNb; i>0; --i) {
        const TopoDS_Shape& aVPrev=aVertVa(i);
        const gp_Pnt2d& aPaPrev=aCoordVa(i);
        const TopoDS_Edge& aEPrev = TopoDS::Edge(aLS(i));
        //
        aBuf.Append(aEPrev);
        if (!bHasEdge) {
          bHasEdge = !BRep_Tool::Degenerated(aEPrev);
          // do not create wire from degenerated edges only
          if (!bHasEdge) {
            continue;
          }
        }
        //
        anIsSameV = aVPrev.IsSame(aVb);
        anIsSameV2d = anIsSameV;
        if (anIsSameV) {
          if(bIsClosed) {
            aD2 = aPaPrev.SquareDistance(aPb);
            anIsSameV2d = aD2 < aTol2D2;
            if (anIsSameV2d) {
              Standard_Real udist = fabs(aPaPrev.X() - aPb.X());
              Standard_Real vdist = fabs(aPaPrev.Y() - aPb.Y());
              Standard_Real aTolU = 2.*UTolerance2D(aVb, aGAS);
              Standard_Real aTolV = 2.*VTolerance2D(aVb, aGAS);
              //
              if((udist > aTolU) || (vdist > aTolV)) {
                anIsSameV2d = Standard_False;
              }
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
    // aEOutb
    BOPAlgo_EdgeInfo *pEdgeInfo=NULL;
    //
    anAngleIn = AngleIn(aEOuta, aLEInfo);
    aMinAngle = 100.;
    anIsFound = Standard_False;
    Standard_Integer iCnt = NbWaysOut(aLEInfo);
    Standard_Integer aCurIndexE = 0;
    anIt.Initialize(aLEInfo);
    for (; anIt.More(); anIt.Next()) {
      BOPAlgo_EdgeInfo& anEI=anIt.ChangeValue();
      const TopoDS_Edge& aE=anEI.Edge();
      anIsOut=!anEI.IsIn();
      anIsNotPassed=!anEI.Passed();
      //
      if (anIsOut && anIsNotPassed) {
        aCurIndexE++;
        //
        // Is there one way to go out of the vertex 
        // we have to use it only.
        //
        if (!iCnt) {
          // no way to go . (Error)
          return;
        }
        //
        if (iCnt==1) {
          // the one and only way to go out .
          pEdgeInfo=&anEI;
          anIsFound=Standard_True;
          break;
        }
        //
        if (aE.IsSame(aEOuta)) {
          anAngle = aTwoPI;
        } else {
          //check 2d distance
          if (bIsClosed) {
            gp_Pnt2d aP2Dx;
            //
            aP2Dx = Coord2dVf(aE, myFace);
            //
            aD2 = aP2Dx.SquareDistance(aPb);
            if (aD2 > aTol2D2){
              continue;
            }
          }
          //
          // Look for minimal angle and make the choice.
          anAngleOut=anEI.Angle();
          anAngle=ClockWiseAngle(anAngleIn, anAngleOut);
        }
        if (anAngle < aMinAngle - eps) {
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
    //
    aVa = aVb;
    aEOuta = pEdgeInfo->Edge();
    anEdgeInfo = pEdgeInfo;
  }
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
                         const Standard_Boolean bIsIN,
                         const Handle(IntTools_Context)& theContext)
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
                                        aFirst, aLast, aToler, theContext);
  Standard_Real tol2d =2.*Tolerance2D(aV, aGAS);
  //
  GeomAbs_CurveType aType;
  Geom2dAdaptor_Curve aGAC2D(aC2D);
  //
  dt = Max(aGAC2D.Resolution(tol2d), Precision::PConfusion());
  //
  aType=aGAC2D.GetType();
  if (aType != GeomAbs_Line ) 
  {
    Geom2dLProp_CLProps2d LProp(aC2D,  aTV, 2, Precision::PConfusion());
    if(LProp.IsTangentDefined())
    {
      Standard_Real R = LProp.Curvature();
      if(R > Precision::PConfusion())
      {
        R = 1./R;
        Standard_Real cosphi = R / (R + tol2d);
        dt = Max(dt, ACos(cosphi)); //to avoid small dt for big R.
      }
    }
  }
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

  if (fabs (aTV-aFirst) < fabs(aTV - aLast)) {
    aTV1=aTV + dt;
  }
  else {
    aTV1=aTV - dt;
  }
  //
  aGAC2D.D0 (aTV1, aPV1);
  aGAC2D.D0 (aTV, aPV);
  //
  aV2D = bIsIN ? gp_Vec2d(aPV1, aPV) : gp_Vec2d(aPV, aPV1);
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
//function : RefineAngles
//purpose  : 
//=======================================================================
void RefineAngles(const TopoDS_Face& myFace,
                  const BOPCol_ListOfShape& myEdges,
                  BOPAlgo_IndexedDataMapOfShapeListOfEdgeInfo& mySmartMap,
                  const Handle(IntTools_Context)& theContext)
{
  Standard_Integer aNb, i;
  BOPCol_IndexedDataMapOfShapeInteger aMSI;
  BOPCol_MapOfShape aMBE;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  // 1. Boundary Edges
  aIt.Initialize(myEdges);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE=aIt.Value();
    if(aMSI.Contains(aE)) {
      Standard_Integer& iCnt = aMSI.ChangeFromKey(aE);
      ++iCnt;
    }
    else {
      Standard_Integer iCnt = 1;
      aMSI.Add(aE, iCnt);
    }
  }
  //
  aNb = aMSI.Extent();
  for (i = 1; i <= aNb; ++i) {
    Standard_Integer iCnt = aMSI(i);
    if (iCnt == 1) {
      const TopoDS_Shape& aE = aMSI.FindKey(i);
      aMBE.Add(aE);
    }
  }
  //
  aMSI.Clear();
  //
  aNb = mySmartMap.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Vertex& aV=*((TopoDS_Vertex*)&mySmartMap.FindKey(i)); 
    BOPAlgo_ListOfEdgeInfo& aLEI=mySmartMap(i);
    //
    RefineAngles(aV, myFace, aMBE, aLEI, theContext);
  }
}
//=======================================================================
typedef NCollection_DataMap \
  <TopoDS_Shape, Standard_Real, TopTools_ShapeMapHasher> \
   BOPCol_DataMapOfShapeReal; 
typedef BOPCol_DataMapOfShapeReal::Iterator \
  BOPCol_DataMapIteratorOfDataMapOfShapeReal; 
//
//=======================================================================
//function : RefineAngles
//purpose  : 
//=======================================================================
void RefineAngles(const TopoDS_Vertex& aV,
                  const TopoDS_Face& myFace,
                  const BOPCol_MapOfShape& aMBE,
                  BOPAlgo_ListOfEdgeInfo& aLEI,
                  const Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bIsIn, bIsBoundary, bRefined; 
  Standard_Integer iCntBnd, iCntInt;
  Standard_Real aA, aA1, aA2;
  BOPCol_DataMapOfShapeReal aDMSR;
  BOPAlgo_ListIteratorOfListOfEdgeInfo aItLEI;
  //
  aA1=0.;  // angle of outgoing edge
  aA2=0.;  // angle of incoming edge
  iCntBnd=0;
  iCntInt=0;
  aItLEI.Initialize(aLEI);
  for (; aItLEI.More(); aItLEI.Next()) {
    BOPAlgo_EdgeInfo& aEI=aItLEI.ChangeValue();
    const TopoDS_Edge& aE=aEI.Edge();
    bIsIn=aEI.IsIn();
    aA=aEI.Angle();
    //
    if (aMBE.Contains(aE)) {
      ++iCntBnd;
      if (!bIsIn) {
        aA1=aA;
      }
      else {
        aA2=aA;
      }
    }
    else {
      ++iCntInt;
    }
  }
  //
  if (iCntBnd!=2) {
    return;
  }
  //
  Standard_Real aDelta = ClockWiseAngle(aA2, aA1);
  aItLEI.Initialize(aLEI);
  for (; aItLEI.More(); aItLEI.Next()) {
    BOPAlgo_EdgeInfo& aEI=aItLEI.ChangeValue();
    const TopoDS_Edge& aE=aEI.Edge();
    //
    bIsBoundary=aMBE.Contains(aE);
    bIsIn=aEI.IsIn();
    if (bIsBoundary || bIsIn) {
      continue;
    }
    //
    aA=aEI.Angle();
    Standard_Real aDA = ClockWiseAngle(aA2, aA);
    if (aDA < aDelta) {
      continue;  // already inside
    }
    //
    bRefined=RefineAngle2D(aV, aE, myFace, aA1, aA2, aDelta, aA, theContext);
    if (bRefined) {
      aDMSR.Bind(aE, aA);
    }
    else if (iCntInt == 2) {
      aA = (aA <= aA1) ? (aA1 + Precision::Angular()) :
                         (aA2 - Precision::Angular());
      aDMSR.Bind(aE, aA);
    }
  }
  
  if (aDMSR.IsEmpty()) {
    return;
  }
  //
  // update Angles
  aItLEI.Initialize(aLEI);
  for (; aItLEI.More(); aItLEI.Next()) {
    BOPAlgo_EdgeInfo& aEI=aItLEI.ChangeValue();
    const TopoDS_Edge& aE=aEI.Edge();
    //
    bIsIn=aEI.IsIn();
    if (!aDMSR.IsBound(aE)) {
      continue;
    }
    //
    aA=aDMSR.Find(aE);
    if (bIsIn) {
      aA=aA+M_PI;
    }
    //
    aEI.SetAngle(aA);
  }
}
//=======================================================================
//function : RefineAngle2D
//purpose  : 
//=======================================================================
Standard_Boolean RefineAngle2D(const TopoDS_Vertex& aV,
                               const TopoDS_Edge& aE,
                               const TopoDS_Face& myFace,
                               const Standard_Real aA1,
                               const Standard_Real aA2,
                               const Standard_Real aDelta,
                               Standard_Real& aA,
                               const Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bRet;
  Standard_Integer i, j, aNbP;
  Standard_Real aTV, aTol, aT1, aT2, dT, aAngle, aT, aTOp;
  Standard_Real aTolInt, aAi, aXi, aYi, aT1j, aT2j, aT1max, aT2max, aCf; 
  gp_Pnt2d aPV, aP, aP1, aP2;
  Handle(Geom2d_Curve) aC2D;
  Handle(Geom2d_Line) aLi;
  Geom2dAdaptor_Curve aGAC1, aGAC2;
  Geom2dInt_GInter aGInter;
  IntRes2d_Domain aDomain1, aDomain2;
  //
  bRet=Standard_True;
  aCf=0.01;
  aTolInt=1.e-10;  
  //
  BOPTools_AlgoTools2D::CurveOnSurface(aE, myFace, aC2D, aT1, aT2, aTol, theContext);
  aGAC1.Load(aC2D, aT1, aT2);
  //
  aTV=BRep_Tool::Parameter (aV, aE, myFace);
  aGAC1.D0(aTV, aPV);
  //
  aTOp = (fabs(aTV-aT1) < fabs(aTV-aT2)) ? aT2 : aT1;
  //
  const Standard_Real MaxDT = 0.3 * (aT2 - aT1);
  aGAC1.D0(aT1, aP1);
  aGAC1.D0(aT2, aP2);
  aDomain1.SetValues(aP1, aT1, aTolInt, aP2, aT2, aTolInt);
  //
  for (i=0; i<2; ++i) {
    aAi=(!i) ? aA1 : (aA2 + M_PI);
    aXi=cos(aAi);
    aYi=sin(aAi);
    gp_Dir2d aDiri(aXi, aYi);
    aLi=new Geom2d_Line(aPV, aDiri);
    //
    aGAC2.Load(aLi);
    //
    aGInter.Perform(aGAC1, aDomain1, aGAC2, aDomain2, aTolInt, aTolInt);
    if (!aGInter.IsDone()) {
      continue;
    }
    //
    aNbP = aGInter.NbPoints();
    aT1max = aTV;
    aT2max = -1.;
    for (j = 1; j <= aNbP; ++j) {
      const IntRes2d_IntersectionPoint& aIPj = aGInter.Point(j);
      aT1j = aIPj.ParamOnFirst();
      aT2j = aIPj.ParamOnSecond();
      //
      if (aT2j > aT2max && Abs(aT1j - aTV) < MaxDT) {
        aT2max = aT2j;
        aT1max = aT1j;
      }
    }
    //
    if (aT2max > 0) {
      dT = aTOp - aT1max;
      if (Abs(dT) < aTolInt) {
        continue;
      }
      //
      aT = aT1max + aCf*dT;
      aGAC1.D0(aT, aP);
      gp_Vec2d aV2D(aPV, aP);
      gp_Dir2d aDir2D(aV2D);
      //
      aAngle = Angle(aDir2D);
      Standard_Real aDA = ClockWiseAngle(aA2, aAngle);
      if (aDA < aDelta) {
        aA = aAngle;
        return bRet;
      }
    }
  }// for (i=0; i<2; ++i) {
  return !bRet;
}