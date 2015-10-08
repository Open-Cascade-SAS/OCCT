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
#include <BOPCol_DataMapOfIntegerReal.hxx>
#include <BOPCol_DataMapOfShapeInteger.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfInteger.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfPaveBlock.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_ListOfPave.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_Point.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_VectorOfCurve.hxx>
#include <BOPDS_VectorOfPoint.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Pnt.hxx>
#include <IntSurf_ListOfPntOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_EdgeFace.hxx>
#include <IntTools_FaceFace.hxx>
#include <IntTools_PntOn2Faces.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_SequenceOfPntOn2Faces.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//
static void ToleranceFF(const BRepAdaptor_Surface& aBAS1,
                        const BRepAdaptor_Surface& aBAS2,
                        Standard_Real& aTolFF);

/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class    : BOPAlgo_FaceFace
//purpose  : 
//=======================================================================
class BOPAlgo_FaceFace : 
  public IntTools_FaceFace,
  public BOPAlgo_Algo {

 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_FaceFace() : 
    IntTools_FaceFace(),  
    BOPAlgo_Algo(),
    myIF1(-1), myIF2(-1), myTolFF(1.e-7) {
  }
  //
  virtual ~BOPAlgo_FaceFace() {
  }
  //
  void SetIndices(const Standard_Integer nF1,
                  const Standard_Integer nF2) {
    myIF1=nF1;
    myIF2=nF2;
  }
  //
  void Indices(Standard_Integer& nF1,
               Standard_Integer& nF2) const {
    nF1=myIF1;
    nF2=myIF2;
  }
  //
  void SetFaces(const TopoDS_Face& aF1,
                const TopoDS_Face& aF2) {
    myF1=aF1;
    myF2=aF2;
  }
  //
  const TopoDS_Face& Face1()const {
    return myF1;
  }
  //
  const TopoDS_Face& Face2()const {
    return myF2;
  }
  //
  void SetTolFF(const Standard_Real aTolFF) {
    myTolFF=aTolFF;
  }
  //
  Standard_Real TolFF() const{
    return myTolFF;
  }
  //
  virtual void Perform() {
    BOPAlgo_Algo::UserBreak();
    IntTools_FaceFace::Perform(myF1, myF2);
  }
  //
 protected:
  Standard_Integer myIF1;
  Standard_Integer myIF2;
  Standard_Real myTolFF;
  TopoDS_Face myF1;
  TopoDS_Face myF2;
};
//
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_FaceFace> BOPAlgo_VectorOfFaceFace; 
//
typedef BOPCol_Functor 
  <BOPAlgo_FaceFace,
  BOPAlgo_VectorOfFaceFace> BOPAlgo_FaceFaceFunctor;
