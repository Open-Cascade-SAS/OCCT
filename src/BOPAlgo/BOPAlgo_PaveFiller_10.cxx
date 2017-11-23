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

#include <Precision.hxx>

#include <gp_Pnt.hxx>
#include <Bnd_Box.hxx>

#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>

#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_VectorOfListOfPaveBlock.hxx>
#include <BOPDS_MapOfCommonBlock.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_DS.hxx>

//=======================================================================
//function : SetNonDestructive
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetNonDestructive() 
{
  if (!myIsPrimary || myNonDestructive) {
    return;
  }
  //
  Standard_Boolean bFlag;
  TopTools_ListIteratorOfListOfShape aItLS;
  //
  bFlag=Standard_False;
  aItLS.Initialize(myArguments);
  for(; aItLS.More() && (!bFlag); aItLS.Next()) {
    const TopoDS_Shape& aS=aItLS.Value();
    bFlag=aS.Locked();
  }
  myNonDestructive=bFlag;
}
//=======================================================================
//function : UpdateEdgeTolerance
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::UpdateEdgeTolerance (const Standard_Integer nE,
                                              const Standard_Real aTol)
{
  Standard_Boolean bIsNewShape, bHasShapeSD;
  Standard_Integer nV, nVx;
  Standard_Real aTolV;
  BRep_Builder aBB;
  TColStd_ListIteratorOfListOfInteger aIt;
  //
  BOPDS_ShapeInfo& aSIE=myDS->ChangeShapeInfo(nE);
  const TColStd_ListOfInteger& aLI=aSIE.SubShapes();
  //
  if (myNonDestructive) {
    bIsNewShape=myDS->IsNewShape(nE);
    if (!bIsNewShape) {
      return;
    }
    //
    aIt.Initialize(aLI);
    for (; aIt.More(); aIt.Next()) {
      nV = aIt.Value();
      bHasShapeSD=myDS->HasShapeSD(nV, nVx);
      if (bHasShapeSD) {
        continue;
      }
      bIsNewShape=myDS->IsNewShape(nV);
      if (!bIsNewShape) {
        return;
      }
    }
  }
  //
  const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(nE);
  aBB.UpdateEdge(aE, aTol);
  Bnd_Box& aBoxE=aSIE.ChangeBox();
  BRepBndLib::Add(aE, aBoxE);
  aBoxE.SetGap(aBoxE.GetGap() + Precision::Confusion());
  //
  aIt.Initialize(aLI);
  for (; aIt.More(); aIt.Next()) {
    nV = aIt.Value();
    bHasShapeSD=myDS->HasShapeSD(nV, nVx);
    if (bHasShapeSD) {
      nV=nVx;
    }
    const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&myDS->Shape(nV);
    aTolV = BRep_Tool::Tolerance(aV);
    if (aTolV < aTol) {
      aBB.UpdateVertex(aV, aTol);
      BOPDS_ShapeInfo& aSIV = myDS->ChangeShapeInfo(nV);
      Bnd_Box& aBoxV = aSIV.ChangeBox();
      BRepBndLib::Add(aV, aBoxV);
      aBoxV.SetGap(aBoxV.GetGap() + Precision::Confusion());
    }
  }
}
//=======================================================================
//function : UpdateVertex
//purpose  : 
//=======================================================================
Standard_Integer BOPAlgo_PaveFiller::UpdateVertex
  (const Standard_Integer nV,
   const Standard_Real aTolNew)
{
  Standard_Integer nVNew;
  Standard_Real aTolV;
  BRep_Builder aBB;
  
  nVNew = nV;
  if (myDS->IsNewShape(nVNew) || 
      myDS->HasShapeSD(nV, nVNew) ||
      !myNonDestructive) {
    // nV is a new vertex, it has SD or non-destructive mode is not in force
    const TopoDS_Vertex& aVSD = *(TopoDS_Vertex*)&myDS->Shape(nVNew);
    aTolV = BRep_Tool::Tolerance(aVSD);
    if (aTolV < aTolNew) {
      aBB.UpdateVertex(aVSD, aTolNew);
      BOPDS_ShapeInfo& aSIV = myDS->ChangeShapeInfo(nVNew);
      Bnd_Box& aBoxV = aSIV.ChangeBox();
      BRepBndLib::Add(aVSD, aBoxV);
      aBoxV.SetGap(aBoxV.GetGap() + Precision::Confusion());
    }
    return nVNew;
  }
  //
  // nV is old vertex
  const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&myDS->Shape(nV);
  aTolV = BRep_Tool::Tolerance(aV);
  //
  // create new vertex
  TopoDS_Vertex aVNew;
  gp_Pnt aPV = BRep_Tool::Pnt(aV);
  aBB.MakeVertex(aVNew, aPV, Max(aTolV, aTolNew));
  //
  // append new vertex to DS
  BOPDS_ShapeInfo aSIV;
  aSIV.SetShapeType(TopAbs_VERTEX);
  aSIV.SetShape(aVNew);
  nVNew = myDS->Append(aSIV);
  //
  // bounding box for the new vertex
  BOPDS_ShapeInfo& aSIDS = myDS->ChangeShapeInfo(nVNew);
  Bnd_Box& aBoxDS = aSIDS.ChangeBox();
  BRepBndLib::Add(aVNew, aBoxDS);
  aBoxDS.SetGap(aBoxDS.GetGap() + Precision::Confusion());
  //
  // add vertex to SD map
  myDS->AddShapeSD(nV, nVNew);
  //
  myDS->InitPaveBlocksForVertex(nV);
  //
  return nVNew;
}
//=======================================================================
//function : UpdatePaveBlocksWithSDVertices
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::UpdatePaveBlocksWithSDVertices()
{
  myDS->UpdatePaveBlocksWithSDVertices();
}
//=======================================================================
//function : UpdateCommonBlocksWithSDVertices
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::UpdateCommonBlocksWithSDVertices()
{
  if (!myNonDestructive) {
    UpdatePaveBlocksWithSDVertices();
    return;
  }
  Standard_Integer aNbPBP;
  //
  BOPDS_VectorOfListOfPaveBlock& aPBP=myDS->ChangePaveBlocksPool();
  aNbPBP=aPBP.Length();
  if(!aNbPBP) {
    return;
  }
  //
  Standard_Integer i, nV1, nV2;
  Standard_Real aTolV;
  BOPDS_MapOfCommonBlock aMCB;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  Handle(BOPDS_PaveBlock) aPB;
  // 
  aTolV = Precision::Confusion();
  //
  for (i=0; i<aNbPBP; ++i) {
    BOPDS_ListOfPaveBlock& aLPB=aPBP(i);
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next()) {
      aPB=aItPB.Value();
      const Handle(BOPDS_CommonBlock)& aCB=myDS->CommonBlock(aPB);
      if (aCB.IsNull()) {
        continue;
      }
      //
      if (aMCB.Add(aCB)) {
        aPB->Indices(nV1, nV2);
        UpdateVertex(nV1, aTolV);
        UpdateVertex(nV2, aTolV);
        myDS->UpdateCommonBlockWithSDVertices(aCB);
      }
    }
  }
  UpdatePaveBlocksWithSDVertices();
}

namespace
{
  //=======================================================================
  //function : UpdateInterfsWithSDVertices
  //purpose  : 
  //=======================================================================
  template <class InterfType>
  void UpdateIntfsWithSDVertices(BOPDS_PDS theDS, NCollection_Vector<InterfType>& theInterfs)
  {
    for (Standard_Integer i = 0; i < theInterfs.Length(); i++)
    {
      InterfType& anIntf = theInterfs(i);
      Standard_Integer anInd;
      if (anIntf.HasIndexNew(anInd))
      {
        Standard_Integer anIndSD;
        if (theDS->HasShapeSD(anInd, anIndSD))
        {
          anIntf.SetIndexNew(anIndSD);
        }
      }
    }
  }
}

//=======================================================================
//function : UpdateInterfsWithSDVertices
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::UpdateInterfsWithSDVertices()
{
  UpdateIntfsWithSDVertices(myDS, myDS->InterfVV());
  UpdateIntfsWithSDVertices(myDS, myDS->InterfVE());
  UpdateIntfsWithSDVertices(myDS, myDS->InterfVF());
  UpdateIntfsWithSDVertices(myDS, myDS->InterfEE());
  UpdateIntfsWithSDVertices(myDS, myDS->InterfEF());
}
