// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_MapOfOrientedShape.hxx>
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_CoupleOfShape.hxx>
#include <BOPTools_ListOfCoupleOfShape.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_XYZ.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_Array1.hxx>
#include <algorithm>

//
static
  Standard_Real AngleWithRef(const gp_Dir& theD1,
                             const gp_Dir& theD2,
                             const gp_Dir& theDRef);

static
  Standard_Boolean FindFacePairs (const TopoDS_Edge& theE,
                                  const BOPCol_ListOfShape& thLF,
                                  BOPTools_ListOfCoupleOfShape& theLCFF,
                                  Handle(IntTools_Context)& theContext);
static
  TopAbs_Orientation Orientation(const TopoDS_Edge& anE,
                                 const TopoDS_Face& aF);

static
  Standard_Boolean GetFaceDir(const TopoDS_Edge& aE,
                              const TopoDS_Face& aF,
                              const gp_Pnt& aP,
                              const Standard_Real aT,
                              const gp_Dir& aDTgt,
                              const Standard_Boolean theSmallFaces,
                              gp_Dir& aDN,
                              gp_Dir& aDB,
                              Handle(IntTools_Context)& theContext,
                              GeomAPI_ProjectPointOnSurf& aProjPL,
                              const Standard_Real aDt);
static
  Standard_Boolean FindPointInFace(const TopoDS_Face& aF,
                                   const gp_Pnt& aP,
                                   gp_Dir& aDB,
                                   gp_Pnt& aPOut,
                                   Handle(IntTools_Context)& theContext,
                                   GeomAPI_ProjectPointOnSurf& aProjPL,
                                   const Standard_Real aDt,
                                   const Standard_Real aTolE);
static 
  Standard_Real MinStep3D(const TopoDS_Edge& theE1,
                          const TopoDS_Face& theF1,
                          const BOPTools_ListOfCoupleOfShape& theLCS,
                          const gp_Pnt& aP,
                          Handle(IntTools_Context)& theContext,
                          Standard_Boolean& theSmallFaces);



//=======================================================================
// function: MakeConnexityBlocks
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakeConnexityBlocks
  (const TopoDS_Shape& theS,
   const TopAbs_ShapeEnum theType1,
   const TopAbs_ShapeEnum theType2,
   BOPCol_ListOfShape& theLCB)
{
  // Map shapes to find connected elements
  BOPCol_IndexedDataMapOfShapeListOfShape aDMSLS;
  BOPTools::MapShapesAndAncestors(theS, theType1, theType2, aDMSLS);
  // Fence map
  BOPCol_MapOfShape aMFence;
  Standard_Integer i;
  BRep_Builder aBB;
  //
  TopExp_Explorer aExp(theS, theType2);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aS = aExp.Current();
    if (!aMFence.Add(aS)) {
      continue;
    }
    // The block
    BOPCol_IndexedMapOfShape aMBlock;
    TopoDS_Compound aBlock;
    aBB.MakeCompound(aBlock);
    // Start the block
    aMBlock.Add(aS);
    aBB.Add(aBlock, aS);
    // Look for connected parts
    for (i = 1; i <= aMBlock.Extent(); ++i) {
      const TopoDS_Shape& aS1 = aMBlock(i);
      TopExp_Explorer aExpSS(aS1, theType1);
      for (; aExpSS.More(); aExpSS.Next()) {
        const TopoDS_Shape& aSubS = aExpSS.Current();
        const BOPCol_ListOfShape& aLS = aDMSLS.FindFromKey(aSubS);
        BOPCol_ListIteratorOfListOfShape aItLS(aLS);
        for (; aItLS.More(); aItLS.Next()) {
          const TopoDS_Shape& aS2 = aItLS.Value();
          if (aMFence.Add(aS2)) {
            aMBlock.Add(aS2);
            aBB.Add(aBlock, aS2);
          }
        }
      }
    }
    // Add the block into result
    theLCB.Append(aBlock);
  }
}
//=======================================================================
// function: OrientEdgesOnWire
// purpose: Reorient edges on wire for correct ordering
//=======================================================================
void BOPTools_AlgoTools::OrientEdgesOnWire(TopoDS_Shape& theWire)
{
  // make vertex-edges connexity map
  BOPCol_IndexedDataMapOfShapeListOfShape aVEMap;
  BOPTools::MapShapesAndAncestors(theWire, TopAbs_VERTEX, TopAbs_EDGE, aVEMap);
  //
  if (aVEMap.IsEmpty()) {
    return;
  }
  //
  BRep_Builder aBB;
  // new wire
  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  // fence map
  BOPCol_MapOfOrientedShape aMFence;
  //
  TopoDS_Iterator aIt(theWire);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Edge& aEC = TopoDS::Edge(aIt.Value());
    if (!aMFence.Add(aEC)) {
      continue;
    }
    //
    // add edge to a wire as it is
    aBB.Add(aWire, aEC);
    //
    TopoDS_Vertex aV1, aV2;
    TopExp::Vertices(aEC, aV1, aV2, Standard_True);
    //
    if (aV1.IsSame(aV2)) {
      // closed edge, go to the next edge
      continue;
    }
    //
    // orient the adjacent edges
    for (Standard_Integer i = 0; i < 2; ++i) {
      TopoDS_Shape aVC = !i ? aV1 : aV2;
      //
      for (;;) {
        const BOPCol_ListOfShape& aLE = aVEMap.FindFromKey(aVC);
        if (aLE.Extent() != 2) {
          // free vertex or multi-connexity, go to the next edge
          break;
        }
        //
        Standard_Boolean bStop = Standard_True;
        //
        BOPCol_ListIteratorOfListOfShape aItLE(aLE);
        for (; aItLE.More(); aItLE.Next()) {
          const TopoDS_Edge& aEN = TopoDS::Edge(aItLE.Value());
          if (aMFence.Contains(aEN)) {
            continue;
          }
          //
          TopoDS_Vertex aVN1, aVN2;
          TopExp::Vertices(aEN, aVN1, aVN2, Standard_True);
          if (aVN1.IsSame(aVN2)) {
            // closed edge, go to the next edge
            break;
          }
          //
          // change orientation if necessary and go to the next edges
          if ((!i && aVC.IsSame(aVN2)) || (i && aVC.IsSame(aVN1))) {
            aBB.Add(aWire, aEN);
          }
          else {
            aBB.Add(aWire, aEN.Reversed());
          }
          aMFence.Add(aEN);
          aVC = aVC.IsSame(aVN1) ? aVN2 : aVN1;
          bStop = Standard_False;
          break;
        }
        //
        if (bStop) {
          break;
        }
      }
    }
  }
  //
  theWire = aWire;
}
//=======================================================================
// function: OrientFacesOnShell
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::OrientFacesOnShell (TopoDS_Shape& aShell)
{
  Standard_Boolean bIsProcessed1, bIsProcessed2;
  Standard_Integer i, aNbE, aNbF, j;
  TopAbs_Orientation anOrE1, anOrE2;
  TopoDS_Face aF1x, aF2x;
  TopoDS_Shape aShellNew;
  BOPCol_IndexedDataMapOfShapeListOfShape aEFMap;
  BOPCol_IndexedMapOfShape aProcessedFaces;
  BRep_Builder aBB;
  //
  BOPTools_AlgoTools::MakeContainer(TopAbs_SHELL, aShellNew);
  //
  BOPTools::MapShapesAndAncestors(aShell, 
                                  TopAbs_EDGE, TopAbs_FACE, 
                                  aEFMap);
  aNbE=aEFMap.Extent();
  // 
  // One seam edge  in aEFMap contains  2 equivalent faces.
  for (i=1; i<=aNbE; ++i) {
    BOPCol_ListOfShape& aLF=aEFMap.ChangeFromIndex(i);
    aNbF=aLF.Extent();
    if (aNbF>1) {
      BOPCol_ListOfShape aLFTmp;
      BOPCol_IndexedMapOfShape aFM;
      //
      BOPCol_ListIteratorOfListOfShape anIt(aLF);
      for (; anIt.More(); anIt.Next()) {
        const TopoDS_Shape& aF=anIt.Value();
        if (!aFM.Contains(aF)) {
          aFM.Add(aF);
          aLFTmp.Append(aF);
        }
      }
      aLF.Clear();
      aLF=aLFTmp;
    }
  }
  //
  // Do
  for (i=1; i<=aNbE; ++i) {
    const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aEFMap.FindKey(i)));
    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //
    const BOPCol_ListOfShape& aLF=aEFMap.FindFromIndex(i);
    aNbF=aLF.Extent();
    if (aNbF!=2) {
      continue;
    }
    //
    TopoDS_Face& aF1=(*(TopoDS_Face*)(&aLF.First()));
    TopoDS_Face& aF2=(*(TopoDS_Face*)(&aLF.Last()));
    //    
    bIsProcessed1=aProcessedFaces.Contains(aF1);
    bIsProcessed2=aProcessedFaces.Contains(aF2);
    if (bIsProcessed1 && bIsProcessed2) {
      continue;
    }
    
    if (!bIsProcessed1 && !bIsProcessed2) {
      aProcessedFaces.Add(aF1);
      aBB.Add(aShellNew, aF1);
      bIsProcessed1=!bIsProcessed1;
    }
    //
    aF1x=aF1;
    if (bIsProcessed1) {
      j=aProcessedFaces.FindIndex(aF1);
      aF1x=(*(TopoDS_Face*)(&aProcessedFaces.FindKey(j)));
    }
    //
    aF2x=aF2;
    if (bIsProcessed2) {
      j=aProcessedFaces.FindIndex(aF2);
      aF2x=(*(TopoDS_Face*)(&aProcessedFaces.FindKey(j)));
    }
    //
    anOrE1=Orientation(aE, aF1x); 
    anOrE2=Orientation(aE, aF2x);
    //
    if (bIsProcessed1 && !bIsProcessed2) {
      if (anOrE1==anOrE2) {
        if (!BRep_Tool::IsClosed(aE, aF1) &&
            !BRep_Tool::IsClosed(aE, aF2)) {
          aF2.Reverse();
        }
      }
      aProcessedFaces.Add(aF2);
      aBB.Add(aShellNew, aF2);
    }
    else if (!bIsProcessed1 && bIsProcessed2) {
      if (anOrE1==anOrE2) {
        if (!BRep_Tool::IsClosed(aE, aF1) &&
            !BRep_Tool::IsClosed(aE, aF2)) {
          aF1.Reverse();
        }
      }
      aProcessedFaces.Add(aF1);
      aBB.Add(aShellNew, aF1);
    }
  }
  //
  //
  for (i=1; i<=aNbE; ++i) {
    const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aEFMap.FindKey(i)));
    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //
    const BOPCol_ListOfShape& aLF=aEFMap.FindFromIndex(i);
    aNbF=aLF.Extent();
    if (aNbF!=2) {
      BOPCol_ListIteratorOfListOfShape anIt(aLF);
      for(; anIt.More(); anIt.Next()) {
        const TopoDS_Face& aF=(*(TopoDS_Face*)(&anIt.Value()));
        if (!aProcessedFaces.Contains(aF)) {
          aProcessedFaces.Add(aF);
          aBB.Add(aShellNew, aF);
        }
      }
    }
  }
  aShell=aShellNew;
}
//=======================================================================
//function : Orientation
//purpose  :
//=======================================================================
TopAbs_Orientation Orientation(const TopoDS_Edge& anE,
                               const TopoDS_Face& aF)
{
  TopAbs_Orientation anOr=TopAbs_INTERNAL;

  TopExp_Explorer anExp;
  anExp.Init(aF, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Edge& anEF1=(*(TopoDS_Edge*)(&anExp.Current()));
    if (anEF1.IsSame(anE)) {
      anOr=anEF1.Orientation();
      break;
    }
  }
  return anOr;
}
//=======================================================================
// function: MakeConnexityBlock.
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakeConnexityBlock 
  (BOPCol_ListOfShape& theLFIn,
   BOPCol_IndexedMapOfShape& theMEAvoid,
   BOPCol_ListOfShape& theLCB,
   const Handle(NCollection_BaseAllocator)& theAllocator)
{
  Standard_Integer  aNbF, aNbAdd1, aNbAdd, i;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  BOPCol_IndexedMapOfShape aMCB(100, theAllocator);
  BOPCol_IndexedMapOfShape aMAdd(100, theAllocator);
  BOPCol_IndexedMapOfShape aMAdd1(100, theAllocator);
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF(100, theAllocator);
  //
  // 1. aMEF
  aNbF=theLFIn.Extent();
  aIt.Initialize(theLFIn);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF=aIt.Value();      
    BOPTools::MapShapesAndAncestors(aF, TopAbs_EDGE, TopAbs_FACE, aMEF);
  }
  //
  // 2. aMCB
  const TopoDS_Shape& aF1=theLFIn.First();
  aMAdd.Add(aF1);
  //
  for(;;) {
    aMAdd1.Clear();
    aNbAdd = aMAdd.Extent();
    for (i=1; i<=aNbAdd; ++i) {
      const TopoDS_Shape& aF=aMAdd(i);
      //
      //aMAdd1.Clear();
      aExp.Init(aF, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE=aExp.Current();
        if (theMEAvoid.Contains(aE)){
          continue;
        }
        //
        const BOPCol_ListOfShape& aLF=aMEF.FindFromKey(aE);
        aIt.Initialize(aLF);
        for (; aIt.More(); aIt.Next()) {
          const TopoDS_Shape& aFx=aIt.Value();
          if (aFx.IsSame(aF)) {
            continue;
          }
          if (aMCB.Contains(aFx)) {
            continue;
          }
          aMAdd1.Add(aFx);
        }
      }//for (; aExp.More(); aExp.Next()){
      aMCB.Add(aF);
    }// for (i=1; i<=aNbAdd; ++i) {
    //
    aNbAdd1=aMAdd1.Extent();
    if (!aNbAdd1) {
      break;
    }
    //
    aMAdd.Clear();
    for (i=1; i<=aNbAdd1; ++i) {
      const TopoDS_Shape& aFAdd=aMAdd1(i);
      aMAdd.Add(aFAdd);
    }
    //
  }//while(1) {
  
  //
  aNbF=aMCB.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Shape& aF=aMCB(i);
    theLCB.Append(aF);
  }
}
//=======================================================================
// function:  ComputeStateByOnePoint
// purpose: 
//=======================================================================
TopAbs_State BOPTools_AlgoTools::ComputeStateByOnePoint
  (const TopoDS_Shape& theS,
   const TopoDS_Solid& theRef,
   const Standard_Real theTol,
   Handle(IntTools_Context)& theContext)
{
  TopAbs_State aState;
  TopAbs_ShapeEnum aType;
  //
  aState=TopAbs_UNKNOWN;
  aType=theS.ShapeType();
  if (aType==TopAbs_VERTEX) {
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex*)(&theS));
    aState=BOPTools_AlgoTools::ComputeState(aV, theRef, theTol, theContext);
  }
  else if (aType==TopAbs_EDGE) {
    const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&theS));
    aState=BOPTools_AlgoTools::ComputeState(aE, theRef, theTol, theContext);
  }
  return aState;
}

