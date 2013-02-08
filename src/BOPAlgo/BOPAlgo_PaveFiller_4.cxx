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

#include <NCollection_IncAllocator.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>

#include <BOPCol_MapOfInteger.hxx>

#include <BOPInt_Context.hxx>

#include <BOPDS_Iterator.hxx>
#include <BOPDS_VectorOfInterfVF.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_SubIterator.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPCol_MapOfInteger.hxx>

//=======================================================================
// function: PerformVF
// purpose: 
//=======================================================================
  void BOPAlgo_PaveFiller::PerformVF()
{
  Standard_Boolean bJustAdd;
  Standard_Integer iSize, nV, nF, nVSD, iFlag, nVx, i;
  Standard_Real aT1, aT2, aTolF, aTolV;
  BRep_Builder aBB;
  //
  myErrorStatus=0;
  //
  myIterator->Initialize(TopAbs_VERTEX, TopAbs_FACE);
  iSize=myIterator->ExpectedLength();
  if (iSize) {
    //
    BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
    aVFs.SetStartSize(iSize);
    aVFs.SetIncrement(iSize);
    aVFs.Init();
    //
    for (; myIterator->More(); myIterator->Next()) {
      myIterator->Value(nV, nF, bJustAdd);
      if(bJustAdd) {
        continue;
      }
      //
      if (myDS->IsSubShape(nV, nF)) {
        continue;
      }
      //
      if (myDS->HasInterfShapeSubShapes(nV, nF)) {
        continue;
      }
      //
      nVx=nV;
      if (myDS->HasShapeSD(nV, nVSD)) {
        nVx=nVSD;
      }
      //
      if (myDS->HasInterf(nVx, nF)) {
        continue;
      }
      //
      const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nVx))); 
      const TopoDS_Face& aF=(*(TopoDS_Face *)(&myDS->Shape(nF))); 
      //
      aTolV = BRep_Tool::Tolerance(aV);
      aTolF = BRep_Tool::Tolerance(aF);
      //
      iFlag=myContext->ComputeVF(aV, aF, aT1, aT2);
      if (!iFlag) {
        // 1
        i=aVFs.Append()-1;
        BOPDS_InterfVF& aVF=aVFs(i);
        aVF.SetIndices(nVx, nF);
        aVF.SetUV(aT1, aT2);
        // 2
        myDS->AddInterf(nVx, nF);
        if (aTolV < aTolF) {
          aBB.UpdateVertex(aV, aTolF);
          BOPDS_ShapeInfo& aSIV = myDS->ChangeShapeInfo(nVx);
          Bnd_Box& aBoxV = aSIV.ChangeBox();
          BRepBndLib::Add(aV, aBoxV);
        }
      }
    }// for (; myIterator->More(); myIterator->Next()) {
  }// if (iSize) {
  else {
    iSize=10;
    BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
    aVFs.SetStartSize(iSize);
    aVFs.SetIncrement(iSize);
    aVFs.Init();
  }
    //
  TreatVerticesEE();
} 


//=======================================================================
//function : TreatVerticesEE
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::TreatVerticesEE()
{
  Standard_Integer i, aNbS, aNbEEs, nF, nV, iFlag;
  Standard_Real aT1, aT2;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  Handle(NCollection_IncAllocator) aAllocator;
  //
  //-----------------------------------------------------scope_1 f
  aAllocator=new NCollection_IncAllocator();
  BOPCol_ListOfInteger aLIV(aAllocator), aLIF(aAllocator);
  BOPCol_MapOfInteger aMI(100, aAllocator);
  BOPDS_MapOfPaveBlock aMPBF(100, aAllocator);
  //
  myErrorStatus=0;
  //
  
  aNbS=myDS->NbSourceShapes();
  //
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  aNbEEs=aEEs.Extent();
  for (i=0; i<aNbEEs; ++i) {
    BOPDS_InterfEE& aEE=aEEs(i);
    if (aEE.HasIndexNew()) {
      nV=aEE.IndexNew();
      if (aMI.Add(nV)) {
        aLIV.Append(nV);
      }   
    }   
  }
  if (!aLIV.Extent()) {
    aAllocator.Nullify();
    return;
  }
  //
  aNbS=myDS->NbSourceShapes();
  for (nF=0; nF<aNbS; ++nF) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(nF);
    if (aSI.ShapeType()==TopAbs_FACE) {
      aLIF.Append(nF);
    }
  }
  if (!aLIF.Extent()) {
    aAllocator.Nullify();
    return;
  }
  //-------------------------------------------------------------
  BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
  //
  BOPDS_SubIterator aIt(aAllocator);
  //
  aIt.SetDS(myDS);
  aIt.SetSubSet1(aLIF);
  aIt.SetSubSet2(aLIV);
  aIt.Prepare();
  aIt.Initialize();
  for (; aIt.More(); aIt.Next()) {
    aIt.Value(nV, nF);
    //
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
    const BOPCol_MapOfInteger& aMVOn=aFI.VerticesOn();
    //
    if (!aMVOn.Contains(nV)) {
      const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nV))); 
      const TopoDS_Face& aF=(*(TopoDS_Face *)(&myDS->Shape(nF))); 
      iFlag=myContext->ComputeVF(aV, aF, aT1, aT2);
      if (!iFlag) {
        // 1
        i=aVFs.Append()-1;
        BOPDS_InterfVF& aVF=aVFs(i);
        aVF.SetIndices(nV, nF);
        aVF.SetUV(aT1, aT2);
        // 2
        myDS->AddInterf(nV, nF);
        //
        BOPCol_MapOfInteger& aMVIn=aFI.ChangeVerticesIn();
        aMVIn.Add(nV);
      }
    }
  }
  //
  aAllocator.Nullify();
  //-----------------------------------------------------scope_1 t
}