//
typedef BOPCol_Cnt 
  <BOPAlgo_FaceFaceFunctor,
  BOPAlgo_VectorOfFaceFace> BOPAlgo_FaceFaceCnt;
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//function : PerformFF
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::PerformFF()
{
  Standard_Integer iSize;
  Standard_Boolean bValid;
  //
  myErrorStatus=0;
  //
  myIterator->Initialize(TopAbs_FACE, TopAbs_FACE);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  Standard_Boolean bJustAdd, bApp, bCompC2D1, bCompC2D2, bIsDone;
  Standard_Boolean bToSplit, bTangentFaces;
  Standard_Integer nF1, nF2, aNbCurves, aNbPoints, i, aNbLP;
  Standard_Integer aNbFaceFace, k;
  Standard_Real aApproxTol, aTolR3D, aTolR2D, aTolFF;
  BRepAdaptor_Surface aBAS1, aBAS2;
  BOPCol_MapOfInteger aMI;
  BOPAlgo_VectorOfFaceFace aVFaceFace;
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aFFs.SetIncrement(iSize);
  //
  bApp=mySectionAttribute.Approximation();
  bCompC2D1=mySectionAttribute.PCurveOnS1();
  bCompC2D2=mySectionAttribute.PCurveOnS2();
  aApproxTol=1.e-7;
  bToSplit = Standard_False;
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nF1, nF2, bJustAdd);
    if(bJustAdd) {
      continue;
    }
    //
    const TopoDS_Face& aF1=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
    const TopoDS_Face& aF2=(*(TopoDS_Face *)(&myDS->Shape(nF2)));
    //
    aBAS1.Initialize(aF1, Standard_False);
    aBAS2.Initialize(aF2, Standard_False);
    //
    if (aBAS1.GetType() == GeomAbs_Plane && 
        aBAS2.GetType() == GeomAbs_Plane) {
      Standard_Boolean bToIntersect;
      //
      if (aMI.Add(nF1)) {
        myDS->UpdateFaceInfoOn(nF1);
        myDS->UpdateFaceInfoIn(nF1);
      }
      if (aMI.Add(nF2)) {
        myDS->UpdateFaceInfoOn(nF2);
        myDS->UpdateFaceInfoIn(nF2);
      }
      //
      bToIntersect = CheckPlanes(nF1, nF2);
      if (!bToIntersect) {
        myDS->AddInterf(nF1, nF2);
        BOPDS_InterfFF& aFF=aFFs.Append1();
        aFF.SetIndices(nF1, nF2);
        aFF.Init(0, 0);
        continue;
      }
    }
    //
    ToleranceFF(aBAS1, aBAS2, aTolFF); 
    //
    BOPAlgo_FaceFace& aFaceFace=aVFaceFace.Append1();
    //
    aFaceFace.SetIndices(nF1, nF2);
    aFaceFace.SetFaces(aF1, aF2);
    aFaceFace.SetTolFF(aTolFF);
    //
    IntSurf_ListOfPntOn2S aListOfPnts;
    GetEFPnts(nF1, nF2, aListOfPnts);
    aNbLP = aListOfPnts.Extent();
    if (aNbLP) {
      aFaceFace.SetList(aListOfPnts);
    }
    //
    aFaceFace.SetParameters(bApp, bCompC2D1, bCompC2D2, aApproxTol);
    aFaceFace.SetProgressIndicator(myProgressIndicator);
  }//for (; myIterator->More(); myIterator->Next()) {
  //
  aNbFaceFace=aVFaceFace.Extent();
  //======================================================
  BOPAlgo_FaceFaceCnt::Perform(myRunParallel, aVFaceFace);
  //======================================================
  //
  for (k=0; k < aNbFaceFace; ++k) {
    BOPAlgo_FaceFace& aFaceFace=aVFaceFace(k);
    //
    aFaceFace.Indices(nF1, nF2);
    aTolFF=aFaceFace.TolFF();
    //
    bIsDone=aFaceFace.IsDone();
    if (bIsDone) {
      aTolR3D=aFaceFace.TolReached3d();
      aTolR2D=aFaceFace.TolReached2d();
      bTangentFaces=aFaceFace.TangentFaces();
      //
      if (aTolR3D < aTolFF){
        aTolR3D=aTolFF;
      }
      if (aTolR2D < 1.e-7){
        aTolR2D=1.e-7;
      }
      //
      aFaceFace.PrepareLines3D(bToSplit);
      //
      const IntTools_SequenceOfCurves& aCvsX=aFaceFace.Lines();
      const IntTools_SequenceOfPntOn2Faces& aPntsX=aFaceFace.Points();
      //
      aNbCurves=aCvsX.Length();
      aNbPoints=aPntsX.Length();
      //
      if (aNbCurves || aNbPoints) {
        myDS->AddInterf(nF1, nF2);
      } 
      //
      BOPDS_InterfFF& aFF=aFFs.Append1();
      aFF.SetIndices(nF1, nF2);
      //
      aFF.SetTolR3D(aTolR3D);
      aFF.SetTolR2D(aTolR2D);
      aFF.SetTangentFaces(bTangentFaces);
      //
      // Curves, Points 
      aFF.Init(aNbCurves, aNbPoints);
      //
      // Curves

      // Fix bounding box expanding coefficient.
      Standard_Real aBoxExpandValue = aTolR3D;
      if (aNbCurves > 0)
      {
        // Modify geometric expanding coefficient by topology value,
        // since this bounging box used in sharing (vertex or edge).
        Standard_Real aMaxVertexTol = Max (BRep_Tool::MaxTolerance(aFaceFace.Face1(), TopAbs_VERTEX),
                                           BRep_Tool::MaxTolerance(aFaceFace.Face2(), TopAbs_VERTEX));
        aBoxExpandValue += aMaxVertexTol;
      }

      BOPDS_VectorOfCurve& aVNC=aFF.ChangeCurves();
      for (i=1; i<=aNbCurves; ++i) {
        Bnd_Box aBox;
        //
        const IntTools_Curve& aIC=aCvsX(i);
        const Handle(Geom_Curve)& aC3D= aIC.Curve();
        bValid=IntTools_Tools::CheckCurve(aC3D, aBoxExpandValue, aBox);
        if (bValid) {
          BOPDS_Curve& aNC=aVNC.Append1();
          aNC.SetCurve(aIC);
          aNC.SetBox(aBox);
        }
      }
      //
      // Points
      BOPDS_VectorOfPoint& aVNP=aFF.ChangePoints();
      for (i=1; i<=aNbPoints; ++i) {
        const IntTools_PntOn2Faces& aPi=aPntsX(i);
        const gp_Pnt& aP=aPi.P1().Pnt();
        //
        BOPDS_Point& aNP=aVNP.Append1();
        aNP.SetPnt(aP);
      }
    //}// if (aNbCs || aNbPs)
    }// if (bIsDone) {
    else {// 904/L1
      BOPDS_InterfFF& aFF=aFFs.Append1();
      aFF.SetIndices(nF1, nF2);
      aNbCurves=0;
      aNbPoints=0;
      aFF.Init(aNbCurves, aNbPoints);
    }
  }// for (k=0; k < aNbFaceFace; ++k) {
}
//=======================================================================
//function : MakeBlocks
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::MakeBlocks()
{
  Standard_Integer aNbFF;
  //
  myErrorStatus=0;
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aNbFF=aFFs.Extent();
  if (!aNbFF) {
    return;
  }
  //
  Standard_Boolean bExist, bValid2D;
  Standard_Integer i, nF1, nF2, aNbC, aNbP, j;
  Standard_Integer nV1, nV2;
  Standard_Real aTolR3D, aT1, aT2, aTol;
  Handle(NCollection_BaseAllocator) aAllocator;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  TopoDS_Edge aES;
  Handle(BOPDS_PaveBlock) aPBOut;
  //
  //-----------------------------------------------------scope f
  aAllocator=
    NCollection_BaseAllocator::CommonBaseAllocator();
  //
  BOPCol_ListOfInteger aLSE(aAllocator), aLBV(aAllocator);
  BOPCol_MapOfInteger aMVOnIn(100, aAllocator), aMF(100, aAllocator),
                      aMVStick(100,aAllocator), aMVEF(100, aAllocator),
                      aMI(100, aAllocator);
  BOPDS_IndexedMapOfPaveBlock aMPBOnIn(100, aAllocator);
  BOPDS_MapOfPaveBlock aMPBAdd(100, aAllocator);
  BOPDS_ListOfPaveBlock aLPB(aAllocator);
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks aMSCPB(100, aAllocator); 
  BOPCol_DataMapOfShapeInteger aMVI(100, aAllocator);
  BOPDS_DataMapOfPaveBlockListOfPaveBlock aDMExEdges(100, aAllocator);
  BOPCol_DataMapOfIntegerReal aMVTol(100, aAllocator);
  BOPCol_DataMapOfIntegerInteger aDMI(100, aAllocator);
  BOPCol_DataMapOfIntegerListOfInteger aDMBV(100, aAllocator);
  BOPCol_DataMapIteratorOfDataMapOfIntegerReal aItMV;
  //
  for (i=0; i<aNbFF; ++i) {
    //
    UserBreak();
    //
    BOPDS_InterfFF& aFF=aFFs(i);
    aFF.Indices(nF1, nF2);
    //
    BOPDS_VectorOfPoint& aVP=aFF.ChangePoints();
    aNbP=aVP.Extent();
    BOPDS_VectorOfCurve& aVC=aFF.ChangeCurves();
    aNbC=aVC.Extent();
    if (!aNbP && !aNbC) {
      continue;
    }
    //
    const TopoDS_Face& aF1=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
    const TopoDS_Face& aF2=(*(TopoDS_Face *)(&myDS->Shape(nF2)));
    //
    aTolR3D=aFF.TolR3D();
    //
    // Update face info
    if (aMF.Add(nF1)) {
      myDS->UpdateFaceInfoOn(nF1);
      myDS->UpdateFaceInfoIn(nF1);
    }
    if (aMF.Add(nF2)) {
      myDS->UpdateFaceInfoOn(nF2);
      myDS->UpdateFaceInfoIn(nF2);
    }
    //
    BOPDS_FaceInfo& aFI1=myDS->ChangeFaceInfo(nF1);
    BOPDS_FaceInfo& aFI2=myDS->ChangeFaceInfo(nF2);
    //
    aMVOnIn.Clear();
    aMPBOnIn.Clear();
    aDMBV.Clear();
    aMVTol.Clear();
    //
    myDS->VerticesOnIn(nF1, nF2, aMVOnIn, aMPBOnIn);
    myDS->SharedEdges(nF1, nF2, aLSE, aAllocator);
    
    // 1. Treat Points
    for (j=0; j<aNbP; ++j) {
      TopoDS_Vertex aV;
      BOPDS_CoupleOfPaveBlocks aCPB;
      //
      BOPDS_Point& aNP=aVP.ChangeValue(j);
      const gp_Pnt& aP=aNP.Pnt();
      //
      bExist=IsExistingVertex(aP, aTolR3D, aMVOnIn);
      if (!bExist) {
        BOPTools_AlgoTools::MakeNewVertex(aP, aTolR3D, aV);
        //
        aCPB.SetIndexInterf(i);
        aCPB.SetIndex(j);
        aMSCPB.Add(aV, aCPB);
      }
    }
    //
    // 2. Treat Curves
    aMVStick.Clear();
    aMVEF.Clear();
    GetStickVertices(nF1, nF2, aMVStick, aMVEF, aMI);
    //
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVC.ChangeValue(j);
      const IntTools_Curve& aIC=aNC.Curve();
      // DEBt
      aNC.InitPaveBlock1();
      //
      PutPavesOnCurve(aMVOnIn, aTolR3D, aNC, nF1, nF2, aMI, aMVEF, aMVTol);
      //
      PutStickPavesOnCurve(aF1, aF2, aMI, aNC, aMVStick, aMVTol);
      //904/F7
      if (aNbC == 1) {
        PutEFPavesOnCurve(aNC, aMI, aMVEF, aMVTol);
      }
      //
      if (aIC.HasBounds()) {
        aLBV.Clear();
        //
        PutBoundPaveOnCurve(aF1, aF2, aTolR3D, aNC, aLBV);
        //
        if (!aLBV.IsEmpty()) {
          aDMBV.Bind(j, aLBV);
        }
      }
    }//for (j=0; j<aNbC; ++j) {
    //
    // Put closing pave if needed
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVC.ChangeValue(j);
      PutClosingPaveOnCurve (aNC);
    }
    //
    // 3. Make section edges
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVC.ChangeValue(j);
      const IntTools_Curve& aIC=aNC.Curve();
      //
      BOPDS_ListOfPaveBlock& aLPBC=aNC.ChangePaveBlocks();
      Handle(BOPDS_PaveBlock)& aPB1=aNC.ChangePaveBlock1();
      //
      aLPB.Clear();
      aPB1->Update(aLPB, Standard_False);
      //
      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next()) {
        Handle(BOPDS_PaveBlock)& aPB=aItLPB.ChangeValue();
        aPB->Indices(nV1, nV2);
        aPB->Range  (aT1, aT2);
        //
        if (fabs(aT1 - aT2) < Precision::PConfusion()) {
          continue;
        }
        //
        bValid2D=myContext->IsValidBlockForFaces(aT1, aT2, aIC, 
                                                 aF1, aF2, aTolR3D);
        if (!bValid2D) {
          continue;
        }
        //
        bExist=IsExistingPaveBlock(aPB, aNC, aTolR3D, aLSE);
        if (bExist) {
          continue;
        }
        //
        bExist=IsExistingPaveBlock(aPB, aNC, aTolR3D, aMPBOnIn, aPBOut);
        if (bExist) {
          if (aMPBAdd.Add(aPBOut)) {
            Standard_Boolean bInBothFaces = Standard_True;
            if (!myDS->IsCommonBlock(aPBOut)) {
              Standard_Integer nE;
              Standard_Real aTolE;
              //
              nE = aPBOut->Edge();
              const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(nE);
              aTolE = BRep_Tool::Tolerance(aE);
              if (aTolR3D > aTolE) {
                myDS->UpdateEdgeTolerance(nE, aTolR3D);
              }
              bInBothFaces = Standard_False;
            } else {
              bInBothFaces = (aFI1.PaveBlocksOn().Contains(aPBOut) ||
                              aFI1.PaveBlocksIn().Contains(aPBOut))&&
                             (aFI2.PaveBlocksOn().Contains(aPBOut) ||
                              aFI2.PaveBlocksIn().Contains(aPBOut));
            }
            if (!bInBothFaces) {
              PreparePostTreatFF(i, j, aPBOut, aMSCPB, aMVI, aLPBC);
            }
          }
          continue;
        }
        //
        // Make Edge
        const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
        const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
        //
        BOPTools_AlgoTools::MakeEdge (aIC, aV1, aT1, 
                                      aV2, aT2, aTolR3D, aES);
        BOPTools_AlgoTools::MakePCurve(aES, aF1, aF2, aIC, 
                                       mySectionAttribute.PCurveOnS1(),
                                       mySectionAttribute.PCurveOnS2());
        //
        if (BOPTools_AlgoTools::IsMicroEdge(aES, myContext)) {
          continue;
        }
        //
        // Append the Pave Block to the Curve j
        aLPBC.Append(aPB);
        //
        // Keep info for post treatment 
        BOPDS_CoupleOfPaveBlocks aCPB;
        aCPB.SetIndexInterf(i);
        aCPB.SetIndex(j);
        aCPB.SetPaveBlock1(aPB);
        //
        aMSCPB.Add(aES, aCPB);
        aMVI.Bind(aV1, nV1);
        aMVI.Bind(aV2, nV2);
        //
        aMVTol.UnBind(nV1);
        aMVTol.UnBind(nV2);
      }
      //
      aLPBC.RemoveFirst();
    }//for (j=0; j<aNbC; ++j) {
    //back to previous tolerance values for unused vertices
    aItMV.Initialize(aMVTol);
    for (; aItMV.More(); aItMV.Next()) {
      nV1 = aItMV.Key();
      aTol = aItMV.Value();
      //
      const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&myDS->Shape(nV1);
      const Handle(BRep_TVertex)& TV = 
        *((Handle(BRep_TVertex)*)&aV.TShape());
      TV->Tolerance(aTol);
    }
    //
    ProcessExistingPaveBlocks(i, aMPBOnIn, aDMBV, aMSCPB, aMVI, aMPBAdd);
  }//for (i=0; i<aNbFF; ++i) {
  // 
  // post treatment
  myErrorStatus=PostTreatFF(aMSCPB, aMVI, aDMExEdges, aDMI, aAllocator);
  if (myErrorStatus) {
    return;
  }
  //
  // update face info
  UpdateFaceInfo(aDMExEdges, aDMI);
  //Update all pave blocks
  UpdatePaveBlocks(aDMI);
  //-----------------------------------------------------scope t
  aMF.Clear();
  aMVStick.Clear();
  aMPBOnIn.Clear();
  aMVOnIn.Clear();
  aDMExEdges.Clear();
  aMI.Clear();
  aDMI.Clear();
}