//=======================================================================
// function:  ComputeState
// purpose: 
//=======================================================================
TopAbs_State BOPTools_AlgoTools::ComputeState
  (const TopoDS_Face& theF,
   const TopoDS_Solid& theRef,
   const Standard_Real theTol,
   BOPCol_IndexedMapOfShape& theBounds,
   Handle(IntTools_Context)& theContext)
{
  TopAbs_State aState;
  TopExp_Explorer aExp; 
  TopoDS_Edge aE1;
  gp_Pnt2d aP2D;
  gp_Pnt aP3D; 
  //
  aState=TopAbs_UNKNOWN;
  //
  aExp.Init(theF, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aSE=(*(TopoDS_Edge*)(&aExp.Current()));
    if (BRep_Tool::Degenerated(aSE)) {
      continue;
    }
    //
    if (!theBounds.Contains(aSE)) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aSE));
      aState=BOPTools_AlgoTools::ComputeState(aE, theRef, theTol, 
                                              theContext);
      return aState;
    }
    if (aE1.IsNull()) {
      aE1=(*(TopoDS_Edge*)(&aSE));
    }
  }
  // !!<- process edges that are all on theRef
  if (!aE1.IsNull()) {
    BOPTools_AlgoTools3D::PointNearEdge(aE1, theF, 
                                        aP2D, aP3D, theContext);
    aState=BOPTools_AlgoTools::ComputeState(aP3D, theRef, theTol, 
                                            theContext);
  }
  //
  return aState;
}
//=======================================================================
// function:  ComputeState
// purpose: 
//=======================================================================
TopAbs_State BOPTools_AlgoTools::ComputeState
  (const TopoDS_Vertex& theV,
   const TopoDS_Solid& theRef,
   const Standard_Real theTol,
   Handle(IntTools_Context)& theContext)
{
  TopAbs_State aState;
  gp_Pnt aP3D; 
  //
  aP3D=BRep_Tool::Pnt(theV);
  aState=BOPTools_AlgoTools::ComputeState(aP3D, theRef, theTol, 
                                          theContext);
  return aState;
}
//=======================================================================
// function:  ComputeState
// purpose: 
//=======================================================================
TopAbs_State BOPTools_AlgoTools::ComputeState
  (const TopoDS_Edge& theE,
   const TopoDS_Solid& theRef,
   const Standard_Real theTol,
   Handle(IntTools_Context)& theContext)
{
  Standard_Real aT1, aT2, aT = 0.;
  TopAbs_State aState;
  Handle(Geom_Curve) aC3D;
  gp_Pnt aP3D; 
  //
  aC3D = BRep_Tool::Curve(theE, aT1, aT2);
  //
  if(aC3D.IsNull()) {
    //it means that we are in degenerated edge
    const TopoDS_Vertex& aV = TopExp::FirstVertex(theE);
    if(aV.IsNull()){
      return TopAbs_UNKNOWN;
    }
    aP3D=BRep_Tool::Pnt(aV);
  }
  else {//usual case
    Standard_Boolean bF2Inf, bL2Inf;
    Standard_Real dT=10.;
    //
    bF2Inf = Precision::IsNegativeInfinite(aT1);
    bL2Inf = Precision::IsPositiveInfinite(aT2);
    //
    if (bF2Inf && !bL2Inf) {
      aT=aT2-dT;
    }
    else if (!bF2Inf && bL2Inf) {
      aT=aT1+dT;
    }
    else if (bF2Inf && bL2Inf) {
      aT=0.;
    }
    else {
      aT=IntTools_Tools::IntermediatePoint(aT1, aT2);
    }
    aC3D->D0(aT, aP3D);
  }
  //
  aState=BOPTools_AlgoTools::ComputeState(aP3D, theRef, theTol, 
                                          theContext);
  //
  return aState;
}
//=======================================================================
// function:  ComputeState
// purpose: 
//=======================================================================
TopAbs_State BOPTools_AlgoTools::ComputeState
  (const gp_Pnt& theP,
   const TopoDS_Solid& theRef,
   const Standard_Real theTol,
   Handle(IntTools_Context)& theContext)
{
  TopAbs_State aState;
  //
  BRepClass3d_SolidClassifier& aSC=theContext->SolidClassifier(theRef);
  aSC.Perform(theP, theTol);
  //
  aState=aSC.State();
  //
  return aState;
}
//=======================================================================
//function : IsInternalFace
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::IsInternalFace
  (const TopoDS_Face& theFace,
   const TopoDS_Solid& theSolid,
   BOPCol_IndexedDataMapOfShapeListOfShape& theMEF,
   const Standard_Real theTol,
   Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bDegenerated;
  Standard_Integer aNbF, iRet, iFound;
  TopAbs_Orientation aOr;
  TopoDS_Edge aE1;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape aItF;
  //
  // For all invoked functions: [::IsInternalFace(...)] 
  // the returned value iRet means:
  // iRet=0;  - state is not IN
  // iRet=1;  - state is IN
  // iRet=2;  - state can not be found by the method of angles
  //
  // For this function the returned value iRet means:
  // iRet=0;  - state is not IN
  // iRet=1;  - state is IN
  //
  iRet=0; 
  // 1 Try to find an edge from theFace in theMEF
  iFound=0;
  aExp.Init(theFace, TopAbs_EDGE);
  for(; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aExp.Current()));
    if (!theMEF.Contains(aE)) {
      continue;
    }
    //
    ++iFound;
    //
    aOr=aE.Orientation();
    if (aOr==TopAbs_INTERNAL) {
      continue;
    }
    bDegenerated=BRep_Tool::Degenerated(aE);
    if (bDegenerated){
      continue;
    }
    // aE
    BOPCol_ListOfShape& aLF=theMEF.ChangeFromKey(aE);
    aNbF=aLF.Extent();
    if (!aNbF) {
      return iRet != 0; // it can not be so
    }
    //
    else if (aNbF==1) {
      // aE is internal edge on aLF.First()
      const TopoDS_Face& aF1=(*(TopoDS_Face*)(&aLF.First()));
      BOPTools_AlgoTools::GetEdgeOnFace(aE, aF1, aE1);
      if (aE1.Orientation()!=TopAbs_INTERNAL) {
        iRet=2; 
        break;
      }
      //
      iRet=BOPTools_AlgoTools::IsInternalFace(theFace, aE, aF1, aF1, 
                                              theContext);
      break;
    }
    //
    else if (aNbF==2) {
      const TopoDS_Face& aF1=(*(TopoDS_Face*)(&aLF.First()));
      const TopoDS_Face& aF2=(*(TopoDS_Face*)(&aLF.Last()));
      //
      if (aF2.IsSame(aF1) && BRep_Tool::IsClosed(aE, aF1)) {
        // treat as it was for 1 face
        iRet=BOPTools_AlgoTools::IsInternalFace(theFace, aE, aF1, aF2, 
                                                theContext);
        break;
      }
    }
    //
    if (aNbF%2) {
      return Standard_False; // it can not be so
    }
    else { // aNbF=2,4,6,8,...
      iRet=BOPTools_AlgoTools::IsInternalFace(theFace, aE, aLF, 
                                              theContext);
      break;
    }
  }//for(; aExp.More(); aExp.Next()) {
  //
  if (!iFound) {
    // the face has no shared edges with the solid
    iRet=2;
  }
  //
  if (iRet!=2) {
    return iRet == 1;
  }
  //
  //========================================
  // 2. Classify face using classifier
  //
  TopAbs_State aState;
  BOPCol_IndexedMapOfShape aBounds;
  //
  BOPTools::MapShapes(theSolid, TopAbs_EDGE, aBounds);
  //
  aState=BOPTools_AlgoTools::ComputeState(theFace, theSolid, 
                                          theTol, aBounds, theContext);
  return aState == TopAbs_IN;
}
//=======================================================================
//function : IsInternalFace
//purpose  : 
//=======================================================================
Standard_Integer BOPTools_AlgoTools::IsInternalFace
  (const TopoDS_Face& theFace,
   const TopoDS_Edge& theEdge,
   BOPCol_ListOfShape& theLF,
   Handle(IntTools_Context)& theContext)
{
  Standard_Integer aNbF, iRet;
  //
  iRet=0;
  //
  aNbF=theLF.Extent();
  if (aNbF==2) {
    const TopoDS_Face& aF1=(*(TopoDS_Face*)(&theLF.First()));
    const TopoDS_Face& aF2=(*(TopoDS_Face*)(&theLF.Last()));
    iRet=BOPTools_AlgoTools::IsInternalFace(theFace, theEdge, aF1, aF2, 
                                            theContext);
    return iRet;
  }
  //
  else {
    BOPTools_ListOfCoupleOfShape aLCFF;
    BOPTools_ListIteratorOfListOfCoupleOfShape aIt;
    //
    FindFacePairs(theEdge, theLF, aLCFF, theContext);
    //
    aIt.Initialize(aLCFF);
    for (; aIt.More(); aIt.Next()) {
      BOPTools_CoupleOfShape& aCSFF=aIt.ChangeValue();
      //
      const TopoDS_Face& aF1=(*(TopoDS_Face*)(&aCSFF.Shape1()));
      const TopoDS_Face& aF2=(*(TopoDS_Face*)(&aCSFF.Shape2()));
      iRet=BOPTools_AlgoTools::IsInternalFace(theFace, theEdge, aF1, aF2, 
                                              theContext);
      if (iRet) {
        return iRet;
      }
    }
  }
  return iRet;
}
//=======================================================================
//function : IsInternalFace
//purpose  : 
//=======================================================================
Standard_Integer BOPTools_AlgoTools::IsInternalFace
  (const TopoDS_Face& theFace,
   const TopoDS_Edge& theEdge,
   const TopoDS_Face& theFace1,
   const TopoDS_Face& theFace2,
   Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bRet;
  Standard_Integer iRet;
  TopoDS_Edge aE1, aE2;
  TopoDS_Face aFOff;
  BOPTools_ListOfCoupleOfShape theLCSOff;
  BOPTools_CoupleOfShape aCS1, aCS2;
  //
  BOPTools_AlgoTools::GetEdgeOnFace(theEdge, theFace1, aE1);
  if (aE1.Orientation()==TopAbs_INTERNAL) {
    aE2=aE1;
    aE1.Orientation(TopAbs_FORWARD);
    aE2.Orientation(TopAbs_REVERSED);
  }
  else if (theFace1==theFace2) {
    aE2=aE1;
    aE1.Orientation(TopAbs_FORWARD);
    aE2.Orientation(TopAbs_REVERSED);
  }
  else {
    BOPTools_AlgoTools::GetEdgeOnFace(theEdge, theFace2, aE2);
  }
  //
  aCS1.SetShape1(theEdge);
  aCS1.SetShape2(theFace);
  theLCSOff.Append(aCS1);
  //
  aCS2.SetShape1(aE2);
  aCS2.SetShape2(theFace2);
  theLCSOff.Append(aCS2);
  //
  bRet=GetFaceOff(aE1, theFace1, theLCSOff, aFOff, theContext);
  //
  iRet=0; // theFace is not internal
  if (theFace.IsEqual(aFOff)) {
    // theFace is internal
    iRet=1;  
    if (!bRet) {
      // theFace seems to be internal  
      iRet=2;
    }
  }
  return iRet;
}
//=======================================================================
//function : GetFaceOff
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::GetFaceOff
  (const TopoDS_Edge& theE1,
   const TopoDS_Face& theF1,
   BOPTools_ListOfCoupleOfShape& theLCSOff,
   TopoDS_Face& theFOff,
   Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bRet, bIsComputed;
  Standard_Real aT, aT1, aT2, aAngle, aTwoPI, aAngleMin, aDt3D;
  Standard_Real aUmin, aUsup, aVmin, aVsup, aPA;
  gp_Pnt aPn1, aPn2, aPx;
  gp_Dir aDN1, aDN2, aDBF, aDBF2, aDTF;
  gp_Vec aVTgt;
  TopAbs_Orientation aOr;
  Handle(Geom_Curve)aC3D;
  Handle(Geom_Plane) aPL;
  BOPTools_ListIteratorOfListOfCoupleOfShape aIt;
  GeomAPI_ProjectPointOnSurf aProjPL;
  //
  aPA=Precision::Angular();
  aAngleMin=100.;
  aTwoPI=M_PI+M_PI;
  aC3D =BRep_Tool::Curve(theE1, aT1, aT2);
  aT=BOPTools_AlgoTools2D::IntermediatePoint(aT1, aT2);
  aC3D->D0(aT, aPx);
  //
  BOPTools_AlgoTools2D::EdgeTangent(theE1, aT, aVTgt);
  gp_Dir aDTgt(aVTgt), aDTgt2;
  aOr = theE1.Orientation();
  //
  aPL = new Geom_Plane(aPx, aDTgt);
  aPL->Bounds(aUmin, aUsup, aVmin, aVsup);
  aProjPL.Init(aPL, aUmin, aUsup, aVmin, aVsup);
  //
  Standard_Boolean bSmallFaces = Standard_False;
  aDt3D = MinStep3D(theE1, theF1, theLCSOff, aPx, theContext, bSmallFaces);
  bIsComputed = GetFaceDir(theE1, theF1, aPx, aT, aDTgt, bSmallFaces,
                           aDN1, aDBF, theContext, aProjPL, aDt3D);
  if (!bIsComputed) {
#ifdef OCCT_DEBUG
    cout << "BOPTools_AlgoTools::GetFaceOff(): incorrect computation of bi-normal direction." << endl;
#endif
  }
  //
  aDTF=aDN1^aDBF;
  //
  bRet=Standard_True;
  aIt.Initialize(theLCSOff);
  for (; aIt.More(); aIt.Next()) {
    const BOPTools_CoupleOfShape& aCS=aIt.Value();
    const TopoDS_Edge& aE2=(*(TopoDS_Edge*)(&aCS.Shape1()));
    const TopoDS_Face& aF2=(*(TopoDS_Face*)(&aCS.Shape2()));
    //
    aDTgt2 = (aE2.Orientation()==aOr) ? aDTgt : aDTgt.Reversed();
    bIsComputed = GetFaceDir(aE2, aF2, aPx, aT, aDTgt2, bSmallFaces, aDN2,
                             aDBF2, theContext, aProjPL, aDt3D);
    if (!bIsComputed) {
#ifdef OCCT_DEBUG
      cout << "BOPTools_AlgoTools::GetFaceOff(): incorrect computation of bi-normal direction." << endl;
#endif
    }
    //Angle
    aAngle=AngleWithRef(aDBF, aDBF2, aDTF);
    //
    if(aAngle<0.) {
      aAngle=aTwoPI+aAngle;
    }
    //
    if (aAngle<aPA) {
      if (aF2==theF1) {
        aAngle=M_PI;
      }
      else if (aF2.IsSame(theF1)) {
        aAngle=aTwoPI;
      }
    }
    //
    if (fabs(aAngle-aAngleMin)<aPA) {
      // the minimal angle can not be found
      bRet=Standard_False; 
    }
    //
    if (aAngle<aAngleMin){
      aAngleMin=aAngle;
      theFOff=aF2;
    }
    else if (aAngle==aAngleMin) {
      // the minimal angle can not be found
      bRet=Standard_False;  
    }
  }
  return bRet;
}
//=======================================================================
//function : GetEdgeOff
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::GetEdgeOff(const TopoDS_Edge& theE1,
                                                const TopoDS_Face& theF2,
                                                TopoDS_Edge& theE2)
{
  Standard_Boolean bFound;
  TopAbs_Orientation aOr1, aOr1C, aOr2;
  TopExp_Explorer anExp;
  //
  bFound=Standard_False;
  aOr1=theE1.Orientation();
  aOr1C=TopAbs::Reverse(aOr1);
  //
  anExp.Init(theF2, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Edge& aEF2=(*(TopoDS_Edge*)(&anExp.Current()));
    if (aEF2.IsSame(theE1)) {
      aOr2=aEF2.Orientation();
      if (aOr2==aOr1C) {
        theE2=aEF2;
        bFound=!bFound;
        return bFound;
      }
    }
  }
  return bFound;
}

