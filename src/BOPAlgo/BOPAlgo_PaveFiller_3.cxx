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


#include <Bnd_Box.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPCol_BoxBndTree.hxx>
#include <BOPCol_DataMapOfIntegerShape.hxx>
#include <BOPCol_DataMapOfShapeInteger.hxx>
#include <BOPCol_DataMapOfShapeListOfShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeBox.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfInteger.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfPaveBlock.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_VectorOfInterfEE.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_SequenceOfRanges.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <IntTools_Tools.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <Precision.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class    : BOPAlgo_EdgeEdge
//purpose  : 
//=======================================================================
class BOPAlgo_EdgeEdge : 
  public IntTools_EdgeEdge,
  public BOPAlgo_Algo {
 
 public:

  DEFINE_STANDARD_ALLOC
  //
  BOPAlgo_EdgeEdge(): 
    IntTools_EdgeEdge(),
    BOPAlgo_Algo() {
  };
  //
  virtual ~BOPAlgo_EdgeEdge(){
  };
  //
  void SetPaveBlock1(const Handle(BOPDS_PaveBlock)& aPB) {
    myPB1=aPB;
  }
  //
  Handle(BOPDS_PaveBlock)& PaveBlock1() {
    return myPB1;
  }
  //
  void SetPaveBlock2(const Handle(BOPDS_PaveBlock)& aPB) {
    myPB2=aPB;
  }
  //
  Handle(BOPDS_PaveBlock)& PaveBlock2() {
    return myPB2;
  }
  // 
  virtual void Perform() {
    BOPAlgo_Algo::UserBreak();
    IntTools_EdgeEdge::Perform();
  }
  //
 protected:
  Handle(BOPDS_PaveBlock) myPB1;
  Handle(BOPDS_PaveBlock) myPB2;
};
//
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_EdgeEdge> BOPAlgo_VectorOfEdgeEdge; 
//
typedef BOPCol_Functor 
  <BOPAlgo_EdgeEdge,
  BOPAlgo_VectorOfEdgeEdge> BOPAlgo_EdgeEdgeFunctor;
//
typedef BOPCol_Cnt 
  <BOPAlgo_EdgeEdgeFunctor,
  BOPAlgo_VectorOfEdgeEdge> BOPAlgo_EdgeEdgeCnt;
//
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class    : BOPAlgo_TNV
//purpose  : 
//=======================================================================
class BOPAlgo_TNV : public BOPCol_BoxBndTreeSelector{
 public:
  BOPAlgo_TNV() 
    : BOPCol_BoxBndTreeSelector(), myTree(NULL) {
  };
  //
  ~BOPAlgo_TNV(){
  };
  //
  void SetVertex(const TopoDS_Vertex& aV) {
    myV=aV;
  }
  //
  const TopoDS_Vertex& Vertex()const {
    return myV;
  }
  //
  void SetTree(BOPCol_BoxBndTree& aTree) {
    myTree=&aTree;
  }
  //
  void Perform() {
    myTree->Select(*this);
  }
  //
 protected:
  TopoDS_Vertex myV;
  BOPCol_BoxBndTree *myTree;
};
//
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_TNV> BOPAlgo_VectorOfTNV; 
//
typedef BOPCol_Functor 
  <BOPAlgo_TNV,
  BOPAlgo_VectorOfTNV> BOPAlgo_TNVFunctor;
