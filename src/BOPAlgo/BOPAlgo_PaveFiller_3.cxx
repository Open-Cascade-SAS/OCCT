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

#include <Precision.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_UBTreeFiller.hxx>

#include <Bnd_Box.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepBndLib.hxx>
//
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_SequenceOfRanges.hxx>
//
#include <BOPTools_AlgoTools.hxx>
//
#include <BOPCol_DataMapOfShapeInteger.hxx>
#include <BOPCol_DataMapOfIntegerShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeBox.hxx>
//
#include <BOPInt_Context.hxx>
#include <BOPInt_ShrunkRange.hxx>
#include <BOPInt_Tools.hxx>
//
#include <BOPDS_DataMapOfPaveBlockListOfPaveBlock.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfInteger.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_VectorOfInterfEE.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_BoxBndTree.hxx>

#include <BOPAlgo_Tools.hxx>

//=======================================================================
// function: PerformEE
// purpose: 
//=======================================================================
  void BOPAlgo_PaveFiller::PerformEE()
{
  Standard_Boolean bJustAdd, bOrder;
  Standard_Integer i, iX, iSize, nE1, nE2, aDiscretize;
  Standard_Integer aNbCPrts, nWhat, nWith;
  Standard_Real aTS11, aTS12, aTS21, aTS22;
  Standard_Real aTolE1, aTolE2, aDeflection;
  TopAbs_ShapeEnum aType;
  TopoDS_Edge aEWhat, aEWith; 
  BOPDS_ListIteratorOfListOfPaveBlock aIt1, aIt2;
  Handle(NCollection_IncAllocator) aAllocator;
  //
  myErrorStatus=0;
  //
  myIterator->Initialize(TopAbs_EDGE, TopAbs_EDGE);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  //-----------------------------------------------------scope f
  aAllocator=new NCollection_IncAllocator();
  BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock aMPBLPB(100, aAllocator);
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks aMVCPB(100, aAllocator);
  //
  aDiscretize=30;
  aDeflection=0.01;
  //
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  aEEs.SetStartSize(iSize);
  aEEs.SetIncrement(iSize);
  aEEs.Init();
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
    aTolE1=BRep_Tool::Tolerance(aE1);
    aTolE2=BRep_Tool::Tolerance(aE2);
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
        FillShrunkData(aPB1);
        if (myWarningStatus) {
          continue;
        }
      }
      aPB1->ShrunkData(aTS11, aTS12, aBB1);
      //
      aIt2.Initialize(aLPB2);
      for (; aIt2.More(); aIt2.Next()) {
        Bnd_Box aBB2;
        //
        Handle(BOPDS_PaveBlock)& aPB2=aIt2.ChangeValue();
        if (!aPB2->HasShrunkData()) {
          FillShrunkData(aPB2);
          if (myWarningStatus) {
            continue;
          }
        }
        aPB2->ShrunkData(aTS21, aTS22, aBB2);
        //
        if (aBB1.IsOut(aBB2)) {
          continue;
        }
        //
        // -----------f
        //DEBft
        //printf(" nE1=%d nE2=%d\n", nE1, nE2);
        //
        IntTools_EdgeEdge aEdgeEdge;
        //
        aEdgeEdge.SetEdge1 (aE1);
        aEdgeEdge.SetEdge2 (aE2);
        aEdgeEdge.SetTolerance1 (aTolE1);
        aEdgeEdge.SetTolerance2 (aTolE2);
        aEdgeEdge.SetDiscretize (aDiscretize);
        aEdgeEdge.SetDeflection (aDeflection);
        //
        IntTools_Range aSR1(aTS11, aTS12);
        IntTools_Range aSR2(aTS21, aTS22);
        IntTools_Range anewSR1 = aSR1;
        IntTools_Range anewSR2 = aSR2;
        //
        BOPTools_AlgoTools::CorrectRange (aE1, aE2, aSR1, anewSR1);
        BOPTools_AlgoTools::CorrectRange (aE2, aE1, aSR2, anewSR2);
        //
        aEdgeEdge.SetRange1(anewSR1);
        aEdgeEdge.SetRange2(anewSR2);
        //
        aEdgeEdge.Perform();
        if (!aEdgeEdge.IsDone()) {
          continue;
        }
        //
        bOrder=aEdgeEdge.Order();
        if (!bOrder) {
          aEWhat=aE1;
          aEWith=aE2;
          nWhat=nE1;
          nWith=nE2;
        }
        else {
          nWhat=nE2;
          nWith=nE1;
          aEWhat=aE2;
          aEWith=aE1;
        }
        //
        const IntTools_SequenceOfCommonPrts& aCPrts=aEdgeEdge.CommonParts();
        //
        aNbCPrts=aCPrts.Length();
        for (i=1; i<=aNbCPrts; ++i) {
          const IntTools_CommonPrt& aCPart=aCPrts(i);
          aType=aCPart.Type();
          switch (aType) {
            case TopAbs_VERTEX:  { 
              Standard_Boolean bIsOnPave1, bIsOnPave2;
              Standard_Real aT1, aT2, aTol;
              IntTools_Range aR1, aR2;
              TopoDS_Vertex aVnew;
              //
              BOPInt_Tools::VertexParameters(aCPart, aT1, aT2);
              aTol=Precision::Confusion();
              // 
              //decide to keep the pave or not
              aR1 = (bOrder) ? anewSR2 : anewSR1;
              aR2 = (bOrder) ? anewSR1 : anewSR2;
              //
              bIsOnPave1=BOPInt_Tools::IsOnPave(aT1, aR1, aTol);
              bIsOnPave2=BOPInt_Tools::IsOnPave(aT2, aR2, aTol);
              //
              if(bIsOnPave1 || bIsOnPave2) {
                continue;
              }
              //
              BOPTools_AlgoTools::MakeNewVertex(aEWhat, aT1, aEWith, aT2, aVnew);
              // <-LXBR
              {
                Standard_Integer nV11, nV12, nV21, nV22, nVS[2], k, j, iFound;
                Standard_Real aTolVx, aTolVnew, aD2, aDT2;
                BOPCol_MapOfInteger aMV;
                gp_Pnt aPnew, aPx;
                //
                iFound=0;
                j=-1;
                nV11=aPB1->Pave1().Index();
                nV12=aPB1->Pave2().Index();
                nV21=aPB2->Pave1().Index();
                nV22=aPB2->Pave2().Index();
                aMV.Add(nV11);
                aMV.Add(nV12);
                //
                if (aMV.Contains(nV21)) {
                  ++j;
                  nVS[j]=nV21;
                }
                if (aMV.Contains(nV22)) {
                  ++j;
                  nVS[j]=nV22;
                }
                //
                aTolVnew=BRep_Tool::Tolerance(aVnew);
                aPnew=BRep_Tool::Pnt(aVnew);
                //
                for (k=0; k<=j; ++k) {
                  const TopoDS_Vertex& aVx= *(TopoDS_Vertex*)&(myDS->Shape(nVS[k]));
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
                            
              // 1
              iX=aEEs.Append()-1;
              BOPDS_InterfEE& aEE=aEEs(iX);
              aEE.SetIndices(nWhat, nWith);
              aEE.SetCommonPart(aCPart);
              // 2
              myDS->AddInterf(nWhat, nWith);
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
              Standard_Boolean bHasSameBounds;
              Standard_Integer aNbComPrt2;
              //
              aNbComPrt2=aCPart.Ranges2().Length();
              if (aNbComPrt2>1){
                break;
              }
              //// <-LXBR   
              bHasSameBounds=aPB1->HasSameBounds(aPB2);
              if (!bHasSameBounds) {
                break;
              }
              // 1
              iX=aEEs.Append()-1;
              BOPDS_InterfEE& aEE=aEEs(iX);
              aEE.SetIndices(nWhat, nWith);
              aEE.SetCommonPart(aCPart);
              // 2
              myDS->AddInterf(nWhat, nWith);
              //
              BOPAlgo_Tools::FillMap(aPB1, aPB2, aMPBLPB, aAllocator);
            }//case TopAbs_EDGE
            break;
            default:
              break;
          }//switch (aType) {
        }//for (i=1; i<=aNbCPrts; i++) {
        // -----------t
        //
      }// for (; aIt2.More(); aIt2.Next()) {
    }// for (; aIt1.More(); aIt1.Next()) {
  }
  // 
  //=========================================
  // post treatment
  //=========================================
  BOPAlgo_Tools::PerformCommonBlocks(aMPBLPB, aAllocator);
  PerformVerticesEE(aMVCPB, aAllocator);
  //-----------------------------------------------------scope t
  aMPBLPB.Clear();
  aMVCPB.Clear();
  aAllocator.Nullify();
}
//=======================================================================
//function : PerformVertices
//purpose  : 
//=======================================================================
  Standard_Integer BOPAlgo_PaveFiller::PerformVerticesEE
    (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB,
     Handle(NCollection_BaseAllocator)& theAllocator)
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
  TopoDS_Shape aV;
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
  //
  // 5 
  // 5.1  Compute Extra Paves and 
  // 5.2. Add Extra Paves to the PaveBlocks
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
      iFlag=myContext->ComputeVE (aVx, aE, aT);
      if (!iFlag) {
        aPave.SetIndex(nVx);
        aPave.SetParameter(aT);
        aPB->AppendExtPave(aPave);
      }
    }
  }
  // 6  Split PaveBlocksa
  aNb=aMPBLI.Extent();
  for(i=1; i<=aNb; ++i) {
    Handle(BOPDS_PaveBlock) aPB=aMPBLI.FindKey(i);
    nE=aPB->OriginalEdge();
    // 3
    if (!aPB->IsCommonBlock()) {
      myDS->UpdatePaveBlock(aPB);
    }
    else {
      const Handle(BOPDS_CommonBlock)& aCB=aPB->CommonBlock();
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
  void BOPAlgo_PaveFiller::TreatNewVertices(
       const BOPCol_IndexedDataMapOfShapeInteger& aMapVI,
       BOPCol_IndexedDataMapOfShapeListOfShape& myImages)
{
  Standard_Integer j, i, aNbV, aNbVSD;
  Standard_Real aTol;
  TopoDS_Shape aVF;
  TopoDS_Vertex aVnew;
  BOPCol_IndexedMapOfShape aMVProcessed;

  BOPCol_ListIteratorOfListOfInteger aIt;
  BOPCol_IndexedDataMapOfShapeListOfShape aMVLV;
  BOPCol_DataMapOfIntegerShape aMIS;
  BOPCol_IndexedDataMapOfShapeBox aMSB;
  //
  BOPDS_BoxBndTreeSelector aSelector;
  BOPDS_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  aNbV = aMapVI.Extent();
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Shape& aV=aMapVI.FindKey(i);
    Bnd_Box aBox;
    //
    aTol=BRep_Tool::Tolerance(*(TopoDS_Vertex*)(&aV));
    aBox.SetGap(aTol);
    BRepBndLib::Add(aV, aBox);
    //
    aTreeFiller.Add(i, aBox);
    //
    aMIS.Bind(i, aV);
    aMSB.Add(aV, aBox);
  }
  //
  aTreeFiller.Fill();

  // Chains
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Shape& aV=aMapVI.FindKey(i);
    //
    if (aMVProcessed.Contains(aV)) {
      continue;
    }
    //
    Standard_Integer aNbIP, aIP, aNbIP1, aIP1;
    BOPCol_ListOfShape aLVSD;
    BOPCol_MapOfInteger aMIP, aMIP1, aMIPC;
    BOPCol_MapIteratorOfMapOfInteger aIt1;
    //
    aMIP.Add(i);
    while(1) {
      aNbIP=aMIP.Extent();
      aIt1.Initialize(aMIP);
      for(; aIt1.More(); aIt1.Next()) {
        aIP=aIt1.Key();
        if (aMIPC.Contains(aIP)) {
          continue;
        }
        //
        const TopoDS_Shape& aVP=aMIS.Find(aIP);
        const Bnd_Box& aBoxVP=aMSB.FindFromKey(aVP);
        //
        aSelector.Clear();
        aSelector.SetBox(aBoxVP);
        //
        aNbVSD=aBBTree.Select(aSelector);
        if (!aNbVSD) {
          continue;  // it must not be
        }
        //
        const BOPCol_ListOfInteger& aLI=aSelector.Indices();
        aIt.Initialize(aLI);
        for (; aIt.More(); aIt.Next()) {
          aIP1=aIt.Value();
          if (aMIP.Contains(aIP1)) {
            continue;
          }
          aMIP1.Add(aIP1);
        } //for (; aIt.More(); aIt.Next()) {
      }//for(; aIt1.More(); aIt1.Next()) {
      //
      aNbIP1=aMIP1.Extent();
      if (!aNbIP1) {
        break; // from while(1)
      }
      //
      aIt1.Initialize(aMIP);
      for(; aIt1.More(); aIt1.Next()) {
        aIP=aIt1.Key();
        aMIPC.Add(aIP);
      }
      //
      aMIP.Clear();
      aIt1.Initialize(aMIP1);
      for(; aIt1.More(); aIt1.Next()) {
        aIP=aIt1.Key();
        aMIP.Add(aIP);
      }
      aMIP1.Clear();
    }// while(1)
    //...
    aNbIP=aMIPC.Extent();
    if (!aNbIP) {
      aMIPC.Add(i);
    }
    //
    aIt1.Initialize(aMIPC);
    for(j=0; aIt1.More(); aIt1.Next(), ++j) {
      aIP=aIt1.Key();
      const TopoDS_Shape& aVP=aMIS.Find(aIP);
      if (!j) {
        aVF=aVP;
      }
      aLVSD.Append(aVP);
      aMVProcessed.Add(aVP);
    }
    aMVLV.Add(aVF, aLVSD);
  }// for (i=1; i<=aNbV; ++i) {

  // Make new vertices
  aNbV=aMVLV.Extent();
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Shape& aV=aMVLV.FindKey(i);
    BOPCol_ListOfShape& aLVSD=aMVLV.ChangeFromIndex(i);
    aNbVSD=aLVSD.Extent();
    if (aNbVSD>1) {
      BOPTools_AlgoTools::MakeVertex(aLVSD, aVnew);
      myImages.Add(aVnew, aLVSD);
    } else {
      myImages.Add(aV, aLVSD);
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
  BOPInt_ShrunkRange aSR;
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
  aSR.SetData(aE, aT1, aT2, aV1, aV2, myContext);
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