//=======================================================================
//function : AreFacesSameDomain
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::AreFacesSameDomain
  (const TopoDS_Face& theF1,
   const TopoDS_Face& theF2,
   Handle(IntTools_Context)& theContext,
   const Standard_Real theFuzz)
{
  Standard_Boolean bFlag;
  Standard_Integer iErr;
  Standard_Real aTolF1, aTolF2, aTol;
  gp_Pnt2d aP2D;
  gp_Pnt aP;
  TopoDS_Face aF1, aF2;
  TopoDS_Edge aE1;
  TopExp_Explorer aExp;
  Standard_Real aFuzz1 = (theFuzz > Precision::Confusion() ? theFuzz : Precision::Confusion());
  //
  bFlag=Standard_False;
  //
  aF1=theF1;
  aF1.Orientation(TopAbs_FORWARD);
  aF2=theF2;
  aF2.Orientation(TopAbs_FORWARD);
  //
  aTolF1=BRep_Tool::Tolerance(aF1);
  // 1
  aExp.Init(aF1, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    aE1=(*(TopoDS_Edge*)(&aExp.Current()));
    if (!BRep_Tool::Degenerated(aE1)) {
      Standard_Real aTolE = BRep_Tool::Tolerance(aE1);
      if (aTolE > aTolF1) {
        aTolF1 = aTolE;
      }
    }
  }
  // 2
  aTolF2=BRep_Tool::Tolerance(aF2);
  aTol = aTolF1 + aTolF2 + aFuzz1;
  //
  iErr = BOPTools_AlgoTools3D::PointInFace(aF1, aP, aP2D,
                                           theContext);
  if (!iErr) {
    bFlag=theContext->IsValidPointForFace(aP, aF2, aTol);
  }
  //
  return bFlag;
}