//
typedef BOPCol_Cnt 
  <BOPAlgo_TNVFunctor,
  BOPAlgo_VectorOfTNV> BOPAlgo_TNVCnt;
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class    : BOPAlgo_PVE
//purpose  : 
//=======================================================================
class BOPAlgo_PVE {
 public:
  BOPAlgo_PVE()
    : myIV(-1), myIE(-1), myFlag(-1), myT(-1.) {
  };
  //
  ~BOPAlgo_PVE(){
  };
  //
  void SetIndices(const Standard_Integer nV,
                  const Standard_Integer nE){
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
  void SetPaveBlock(const Handle(BOPDS_PaveBlock)& aPB) {
    myPB=aPB;
  }
  //
  Handle(BOPDS_PaveBlock)& PaveBlock() {
    return myPB;
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
  void Perform() {
    myFlag=myContext->ComputeVE (myV, myE, myT);
  };
  //
 protected:
  Standard_Integer myIV;
  Standard_Integer myIE;
  Standard_Integer myFlag;
  Standard_Real myT;
  TopoDS_Vertex myV;
  TopoDS_Edge myE;
  Handle(BOPDS_PaveBlock) myPB;
  Handle(IntTools_Context) myContext;
};
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_PVE> BOPAlgo_VectorOfPVE; 
//
typedef BOPCol_ContextFunctor 
  <BOPAlgo_PVE,
  BOPAlgo_VectorOfPVE,
  Handle(IntTools_Context), 
  IntTools_Context> BOPAlgo_PVEFunctor;
//
typedef BOPCol_ContextCnt 
  <BOPAlgo_PVEFunctor,
  BOPAlgo_VectorOfPVE,
  Handle(IntTools_Context)> BOPAlgo_PVECnt;
/////////////////////////////////////////////////////////////////////////
//=======================================================================
// function: PerformEE
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformEE()
{
  Standard_Integer iSize;
  //
  myErrorStatus=0;
  //
  FillShrunkData(TopAbs_EDGE, TopAbs_EDGE);
  //
  myIterator->Initialize(TopAbs_EDGE, TopAbs_EDGE);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  Standard_Boolean bJustAdd;
  Standard_Integer i, iX, nE1, nE2, aNbCPrts, k, aNbFdgeEdge;
  Standard_Real aTS11, aTS12, aTS21, aTS22, aT11, aT12, aT21, aT22;
  TopAbs_ShapeEnum aType;
  BOPDS_ListIteratorOfListOfPaveBlock aIt1, aIt2;
  Handle(NCollection_BaseAllocator) aAllocator;
  BOPDS_MapOfPaveBlock aMPBToUpdate;
  BOPAlgo_VectorOfEdgeEdge aVEdgeEdge;
  BOPDS_MapIteratorOfMapOfPaveBlock aItPB; 
  //
  aAllocator=NCollection_BaseAllocator::CommonBaseAllocator();
  //-----------------------------------------------------scope f
  BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock aMPBLPB(100, aAllocator);
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks aMVCPB(100, aAllocator);
  //
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  aEEs.SetIncrement(iSize);
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nE1, nE2, bJustAdd);
    if(bJustAdd) {
      continue;
    }
    //
    const BOPDS_ShapeInfo& aSIE1=myDS->ShapeInfo(nE1);
    if (aSIE1.HasFlag()){
      continue;
    }
    const BOPDS_ShapeInfo& aSIE2=myDS->ShapeInfo(nE2);
    if (aSIE2.HasFlag()){
      continue;
    }
    //
    const TopoDS_Edge& aE1=(*(TopoDS_Edge *)(&aSIE1.Shape()));
    const TopoDS_Edge& aE2=(*(TopoDS_Edge *)(&aSIE2.Shape()));  
    //
    BOPDS_ListOfPaveBlock& aLPB1=myDS->ChangePaveBlocks(nE1);
    BOPDS_ListOfPaveBlock& aLPB2=myDS->ChangePaveBlocks(nE2);
    //
    aIt1.Initialize(aLPB1);
    for (; aIt1.More(); aIt1.Next()) {
      Bnd_Box aBB1;
      //
      Handle(BOPDS_PaveBlock)& aPB1=aIt1.ChangeValue();
      if (!aPB1->HasShrunkData()) {
        continue;
      }
      aPB1->ShrunkData(aTS11, aTS12, aBB1);
      //
      aIt2.Initialize(aLPB2);
      for (; aIt2.More(); aIt2.Next()) {
        Bnd_Box aBB2;
        //
        Handle(BOPDS_PaveBlock)& aPB2=aIt2.ChangeValue();
        if (!aPB2->HasShrunkData()) {
          continue;
        }
        aPB2->ShrunkData(aTS21, aTS22, aBB2);
        //
        if (aBB1.IsOut(aBB2)) {
          continue;
        }
        //
        aPB1->Range(aT11, aT12);
        aPB2->Range(aT21, aT22);
        //
        BOPAlgo_EdgeEdge& anEdgeEdge=aVEdgeEdge.Append1();
        // 
        anEdgeEdge.SetPaveBlock1(aPB1);
        anEdgeEdge.SetPaveBlock2(aPB2);
        //
        anEdgeEdge.SetEdge1(aE1, aT11, aT12);
        anEdgeEdge.SetEdge2(aE2, aT21, aT22);
        anEdgeEdge.SetProgressIndicator(myProgressIndicator);
      }//for (; aIt2.More(); aIt2.Next()) {
    }//for (; aIt1.More(); aIt1.Next()) {
  }//for (; myIterator->More(); myIterator->Next()) {
  //
  aNbFdgeEdge=aVEdgeEdge.Extent();
  //======================================================
  BOPAlgo_EdgeEdgeCnt::Perform(myRunParallel, aVEdgeEdge);
  //======================================================
  //
  for (k=0; k < aNbFdgeEdge; ++k) {
    Bnd_Box aBB1, aBB2;
    //
    BOPAlgo_EdgeEdge& anEdgeEdge=aVEdgeEdge(k);
    if (!anEdgeEdge.IsDone()) {
      continue;
    }
    //
    //--------------------------------------------
    Handle(BOPDS_PaveBlock)& aPB1=anEdgeEdge.PaveBlock1();
    nE1=aPB1->OriginalEdge();
    aPB1->Range(aT11, aT12);
    aPB1->ShrunkData(aTS11, aTS12, aBB1);
    //
    Handle(BOPDS_PaveBlock)& aPB2=anEdgeEdge.PaveBlock2();
    nE2=aPB2->OriginalEdge();
    aPB2->Range(aT21, aT22);
    aPB2->ShrunkData(aTS21, aTS22, aBB2);
    //
    //--------------------------------------------
    IntTools_Range aR11(aT11, aTS11), aR12(aTS12, aT12),
                   aR21(aT21, aTS21), aR22(aTS22, aT22);
    //
    const IntTools_SequenceOfCommonPrts& aCPrts = anEdgeEdge.CommonParts();
    aNbCPrts=aCPrts.Length();
    for (i=1; i<=aNbCPrts; ++i) {
      const IntTools_CommonPrt& aCPart=aCPrts(i);
      //
      const TopoDS_Edge& aE1=aCPart.Edge1();
      const TopoDS_Edge& aE2=aCPart.Edge2();
      //
      aType=aCPart.Type();
      switch (aType) {
        case TopAbs_VERTEX:  { 
          Standard_Boolean bIsOnPave[4], bFlag;
          Standard_Integer nV[4], j;
          Standard_Real aT1, aT2, aTol;
          TopoDS_Vertex aVnew;
          IntTools_Range aCR1, aCR2;
          //
          IntTools_Tools::VertexParameters(aCPart, aT1, aT2);
          aTol = Precision::Confusion();
          aCR1 = aCPart.Range1();
          aCR2 = aCPart.Ranges2()(1);
          // 
          //decide to keep the pave or not
          bIsOnPave[0] = IntTools_Tools::IsOnPave1(aT1, aR11, aTol) ||
            IntTools_Tools::IsOnPave1(aR11.First(), aCR1, aTol);
          bIsOnPave[1] = IntTools_Tools::IsOnPave1(aT1, aR12, aTol) || 
            IntTools_Tools::IsOnPave1(aR12.Last(), aCR1, aTol);
          bIsOnPave[2] = IntTools_Tools::IsOnPave1(aT2, aR21, aTol) ||
            IntTools_Tools::IsOnPave1(aR21.First(), aCR2, aTol);
          bIsOnPave[3] = IntTools_Tools::IsOnPave1(aT2, aR22, aTol) ||
            IntTools_Tools::IsOnPave1(aR22.Last(), aCR2, aTol);
          //
          aPB1->Indices(nV[0], nV[1]);
          aPB2->Indices(nV[2], nV[3]);
          //
          if((bIsOnPave[0] && bIsOnPave[2]) || 
             (bIsOnPave[0] && bIsOnPave[3]) ||
             (bIsOnPave[1] && bIsOnPave[2]) || 
             (bIsOnPave[1] && bIsOnPave[3])) {
            continue;
          }
          //
          bFlag = Standard_False;
          for (j = 0; j < 4; ++j) {
            if (bIsOnPave[j]) {
              //add interf VE(nV[j], nE)
              Handle(BOPDS_PaveBlock)& aPB = (j < 2) ? aPB2 : aPB1;
              ForceInterfVE(nV[j], aPB, aMPBToUpdate);
              bFlag = Standard_True;
              break;
            }
          }
          if (bFlag) {
            continue;
          }
          //
          BOPTools_AlgoTools::MakeNewVertex(aE1, aT1, aE2, aT2, aVnew);
          // <-LXBR
          {
            Standard_Integer nVS[2], iFound;
            Standard_Real aTolVx, aTolVnew, aD2, aDT2;
            BOPCol_MapOfInteger aMV;
            gp_Pnt aPnew, aPx;
            //
            iFound=0;
            j=-1;
            aMV.Add(nV[0]);
            aMV.Add(nV[1]);
            //
            if (aMV.Contains(nV[2])) {
              ++j;
              nVS[j]=nV[2];
            }
            if (aMV.Contains(nV[3])) {
              ++j;
              nVS[j]=nV[3];
            }
            //
            aTolVnew=BRep_Tool::Tolerance(aVnew);
            aPnew=BRep_Tool::Pnt(aVnew);
            //
            for (Standard_Integer k1=0; k1<=j; ++k1) {
              const TopoDS_Vertex& aVx= *(TopoDS_Vertex*)&(myDS->Shape(nVS[k1]));
              aTolVx=BRep_Tool::Tolerance(aVx);
              aPx=BRep_Tool::Pnt(aVx);
              aD2=aPnew.SquareDistance(aPx);
              //
              aDT2=100.*(aTolVnew+aTolVx)*(aTolVnew+aTolVx);
              //
              if (aD2<aDT2) {
                iFound=1;
                break;
              }
            }
            //
            if (iFound) {
              continue;
            }
          }
          //
          // 1
          BOPDS_InterfEE& aEE=aEEs.Append1();
          iX=aEEs.Extent()-1;
          aEE.SetIndices(nE1, nE2);
          aEE.SetCommonPart(aCPart);
          // 2
          myDS->AddInterf(nE1, nE2);
          //
          BOPDS_CoupleOfPaveBlocks aCPB;
          //
          aCPB.SetPaveBlocks(aPB1, aPB2);
          aCPB.SetIndexInterf(iX);
          aMVCPB.Add(aVnew, aCPB);
        }//case TopAbs_VERTEX: 
          break;
            //
        case TopAbs_EDGE: {
          if (aNbCPrts > 1) {
            break;
          }
          //
          Standard_Boolean bHasSameBounds;
          bHasSameBounds=aPB1->HasSameBounds(aPB2);
          if (!bHasSameBounds) {
            break;
          }
          // 1
          BOPDS_InterfEE& aEE=aEEs.Append1();
          iX=aEEs.Extent()-1;
          aEE.SetIndices(nE1, nE2);
          aEE.SetCommonPart(aCPart);
          // 2
          myDS->AddInterf(nE1, nE2);
          //
          BOPAlgo_Tools::FillMap(aPB1, aPB2, aMPBLPB, aAllocator);
        }//case TopAbs_EDGE
          break;
        default:
          break;
      }//switch (aType) {
    }//for (i=1; i<=aNbCPrts; i++) {
  }//for (k=0; k < aNbFdgeEdge; ++k) {
  // 
  //=========================================
  // post treatment
  //=========================================
  {
    Standard_Integer aNbV;
    Handle(BOPDS_PaveBlock) aPB1, aPB2;
    //
    aNbV=aMVCPB.Extent();
    for (i=1; i<=aNbV; ++i) {
      const BOPDS_CoupleOfPaveBlocks& aCPB=aMVCPB.FindFromIndex(i);
      aCPB.PaveBlocks(aPB1, aPB2); 
      //
      aMPBToUpdate.Remove(aPB1);
      aMPBToUpdate.Remove(aPB2);
    }
  }
  //
  aItPB.Initialize(aMPBToUpdate);
  for (; aItPB.More(); aItPB.Next()) {
    Handle(BOPDS_PaveBlock) aPB=aItPB.Value();
    if (!myDS->IsCommonBlock(aPB)) {
      myDS->UpdatePaveBlock(aPB);
    }
    else {
      const Handle(BOPDS_CommonBlock)& aCB=myDS->CommonBlock(aPB);
      myDS->UpdateCommonBlock(aCB);
    }
  }
  //
  BOPAlgo_Tools::PerformCommonBlocks(aMPBLPB, aAllocator, myDS);
  PerformVerticesEE(aMVCPB, aAllocator);
  //-----------------------------------------------------scope t
  aMPBLPB.Clear();
  aMVCPB.Clear();
  aMPBToUpdate.Clear();
}
//=======================================================================
//function : PerformVertices
//purpose  : 
//=======================================================================
Standard_Integer BOPAlgo_PaveFiller::PerformVerticesEE
  (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB,
   const Handle(NCollection_BaseAllocator)& theAllocator)
{
  Standard_Integer aNbV, iRet;
  //
  iRet=0;
  aNbV=theMVCPB.Extent();
  if (!aNbV) {
    return iRet;
  }
  //
  Standard_Integer nVx, iV, j, nE, iFlag, iX, i, aNb; 
  Standard_Real aT;
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  BOPDS_ShapeInfo aSI;
  BOPDS_Pave aPave;
  //
  BOPDS_IndexedDataMapOfPaveBlockListOfInteger aMPBLI(100, theAllocator);
  BOPCol_ListOfShape aLS(theAllocator);
  BOPCol_IndexedDataMapOfShapeInteger aMVI(100, theAllocator);
  BOPCol_IndexedDataMapOfShapeListOfShape aImages;
  //
  aSI.SetShapeType(TopAbs_VERTEX);
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  //
  // 1 prepare arguments
  //
  // <- DEB
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Shape& aS=theMVCPB.FindKey(i);
    const BOPDS_CoupleOfPaveBlocks& aCPB=theMVCPB.FindFromIndex(i);
    iV=aCPB.IndexInterf();
    aMVI.Add(aS, iV);
  }
  //
  // 2 Fuse vertices
  TreatNewVertices(aMVI, aImages);
  //
  // 3 Add new vertices to myDS; 
  //   connect indices to CPB structure
  aNb = aImages.Extent();
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex*)(&aImages.FindKey(i)));
    const BOPCol_ListOfShape& aLVSD=aImages.FindFromIndex(i);
    //
    aSI.SetShape(aV);
    iV=myDS->Append(aSI);
    //
    BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(iV);
    Bnd_Box& aBox=aSIDS.ChangeBox();
    BRepBndLib::Add(aV, aBox);
    //
    aItLS.Initialize(aLVSD);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aVx = aItLS.Value();
      BOPDS_CoupleOfPaveBlocks &aCPB=theMVCPB.ChangeFromKey(aVx);
      aCPB.SetIndex(iV);
      // update EE interference
      iX=aCPB.IndexInterf();
      BOPDS_InterfEE& aEE=aEEs(iX);
      aEE.SetIndexNew(iV);
    }
  }
  //
  // 4 Map PaveBlock/ListOfVertices to add to this PaveBlock ->aMPBLI
  {
    Handle(BOPDS_PaveBlock) aPB[2];
    //
    for (i=1; i<=aNbV; ++i) {
      const BOPDS_CoupleOfPaveBlocks& aCPB=theMVCPB.FindFromIndex(i);
      iV=aCPB.Index();
      aCPB.PaveBlocks(aPB[0], aPB[1]);
      for (j=0; j<2; ++j) {
        if (aMPBLI.Contains(aPB[j])) {
          BOPCol_ListOfInteger& aLI=aMPBLI.ChangeFromKey(aPB[j]);
          aLI.Append(iV);
        }
        else {
          BOPCol_ListOfInteger aLI(theAllocator);
          aLI.Append(iV);
          aMPBLI.Add(aPB[j], aLI);
        }
      }
    }
  }
  // 5 
  // 5.1  Compute Extra Paves and 
  // 5.2. Add Extra Paves to the PaveBlocks
  //-------------------------------------------------------------
  Standard_Integer k, aNbVPVE;
  BOPAlgo_VectorOfPVE aVPVE;
  //
  aNb=aMPBLI.Extent();
  for(i=1; i<=aNb; ++i) {
    Handle(BOPDS_PaveBlock) aPB=aMPBLI.FindKey(i);
    nE=aPB->OriginalEdge();
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE)));
    // 1,2
    const BOPCol_ListOfInteger& aLI=aMPBLI.FindFromIndex(i);
    aItLI.Initialize(aLI);
    for (; aItLI.More(); aItLI.Next()) {
      nVx=aItLI.Value();
      const TopoDS_Vertex& aVx=(*(TopoDS_Vertex *)(&myDS->Shape(nVx)));
      //
      BOPAlgo_PVE& aPVE=aVPVE.Append1();
      aPVE.SetIndices(nVx, nE);
      aPVE.SetVertex(aVx);
      aPVE.SetEdge(aE);
      aPVE.SetPaveBlock(aPB);
    }
  }
  //
  aNbVPVE=aVPVE.Extent();
  //=============================================================
  BOPAlgo_PVECnt::Perform(myRunParallel, aVPVE, myContext);
  //=============================================================
  //
  for (k=0; k < aNbVPVE; ++k) {
    BOPAlgo_PVE& aPVE=aVPVE(k);
    iFlag=aPVE.Flag();
    if (!iFlag) {
      aPVE.Indices(nVx, nE);
      aT=aPVE.Parameter();
      Handle(BOPDS_PaveBlock)& aPB=aPVE.PaveBlock();
      //
      aPave.SetIndex(nVx);
      aPave.SetParameter(aT);
      aPB->AppendExtPave(aPave);
    }
  }
  // 6  Split PaveBlocksa
  aNb=aMPBLI.Extent();
  for(i=1; i<=aNb; ++i) {
    Handle(BOPDS_PaveBlock) aPB=aMPBLI.FindKey(i);
    nE=aPB->OriginalEdge();
    // 3
    if (!myDS->IsCommonBlock(aPB)) {
      myDS->UpdatePaveBlock(aPB);
    }
    else {
      const Handle(BOPDS_CommonBlock)& aCB=myDS->CommonBlock(aPB);
      myDS->UpdateCommonBlock(aCB);
    }    
  }//for (; aItMPBLI.More(); aItMPBLI.Next()) {
  //
  return iRet;
}
//=======================================================================
//function : TreatNewVertices
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::TreatNewVertices
  (const BOPCol_IndexedDataMapOfShapeInteger& aMapVI,
   BOPCol_IndexedDataMapOfShapeListOfShape& myImages)
{
  Standard_Integer  i, aNbV;//, aNbVSD;
  Standard_Real aTol;
  TopoDS_Vertex aVnew;
  TopoDS_Shape aVF;
  BOPCol_IndexedMapOfShape aMVProcessed;
  BOPCol_MapOfInteger aMFence;
  BOPCol_ListIteratorOfListOfInteger aIt;
  BOPCol_IndexedDataMapOfShapeListOfShape aDMVLV;
  //
  BOPCol_BoxBndTreeSelector aSelector;
  BOPCol_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, 
                            Bnd_Box> aTreeFiller(aBBTree);
  BOPAlgo_VectorOfTNV aVTNV;
  //
  aNbV = aMapVI.Extent();
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Vertex& aV=*((TopoDS_Vertex*)&aMapVI.FindKey(i));
    Bnd_Box aBox;
    //
    aTol=BRep_Tool::Tolerance(aV);
    aBox.SetGap(aTol);
    aBox.Add(BRep_Tool::Pnt(aV));
    aBox.Enlarge(aTol);
    //
    aTreeFiller.Add(i, aBox);
    //
    BOPAlgo_TNV& aTNV=aVTNV.Append1();
    aTNV.SetTree(aBBTree);
    aTNV.SetBox(aBox);
    aTNV.SetVertex(aV);
  }
  //
  aTreeFiller.Fill();
  //
  //===========================================
  BOPAlgo_TNVCnt::Perform(myRunParallel, aVTNV);
  //===========================================
  //
  // Chains
  for (i=1; i<=aNbV; ++i) {
    if (!aMFence.Add(i)) {
      continue;
    }
    //
    Standard_Integer aIP, aNbIP1, aIP1;
    BOPCol_ListOfShape aLVSD;
    BOPCol_ListOfInteger aLIP, aLIP1, aLIPC;
    BOPCol_ListIteratorOfListOfInteger aItLIP;
    //
    aLIPC.Append(i);
    aLIP.Append(i);
    for(;;) {
      aItLIP.Initialize(aLIP);
      for(; aItLIP.More(); aItLIP.Next()) {
        aIP=aItLIP.Value();
        //
        BOPAlgo_TNV& aTNV=aVTNV(aIP-1);
        const BOPCol_ListOfInteger& aLI=aTNV.Indices();
        aIt.Initialize(aLI);
        for (; aIt.More(); aIt.Next()) {
          aIP1=aIt.Value();
          if (!aMFence.Add(aIP1)) {
            continue;
          }
          aLIP1.Append(aIP1);
        } //for (; aIt.More(); aIt.Next()) {
      }//for(; aIt1.More(); aIt1.Next()) {
      //
      aNbIP1=aLIP1.Extent();
      if (!aNbIP1) {
        break; // from for(;;) 
      }
      //
      aLIP.Clear();
      aItLIP.Initialize(aLIP1);
      for(; aItLIP.More(); aItLIP.Next()) {
        aIP=aItLIP.Value();
        aLIP.Append(aIP);
        aLIPC.Append(aIP);
      }
      aLIP1.Clear();
    }// for(;;) {
    //
    aItLIP.Initialize(aLIPC);
    for(; aItLIP.More(); aItLIP.Next()) {
      aIP=aItLIP.Value();
      const TopoDS_Vertex& aVP=aVTNV(aIP-1).Vertex(); 
      aLVSD.Append(aVP);
    }
    aVF=aLVSD.First();
    aDMVLV.Add(aVF, aLVSD);
  }// for (i=1; i<=aNbV; ++i) {

  // Make new vertices
  aNbV = aDMVLV.Extent();
  for (i = 1; i <= aNbV; ++i) {
    const TopoDS_Shape& aV = aDMVLV.FindKey(i);
    const BOPCol_ListOfShape& aLVSD = aDMVLV(i);
    if (aLVSD.IsEmpty()) {
      myImages.Add(aV, aLVSD);
    }
    else {
      BOPCol_ListOfShape* pLVSD=(BOPCol_ListOfShape*)&aLVSD;
      BOPTools_AlgoTools::MakeVertex(*pLVSD, aVnew);
      myImages.Add(aVnew, aLVSD);
    } 
  }
}
//=======================================================================
//function : FillShrunkData
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::FillShrunkData(Handle(BOPDS_PaveBlock)& thePB)
{
  Standard_Integer nE, nV1, nV2, iErr;
  Standard_Real aT1, aT2, aTS1, aTS2;
  IntTools_ShrunkRange aSR;
  //
  myErrorStatus=0;
  myWarningStatus = 0;
  //
  const BOPDS_Pave& aPave1=thePB->Pave1();
  nV1=aPave1.Index();
  aT1=aPave1.Parameter();
  const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1))); 
  //
  const BOPDS_Pave& aPave2=thePB->Pave2();
  nV2=aPave2.Index();
  aT2=aPave2.Parameter();
  const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2))); 
  //
  nE=thePB->OriginalEdge();
  const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE))); 
  //
  aSR.SetContext(myContext);
  aSR.SetData(aE, aT1, aT2, aV1, aV2);
  //
  aSR.Perform();
  iErr=aSR.ErrorStatus();
  if (iErr) {
    myWarningStatus = 1;
    //myErrorStatus=40;
    return;
  }
  //
  aSR.ShrunkRange(aTS1, aTS2);
  const Bnd_Box& aBox=aSR.BndBox();
  //
  thePB->SetShrunkData(aTS1, aTS2, aBox);
}
//=======================================================================
//function : ForceInterfVE
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::ForceInterfVE(const Standard_Integer nV,
                                       Handle(BOPDS_PaveBlock)& aPB,
                                       BOPDS_MapOfPaveBlock& aMPBToUpdate)
{
  Standard_Integer aNbPnt, nE;
  gp_Pnt aP;
  //
  nE = aPB->OriginalEdge();
  //
  const BOPDS_ShapeInfo& aSIE=myDS->ShapeInfo(nE);
  if (aSIE.HasSubShape(nV)) {
    return;
  }
  //
  if (myDS->HasInterf(nV, nE)) {
    return;
  }   
  //
  if (myDS->HasInterfShapeSubShapes(nV, nE)) {
    return;
  }
  //
  if (aPB->Pave1().Index() == nV || aPB->Pave2().Index() == nV) {
    return;
  }
  //
  const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&myDS->Shape(nV);
  const TopoDS_Edge&   aE = *(TopoDS_Edge*)  &myDS->Shape(nE);
  aP=BRep_Tool::Pnt(aV);
  //
  GeomAPI_ProjectPointOnCurve& aProjector=myContext->ProjPC(aE);
  aProjector.Perform(aP);
  //
  aNbPnt = aProjector.NbPoints();
  if (aNbPnt) {
    Standard_Real aT, aDist;
    //Standard_Integer i;
    BRep_Builder aBB;
    BOPDS_Pave aPave;
    //
    aDist=aProjector.LowerDistance();
    aT=aProjector.LowerDistanceParameter();
    //
    BOPDS_VectorOfInterfVE& aVEs=myDS->InterfVE();
    aVEs.SetIncrement(10);
    BOPDS_InterfVE& aVE=aVEs.Append1();
    aVE.SetIndices(nV, nE);
    aVE.SetParameter(aT);
    //
    myDS->AddInterf(nV, nE);
    //
    aBB.UpdateVertex(aV, aDist);
    BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nV);
    Bnd_Box& aBox=aSIDS.ChangeBox();
    BRepBndLib::Add(aV, aBox);
    //
    aPave.SetIndex(nV);
    aPave.SetParameter(aT);
    aPB->AppendExtPave(aPave);
    //
    aMPBToUpdate.Add(aPB);
  }
}

 /*
  // DEBf
  { 
    TopoDS_Compound aCx;
    BRep_Builder aBBx;
    aBBx.MakeCompound(aCx);
    aItMVCPB.Initialize(theMVCPB);
    for (; aItMVCPB.More(); aItMVCPB.Next()) {
      const TopoDS_Shape& aS=aItMVCPB.Key();
      aBBx.Add(aCx, aS);
    }
    BRepTools::Write(aCx, "cx");
  }
  // DEBt
  */
