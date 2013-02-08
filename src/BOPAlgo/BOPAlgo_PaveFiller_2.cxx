// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