//=======================================================================
//function : PostTreatFF
//purpose  : 
//=======================================================================
Standard_Integer BOPAlgo_PaveFiller::PostTreatFF
    (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
     BOPCol_DataMapOfShapeInteger& aMVI,
     BOPDS_DataMapOfPaveBlockListOfPaveBlock& aDMExEdges,
     BOPCol_DataMapOfIntegerInteger& aDMI,
     const Handle(NCollection_BaseAllocator)& theAllocator)
{
  Standard_Integer iRet, aNbS;
  //
  iRet=0;
  aNbS=theMSCPB.Extent();
  if (!aNbS) {
    return iRet;
  }
  //
  Standard_Boolean bHasPaveBlocks, bOld;
  Standard_Integer iErr, nSx, nVSD, iX, iP, iC, j, nV, iV = 0, iE, k;
  Standard_Integer jx, aNbLPBx;
  Standard_Real aT;
  TopAbs_ShapeEnum aType;
  TopoDS_Shape aV, aE;
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  BOPDS_PDS aPDS;
  Handle(BOPDS_PaveBlock) aPB1;
  BOPDS_Pave aPave[2], aPave1[2];
  BOPDS_ShapeInfo aSI;
  //
  BOPCol_ListOfShape aLS(theAllocator);
  BOPAlgo_PaveFiller aPF(theAllocator);
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  //
  // 0
  if (aNbS==1) {
    const TopoDS_Shape& aS=theMSCPB.FindKey(1);
    const BOPDS_CoupleOfPaveBlocks &aCPB=theMSCPB.FindFromIndex(1);
    
    //
    aType=aS.ShapeType();
    if (aType==TopAbs_VERTEX) {
      aSI.SetShapeType(aType);
      aSI.SetShape(aS);
      iV=myDS->Append(aSI);
      //
      iX=aCPB.IndexInterf();
      iP=aCPB.Index();
      BOPDS_InterfFF& aFF=aFFs(iX); 
      BOPDS_VectorOfPoint& aVNP=aFF.ChangePoints();
      BOPDS_Point& aNP=aVNP(iP);
      aNP.SetIndex(iV);
    }
    else if (aType==TopAbs_EDGE) {
      aPB1=aCPB.PaveBlock1();
      //
      if (aPB1->HasEdge()) {
        BOPDS_ListOfPaveBlock aLPBx;
        aLPBx.Append(aPB1);
        aDMExEdges.Bind(aPB1, aLPBx);
      } else {
        aSI.SetShapeType(aType);
        aSI.SetShape(aS);
        iE=myDS->Append(aSI);
        //
        aPB1->SetEdge(iE);
      }
    }
    return iRet;
  }
  //
  // 1 prepare arguments
  for (k=1; k<=aNbS; ++k) {
    const TopoDS_Shape& aS=theMSCPB.FindKey(k);
    aLS.Append(aS);
  }
  //
  // 2 Fuse shapes
  aPF.SetProgressIndicator(myProgressIndicator);
  aPF.SetRunParallel(myRunParallel);
  aPF.SetArguments(aLS);
  aPF.Perform();
  iErr=aPF.ErrorStatus();
  if (iErr) {
    return iRet;
  }
  aPDS=aPF.PDS();
  //
  aItLS.Initialize(aLS);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aSx=aItLS.Value();
    nSx=aPDS->Index(aSx);
    const BOPDS_ShapeInfo& aSIx=aPDS->ShapeInfo(nSx);
    //
    aType=aSIx.ShapeType();
    //
    if (aType==TopAbs_VERTEX) {
      if (aPDS->HasShapeSD(nSx, nVSD)) {
        aV=aPDS->Shape(nVSD);
      }
      else {
        aV=aSx;
      }
      // index of new vertex in theDS -> iV
      if (!aMVI.IsBound(aV)) {
        aSI.SetShapeType(aType);
        aSI.SetShape(aV);
        iV=myDS->Append(aSI);
        //
        aMVI.Bind(aV, iV);
      }
      else {
        iV=aMVI.Find(aV);
      }
      // update FF interference
      const BOPDS_CoupleOfPaveBlocks &aCPB=theMSCPB.FindFromKey(aSx);
      iX=aCPB.IndexInterf();
      iP=aCPB.Index();
      BOPDS_InterfFF& aFF=aFFs(iX);
      BOPDS_VectorOfPoint& aVNP=aFF.ChangePoints();
      BOPDS_Point& aNP=aVNP(iP);
      aNP.SetIndex(iV);
    }//if (aType==TopAbs_VERTEX) {
    //
    else if (aType==TopAbs_EDGE) {
      bHasPaveBlocks=aPDS->HasPaveBlocks(nSx);
      const BOPDS_CoupleOfPaveBlocks &aCPB=theMSCPB.FindFromKey(aSx);
      iX=aCPB.IndexInterf();
      iC=aCPB.Index();
      aPB1=aCPB.PaveBlock1();
      //
      bOld = aPB1->HasEdge();
      if (bOld) {
        BOPDS_ListOfPaveBlock aLPBx;
        aDMExEdges.Bind(aPB1, aLPBx);
      }
      //
      if (!bHasPaveBlocks) {
        if (bOld) {
          aDMExEdges.ChangeFind(aPB1).Append(aPB1);
        }
        else {
          aSI.SetShapeType(aType);
          aSI.SetShape(aSx);
          iE=myDS->Append(aSI);
          //
          aPB1->SetEdge(iE);
        }
      }
      else {
        BOPDS_InterfFF& aFF=aFFs(iX);
        BOPDS_VectorOfCurve& aVNC=aFF.ChangeCurves();
        BOPDS_Curve& aNC=aVNC(iC);
        BOPDS_ListOfPaveBlock& aLPBC=aNC.ChangePaveBlocks();
        //
        const BOPDS_ListOfPaveBlock& aLPBx=aPDS->PaveBlocks(nSx);
        aNbLPBx=aLPBx.Extent();
        //
        if (bOld && !aNbLPBx) {
          aDMExEdges.ChangeFind(aPB1).Append(aPB1);
          continue;
        }
        //
        if (!bOld) {
          aItLPB.Initialize(aLPBC);
          for (; aItLPB.More(); aItLPB.Next()) {
            const Handle(BOPDS_PaveBlock)& aPBC=aItLPB.Value();
            if (aPBC==aPB1) {
              aLPBC.Remove(aItLPB);
              break;
            }
          } 
        }
        //
        if (!aNbLPBx) {
          aE=aSx;
          //
          if (!aMVI.IsBound(aE)) {
            aSI.SetShapeType(aType);
            aSI.SetShape(aE);
            iE=myDS->Append(aSI);
            aMVI.Bind(aE, iE);
          }
          else {
            iE=aMVI.Find(aE);
          }
          // append new PaveBlock to aLPBC
          aPB1->SetEdge(iE);
          aLPBC.Append(aPB1);
        } // if (!aNbLPBx) {
        //
        else {
          aItLPB.Initialize(aLPBx);
          if (bOld) {
            aPave1[0] = aPB1->Pave1();
            aPave1[1] = aPB1->Pave2();
          }
          for (; aItLPB.More(); aItLPB.Next()) {
            const Handle(BOPDS_PaveBlock)& aPBx=aItLPB.Value();
            const Handle(BOPDS_PaveBlock) aPBRx=aPDS->RealPaveBlock(aPBx);
            //
            // update vertices of paves
            aPave[0]=aPBx->Pave1();
            aPave[1]=aPBx->Pave2();
            for (j=0; j<2; ++j) {
              jx = 0;
              if (bOld) {
                aT = aPave[j].Parameter();
                if (aT == aPave1[0].Parameter()) {
                  jx = 1;
                } else if (aT == aPave1[1].Parameter()) {
                  jx = 2;
                }
                //
                if (jx) {
                  iV = aPave1[jx-1].Index();
                }
              } 
              if (!jx) {
                nV=aPave[j].Index();
                aV=aPDS->Shape(nV);
                //
                if (!aMVI.IsBound(aV)) {
                  // index of new vertex in theDS -> iV
                  aSI.SetShapeType(TopAbs_VERTEX);
                  aSI.SetShape(aV);
                  iV=myDS->Append(aSI);
                  aMVI.Bind(aV, iV);
                }
                else {
                  iV=aMVI.Find(aV);
                }
              }
              const BOPDS_Pave& aP1 = !j ? aPB1->Pave1() : aPB1->Pave2();
              if (aP1.Parameter() == aPave[j].Parameter() && 
                  aP1.Index() != iV) {
                aDMI.Bind(aP1.Index(), iV);
                myDS->AddShapeSD(aP1.Index(), iV);
              }
              //
              aPave[j].SetIndex(iV);
            }
            //
            // add edge
            aE=aPDS->Shape(aPBRx->Edge());
            //
            if (!aMVI.IsBound(aE)) {
              aSI.SetShapeType(aType);
              aSI.SetShape(aE);
              iE=myDS->Append(aSI);
              aMVI.Bind(aE, iE);
            }
            else {
              iE=aMVI.Find(aE);
            }
            // append new PaveBlock to aLPBC
            Handle(BOPDS_PaveBlock) aPBC=new BOPDS_PaveBlock();
            //
            aPBC->SetPave1(aPave[0]);
            aPBC->SetPave2(aPave[1]);
            aPBC->SetEdge(iE);
            if (bOld) {
              aPBC->SetOriginalEdge(aPB1->OriginalEdge());
              aDMExEdges.ChangeFind(aPB1).Append(aPBC);
            }
            else {
              aLPBC.Append(aPBC);
            }
          }
        }
      }
    }//else if (aType==TopAbs_EDGE)
  }//for (; aItLS.More(); aItLS.Next()) {
  return iRet;
}

