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

#include <BOPAlgo_PaveFiller.ixx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>

#include <BOPInt_Context.hxx>

#include <BOPDS_Iterator.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_PassKey.hxx>
#include <BOPDS_MapOfPassKey.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Builder.hxx>


//=======================================================================
// function: PerformVE
// purpose: 
//=======================================================================
  void BOPAlgo_PaveFiller::PerformVE()
{
  Standard_Boolean bJustAdd;
  Standard_Integer iSize, nV, nE, nVSD, iFlag, nVx, i;
  Standard_Real aT, aTolE, aTolV;
  BOPDS_Pave aPave;
  BOPDS_PassKey aPK;
  BOPDS_MapOfPassKey aMPK;
  BRep_Builder aBB;
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
  aVEs.SetStartSize(iSize);
  aVEs.SetIncrement(iSize);
  aVEs.Init();
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
    iFlag=myContext->ComputeVE (aV, aE, aT);
    if (!iFlag) {
      // 1
      i=aVEs.Append()-1;
      BOPDS_InterfVE& aVE=aVEs(i);
      aVE.SetIndices(nV, nE);
      aVE.SetParameter(aT);
      // 2
      myDS->AddInterf(nV, nE);
      // 3
      BOPDS_ListOfPaveBlock& aLPB=myDS->ChangePaveBlocks(nE);
      Handle(BOPDS_PaveBlock)& aPB=*((Handle_BOPDS_PaveBlock*)&aLPB.First());
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
  }//for (; myIterator->More(); myIterator->Next()) {
} 