//=======================================================================
//function : CheckSameGeom
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::CheckSameGeom
  (const TopoDS_Face& theF1,
   const TopoDS_Face& theF2,
   Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bRet;
  Standard_Real aTolF1, aTolF2, aTol;
  gp_Pnt2d aP2D;
  gp_Pnt aP;
  TopExp_Explorer aExp;
  //
  bRet=Standard_False;
  aExp.Init(theF1, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aExp.Current()));
    if (!BRep_Tool::Degenerated(aE)) {
      aTolF1=BRep_Tool::Tolerance(theF1);
      aTolF2=BRep_Tool::Tolerance(theF2);
      aTol=aTolF1+aTolF2;
      BOPTools_AlgoTools3D::PointNearEdge(aE, theF1, aP2D, aP, theContext);
      bRet=theContext->IsValidPointForFace(aP, theF2, aTol);
      break;
    }
  }
  return bRet;
}
//=======================================================================
// function: Sense
// purpose: 
//=======================================================================
Standard_Integer BOPTools_AlgoTools::Sense (const TopoDS_Face& theF1,
                                            const TopoDS_Face& theF2,
                                            const Handle(IntTools_Context)& theContext)
{
  Standard_Integer iSense=0;
  gp_Dir aDNF1, aDNF2;
  TopoDS_Edge aE1, aE2;
  TopExp_Explorer aExp;
  //
  aExp.Init(theF1, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    aE1=(*(TopoDS_Edge*)(&aExp.Current()));
    if (!BRep_Tool::Degenerated(aE1)) {
      if (!BRep_Tool::IsClosed(aE1, theF1)) {
        break;
      }
    }
  }
  //
  aExp.Init(theF2, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    aE2=(*(TopoDS_Edge*)(&aExp.Current()));
    if (!BRep_Tool::Degenerated(aE2)) {
      if (!BRep_Tool::IsClosed(aE2, theF2)) {
        if (aE2.IsSame(aE1)) {
          iSense=1;
          break;
        }
      }
    }
  }
  //
  if (!iSense) {
    return iSense;
  }
  //
  BOPTools_AlgoTools3D::GetNormalToFaceOnEdge(aE1, theF1, aDNF1, theContext);
  BOPTools_AlgoTools3D::GetNormalToFaceOnEdge(aE2, theF2, aDNF2, theContext);
  //
  iSense=BOPTools_AlgoTools3D::SenseFlag(aDNF1, aDNF2);
  //
  return iSense;
}
//=======================================================================
// function: IsSplitToReverse
// purpose: 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::IsSplitToReverse
  (const TopoDS_Shape& theSp,
   const TopoDS_Shape& theSr,
   Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bRet;
  TopAbs_ShapeEnum aType;
  //
  bRet=Standard_False;
  //
  aType=theSp.ShapeType();
  switch (aType) {
    case TopAbs_EDGE: {
      const TopoDS_Edge& aESp=(*(TopoDS_Edge*)(&theSp));
      const TopoDS_Edge& aESr=(*(TopoDS_Edge*)(&theSr));
      bRet=BOPTools_AlgoTools::IsSplitToReverse(aESp, aESr, theContext);
    }
      break;
      //
    case TopAbs_FACE: {
      const TopoDS_Face& aFSp=(*(TopoDS_Face*)(&theSp));
      const TopoDS_Face& aFSr=(*(TopoDS_Face*)(&theSr));
      bRet=BOPTools_AlgoTools::IsSplitToReverse(aFSp, aFSr, theContext);
    }
      break;
      //
    default:
      break;
  }
  return bRet;
}
//=======================================================================
//function :IsSplitToReverse
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::IsSplitToReverse
  (const TopoDS_Face& theFSp,
   const TopoDS_Face& theFSr,
   Handle(IntTools_Context)& theContext)
{
  // Compare surfaces
  Handle(Geom_Surface) aSFSp = BRep_Tool::Surface(theFSp);
  Handle(Geom_Surface) aSFOr = BRep_Tool::Surface(theFSr);
  if (aSFSp == aSFOr) {
    return theFSp.Orientation() != theFSr.Orientation();
  }
  //
  Standard_Boolean bDone = Standard_False;
  // Find the point inside the split face
  gp_Pnt aPFSp;
  gp_Pnt2d aP2DFSp;
  //
  // Error status
  Standard_Integer iErr;
  // Use the hatcher to find the point in the middle of the face
  iErr = BOPTools_AlgoTools3D::PointInFace(theFSp, aPFSp, aP2DFSp, theContext);
  if (iErr) {
    // Hatcher has failed to find a point.
    // Try to get the point near some not closed and
    // not degenerated edge on the split face.
    TopExp_Explorer anExp(theFSp, TopAbs_EDGE);
    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Edge& aESp = (*(TopoDS_Edge*)(&anExp.Current()));
      if (!BRep_Tool::Degenerated(aESp) && !BRep_Tool::IsClosed(aESp, theFSp)) {
        iErr = BOPTools_AlgoTools3D::PointNearEdge
                 (aESp, theFSp, aP2DFSp, aPFSp, theContext);
        if (!iErr) {
          break;
        }
      }
    }
    //
    if (!anExp.More()) {
      // The point has not been found.
      return bDone;
    }
  }
  //
  // Compute normal direction of the split face
  gp_Dir aDNFSp;
  bDone = BOPTools_AlgoTools3D::GetNormalToSurface
    (aSFSp, aP2DFSp.X(), aP2DFSp.Y(), aDNFSp);
  if (!bDone) {
    return bDone;
  }
  //
  if (theFSp.Orientation() == TopAbs_REVERSED){
    aDNFSp.Reverse();
  }
  //
  // Project the point from the split face on the original face
  // to find its UV coordinates
  GeomAPI_ProjectPointOnSurf& aProjector = theContext->ProjPS(theFSr);
  aProjector.Perform(aPFSp);
  bDone = (aProjector.NbPoints() > 0);
  if (!bDone) {
    return bDone;
  }
  // UV coordinates of the point on the original face
  Standard_Real aU, aV;
  aProjector.LowerDistanceParameters(aU, aV);
  //
  // Compute normal direction for the original face in this point
  gp_Dir aDNFOr;
  bDone = BOPTools_AlgoTools3D::GetNormalToSurface(aSFOr, aU, aV, aDNFOr);
  if (!bDone) {
    return bDone;
  }
  //
  if (theFSr.Orientation() == TopAbs_REVERSED) {
    aDNFOr.Reverse();
  }
  //
  // compare the normals
  Standard_Real aCos = aDNFSp*aDNFOr;
  return (aCos < 0.);
}
//=======================================================================
//function :IsSplitToReverse
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::IsSplitToReverse
  (const TopoDS_Edge& aEF1,
   const TopoDS_Edge& aEF2,
   Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bRet, bIsDegenerated;
  //
  bRet=Standard_False;
  bIsDegenerated=(BRep_Tool::Degenerated(aEF1) || 
                  BRep_Tool::Degenerated(aEF2));
  if (bIsDegenerated) {
    return bRet;
  }
  //
  Standard_Real a, b;
  TopAbs_Orientation aOrE, aOrSp;
  Handle(Geom_Curve)aC1, aC2;
  //
  aC2=BRep_Tool::Curve(aEF2, a, b);
  aC1=BRep_Tool::Curve(aEF1, a, b);
  //
  if (aC1==aC2) {
    aOrE=aEF2.Orientation();
    aOrSp=aEF1.Orientation();
    bRet=(aOrE!=aOrSp);
    return bRet;
  }
  //
  Standard_Real aT1, aT2, aScPr;
  gp_Vec aV1, aV2;
  gp_Pnt aP;
  //
  aT1=BOPTools_AlgoTools2D::IntermediatePoint(a, b);
  aC1->D0(aT1, aP);
  BOPTools_AlgoTools2D::EdgeTangent(aEF1, aT1, aV1);
  gp_Dir aDT1(aV1);
  //
  theContext->ProjectPointOnEdge(aP, aEF2, aT2);
  //
  BOPTools_AlgoTools2D::EdgeTangent(aEF2, aT2, aV2);
  gp_Dir aDT2(aV2);
  //
  aScPr=aDT1*aDT2;
  bRet=(aScPr<0.);
  //
  return bRet;
}

