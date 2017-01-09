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


#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Tools.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <Precision.hxx>

//=======================================================================
//class    : BOPAlgo_VertexEdgeEdge
//purpose  : 
//=======================================================================
class BOPAlgo_VertexEdge : public BOPAlgo_Algo {

 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_VertexEdge() : 
    BOPAlgo_Algo(),
    myIV(-1), myIE(-1), myFlag(-1), myT(-1.), myTolVNew(-1.) {
  };
  //
  virtual ~BOPAlgo_VertexEdge(){
  };
  //
  void SetIndices(const Standard_Integer nV,
                  const Standard_Integer nE) {
    myIV=nV;
    myIE=nE;
  }
  //
  void Indices(Standard_Integer& nV,
               Standard_Integer& nE) const {
    nV=myIV;
    nE=myIE;
  }
  //
  void SetVertex(const TopoDS_Vertex& aV) {
    myV=aV;
  }
  //
  void SetEdge(const TopoDS_Edge& aE) {
    myE=aE;
  }
  //
  const TopoDS_Vertex& Vertex() const {
    return myV;
  }
  //
  const TopoDS_Edge& Edge() const {
    return myE;
  }
  //
  Standard_Integer Flag()const {
    return myFlag;
  }
  //
  Standard_Real Parameter()const {
    return myT;
  }
  //
  Standard_Real VertexNewTolerance()const {
    return myTolVNew;
  }
  //
  void SetContext(const Handle(IntTools_Context)& aContext) {
    myContext=aContext;
  }
  //
  const Handle(IntTools_Context)& Context()const {
    return myContext;
  }
  //
  virtual void Perform() {
    BOPAlgo_Algo::UserBreak();
    myFlag=myContext->ComputeVE (myV, myE, myT, myTolVNew, myFuzzyValue);
  };
  //
 protected:
  Standard_Integer myIV;
  Standard_Integer myIE;
  Standard_Integer myFlag;
  Standard_Real myT;
  Standard_Real myTolVNew;
  TopoDS_Vertex myV;
  TopoDS_Edge myE;
  Handle(IntTools_Context) myContext;
};
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_VertexEdge> BOPAlgo_VectorOfVertexEdge; 
//
typedef BOPCol_ContextFunctor 
  <BOPAlgo_VertexEdge,
  BOPAlgo_VectorOfVertexEdge,
  Handle(IntTools_Context), 
  IntTools_Context> BOPAlgo_VertexEdgeFunctor;
//
typedef BOPCol_ContextCnt 
  <BOPAlgo_VertexEdgeFunctor,
  BOPAlgo_VectorOfVertexEdge,
  Handle(IntTools_Context)> BOPAlgo_VertexEdgeCnt;
//
//=======================================================================
// function: PerformVE
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformVE()
{
  Standard_Integer iSize, nV, nE, nVSD, iFlag, nVx,  k, aNbVE;
  Standard_Real aT, aT1, aT2, aTS1, aTS2;
  BOPDS_Pave aPave;
  BOPDS_Pair aPK;
  BOPDS_MapOfPair aMPK;
  BOPAlgo_VectorOfVertexEdge aVVE;
  //
  myErrorStatus=0;
  //
  FillShrunkData(TopAbs_VERTEX, TopAbs_EDGE);
  //
  myIterator->Initialize(TopAbs_VERTEX, TopAbs_EDGE);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  BOPDS_VectorOfInterfVE& aVEs=myDS->InterfVE();
  aVEs.SetIncrement(iSize);
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nV, nE);
    //
    const BOPDS_ShapeInfo& aSIE=myDS->ShapeInfo(nE);
    if (aSIE.HasSubShape(nV)) {
      continue;
    }
    //
    if (aSIE.HasFlag()){
      continue;
    }
    //
    if (myDS->HasInterfShapeSubShapes(nV, nE)) {
      myDS->ChangePaveBlocks(nE);
      continue;
    }
    //
    nVx=nV;
    if (myDS->HasShapeSD(nV, nVSD)) {
      nVx=nVSD;
    }
    //
    aPK.SetIndices(nVx, nE);
    if (!aMPK.Add(aPK)) {
      continue;
    }
    //
    const BOPDS_ListOfPaveBlock& aLPB = myDS->PaveBlocks(nE);
    if (aLPB.IsEmpty()) {
      continue;
    }
    //
    const Handle(BOPDS_PaveBlock)& aPB = aLPB.First();
    if (!aPB->IsSplittable()) {
      // this is a micro edge, ignore it
      continue;
    }
    //
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aSIE.Shape())); 
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nVx))); 
    //
    BOPAlgo_VertexEdge& aVESolver=aVVE.Append1();
    //
    aVESolver.SetIndices(nV, nE);
    aVESolver.SetVertex(aV);
    aVESolver.SetEdge(aE);
    aVESolver.SetFuzzyValue(myFuzzyValue);
    aVESolver.SetProgressIndicator(myProgressIndicator);
    //
  }// for (; myIterator->More(); myIterator->Next()) {
  //
  aNbVE=aVVE.Extent();
  //=============================================================
  BOPAlgo_VertexEdgeCnt::Perform(myRunParallel, aVVE, myContext);
  //=============================================================
  //
  for (k=0; k < aNbVE; ++k) {
    const BOPAlgo_VertexEdge& aVESolver=aVVE(k);
    iFlag=aVESolver.Flag();
    if (!iFlag) {
      aVESolver.Indices(nV, nE);
      aT=aVESolver.Parameter();
      // 
      // check if vertex hits beyond shrunk range, in such case create V-V interf
      const BOPDS_ListOfPaveBlock& aLPB = myDS->PaveBlocks(nE);
      const Handle(BOPDS_PaveBlock)& aPB = aLPB.First();
      Bnd_Box aBox;
      Standard_Boolean bIsPBSplittable;
      aPB->Range(aT1, aT2);
      aPB->ShrunkData(aTS1, aTS2, aBox, bIsPBSplittable);
      IntTools_Range aPaveR[2] = { IntTools_Range(aT1, aTS1), IntTools_Range(aTS2, aT2) };
      Standard_Real aTol = Precision::Confusion();
      Standard_Boolean isOnPave = Standard_False;
      for (Standard_Integer i = 0; i < 2; i++) {
        if (!bIsPBSplittable || IntTools_Tools::IsOnPave1(aT, aPaveR[i], aTol)) {
          Standard_Integer nV1 = (i == 0 ? aPB->Pave1().Index() : aPB->Pave2().Index());
          if (!myDS->HasInterf(nV, nV1)) {
            BOPCol_ListOfInteger aLI;
            aLI.Append(nV);
            aLI.Append(nV1);
            MakeSDVertices(aLI);
          }
          isOnPave = Standard_True;
          break;
        }
      }
      if (isOnPave)
        continue;
      //
      // 1
      BOPDS_InterfVE& aVE=aVEs.Append1();
      aVE.SetIndices(nV, nE);
      aVE.SetParameter(aT);
      // 2
      myDS->AddInterf(nV, nE);
      //
      // 3 update vertex V/E if necessary
      Standard_Real aTolVNew = aVESolver.VertexNewTolerance();
      nVx=UpdateVertex(nV, aTolVNew);
      //4
      if (myDS->IsNewShape(nVx)) {
        aVE.SetIndexNew(nVx);
      }
      //5 append ext pave to pave block
      aPave.SetIndex(nVx);
      aPave.SetParameter(aT);
      aPB->AppendExtPave(aPave);
    }
  }//for (k=0; k < aNbVE; ++k) {
} 
