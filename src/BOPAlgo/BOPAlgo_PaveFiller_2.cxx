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
#include <BOPDS_MapOfPassKey.hxx>
#include <BOPDS_PassKey.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//class    : BOPAlgo_VertexEdgeEdge
//purpose  : 
//=======================================================================
class BOPAlgo_VertexEdge : public BOPAlgo_Algo {

 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_VertexEdge() : 
    BOPAlgo_Algo(),
    myIV(-1), myIE(-1), myIVx(-1), myFlag(-1), myT(-1.) {
  };
  //
  virtual ~BOPAlgo_VertexEdge(){
  };
  //
  void SetIndices(const Standard_Integer nV,
                  const Standard_Integer nE,
                  const Standard_Integer nVx) {
    myIV=nV;
    myIE=nE;
    myIVx=nVx;
  }
  //
  void Indices(Standard_Integer& nV,
               Standard_Integer& nE,
               Standard_Integer& nVx) const {
    nV=myIV;
    nE=myIE;
    nVx=myIVx;
  }
  //
  void SetVertex(const TopoDS_Vertex& aV) {
    myV=aV;
  }
  //
  const TopoDS_Vertex& Vertex()const {
    return myV;
  }
  //
  void SetEdge(const TopoDS_Edge& aE) {
    myE=aE;
  }
  //
  const TopoDS_Edge& Edge()const {
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
    myFlag=myContext->ComputeVE (myV, myE, myT);
  };
  //
 protected:
  Standard_Integer myIV;
  Standard_Integer myIE;
  Standard_Integer myIVx;
  Standard_Integer myFlag;
  Standard_Real myT;
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
  Standard_Boolean bJustAdd;
  Standard_Integer iSize, nV, nE, nVSD, iFlag, nVx,  k, aNbVE;
  Standard_Real aT, aTolE, aTolV;
  BOPDS_Pave aPave;
  BOPDS_PassKey aPK;
  BOPDS_MapOfPassKey aMPK;
  BRep_Builder aBB;
  BOPAlgo_VectorOfVertexEdge aVVE;
  //
  myErrorStatus=0;
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
    myIterator->Value(nV, nE, bJustAdd);
    if(bJustAdd) {
      continue;
    }
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
    aPK.SetIds(nVx, nE);
    if (!aMPK.Add(aPK)) {
      continue;
    }
    //
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aSIE.Shape())); 
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nVx))); 
    //
    BOPAlgo_VertexEdge& aVESolver=aVVE.Append1();
    //
    aVESolver.SetIndices(nV, nE, nVx);
    aVESolver.SetVertex(aV);
    aVESolver.SetEdge(aE);
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
      aVESolver.Indices(nV, nE, nVx);
      aT=aVESolver.Parameter();
      const TopoDS_Vertex& aV=aVESolver.Vertex();
      const TopoDS_Edge& aE=aVESolver.Edge();
      // 1
      BOPDS_InterfVE& aVE=aVEs.Append1();
      aVE.SetIndices(nV, nE);
      aVE.SetParameter(aT);
      // 2
      myDS->AddInterf(nV, nE);
      // 3
      BOPDS_ListOfPaveBlock& aLPB=myDS->ChangePaveBlocks(nE);
      Handle(BOPDS_PaveBlock)& aPB=*((Handle(BOPDS_PaveBlock)*)&aLPB.First());
      // 
      aPave.SetIndex(nVx);
      aPave.SetParameter(aT);
      aPB->AppendExtPave(aPave);
      aTolV = BRep_Tool::Tolerance(aV);
      aTolE = BRep_Tool::Tolerance(aE);
      if ( aTolV < aTolE) {
        aBB.UpdateVertex(aV, aTolE);
        BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nVx);
        Bnd_Box& aBoxDS=aSIDS.ChangeBox();
        BRepBndLib::Add(aV, aBoxDS);
      }
    }
  }//for (k=0; k < aNbVE; ++k) {
} 