//=======================================================================
//function : IsHole
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::IsHole(const TopoDS_Shape& aW,
                                            const TopoDS_Shape& aFace)
{
  Standard_Boolean bIsHole;
  Standard_Integer i, aNbS;
  Standard_Real aT1, aT2, aS;
  Standard_Real aU1, aU, dU;
  Standard_Real aX1, aY1, aX0, aY0;
  TopAbs_Orientation aOr;
  
  gp_Pnt2d aP2D0, aP2D1;
  Handle(Geom2d_Curve) aC2D;
  TopoDS_Face aF, aFF;
  TopoDS_Iterator aItW;
  //
  bIsHole=Standard_False;
  //
  aF=(*(TopoDS_Face *)(&aFace));
  aFF=aF;
  aFF.Orientation(TopAbs_FORWARD);
  //
  aS=0.;
  aItW.Initialize(aW);
  for (; aItW.More(); aItW.Next()) { 
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aItW.Value()));
    aOr=aE.Orientation();
    if (!(aOr==TopAbs_FORWARD || 
          aOr==TopAbs_REVERSED)) {
      continue;
    }
    //
    aC2D=BRep_Tool::CurveOnSurface(aE, aFF, aT1, aT2);
    if (aC2D.IsNull()) {
      break; //xx
    }
    //
    BRepAdaptor_Curve2d aBAC2D(aE, aFF);
    aNbS=Geom2dInt_Geom2dCurveTool::NbSamples(aBAC2D);
    if (aNbS>2) {
      aNbS*=4;
    }
    //
    dU=(aT2-aT1)/(Standard_Real)(aNbS-1);
    aU =aT1;
    aU1=aT1;
    if (aOr==TopAbs_REVERSED) {
      aU =aT2;
      aU1=aT2;
      dU=-dU;
    }
    //
    aBAC2D.D0(aU, aP2D0);
    for(i=2; i<=aNbS; i++) {
      aU=aU1+(i-1)*dU;
      aBAC2D.D0(aU, aP2D1);
      aP2D0.Coord(aX0, aY0);
      aP2D1.Coord(aX1, aY1);
      //
      aS=aS+(aY0+aY1)*(aX1-aX0); 
      //
      aP2D0=aP2D1;
    }
  }//for (; aItW.More(); aItW.Next()) { 
  bIsHole=(aS>0.);
  return bIsHole;
}

