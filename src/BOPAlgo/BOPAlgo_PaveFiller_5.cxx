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
#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_EdgeFace.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_Tools.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//class    : BOPAlgo_EdgeFace
//purpose  : 
//=======================================================================
class BOPAlgo_EdgeFace : 
  public IntTools_EdgeFace,
  public BOPAlgo_Algo {
 
 public:
  DEFINE_STANDARD_ALLOC
  
  BOPAlgo_EdgeFace() : 
    IntTools_EdgeFace(), 
    BOPAlgo_Algo(),
    myIE(-1), myIF(-1) {
  };
  //
  virtual ~BOPAlgo_EdgeFace(){
  };
  //
  void SetIndices(const Standard_Integer nE,
                  const Standard_Integer nF) {
    myIE=nE;
    myIF=nF;
  }
  //
  void Indices(Standard_Integer& nE,
               Standard_Integer& nF) {
    nE=myIE;
    nF=myIF;
  }
  //
  void SetNewSR(const IntTools_Range& aR){
    myNewSR=aR;
  }
  //
  IntTools_Range& NewSR(){
    return myNewSR;
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
  void SetFuzzyValue(const Standard_Real theFuzz) {
    IntTools_EdgeFace::SetFuzzyValue(theFuzz);
  }
  //
  virtual void Perform() {
    BOPAlgo_Algo::UserBreak();
    try
    {
      OCC_CATCH_SIGNALS

      IntTools_EdgeFace::Perform();
    }
    catch (Standard_Failure)
    {
      AddError(new BOPAlgo_AlertIntersectionFailed);
    }
  }
  //
 protected:
  Standard_Integer myIE;
  Standard_Integer myIF;
  IntTools_Range myNewSR;
  Handle(BOPDS_PaveBlock) myPB;
};
//
//=======================================================================
typedef NCollection_Vector<BOPAlgo_EdgeFace> BOPAlgo_VectorOfEdgeFace; 
//
typedef BOPTools_ContextFunctor 
  <BOPAlgo_EdgeFace,
  BOPAlgo_VectorOfEdgeFace,
  Handle(IntTools_Context), 
  IntTools_Context> BOPAlgo_EdgeFaceFunctor;
//
typedef BOPTools_ContextCnt 
  <BOPAlgo_EdgeFaceFunctor,
  BOPAlgo_VectorOfEdgeFace,
  Handle(IntTools_Context)> BOPAlgo_EdgeFaceCnt;
//
//=======================================================================
//function : PerformEF
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::PerformEF()
{
  FillShrunkData(TopAbs_EDGE, TopAbs_FACE);
  //
  myIterator->Initialize(TopAbs_EDGE, TopAbs_FACE);
  Standard_Integer iSize = myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  Standard_Integer nE, nF;
  //
  if (myGlue == BOPAlgo_GlueFull) {
    // there is no need to intersect edges with faces in this mode
    // just initialize FaceInfo for faces
    for (; myIterator->More(); myIterator->Next()) {
      myIterator->Value(nE, nF);
      if (!myDS->ShapeInfo(nE).HasFlag()) {
        myDS->ChangeFaceInfo(nF);
      }
    }
    return;
  }
  //
  Standard_Boolean bV[2], bIsPBSplittable;
  Standard_Boolean bV1, bV2, bExpressCompute;
  Standard_Integer nV1, nV2;
  Standard_Integer i, aNbCPrts, iX, nV[2];
  Standard_Integer aNbEdgeFace, k;
  Standard_Real aTolE, aTolF, aTS1, aTS2, aT1, aT2;
  Handle(NCollection_BaseAllocator) aAllocator;
  TopAbs_ShapeEnum aType;
  BOPDS_ListIteratorOfListOfPaveBlock aIt;
  BOPAlgo_VectorOfEdgeFace aVEdgeFace; 
  //-----------------------------------------------------scope f
  //
  aAllocator=NCollection_BaseAllocator::CommonBaseAllocator();
  //
  TColStd_MapOfInteger aMIEFC(100, aAllocator);
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks aMVCPB(100, aAllocator);
  BOPDS_IndexedDataMapOfPaveBlockListOfInteger aMPBLI(100, aAllocator);
  BOPAlgo_DataMapOfPaveBlockBndBox aDMPBBox(100, aAllocator);
  //
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  aEFs.SetIncrement(iSize);
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nE, nF);
    //
    const BOPDS_ShapeInfo& aSIE=myDS->ShapeInfo(nE);
    if (aSIE.HasFlag()){//degenerated 
      continue;
    }
    //
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aSIE.Shape()));
    const TopoDS_Face& aF=(*(TopoDS_Face *)(&myDS->Shape(nF)));
    const Bnd_Box& aBBF=myDS->ShapeInfo(nF).Box(); 
    //
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
    const BOPDS_IndexedMapOfPaveBlock& aMPBF=aFI.PaveBlocksOn();
    //
    const TColStd_MapOfInteger& aMVIn=aFI.VerticesIn();
    const TColStd_MapOfInteger& aMVOn=aFI.VerticesOn();
    //
    aTolE=BRep_Tool::Tolerance(aE);
    aTolF=BRep_Tool::Tolerance(aF);
    //
    BOPDS_ListOfPaveBlock& aLPB=myDS->ChangePaveBlocks(nE);
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next()) {
      Handle(BOPDS_PaveBlock)& aPB=aIt.ChangeValue();
      //
      const Handle(BOPDS_PaveBlock) aPBR=myDS->RealPaveBlock(aPB);
      if (aMPBF.Contains(aPBR)) {
        continue;
      }
      //
      Bnd_Box aBBE;
      if (!GetPBBox(aE, aPB, aDMPBBox, aT1, aT2, aTS1, aTS2, aBBE)) {
        continue;
      }
      //
      if (aBBF.IsOut (aBBE)) {
        continue;
      }
      //
      aPBR->Indices(nV1, nV2);
      bV1=aMVIn.Contains(nV1) || aMVOn.Contains(nV1);
      bV2=aMVIn.Contains(nV2) || aMVOn.Contains(nV2);
      bExpressCompute=bV1 && bV2;
      //
      BOPAlgo_EdgeFace& aEdgeFace=aVEdgeFace.Appended();
      //
      aEdgeFace.SetIndices(nE, nF);
      aEdgeFace.SetPaveBlock(aPB);
      //
      aEdgeFace.SetEdge (aE);
      aEdgeFace.SetFace (aF);
      aEdgeFace.SetFuzzyValue(myFuzzyValue);
      aEdgeFace.UseQuickCoincidenceCheck(bExpressCompute);
      //
      IntTools_Range aSR(aTS1, aTS2);
      IntTools_Range anewSR=aSR;
      BOPTools_AlgoTools::CorrectRange(aE, aF, aSR, anewSR);
      aEdgeFace.SetNewSR(anewSR);
      //
      IntTools_Range aPBRange(aT1, aT2);
      aSR = aPBRange;
      BOPTools_AlgoTools::CorrectRange(aE, aF, aSR, aPBRange);
      aEdgeFace.SetRange (aPBRange);
      aEdgeFace.SetProgressIndicator(myProgressIndicator);
      //
    }//for (; aIt.More(); aIt.Next()) {
  }//for (; myIterator->More(); myIterator->Next()) {
  //
  aNbEdgeFace=aVEdgeFace.Length();
  //=================================================================
  BOPAlgo_EdgeFaceCnt::Perform(myRunParallel, aVEdgeFace, myContext);
  //=================================================================
  //
  for (k=0; k < aNbEdgeFace; ++k) {
    BOPAlgo_EdgeFace& aEdgeFace=aVEdgeFace(k);
    if (!aEdgeFace.IsDone() || aEdgeFace.HasErrors()) {
      // Warn about failed intersection of sub-shapes
      AddIntersectionFailedWarning(aEdgeFace.Edge(), aEdgeFace.Face());
      continue;
    }
    //
    const IntTools_SequenceOfCommonPrts& aCPrts=aEdgeFace.CommonParts();
    aNbCPrts = aCPrts.Length();
    if (!aNbCPrts) {
      continue;
    }
    //
    aEdgeFace.Indices(nE, nF);
    //
    const TopoDS_Edge& aE=aEdgeFace.Edge();
    const TopoDS_Face& aF=aEdgeFace.Face();
    //
    aTolE=BRep_Tool::Tolerance(aE);
    aTolF=BRep_Tool::Tolerance(aF);
    const IntTools_Range& anewSR=aEdgeFace.NewSR();
    Handle(BOPDS_PaveBlock)& aPB=aEdgeFace.PaveBlock();
    //
    aPB->Range(aT1, aT2);
    aPB->Indices(nV[0], nV[1]);
    bIsPBSplittable = aPB->IsSplittable();
    //
    anewSR.Range(aTS1, aTS2);
    //
    if (aCPrts(1).Type() == TopAbs_VERTEX) {
      // for the intersection type VERTEX
      // extend vertices ranges using Edge/Edge intersections
      // between the edge aE and the edges of the face aF.
      // thereby the edge's intersection range is reduced
      ReduceIntersectionRange(nV[0], nV[1], nE, nF, aTS1, aTS2);
    }
    //
    IntTools_Range aR1(aT1, aTS1), aR2(aTS2, aT2);
    //
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
    const TColStd_MapOfInteger& aMIFOn=aFI.VerticesOn();
    const TColStd_MapOfInteger& aMIFIn=aFI.VerticesIn();
    //
    Standard_Boolean bLinePlane = Standard_False;
    if (aNbCPrts) {
      BRepAdaptor_Curve aBAC(aE);
      bLinePlane = (aBAC.GetType() == GeomAbs_Line &&
                    myContext->SurfaceAdaptor(aF).GetType() == GeomAbs_Plane);
    }
    //
    for (i=1; i<=aNbCPrts; ++i) {
      const IntTools_CommonPrt& aCPart=aCPrts(i);
      aType=aCPart.Type();
      switch (aType) {
        case TopAbs_VERTEX: {
          Standard_Boolean bIsOnPave[2];
          Standard_Integer j;
          Standard_Real aT, aTolToDecide; 
          TopoDS_Vertex aVnew;
          //
          IntTools_Tools::VertexParameter(aCPart, aT);
          BOPTools_AlgoTools::MakeNewVertex(aE, aT, aF, aVnew);
          //
          const IntTools_Range& aR=aCPart.Range1();
          aTolToDecide=5.e-8;
          //
          bIsOnPave[0]=IntTools_Tools::IsInRange(aR1, aR, aTolToDecide); 
          bIsOnPave[1]=IntTools_Tools::IsInRange(aR2, aR, aTolToDecide); 
          //
          if ((bIsOnPave[0] && bIsOnPave[1]) || 
              (bLinePlane && (bIsOnPave[0] || bIsOnPave[1]))) {
            bV[0]=CheckFacePaves(nV[0], aMIFOn, aMIFIn);
            bV[1]=CheckFacePaves(nV[1], aMIFOn, aMIFIn);
            if (bV[0] && bV[1]) {
              IntTools_CommonPrt aCP = aCPart;
              aCP.SetType(TopAbs_EDGE);
              BOPDS_InterfEF& aEF=aEFs.Appended();
              iX=aEFs.Length()-1;
              aEF.SetIndices(nE, nF);
              aEF.SetCommonPart(aCP);
              myDS->AddInterf(nE, nF);
              //
              aMIEFC.Add(nF);
              //           
              BOPAlgo_Tools::FillMap(aPB, nF, aMPBLI, aAllocator);
              break;
            }
          }
          //
          if (!bIsPBSplittable) {
            continue;
          }
          //
          for (j=0; j<2; ++j) {
            if (bIsOnPave[j]) {
              bV[j]=CheckFacePaves(nV[j], aMIFOn, aMIFIn);
              if (bV[j]) {
                const TopoDS_Vertex& aV=
                  (*(TopoDS_Vertex *)(&myDS->Shape(nV[j])));
                //
                Standard_Real f, l, aTolVnew, aDistPP, aTolPC, aTolV;
                //
                const Handle(Geom_Curve)& aCur = BRep_Tool::Curve(aE, f, l);
                //
                gp_Pnt aP1 = BRep_Tool::Pnt(aV);
                gp_Pnt aP2 = aCur->Value(aT);
                //
                aDistPP=aP1.Distance(aP2);
                //
                aTolPC=Precision::PConfusion();
                aTolV=BRep_Tool::Tolerance(aV);
                if (aDistPP > (aTolV+aTolPC)) {
                  aTolVnew=Max(aTolE, aDistPP);
                  UpdateVertex(nV[j], aTolVnew);
                }
              }
              else {
                bIsOnPave[j] = ForceInterfVF(nV[j], nF);
              }
            }
          }
          //
          if (!bIsOnPave[0] && !bIsOnPave[1]) {
            if (CheckFacePaves(aVnew, aMIFOn)) {
              continue;
            }
            //
            Standard_Real aTolVnew = BRep_Tool::Tolerance(aVnew);
            aTolVnew = Max(aTolVnew, Max(aTolE, aTolF));
            BRep_Builder().UpdateVertex(aVnew, aTolVnew);
            if (bLinePlane) {
              // increase tolerance for Line/Plane intersection, but do not update 
              // the vertex till its intersection with some other shape
              IntTools_Range aCR = aCPart.Range1();
              aTolVnew = Max(aTolVnew, (aCR.Last() - aCR.First()) / 2.);
            }
            //
            const gp_Pnt& aPnew = BRep_Tool::Pnt(aVnew);
            //
            if (!myContext->IsPointInFace(aPnew, aF, aTolVnew)) {
              continue;
            }
            //
            aMIEFC.Add(nF);
            // 1
            BOPDS_InterfEF& aEF=aEFs.Appended();
            iX=aEFs.Length()-1;
            aEF.SetIndices(nE, nF);
            aEF.SetCommonPart(aCPart);
            // 2
            myDS->AddInterf(nE, nF);
            // 3
            BOPDS_CoupleOfPaveBlocks aCPB;
            //
            aCPB.SetPaveBlocks(aPB, aPB);
            aCPB.SetIndexInterf(iX);
            aCPB.SetTolerance(aTolVnew);
            aMVCPB.Add(aVnew, aCPB);
          }
        }
          break;
        case TopAbs_EDGE:  {
          aMIEFC.Add(nF);
          //
          // 1
          BOPDS_InterfEF& aEF=aEFs.Appended();
          iX=aEFs.Length()-1;
          aEF.SetIndices(nE, nF);
          //
          bV[0]=CheckFacePaves(nV[0], aMIFOn, aMIFIn);
          bV[1]=CheckFacePaves(nV[1], aMIFOn, aMIFIn);
          if (!bV[0] || !bV[1]) {
            myDS->AddInterf(nE, nF);
            break;
          }
          aEF.SetCommonPart(aCPart);
          // 2
          myDS->AddInterf(nE, nF);
          // 3
          BOPAlgo_Tools::FillMap(aPB, nF, aMPBLI, aAllocator);
          
        }
          break; 
        default:
          break; 
      }//switch (aType) {
    }//for (i=1; i<=aNbCPrts; ++i) {
  }// for (k=0; k < aNbEdgeEdge; ++k) {
  // 
  //=========================================
  // post treatment
  //=========================================
  BOPAlgo_Tools::PerformCommonBlocks(aMPBLI, aAllocator, myDS);
  PerformNewVertices(aMVCPB, aAllocator, Standard_False);
  //
  // Update FaceInfoIn for all faces having EF common parts
  TColStd_MapIteratorOfMapOfInteger aItMI;
  aItMI.Initialize(aMIEFC);
  for (; aItMI.More(); aItMI.Next()) {
    nF=aItMI.Value();
    myDS->UpdateFaceInfoIn(nF);
  }
  //-----------------------------------------------------scope t
  aMIEFC.Clear();
  aMVCPB.Clear();
  aMPBLI.Clear();
  ////aAllocator.Nullify();
}
//=======================================================================
// function: CheckFacePaves
// purpose: 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::CheckFacePaves 
  (const Standard_Integer nVx,
   const TColStd_MapOfInteger& aMIFOn,
   const TColStd_MapOfInteger& aMIFIn)
{
  Standard_Boolean bRet;
  Standard_Integer nV;
  TColStd_MapIteratorOfMapOfInteger aIt;
  //
  bRet=Standard_False;
  //
  aIt.Initialize(aMIFOn);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    if (nV==nVx) {
      bRet=!bRet;
      return bRet;
    }
  }
  aIt.Initialize(aMIFIn);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    if (nV==nVx) {
      bRet=!bRet;
      return bRet;
    }
  }
  //
  return bRet;
}
//=======================================================================
// function: CheckFacePaves
// purpose: 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::CheckFacePaves 
  (const TopoDS_Vertex& aVnew,
   const TColStd_MapOfInteger& aMIF)
{
  Standard_Boolean bRet;
  Standard_Integer nV, iFlag;
  TColStd_MapIteratorOfMapOfInteger aIt;
  //
  bRet=Standard_True;
  //
  aIt.Initialize(aMIF);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nV)));
    iFlag=BOPTools_AlgoTools::ComputeVV(aVnew, aV);
    if (!iFlag) {
      return bRet;
    }
  }
  //
  return !bRet;
}
//=======================================================================
//function : ForceInterfVF
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::ForceInterfVF
  (const Standard_Integer nV, 
   const Standard_Integer nF)
{
  Standard_Boolean bRet;
  Standard_Integer iFlag, nVx;
  Standard_Real U, V, aTolVNew;
  //
  bRet = Standard_False;
  const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&myDS->Shape(nV);
  const TopoDS_Face&   aF = *(TopoDS_Face*)  &myDS->Shape(nF);
  //
  iFlag = myContext->ComputeVF(aV, aF, U, V, aTolVNew, myFuzzyValue);
  if (iFlag == 0 || iFlag == -2) {
    bRet=!bRet;
  //
    BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
    aVFs.SetIncrement(10);
    // 1
    BOPDS_InterfVF& aVF=aVFs.Appended();
    //
    aVF.SetIndices(nV, nF);
    aVF.SetUV(U, V);
    // 2
    myDS->AddInterf(nV, nF);
    //
    // 3 update vertex V/F if necessary
    nVx=UpdateVertex(nV, aTolVNew);
    // 4
    if (myDS->IsNewShape(nVx)) {
      aVF.SetIndexNew(nVx);
    }
    //
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
    TColStd_MapOfInteger& aMVIn=aFI.ChangeVerticesIn();
    aMVIn.Add(nVx);
    //
    // check for self-interference
    Standard_Integer iRV = myDS->Rank(nV);
    if (iRV >= 0 && iRV == myDS->Rank(nF)) {
      // add warning status
      TopoDS_Compound aWC;
      BRep_Builder().MakeCompound(aWC);
      BRep_Builder().Add(aWC, aV);
      BRep_Builder().Add(aWC, aF);
      AddWarning (new BOPAlgo_AlertSelfInterferingShape (aWC));
    }

  }
  return bRet;
}
//=======================================================================
//function : ReduceIntersectionRange
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::ReduceIntersectionRange(const Standard_Integer theV1,
                                                 const Standard_Integer theV2,
                                                 const Standard_Integer theE,
                                                 const Standard_Integer theF,
                                                 Standard_Real& theTS1,
                                                 Standard_Real& theTS2)
{
  if (!myDS->IsNewShape(theV1) &&
      !myDS->IsNewShape(theV2)) {
    return;
  }
  //
  if (!myDS->HasInterfShapeSubShapes(theE, theF)) {
    return;
  }
  //
  BOPDS_VectorOfInterfEE& aEEs = myDS->InterfEE();
  Standard_Integer aNbEEs = aEEs.Length();
  if (!aNbEEs) {
    return;
  }
  //
  Standard_Integer i, nV, nE1, nE2;
  Standard_Real aTR1, aTR2;
  //
  // get face's edges to check that E/E contains the edge from the face
  TColStd_MapOfInteger aMFE;
  const TColStd_ListOfInteger& aLI = myDS->ShapeInfo(theF).SubShapes();
  TColStd_ListIteratorOfListOfInteger aItLI(aLI);
  for (; aItLI.More(); aItLI.Next()) {
    nE1 = aItLI.Value();
    if (myDS->ShapeInfo(nE1).ShapeType() == TopAbs_EDGE) {
      aMFE.Add(nE1);
    }
  }
  //
  for (i = 0; i < aNbEEs; ++i) {
    BOPDS_InterfEE& aEE = aEEs(i);
    if (!aEE.HasIndexNew()) {
      continue;
    }
    //
    // check the vertex
    nV = aEE.IndexNew();
    if (nV != theV1 && nV != theV2) {
      continue;
    }
    //
    // check that the intersection is between the edge
    // and one of the face's edge
    aEE.Indices(nE1, nE2);
    if (((theE != nE1) && (theE != nE2)) ||
        (!aMFE.Contains(nE1) && !aMFE.Contains(nE2))) {
      continue;
    }
    //
    // update the intersection range
    const IntTools_CommonPrt& aCPart = aEE.CommonPart();
    const IntTools_Range& aCRange = 
      (theE == nE1) ? aCPart.Range1() : aCPart.Ranges2().First();
    aCRange.Range(aTR1, aTR2);
    //
    if (nV == theV1) {
      if (theTS1 < aTR2) {
        theTS1 = aTR2;
      }
    }
    else {
      if (theTS2 > aTR1) {
        theTS2 = aTR1;
      }
    }
  }
}