//=======================================================================
//function : UpdateFaceInfo
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::UpdateFaceInfo
  (BOPDS_DataMapOfPaveBlockListOfPaveBlock& theDME,
   const BOPCol_DataMapOfIntegerInteger& theDMV)
{
  Standard_Integer i, j, nV1, nF1, nF2, 
                   aNbFF, aNbC, aNbP;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  BOPCol_MapOfInteger aMF;
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aNbFF=aFFs.Extent();
  //
  //1. Sections (curves, points);
  for (i=0; i<aNbFF; ++i) {
    BOPDS_InterfFF& aFF=aFFs(i);
    aFF.Indices(nF1, nF2);
    //
    BOPDS_FaceInfo& aFI1=myDS->ChangeFaceInfo(nF1);
    BOPDS_FaceInfo& aFI2=myDS->ChangeFaceInfo(nF2);
    //
    // 1.1. Section edges
    BOPDS_VectorOfCurve& aVNC=aFF.ChangeCurves();
    aNbC=aVNC.Extent();
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVNC(j);
      BOPDS_ListOfPaveBlock& aLPBC=aNC.ChangePaveBlocks();
      //
      // Add section edges to face info
      aItLPB.Initialize(aLPBC);
      for (; aItLPB.More(); ) {
        const Handle(BOPDS_PaveBlock)& aPB=aItLPB.Value();
        //
        // Treat existing pave blocks
        if (theDME.IsBound(aPB)) {
          BOPDS_ListOfPaveBlock& aLPB=theDME.ChangeFind(aPB);
          UpdateExistingPaveBlocks(aPB, aLPB, nF1, nF2);
          aLPBC.Remove(aItLPB);
          continue;
        }
        //
        aFI1.ChangePaveBlocksSc().Add(aPB);
        aFI2.ChangePaveBlocksSc().Add(aPB);
        aItLPB.Next();
      }
    }
    //
    // 1.2. Section vertices
    const BOPDS_VectorOfPoint& aVNP=aFF.Points();
    aNbP=aVNP.Extent();
    for (j=0; j<aNbP; ++j) {
      const BOPDS_Point& aNP=aVNP(j);
      nV1=aNP.Index();
      if (nV1<0) {
        continue;
      }
      aFI1.ChangeVerticesSc().Add(nV1);
      aFI2.ChangeVerticesSc().Add(nV1);
    }
    //
    aMF.Add(nF1);
    aMF.Add(nF2);
  }
  //
  Standard_Boolean bVerts, bEdges;
  //
  bVerts = theDMV.Extent() > 0;
  bEdges = theDME.Extent() > 0;
  //
  if (!bVerts && !bEdges) {
    return;
  }
  //
  // 2. Update Face Info information with new vertices and new
  //    pave blocks created in PostTreatFF from existing ones
  Standard_Integer nV2, aNbPB;
  BOPCol_MapIteratorOfMapOfInteger aItMF;
  BOPCol_DataMapIteratorOfDataMapOfIntegerInteger aItMV;
  //
  aItMF.Initialize(aMF);
  for (; aItMF.More(); aItMF.Next()) {
    nF1 = aItMF.Value();
    //
    BOPDS_FaceInfo& aFI = myDS->ChangeFaceInfo(nF1);
    //
    // 2.1. Update information about vertices
    if (bVerts) {
      BOPCol_MapOfInteger& aMVOn = aFI.ChangeVerticesOn();
      BOPCol_MapOfInteger& aMVIn = aFI.ChangeVerticesIn();
      //
      aItMV.Initialize(theDMV);
      for (; aItMV.More(); aItMV.Next()) {
        nV1 = aItMV.Key();
        nV2 = aItMV.Value();
        //
        if (aMVOn.Remove(nV1)) {
          aMVOn.Add(nV2);
        }
        //
        if (aMVIn.Remove(nV1)) {
          aMVIn.Add(nV2);
        }
      } // for (; aItMV.More(); aItMV.Next()) {
    } // if (bVerts) {
    //
    // 2.2. Update information about pave blocks
    if (bEdges) {
      BOPDS_IndexedMapOfPaveBlock& aMPBOn = aFI.ChangePaveBlocksOn();
      BOPDS_IndexedMapOfPaveBlock& aMPBIn = aFI.ChangePaveBlocksIn();
      //
      BOPDS_IndexedMapOfPaveBlock aMPBCopy;
      for (i = 0; i < 2; ++i) {
        BOPDS_IndexedMapOfPaveBlock& aMPBOnIn = !i ? aMPBOn : aMPBIn;
        aMPBCopy = aMPBOnIn;
        aMPBOnIn.Clear();
        //
        aNbPB = aMPBCopy.Extent();
        for (j = 1; j <= aNbPB; ++j) {
          const Handle(BOPDS_PaveBlock)& aPB = aMPBCopy(j);
          if (theDME.IsBound(aPB)) {
            const BOPDS_ListOfPaveBlock& aLPB = theDME.Find(aPB);
            if (aLPB.IsEmpty()) {
              aMPBOnIn.Add(aPB);
              continue;
            }
            //
            aItLPB.Initialize(aLPB);
            for (; aItLPB.More(); aItLPB.Next()) {
              const Handle(BOPDS_PaveBlock)& aPB1 = aItLPB.Value();
              aMPBOnIn.Add(aPB1);
            }
          }
          else {
            aMPBOnIn.Add(aPB);
          }
        } // for (j = 1; j <= aNbPB; ++j) {
      } // for (i = 0; i < 2; ++i) {
    } // if (bEdges) {
  }
}
//=======================================================================
//function : IsExistingVertex
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_PaveFiller::IsExistingVertex
    (const gp_Pnt& aP,
     const Standard_Real theTolR3D,
     const BOPCol_MapOfInteger& aMVOnIn)const
{
  Standard_Boolean bRet;
  Standard_Integer nV, iFlag;
  Standard_Real aTolV;
  gp_Pnt aPV;
  Bnd_Box aBoxP;
  BOPCol_MapIteratorOfMapOfInteger aIt;
  //
  bRet=Standard_True;
  //
  aBoxP.Add(aP);
  aBoxP.Enlarge(theTolR3D);
  //
  aIt.Initialize(aMVOnIn);
  for (; aIt.More(); aIt.Next()) {
    Bnd_Box aBoxV;
    //
    nV=aIt.Value();
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nV)));
    aPV=BRep_Tool::Pnt(aV);
    aTolV=BRep_Tool::Tolerance(aV);
    aBoxV.Add(aP);
    aBoxV.Enlarge(aTolV);
    //
    if (!aBoxP.IsOut(aBoxV)) {
      iFlag=BOPTools_AlgoTools::ComputeVV(aV, aP, theTolR3D);
      if (!iFlag) {
        return bRet;
      }
    }
  }
  return !bRet;
}
//=======================================================================
//function : IsExistingPaveBlock
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_PaveFiller::IsExistingPaveBlock
    (const Handle(BOPDS_PaveBlock)& thePB,
     const BOPDS_Curve& theNC,
     const Standard_Real theTolR3D,
     const BOPCol_ListOfInteger& theLSE)
{
  Standard_Boolean bRet=Standard_True;
  //
  if (theLSE.IsEmpty()) {
    return !bRet;
  } 
  //
  Standard_Real aT1, aT2, aTm, aTx, aTol;
  Standard_Integer nE, iFlag;
  gp_Pnt aPm;
  Bnd_Box aBoxPm;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  //
  thePB->Range(aT1, aT2);
  aTm=IntTools_Tools::IntermediatePoint (aT1, aT2);
  theNC.Curve().D0(aTm, aPm);
  aBoxPm.Add(aPm);
  aBoxPm.Enlarge(theTolR3D);
  //
  aItLI.Initialize(theLSE);
  for (; aItLI.More(); aItLI.Next()) {
    nE=aItLI.Value();
    const BOPDS_ShapeInfo& aSIE=myDS->ChangeShapeInfo(nE);
    const Bnd_Box& aBoxE=aSIE.Box();
    if (!aBoxE.IsOut(aBoxPm)) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aSIE.Shape()));
      aTol = BRep_Tool::Tolerance(aE);
      aTol = aTol > theTolR3D ? aTol : theTolR3D;
      iFlag=myContext->ComputePE(aPm, aTol, aE, aTx);
      if (!iFlag) {
        return bRet;
      }
    }
  }
  return !bRet;
}

