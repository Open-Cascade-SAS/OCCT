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
#include <BOPAlgo_Tools.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_VectorOfInterfEE.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_SequenceOfRanges.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

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
  void SetFuzzyValue(const Standard_Real theFuzz) {
    IntTools_EdgeEdge::SetFuzzyValue(theFuzz);
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
// function: PerformEE
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformEE()
{
  FillShrunkData(TopAbs_EDGE, TopAbs_EDGE);
  //
  myIterator->Initialize(TopAbs_EDGE, TopAbs_EDGE);
  Standard_Integer iSize = myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  Standard_Boolean bExpressCompute, bIsPBSplittable1, bIsPBSplittable2;
  Standard_Integer i, iX, nE1, nE2, aNbCPrts, k, aNbEdgeEdge;
  Standard_Integer nV11, nV12, nV21, nV22;
  Standard_Real aTS11, aTS12, aTS21, aTS22, aT11, aT12, aT21, aT22;
  TopAbs_ShapeEnum aType;
  BOPDS_ListIteratorOfListOfPaveBlock aIt1, aIt2;
  Handle(NCollection_BaseAllocator) aAllocator;
  BOPAlgo_VectorOfEdgeEdge aVEdgeEdge;
  BOPDS_MapIteratorOfMapOfPaveBlock aItPB; 
  // keep modified edges for further update
  BOPCol_MapOfInteger aMEdges;
  //
  aAllocator=NCollection_BaseAllocator::CommonBaseAllocator();
  //-----------------------------------------------------scope f
  BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock aMPBLPB(100, aAllocator);
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks aMVCPB(100, aAllocator);
  BOPAlgo_DataMapOfPaveBlockBndBox aDMPBBox(100, aAllocator);
  //
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  aEEs.SetIncrement(iSize);
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nE1, nE2);
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
    BOPDS_ListOfPaveBlock& aLPB1 = myDS->ChangePaveBlocks(nE1);
    if (aLPB1.IsEmpty()) {
      continue;
    }
    //
    BOPDS_ListOfPaveBlock& aLPB2 = myDS->ChangePaveBlocks(nE2);
    if (aLPB2.IsEmpty()) {
      continue;
    }
    //
    const TopoDS_Edge& aE1=(*(TopoDS_Edge *)(&aSIE1.Shape()));
    const TopoDS_Edge& aE2=(*(TopoDS_Edge *)(&aSIE2.Shape()));
    //
    aIt1.Initialize(aLPB1);
    for (; aIt1.More(); aIt1.Next()) {
      Bnd_Box aBB1;
      //
      Handle(BOPDS_PaveBlock)& aPB1=aIt1.ChangeValue();
      //
      if (!GetPBBox(aE1, aPB1, aDMPBBox, aT11, aT12, aTS11, aTS12, aBB1)) {
        continue;
      }
      //
      aPB1->Indices(nV11, nV12);
      //
      aIt2.Initialize(aLPB2);
      for (; aIt2.More(); aIt2.Next()) {
        Bnd_Box aBB2;
        //
        Handle(BOPDS_PaveBlock)& aPB2=aIt2.ChangeValue();
        //
        if (!GetPBBox(aE2, aPB2, aDMPBBox, aT21, aT22, aTS21, aTS22, aBB2)) {
          continue;
        }
        //
        if (aBB1.IsOut(aBB2)) {
          continue;
        }
        //
        aPB2->Indices(nV21, nV22);
        //
        bExpressCompute=((nV11==nV21 && nV12==nV22) ||
                         (nV12==nV21 && nV11==nV22));
        //
        BOPAlgo_EdgeEdge& anEdgeEdge=aVEdgeEdge.Append1();
        //
        anEdgeEdge.UseQuickCoincidenceCheck(bExpressCompute);
        //
        anEdgeEdge.SetPaveBlock1(aPB1);
        anEdgeEdge.SetPaveBlock2(aPB2);
        //
        anEdgeEdge.SetEdge1(aE1, aT11, aT12);
        anEdgeEdge.SetEdge2(aE2, aT21, aT22);
        anEdgeEdge.SetFuzzyValue(myFuzzyValue);
        anEdgeEdge.SetProgressIndicator(myProgressIndicator);
      }//for (; aIt2.More(); aIt2.Next()) {
    }//for (; aIt1.More(); aIt1.Next()) {
  }//for (; myIterator->More(); myIterator->Next()) {
  //
  aNbEdgeEdge=aVEdgeEdge.Extent();
  //======================================================
  BOPAlgo_EdgeEdgeCnt::Perform(myRunParallel, aVEdgeEdge);
  //======================================================
  //
  for (k = 0; k < aNbEdgeEdge; ++k) {
    Bnd_Box aBB1, aBB2;
    //
    BOPAlgo_EdgeEdge& anEdgeEdge=aVEdgeEdge(k);
    if (!anEdgeEdge.IsDone()) {
      continue;
    }
    //
    const IntTools_SequenceOfCommonPrts& aCPrts = anEdgeEdge.CommonParts();
    aNbCPrts = aCPrts.Length();
    if (!aNbCPrts) {
      continue;
    }
    //--------------------------------------------
    Handle(BOPDS_PaveBlock)& aPB1=anEdgeEdge.PaveBlock1();
    nE1=aPB1->OriginalEdge();
    aPB1->Range(aT11, aT12);
    if (!aPB1->HasShrunkData()) {
      aTS11 = aT11;
      aTS12 = aT12;
      bIsPBSplittable1 = Standard_False;
    }
    else {
      aPB1->ShrunkData(aTS11, aTS12, aBB1, bIsPBSplittable1);
    }
    //
    Handle(BOPDS_PaveBlock)& aPB2=anEdgeEdge.PaveBlock2();
    nE2=aPB2->OriginalEdge();
    aPB2->Range(aT21, aT22);
    if (!aPB2->HasShrunkData()) {
      aTS21 = aT21;
      aTS22 = aT22;
      bIsPBSplittable2 = Standard_False;
    }
    else {
      aPB2->ShrunkData(aTS21, aTS22, aBB2, bIsPBSplittable2);
    }
    //
    //--------------------------------------------
    IntTools_Range aR11(aT11, aTS11), aR12(aTS12, aT12),
                   aR21(aT21, aTS21), aR22(aTS22, aT22);
    //
    Standard_Boolean bAnalytical = Standard_False;
    {
      const TopoDS_Edge& aOE1 = *(TopoDS_Edge*)&myDS->Shape(nE1);
      const TopoDS_Edge& aOE2 = *(TopoDS_Edge*)&myDS->Shape(nE2);
      //
      BRepAdaptor_Curve aBAC1(aOE1), aBAC2(aOE2);
      //
      GeomAbs_CurveType aType1 = aBAC1.GetType();
      GeomAbs_CurveType aType2 = aBAC2.GetType();
      //
      bAnalytical = (((aType1 == GeomAbs_Line) &&
                      (aType2 == GeomAbs_Line ||
                       aType2 == GeomAbs_Circle)) ||
                     ((aType2 == GeomAbs_Line) &&
                      (aType1 == GeomAbs_Line ||
                       aType1 == GeomAbs_Circle)));
    }
    //
    for (i=1; i<=aNbCPrts; ++i) {
      const IntTools_CommonPrt& aCPart=aCPrts(i);
      //
      const TopoDS_Edge& aE1=aCPart.Edge1();
      const TopoDS_Edge& aE2=aCPart.Edge2();
      //
      aType=aCPart.Type();
      switch (aType) {
        case TopAbs_VERTEX:  { 
          if (!bIsPBSplittable1 || !bIsPBSplittable2) {
            continue;
          }
          //
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
              ForceInterfVE(nV[j], aPB, aMEdges);
              bFlag = Standard_True;
              break;
            }
          }
          if (bFlag) {
            BOPDS_InterfEE& aEE = aEEs.Append1();
            aEE.SetIndices(nE1, nE2);
            aEE.SetCommonPart(aCPart);
            continue;
          }
          //
          BOPTools_AlgoTools::MakeNewVertex(aE1, aT1, aE2, aT2, aVnew);
          Standard_Real aTolVnew = BRep_Tool::Tolerance(aVnew);
          if (bAnalytical) {
            // increase tolerance for Line/Line intersection, but do not update 
            // the vertex till its intersection with some other shape
            Standard_Real aTolMin = (BRepAdaptor_Curve(aE1).GetType() == GeomAbs_Line) ?
              (aCR1.Last() - aCR1.First()) / 2. : (aCR2.Last() - aCR2.First()) / 2.;
            if (aTolMin > aTolVnew) {
              aTolVnew = aTolMin;
            }
          }
          // <-LXBR
          {
            Standard_Integer nVS[2], iFound;
            Standard_Real aTolVx, aD2, aDT2;
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
          aCPB.SetTolerance(aTolVnew);
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
          BOPAlgo_Tools::FillMap<Handle(BOPDS_PaveBlock), TColStd_MapTransientHasher>(aPB1, aPB2, aMPBLPB, aAllocator);
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
  BOPAlgo_Tools::PerformCommonBlocks(aMPBLPB, aAllocator, myDS);
  PerformNewVertices(aMVCPB, aAllocator);
  //
  if (aMEdges.Extent()) {
    Standard_Integer aNbV = aMVCPB.Extent();
    for (i = 1; i <= aNbV; ++i) {
      Handle(BOPDS_PaveBlock) aPB1, aPB2;
      const BOPDS_CoupleOfPaveBlocks& aCPB = aMVCPB.FindFromIndex(i);
      aCPB.PaveBlocks(aPB1, aPB2);
      //
      aMEdges.Remove(aPB1->OriginalEdge());
      aMEdges.Remove(aPB2->OriginalEdge());
    }
    //
    SplitPaveBlocks(aMEdges, Standard_False);
  }
  //
  //-----------------------------------------------------scope t
  aMPBLPB.Clear();
  aMVCPB.Clear();
}
//=======================================================================
//function : PerformVerticesEE
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::PerformNewVertices
  (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB,
   const Handle(NCollection_BaseAllocator)& theAllocator,
   const Standard_Boolean bIsEEIntersection)
{
  Standard_Integer aNbV = theMVCPB.Extent();
  if (!aNbV) {
    return;
  }
  //
  Standard_Real aTolAdd = myFuzzyValue / 2.;
  //
  // 1. Fuse the new vertices
  BOPCol_IndexedDataMapOfShapeListOfShape aImages;
  TreatNewVertices(theMVCPB, aImages);
  //
  // 2. Add new vertices to myDS and connect indices to CPB structure
  BOPDS_VectorOfInterfEE& aEEs = myDS->InterfEE();
  BOPDS_VectorOfInterfEF& aEFs = myDS->InterfEF();
  //
  Standard_Integer i, aNb = aImages.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aImages.FindKey(i));
    const BOPCol_ListOfShape& aLVSD = aImages.FindFromIndex(i);
    //
    BOPDS_ShapeInfo aSI;
    aSI.SetShapeType(TopAbs_VERTEX);
    aSI.SetShape(aV);
    Standard_Integer iV = myDS->Append(aSI);
    //
    BOPDS_ShapeInfo& aSIDS = myDS->ChangeShapeInfo(iV);
    Bnd_Box& aBox = aSIDS.ChangeBox();
    aBox.Add(BRep_Tool::Pnt(aV));
    aBox.SetGap(BRep_Tool::Tolerance(aV) + aTolAdd);
    //
    BOPCol_ListIteratorOfListOfShape aItLS(aLVSD);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aVx = aItLS.Value();
      BOPDS_CoupleOfPaveBlocks &aCPB = theMVCPB.ChangeFromKey(aVx);
      aCPB.SetIndex(iV);
      // update interference
      Standard_Integer iX = aCPB.IndexInterf();
      BOPDS_Interf *aInt = bIsEEIntersection ? (BOPDS_Interf*)(&aEEs(iX)) : (BOPDS_Interf*) (&aEFs(iX));
      aInt->SetIndexNew(iV);
    }
  }
  //
  // 3. Map PaveBlock/ListOfVertices to add to this PaveBlock ->aMPBLI
  BOPDS_IndexedDataMapOfPaveBlockListOfInteger aMPBLI(100, theAllocator);
  for (i = 1; i <= aNbV; ++i) {
    const BOPDS_CoupleOfPaveBlocks& aCPB = theMVCPB.FindFromIndex(i);
    Standard_Integer iV = aCPB.Index();
    //
    Handle(BOPDS_PaveBlock) aPB[2];
    aCPB.PaveBlocks(aPB[0], aPB[1]);
    for (Standard_Integer j = 0; j < 2; ++j) {
      BOPCol_ListOfInteger *pLI = aMPBLI.ChangeSeek(aPB[j]);
      if (!pLI) {
        pLI = &aMPBLI(aMPBLI.Add(aPB[j], BOPCol_ListOfInteger(theAllocator)));
      }
      pLI->Append(iV);
      //
      if (aPB[0] == aPB[1]) {
        break;
      }
    }
  }
  //
  // 4. Compute Extra Paves and split Pave blocks by the Extra paves
  IntersectVE(aMPBLI, Standard_False);
}
//=======================================================================
//function : TreatNewVertices
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::TreatNewVertices
  (const BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB,
   BOPCol_IndexedDataMapOfShapeListOfShape& myImages)
{
  //
  // Prepare for intersection
  BOPCol_IndexedDataMapOfShapeReal aVerts;
  Standard_Integer i, aNbV = theMVCPB.Extent();
  for (i = 1; i <= aNbV; ++i) {
    const TopoDS_Shape& aV = theMVCPB.FindKey(i);
    Standard_Real aTol = theMVCPB.FindFromIndex(i).Tolerance();
    aVerts.Add(aV, aTol);
  }
  //
  // Perform intersection
  BOPCol_ListOfListOfShape aChains;
  BOPAlgo_Tools::IntersectVertices(aVerts, myRunParallel, myFuzzyValue, aChains);
  //
  // Treat the results - make new vertices for each chain
  BOPCol_ListOfListOfShape::Iterator aItC(aChains);
  for (; aItC.More(); aItC.Next()) {
    const BOPCol_ListOfShape& aLVSD = aItC.Value();
    //
    TopoDS_Vertex aVNew;
    BOPTools_AlgoTools::MakeVertex(aLVSD, aVNew);
    myImages.Add(aVNew, aLVSD);
  }
}
//=======================================================================
//function : FillShrunkData
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::FillShrunkData(Handle(BOPDS_PaveBlock)& thePB)
{
  // Vertices
  Standard_Integer nV1, nV2;
  thePB->Indices(nV1, nV2);
  const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1))); 
  const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2))); 
  // Original edge
  Standard_Integer nE = thePB->OriginalEdge();
  const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE))); 
  // Range
  Standard_Real aT1, aT2;
  thePB->Range(aT1, aT2);
  //
  IntTools_ShrunkRange aSR;
  aSR.SetContext(myContext);
  aSR.SetData(aE, aT1, aT2, aV1, aV2);
  aSR.Perform();
  // Analyze the results of computations
  AnalyzeShrunkData(thePB, aSR);
}
//=======================================================================
// function: AnalyzeShrunkData
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::AnalyzeShrunkData(const Handle(BOPDS_PaveBlock)& thePB,
                                           const IntTools_ShrunkRange& theSR)
{
  // in case of error treat the warning status
  Standard_Boolean bWholeEdge = Standard_False;
  TopoDS_Shape aWarnShape;
  //
  if (!theSR.IsDone() || !theSR.IsSplittable()) {
    Standard_Real aEFirst, aELast, aPBFirst, aPBLast;
    BRep_Tool::Range(theSR.Edge(), aEFirst, aELast);
    thePB->Range(aPBFirst, aPBLast);
    bWholeEdge = !(aPBFirst > aEFirst || aPBLast < aELast);
    if (bWholeEdge) {
      aWarnShape = theSR.Edge();
    }
    else {
      const TopoDS_Shape& aV1 = myDS->Shape(thePB->Pave1().Index());
      const TopoDS_Shape& aV2 = myDS->Shape(thePB->Pave2().Index());
      BRep_Builder().MakeCompound(TopoDS::Compound(aWarnShape));
      BRep_Builder().Add(aWarnShape, theSR.Edge());
      BRep_Builder().Add(aWarnShape, aV1);
      BRep_Builder().Add(aWarnShape, aV2);
    }
    //
    if (!theSR.IsDone()) {
      if (bWholeEdge)
        AddWarning (new BOPAlgo_AlertTooSmallEdge (aWarnShape));
      else
        AddWarning (new BOPAlgo_AlertBadPositioning (aWarnShape));
      return;
    }
    //
    if (bWholeEdge)
      AddWarning (new BOPAlgo_AlertNotSplittableEdge (aWarnShape));
    else
      AddWarning (new BOPAlgo_AlertBadPositioning (aWarnShape));
  }
  //
  Standard_Real aTS1, aTS2;
  theSR.ShrunkRange(aTS1, aTS2);
  Bnd_Box aBox = theSR.BndBox();
  aBox.SetGap(aBox.GetGap() + myFuzzyValue / 2.);
  thePB->SetShrunkData(aTS1, aTS2, aBox, theSR.IsSplittable());
}
//=======================================================================
//function : ForceInterfVE
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::ForceInterfVE(const Standard_Integer nV,
                                       Handle(BOPDS_PaveBlock)& aPB,
                                       BOPCol_MapOfInteger& theMEdges)
{
  Standard_Integer nE, nVx, nVSD, iFlag;
  Standard_Real aT, aTolVNew;
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
  if (aPB->Pave1().Index() == nV || 
      aPB->Pave2().Index() == nV) {
    return;
  }
  //
  nVx = nV;
  if (myDS->HasShapeSD(nV, nVSD)) {
    nVx = nVSD;
  }
  //
  const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&myDS->Shape(nVx);
  const TopoDS_Edge&   aE = *(TopoDS_Edge*)  &myDS->Shape(nE);
  //
  iFlag = myContext->ComputeVE(aV, aE, aT, aTolVNew, myFuzzyValue);
  if (iFlag == 0 || iFlag == -4) {
    BOPDS_Pave aPave;
    //
    //
    BOPDS_VectorOfInterfVE& aVEs=myDS->InterfVE();
    aVEs.SetIncrement(10);
    // 1
    BOPDS_InterfVE& aVE=aVEs.Append1();
    aVE.SetIndices(nV, nE);
    aVE.SetParameter(aT);
    // 2
    myDS->AddInterf(nV, nE);
    //
    // 3 update vertex V/E if necessary
    nVx=UpdateVertex(nV, aTolVNew);
    // 4
    if (myDS->IsNewShape(nVx)) {
      aVE.SetIndexNew(nVx);
    }
    // 5 append ext pave to pave block
    aPave.SetIndex(nVx);
    aPave.SetParameter(aT);
    aPB->AppendExtPave(aPave);
    //
    theMEdges.Add(nE);
    //
    // check for self-interference
    Standard_Integer iRV = myDS->Rank(nV);
    if (iRV >= 0 && iRV == myDS->Rank(nE)) {
      // add warning status
      TopoDS_Compound aWC;
      BRep_Builder().MakeCompound(aWC);
      BRep_Builder().Add(aWC, aV);
      BRep_Builder().Add(aWC, aE);
      AddWarning (new BOPAlgo_AlertSelfInterferingShape (aWC));
    }
  }
}

