// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BOPAlgo_BuilderFace.ixx>

#include <gp_Pnt2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <Geom_Surface.hxx>

#include <TopAbs.hxx>
#include <TopLoc_Location.hxx>

#include <TopoDS_Iterator.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <IntTools_FClass2d.hxx>
#include <BOPInt_Context.hxx>

//
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPAlgo_WireEdgeSet.hxx>
//
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPTools.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPAlgo_WireSplitter.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_DataMapOfShapeListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>


static
  Standard_Boolean IsGrowthWire(const TopoDS_Shape& ,
                                const BOPCol_IndexedMapOfShape& );

static 
  Standard_Boolean IsInside(const TopoDS_Shape& ,
                            const TopoDS_Shape& ,
                            Handle(BOPInt_Context)& );
static
  void MakeInternalWires(const BOPCol_MapOfShape& ,
                         BOPCol_ListOfShape& );

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPAlgo_BuilderFace::BOPAlgo_BuilderFace()
:
  BOPAlgo_BuilderArea()
{
  myOrientation=TopAbs_EXTERNAL;
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPAlgo_BuilderFace::BOPAlgo_BuilderFace(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_BuilderArea(theAllocator)
{ 
  myOrientation=TopAbs_EXTERNAL;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  BOPAlgo_BuilderFace::~BOPAlgo_BuilderFace()
{
}
//=======================================================================
//function : SetFace
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderFace::SetFace(const TopoDS_Face& theFace)
{
  myOrientation=theFace.Orientation();
  myFace=theFace;
  myFace.Orientation(TopAbs_FORWARD);
}
//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================
TopAbs_Orientation BOPAlgo_BuilderFace::Orientation()const
{
  return myOrientation;
}
//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
  const TopoDS_Face& BOPAlgo_BuilderFace::Face()const
{
  return myFace;
}
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderFace::CheckData()
{
  myErrorStatus=0;
  //
  if (myContext.IsNull()) {
    //myErrorStatus=11;// Null Context
    //return;
    myContext = new BOPInt_Context;
  }
  //
  if (myFace.IsNull()) {
    myErrorStatus=12;// Null face generix
    return;
  }
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderFace::Perform()
{
  myErrorStatus=0;
  //
  CheckData();
  if (myErrorStatus) {
    return;
  }
  //
  PerformShapesToAvoid();
  if (myErrorStatus) {
    return;
  }
  //
  PerformLoops();
  if (myErrorStatus) {
    return;
  }
  //
  PerformAreas();
  if (myErrorStatus) {
    return;
  }
  //
  PerformInternalShapes();
  if (myErrorStatus) {
    return;
  }
}
//=======================================================================
//function :PerformShapesToAvoid
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderFace::PerformShapesToAvoid()
{
  Standard_Boolean bFound;
  Standard_Integer i, iCnt, aNbV, aNbE;
  BOPCol_IndexedDataMapOfShapeListOfShape aMVE;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  myShapesToAvoid.Clear();
  //
  iCnt=0;
  for(;;) {
    ++iCnt;
    bFound=Standard_False;
    //
    // 1. MEF
    aMVE.Clear();
    aIt.Initialize (myShapes);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE=aIt.Value();
      if (!myShapesToAvoid.Contains(aE)) {
        BOPTools::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
      }
      //else {
	//int a=0;
      //}
    }
    aNbV=aMVE.Extent();
    //
    // 2. myEdgesToAvoid
    for (i=1; i<=aNbV; ++i) {
      const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&aMVE.FindKey(i)));
      //
      BOPCol_ListOfShape& aLE=aMVE.ChangeFromKey(aV);
      aNbE=aLE.Extent();
      if (!aNbE) {
        continue;
      }
      //
      const TopoDS_Edge& aE1=(*(TopoDS_Edge *)(&aLE.First()));
      if (aNbE==1) {
        if (BRep_Tool::Degenerated(aE1)) {
          continue;
        }
        if (aV.Orientation()==TopAbs_INTERNAL) {
          continue;
        }
        bFound=Standard_True;
        myShapesToAvoid.Add(aE1);
      }
      else if (aNbE==2) {
        const TopoDS_Edge& aE2=(*(TopoDS_Edge *)(&aLE.Last()));
        if (aE2.IsSame(aE1)) {
          TopoDS_Vertex aV1x, aV2x;
          //
          TopExp::Vertices(aE1, aV1x, aV2x);
          if (aV1x.IsSame(aV2x)) {
            continue;
          }
          bFound=Standard_True;
          myShapesToAvoid.Add(aE1);
          myShapesToAvoid.Add(aE2);
        }
      }
    }// for (i=1; i<=aNbE; ++i) {
    //
    if (!bFound) {
      break;
    }
    //
  }//while (1) 
  //printf(" EdgesToAvoid=%d, iCnt=%d\n", EdgesToAvoid.Extent(), iCnt);
}  
//=======================================================================
//function : PerformLoops
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderFace::PerformLoops()
{
  myErrorStatus=0;
  //
  Standard_Boolean bFlag;
  Standard_Integer iErr, aNbEA;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPCol_MapIteratorOfMapOfOrientedShape aItM;
  BOPCol_IndexedDataMapOfShapeListOfShape aVEMap;
  BOPCol_MapOfOrientedShape aMAdded;
  TopoDS_Iterator aItW;
  BRep_Builder aBB; 
  BOPAlgo_WireEdgeSet aWES(myAllocator);
  BOPAlgo_WireSplitter aWSp(myAllocator);
  //
  // 1. 
  myLoops.Clear();
  aWES.SetFace(myFace);
  //
  aIt.Initialize(myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE=aIt.Value();
    if (!myShapesToAvoid.Contains(aE)) {
      aWES.AddStartElement(aE);
    }
  }
  //
  aWSp.SetWES(aWES);
  aWSp.Perform();
  iErr=aWSp.ErrorStatus();
  if (iErr) {
    return;
  }
  //
  const BOPCol_ListOfShape& aLW=aWES.Shapes();
  aIt.Initialize (aLW);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aW=aIt.Value();
    myLoops.Append(aW);
  }
  // Post Treatment
  BOPCol_MapOfOrientedShape aMEP;
  // 
  // a. collect all edges that are in loops
  aIt.Initialize (myLoops);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aW=aIt.Value();
    aItW.Initialize(aW);
    for (; aItW.More(); aItW.Next()) {
      const TopoDS_Shape& aE=aItW.Value();
      aMEP.Add(aE);
    }
  }
  // 
  // b. collect all edges that are to avoid
  aItM.Initialize(myShapesToAvoid);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aE=aItM.Key();
    aMEP.Add(aE);
  }
  //
  // c. add all edges that are not processed to myShapesToAvoid
  aIt.Initialize (myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE=aIt.Value();
    if (!aMEP.Contains(aE)) {
      myShapesToAvoid.Add(aE);
    }
  }
  //
  // 2. Internal Wires
  myLoopsInternal.Clear();
  //
  aNbEA=myShapesToAvoid.Extent();
  aItM.Initialize(myShapesToAvoid);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aEE=aItM.Key();
    BOPTools::MapShapesAndAncestors(aEE, TopAbs_VERTEX, TopAbs_EDGE, aVEMap);
  }
  //
  bFlag=Standard_True;
  aItM.Initialize(myShapesToAvoid);
  for (; aItM.More()&&bFlag; aItM.Next()) {
    const TopoDS_Shape& aEE=aItM.Key();
    if (!aMAdded.Add(aEE)) {
      continue;
    }
    //
    // make new wire
    TopoDS_Wire aW;
    aBB.MakeWire(aW);
    aBB.Add(aW, aEE);
    //
    aItW.Initialize(aW);
    for (; aItW.More()&&bFlag; aItW.Next()) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aItW.Value()));
      //
      TopoDS_Iterator aItE(aE);
      for (; aItE.More()&&bFlag; aItE.Next()) {
        const TopoDS_Vertex& aV = (*(TopoDS_Vertex *)(&aItE.Value()));
        const BOPCol_ListOfShape& aLE=aVEMap.FindFromKey(aV);
        aIt.Initialize(aLE);
        for (; aIt.More()&&bFlag; aIt.Next()) { 
          const TopoDS_Shape& aEx=aIt.Value();
          if (aMAdded.Add(aEx)) {
            aBB.Add(aW, aEx);
            if(aMAdded.Extent()==aNbEA) {
              bFlag=!bFlag;
            }
          }
        }//for (; aIt.More(); aIt.Next()) { 
      }//for (; aItE.More(); aItE.Next()) {
    }//for (; aItW.More(); aItW.Next()) {
    myLoopsInternal.Append(aW);
  }//for (; aItM.More(); aItM.Next()) {
}
//=======================================================================
//function : PerformAreas
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderFace::PerformAreas()
{
  myErrorStatus=0;
  //
  Standard_Boolean bIsGrowth, bIsHole;
  Standard_Real aTol;
  TopoDS_Shape anInfinitePointShape;
  //
  BOPCol_ListOfShape aNewFaces, aHoleWires; 
  BOPCol_DataMapOfShapeShape aInOutMap;
  BOPCol_DataMapOfShapeListOfShape aMSH;
  BOPCol_IndexedMapOfShape aMHE;

  BOPCol_DataMapIteratorOfDataMapOfShapeListOfShape aItMSH;
  BOPCol_ListIteratorOfListOfShape aIt1, aIt2;
  BRep_Builder aBB;
  Handle(Geom_Surface) aS;
  TopLoc_Location aLoc;
  //
  aTol=BRep_Tool::Tolerance(myFace);
  aS=BRep_Tool::Surface(myFace, aLoc);
  //
  myAreas.Clear();
  //
  //  Draft faces [aNewFaces]
  aIt1.Initialize(myLoops);
  for ( ; aIt1.More(); aIt1.Next()) {
    const TopoDS_Shape& aWire=aIt1.Value();
    //
    bIsGrowth=IsGrowthWire(aWire, aMHE);
    if (bIsGrowth) {
      // make a growth face from a wire
      TopoDS_Face aFace;
      aBB.MakeFace(aFace, aS, aLoc, aTol);
      aBB.Add (aFace, aWire);
      //
      aNewFaces.Append (aFace);
    }
    else{
      // check if a wire is a hole 
      //XX
      TopoDS_Face aFace;
      aBB.MakeFace(aFace, aS, aLoc, aTol);
      aBB.Add (aFace, aWire);
      //
      IntTools_FClass2d& aClsf=myContext->FClass2d(aFace);
      aClsf.Init(aFace, aTol);
      //
      bIsHole=aClsf.IsHole();
      //
      //bIsHole=BOPTools_AlgoTools::IsHole(aWire, myFace); 
      //XX
      if (bIsHole) {
        aHoleWires.Append(aWire);
        BOPTools::MapShapes(aWire, TopAbs_EDGE, aMHE);
      }
      else {
        // make a growth face from a wire
        TopoDS_Face aFace;
        aBB.MakeFace(aFace, aS, aLoc, aTol);
        aBB.Add (aFace, aWire);
        //
        aNewFaces.Append (aFace);
      }
    }
  }
  //
  // 2. Find outer growth shell that is most close to each hole shell
  aIt2.Initialize(aHoleWires);
  for (; aIt2.More(); aIt2.Next()) {
    const TopoDS_Shape& aHole = aIt2.Value();
    //
    aIt1.Initialize(aNewFaces);
    for ( ; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aF=aIt1.Value();
      //
      if (!IsInside(aHole, aF, myContext)){
        continue;
      }
      //
      if ( aInOutMap.IsBound (aHole)){
        const TopoDS_Shape& aF2=aInOutMap(aHole);
        if (IsInside(aF, aF2, myContext)) {
          aInOutMap.UnBind(aHole);
          aInOutMap.Bind (aHole, aF);
        }
      }
      else{
        aInOutMap.Bind (aHole, aF);
      }
    }
    //
    // Add aHole to a map Face/ListOfHoles [aMSH]
    if (aInOutMap.IsBound(aHole)){
      const TopoDS_Shape& aF=aInOutMap(aHole);
      if (aMSH.IsBound(aF)) {
        BOPCol_ListOfShape& aLH=aMSH.ChangeFind(aF);
        aLH.Append(aHole);
      }
      else {
        BOPCol_ListOfShape aLH;
        aLH.Append(aHole);
        aMSH.Bind(aF, aLH);
      }
    }
  }// for (; aIt2.More(); aIt2.Next())
  //
  // 3. Add aHoles to Faces
  aItMSH.Initialize(aMSH);
  for (; aItMSH.More(); aItMSH.Next()) {
    TopoDS_Face aF=(*(TopoDS_Face *)(&aItMSH.Key()));
    //
    const BOPCol_ListOfShape& aLH=aItMSH.Value();
    aIt2.Initialize(aLH);
    for (; aIt2.More(); aIt2.Next()) {
      const TopoDS_Shape& aHole = aIt2.Value();
      aBB.Add (aF, aHole);
    }
    //
    // update classifier 
    aTol=BRep_Tool::Tolerance(aF);
    IntTools_FClass2d& aClsf=myContext->FClass2d(aF);
    aClsf.Init(aF, aTol);
  }
  //
  // These aNewFaces are draft faces that 
  // do not contain any internal shapes
  //
  aIt1.Initialize(aNewFaces);
  for ( ; aIt1.More(); aIt1.Next()) {
    const TopoDS_Shape& aF=aIt1.Value();
    myAreas.Append(aF);
  }
}
//=======================================================================
//function : PerformInternalShapes
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderFace::PerformInternalShapes()
{
  myErrorStatus=0;
  //
  Standard_Integer aNbWI=myLoopsInternal.Extent();
  if (!aNbWI) {// nothing to do
    return;
  }
  // 
  //Standard_Real aTol;
  BRep_Builder aBB;
  BOPCol_ListIteratorOfListOfShape aIt1, aIt2;
  TopoDS_Iterator aIt; 
  BOPCol_MapOfShape aME, aMEP;
  BOPCol_MapIteratorOfMapOfShape aItME;
  BOPCol_IndexedDataMapOfShapeListOfShape aMVE;
  BOPCol_ListOfShape aLSI;
  //
  // 1. All internal edges
  aIt1.Initialize(myLoopsInternal);
  for (; aIt1.More(); aIt1.Next()) {
    const TopoDS_Shape& aWire=aIt1.Value();
    aIt.Initialize(aWire);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE=aIt.Value();
      aME.Add(aE);
    }
  }
  aNbWI=aME.Extent();
  //
  // 2 Process faces
  aIt2.Initialize(myAreas);
  for ( ; aIt2.More(); aIt2.Next()) {
    TopoDS_Face& aF=(*(TopoDS_Face *)(&aIt2.Value()));
    //
    aMVE.Clear();
    BOPTools::MapShapesAndAncestors(aF, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
    //
    // 2.1 Separate faces to process aMEP
    aMEP.Clear();
    aItME.Initialize(aME);
    for (; aItME.More(); aItME.Next()) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aItME.Key()));
      if (IsInside(aE, aF, myContext)) {
        aMEP.Add(aE);
      }
    }
    //
    // 2.2 Make Internal Wires
    aLSI.Clear();
    MakeInternalWires(aMEP, aLSI);
    //
    // 2.3 Add them to aF
    aIt1.Initialize(aLSI);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aSI=aIt1.Value();
      aBB.Add (aF, aSI);
    }
    //
    // 2.4 Remove faces aMFP from aMF
    aItME.Initialize(aMEP);
    for (; aItME.More(); aItME.Next()) {
      const TopoDS_Shape& aE=aItME.Key();
      aME.Remove(aE);
    }
    //
    aNbWI=aME.Extent();
    if (!aNbWI) {
      break;
    }
  } //for ( ; aIt2.More(); aIt2.Next()) {
}
//=======================================================================
//function : MakeInternalWires
//purpose  : 
//=======================================================================
void MakeInternalWires(const BOPCol_MapOfShape& theME,
                       BOPCol_ListOfShape& theWires)
{
  BOPCol_MapIteratorOfMapOfShape aItM;
  BOPCol_MapOfShape aAddedMap;
  BOPCol_ListIteratorOfListOfShape aItE;
  BOPCol_IndexedDataMapOfShapeListOfShape aMVE;
  BRep_Builder aBB;
  //
  aItM.Initialize(theME);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aE=aItM.Key();
    BOPTools::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  }
  //
  aItM.Initialize(theME);
  for (; aItM.More(); aItM.Next()) {
    TopoDS_Shape aEE=aItM.Key();
    if (!aAddedMap.Add(aEE)) {
      continue;
    }
    //
    // make a new shell
    TopoDS_Wire aW;
    aBB.MakeWire(aW);    
    aEE.Orientation(TopAbs_INTERNAL);
    aBB.Add(aW, aEE);
    //
    TopoDS_Iterator aItAdded (aW);
    for (; aItAdded.More(); aItAdded.Next()) {
      const TopoDS_Shape& aE =aItAdded.Value();
      //
      TopExp_Explorer aExp(aE, TopAbs_VERTEX);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aV =aExp.Current();
        const BOPCol_ListOfShape& aLE=aMVE.FindFromKey(aV);
        aItE.Initialize(aLE);
        for (; aItE.More(); aItE.Next()) { 
          TopoDS_Shape aEL=aItE.Value();
          if (aAddedMap.Add(aEL)){
            aEL.Orientation(TopAbs_INTERNAL);
            aBB.Add(aW, aEL);
          }
        }
      }
    }
    theWires.Append(aW);
  }
}
//=======================================================================
//function : IsInside
//purpose  : 
//=======================================================================
Standard_Boolean IsInside(const TopoDS_Shape& theHole,
                          const TopoDS_Shape& theF2,
                          Handle(BOPInt_Context)& theContext)
{
  Standard_Boolean bRet;
  Standard_Real aT, aU, aV;
  
  TopAbs_State aState;
  TopExp_Explorer aExp;
  BOPCol_IndexedMapOfShape aME2;
  gp_Pnt2d aP2D;
  //
  bRet=Standard_False;
  aState=TopAbs_UNKNOWN;
  const TopoDS_Face& aF2=(*(TopoDS_Face *)(&theF2));
  //
  BOPTools::MapShapes(aF2, TopAbs_EDGE, aME2);//AA
  //
  aExp.Init(theHole, TopAbs_EDGE);
  if (aExp.More()) {
    const TopoDS_Edge& aE =(*(TopoDS_Edge *)(&aExp.Current()));
    if (aME2.Contains(aE)) {
      return bRet;
    }
    if (!BRep_Tool::Degenerated(aE)) {
      //
      aT=BOPTools_AlgoTools2D::IntermediatePoint(aE);
      BOPTools_AlgoTools2D::PointOnSurface(aE, aF2, aT, aU, aV);
      aP2D.SetCoord(aU, aV);
      //
      IntTools_FClass2d& aClsf=theContext->FClass2d(aF2);
      aState=aClsf.Perform(aP2D);
      bRet=(aState==TopAbs_IN);
    }
  }
  //
  return bRet;
}

//=======================================================================
//function : IsGrowthWire
//purpose  : 
//=======================================================================
Standard_Boolean IsGrowthWire(const TopoDS_Shape& theWire,
                              const BOPCol_IndexedMapOfShape& theMHE)
{
  Standard_Boolean bRet;
  TopoDS_Iterator aIt;
  // 
  bRet=Standard_False;
  if (theMHE.Extent()) {
    aIt.Initialize(theWire);
    for(; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE=aIt.Value();
      if (theMHE.Contains(aE)) {
        return !bRet;
      }
    }
  }
  return bRet;
}

//BRepTools::Write(aFF, "ff");
//
//  ErrorStatus :
// 11 - Null Context
// 12 - Null face generix