//=======================================================================
//function : IsExistingPaveBlock
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_PaveFiller::IsExistingPaveBlock
    (const Handle(BOPDS_PaveBlock)& thePB,
     const BOPDS_Curve& theNC,
     const Standard_Real theTolR3D,
     const BOPDS_IndexedMapOfPaveBlock& theMPBOnIn,
     Handle(BOPDS_PaveBlock)& aPBOut)
{
  Standard_Boolean bRet;
  Standard_Real aT1, aT2, aTm, aTx;
  Standard_Integer nSp, iFlag1, iFlag2, nV11, nV12, nV21, nV22, i, aNbPB;
  gp_Pnt aP1, aPm, aP2;
  Bnd_Box aBoxP1, aBoxPm, aBoxP2;
  //
  bRet=Standard_False;
  const IntTools_Curve& aIC=theNC.Curve();
  //
  thePB->Range(aT1, aT2);
  thePB->Indices(nV11, nV12);
  //first point
  aIC.D0(aT1, aP1);
  aBoxP1.Add(aP1);
  aBoxP1.Enlarge(theTolR3D);
  //intermediate point
  aTm=IntTools_Tools::IntermediatePoint (aT1, aT2);
  aIC.D0(aTm, aPm);
  aBoxPm.Add(aPm);
  aBoxPm.Enlarge(theTolR3D);
  //last point
  aIC.D0(aT2, aP2);
  aBoxP2.Add(aP2);
  aBoxP2.Enlarge(theTolR3D);
  //
  aNbPB = theMPBOnIn.Extent();
  for (i = 1; i <= aNbPB; ++i) {
    const Handle(BOPDS_PaveBlock)& aPB = theMPBOnIn(i);
    aPB->Indices(nV21, nV22);
    nSp=aPB->Edge();
    const BOPDS_ShapeInfo& aSISp=myDS->ChangeShapeInfo(nSp);
    const TopoDS_Edge& aSp=(*(TopoDS_Edge *)(&aSISp.Shape()));
    const Bnd_Box& aBoxSp=aSISp.Box();
    //
    iFlag1 = (nV11 == nV21 || nV11 == nV22) ? 2 : 
      (!aBoxSp.IsOut(aBoxP1) ? 1 : 0);
    iFlag2 = (nV12 == nV21 || nV12 == nV22) ? 2 : 
      (!aBoxSp.IsOut(aBoxP2) ? 1 : 0);
    if (iFlag1 && iFlag2) {
      if (aBoxSp.IsOut(aBoxPm) || myContext->ComputePE(aPm, 
                                                       theTolR3D, 
                                                       aSp, 
                                                       aTx)) {
        continue;
      }
      //
      if (iFlag1 == 1) {
        iFlag1 = !myContext->ComputePE(aP1, theTolR3D, aSp, aTx);
      }
      //
      if (iFlag2 == 1) {
        iFlag2 = !myContext->ComputePE(aP2, theTolR3D, aSp, aTx);
      }
      //
      if (iFlag1 && iFlag2) {
        aPBOut = aPB;
        bRet=Standard_True;
        break;
      }
    }
  }
  return bRet;
}
//=======================================================================
//function : PutBoundPaveOnCurve
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutBoundPaveOnCurve(const TopoDS_Face& aF1,
                                               const TopoDS_Face& aF2,
                                               const Standard_Real aTolR3D,
                                               BOPDS_Curve& aNC,
                                               BOPCol_ListOfInteger& aLVB)
{
  Standard_Boolean bVF;
  Standard_Integer nV, iFlag, nVn, j, aNbEP;
  Standard_Real aT[2], aTmin, aTmax, aTV, aTol, aTolVnew;
  gp_Pnt aP[2];
  TopoDS_Vertex aVn;
  BOPDS_ListIteratorOfListOfPave aItLP;
  BOPDS_Pave aPn, aPMM[2];
  //
  aTolVnew = Precision::Confusion();
  //
  const IntTools_Curve& aIC=aNC.Curve();
  aIC.Bounds(aT[0], aT[1], aP[0], aP[1]);
  //
  Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
  const BOPDS_ListOfPave& aLP=aPB->ExtPaves();
  //
  aNbEP=aLP.Extent();
  if (aNbEP) {
    aTmin=1.e10;
    aTmax=-aTmin;
    //
    aItLP.Initialize(aLP);
    for (; aItLP.More(); aItLP.Next()) {
      const BOPDS_Pave& aPv=aItLP.Value();
      aPv.Contents(nV, aTV);
      if (aTV<aTmin) {
        aPMM[0]=aPv;
        aTmin=aTV;
      }
      if (aTV>aTmax) {
        aPMM[1]=aPv;
        aTmax=aTV;
      }
    }
  }
  //
  for (j=0; j<2; ++j) {
    //if curve is closed, process only one bound
    if (j && aP[1].IsEqual(aP[0], aTolVnew)) {
      continue;
    }
    //
    iFlag=1;
    //
    if (aNbEP) {
      Bnd_Box aBoxP;
      //
      aBoxP.Set(aP[j]);
      aTol = aTolR3D+Precision::Confusion();
      aBoxP.Enlarge(aTol);
      const BOPDS_Pave& aPV=aPMM[j];
      nV=aPV.Index();
      const BOPDS_ShapeInfo& aSIV=myDS->ShapeInfo(nV);
      const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&aSIV.Shape()));
      const Bnd_Box& aBoxV=aSIV.Box();
      if (!aBoxP.IsOut(aBoxV)){
        iFlag=BOPTools_AlgoTools::ComputeVV(aV, aP[j], aTol);
      }
    }
    if (iFlag) {
      // 900/L5
      bVF=myContext->IsValidPointForFaces (aP[j], aF1, aF2, aTolR3D);
      if (!bVF) {
        continue;
      }
      //
      BOPDS_ShapeInfo aSIVn;
      //
      BOPTools_AlgoTools::MakeNewVertex(aP[j], aTolR3D, aVn);
      aSIVn.SetShapeType(TopAbs_VERTEX);
      aSIVn.SetShape(aVn);
      //
      nVn=myDS->Append(aSIVn);
      //
      aPn.SetIndex(nVn);
      aPn.SetParameter(aT[j]);
      aPB->AppendExtPave(aPn);
      //
      aVn=(*(TopoDS_Vertex *)(&myDS->Shape(nVn)));
      BOPTools_AlgoTools::UpdateVertex (aIC, aT[j], aVn);
      //
      aTolVnew = BRep_Tool::Tolerance(aVn);
      //
      BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nVn);
      Bnd_Box& aBoxDS=aSIDS.ChangeBox();
      BRepBndLib::Add(aVn, aBoxDS);
      //
      aLVB.Append(nVn);
    }
  }
}

//=======================================================================
//function : PutPavesOnCurve
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutPavesOnCurve
  (const BOPCol_MapOfInteger& aMVOnIn,
   const Standard_Real aTolR3D,
   BOPDS_Curve& aNC,
   const Standard_Integer nF1,
   const Standard_Integer nF2,
   const BOPCol_MapOfInteger& aMI,
   const BOPCol_MapOfInteger& aMVEF,
   BOPCol_DataMapOfIntegerReal& aMVTol)
{
  Standard_Boolean bInBothFaces;
  Standard_Integer nV;
  BOPCol_MapIteratorOfMapOfInteger aIt;
  //
  const Bnd_Box& aBoxC=aNC.Box();
  //
  //Put EF vertices first
  aIt.Initialize(aMVEF);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    PutPaveOnCurve(nV, aTolR3D, aNC, aMI, aMVTol, 2);
  }
  //Put all other vertices
  aIt.Initialize(aMVOnIn);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    if (aMVEF.Contains(nV)) {
      continue;
    }
    //
    const BOPDS_ShapeInfo& aSIV=myDS->ShapeInfo(nV);
    const Bnd_Box& aBoxV=aSIV.Box();
    //
    if (aBoxC.IsOut(aBoxV)){
      continue;
    }
    if (!myDS->IsNewShape(nV)) {
      const BOPDS_FaceInfo& aFI1 = myDS->FaceInfo(nF1);
      const BOPDS_FaceInfo& aFI2 = myDS->FaceInfo(nF2);
      //
      bInBothFaces = (aFI1.VerticesOn().Contains(nV) ||
                      aFI1.VerticesIn().Contains(nV))&&
                     (aFI2.VerticesOn().Contains(nV) ||
                      aFI2.VerticesIn().Contains(nV));
      if (!bInBothFaces) {
        continue;
      }
    }
    //
    PutPaveOnCurve(nV, aTolR3D, aNC, aMI, aMVTol, 1);
  }
}

//=======================================================================
//function : ExtendedTolerance
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::ExtendedTolerance
  (const Standard_Integer nV,
   const BOPCol_MapOfInteger& aMI,
   Standard_Real& aTolVExt,
   const Standard_Integer aType)
{
  Standard_Boolean bFound = Standard_False;
  if (!(myDS->IsNewShape(nV))) {
    return bFound;
  }
  //
  Standard_Integer i, k, aNbLines, aNbInt;
  Standard_Real aT11, aT12, aD1, aD2, aD;
  TopoDS_Vertex aV;
  gp_Pnt aPV, aP11, aP12;
  //
  k = 0;
  aNbInt = 2;
  if (aType == 1) {
    aNbInt = 1;
  } else if (aType == 2) {
    k = 1;
  }
  //
  aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV)));
  aPV=BRep_Tool::Pnt(aV);
  //
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  //
  for (; k<aNbInt; ++k) {
    aNbLines = !k ? aEEs.Extent() : aEFs.Extent();
    for (i = 0; i < aNbLines; ++i) {
      BOPDS_Interf *aInt = !k ? (BOPDS_Interf*) (&aEEs(i)) :
        (BOPDS_Interf*) (&aEFs(i));
      if (aInt->IndexNew() == nV) {
        if (aMI.Contains(aInt->Index1()) && 
            aMI.Contains(aInt->Index2())) {
          const IntTools_CommonPrt& aComPrt = !k ? aEEs(i).CommonPart() :
            aEFs(i).CommonPart();
          //
          const TopoDS_Edge& aE1=aComPrt.Edge1();
          aComPrt.Range1(aT11, aT12);
          BOPTools_AlgoTools::PointOnEdge(aE1, aT11, aP11);
          BOPTools_AlgoTools::PointOnEdge(aE1, aT12, aP12);
          aD1=aPV.Distance(aP11);
          aD2=aPV.Distance(aP12);
          aD=(aD1>aD2)? aD1 : aD2;
          if (aD>aTolVExt) {
            aTolVExt=aD;
          }
          return !bFound;
        }//if (aMI.Contains(aEF.Index1()) && aMI.Contains(aEF.Index2())) {
      }//if (aInt->IndexNew() == nV) {
    }//for (i = 0; i < aNbLines; ++i) {
  }//for (k=0; k<2; ++k) {
  return bFound;
}

