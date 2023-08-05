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
#include <Bnd_Tools.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_EdgeFace.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_Tools.hxx>
#include <NCollection_IncAllocator.hxx>
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
  public BOPAlgo_ParallelAlgo {
 
 public:
  DEFINE_STANDARD_ALLOC
  
  BOPAlgo_EdgeFace() : 
    IntTools_EdgeFace(), 
    BOPAlgo_ParallelAlgo(),
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
  void SetBoxes (const Bnd_Box& theBox1,
                 const Bnd_Box& theBox2)
  {
    myBox1 = theBox1;
    myBox2 = theBox2;
  }
  //
  virtual void Perform() {
    Message_ProgressScope aPS(myProgressRange, NULL, 1);
    if (UserBreak(aPS))
    {
      return;
    }
    TopoDS_Face aFace = myFace;
    TopoDS_Edge anEdge = myEdge;
    Standard_Boolean hasTrsf = false;
    try
    {
      OCC_CATCH_SIGNALS

      gp_Trsf aTrsf;
      if (BOPAlgo_Tools::TrsfToPoint (myBox1, myBox2, aTrsf))
      {
        // Shapes are located far from origin, move the shapes to the origin,
        // to increase the accuracy of intersection.
        TopLoc_Location aLoc (aTrsf);
        myEdge.Move (aLoc);
        myFace.Move (aLoc);
        hasTrsf = Standard_True;
      }

      IntTools_EdgeFace::Perform();
    }
    catch (Standard_Failure const&)
    {
      AddError(new BOPAlgo_AlertIntersectionFailed);
    }
    myFace = aFace;
    myEdge = anEdge;

    if (hasTrsf)
    {
      for (Standard_Integer i = 1; i <= mySeqOfCommonPrts.Length(); ++i)
      {
        IntTools_CommonPrt& aCPart = mySeqOfCommonPrts (i);
        aCPart.SetEdge1 (myEdge);
      }
    }
  }
  //
 protected:
  Standard_Integer myIE;
  Standard_Integer myIF;
  IntTools_Range myNewSR;
  Handle(BOPDS_PaveBlock) myPB;
  Bnd_Box myBox1;
  Bnd_Box myBox2;
};
//
//=======================================================================
typedef NCollection_Vector<BOPAlgo_EdgeFace> BOPAlgo_VectorOfEdgeFace; 