//=======================================================================
// function: MakeContainer
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakeContainer(const TopAbs_ShapeEnum theType,
                                       TopoDS_Shape& theC)
{
  BRep_Builder aBB;
  //
  switch(theType) {
    case TopAbs_COMPOUND:{
      TopoDS_Compound aC;
      aBB.MakeCompound(aC);
      theC=aC;
    }
      break;
      //
    case TopAbs_COMPSOLID:{
      TopoDS_CompSolid aCS;
      aBB.MakeCompSolid(aCS);
      theC=aCS;
    }
      break;
      //
    case TopAbs_SOLID:{
      TopoDS_Solid aSolid;
      aBB.MakeSolid(aSolid);
      theC=aSolid;
    }  
      break;
      //
      //
    case TopAbs_SHELL:{
      TopoDS_Shell aShell;
      aBB.MakeShell(aShell);
      theC=aShell;
    }  
      break;
      //
    case TopAbs_WIRE: {
      TopoDS_Wire aWire;
      aBB.MakeWire(aWire);
      theC=aWire;
    }
      break;
      //
    default:
      break;
  }
}
//=======================================================================
// function: MakePCurve
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakePCurve(const TopoDS_Edge& aE,
                                    const TopoDS_Face& aF1,
                                    const TopoDS_Face& aF2,
                                    const IntTools_Curve& aIC,
                                    const Standard_Boolean bPC1,
                                    const Standard_Boolean bPC2,
                                    const Handle(IntTools_Context)& theContext)

{
  Standard_Integer i;
  Standard_Real aTolE, aT1, aT2, aOutFirst, aOutLast, aOutTol;
  Handle(Geom2d_Curve) aC2D, aC2DA, aC2Dx1;
  TopoDS_Face aFFWD; 
  BRep_Builder aBB;
  Standard_Boolean bPC;
  //
  aTolE=BRep_Tool::Tolerance(aE);
  //
  const Handle(Geom_Curve)& aC3DE=BRep_Tool::Curve(aE, aT1, aT2);
  Handle(Geom_TrimmedCurve)aC3DETrim=
    new Geom_TrimmedCurve(aC3DE, aT1, aT2);
  //
  for (i=0; i<2; ++i) {
    bPC = !i ? bPC1 : bPC2;
    if (!bPC) {
      continue;
    }
    //
    if (!i) {
      aFFWD=aF1;
      aC2Dx1=aIC.FirstCurve2d();
    }
    else {
      aFFWD=aF2;
      aC2Dx1=aIC.SecondCurve2d();
    }
    //
    aFFWD.Orientation(TopAbs_FORWARD);
    //
    aC2D=aC2Dx1;
    if (aC2D.IsNull()) { 
      BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aE, aFFWD, theContext);
      BOPTools_AlgoTools2D::CurveOnSurface(aE, aFFWD, aC2D, 
                                       aOutFirst, aOutLast, 
                                       aOutTol, theContext);
      }
    //
    if (aC3DE->IsPeriodic()) {
      BOPTools_AlgoTools2D::AdjustPCurveOnFace(aFFWD, aT1, aT2,  aC2D, 
                                               aC2DA, theContext);
    }
    else {
      BOPTools_AlgoTools2D::AdjustPCurveOnFace(aFFWD, aC3DETrim, aC2D, 
                                               aC2DA, theContext);
    }
    //
    aBB.UpdateEdge(aE, aC2DA, aFFWD, aTolE);
    //BRepLib::SameParameter(aE);
  }
  BRepLib::SameParameter(aE);
}
//=======================================================================
// function: MakeEdge
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakeEdge(const IntTools_Curve& theIC,
                                  const TopoDS_Vertex& theV1,
                                  const Standard_Real theT1,
                                  const TopoDS_Vertex& theV2,
                                  const Standard_Real theT2,
                                  const Standard_Real theTolR3D,
                                  TopoDS_Edge& theE)
{
  BRep_Builder aBB;
  Standard_Real aNeedTol = theTolR3D + 1e-12;
  //
  aBB.UpdateVertex(theV1, aNeedTol);
  aBB.UpdateVertex(theV2, aNeedTol);
  //
  BOPTools_AlgoTools::MakeSectEdge (theIC, theV1, theT1, theV2, theT2, 
                                    theE);
  //
  aBB.UpdateEdge(theE, theTolR3D);
}
//=======================================================================
// function: ComputeVV
// purpose: 
//=======================================================================
Standard_Integer BOPTools_AlgoTools::ComputeVV(const TopoDS_Vertex& aV1, 
                                               const gp_Pnt& aP2,
                                               const Standard_Real aTolP2)
{
  Standard_Real aTolV1, aTolSum, aTolSum2, aD2;
  gp_Pnt aP1;
  //
  aTolV1=BRep_Tool::Tolerance(aV1);
  
  aTolSum = aTolV1 + aTolP2 + Precision::Confusion();
  aTolSum2=aTolSum*aTolSum;
  //
  aP1=BRep_Tool::Pnt(aV1);
  //
  aD2=aP1.SquareDistance(aP2);
  if (aD2>aTolSum2) {
    return 1;
  }
  return 0;
} 
//=======================================================================
// function: ComputeVV
// purpose: 
//=======================================================================
Standard_Integer BOPTools_AlgoTools::ComputeVV(const TopoDS_Vertex& aV1, 
                                               const TopoDS_Vertex& aV2,
                                               const Standard_Real aFuzz)
{
  Standard_Real aTolV1, aTolV2, aTolSum, aTolSum2, aD2;
  gp_Pnt aP1, aP2;
  Standard_Real aFuzz1 = (aFuzz > Precision::Confusion() ? aFuzz : Precision::Confusion());
  //
  aTolV1=BRep_Tool::Tolerance(aV1);
  aTolV2=BRep_Tool::Tolerance(aV2);
  aTolSum=aTolV1+aTolV2+aFuzz1;
  aTolSum2=aTolSum*aTolSum;
  //
  aP1=BRep_Tool::Pnt(aV1);
  aP2=BRep_Tool::Pnt(aV2);
  //
  aD2=aP1.SquareDistance(aP2);
  if (aD2>aTolSum2) {
    return 1;
  }
  return 0;
}
//=======================================================================
// function: MakeVertex
// purpose : 
//=======================================================================
void BOPTools_AlgoTools::MakeVertex(const BOPCol_ListOfShape& aLV,
                                    TopoDS_Vertex& aVnew)
{
  Standard_Integer aNb = aLV.Extent();
  if (aNb == 1)
    aVnew=*((TopoDS_Vertex*)(&aLV.First()));
  else if (aNb > 1)
  {
    Standard_Real aNTol;
    gp_Pnt aNC;
    BRepLib::BoundingVertex(aLV, aNC, aNTol);
    BRep_Builder aBB;
    aBB.MakeVertex(aVnew, aNC, aNTol);
  }
}
//=======================================================================
//function : GetEdgeOnFace
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::GetEdgeOnFace
(const TopoDS_Edge& theE1,
 const TopoDS_Face& theF2,
 TopoDS_Edge& theE2)
{
  Standard_Boolean bFound;
  TopoDS_Iterator aItF, aItW;
  //
  bFound=Standard_False;
  //
  aItF.Initialize(theF2);
  for (; aItF.More(); aItF.Next()) {
    const TopoDS_Shape& aW=aItF.Value();
    aItW.Initialize(aW);
    for (; aItW.More(); aItW.Next()) {
      const TopoDS_Shape& aE=aItW.Value();
      if (aE.IsSame(theE1)) {
        theE2=(*(TopoDS_Edge*)(&aE));
        bFound=!bFound;
        return bFound;
      }
    }
  }
  return bFound;
}
//=======================================================================
//function : FindFacePairs
//purpose  : 
//=======================================================================
Standard_Boolean FindFacePairs (const TopoDS_Edge& theE,
                                const BOPCol_ListOfShape& thLF,
                                BOPTools_ListOfCoupleOfShape& theLCFF,
                                Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bFound;
  Standard_Integer i, aNbCEF;
  TopAbs_Orientation aOr, aOrC = TopAbs_FORWARD;
  BOPCol_MapOfShape aMFP;
  TopoDS_Face aF1, aF2;
  TopoDS_Edge aEL, aE1;
  BOPCol_ListIteratorOfListOfShape aItLF;
  BOPTools_CoupleOfShape aCEF, aCFF;
  BOPTools_ListOfCoupleOfShape aLCEF, aLCEFx;
  BOPTools_ListIteratorOfListOfCoupleOfShape aIt;
  //
  bFound=Standard_True;
  //
  // Preface aLCEF
  aItLF.Initialize(thLF);
  for (; aItLF.More(); aItLF.Next()) { 
    const TopoDS_Face& aFL=(*(TopoDS_Face*)(&aItLF.Value()));
    //
    bFound=BOPTools_AlgoTools::GetEdgeOnFace(theE, aFL, aEL);
    if (!bFound) {
      return bFound; // it can not be so
    }
    //
    aCEF.SetShape1(aEL);
    aCEF.SetShape2(aFL);
    aLCEF.Append(aCEF);
  }
  //
  aNbCEF=aLCEF.Extent();
  while(aNbCEF) {
    //
    // aLCEFx
    aLCEFx.Clear();
    aIt.Initialize(aLCEF);
    for (i=0; aIt.More(); aIt.Next(), ++i) {
      const BOPTools_CoupleOfShape& aCSx=aIt.Value();
      const TopoDS_Shape& aEx=aCSx.Shape1();
      const TopoDS_Shape& aFx=aCSx.Shape2();
      //
      aOr=aEx.Orientation();
      //
      if (!i) {
        aOrC=TopAbs::Reverse(aOr);
        aE1=(*(TopoDS_Edge*)(&aEx));
        aF1=(*(TopoDS_Face*)(&aFx));
        aMFP.Add(aFx);
        continue;
      }
      //
      if (aOr==aOrC) {
        aLCEFx.Append(aCSx);
        aMFP.Add(aFx);
      }
    }
    //
    // F2
    BOPTools_AlgoTools::GetFaceOff(aE1, aF1, aLCEFx, aF2, theContext);
    //
    aCFF.SetShape1(aF1);
    aCFF.SetShape2(aF2);
    theLCFF.Append(aCFF);
    //
    aMFP.Add(aF1);
    aMFP.Add(aF2);
    //
    // refine aLCEF
    aLCEFx.Clear();
    aLCEFx=aLCEF;
    aLCEF.Clear();
    aIt.Initialize(aLCEFx);
    for (; aIt.More(); aIt.Next()) {
      const BOPTools_CoupleOfShape& aCSx=aIt.Value();
      const TopoDS_Shape& aFx=aCSx.Shape2();
      if (!aMFP.Contains(aFx)) {
        aLCEF.Append(aCSx);
      }
    }
    //
    aNbCEF=aLCEF.Extent();
  }//while(aNbCEF) {
  //
  return bFound;
}
//=======================================================================
//function : AngleWithRef
//purpose  : 
//=======================================================================
Standard_Real AngleWithRef(const gp_Dir& theD1,
                           const gp_Dir& theD2,
                           const gp_Dir& theDRef)
{
  Standard_Real aCosinus, aSinus, aBeta, aHalfPI, aScPr;
  gp_XYZ aXYZ;
  //
  aHalfPI=0.5*M_PI;
  //
  const gp_XYZ& aXYZ1=theD1.XYZ();
  const gp_XYZ& aXYZ2=theD2.XYZ();
  aXYZ=aXYZ1.Crossed(aXYZ2);
  aSinus=aXYZ.Modulus();
  aCosinus=theD1*theD2;
  //
  aBeta=0.;
  if (aSinus>=0.) {
    aBeta=aHalfPI*(1.-aCosinus);
  }
  else {
    aBeta=2.*M_PI-aHalfPI*(3.+aCosinus);
  }
  //
  aScPr=aXYZ.Dot(theDRef.XYZ());
  if (aScPr<0.) {
    aBeta=-aBeta;
  }
  return aBeta;
}
//=======================================================================
// function: IsBlockInOnFace
// purpose: 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::IsBlockInOnFace
  (const IntTools_Range& aShrR,
   const TopoDS_Face& aF,
   const TopoDS_Edge& aE1,
   Handle(IntTools_Context)& aContext)
{
  Standard_Boolean bFlag;
  Standard_Real f1, l1, ULD, VLD;
  gp_Pnt2d aP2D;
  gp_Pnt aP11, aP12;
  //
  aShrR.Range(f1, l1);
  Standard_Real dt=0.0075,  k;//dt=0.001,  k;
  k=dt*(l1-f1);
  f1=f1+k;
  l1=l1-k;
  //
  // Treatment P11
  BOPTools_AlgoTools::PointOnEdge(aE1, f1, aP11);
  //
  GeomAPI_ProjectPointOnSurf& aProjector=aContext->ProjPS(aF);
  aProjector.Perform(aP11);
  //
  bFlag=aProjector.IsDone();
  if (!bFlag) {
    return bFlag;
  }
  
  aProjector.LowerDistanceParameters(ULD, VLD);
  aP2D.SetCoord(ULD, VLD);
  //
  bFlag=aContext->IsPointInOnFace (aF, aP2D);
  //
  if (!bFlag) {
    return bFlag;
  }
  //
  // Treatment P12
  BOPTools_AlgoTools::PointOnEdge(aE1, l1, aP12);
  //
  aProjector.Perform(aP12);
  //
  bFlag=aProjector.IsDone();
  if (!bFlag) {
    return bFlag;
  }
  
  aProjector.LowerDistanceParameters(ULD, VLD);
  aP2D.SetCoord(ULD, VLD);
  //
  bFlag=aContext->IsPointInOnFace (aF, aP2D);
  //
  if (!bFlag) {
    return bFlag;
  }
  //
  // Treatment intemediate
  Standard_Real m1, aTolF, aTolE, aTol, aDist;
  m1=IntTools_Tools::IntermediatePoint(f1, l1);
  BOPTools_AlgoTools::PointOnEdge(aE1, m1, aP12);
  //
  aProjector.Perform(aP12);
  //
  bFlag=aProjector.IsDone();
  if (!bFlag) {
    return bFlag;
  }
  //
  aTolE=BRep_Tool::Tolerance(aE1);
  aTolF=BRep_Tool::Tolerance(aF);
  aTol=aTolE+aTolF;
  aDist=aProjector.LowerDistance();
  if (aDist > aTol){
   return Standard_False;
  }

  aProjector.LowerDistanceParameters(ULD, VLD);
  aP2D.SetCoord(ULD, VLD);
  //
  bFlag=aContext->IsPointInOnFace (aF, aP2D);
  //
  if (!bFlag) {
    return bFlag;
  }
  return bFlag;
}