//=======================================================================
//function : GetEFPnts
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::GetEFPnts(const Standard_Integer nF1,
                                     const Standard_Integer nF2,
                                     IntSurf_ListOfPntOn2S& aListOfPnts)
{
  Standard_Integer nE, nF, nFOpposite, aNbEFs, i;
  Standard_Real U1, U2, V1, V2, f, l;
  BOPCol_MapOfInteger aMI;
  //
  //collect indexes of all shapes from nF1 and nF2.
  GetFullShapeMap(nF1, aMI);
  GetFullShapeMap(nF2, aMI);
  //
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  aNbEFs = aEFs.Extent();
  //
  for(i = 0; i < aNbEFs; ++i) {
    const BOPDS_InterfEF& aEF = aEFs(i);
    if (aEF.HasIndexNew()) {
      aEF.Indices(nE, nFOpposite);
      if(aMI.Contains(nE) && aMI.Contains(nFOpposite)) {
        const IntTools_CommonPrt& aCP = aEF.CommonPart();
        Standard_Real aPar = aCP.VertexParameter1();
        const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&myDS->Shape(nE)));
        const TopoDS_Face& aFOpposite = 
          (*(TopoDS_Face*)(&myDS->Shape(nFOpposite)));
        //
        const Handle(Geom_Curve)& aCurve = BRep_Tool::Curve(aE, f, l);
        //
        nF = (nFOpposite == nF1) ? nF2 : nF1;
        const TopoDS_Face& aF = (*(TopoDS_Face*)(&myDS->Shape(nF)));
        Handle(Geom2d_Curve) aPCurve = 
          BRep_Tool::CurveOnSurface(aE, aF, f, l);
        //
        GeomAPI_ProjectPointOnSurf& aProj=myContext->ProjPS(aFOpposite);
        //
        gp_Pnt aPoint;
        aCurve->D0(aPar, aPoint);
        IntSurf_PntOn2S aPnt;
        if(!aPCurve.IsNull()) {
          gp_Pnt2d aP2d = aPCurve->Value(aPar);
          aProj.Perform(aPoint);
          if(aProj.IsDone()) {
            aProj.LowerDistanceParameters(U1,V1);
            if (nF == nF1) {
              aPnt.SetValue(aP2d.X(),aP2d.Y(),U1,V1);
            } else {
              aPnt.SetValue(U1,V1,aP2d.X(),aP2d.Y());
            }
            aListOfPnts.Append(aPnt);
          }
        }
        else {
          GeomAPI_ProjectPointOnSurf& aProj1 = myContext->ProjPS(aF);
          aProj1.Perform(aPoint);
          aProj.Perform(aPoint);
          if(aProj1.IsDone() && aProj.IsDone()){
            aProj1.LowerDistanceParameters(U1,V1);
            aProj.LowerDistanceParameters(U2,V2);
            if (nF == nF1) {
              aPnt.SetValue(U1,V1,U2,V2);
            } else {
              aPnt.SetValue(U2,V2,U1,V1);
            }
            aListOfPnts.Append(aPnt);
          }
        }
      }
    }
  }
}

//=======================================================================
//function : ProcessUnUsedVertices
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutEFPavesOnCurve
  (BOPDS_Curve& aNC,
   const BOPCol_MapOfInteger& aMI,
   const BOPCol_MapOfInteger& aMVEF,
   BOPCol_DataMapOfIntegerReal& aMVTol)
{
  if (!aMVEF.Extent()) {
    return;
  }
  //
  const IntTools_Curve& aIC=aNC.Curve();
  GeomAbs_CurveType aTypeC;
  aTypeC=aIC.Type();
  if (!(aTypeC==GeomAbs_BezierCurve || aTypeC==GeomAbs_BSplineCurve)) {
    return;
  }
  //
  Standard_Integer nV;
  BOPCol_MapOfInteger aMV;
  //
  aMV.Assign(aMVEF);
  RemoveUsedVertices(aNC, aMV);
  if (!aMV.Extent()) {
    return;
  }
  //
  Standard_Real aDist;
  BOPDS_Pave aPave;
  //
  const Handle(Geom_Curve)& aC3D=aIC.Curve();
  GeomAPI_ProjectPointOnCurve& aProjPT = myContext->ProjPT(aC3D);
  //
  BOPCol_MapIteratorOfMapOfInteger aItMI;
  aItMI.Initialize(aMV);
  for (; aItMI.More(); aItMI.Next()) {
    nV = aItMI.Value();
    const TopoDS_Vertex& aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV)));
    gp_Pnt aPV = BRep_Tool::Pnt(aV);
    aProjPT.Perform(aPV);
    Standard_Integer aNbPoints = aProjPT.NbPoints();
    if (aNbPoints) {
      aDist = aProjPT.LowerDistance();
      PutPaveOnCurve(nV, aDist, aNC, aMI, aMVTol);
    }
  }
}

//=======================================================================
//function : ProcessUnUsedVertices
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutStickPavesOnCurve
  (const TopoDS_Face& aF1,
   const TopoDS_Face& aF2,
   const BOPCol_MapOfInteger& aMI,
   BOPDS_Curve& aNC,
   const BOPCol_MapOfInteger& aMVStick,
   BOPCol_DataMapOfIntegerReal& aMVTol)
{
  BOPCol_MapOfInteger aMV;
  aMV.Assign(aMVStick);
  RemoveUsedVertices(aNC, aMV);
  //
  if (!aMV.Extent()) {
    return;
  }
  //
  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  Handle(Geom_Surface) aS2=BRep_Tool::Surface(aF2);
  //
  const IntTools_Curve& aIC=aNC.Curve();
  //if (aTypeC==GeomAbs_BezierCurve || aTypeC==GeomAbs_BSplineCurve) {
  Handle(Geom2d_Curve) aC2D[2];
  //
  aC2D[0]=aIC.FirstCurve2d();
  aC2D[1]=aIC.SecondCurve2d();
  if (!aC2D[0].IsNull() && !aC2D[1].IsNull()) {
    Standard_Integer nV, m, n;
    Standard_Real aTC[2], aD, aD2, u, v, aDT2, aScPr, aDScPr;
    gp_Pnt aPC[2], aPV;
    gp_Dir aDN[2];
    gp_Pnt2d aP2D;
    BOPCol_MapIteratorOfMapOfInteger aItMI, aItMI1;
    //
    aDT2=2e-7;     // the rich criteria
    aDScPr=5.e-9;  // the creasing criteria
    aIC.Bounds(aTC[0], aTC[1], aPC[0], aPC[1]);
    //
    aItMI.Initialize(aMV);
    for (; aItMI.More(); aItMI.Next()) {
      nV = aItMI.Value();
      const TopoDS_Vertex& aV=*((TopoDS_Vertex*)&myDS->Shape(nV));
      aPV=BRep_Tool::Pnt(aV);
      //
      for (m=0; m<2; ++m) {
        aD2=aPC[m].SquareDistance(aPV);
        if (aD2>aDT2) {// no rich
          continue; 
        }
        //
        for (n=0; n<2; ++n) {
          Handle(Geom_Surface)& aS=(!n)? aS1 : aS2;
          aC2D[n]->D0(aTC[m], aP2D);
          aP2D.Coord(u, v);
          BOPTools_AlgoTools3D::GetNormalToSurface(aS, u, v, aDN[n]);
        }
        // 
        aScPr=aDN[0]*aDN[1];
        if (aScPr<0.) {
          aScPr=-aScPr;
        }
        aScPr=1.-aScPr;
        //
        if (aScPr>aDScPr) {
          continue;
        }
        //
        // The intersection curve aIC is vanishing curve (the crease)
        aD=sqrt(aD2);
        //
        PutPaveOnCurve(nV, aD, aNC, aMI, aMVTol);
      }
    }//for (jVU=1; jVU=aNbVU; ++jVU) {
  }
  //}//if (aTypeC==GeomAbs_BezierCurve || aTypeC==GeomAbs_BSplineCurve) {
  //}//if(aType1==GeomAbs_Torus  || aType2==GeomAbs_Torus) {
}

//=======================================================================
//function : GetStickVertices
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::GetStickVertices(const Standard_Integer nF1,
                                            const Standard_Integer nF2,
                                            BOPCol_MapOfInteger& aMVStick,
                                            BOPCol_MapOfInteger& aMVEF,
                                            BOPCol_MapOfInteger& aMI)
{
  Standard_Integer nS1, nS2, nVNew, aTypeInt, i;
  //
  BOPDS_VectorOfInterfVV& aVVs=myDS->InterfVV();
  BOPDS_VectorOfInterfVE& aVEs=myDS->InterfVE();
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  //
  Standard_Integer aNbLines[5] = {
    aVVs.Extent(), aVEs.Extent(), aEEs.Extent(),
    aVFs.Extent(), aEFs.Extent()
    };
  //collect indices of all shapes from nF1 and nF2.
  aMI.Clear();
  GetFullShapeMap(nF1, aMI);
  GetFullShapeMap(nF2, aMI);
  //
  //collect VV, VE, EE, VF interferences
  for (aTypeInt = 0; aTypeInt < 4; ++aTypeInt) {
    for (i = 0; i < aNbLines[aTypeInt]; ++i) {
      BOPDS_Interf* aInt = (aTypeInt==0) ? (BOPDS_Interf*)(&aVVs(i)) : 
        ((aTypeInt==1) ? (BOPDS_Interf*)(&aVEs(i)) :
         ((aTypeInt==2) ? (BOPDS_Interf*)(&aEEs(i)) : 
          (BOPDS_Interf*)(&aVFs(i))));
      if (aInt->HasIndexNew()) {
        aInt->Indices(nS1, nS2);
        if(aMI.Contains(nS1) && aMI.Contains(nS2)) {
          nVNew = aInt->IndexNew();
          aMVStick.Add(nVNew);
        }
      }
    }
  }
  //collect EF interferences
  for (i = 0; i < aNbLines[4]; ++i) {
    const BOPDS_InterfEF& aInt = aEFs(i);
    if (aInt.HasIndexNew()) {
      aInt.Indices(nS1, nS2);
      if(aMI.Contains(nS1) && aMI.Contains(nS2)) {
        nVNew = aInt.IndexNew();
        aMVStick.Add(nVNew);
        aMVEF.Add(nVNew);
      }
    }
  }
}

//=======================================================================
// function: GetFullShapeMap
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::GetFullShapeMap(const Standard_Integer nF,
                                         BOPCol_MapOfInteger& aMI)
{
  BOPCol_ListIteratorOfListOfInteger aIt;
  Standard_Integer nS;
  //
  const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(nF);
  const BOPCol_ListOfInteger& aLI = aSI.SubShapes();
  //
  aMI.Add(nF);
  aIt.Initialize(aLI);
  for (; aIt.More(); aIt.Next()) {
    nS = aIt.Value();
    aMI.Add(nS);
  }
}

//=======================================================================
// function: RemoveUsedVertices
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::RemoveUsedVertices(BOPDS_Curve& aNC,
                                            BOPCol_MapOfInteger& aMV)
{
  if (!aMV.Extent()) {
    return;
  }
  Standard_Integer nV;
  BOPDS_Pave aPave;
  BOPDS_ListIteratorOfListOfPave aItLP;
  //
  Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
  const BOPDS_ListOfPave& aLP = aPB->ExtPaves();
  aItLP.Initialize(aLP);
  for (;aItLP.More();aItLP.Next()) {
    aPave = aItLP.Value();
    nV = aPave.Index();
    aMV.Remove(nV);
  }
}

