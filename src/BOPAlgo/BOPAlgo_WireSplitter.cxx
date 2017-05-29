// Created by: Peter KURNEV
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
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
#include <BOPAlgo_Alerts.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_WireSplitter::BOPAlgo_WireSplitter()
:
  BOPAlgo_Algo(),
  myWES(NULL),
  myLCB(myAllocator)
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_WireSplitter::BOPAlgo_WireSplitter
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Algo(theAllocator),
  myWES(NULL),
  myLCB(myAllocator)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_WireSplitter::~BOPAlgo_WireSplitter()
{
}
//=======================================================================
//function : SetWES
//purpose  : 
//=======================================================================
void BOPAlgo_WireSplitter::SetWES(const BOPAlgo_WireEdgeSet& theWES)
{
  myWES=(BOPAlgo_WireEdgeSet*)&theWES;
}
//=======================================================================
//function : WES
//purpose  : 
//=======================================================================
BOPAlgo_WireEdgeSet& BOPAlgo_WireSplitter::WES()
{
  return *myWES;
}
//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================
void BOPAlgo_WireSplitter::SetContext(const Handle(IntTools_Context)& theContext)
{
  myContext = theContext;
}
//=======================================================================
//function : Context
//purpose  : 
//=======================================================================
const Handle(IntTools_Context)& BOPAlgo_WireSplitter::Context()
{
  return myContext;
}
//=======================================================================
// function: CheckData
// purpose: 
//=======================================================================
void BOPAlgo_WireSplitter::CheckData()
{
  if (!myWES) {
    AddError (new BOPAlgo_AlertNullInputShapes);
    return;
  }
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_WireSplitter::Perform()
{
  GetReport()->Clear();
  //
  CheckData();
  if (HasErrors()) {
    return;
  }
  //
  // create a context
  if (myContext.IsNull()) {
    myContext = new IntTools_Context;
  }
  //
  MakeConnexityBlocks();
  MakeWires();
}

//=======================================================================
//function : MakeConnexityBlocks
//purpose  : 
//=======================================================================
void BOPAlgo_WireSplitter::MakeConnexityBlocks()
{
  Standard_Boolean bRegular, bClosed;
  Standard_Integer i, j, aNbV, aNbVS, aNbVP, k;
  TopoDS_Iterator aItE;
  TopoDS_Shape aER;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  BOPCol_IndexedDataMapOfShapeListOfShape aMVE(100, myAllocator);
  BOPCol_IndexedMapOfShape aMVP(100, myAllocator);
  BOPCol_IndexedMapOfShape aMEC(100, myAllocator);
  BOPCol_MapOfShape aMER(100, myAllocator);
  BOPCol_MapOfShape aMEP(100, myAllocator);
  BOPCol_IndexedMapOfShape aMVAdd(100, myAllocator);
  BOPCol_MapOfShape aMVS(100, myAllocator);
  //
  myLCB.Clear();
  //
  const BOPCol_ListOfShape& aLSE=myWES->StartElements();
  aIt.Initialize(aLSE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE=aIt.Value();
    if (aMEP.Add(aE)) {
      BOPTools::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
    }
    else {
      aMER.Add(aE);
    }
  }
  //
  // 2
  aNbV=aMVE.Extent();
  for (i=1; i<=aNbV; ++i) {
    aNbVS=aMVS.Extent();
    if (aNbVS==aNbV) {
      break;
    }
    //
    const TopoDS_Shape& aV=aMVE.FindKey(i);
    //
    if (!aMVS.Add(aV)) {
      continue;
    }
        // aMVS - globally processed vertices
    //
    //------------------------------------- goal: aMEC
    aMEC.Clear();    // aMEC - edges of CB
    aMVP.Clear();    // aMVP - vertices to process right now 
    aMVAdd.Clear();  // aMVAdd vertices to process on next step of while(1)
    //
    aMVP.Add(aV);
    //
    for(;;) {
      aNbVP=aMVP.Extent();
      for (k=1; k<=aNbVP; ++k) {
        const TopoDS_Shape& aVP=aMVP(k);
        const BOPCol_ListOfShape& aLE=aMVE.FindFromKey(aVP);
        aIt.Initialize(aLE);
        for (; aIt.More(); aIt.Next()) {
          const TopoDS_Shape& aE=aIt.Value();
          if (aMEC.Add(aE)) {
            aItE.Initialize(aE);
            for (; aItE.More(); aItE.Next()) {
              const TopoDS_Shape& aVE=aItE.Value();
              if (aMVS.Add(aVE)) {
                aMVAdd.Add(aVE);
              }
            }
          }
        }
      }//for (k=1; k<=aNbVP; ++k) {
      //
      aNbVP=aMVAdd.Extent();
      if (!aNbVP) {
        break; // from while(1)
      }
      //
      aMVP.Clear();
      //
      for (k=1; k<=aNbVP; ++k) {
        const TopoDS_Shape& aVE=aMVAdd(k);
        aMVP.Add(aVE);
      }
      aMVAdd.Clear();
    }// while(1) {

    //-------------------------------------
    BOPTools_ConnexityBlock aCB(myAllocator);
    BOPCol_ListOfShape& aLEC=aCB.ChangeShapes();
    
    BOPCol_IndexedDataMapOfShapeListOfShape aMVER(100, myAllocator);
    //
    bRegular=Standard_True;
    Standard_Integer aNbCB = aMEC.Extent();
    for (j = 1; j <= aNbCB; j++) {
      aER = aMEC(j);
      //
      if (aMER.Contains(aER)) {
        aER.Orientation(TopAbs_FORWARD);
        aLEC.Append(aER);
        aER.Orientation(TopAbs_REVERSED);
        aLEC.Append(aER);
        bRegular=Standard_False;
      }
      else {
        aLEC.Append(aER);
      }
      //
      if (bRegular) {
        BOPTools::MapShapesAndAncestors(aER, TopAbs_VERTEX, TopAbs_EDGE, aMVER);
      }
    }
    //
    if (bRegular) {
      Standard_Integer aNbVR, aNbER;
      //
      aNbVR=aMVER.Extent();
      for (k=1; k<=aNbVR; ++k) {
        const BOPCol_ListOfShape& aLER=aMVER(k);
        aNbER=aLER.Extent();
        if (aNbER==1) {
          const TopoDS_Edge& aEx=TopoDS::Edge(aER);
          bClosed=BRep_Tool::IsClosed(aEx, myWES->Face());
          if (!bClosed) {
            bRegular=!bRegular;
            break;
          }
        }
        if (aNbER>2) {
          bRegular=!bRegular;
          break;
        }
      }
    }
    //
    aCB.SetRegular(bRegular);
    myLCB.Append(aCB);
  }
}
/////////////////////////////////////////////////////////////////////////
class BOPAlgo_WS_ConnexityBlock {
public:
  BOPAlgo_WS_ConnexityBlock() {};
  ~BOPAlgo_WS_ConnexityBlock() {};

  void SetFace(const TopoDS_Face& theF) {
    myFace = theF;
  }

  const TopoDS_Face& Face() const {
    return myFace;
  }

  void SetConnexityBlock(const BOPTools_ConnexityBlock& theCB) {
    myCB = theCB;
  }

  const BOPTools_ConnexityBlock& ConnexityBlock() const {
    return myCB;
  }

  void SetContext(const Handle(IntTools_Context)& aContext) {
    myContext = aContext;
  }
  //
  const Handle(IntTools_Context)& Context()const {
    return myContext;
  }

  void Perform() {
    BOPAlgo_WireSplitter::SplitBlock(myFace, myCB, myContext);
  }

protected:
  TopoDS_Face myFace;
  BOPTools_ConnexityBlock myCB;
  Handle(IntTools_Context) myContext;
};

typedef BOPCol_NCVector<BOPAlgo_WS_ConnexityBlock> \
  BOPAlgo_VectorOfConnexityBlock;
//
typedef BOPCol_ContextFunctor
  <BOPAlgo_WS_ConnexityBlock,
  BOPAlgo_VectorOfConnexityBlock,
  Handle(IntTools_Context),
  IntTools_Context> BOPAlgo_SplitBlockFunctor;
//
typedef BOPCol_ContextCnt
  <BOPAlgo_SplitBlockFunctor,
  BOPAlgo_VectorOfConnexityBlock,
  Handle(IntTools_Context)> BOPAlgo_SplitBlockCnt;

//=======================================================================
//function : MakeWires
//purpose  : 
//=======================================================================
void BOPAlgo_WireSplitter::MakeWires()
{
  Standard_Boolean bIsRegular;
  Standard_Integer aNbVCB, k;
  TopoDS_Wire aW;
  BOPTools_ListIteratorOfListOfConnexityBlock aItCB;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPAlgo_VectorOfConnexityBlock aVCB;
  //
  const TopoDS_Face& aF=myWES->Face();
  //
  aItCB.Initialize(myLCB);
  for (; aItCB.More(); aItCB.Next()) {
    BOPTools_ConnexityBlock& aCB=aItCB.ChangeValue();
    bIsRegular=aCB.IsRegular();
    if (bIsRegular) {
      BOPCol_ListOfShape& aLE=aCB.ChangeShapes();
      BOPAlgo_WireSplitter::MakeWire(aLE, aW);
      myWES->AddShape(aW);
    }
    else {
      BOPAlgo_WS_ConnexityBlock& aWSCB = aVCB.Append1();
      aWSCB.SetFace(aF);
      aWSCB.SetConnexityBlock(aCB);
    }
  }
  //===================================================
  BOPAlgo_SplitBlockCnt::Perform(myRunParallel, aVCB, myContext);
  //===================================================
  aNbVCB=aVCB.Extent();
  for (k=0; k<aNbVCB; ++k) {
    const BOPAlgo_WS_ConnexityBlock& aCB=aVCB(k);
    const BOPCol_ListOfShape& aLW=aCB.ConnexityBlock().Loops();
    aIt.Initialize(aLW);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aWx=aIt.Value();
      myWES->AddShape(aWx);
    }
  }
}