//=======================================================================
//function : IsMicroEdge
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::IsMicroEdge
  (const TopoDS_Edge& aE,
   const Handle(IntTools_Context)& aCtx,
   const Standard_Boolean bCheckSplittable)
{
  Standard_Boolean bRet;
  Standard_Real aT1, aT2, aTmp;
  Handle(Geom_Curve) aC3D;
  TopoDS_Vertex aV1, aV2;
  //
  bRet=(BRep_Tool::Degenerated(aE) ||
        !BRep_Tool::IsGeometric(aE));
  if (bRet) {
    return bRet;
  }
  //
  aC3D=BRep_Tool::Curve(aE, aT1, aT2);
  TopExp::Vertices(aE, aV1, aV2);
  aT1=BRep_Tool::Parameter(aV1, aE);
  aT2=BRep_Tool::Parameter(aV2, aE);
  if (aT2<aT1) {
    aTmp=aT1;
    aT1=aT2;
    aT2=aTmp;
  }
  //
  IntTools_ShrunkRange aSR;
  aSR.SetContext(aCtx);
  aSR.SetData(aE, aT1, aT2, aV1, aV2);
  aSR.Perform();
  bRet = !aSR.IsDone();
  if (!bRet && bCheckSplittable) {
    bRet = !aSR.IsSplittable();
  }
  //
  return bRet;
}