//=======================================================================
//function : PutPaveOnCurve
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutPaveOnCurve
  (const Standard_Integer nV,
   const Standard_Real aTolR3D,
   BOPDS_Curve& aNC,
   const BOPCol_MapOfInteger& aMI,
   BOPCol_DataMapOfIntegerReal& aMVTol,
   const Standard_Integer iCheckExtend)
{
  Standard_Boolean bIsVertexOnLine;
  Standard_Real aT, aTolV;
  //
  const TopoDS_Vertex& aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV)));
  Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
  const IntTools_Curve& aIC = aNC.Curve();
  //
  bIsVertexOnLine=myContext->IsVertexOnLine(aV, aIC, aTolR3D, aT);
  if (!bIsVertexOnLine && iCheckExtend) {
    aTolV = BRep_Tool::Tolerance(aV);
    //
    ExtendedTolerance(nV, aMI, aTolV, iCheckExtend);
    bIsVertexOnLine=myContext->IsVertexOnLine(aV, aTolV, aIC, aTolR3D, aT);
  }
  //
  if (bIsVertexOnLine) {
    // check if aPB contains the parameter aT
    Standard_Boolean bExist;
    Standard_Integer nVToUpdate;
    Standard_Real aPTol, aDist, aTolVNew, aTolV2, aDTol;
    TopoDS_Vertex aVToUpdate;
    gp_Pnt aP1, aP2;
    //
    aTolV2 = 0.;
    aDTol = 1.e-12;
    //
    GeomAdaptor_Curve aGAC(aIC.Curve());
    aPTol = aGAC.Resolution(aTolR3D);
    //
    bExist = aPB->ContainsParameter(aT, aPTol, nVToUpdate);
    if (bExist) {
      // use existing pave
      aP1 = BRep_Tool::Pnt(aV);
      aTolV2 = BRep_Tool::Tolerance(aV);
      aVToUpdate = (*(TopoDS_Vertex *)(&myDS->Shape(nVToUpdate)));
    }
    else {
      // add new pave
      BOPDS_Pave aPave;
      aPave.SetIndex(nV);
      aPave.SetParameter(aT);
      aPB->AppendExtPave(aPave);
      //
      aP1 = aGAC.Value(aT);
      nVToUpdate = nV;
      aVToUpdate = aV;
    }
    //
    aTolV = BRep_Tool::Tolerance(aVToUpdate);
    aP2 = BRep_Tool::Pnt(aVToUpdate);
    aDist = aP1.Distance(aP2);
    aTolVNew = aDist - aTolV2;
    //
    if (aTolVNew > aTolV) {
      BRep_Builder aBB;
      aBB.UpdateVertex(aVToUpdate, aTolVNew+aDTol);
      //
      if (!aMVTol.IsBound(nVToUpdate)) {
        aMVTol.Bind(nVToUpdate, aTolV);
      }
      // 
      BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nVToUpdate);
      Bnd_Box& aBoxDS=aSIDS.ChangeBox();
      BRepBndLib::Add(aVToUpdate, aBoxDS);
    }
  }
}

//=======================================================================
//function : ProcessOldPaveBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::ProcessExistingPaveBlocks
    (const Standard_Integer theInt,
     const BOPDS_IndexedMapOfPaveBlock& aMPBOnIn,
     const BOPCol_DataMapOfIntegerListOfInteger& aDMBV,
     BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& aMSCPB,
     BOPCol_DataMapOfShapeInteger& aMVI,
     BOPDS_MapOfPaveBlock& aMPB)
{
  if (aDMBV.IsEmpty()) {
    return;
  }
  //
  Standard_Real aT;
  Standard_Integer i, nV, nE, iC, aNbPB, iFlag;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  BOPCol_DataMapIteratorOfDataMapOfIntegerListOfInteger aItBV;
  //
  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  BOPDS_InterfFF& aFF = aFFs(theInt);
  BOPDS_VectorOfCurve& aVC = aFF.ChangeCurves();
  //
  aNbPB = aMPBOnIn.Extent();
  //
  aItBV.Initialize(aDMBV);
  for (; aItBV.More(); aItBV.Next()) {
    iC = aItBV.Key();
    const BOPCol_ListOfInteger& aLBV = aItBV.Value();
    //
    BOPDS_Curve& aNC = aVC.ChangeValue(iC);
    BOPDS_ListOfPaveBlock& aLPBC = aNC.ChangePaveBlocks();
    //
    aItLI.Initialize(aLBV);
    for (; aItLI.More(); aItLI.Next()) {
      nV = aItLI.Value();
      const BOPDS_ShapeInfo& aSIV=myDS->ShapeInfo(nV);
      const Bnd_Box& aBoxV=aSIV.Box();
      const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&aSIV.Shape();
      if (!aMVI.IsBound(aV)) {
        continue;
      }
      //
      for (i = 1; i <= aNbPB; ++i) {
        const Handle(BOPDS_PaveBlock)& aPB = aMPBOnIn(i);
        if (aPB->Pave1().Index() == nV || aPB->Pave2().Index() == nV) {
          continue;
        }
        //
        if (aMPB.Contains(aPB)) {
          continue;
        }
        //
        nE = aPB->Edge();
        const BOPDS_ShapeInfo& aSIE = myDS->ShapeInfo(nE);
        const Bnd_Box& aBoxE = aSIE.Box();
        //
        if (aBoxV.IsOut(aBoxE)) {
          continue;
        }
        //
        const TopoDS_Edge& aE = *(TopoDS_Edge*)&aSIE.Shape();
        //
        iFlag = myContext->ComputeVE(aV, aE, aT);
        if (!iFlag) {
          aMPB.Add(aPB);
          PreparePostTreatFF(theInt, iC, aPB, aMSCPB, aMVI, aLPBC);
        }
      }
    }
  }
}
//=======================================================================
//function : UpdateExistingPaveBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::UpdateExistingPaveBlocks
    (const Handle(BOPDS_PaveBlock)& aPBf,
     BOPDS_ListOfPaveBlock& aLPB,
     const Standard_Integer nF1,
     const Standard_Integer nF2) 
{
  Standard_Integer nE;
  Standard_Boolean bCB;
  Handle(BOPDS_PaveBlock) aPB, aPB1, aPB2, aPB2n;
  Handle(BOPDS_CommonBlock) aCB;
  BOPDS_ListIteratorOfListOfPaveBlock aIt, aIt1, aIt2;
  //
  // 1. Remove micro edges from aLPB
  aIt.Initialize(aLPB);
  for (; aIt.More();) {
    aPB = aIt.Value();
    const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(aPB->Edge());
    if (BOPTools_AlgoTools::IsMicroEdge(aE, myContext)) {
      aLPB.Remove(aIt);
      continue;
    }
    aIt.Next();
  }
  //
  if (!aLPB.Extent()) {
    return;
  }
  //
  BOPDS_FaceInfo& aFI1 = myDS->ChangeFaceInfo(nF1);
  BOPDS_FaceInfo& aFI2 = myDS->ChangeFaceInfo(nF2);
  //
  BOPDS_IndexedMapOfPaveBlock& aMPBOn1 = aFI1.ChangePaveBlocksOn();
  BOPDS_IndexedMapOfPaveBlock& aMPBIn1 = aFI1.ChangePaveBlocksIn();
  BOPDS_IndexedMapOfPaveBlock& aMPBOn2 = aFI2.ChangePaveBlocksOn();
  BOPDS_IndexedMapOfPaveBlock& aMPBIn2 = aFI2.ChangePaveBlocksIn();
  //
  // 2. Remove old pave blocks
  const Handle(BOPDS_CommonBlock)& aCB1 = myDS->CommonBlock(aPBf);
  bCB = !aCB1.IsNull();
  BOPDS_ListOfPaveBlock aLPB1;
  //
  if (bCB) {
    aLPB1.Assign(aCB1->PaveBlocks());
  } else {
    aLPB1.Append(aPBf);
  }
  aIt1.Initialize(aLPB1);
  for (; aIt1.More(); aIt1.Next()) {
    aPB1 = aIt1.Value();
    nE = aPB1->OriginalEdge();
    //
    BOPDS_ListOfPaveBlock& aLPB2 = myDS->ChangePaveBlocks(nE);
    aIt2.Initialize(aLPB2);
    for (; aIt2.More(); aIt2.Next()) {
      aPB2 = aIt2.Value();
      if (aPB1 == aPB2) {
        aLPB2.Remove(aIt2);
        break;
      }
    }
  }
  //
  // 3. Update pave blocks
  if (bCB) {
    //create new common blocks
    BOPDS_ListOfPaveBlock aLPBNew;
    const BOPCol_ListOfInteger& aFaces = aCB1->Faces();
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next()) {
      const Handle(BOPDS_PaveBlock)& aPBValue = aIt.Value();
      //
      aCB = new BOPDS_CommonBlock;
      aIt1.Initialize(aLPB1);
      for (; aIt1.More(); aIt1.Next()) {
        aPB2 = aIt1.Value();
        nE = aPB2->OriginalEdge();
        //
        aPB2n = new BOPDS_PaveBlock;
        aPB2n->SetPave1(aPBValue->Pave1());
        aPB2n->SetPave2(aPBValue->Pave2());
        aPB2n->SetEdge(aPBValue->Edge());
        aPB2n->SetOriginalEdge(nE);
        aCB->AddPaveBlock(aPB2n);
        myDS->SetCommonBlock(aPB2n, aCB);
        myDS->ChangePaveBlocks(nE).Append(aPB2n);
      }
      aCB->AddFaces(aFaces);
      myDS->SortPaveBlocks(aCB);
      //
      const Handle(BOPDS_PaveBlock)& aPBNew = aCB->PaveBlocks().First();
      aLPBNew.Append(aPBNew);
    }
    //
    aLPB = aLPBNew;
  } 
  else {
    nE = aPBf->OriginalEdge();
    BOPDS_ListOfPaveBlock& aLPBE = myDS->ChangePaveBlocks(nE);
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next()) {
      aPB = aIt.Value();
      aLPBE.Append(aPB);
    }
  }
  //
  Standard_Boolean bIn1, bIn2;
  //
  bIn1 = aMPBOn1.Contains(aPBf) || aMPBIn1.Contains(aPBf);
  bIn2 = aMPBOn2.Contains(aPBf) || aMPBIn2.Contains(aPBf);
  //
  if (bIn1 && bIn2) {
    return;
  }
  //
  // 4. Check new pave blocks for coincidence 
  //    with the opposite face.
  //    In case of coincidence create common blocks
  Standard_Integer nF;
  Standard_Real aTolE, aTolF;
  //
  nF = bIn1 ? nF2 : nF1;
  const TopoDS_Face& aF = *(TopoDS_Face*)&myDS->Shape(nF);
  BOPDS_IndexedMapOfPaveBlock& aMPBIn = bIn1 ? aMPBIn2 : aMPBIn1;
  aTolF = BRep_Tool::Tolerance(aF);
  //
  aIt.Initialize(aLPB);
  for (; aIt.More(); aIt.Next()) {
    Handle(BOPDS_PaveBlock)& aPBChangeValue = aIt.ChangeValue();
    const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(aPBChangeValue->Edge());
    aTolE = BRep_Tool::Tolerance(aE);
    //
    IntTools_EdgeFace anEF;
    anEF.SetEdge(aE);
    anEF.SetFace(aF);
    anEF.SetTolE(aTolE);
    anEF.SetTolF(aTolF);
    anEF.SetRange(aPBChangeValue->Pave1().Parameter(), aPBChangeValue->Pave2().Parameter());
    anEF.SetContext(myContext);
    anEF.Perform();
    //
    const IntTools_SequenceOfCommonPrts& aCPrts=anEF.CommonParts();
    if (aCPrts.Length() == 1) {
      Standard_Boolean bCoinc = (aCPrts(1).Type() == TopAbs_EDGE);
      if (bCoinc) {
        if (bCB) {
          aCB = myDS->CommonBlock(aPBChangeValue);
        } else {
          aCB = new BOPDS_CommonBlock;
          aCB->AddPaveBlock(aPBChangeValue);
          myDS->SetCommonBlock(aPBChangeValue, aCB);
        }
        aCB->AddFace(nF);
        //
        aMPBIn.Add(aPBChangeValue);
      }
    }
  }
}
//=======================================================================
// function: PutClosingPaveOnCurve
// purpose:
//=======================================================================
  void BOPAlgo_PaveFiller::PutClosingPaveOnCurve(BOPDS_Curve& aNC)
{
  Standard_Boolean bIsClosed, bHasBounds, bAdded;
  Standard_Integer nVC, j;
  Standard_Real aT[2], aTC, dT, aTx;
  gp_Pnt aP[2] ; 
  BOPDS_Pave aPVx;
  BOPDS_ListIteratorOfListOfPave aItLP;
  //
  const IntTools_Curve& aIC=aNC.Curve();
  const Handle(Geom_Curve)& aC3D=aIC.Curve();
  if(aC3D.IsNull()) {
    return;
  }
  //
  bIsClosed=IntTools_Tools::IsClosed(aC3D);
  if (!bIsClosed) {
    return;
  }
  //
  bHasBounds=aIC.HasBounds ();
  if (!bHasBounds){
    return;
  }
  // 
  bAdded=Standard_False;
  dT=Precision::PConfusion();
  aIC.Bounds (aT[0], aT[1], aP[0], aP[1]);
  //
  Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
  BOPDS_ListOfPave& aLP=aPB->ChangeExtPaves();
  //
  aItLP.Initialize(aLP);
  for (; aItLP.More() && !bAdded; aItLP.Next()) {
    const BOPDS_Pave& aPC=aItLP.Value();
    nVC=aPC.Index();
    aTC=aPC.Parameter();
    //
    for (j=0; j<2; ++j) {
      if (fabs(aTC-aT[j]) < dT) {
        aTx=(!j) ? aT[1] : aT[0];
        aPVx.SetIndex(nVC);
        aPVx.SetParameter(aTx);
        aLP.Append(aPVx);
        //
        bAdded=Standard_True;
        break;
      }
    }
  }
}
//=======================================================================
//function : PreparePostTreatFF
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PreparePostTreatFF
    (const Standard_Integer aInt,
     const Standard_Integer aCur,
     const Handle(BOPDS_PaveBlock)& aPB,
     BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& aMSCPB,
     BOPCol_DataMapOfShapeInteger& aMVI,
     BOPDS_ListOfPaveBlock& aLPBC)
{
  Standard_Integer nV1, nV2;
  //
  aLPBC.Append(aPB);
  //
  aPB->Indices(nV1, nV2);
  const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
  const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
  const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(aPB->Edge());
  // Keep info for post treatment 
  BOPDS_CoupleOfPaveBlocks aCPB;
  aCPB.SetIndexInterf(aInt);
  aCPB.SetIndex(aCur);
  aCPB.SetPaveBlock1(aPB);
  //
  aMSCPB.Add(aE, aCPB);
  aMVI.Bind(aV1, nV1);
  aMVI.Bind(aV2, nV2);
}