//=======================================================================
//function : GetPBBox
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::GetPBBox(const TopoDS_Edge& theE,
                                              const Handle(BOPDS_PaveBlock)& thePB,
                                              BOPAlgo_DataMapOfPaveBlockBndBox& thePBBox,
                                              Standard_Real& theFirst,
                                              Standard_Real& theLast,
                                              Standard_Real& theSFirst,
                                              Standard_Real& theSLast,
                                              Bnd_Box& theBox)
{
  thePB->Range(theFirst, theLast);
  // check the validity of PB's range
  Standard_Boolean bValid = theLast - theFirst > Precision::PConfusion();
  if (!bValid) {
    return bValid;
  }
  //
  // check shrunk data
  if (thePB->HasShrunkData()) {
    Standard_Boolean bIsSplittable;
    thePB->ShrunkData(theSFirst, theSLast, theBox, bIsSplittable);
    return bValid;
  }
  //
  theSFirst = theFirst;
  theSLast = theLast;
  // check the map
  if (thePBBox.IsBound(thePB)) {
    theBox = thePBBox.Find(thePB);
  }
  else {
    // build bounding box
    BRepAdaptor_Curve aBAC(theE);
    Standard_Real aTol = BRep_Tool::Tolerance(theE) + Precision::Confusion();
    BndLib_Add3dCurve::Add(aBAC, theSFirst, theSLast, aTol, theBox);
    thePBBox.Bind(thePB, theBox);
  }
  return bValid;
}