//=======================================================================
//function : GetFaceDir
//purpose  : Get binormal direction for the face in the point aP
//=======================================================================
Standard_Boolean GetFaceDir(const TopoDS_Edge& aE,
                            const TopoDS_Face& aF,
                            const gp_Pnt& aP,
                            const Standard_Real aT,
                            const gp_Dir& aDTgt,
                            const Standard_Boolean theSmallFaces,
                            gp_Dir& aDN,
                            gp_Dir& aDB,
                            Handle(IntTools_Context)& theContext,
                            GeomAPI_ProjectPointOnSurf& aProjPL,
                            const Standard_Real aDt)
{
  Standard_Real aTolE;
  gp_Pnt aPx;
  //
  BOPTools_AlgoTools3D::GetNormalToFaceOnEdge(aE, aF, aT, aDN, theContext);
  if (aF.Orientation()==TopAbs_REVERSED){
    aDN.Reverse();
  }
  //
  aTolE=BRep_Tool::Tolerance(aE);
  aDB = aDN^aDTgt;
  //
  // do not try to look for the point in the small face by intersecting
  // it with the circle because, most likely, the intersection point will
  // be out of the face
  Standard_Boolean bFound = !theSmallFaces &&
    FindPointInFace(aF, aP, aDB, aPx, theContext, aProjPL, aDt, aTolE);
  if (!bFound) {
    // if the first method did not succeed, try to use hatcher to find the point
    bFound = BOPTools_AlgoTools3D::GetApproxNormalToFaceOnEdge
      (aE, aF, aT, aDt, aPx, aDN, theContext);
    aProjPL.Perform(aPx);
    aPx = aProjPL.NearestPoint();
    gp_Vec aVec(aP, aPx);
    aDB.SetXYZ(aVec.XYZ());
  }
  //
  return bFound;
}
//=======================================================================
//function : FindPointInFace
//purpose  : Find a point in the face in direction of <aDB>.
//           To get this point the method intersects the circle with radius
//           <aDt> built in point <aP> with normal perpendicular to <aDB>.
//=======================================================================
Standard_Boolean FindPointInFace(const TopoDS_Face& aF,
                                 const gp_Pnt& aP,
                                 gp_Dir& aDB,
                                 gp_Pnt& aPOut,
                                 Handle(IntTools_Context)& theContext,
                                 GeomAPI_ProjectPointOnSurf& aProjPL,
                                 const Standard_Real aDt,
                                 const Standard_Real aTolE)
{
  Standard_Integer aNbItMax;
  Standard_Real aDist, aDTol, aPM, anEps;
  Standard_Boolean bRet;
  gp_Pnt aP1, aPS;
  //
  aDTol = Precision::Angular();
  aPM = aP.XYZ().Modulus();
  if (aPM > 1000.) {
    aDTol = 5.e-16 * aPM;
  }
  bRet = Standard_False;
  aNbItMax = 15;
  anEps = Precision::SquareConfusion();
  //
  GeomAPI_ProjectPointOnSurf& aProj=theContext->ProjPS(aF);
  //
  aPS=aP;
  aProj.Perform(aPS);
  if (!aProj.IsDone()) {
    return bRet;
  }
  aPS=aProj.NearestPoint();
  aProjPL.Perform(aPS);
  aPS=aProjPL.NearestPoint();
  //
  aPS.SetXYZ(aPS.XYZ()+2.*aTolE*aDB.XYZ());
  aProj.Perform(aPS);
  if (!aProj.IsDone()) {
    return bRet;
  }
  aPS=aProj.NearestPoint();
  aProjPL.Perform(aPS);
  aPS=aProjPL.NearestPoint();
  //
  do {
    aP1.SetXYZ(aPS.XYZ()+aDt*aDB.XYZ());
    //
    aProj.Perform(aP1);
    if (!aProj.IsDone()) {
      return bRet;
    }
    aPOut = aProj.NearestPoint();
    aDist = aProj.LowerDistance();
    //
    aProjPL.Perform(aPOut);
    aPOut = aProjPL.NearestPoint();
    //
    gp_Vec aV(aPS, aPOut);
    if (aV.SquareMagnitude() < anEps) {
      return bRet;
    }
    aDB.SetXYZ(aV.XYZ());
  } while (aDist > aDTol && --aNbItMax);
  //
  bRet = aDist < aDTol;
  return bRet;
}
//=======================================================================
//function : MinStep3D
//purpose  : 
//=======================================================================
Standard_Real MinStep3D(const TopoDS_Edge& theE1,
                        const TopoDS_Face& theF1,
                        const BOPTools_ListOfCoupleOfShape& theLCS,
                        const gp_Pnt& aP,
                        Handle(IntTools_Context)& theContext,
                        Standard_Boolean& theSmallFaces)
{
  Standard_Real aDt, aTolE, aTolF, aDtMax, aDtMin;
  //
  // add the current pair of edge/face for checking as well
  BOPTools_CoupleOfShape aCS1;
  aCS1.SetShape1(theE1);
  aCS1.SetShape2(theF1);
  //
  BOPTools_ListOfCoupleOfShape aLCS = theLCS;
  aLCS.Append(aCS1);
  //
  aTolE = BRep_Tool::Tolerance(theE1);
  aDtMax = -1.;
  aDtMin = 5.e-6;
  //
  BOPTools_ListIteratorOfListOfCoupleOfShape aIt(aLCS);
  for (; aIt.More(); aIt.Next()) {
    const BOPTools_CoupleOfShape& aCS = aIt.Value();
    const TopoDS_Face& aF = (*(TopoDS_Face*)(&aCS.Shape2()));
    //
    aTolF = BRep_Tool::Tolerance(aF);
    aDt = 2*(aTolE + aTolF);
    if (aDt > aDtMax) {
      aDtMax = aDt;
    }
    //
    // try to compute the minimal 3D step
    const BRepAdaptor_Surface& aBAS = theContext->SurfaceAdaptor(aF);
    Standard_Real aR = 0.;
    GeomAbs_SurfaceType aSType = aBAS.GetType();
    switch (aSType) {
    case GeomAbs_Cylinder: {
      aR = aBAS.Cylinder().Radius();
      break;
    }
    case GeomAbs_Cone: {
      gp_Lin aL(aBAS.Cone().Axis());
      aR = aL.Distance(aP);
      break;
    }
    case GeomAbs_Sphere: {
      aDtMin = Max(aDtMin, 5.e-4);
      aR = aBAS.Sphere().Radius();
      break;
    }
    case GeomAbs_Torus: {
      aR = aBAS.Torus().MajorRadius();
      break;
    }
    default:
      aDtMin = Max(aDtMin, 5.e-4);
      break;
    }
    //
    if (aR > 100.) {
      Standard_Real d = 10*Precision::PConfusion();
      aDtMin = Max(aDtMin, sqrt(d*d + 2*d*aR));
    }
  }
  //
  if (aDtMax < aDtMin) {
    aDtMax = aDtMin;
  }
  //
  // check if the computed 3D step is too big for any of the faces in the list
  aIt.Initialize(aLCS);
  for (; aIt.More(); aIt.Next()) {
    const BOPTools_CoupleOfShape& aCS = aIt.Value();
    const TopoDS_Face& aF = (*(TopoDS_Face*)(&aCS.Shape2()));
    //
    const BRepAdaptor_Surface& aBAS = theContext->SurfaceAdaptor(aF);
    //
    Standard_Real aUMin, aUMax, aVMin, aVMax;
    theContext->UVBounds(aF, aUMin, aUMax, aVMin, aVMax);
    //
    Standard_Real aDU = aUMax - aUMin;
    if (aDU > 0.) {
      Standard_Real aURes = aBAS.UResolution(aDtMax);
      if (2*aURes > aDU) {
        break;
      }
    }
    //
    Standard_Real aDV = aVMax - aVMin;
    if (aDV > 0.) {
      Standard_Real aVRes = aBAS.VResolution(aDtMax);
      if (2*aVRes > aDV) {
        break;
      }
    }
  }
  //
  theSmallFaces = aIt.More();
  //
  return aDtMax;
}
//=======================================================================
//function : IsOpenShell
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::IsOpenShell(const TopoDS_Shell& aSh)
{
  Standard_Boolean bRet;
  Standard_Integer i, aNbE, aNbF;
  TopAbs_Orientation aOrF;
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF; 
  BOPCol_ListIteratorOfListOfShape aItLS;
  //
  bRet=Standard_False;
  //
  BOPTools::MapShapesAndAncestors(aSh, TopAbs_EDGE, TopAbs_FACE, aMEF);
  // 
  aNbE=aMEF.Extent();
  for (i=1; i<=aNbE; ++i) {
    const TopoDS_Edge& aE=*((TopoDS_Edge*)&aMEF.FindKey(i));
    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //
    aNbF=0;
    const BOPCol_ListOfShape& aLF=aMEF(i);
    aItLS.Initialize(aLF);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aF=aItLS.Value();
      aOrF=aF.Orientation();
      if (aOrF==TopAbs_INTERNAL || aOrF==TopAbs_EXTERNAL) {
        continue;
      }
      ++aNbF;
    }
    //
    if (aNbF==1) {
      bRet=!bRet; // True
      break;
    }
  }
  //
  return bRet;
}
//=======================================================================
//function : IsInvertedSolid
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::IsInvertedSolid
  (const TopoDS_Solid& aSolid)
{
  Standard_Real aTolS;
  TopAbs_State aState;
  BRepClass3d_SolidClassifier aSC(aSolid);
  //
  aTolS=1.e-7;
  aSC.PerformInfinitePoint(aTolS);
  aState=aSC.State();
  return (aState==TopAbs_IN); 
}