//=======================================================================
//function : PerformEF
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::PerformEF(const Message_ProgressRange& theRange)
{
  FillShrunkData(TopAbs_EDGE, TopAbs_FACE);
  //
  myIterator->Initialize(TopAbs_EDGE, TopAbs_FACE);
  Message_ProgressScope aPSOuter(theRange, NULL, 10);
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
    if (UserBreak(aPSOuter))
    {
      return;
    }
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
      if (UserBreak(aPSOuter))
      {
        return;
      }
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
      aEdgeFace.SetBoxes (myDS->ShapeInfo(nE).Box(), myDS->ShapeInfo (nF).Box());
      aEdgeFace.SetFuzzyValue(myFuzzyValue);
      aEdgeFace.UseQuickCoincidenceCheck(bExpressCompute);

      IntTools_Range aSR(aTS1, aTS2);
      IntTools_Range anewSR = aSR;
      BOPTools_AlgoTools::CorrectRange(aE, aF, aSR, anewSR);
      aEdgeFace.SetNewSR(anewSR);
      //
      IntTools_Range aPBRange(aT1, aT2);
      aSR = aPBRange;
      BOPTools_AlgoTools::CorrectRange(aE, aF, aSR, aPBRange);
      aEdgeFace.SetRange(aPBRange);
      //
      // Save the pair to avoid their forced intersection
      BOPDS_MapOfPaveBlock* pMPB = myFPBDone.ChangeSeek(nF);
      if (!pMPB)
        pMPB = myFPBDone.Bound(nF, BOPDS_MapOfPaveBlock());
      pMPB->Add(aPB);
    }//for (; aIt.More(); aIt.Next()) {
  }//for (; myIterator->More(); myIterator->Next()) {
  //
  aNbEdgeFace=aVEdgeFace.Length();
  Message_ProgressScope aPS(aPSOuter.Next(9), "Performing Edge-Face intersection", aNbEdgeFace);
  for (Standard_Integer index = 0; index < aNbEdgeFace; index++)
  {
    BOPAlgo_EdgeFace& aEdgeFace = aVEdgeFace.ChangeValue(index);
    aEdgeFace.SetProgressRange(aPS.Next());
  }
  //=================================================================
  BOPTools_Parallel::Perform (myRunParallel, aVEdgeFace, myContext);
  //=================================================================
  if (UserBreak(aPSOuter))
  {
    return;
  }
  //
  for (k=0; k < aNbEdgeFace; ++k) {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    BOPAlgo_EdgeFace& aEdgeFace=aVEdgeFace(k);
    if (!aEdgeFace.IsDone() || aEdgeFace.HasErrors()) {
      // Warn about failed intersection of sub-shapes
      AddIntersectionFailedWarning(aEdgeFace.Edge(), aEdgeFace.Face());
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
    //
    const IntTools_SequenceOfCommonPrts& aCPrts=aEdgeFace.CommonParts();
    aNbCPrts = aCPrts.Length();
    if (!aNbCPrts) {
      if (aEdgeFace.MinimalDistance() < RealLast() &&
          aEdgeFace.MinimalDistance() > aTolE + aTolF)
      {
        const Handle(BOPDS_PaveBlock)& aPB=aEdgeFace.PaveBlock();
        aPB->Range(aT1, aT2);
        NCollection_List<EdgeRangeDistance>* pList = myDistances.ChangeSeek (BOPDS_Pair (nE, nF));
        if (!pList)
          pList = myDistances.Bound (BOPDS_Pair (nE, nF), NCollection_List<EdgeRangeDistance>());
        pList->Append (EdgeRangeDistance (aT1, aT2, aEdgeFace.MinimalDistance()));
      }
      continue;
    }
    //
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
      if (UserBreak(aPSOuter))
      {
        return;
      }
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
          for (j = 0; j < 2; ++j)
          {
            if (bIsOnPave[j])
            {
              bV[j] = CheckFacePaves(nV[j], aMIFOn, aMIFIn);
              if (!bV[j])
                bIsOnPave[j] = ForceInterfVF(nV[j], nF);
            }
          }

          if (bIsOnPave[0] || bIsOnPave[1])
          {
            // The found intersection point is located closely to one of the pave block's
            // bounds. So, do not create the new vertex in this point.
            // Check if this point is a real intersection, or just a touching point.
            // If it is a touching point, do nothing.
            // If it is an intersection point, update the existing vertex to cover the
            // intersection point.
            GeomAPI_ProjectPointOnSurf& aProjPS = myContext->ProjPS(aF);
            const gp_Pnt aPnew = BRep_Tool::Pnt(aVnew);
            aProjPS.Perform(aPnew);
            Standard_Real aMinDistEF = (aProjPS.IsDone() && aProjPS.NbPoints()) ?
                                        aProjPS.LowerDistance() : Precision::Infinite();
            Standard_Boolean hasRealIntersection = aMinDistEF < Precision::Intersection();

            if (!hasRealIntersection)
              // no intersection point
              continue;

            // Real intersection is present.
            // Update the existing vertex to cover the intersection point.
            for (j = 0; j < 2; ++j)
            {
              if (bIsOnPave[j])
              {
                const TopoDS_Vertex& aV = TopoDS::Vertex(myDS->Shape(nV[j]));
                const gp_Pnt aP = BRep_Tool::Pnt(aV);
                Standard_Real aDistPP = aP.Distance(aPnew);
                Standard_Real aTol = BRep_Tool::Tolerance(aV);
                Standard_Real aMaxDist = 1.e4 * aTol;
                if (aTol < .01)
                {
                  aMaxDist = Min(aMaxDist, 0.1);
                }
                if (aDistPP < aMaxDist)
                {
                  UpdateVertex(nV[j], aDistPP);
                  myVertsToAvoidExtension.Add(nV[j]);
                }
              }
            }
            continue;
          }

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
          BOPDS_InterfEF& aEF = aEFs.Appended();
          iX = aEFs.Length() - 1;
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
  BOPAlgo_Tools::PerformCommonBlocks(aMPBLI, aAllocator, myDS, myContext);
  UpdateVerticesOfCB();
  PerformNewVertices(aMVCPB, aAllocator, aPSOuter.Next(1), Standard_False);
  if (HasErrors())
  {
    return;
  }
  //
  // Update FaceInfoIn for all faces having EF common parts
  myDS->UpdateFaceInfoIn (aMIEFC);

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
  if (aMIFOn.Contains(nVx) || aMIFIn.Contains(nVx))
  {
    return true;
  }
  return false;
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

//=======================================================================
//function : ForceInterfEF
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::ForceInterfEF(const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPS(theRange, NULL, 1);
  if (!myIsPrimary)
    return;

  // Now that we have vertices increased and unified, try to find additional
  // edge/face common blocks among the pairs of edge/face.
  // Here, we are interested in common blocks only, as all real intersections
  // should have happened already. Thus, we need to check only those pairs
  // of edge/face which have the same vertices.

  // Collect all pave blocks
  BOPDS_IndexedMapOfPaveBlock aMPB;
  const Standard_Integer aNbS = myDS->NbSourceShapes();
  for (Standard_Integer nE = 0; nE < aNbS; ++nE)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(nE);
    if (aSI.ShapeType() != TopAbs_EDGE)
      // Not an edge
      continue;

    if (!aSI.HasReference())
      // Edge has no pave blocks
      continue;

    if (aSI.HasFlag())
      // Degenerated edge
      continue;

    if (UserBreak(aPS))
    {
      return;
    }
    const BOPDS_ListOfPaveBlock& aLPB = myDS->PaveBlocks(nE);
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
    for (; aItLPB.More(); aItLPB.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
      const Handle(BOPDS_PaveBlock)& aPBR = myDS->RealPaveBlock(aPB);
      aMPB.Add(aPBR);
    }
  }

  // Perform intersection of collected pave blocks with faces

  ForceInterfEF(aMPB, aPS.Next(), Standard_True);
}

//=======================================================================
//function : ForceInterfEF
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::ForceInterfEF(const BOPDS_IndexedMapOfPaveBlock& theMPB,
                                       const Message_ProgressRange& theRange,
                                       const Standard_Boolean theAddInterf)
{
  // Split progress on preparation, intersection and post-treatment stages
  Message_ProgressScope aPSOuter(theRange, NULL, 10);
  if (theMPB.IsEmpty())
    return;
  // Fill the tree with bounding boxes of the pave blocks
  BOPTools_BoxTree aBBTree;

  Handle(NCollection_IncAllocator) anAlloc = new NCollection_IncAllocator;
  BOPDS_IndexedMapOfPaveBlock aPBMap(1, anAlloc);

  Standard_Integer aNbPB = theMPB.Extent();
  for (Standard_Integer iPB = 1; iPB <= aNbPB; ++iPB)
  {
    Handle(BOPDS_PaveBlock) aPB = theMPB(iPB);
    if (!aPB->HasShrunkData() || !myDS->IsValidShrunkData(aPB))
    {
      FillShrunkData(aPB);
      if (!aPB->HasShrunkData())
        continue;
    }
    if (UserBreak(aPSOuter))
    {
      return;
    }

    Standard_Real f, l;
    Bnd_Box aPBBox;
    Standard_Boolean isSplit;
    aPB->ShrunkData(f, l, aPBBox, isSplit);

    aBBTree.Add(aPBMap.Add(aPB), Bnd_Tools::Bnd2BVH(aPBBox));
  }

  // Shake the tree
  aBBTree.Build();

  const Standard_Boolean bSICheckMode = (myArguments.Extent() == 1);

  // Find pairs of Face/PaveBlock containing the same vertices
  // and prepare those pairs for intersection.
  BOPAlgo_VectorOfEdgeFace aVEdgeFace;

  const Standard_Integer aNbS = myDS->NbSourceShapes();
  for (Standard_Integer nF = 0; nF < aNbS; ++nF)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(nF);
    if (aSI.ShapeType() != TopAbs_FACE)
      // Not a face
      continue;

    if (!aSI.HasReference())
      // Face has no face info
      continue;

    if (UserBreak(aPSOuter))
    {
      return;
    }

    const Bnd_Box& aBoxF = aSI.Box();
    BOPTools_BoxTreeSelector aSelector;
    aSelector.SetBox(Bnd_Tools::Bnd2BVH(aBoxF));
    aSelector.SetBVHSet (&aBBTree);
    if (!aSelector.Select())
      continue;

    const TopoDS_Face& aF = TopoDS::Face(aSI.Shape());
    const BOPDS_FaceInfo& aFI = myDS->FaceInfo(nF);
    // Vertices of the face
    TColStd_MapOfInteger aMVF;
    const TColStd_MapOfInteger* pMVF[] = { &aFI.VerticesOn(),
                                           &aFI.VerticesIn(),
                                           &aFI.VerticesSc() };
    for (Standard_Integer iM = 0; iM < 3; ++iM)
    {
      TColStd_MapIteratorOfMapOfInteger itM(*pMVF[iM]);
      for (; itM.More(); itM.Next())
        aMVF.Add(itM.Value());
    }

    // Pave Blocks of the face
    const BOPDS_IndexedMapOfPaveBlock* pMPBF[] = { &aFI.PaveBlocksOn(),
                                                   &aFI.PaveBlocksIn(),
                                                   &aFI.PaveBlocksSc() };
    for (Standard_Integer iM = 0; iM < 3; ++iM)
    {
      const Standard_Integer aNb = pMPBF[iM]->Extent();
      for (Standard_Integer iPB = 1; iPB <= aNb; ++iPB)
      {
        const Handle(BOPDS_PaveBlock)& aPB = pMPBF[iM]->FindKey(iPB);
        aMVF.Add(aPB->Pave1().Index());
        aMVF.Add(aPB->Pave2().Index());
      }
    }

    // Projection tool
    GeomAPI_ProjectPointOnSurf& aProjPS = myContext->ProjPS(aF);
    BRepAdaptor_Surface& aSurfAdaptor = myContext->SurfaceAdaptor (aF);

    // Iterate on pave blocks and combine pairs containing
    // the same vertices
    const TColStd_ListOfInteger& aLIPB = aSelector.Indices();
    TColStd_ListOfInteger::Iterator itLIPB(aLIPB);
    for (; itLIPB.More(); itLIPB.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPB = aPBMap(itLIPB.Value());
      if (pMPBF[0]->Contains(aPB) ||
          pMPBF[1]->Contains(aPB) ||
          pMPBF[2]->Contains(aPB))
        continue;

      // Check if the face contains both vertices of the pave block
      Standard_Integer nV1, nV2;
      aPB->Indices(nV1, nV2);
      if (!aMVF.Contains(nV1) || !aMVF.Contains(nV2))
        // Face does not contain the vertices
        continue;

      // Get the edge
      Standard_Integer nE;
      if (!aPB->HasEdge(nE))
      {
        nE = aPB->OriginalEdge();
        if (nE < 0)
          continue;

        // Make sure that the edge and face came from different arguments
        if (myDS->Rank(nF) == myDS->Rank(nE))
          continue;
      }

      const TopoDS_Edge& aE = TopoDS::Edge(myDS->Shape(nE));
      BRepAdaptor_Curve aBAC(aE);

      // Check directions coincidence at middle point on the edge
      // and projection of that point on the face.
      // If the angle between tangent vector to the curve and normal
      // of the face is not in the range of 65 - 115 degrees, do not use the additional
      // tolerance, as it may lead to undesired unification of edge with the face.
      Standard_Boolean bUseAddTol = Standard_True;

      Standard_Real aTS[2];
      Bnd_Box aPBBox;
      Standard_Boolean isSplit;
      aPB->ShrunkData(aTS[0], aTS[1], aPBBox, isSplit);

      // Middle point
      gp_Pnt aPOnE;
      // Tangent vector in the middle point
      gp_Vec aVETgt;
      aBAC.D1(BOPTools_AlgoTools2D::IntermediatePoint(aTS[0], aTS[1]), aPOnE, aVETgt);
      if (aVETgt.SquareMagnitude() < gp::Resolution())
        continue;

      aProjPS.Perform(aPOnE);
      if (!aProjPS.NbPoints())
        continue;

      // Check the distance in the middle point, using the max vertices
      // tolerance as the criteria.
      const TopoDS_Vertex& aV1 = TopoDS::Vertex(myDS->Shape(nV1));
      const TopoDS_Vertex& aV2 = TopoDS::Vertex(myDS->Shape(nV2));

      // In the Self-Interference check mode we are interested in real
      // intersections only, so use only the real tolerance of edges,
      // no need to use the extended tolerance.
      Standard_Real aTolCheck = (bSICheckMode ? myFuzzyValue :
        2 * Max(BRep_Tool::Tolerance(aV1), BRep_Tool::Tolerance(aV2)));

      if (aProjPS.LowerDistance() > aTolCheck + myFuzzyValue)
        continue;

      Standard_Real U, V;
      aProjPS.LowerDistanceParameters(U, V);
      if (!myContext->IsPointInFace(aF, gp_Pnt2d(U, V)))
        continue;

      if (aSurfAdaptor.GetType() != GeomAbs_Plane ||
          aBAC.GetType() != GeomAbs_Line)
      {
        gp_Pnt aPOnS = aProjPS.NearestPoint();
        gp_Vec aVFNorm(aPOnS, aPOnE);
        if (aVFNorm.SquareMagnitude() > gp::Resolution())
        {
          // Angle between vectors should be close to 90 degrees.
          // We allow deviation of 25 degrees.
          Standard_Real aCos = aVFNorm.Normalized().Dot (aVETgt.Normalized());
          if (Abs(aCos) > 0.4226)
            bUseAddTol = Standard_False;
        }
      }

      // Compute an addition to Fuzzy value
      Standard_Real aTolAdd = 0.0;
      if (bUseAddTol)
      {
        // Compute the distance from the bounding points of the edge
        // to the face and use the maximal of these distances as a
        // fuzzy tolerance for the intersection.
        // Use the maximal tolerance of the pave block's vertices
        // as a max criteria for the computed distance.

        for (Standard_Integer iP = 0; iP < 2; ++iP)
        {
          gp_Pnt aP = aBAC.Value(aTS[iP]);
          aProjPS.Perform(aP);
          if (aProjPS.NbPoints())
          {
            Standard_Real aDistEF = aProjPS.LowerDistance();
            if (aDistEF < aTolCheck && aDistEF > aTolAdd)
              aTolAdd = aDistEF;
          }
        }
        if (aTolAdd > 0.)
        {
          aTolAdd -= (BRep_Tool::Tolerance(aE) + BRep_Tool::Tolerance(aF));
          if (aTolAdd < 0.)
            aTolAdd = 0.;
        }
      }

      Standard_Boolean bIntersect = aTolAdd > 0;
      if (!bIntersect)
      {
        const BOPDS_MapOfPaveBlock* pMPB = myFPBDone.Seek(nF);
        bIntersect = !pMPB || !(pMPB->Contains(aPB));
      }

      if (bIntersect)
      {
        // Prepare pair for intersection
        BOPAlgo_EdgeFace& aEdgeFace = aVEdgeFace.Appended();
        aEdgeFace.SetIndices(nE, nF);
        aEdgeFace.SetPaveBlock(aPB);
        aEdgeFace.SetEdge(aE);
        aEdgeFace.SetFace(aF);
        aEdgeFace.SetBoxes (myDS->ShapeInfo(nE).Box(), myDS->ShapeInfo (nF).Box());
        aEdgeFace.SetFuzzyValue(myFuzzyValue + aTolAdd);
        aEdgeFace.UseQuickCoincidenceCheck(Standard_True);
        aEdgeFace.SetRange(IntTools_Range(aPB->Pave1().Parameter(), aPB->Pave2().Parameter()));
      }
    }
  }

  Standard_Integer aNbEFs = aVEdgeFace.Length();
  if (!aNbEFs)
  {
    return;
  }

  // close preparation step
  aPSOuter.Next(0.7);

  aPBMap.Clear();
  anAlloc->Reset(false);

  Message_ProgressScope aPS(aPSOuter.Next(9), "Checking for edges coinciding with faces", aNbEFs);
  for (Standard_Integer i = 0; i < aNbEFs; i++)
  {
    BOPAlgo_EdgeFace& aEdgeFace = aVEdgeFace.ChangeValue(i);
    aEdgeFace.SetProgressRange(aPS.Next());
  }
  // Perform intersection of the found pairs
  BOPTools_Parallel::Perform (myRunParallel, aVEdgeFace, myContext);
  if (UserBreak(aPSOuter))
  {
    return;
  }

  BOPDS_VectorOfInterfEF& aEFs = myDS->InterfEF();
  if (theAddInterf && aEFs.IsEmpty())
    aEFs.SetIncrement(10);

  // Analyze the results of intersection looking for TopAbs_EDGE
  // intersection type only.

  // Collect all pairs for common block creation
  BOPDS_IndexedDataMapOfPaveBlockListOfInteger aMPBLI(1, anAlloc);
  for (Standard_Integer i = 0; i < aNbEFs; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    BOPAlgo_EdgeFace& anEdgeFace = aVEdgeFace(i);
    if (!anEdgeFace.IsDone() || anEdgeFace.HasErrors())
    {
      // Warn about failed intersection of sub-shapes
      AddIntersectionFailedWarning(anEdgeFace.Edge(), anEdgeFace.Face());
      continue;
    }

    const IntTools_SequenceOfCommonPrts& aCParts = anEdgeFace.CommonParts();
    if (aCParts.Length() != 1)
      continue;

    const IntTools_CommonPrt& aCP = aCParts(1);
    if (aCP.Type() != TopAbs_EDGE)
      continue;

    Standard_Integer nE, nF;
    anEdgeFace.Indices(nE, nF);
    if (theAddInterf)
    {
      // Add interference
      BOPDS_InterfEF& aEF = aEFs.Appended();
      aEF.SetIndices(nE, nF);
      aEF.SetCommonPart(aCP);
      myDS->AddInterf(nE, nF);
    }

    const Handle(BOPDS_PaveBlock)& aPB = anEdgeFace.PaveBlock();
    // Update face information with new IN pave block
    myDS->ChangeFaceInfo(nF).ChangePaveBlocksIn().Add(aPB);
    if (theAddInterf)
      // Fill map for common blocks creation
      BOPAlgo_Tools::FillMap(aPB, nF, aMPBLI, anAlloc);
  }

  if (aMPBLI.Extent())
    // Create new common blocks for coinciding pairs
    BOPAlgo_Tools::PerformCommonBlocks(aMPBLI, anAlloc, myDS);
}