//=======================================================================
//function : CheckPlanes
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::CheckPlanes
  (const Standard_Integer nF1,
   const Standard_Integer nF2)const
{
  Standard_Boolean bToIntersect;
  Standard_Integer i, nV2, iCnt;
  BOPCol_MapIteratorOfMapOfInteger aIt;
  //
  bToIntersect=Standard_False;
  //
  const BOPDS_FaceInfo& aFI1=myDS->ChangeFaceInfo(nF1);
  const BOPDS_FaceInfo& aFI2=myDS->ChangeFaceInfo(nF2);
  //
  const BOPCol_MapOfInteger& aMVIn1=aFI1.VerticesIn();
  const BOPCol_MapOfInteger& aMVOn1=aFI1.VerticesOn();
  //
  iCnt=0;
  for (i=0; (i<2 && !bToIntersect); ++i) {
    const BOPCol_MapOfInteger& aMV2=(!i) ? aFI2.VerticesIn() 
      : aFI2.VerticesOn();
    //
    aIt.Initialize(aMV2);
    for (; aIt.More(); aIt.Next()) {
      nV2=aIt.Value();
      if (aMVIn1.Contains(nV2) || aMVOn1.Contains(nV2)) {
        ++iCnt;
        if (iCnt>1) {
          bToIntersect=!bToIntersect;
          break;
        }
      }
    }
  }
  //
  return bToIntersect;
}
//=======================================================================
//function : UpdatePaveBlocks
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::UpdatePaveBlocks
  (const BOPCol_DataMapOfIntegerInteger& aDMI)
{
  if (aDMI.IsEmpty()) {
    return;
  }
  //
  Standard_Integer nSp, aNbPBP, nV[2], i, j;
  Standard_Real aT[2];
  Standard_Boolean bCB, bRebuild;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  BOPDS_MapOfPaveBlock aMPB;
  //
  BOPDS_VectorOfListOfPaveBlock& aPBP=myDS->ChangePaveBlocksPool();
  aNbPBP = aPBP.Extent();
  for (i=0; i<aNbPBP; ++i) {
    BOPDS_ListOfPaveBlock& aLPB=aPBP(i);
    //
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next()) {
      Handle(BOPDS_PaveBlock) aPB=aItPB.Value();
      const Handle(BOPDS_CommonBlock)& aCB=myDS->CommonBlock(aPB);
      bCB = !aCB.IsNull();
      if (bCB) {
        aPB=aCB->PaveBlock1();
      }
      //
      if (aMPB.Add(aPB)) {
        bRebuild = Standard_False;
        aPB->Indices(nV[0], nV[1]);
        aPB->Range(aT[0], aT[1]);
        //
        for (j = 0; j < 2; ++j) {
          if (aDMI.IsBound(nV[j])) {
            BOPDS_Pave aPave;
            //
            nV[j] = aDMI.Find(nV[j]);
            aPave.SetIndex(nV[j]);
            aPave.SetParameter(aT[j]);
            //
            bRebuild = Standard_True;
            if (!j) {
              aPB->SetPave1(aPave);
            } else {
              aPB->SetPave2(aPave);
            }
          }
        }
        //
        if (bRebuild) {
          nSp = SplitEdge(aPB->OriginalEdge(), nV[0], aT[0], nV[1], aT[1]);
          if (bCB) {
            aCB->SetEdge(nSp);
          }
          else {
            aPB->SetEdge(nSp);
          }
        }// if (bRebuild) {
      }// if (aMPB.Add(aPB)) {
    }// for (; aItPB.More(); aItPB.Next()) {
  }// for (i=0; i<aNbPBP; ++i) {
  aMPB.Clear();
}
//=======================================================================
//function : ToleranceFF
//purpose  : Computes the TolFF according to the tolerance value and 
//           types of the faces.
//=======================================================================
  void ToleranceFF(const BRepAdaptor_Surface& aBAS1,
                   const BRepAdaptor_Surface& aBAS2,
                   Standard_Real& aTolFF)
{
  Standard_Real aTol1, aTol2;
  Standard_Boolean isAna1, isAna2;
  //
  aTol1 = aBAS1.Tolerance();
  aTol2 = aBAS2.Tolerance();
  aTolFF = Max(aTol1, aTol2);
  //
  isAna1 = (aBAS1.GetType() == GeomAbs_Plane ||
            aBAS1.GetType() == GeomAbs_Cylinder ||
            aBAS1.GetType() == GeomAbs_Cone ||
            aBAS1.GetType() == GeomAbs_Sphere ||
            aBAS1.GetType() == GeomAbs_Torus);
  //
  isAna2 = (aBAS2.GetType() == GeomAbs_Plane ||
            aBAS2.GetType() == GeomAbs_Cylinder ||
            aBAS2.GetType() == GeomAbs_Cone ||
            aBAS2.GetType() == GeomAbs_Sphere ||
            aBAS2.GetType() == GeomAbs_Torus);
  //
  if (!isAna1 || !isAna2) {
    aTolFF =  Max(aTolFF, 5.e-6);
  }
}
