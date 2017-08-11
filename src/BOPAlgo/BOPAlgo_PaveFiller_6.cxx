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
#include <BOPAlgo_Alerts.hxx>
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
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TEdge.hxx>
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
#include <IntTools.hxx>
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
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//
static Standard_Real ToleranceFF(const BRepAdaptor_Surface& aBAS1,
                                 const BRepAdaptor_Surface& aBAS2);

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
  void SetFuzzyValue(const Standard_Real theFuzz) {
    IntTools_FaceFace::SetFuzzyValue(theFuzz);
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
  myIterator->Initialize(TopAbs_FACE, TopAbs_FACE);
  Standard_Integer iSize = myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  aFFs.SetIncrement(iSize);
  //
  // Options for the intersection algorithm
  Standard_Boolean bApprox   = mySectionAttribute.Approximation(),
                   bCompC2D1 = mySectionAttribute.PCurveOnS1(),
                   bCompC2D2 = mySectionAttribute.PCurveOnS2();
  Standard_Real    anApproxTol = 1.e-7;
  // Post-processing options
  Standard_Boolean bSplitCurve = Standard_False;
  //
  // Fence map to store faces with updated FaceInfo structure
  BOPCol_MapOfInteger aMIFence;
  // Prepare the pairs of faces for intersection
  BOPAlgo_VectorOfFaceFace aVFaceFace;
  Standard_Integer nF1, nF2;
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nF1, nF2);
    //
    const TopoDS_Face& aF1=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
    const TopoDS_Face& aF2=(*(TopoDS_Face *)(&myDS->Shape(nF2)));
    //
    if (aMIFence.Add(nF1)) {
      myDS->UpdateFaceInfoOn(nF1);
      myDS->UpdateFaceInfoIn(nF1);
    }
    if (aMIFence.Add(nF2)) {
      myDS->UpdateFaceInfoOn(nF2);
      myDS->UpdateFaceInfoIn(nF2);
    }
    //
    const BRepAdaptor_Surface& aBAS1 = myContext->SurfaceAdaptor(aF1);
    const BRepAdaptor_Surface& aBAS2 = myContext->SurfaceAdaptor(aF2);
    if (aBAS1.GetType() == GeomAbs_Plane && 
        aBAS2.GetType() == GeomAbs_Plane) {
      // Check if the planes are really interfering
      Standard_Boolean bToIntersect = CheckPlanes(nF1, nF2);
      if (!bToIntersect) {
        BOPDS_InterfFF& aFF=aFFs.Append1();
        aFF.SetIndices(nF1, nF2);
        aFF.Init(0, 0);
        continue;
      }
    }
    //
    if (myGlue == BOPAlgo_GlueOff) {
      BOPAlgo_FaceFace& aFaceFace=aVFaceFace.Append1();
      //
      aFaceFace.SetIndices(nF1, nF2);
      aFaceFace.SetFaces(aF1, aF2);
      // compute minimal tolerance for the curves
      Standard_Real aTolFF = ToleranceFF(aBAS1, aBAS2);
      aFaceFace.SetTolFF(aTolFF);
      //
      IntSurf_ListOfPntOn2S aListOfPnts;
      GetEFPnts(nF1, nF2, aListOfPnts);
      Standard_Integer aNbLP = aListOfPnts.Extent();
      if (aNbLP) {
        aFaceFace.SetList(aListOfPnts);
      }
      //
      aFaceFace.SetParameters(bApprox, bCompC2D1, bCompC2D2, anApproxTol);
      aFaceFace.SetFuzzyValue(myFuzzyValue);
      aFaceFace.SetProgressIndicator(myProgressIndicator);
    }
    else {
      // for the Glue mode just add all interferences of that type
      BOPDS_InterfFF& aFF = aFFs.Append1();
      aFF.SetIndices(nF1, nF2);
      aFF.SetTangentFaces(Standard_False);
      aFF.Init(0, 0);
    }
  }//for (; myIterator->More(); myIterator->Next()) {
  //
  //======================================================
  // Perform intersection
  BOPAlgo_FaceFaceCnt::Perform(myRunParallel, aVFaceFace);
  //======================================================
  // Treatment of the results
  Standard_Integer k, aNbFaceFace = aVFaceFace.Extent();
  for (k = 0; k < aNbFaceFace; ++k) {
    BOPAlgo_FaceFace& aFaceFace = aVFaceFace(k);
    aFaceFace.Indices(nF1, nF2);
    if (!aFaceFace.IsDone()) {
      BOPDS_InterfFF& aFF = aFFs.Append1();
      aFF.SetIndices(nF1, nF2);
      aFF.Init(0, 0);
      continue;
    }
    //
    Standard_Boolean bTangentFaces = aFaceFace.TangentFaces();
    Standard_Real aTolFF = aFaceFace.TolFF();
    //
    aFaceFace.PrepareLines3D(bSplitCurve);
    //
    const IntTools_SequenceOfCurves& aCvsX = aFaceFace.Lines();
    const IntTools_SequenceOfPntOn2Faces& aPntsX = aFaceFace.Points();
    //
    Standard_Integer aNbCurves = aCvsX.Length();
    Standard_Integer aNbPoints = aPntsX.Length();
    //
    if (aNbCurves || aNbPoints) {
      myDS->AddInterf(nF1, nF2);
    }
    //
    BOPDS_InterfFF& aFF = aFFs.Append1();
    aFF.SetIndices(nF1, nF2);
    aFF.SetTangentFaces(bTangentFaces);
    aFF.Init(aNbCurves, aNbPoints);
    //
    // Curves
    // Fix bounding box expanding coefficient.
    Standard_Real aBoxExpandValue = aTolFF;
    if (aNbCurves > 0)
    {
      // Modify geometric expanding coefficient by topology value,
      // since this bounding box used in sharing (vertex or edge).
      Standard_Real aMaxVertexTol = Max(BRep_Tool::MaxTolerance(aFaceFace.Face1(), TopAbs_VERTEX),
        BRep_Tool::MaxTolerance(aFaceFace.Face2(), TopAbs_VERTEX));
      aBoxExpandValue += aMaxVertexTol;
    }
    //
    BOPDS_VectorOfCurve& aVNC = aFF.ChangeCurves();
    for (Standard_Integer i = 1; i <= aNbCurves; ++i) {
      Bnd_Box aBox;
      const IntTools_Curve& aIC = aCvsX(i);
      Standard_Boolean bIsValid = IntTools_Tools::CheckCurve(aIC, aBox);
      if (bIsValid) {
        BOPDS_Curve& aNC = aVNC.Append1();
        aNC.SetCurve(aIC);
        // make sure that the bounding box has the maximal gap
        aBox.Enlarge(aBoxExpandValue);
        aNC.SetBox(aBox);
        aNC.SetTolerance(Max(aIC.Tolerance(), aTolFF));
      }
    }
    //
    // Points
    BOPDS_VectorOfPoint& aVNP = aFF.ChangePoints();
    for (Standard_Integer i = 1; i <= aNbPoints; ++i) {
      const IntTools_PntOn2Faces& aPi = aPntsX(i);
      const gp_Pnt& aP = aPi.P1().Pnt();
      //
      BOPDS_Point& aNP = aVNP.Append1();
      aNP.SetPnt(aP);
    }
  }
}
//=======================================================================
//function : MakeBlocks
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::MakeBlocks()
{
  if (myGlue != BOPAlgo_GlueOff) {
    return;
  }
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  Standard_Integer aNbFF = aFFs.Extent();
  if (!aNbFF) {
    return;
  }
  //
  Standard_Boolean bExist, bValid2D;
  Standard_Integer i, nF1, nF2, aNbC, aNbP, j;
  Standard_Integer nV1, nV2;
  Standard_Real aT1, aT2;
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
  BOPCol_MapOfInteger aMVOnIn(100, aAllocator),
                      aMVStick(100,aAllocator), aMVEF(100, aAllocator),
                      aMI(100, aAllocator), aMVBounds(100, aAllocator);
  BOPDS_IndexedMapOfPaveBlock aMPBOnIn(100, aAllocator);
  BOPDS_MapOfPaveBlock aMPBAdd(100, aAllocator), aMPBCommon;
  BOPDS_ListOfPaveBlock aLPB(aAllocator);
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks aMSCPB(100, aAllocator); 
  BOPCol_DataMapOfShapeInteger aMVI(100, aAllocator);
  BOPDS_DataMapOfPaveBlockListOfPaveBlock aDMExEdges(100, aAllocator);
  BOPCol_DataMapOfIntegerReal aMVTol(100, aAllocator);
  BOPCol_DataMapOfIntegerInteger aDMNewSD(100, aAllocator);
  BOPCol_DataMapOfIntegerListOfInteger aDMVLV;
  BOPCol_DataMapOfIntegerListOfInteger aDMBV(100, aAllocator);
  BOPCol_DataMapIteratorOfDataMapOfIntegerReal aItMV;
  BOPCol_IndexedMapOfShape aMicroEdges(100, aAllocator);
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
    Standard_Real aTolFF = Max(BRep_Tool::Tolerance(aF1), BRep_Tool::Tolerance(aF2));
    //
    BOPDS_FaceInfo& aFI1 = myDS->ChangeFaceInfo(nF1);
    BOPDS_FaceInfo& aFI2 = myDS->ChangeFaceInfo(nF2);
    //
    aMVOnIn.Clear();
    aMPBOnIn.Clear();
    aMPBCommon.Clear();
    aDMBV.Clear();
    aMVTol.Clear();
    aLSE.Clear();
    //
    myDS->SubShapesOnIn(nF1, nF2, aMVOnIn, aMPBOnIn, aMPBCommon);
    myDS->SharedEdges(nF1, nF2, aLSE, aAllocator);
    //
    Standard_Boolean bHasRealSectionEdge = Standard_False;
    // 1. Treat Points
    for (j=0; j<aNbP; ++j) {
      TopoDS_Vertex aV;
      BOPDS_CoupleOfPaveBlocks aCPB;
      //
      BOPDS_Point& aNP=aVP.ChangeValue(j);
      const gp_Pnt& aP=aNP.Pnt();
      //
      bExist=IsExistingVertex(aP, aTolFF, aMVOnIn);
      if (!bExist) {
        BOPTools_AlgoTools::MakeNewVertex(aP, aTolFF, aV);
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
    for (j = 0; j < aNbC; ++j) {
      BOPDS_Curve& aNC = aVC.ChangeValue(j);
      // DEBt
      aNC.InitPaveBlock1();
      //
      PutPavesOnCurve(aMVOnIn, aNC, nF1, nF2, aMI, aMVEF, aMVTol, aDMVLV);
    }

    // if some E-F vertex was put on a curve due to large E-F intersection range,
    // and it also was put on another curve correctly then remove this vertex from
    // the first curve. Detect such case if the distance to curve exceeds aTolR3D.
    FilterPavesOnCurves(aVC);

    for (j = 0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVC.ChangeValue(j);
      const IntTools_Curve& aIC=aNC.Curve();
      //
      PutStickPavesOnCurve(aF1, aF2, aMI, aNC, aMVStick, aMVTol, aDMVLV);
      //904/F7
      if (aNbC == 1) {
        PutEFPavesOnCurve(aNC, aMI, aMVEF, aMVTol, aDMVLV);
      }
      //
      if (aIC.HasBounds()) {
        aLBV.Clear();
        //
        PutBoundPaveOnCurve(aF1, aF2, aNC, aLBV);
        //
        if (!aLBV.IsEmpty()) {
          aDMBV.Bind(j, aLBV);
          BOPCol_ListIteratorOfListOfInteger aItI(aLBV);
          for (; aItI.More(); aItI.Next()) {
            aMVBounds.Add(aItI.Value());
          }
        }
      }
    }//for (j=0; j<aNbC; ++j) {

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
      Standard_Real aTolR3D = Max(aNC.Tolerance(), aNC.TangentialTolerance());
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
        bExist=IsExistingPaveBlock(aPB, aNC, aLSE);
        if (bExist) {
          continue;
        }
        //
        const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
        const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
        //
        // Make Edge
        BOPTools_AlgoTools::MakeEdge (aIC, aV1, aT1, aV2, aT2, aTolR3D, aES);
        //
        // check for micro edge
        if (BOPTools_AlgoTools::IsMicroEdge(aES, myContext, Standard_False)) {
          // If the section edge is a micro edge, i.e. the whole edge is
          // covered by the tolerance spheres of its vertices, it will be
          // passed into post treatment process to fuse its vertices.
          // The edge itself will not be kept.
          if (!aMVBounds.Contains(nV1) && !aMVBounds.Contains(nV2)) {
            aMicroEdges.Add(aES);
            // keep vertices for post treatment
            aMVI.Bind(aV1, nV1);
            aMVI.Bind(aV2, nV2);
          }
          continue;
        }
        //
        Standard_Real aTolNew;
        bExist=IsExistingPaveBlock(aPB, aNC, aTolR3D, aMPBOnIn, aMPBCommon, aPBOut, aTolNew);
        if (bExist) {
          if (!aMPBAdd.Contains(aPBOut)) {
            Standard_Boolean bInBothFaces = Standard_True;
            if (!myDS->IsCommonBlock(aPBOut)) {
              Standard_Integer nE;
              Standard_Real aTolE;
              //
              nE = aPBOut->Edge();
              const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(nE);
              aTolE = BRep_Tool::Tolerance(aE);
              if (aTolNew < aNC.Tolerance())
                aTolNew = aNC.Tolerance();  // use real tolerance of intersection
              if (aTolNew > aTolE) {
                UpdateEdgeTolerance(nE, aTolNew);
              }
              bInBothFaces = Standard_False;
            } 
            else {
              bInBothFaces = (aFI1.PaveBlocksOn().Contains(aPBOut) ||
                              aFI1.PaveBlocksIn().Contains(aPBOut))&&
                             (aFI2.PaveBlocksOn().Contains(aPBOut) ||
                              aFI2.PaveBlocksIn().Contains(aPBOut));
            }
            if (!bInBothFaces) {
              aMPBAdd.Add(aPBOut);
              PreparePostTreatFF(i, j, aPBOut, aMSCPB, aMVI, aLPBC);
            }
          }
          continue;
        }
        //
        // Make p-curves
        BOPTools_AlgoTools::MakePCurve(aES, aF1, aF2, aIC, 
                                       mySectionAttribute.PCurveOnS1(),
                                       mySectionAttribute.PCurveOnS2(),
                                       myContext);
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
        //
        bHasRealSectionEdge = Standard_True;
      }
      //
      aLPBC.RemoveFirst();
    }//for (j=0; j<aNbC; ++j) {
    //
    //back to previous tolerance values for unused vertices
    //and forget about SD groups of such vertices
    aItMV.Initialize(aMVTol);
    for (; aItMV.More(); aItMV.Next()) {
      nV1 = aItMV.Key();
      Standard_Real aTol = aItMV.Value();
      //
      const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&myDS->Shape(nV1);
      const Handle(BRep_TVertex)& TV = 
        *((Handle(BRep_TVertex)*)&aV.TShape());
      TV->Tolerance(aTol);
      // reset bnd box
      BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nV1);
      Bnd_Box& aBoxDS=aSIDS.ChangeBox();
      aBoxDS = Bnd_Box();
      BRepBndLib::Add(aV, aBoxDS);
      aBoxDS.SetGap(aBoxDS.GetGap() + Precision::Confusion());
      //
      if (aDMVLV.IsBound(nV1))
        aDMVLV.UnBind(nV1);
    }
    //
    ProcessExistingPaveBlocks(i, aMPBOnIn, aDMBV, aMSCPB, aMVI, aMPBAdd);
    //
    // If the pair of faces has produced any real section edges
    // it is necessary to check if these edges do not intersect
    // any common IN edges of the faces. For that, all such edges
    // are added for Post Treatment along with sections edges.
    if (bHasRealSectionEdge) {
      const BOPDS_IndexedMapOfPaveBlock& aMPBIn1 = aFI1.PaveBlocksIn();
      const BOPDS_IndexedMapOfPaveBlock& aMPBIn2 = aFI2.PaveBlocksIn();
      //
      // For simplicity add all IN edges into the first set of section curves.
      // These existing edges will be removed from the set on the post treatment
      // stage in the UpdateFaceInfo function.
      BOPDS_ListOfPaveBlock& aLPBC = aVC.ChangeValue(0).ChangePaveBlocks();
      //
      Standard_Integer aNbIn1 = aMPBIn1.Extent();
      for (j = 1; j <= aNbIn1; ++j) {
        const Handle(BOPDS_PaveBlock)& aPB = aMPBIn1(j);
        if (aMPBIn2.Contains(aPB) && aMPBAdd.Add(aPB)) {
          PreparePostTreatFF(i, 0, aPB, aMSCPB, aMVI, aLPBC);
        }
      }
    }
  }//for (i=0; i<aNbFF; ++i) {
  // 
  // post treatment
  MakeSDVerticesFF(aDMVLV, aDMNewSD);
  PostTreatFF(aMSCPB, aDMExEdges, aDMNewSD, aMicroEdges, aAllocator);
  if (HasErrors()) {
    return;
  }
  // reduce tolerances of section edges where it is appropriate
  CorrectToleranceOfSE();
  //
  // update face info
  UpdateFaceInfo(aDMExEdges, aDMNewSD);
  //Update all pave blocks
  UpdatePaveBlocks(aDMNewSD);
  //
  // Treat possible common zones by trying to put each section edge
  // into all faces, not participated in creation of that edge, as IN edge
  PutSEInOtherFaces();
  //
  //-----------------------------------------------------scope t
  aMVStick.Clear();
  aMPBOnIn.Clear();
  aMVOnIn.Clear();
  aDMExEdges.Clear();
  aMI.Clear();
  aDMNewSD.Clear();
}

//=======================================================================
//function : MakeSDVerticesFF
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::MakeSDVerticesFF
  (const BOPCol_DataMapOfIntegerListOfInteger& theDMVLV,
   BOPCol_DataMapOfIntegerInteger& theDMNewSD)
{
  // Create a new SD vertex for each group of coinciding vertices
  // and put new substitutions to theDMNewSD.
  BOPCol_DataMapIteratorOfDataMapOfIntegerListOfInteger aItG(theDMVLV);
  for (; aItG.More(); aItG.Next()) {
    const BOPCol_ListOfInteger& aList = aItG.Value();
    // make SD vertices w/o creation of interfs
    Standard_Integer nSD = MakeSDVertices(aList, Standard_False);
    // update theDMNewSD
    BOPCol_ListIteratorOfListOfInteger aItL(aList);
    for (; aItL.More(); aItL.Next()) {
      Standard_Integer nV = aItL.Value();
      theDMNewSD.Bind(nV, nSD);
    }
  }
}

//=======================================================================
//function : PostTreatFF
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::PostTreatFF
    (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
     BOPDS_DataMapOfPaveBlockListOfPaveBlock& aDMExEdges,
     BOPCol_DataMapOfIntegerInteger& aDMNewSD,
     const BOPCol_IndexedMapOfShape& theMicroEdges,
     const Handle(NCollection_BaseAllocator)& theAllocator)
{
  Standard_Integer aNbS = theMSCPB.Extent();
  if (!aNbS) {
    return;
  }
  //
  Standard_Boolean bHasPaveBlocks, bOld;
  Standard_Integer nSx, nVSD, iX, iP, iC, j, nV, iV = 0, iE, k;
  Standard_Integer aNbLPBx;
  TopAbs_ShapeEnum aType;
  TopoDS_Shape aV, aE;
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  BOPDS_PDS aPDS;
  Handle(BOPDS_PaveBlock) aPB1;
  BOPDS_Pave aPave[2];
  BOPDS_ShapeInfo aSI;
  //
  BOPCol_ListOfShape aLS(theAllocator);
  BOPAlgo_PaveFiller aPF(theAllocator);
  aPF.SetIsPrimary(Standard_False);
  aPF.SetNonDestructive(myNonDestructive);
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  //
  Standard_Integer aNbME = theMicroEdges.Extent();
  // 0
  if (aNbS==1 && (aNbME == 0)) {
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
    return;
  }
  //
  // 1 prepare arguments
  BOPCol_MapOfShape anAddedSD;
  for (k=1; k<=aNbS; ++k) {
    const TopoDS_Shape& aS=theMSCPB.FindKey(k);
    aLS.Append(aS);
    // add vertices-candidates for SD from the map aDMNewSD,
    // so that they took part in fuse operation.
    TopoDS_Iterator itV(aS);
    for (; itV.More(); itV.Next())
    {
      const TopoDS_Shape& aVer = itV.Value();
      Standard_Integer iVer = myDS->Index(aVer);
      const Standard_Integer* pSD = aDMNewSD.Seek(iVer);
      if (pSD)
      {
        const TopoDS_Shape& aVSD = myDS->Shape(*pSD);
        if (anAddedSD.Add(aVSD))
          aLS.Append(aVSD);
      }
    }
  }
  //
  // The section edges considered as a micro should be
  // specially treated - their vertices should be united and
  // the edge itself should be removed. Thus, we add only
  // its vertices into operation.
  //
  BRep_Builder aBB;
  for (k = 1; k <= aNbME; ++k) {
    const TopoDS_Edge& aEM = TopoDS::Edge(theMicroEdges(k));
    //
    TopoDS_Vertex aVerts[2];
    TopExp::Vertices(aEM, aVerts[0], aVerts[1]);
    for (Standard_Integer i = 0; i < 2; ++i) {
      nV = myDS->Index(aVerts[i]);
      const Standard_Integer* pSD = aDMNewSD.Seek(nV);
      if (pSD) {
        aVerts[i] = TopoDS::Vertex(myDS->Shape(*pSD));
      }
      //
      if (anAddedSD.Add(aVerts[i])) {
        aLS.Append(aVerts[i]);
      }
    }
    //
    if (aVerts[0].IsSame(aVerts[1])) {
      continue;
    }
    //
    // make sure these vertices will be united
    const gp_Pnt& aP1 = BRep_Tool::Pnt(aVerts[0]);
    const gp_Pnt& aP2 = BRep_Tool::Pnt(aVerts[1]);
    //
    Standard_Real aDist = aP1.Distance(aP2);
    Standard_Real aTolV1 = BRep_Tool::Tolerance(aVerts[0]);
    Standard_Real aTolV2 = BRep_Tool::Tolerance(aVerts[1]);
    //
    aDist -= (aTolV1 + aTolV2);
    if (aDist > 0.) {
      aDist /= 2.;
      aBB.UpdateVertex(aVerts[0], aTolV1 + aDist);
      aBB.UpdateVertex(aVerts[1], aTolV2 + aDist);
    }
  }
  //
  // 2 Fuse shapes
  aPF.SetProgressIndicator(myProgressIndicator);
  aPF.SetRunParallel(myRunParallel);
  aPF.SetArguments(aLS);
  aPF.Perform();
  if (aPF.HasErrors()) {
    AddError (new BOPAlgo_AlertPostTreatFF);
    return;
  }
  aPDS=aPF.PDS();
  //
  // Map to store the real tolerance of the common block
  // and avoid repeated computation of it
  NCollection_DataMap<Handle(BOPDS_CommonBlock),
                      Standard_Real,
                      TColStd_MapTransientHasher> aMCBTol;
  // Map to avoid creation of different pave blocks for
  // the same intersection edge
  NCollection_DataMap<Standard_Integer, Handle(BOPDS_PaveBlock)> aMEPB;
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
      Standard_Boolean bIntersectionPoint = theMSCPB.Contains(aSx);
      //
      if (aPDS->HasShapeSD(nSx, nVSD)) {
        aV=aPDS->Shape(nVSD);
      }
      else {
        aV=aSx;
      }
      // index of new vertex in theDS -> iV
      iV = myDS->Index(aV);
      if (iV < 0) {
        aSI.SetShapeType(aType);
        aSI.SetShape(aV);
        iV=myDS->Append(aSI);
      }
      //
      if (!bIntersectionPoint) {
        // save SD connection
        nSx = myDS->Index(aSx);
        aDMNewSD.Bind(nSx, iV);
        myDS->AddShapeSD(nSx, iV);
      }
      else {
        // update FF interference
        const BOPDS_CoupleOfPaveBlocks &aCPB=theMSCPB.FindFromKey(aSx);
        iX=aCPB.IndexInterf();
        iP=aCPB.Index();
        BOPDS_InterfFF& aFF=aFFs(iX);
        BOPDS_VectorOfPoint& aVNP=aFF.ChangePoints();
        BOPDS_Point& aNP=aVNP(iP);
        aNP.SetIndex(iV);
      }
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
          iE = myDS->Append(aSI);
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
        // check if edge occured to be micro edge;
        // note we check not the edge aSx itself, but its image in aPDS
        const BOPDS_ListOfPaveBlock& aLPBx = aPDS->PaveBlocks(nSx);
        aNbLPBx = aLPBx.Extent();
        if (aPDS->HasPaveBlocks(nSx) &&
            (aNbLPBx == 0 || (aNbLPBx == 1 && !aLPBx.First()->HasShrunkData()))) {
          BOPDS_ListIteratorOfListOfPaveBlock it(aLPBC);
          for (; it.More(); it.Next()) {
            if (it.Value() == aPB1) {
              aLPBC.Remove(it);
              break;
            }
          }
          continue;
        }
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
        if (aNbLPBx) {
          aItLPB.Initialize(aLPBx);
          for (; aItLPB.More(); aItLPB.Next()) {
            const Handle(BOPDS_PaveBlock)& aPBx=aItLPB.Value();
            const Handle(BOPDS_PaveBlock) aPBRx=aPDS->RealPaveBlock(aPBx);
            //
            // update vertices of paves
            aPave[0] = aPBx->Pave1();
            aPave[1] = aPBx->Pave2();
            for (j=0; j<2; ++j) {
              nV = aPave[j].Index();
              aV = aPDS->Shape(nV);
              // index of new vertex in myDS -> iV
              iV = myDS->Index(aV);
              if (iV < 0) {
                aSI.SetShapeType(TopAbs_VERTEX);
                aSI.SetShape(aV);
                iV = myDS->Append(aSI);
              }
              const BOPDS_Pave& aP1 = !j ? aPB1->Pave1() : aPB1->Pave2();
              if (aP1.Index() != iV) {
                if (aP1.Parameter() == aPave[j].Parameter()) {
                  aDMNewSD.Bind(aP1.Index(), iV);
                  myDS->AddShapeSD(aP1.Index(), iV);
                }
                else {
                  // check aPDS to have the SD connection between these vertices
                  const TopoDS_Shape& aVPave = myDS->Shape(aP1.Index());
                  Standard_Integer nVnewSD, nVnew = aPDS->Index(aVPave);
                  if (aPDS->HasShapeSD(nVnew, nVnewSD)) {
                    if (nVnewSD == nV) {
                      aDMNewSD.Bind(aP1.Index(), iV);
                      myDS->AddShapeSD(aP1.Index(), iV);
                    }
                  }
                }
              }
              //
              aPave[j].SetIndex(iV);
            }
            //
            // add edge
            aE=aPDS->Shape(aPBRx->Edge());
            iE = myDS->Index(aE);
            if (iE < 0) {
              aSI.SetShapeType(aType);
              aSI.SetShape(aE);
              iE=myDS->Append(aSI);
            }
            //
            // update real edge tolerance according to distances in common block if any
            if (aPDS->IsCommonBlock(aPBRx)) {
              const Handle(BOPDS_CommonBlock)& aCB = aPDS->CommonBlock(aPBRx);
              Standard_Real *pTol = aMCBTol.ChangeSeek(aCB);
              if (!pTol) {
                Standard_Real aTol = BOPAlgo_Tools::ComputeToleranceOfCB(aCB, aPDS, aPF.Context());
                pTol = aMCBTol.Bound(aCB, aTol);
              }
              //
              if (aNC.Tolerance() < *pTol) {
                aNC.SetTolerance(*pTol);
              }
            }
            // append new PaveBlock to aLPBC
            Handle(BOPDS_PaveBlock) *pPBC = aMEPB.ChangeSeek(iE);
            if (!pPBC) {
              pPBC = aMEPB.Bound(iE, new BOPDS_PaveBlock());
              BOPDS_Pave aPaveR1, aPaveR2;
              aPaveR1 = aPBRx->Pave1();
              aPaveR2 = aPBRx->Pave2();
              aPaveR1.SetIndex(myDS->Index(aPDS->Shape(aPaveR1.Index())));
              aPaveR2.SetIndex(myDS->Index(aPDS->Shape(aPaveR2.Index())));
              //
              (*pPBC)->SetPave1(aPaveR1);
              (*pPBC)->SetPave2(aPaveR2);
              (*pPBC)->SetEdge(iE);
            }
            //
            if (bOld) {
              (*pPBC)->SetOriginalEdge(aPB1->OriginalEdge());
              aDMExEdges.ChangeFind(aPB1).Append(*pPBC);
            }
            else {
              aLPBC.Append(*pPBC);
            }
          }
        }
      }
    }//else if (aType==TopAbs_EDGE)
  }//for (; aItLS.More(); aItLS.Next()) {

  // Update SD for vertices that did not participate in operation
  BOPCol_DataMapOfIntegerInteger::Iterator itDM(aDMNewSD);
  for (; itDM.More(); itDM.Next())
  {
    const Standard_Integer* pSD = aDMNewSD.Seek(itDM.Value());
    if (pSD)
    {
      itDM.ChangeValue() = *pSD;
      myDS->AddShapeSD(itDM.Key(), *pSD);
    }
  }
  return;
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
  Standard_Real aTolCheck;
  gp_Pnt aPV;
  Bnd_Box aBoxP;
  BOPCol_MapIteratorOfMapOfInteger aIt;
  //
  aTolCheck = theTolR3D + myFuzzyValue;
  bRet=Standard_True;
  //
  aBoxP.Add(aP);
  aBoxP.Enlarge(theTolR3D);
  //
  aIt.Initialize(aMVOnIn);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    const BOPDS_ShapeInfo& aSIV=myDS->ShapeInfo(nV);
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&aSIV.Shape()));
    const Bnd_Box& aBoxV=aSIV.Box();
    //
    if (!aBoxP.IsOut(aBoxV)) {
      iFlag=BOPTools_AlgoTools::ComputeVV(aV, aP, aTolCheck);
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
   const BOPCol_ListOfInteger& theLSE)
{
  Standard_Boolean bRet=Standard_True;
  //
  if (theLSE.IsEmpty()) {
    return !bRet;
  } 
  //
  Standard_Real aT1, aT2, aTm, aTx, aTolE, aTolCheck, aTol, aDist;
  Standard_Integer nE, iFlag, nV1, nV2;
  gp_Pnt aPm;
  Bnd_Box aBoxPm;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  //
  thePB->Range(aT1, aT2);
  thePB->Indices(nV1, nV2);
  const TopoDS_Vertex &aV1 = TopoDS::Vertex(myDS->Shape(nV1)),
                      &aV2 = TopoDS::Vertex(myDS->Shape(nV2));
  const Standard_Real aTolV1 = BRep_Tool::Tolerance(aV1),
                      aTolV2 = BRep_Tool::Tolerance(aV2);

  aTol = Max(aTolV1, aTolV2);

  aTm=IntTools_Tools::IntermediatePoint (aT1, aT2);
  theNC.Curve().D0(aTm, aPm);
  aBoxPm.Add(aPm);
  aBoxPm.Enlarge(aTol);
  //
  aItLI.Initialize(theLSE);
  for (; aItLI.More(); aItLI.Next()) {
    nE=aItLI.Value();
    if (nE < 0)
      continue;
    const BOPDS_ShapeInfo& aSIE=myDS->ChangeShapeInfo(nE);
    const Bnd_Box& aBoxE=aSIE.Box();
    if (!aBoxE.IsOut(aBoxPm)) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aSIE.Shape()));
      aTolE = BRep_Tool::Tolerance(aE);
      aTolCheck = Max(aTolE, aTol) + myFuzzyValue;
      iFlag = myContext->ComputePE(aPm, aTolCheck, aE, aTx, aDist);
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
     const BOPDS_MapOfPaveBlock& theMPBCommon,
     Handle(BOPDS_PaveBlock)& aPBOut,
     Standard_Real& theTolNew)
{
  Standard_Boolean bRet;
  Standard_Real aT1, aT2, aTm, aTx, aTolCheck;
  Standard_Integer nSp, iFlag1, iFlag2, nV11, nV12, nV21, nV22, i, aNbPB;
  gp_Pnt aP1, aPm, aP2;
  Bnd_Box aBoxP1, aBoxPm, aBoxP2, aBoxTmp;
  //
  bRet=Standard_False;
  aTolCheck = theTolR3D + myFuzzyValue;
  const IntTools_Curve& aIC=theNC.Curve();
  //
  thePB->Range(aT1, aT2);
  thePB->Indices(nV11, nV12);
  const Standard_Real aTolV11 = BRep_Tool::Tolerance(TopoDS::Vertex(myDS->Shape(nV11)));
  const Standard_Real aTolV12 = BRep_Tool::Tolerance(TopoDS::Vertex(myDS->Shape(nV12)));
  const Standard_Real aTolV1 = Max(aTolV11, aTolV12) + myFuzzyValue;

  //first point
  aIC.D0(aT1, aP1);
  aBoxP1.Add(aP1);
  aBoxP1.Enlarge(aTolV11);
  //intermediate point
  aTm=IntTools_Tools::IntermediatePoint (aT1, aT2);
  aIC.D0(aTm, aPm);
  aBoxPm.Add(aPm);
  //last point
  aIC.D0(aT2, aP2);
  aBoxP2.Add(aP2);
  aBoxP2.Enlarge(aTolV12);
  //
  theTolNew = 0.;
  aNbPB = theMPBOnIn.Extent();
  for (i = 1; i <= aNbPB; ++i) {
    const Handle(BOPDS_PaveBlock)& aPB = theMPBOnIn(i);
    aPB->Indices(nV21, nV22);
    const Standard_Real aTolV21 = BRep_Tool::Tolerance(TopoDS::Vertex(myDS->Shape(nV21)));
    const Standard_Real aTolV22 = BRep_Tool::Tolerance(TopoDS::Vertex(myDS->Shape(nV22)));
    const Standard_Real aTolV2 = Max(aTolV21, aTolV22) + myFuzzyValue;
    nSp=aPB->Edge();
    if (nSp < 0)
      continue;
    const BOPDS_ShapeInfo& aSISp=myDS->ChangeShapeInfo(nSp);
    const TopoDS_Edge& aSp=(*(TopoDS_Edge *)(&aSISp.Shape()));
    const Bnd_Box& aBoxSp=aSISp.Box();
    //
    iFlag1 = (nV11 == nV21 || nV11 == nV22) ? 2 : 
      (!aBoxSp.IsOut(aBoxP1) ? 1 : 0);
    iFlag2 = (nV12 == nV21 || nV12 == nV22) ? 2 : 
      (!aBoxSp.IsOut(aBoxP2) ? 1 : 0);
    if (iFlag1 && iFlag2) {
      Standard_Real aDist = 0.;

      Standard_Real aRealTol = aTolCheck;
      if (myDS->IsCommonBlock(aPB))
      {
        aRealTol = Max(aRealTol, Max(aTolV1, aTolV2));
        if (theMPBCommon.Contains(aPB))
          // for an edge, which is a common block with a face,
          // increase the chance to coincide with section curve
          aRealTol *= 2.;
      }
      
      aBoxTmp = aBoxPm;
      aBoxTmp.Enlarge(aRealTol);

      if (aBoxSp.IsOut(aBoxTmp) || myContext->ComputePE(aPm, 
                                                        aRealTol,
                                                        aSp, 
                                                        aTx, theTolNew)) {
        continue;
      }
      //
      if (iFlag1 == 1) {
        iFlag1 = !myContext->ComputePE(aP1, aRealTol, aSp, aTx, aDist);
        if (iFlag1 && theTolNew < aDist)
          theTolNew = aDist;
      }
      //
      if (iFlag2 == 1) {
        iFlag2 = !myContext->ComputePE(aP2, aRealTol, aSp, aTx, aDist);
        if (iFlag2 && theTolNew < aDist)
          theTolNew = aDist;
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
  Standard_Real aTolR3D = Max(aNC.Tolerance(), aNC.TangentialTolerance());
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
      aBoxDS.SetGap(aBoxDS.GetGap() + Precision::Confusion());
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
   BOPDS_Curve& aNC,
   const Standard_Integer nF1,
   const Standard_Integer nF2,
   const BOPCol_MapOfInteger& aMI,
   const BOPCol_MapOfInteger& aMVEF,
   BOPCol_DataMapOfIntegerReal& aMVTol,
   BOPCol_DataMapOfIntegerListOfInteger& aDMVLV)
{
  Standard_Boolean bInBothFaces;
  Standard_Integer nV;
  BOPCol_MapIteratorOfMapOfInteger aIt;
  //
  const Bnd_Box& aBoxC=aNC.Box();
  Standard_Real aTolR3D = Max(aNC.Tolerance(), aNC.TangentialTolerance());
  //
  //Put EF vertices first
  aIt.Initialize(aMVEF);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    PutPaveOnCurve(nV, aTolR3D, aNC, aMI, aMVTol, aDMVLV, 2);
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
    PutPaveOnCurve(nV, aTolR3D, aNC, aMI, aMVTol, aDMVLV, 1);
  }
}

//=======================================================================
//function : FilterPavesOnCurves
//purpose  : 
//=======================================================================
namespace {
  struct PaveBlockDist {
    Handle(BOPDS_PaveBlock) PB;
    Standard_Real Dist; // square distance from vertex to the paveblock
    Standard_Real SinAngle; // sinus of angle between projection vector 
    // and tangent at projection point
    Standard_Real Tolerance; // tolerance of the section curve
  };
}
void BOPAlgo_PaveFiller::FilterPavesOnCurves(const BOPDS_VectorOfCurve& theVNC)
{
  // For each vertex found in ExtPaves of pave blocks of section curves
  // collect list of pave blocks with distance to the curve
  NCollection_IndexedDataMap<Standard_Integer,NCollection_List<PaveBlockDist> > aIDMVertPBs;
  Standard_Integer i;
  const Standard_Real anEps = gp::Resolution();
  for (i = 0; i < theVNC.Extent(); ++i)
  {
    const BOPDS_Curve& aNC = theVNC(i);
    const IntTools_Curve& aIC = aNC.Curve();
    const Standard_Real aTolR3D = Max(aNC.Tolerance(), aNC.TangentialTolerance());
    GeomAdaptor_Curve aGAC(aIC.Curve());
    const Handle(BOPDS_PaveBlock)& aPB = aNC.PaveBlocks().First();
    const BOPDS_ListOfPave& aPaves = aPB->ExtPaves();
    BOPDS_ListOfPave::Iterator itPaves(aPaves);
    for (; itPaves.More(); itPaves.Next())
    {
      const BOPDS_Pave& aPave = itPaves.Value();
      Standard_Integer nV = aPave.Index();
      const TopoDS_Vertex& aV = TopoDS::Vertex(myDS->Shape(nV));
      // compute distance from vertex to the point on curve with vertex parameter
      gp_Pnt aPV = BRep_Tool::Pnt(aV);
      Standard_Real aPar = aPave.Parameter();
      gp_Pnt aPonC;
      gp_Vec aD1;
      aGAC.D1(aPar, aPonC, aD1);
      gp_Vec aProjVec(aPV, aPonC);
      Standard_Real aSqDist = aProjVec.SquareMagnitude();
      Standard_Real aSqD1Mod = aD1.SquareMagnitude();
      Standard_Real aSin = aProjVec.CrossSquareMagnitude(aD1);
      if (aSqDist > anEps && aSqD1Mod > anEps)
        aSin = sqrt(aSin / aSqDist / aSqD1Mod);
      NCollection_List<PaveBlockDist>* pList = aIDMVertPBs.ChangeSeek(nV);
      if (!pList)
        pList = &aIDMVertPBs.ChangeFromIndex(aIDMVertPBs.Add(nV, NCollection_List<PaveBlockDist>()));
      PaveBlockDist aPBD = { aPB, aSqDist, aSin, aTolR3D };
      pList->Append(aPBD);
    }
  }

  // Process each vertex
  const Standard_Real aSinAngleMin = 0.5; // angle below which projection is suspicious
  for (i = 1; i <= aIDMVertPBs.Extent(); i++)
  {
    Standard_Integer nV = aIDMVertPBs.FindKey(i);
    const NCollection_List<PaveBlockDist>& aList = aIDMVertPBs(i);
    // Find a pave with minimal distance
    Standard_Real aMinDist = RealLast();
    Handle(BOPDS_PaveBlock) aPBMinDist;
    NCollection_List<PaveBlockDist>::Iterator itL(aList);
    for (; itL.More(); itL.Next())
    {
      const PaveBlockDist& aPBD = itL.Value();
      if (aPBD.Dist < aMinDist)
      {
        aMinDist = aPBD.Dist;
        aPBMinDist = aPBD.PB;
      }
    }
    // Remove a vertex from a pave block if the distance is greater than the tolerance 
    // and there are other pave blocks for which the distance is less than the current.
    // Do not remove a vertex if it is projected on the curve with quite large angle
    // (see test bugs modalg_6 bug27761).
    for (itL.Init(aList); itL.More(); itL.Next())
    {
      const PaveBlockDist& aPBD = itL.Value();
      Standard_Real aCheckDist = 100. * Max(aPBD.Tolerance*aPBD.Tolerance, aMinDist);
      if (aPBD.Dist > aCheckDist && aPBD.SinAngle < aSinAngleMin)
      {
        aPBD.PB->RemoveExtPave(nV);
      }
    }
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
void BOPAlgo_PaveFiller::GetEFPnts
  (const Standard_Integer nF1,
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
//function : PutEFPavesOnCurve
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutEFPavesOnCurve
  (BOPDS_Curve& aNC,
   const BOPCol_MapOfInteger& aMI,
   const BOPCol_MapOfInteger& aMVEF,
   BOPCol_DataMapOfIntegerReal& aMVTol,
   BOPCol_DataMapOfIntegerListOfInteger& aDMVLV)
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
      PutPaveOnCurve(nV, aDist, aNC, aMI, aMVTol, aDMVLV);
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
   BOPCol_DataMapOfIntegerReal& aMVTol,
   BOPCol_DataMapOfIntegerListOfInteger& aDMVLV)
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
        PutPaveOnCurve(nV, aD, aNC, aMI, aMVTol, aDMVLV);
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
   const BOPDS_Curve& aNC,
   const BOPCol_MapOfInteger& aMI,
   BOPCol_DataMapOfIntegerReal& aMVTol,
   BOPCol_DataMapOfIntegerListOfInteger& aDMVLV,
   const Standard_Integer iCheckExtend)
{
  Standard_Boolean bIsVertexOnLine;
  Standard_Real aT;
  //
  const TopoDS_Vertex& aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV)));
  const Handle(BOPDS_PaveBlock)& aPB = aNC.PaveBlocks().First();
  const IntTools_Curve& aIC = aNC.Curve();
  //
  Standard_Real aTolV = (aMVTol.IsBound(nV) ? aMVTol(nV) : BRep_Tool::Tolerance(aV));

  bIsVertexOnLine = myContext->IsVertexOnLine(aV, aTolV, aIC, aTolR3D + myFuzzyValue, aT);
  if (!bIsVertexOnLine && iCheckExtend) {
    ExtendedTolerance(nV, aMI, aTolV, iCheckExtend);
    bIsVertexOnLine = myContext->IsVertexOnLine(aV, aTolV, aIC, aTolR3D + myFuzzyValue, aT);
  }
  //
  if (bIsVertexOnLine) {
    // check if aPB contains the parameter aT
    Standard_Boolean bExist;
    Standard_Integer nVUsed;
    Standard_Real aPTol, aDTol;
    //
    aDTol = 1.e-12;
    //
    GeomAdaptor_Curve aGAC(aIC.Curve());
    aPTol = aGAC.Resolution(Max(aTolR3D, aTolV));
    //
    bExist = aPB->ContainsParameter(aT, aPTol, nVUsed);
    if (bExist) {
      // use existing pave
      BOPCol_ListOfInteger* pList = aDMVLV.ChangeSeek(nVUsed);
      if (!pList) {
        pList = aDMVLV.Bound(nVUsed, BOPCol_ListOfInteger());
        pList->Append(nVUsed);
        if (!aMVTol.IsBound(nVUsed)) {
          const TopoDS_Vertex& aVUsed = (*(TopoDS_Vertex *)(&myDS->Shape(nVUsed)));
          aTolV = BRep_Tool::Tolerance(aVUsed);
          aMVTol.Bind(nVUsed, aTolV);
        }
      }
      // avoid repeated elements in the list
      BOPCol_ListIteratorOfListOfInteger aItLI(*pList);
      for (; aItLI.More(); aItLI.Next()) {
        if (aItLI.Value() == nV) {
          break;
        }
      }
      if (!aItLI.More()) {
        pList->Append(nV);
      }
      // save initial tolerance for the vertex
      if (!aMVTol.IsBound(nV)) {
        aTolV = BRep_Tool::Tolerance(aV);
        aMVTol.Bind(nV, aTolV);
      }
    }
    else {
      // add new pave
      BOPDS_Pave aPave;
      aPave.SetIndex(nV);
      aPave.SetParameter(aT);
      aPB->AppendExtPave(aPave);
      //
      gp_Pnt aP1 = aGAC.Value(aT);
      aTolV = BRep_Tool::Tolerance(aV);
      gp_Pnt aP2 = BRep_Tool::Pnt(aV);
      Standard_Real aDist = aP1.Distance(aP2);
      if (aDist > aTolV) {
        BRep_Builder().UpdateVertex(aV, aDist + aDTol);
        //
        if (!aMVTol.IsBound(nV)) {
          aMVTol.Bind(nV, aTolV);
        }
        //
        BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nV);
        Bnd_Box& aBoxDS=aSIDS.ChangeBox();
        BRepBndLib::Add(aV, aBoxDS);
        aBoxDS.SetGap(aBoxDS.GetGap() + Precision::Confusion());
      }
    }
  }
}

//=======================================================================
//function : ProcessExistingPaveBlocks
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
  Standard_Real aT, dummy;
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
        if (myDS->ShapeInfo(aPB->OriginalEdge()).HasFlag()) { // skip degenerated edges
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
        iFlag = myContext->ComputeVE(aV, aE, aT, dummy, myFuzzyValue);
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
  if (!aLPB.Extent()) {
    return;
  }
  //
  Standard_Integer nE;
  Standard_Boolean bCB;
  Handle(BOPDS_PaveBlock) aPB, aPB1, aPB2, aPB2n;
  Handle(BOPDS_CommonBlock) aCB;
  BOPDS_ListIteratorOfListOfPaveBlock aIt, aIt1, aIt2;
  //
  BOPDS_FaceInfo& aFI1 = myDS->ChangeFaceInfo(nF1);
  BOPDS_FaceInfo& aFI2 = myDS->ChangeFaceInfo(nF2);
  //
  BOPDS_IndexedMapOfPaveBlock& aMPBOn1 = aFI1.ChangePaveBlocksOn();
  BOPDS_IndexedMapOfPaveBlock& aMPBIn1 = aFI1.ChangePaveBlocksIn();
  BOPDS_IndexedMapOfPaveBlock& aMPBOn2 = aFI2.ChangePaveBlocksOn();
  BOPDS_IndexedMapOfPaveBlock& aMPBIn2 = aFI2.ChangePaveBlocksIn();
  //
  // 1. Remove old pave blocks
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
  // 2. Update pave blocks
  if (bCB) {
    // Create new common blocks
    BOPDS_ListOfPaveBlock aLPBNew;
    const BOPCol_ListOfInteger& aFaces = aCB1->Faces();
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next()) {
      const Handle(BOPDS_PaveBlock)& aPBValue = aIt.Value();
      BOPDS_Pave aPBValuePaves[2] = {aPBValue->Pave1(), aPBValue->Pave2()};
      //
      aCB = new BOPDS_CommonBlock;
      aIt1.Initialize(aLPB1);
      for (; aIt1.More(); aIt1.Next()) {
        aPB2 = aIt1.Value();
        nE = aPB2->OriginalEdge();
        //
        // Create new pave block
        aPB2n = new BOPDS_PaveBlock;
        if (aPBValue->OriginalEdge() == nE) {
          aPB2n->SetPave1(aPBValuePaves[0]);
          aPB2n->SetPave2(aPBValuePaves[1]);
        }
        else {
          // For the different original edge compute the parameters of paves
          BOPDS_Pave aPave[2];
          for (Standard_Integer i = 0; i < 2; ++i) {
            Standard_Integer nV = aPBValuePaves[i].Index();
            aPave[i].SetIndex(nV);
            if (nV == aPB2->Pave1().Index()) {
              aPave[i].SetParameter(aPB2->Pave1().Parameter());
            }
            else if (nV == aPB2->Pave2().Index()) {
              aPave[i].SetParameter(aPB2->Pave2().Parameter());
            }
            else {
              // Compute the parameter by projecting the point
              const TopoDS_Vertex& aV = TopoDS::Vertex(myDS->Shape(nV));
              const TopoDS_Edge& aEOr = TopoDS::Edge(myDS->Shape(nE));
              Standard_Real aTOut, aDist;
              Standard_Integer iErr = myContext->ComputeVE(aV, aEOr, aTOut, aDist, myFuzzyValue);
              if (!iErr) {
                aPave[i].SetParameter(aTOut);
              }
              else {
                // Unable to project - set the parameter of the closest boundary
                const TopoDS_Vertex& aV1 = TopoDS::Vertex(myDS->Shape(aPB2->Pave1().Index()));
                const TopoDS_Vertex& aV2 = TopoDS::Vertex(myDS->Shape(aPB2->Pave2().Index()));
                //
                gp_Pnt aP = BRep_Tool::Pnt(aV);
                gp_Pnt aP1 = BRep_Tool::Pnt(aV1);
                gp_Pnt aP2 = BRep_Tool::Pnt(aV2);
                //
                Standard_Real aDist1 = aP.SquareDistance(aP1);
                Standard_Real aDist2 = aP.SquareDistance(aP2);
                //
                aPave[i].SetParameter(aDist1 < aDist2 ? aPB2->Pave1().Parameter() : aPB2->Pave2().Parameter());
              }
            }
          }
          //
          if (aPave[1].Parameter() < aPave[0].Parameter()) {
            BOPDS_Pave aPaveTmp = aPave[0];
            aPave[0] = aPave[1];
            aPave[1] = aPaveTmp;
          }
          //
          aPB2n->SetPave1(aPave[0]);
          aPB2n->SetPave2(aPave[1]);
        }
        //
        aPB2n->SetEdge(aPBValue->Edge());
        aPB2n->SetOriginalEdge(nE);
        aCB->AddPaveBlock(aPB2n);
        myDS->SetCommonBlock(aPB2n, aCB);
        myDS->ChangePaveBlocks(nE).Append(aPB2n);
      }
      aCB->SetFaces(aFaces);
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
  // 3. Check new pave blocks for coincidence 
  //    with the opposite face.
  //    In case of coincidence create common blocks
  Standard_Integer nF;
  //
  nF = bIn1 ? nF2 : nF1;
  const TopoDS_Face& aF = *(TopoDS_Face*)&myDS->Shape(nF);
  BOPDS_IndexedMapOfPaveBlock& aMPBIn = bIn1 ? aMPBIn2 : aMPBIn1;
  //
  aIt.Initialize(aLPB);
  for (; aIt.More(); aIt.Next()) {
    Handle(BOPDS_PaveBlock)& aPBChangeValue = aIt.ChangeValue();
    const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(aPBChangeValue->Edge());
    //
    IntTools_EdgeFace anEF;
    anEF.SetEdge(aE);
    anEF.SetFace(aF);
    anEF.SetFuzzyValue(myFuzzyValue);
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
(const BOPCol_DataMapOfIntegerInteger& aDMNewSD)
{
  if (aDMNewSD.IsEmpty()) {
    return;
  }
  //
  Standard_Integer nSp, aNbPBP, nV[2], i, j;
  Standard_Real aT[2];
  Standard_Boolean bCB, bRebuild;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  BOPDS_MapOfPaveBlock aMPB;
  BOPCol_MapOfInteger aMicroEdges;
  //
  BOPDS_ListOfPaveBlock anAllPBs;

  // Get pave blocks of section edges
  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  Standard_Integer aNbFF = aFFs.Extent();
  for (i = 0; i < aNbFF; ++i)
  {
    const BOPDS_InterfFF& aFF = aFFs(i);
    const BOPDS_VectorOfCurve& aVNC = aFF.Curves();
    Standard_Integer aNbC = aVNC.Extent();
    for (j = 0; j < aNbC; ++j)
    {
      const BOPDS_Curve& aNC = aVNC(j);
      const BOPDS_ListOfPaveBlock& aLPBC = aNC.PaveBlocks();
      aItPB.Initialize(aLPBC);
      for (; aItPB.More(); aItPB.Next())
        anAllPBs.Append(aItPB.Value());
    }
  }

  // Get pave blocks from the pool
  BOPDS_VectorOfListOfPaveBlock& aPBP = myDS->ChangePaveBlocksPool();
  aNbPBP = aPBP.Extent();
  for (i = 0; i < aNbPBP; ++i) {
    BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next())
      anAllPBs.Append(aItPB.Value());
  }

  // Process all pave blocks
  aItPB.Initialize(anAllPBs);
  for (; aItPB.More(); aItPB.Next())
  {
    Handle(BOPDS_PaveBlock) aPB = aItPB.Value();
    const Handle(BOPDS_CommonBlock)& aCB = myDS->CommonBlock(aPB);
    bCB = !aCB.IsNull();
    if (bCB) {
      aPB = aCB->PaveBlock1();
    }
    //
    if (aMPB.Add(aPB)) {
      bRebuild = Standard_False;
      aPB->Indices(nV[0], nV[1]);
      aPB->Range(aT[0], aT[1]);
      // remember the fact if the edge had different vertices before substitution
      Standard_Boolean wasRegularEdge = (nV[0] != nV[1]);
      //
      for (j = 0; j < 2; ++j) {
        if (aDMNewSD.IsBound(nV[j])) {
          BOPDS_Pave aPave;
          //
          nV[j] = aDMNewSD.Find(nV[j]);
          aPave.SetIndex(nV[j]);
          aPave.SetParameter(aT[j]);
          //
          bRebuild = Standard_True;
          if (!j) {
            aPB->SetPave1(aPave);
          }
          else {
            aPB->SetPave2(aPave);
          }
        }
      }
      //
      if (bRebuild) {
        Standard_Integer nE = aPB->Edge();
        // Check if the Pave Block has the edge set
        if (nE < 0) {
          // untouched edge
          nE = aPB->OriginalEdge();
        }
        Standard_Boolean isDegEdge = myDS->ShapeInfo(nE).HasFlag();
        if (wasRegularEdge && !isDegEdge && nV[0] == nV[1]) {
          // now edge has the same vertex on both ends;
          // check if it is not a regular closed curve.
          const TopoDS_Edge& aE = TopoDS::Edge(myDS->Shape(nE));
          const TopoDS_Vertex& aV = TopoDS::Vertex(myDS->Shape(nV[0]));
          Standard_Real aLength = IntTools::Length(aE);
          Standard_Real aTolV = BRep_Tool::Tolerance(aV);
          if (aLength <= aTolV * 2.) {
            // micro edge, so mark it for removal
            aMicroEdges.Add(nE);
            continue;
          }
        }
        nSp = SplitEdge(nE, nV[0], aT[0], nV[1], aT[1]);
        if (bCB)
          aCB->SetEdge(nSp);
        else
          aPB->SetEdge(nSp);
      }// if (bRebuild) {
    }// if (aMPB.Add(aPB)) {
  }// for (; aItPB.More(); aItPB.Next()) {
  aMPB.Clear();

  if (aMicroEdges.Extent())
    RemovePaveBlocks(aMicroEdges);
}
//=======================================================================
//function : RemovePaveBlocks
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::RemovePaveBlocks(const BOPCol_MapOfInteger theEdges)
{
  // Remove all pave blocks referring to input edges:
  //
  // 1. from the Pave Blocks Pool
  BOPDS_VectorOfListOfPaveBlock& aPBP = myDS->ChangePaveBlocksPool();
  Standard_Integer aNbPBP = aPBP.Extent(), i;
  for (i = 0; i < aNbPBP; ++i) {
    BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    //
    BOPDS_ListIteratorOfListOfPaveBlock aItPB(aLPB);
    while (aItPB.More()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
      if (theEdges.Contains(aPB->Edge()))
        aLPB.Remove(aItPB);
      else
        aItPB.Next();
    }
  }

  // 2. from Face Info and section curves
  BOPCol_MapOfInteger aMPassed;
  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  Standard_Integer aNbFF = aFFs.Extent(), j;
  for (i = 0; i < aNbFF; ++i) {
    BOPDS_InterfFF& aFF = aFFs(i);
    Standard_Integer nF1, nF2;
    aFF.Indices(nF1, nF2);
    //
    // rebuild pave block maps of face info
    for (j = 0; j < 2; j++) {
      Standard_Integer nF = (j == 0 ? nF1 : nF2);
      if (!aMPassed.Add(nF))
        continue;
      BOPDS_FaceInfo& aFI = myDS->ChangeFaceInfo(nF);
      BOPDS_IndexedMapOfPaveBlock* aIMPB[] = { &aFI.ChangePaveBlocksIn(),
        &aFI.ChangePaveBlocksOn(), &aFI.ChangePaveBlocksSc() };
      for (Standard_Integer k = 0; k < 3; k++) {
        Standard_Integer aNbPB = aIMPB[k]->Extent(), m;
        for (m = 1; m <= aNbPB; ++m) {
          const Handle(BOPDS_PaveBlock)& aPB = aIMPB[k]->FindKey(m);
          if (theEdges.Contains(aPB->Edge()))
            break;
        }
        if (m <= aNbPB) {
          BOPDS_IndexedMapOfPaveBlock aMPBCopy = *aIMPB[k];
          aIMPB[k]->Clear();
          for (m = 1; m <= aNbPB; ++m) {
            const Handle(BOPDS_PaveBlock)& aPB = aMPBCopy(m);
            if (!theEdges.Contains(aPB->Edge()))
              aIMPB[k]->Add(aPB);
          }
        }
      }
    }
    // remove from Section pave blocks
    BOPDS_VectorOfCurve& aVNC = aFF.ChangeCurves();
    Standard_Integer aNbC = aVNC.Extent();
    for (j = 0; j < aNbC; ++j) {
      BOPDS_Curve& aNC = aVNC(j);
      BOPDS_ListOfPaveBlock& aLPB = aNC.ChangePaveBlocks();
      BOPDS_ListIteratorOfListOfPaveBlock aItPB(aLPB);
      while (aItPB.More()) {
        const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
        if (theEdges.Contains(aPB->Edge()))
          aLPB.Remove(aItPB);
        else
          aItPB.Next();
      }
    }
  }
}
//=======================================================================
//function : ToleranceFF
//purpose  : Computes the TolFF according to the tolerance value and 
//           types of the faces.
//=======================================================================
Standard_Real ToleranceFF(const BRepAdaptor_Surface& aBAS1,
                          const BRepAdaptor_Surface& aBAS2)
{
  Standard_Real aTol1 = aBAS1.Tolerance();
  Standard_Real aTol2 = aBAS2.Tolerance();
  Standard_Real aTolFF = Max(aTol1, aTol2);
  //
  Standard_Boolean isAna1, isAna2;
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
  return aTolFF;
}
//=======================================================================
//function : UpdateBlocksWithSharedVertices
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::UpdateBlocksWithSharedVertices()
{
  if (!myNonDestructive) {
    return;
  }
  //
  Standard_Integer aNbFF;
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aNbFF=aFFs.Extent();
  if (!aNbFF) {
    return;
  }
  //
  Standard_Boolean bOnCurve, bHasShapeSD;
  Standard_Integer i, nF1, nF2, aNbC, j, nV, nVSD;
  Standard_Real aTolV;
  BOPCol_MapOfInteger aMF;
  //
  for (i=0; i<aNbFF; ++i) {
    BOPDS_InterfFF& aFF=aFFs(i);
    //
    BOPDS_VectorOfCurve& aVC=aFF.ChangeCurves();
    aNbC=aVC.Extent();
    if (!aNbC) {
      continue;
    }
    //
    aFF.Indices(nF1, nF2);
    //
    if (aMF.Add(nF1)) {
      myDS->UpdateFaceInfoOn(nF1);
    }
    if (aMF.Add(nF2)) {
      myDS->UpdateFaceInfoOn(nF2);
    }
    //
    // Collect old vertices that are shared for nF1, nF2 ->aMI;
    BOPCol_MapOfInteger aMI;
    BOPCol_MapIteratorOfMapOfInteger aItMI;
    //
    BOPDS_FaceInfo& aFI1=myDS->ChangeFaceInfo(nF1);
    BOPDS_FaceInfo& aFI2=myDS->ChangeFaceInfo(nF2);
    //
    const BOPCol_MapOfInteger& aMVOn1=aFI1.VerticesOn();
    const BOPCol_MapOfInteger& aMVIn1=aFI1.VerticesIn();
    const BOPCol_MapOfInteger& aMVOn2=aFI2.VerticesOn();
    const BOPCol_MapOfInteger& aMVIn2=aFI2.VerticesIn();
    //
    for (j=0; j<2; ++j) {
      const BOPCol_MapOfInteger& aMV1=(!j) ? aMVOn1 : aMVIn1;
      aItMI.Initialize(aMV1);
      for (; aItMI.More(); aItMI.Next()) {
        nV=aItMI.Value();
        if (myDS->IsNewShape(nV)) {
          continue;
        }
        if (aMVOn2.Contains(nV) || aMVIn2.Contains(nV)) {
          aMI.Add(nV);
        }
      }
    }
    //
    // Try to put vertices aMI on curves
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVC.ChangeValue(j);
      Standard_Real aTolR3D = Max(aNC.Tolerance(), aNC.TangentialTolerance());
      //
      aItMI.Initialize(aMI);
      for (; aItMI.More(); aItMI.Next()) {
        nV=aItMI.Value();
        //
        bHasShapeSD=myDS->HasShapeSD(nV, nVSD);
        if (bHasShapeSD) {
          continue;
        }
        //
        bOnCurve=EstimatePaveOnCurve(nV, aNC, aTolR3D);
        if (!bOnCurve) {
          continue;
        }
        //
        const TopoDS_Vertex& aV=*((TopoDS_Vertex *)&myDS->Shape(nV));
        aTolV=BRep_Tool::Tolerance(aV);
        //
        UpdateVertex(nV, aTolV);
      }
    }//for (j=0; j<aNbC; ++j) {
  }//for (i=0; i<aNbFF; ++i) {
  //
  UpdateCommonBlocksWithSDVertices();
}
//=======================================================================
//function : EstimatePaveOnCurve
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::EstimatePaveOnCurve
  (const Standard_Integer nV,
   const BOPDS_Curve& aNC,
   const Standard_Real aTolR3D)
{
  Standard_Boolean bIsVertexOnLine;
  Standard_Real aT;
  //
  const TopoDS_Vertex& aV=*((TopoDS_Vertex *)&myDS->Shape(nV));
  const IntTools_Curve& aIC=aNC.Curve();
  //
  bIsVertexOnLine=myContext->IsVertexOnLine(aV, aIC, aTolR3D, aT);
  return bIsVertexOnLine;
}

//=======================================================================
//function : CorrectToleranceOfSE
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::CorrectToleranceOfSE()
{
  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  NCollection_IndexedDataMap<Standard_Integer,BOPDS_ListOfPaveBlock> aMVIPBs;
  BOPCol_MapOfInteger aMVIToReduce;
  // Fence map to avoid repeated checking of the same edge
  BOPDS_MapOfPaveBlock aMPB;
  //
  // 1. iterate on all sections F-F
  Standard_Integer aNb = aFFs.Extent(), i;
  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfFF& aFF = aFFs(i);
    //
    BOPDS_VectorOfCurve& aVNC = aFF.ChangeCurves();
    Standard_Integer aNbC = aVNC.Extent(), k;
    for (k = 0; k < aNbC; ++k) {
      BOPDS_Curve& aNC = aVNC(k);
      BOPDS_ListOfPaveBlock& aLPB = aNC.ChangePaveBlocks();
      BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
      for (; aItLPB.More(); ) {
        const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
        Standard_Integer nE;
        if (!aPB->HasEdge(nE)) {
          aLPB.Remove(aItLPB);
          continue;
        }
        //
        if (!aMPB.Add(aPB)) {
          aItLPB.Next();
          continue;
        }
        //
        Standard_Boolean bIsReduced = Standard_False;
        if (aPB->OriginalEdge() < 0) {
          // It is possible that due to small angle between faces the
          // common zone between faces can be large and the tangential
          // tolerance of the curve will be large as well.
          // Here we're trying to reduce the tolerance of the section
          // edge using the valid tolerance of the edge.
          // Note, that if the pave block has created common block with
          // other edges its valid tolerance could have been changed to
          // cover all edges in common block (see PostTreatFF() method).
          Standard_Real aTolC = aNC.Tolerance();
          Standard_Real aTolTang = aNC.TangentialTolerance();
          if (aTolC < aTolTang) {
            const TopoDS_Edge& aE = TopoDS::Edge(myDS->Shape(nE));
            Standard_Real aTolE = BRep_Tool::Tolerance(aE);
            if (aTolC < aTolE) {
              // reduce edge tolerance
              reinterpret_cast<BRep_TEdge*>(aE.TShape().operator->())->Tolerance(aTolC);
              bIsReduced = Standard_True;
            }
          }
        }
        //
        // fill in the map vertex index - pave blocks
        for (Standard_Integer j=0; j < 2; j++) {
          Standard_Integer nV = (j == 0 ? aPB->Pave1().Index() : aPB->Pave2().Index());
          myDS->HasShapeSD(nV, nV);
          BOPDS_ListOfPaveBlock *pPBList = aMVIPBs.ChangeSeek(nV);
          if (!pPBList) {
            pPBList = &aMVIPBs.ChangeFromIndex(aMVIPBs.Add(nV, BOPDS_ListOfPaveBlock()));
          }
          pPBList->Append(aPB);
          if (bIsReduced) {
            aMVIToReduce.Add(nV);
          }
        }
        aItLPB.Next();
      }
    }
  }
  //
  if (aMVIToReduce.IsEmpty()) {
    return;
  }
  //
  // 2. try to reduce tolerances of connected vertices
  // 2.1 find all other edges containing these connected vertices to avoid
  //     reducing the tolerance to the value less than the tolerances of edges,
  //     i.e. minimal tolerance for the vertex is the max tolerance of the
  //     edges containing this vertex
  BOPCol_DataMapOfIntegerReal aMVITol;
  BOPDS_VectorOfListOfPaveBlock& aPBP = myDS->ChangePaveBlocksPool();
  aNb = aPBP.Extent();
  for (i = 0; i < aNb; ++i) {
    const BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
    for (; aItLPB.More(); aItLPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
      Standard_Integer nE;
      if (!aPB->HasEdge(nE)) {
        continue;
      }
      const TopoDS_Edge& aE = TopoDS::Edge(myDS->Shape(nE));
      Standard_Real aTolE = BRep_Tool::Tolerance(aE);
      //
      Standard_Integer nV[2];
      aPB->Indices(nV[0], nV[1]);
      //
      for (Standard_Integer j = 0; j < 2; j++) {
        if (aMVIToReduce.Contains(nV[j])) {
          Standard_Real *aMaxTol = aMVITol.ChangeSeek(nV[j]);
          if (!aMaxTol) {
            aMVITol.Bind(nV[j], aTolE);
          }
          else if (aTolE > *aMaxTol) {
            *aMaxTol = aTolE;
          }
        }
      }
    }
  }
  //
  // 2.2 reduce tolerances if possible
  aNb = aMVIPBs.Extent();
  for (i = 1; i <= aNb; ++i) {
    Standard_Integer nV = aMVIPBs.FindKey(i);
    if (!aMVIToReduce.Contains(nV)) {
      continue;
    }
    //
    const TopoDS_Vertex& aV = TopoDS::Vertex(myDS->Shape(nV));
    Standard_Real aTolV = BRep_Tool::Tolerance(aV);
    Standard_Real aMaxTol = aMVITol.IsBound(nV) ? aMVITol.Find(nV) : 0.;
    // it makes no sense to compute the real tolerance if it is
    // impossible to reduce the tolerance at least 0.1% of the current value
    if (aTolV - aMaxTol < 0.001 * aTolV) {
      continue;
    }
    //
    // compute the maximal distance from the vertex to the adjacent edges
    gp_Pnt aP = BRep_Tool::Pnt(aV);
    //
    // Avoid repeated checks
    BOPDS_MapOfPaveBlock aMPBFence;
    //
    const BOPDS_ListOfPaveBlock& aLPB = aMVIPBs.FindFromIndex(i);
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
    for (; aItLPB.More(); aItLPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
      if (!aMPBFence.Add(aPB)) {
        continue;
      }
      Standard_Integer nE = aPB->Edge();
      const TopoDS_Edge& aE = TopoDS::Edge(myDS->Shape(nE));
      BRepAdaptor_Curve aC(aE);
      for (Standard_Integer iPave = 0; iPave < 2; ++iPave) {
        const BOPDS_Pave& aPave = !iPave ? aPB->Pave1() : aPB->Pave2();
        Standard_Integer nVSD = aPave.Index();
        myDS->HasShapeSD(nVSD, nVSD);
        if (nVSD != nV) {
          continue;
        }
        //
        gp_Pnt aPonE = aC.Value(aPave.Parameter());
        Standard_Real aDist = aP.Distance(aPonE);
        aDist += BRep_Tool::Tolerance(aE);
        if (aDist > aMaxTol) {
          aMaxTol = aDist;
        }
      }
    }
    //
    if (aMaxTol < aTolV) {
      reinterpret_cast<BRep_TVertex*>(aV.TShape().operator->())->Tolerance(aMaxTol);
    }
  }
}

//=======================================================================
//function : PutSEInOtherFaces
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::PutSEInOtherFaces()
{
  // Try to intersect each section edge with the faces
  // not participated in its creation
  //
  // 1. Get all section edges
  BOPDS_IndexedMapOfPaveBlock aMPBScAll;
  //
  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  Standard_Integer i, j, aNbFF = aFFs.Extent();
  //
  for (i = 0; i < aNbFF; ++i) {
    const BOPDS_VectorOfCurve& aVNC = aFFs(i).Curves();
    Standard_Integer aNbC = aVNC.Extent();
    for (j = 0; j < aNbC; ++j) {
      const BOPDS_ListOfPaveBlock& aLPBC = aVNC(j).PaveBlocks();
      BOPDS_ListIteratorOfListOfPaveBlock aItPB(aLPBC);
      for (; aItPB.More(); aItPB.Next()) {
        aMPBScAll.Add(aItPB.Value());
      }
    }
  }
  //
  Standard_Integer aNbPBSc = aMPBScAll.Extent();
  //
  // 2. Loop for all faces and check each section curve
  Standard_Integer aNbS = myDS->NbSourceShapes();
  for (i = 0; i < aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE) {
      continue;
    }
    //
    const TopoDS_Face& aF = (*(TopoDS_Face*)(&aSI.Shape()));
    BOPDS_FaceInfo& aFI = myDS->ChangeFaceInfo(i);
    //
    // IN edges to add new ones
    BOPDS_IndexedMapOfPaveBlock& aMFPBIn = aFI.ChangePaveBlocksIn();
    // Section edges to check the participation of the face
    const BOPDS_IndexedMapOfPaveBlock& aMFPBSc = aFI.PaveBlocksSc();
    //
    // Get vertices of the face to check that vertices of the
    // processed section edge belong to the face
    BOPCol_MapOfInteger aMFVerts;
    // Get vertices from ON, IN and Sc pave blocks of the face
    for (j = 0; j < 3; ++j) {
      const BOPDS_IndexedMapOfPaveBlock& aMPB =
        !j ? aFI.PaveBlocksOn() : (j == 1 ? aMFPBIn : aMFPBSc);
      Standard_Integer aNbPB = aMPB.Extent();
      for (Standard_Integer k = 1; k <= aNbPB; ++k) {
        const Handle(BOPDS_PaveBlock)& aPB = aMPB(k);
        aMFVerts.Add(aPB->Pave1().Index());
        aMFVerts.Add(aPB->Pave2().Index());
      }
    }
    // Add ON, IN and Sc vertices of the face
    for (j = 0; j < 3; ++j) {
      const BOPCol_MapOfInteger& aMFV = !j ? aFI.VerticesOn() :
        (j == 1 ? aFI.VerticesIn() : aFI.VerticesSc());
      BOPCol_MapIteratorOfMapOfInteger aItMI(aMFV);
      for (; aItMI.More(); aItMI.Next()) {
        aMFVerts.Add(aItMI.Value());
      }
    }
    //
    // Check each section edge for possible belonging to the face
    for (j = 1; j <= aNbPBSc; ++j) {
      const Handle(BOPDS_PaveBlock)& aPB = aMPBScAll(j);
      if (aMFPBSc.Contains(aPB)) {
        continue;
      }
      //
      // Both vertices of the section edge should belong to the face
      if (!aMFVerts.Contains(aPB->Pave1().Index()) ||
        !aMFVerts.Contains(aPB->Pave2().Index())) {
        continue;
      }
      //
      // Perform intersection
      const TopoDS_Edge& aE = TopoDS::Edge(myDS->Shape(aPB->Edge()));
      //
      IntTools_EdgeFace anEFInt;
      anEFInt.SetEdge(aE);
      anEFInt.SetFace(aF);
      anEFInt.SetFuzzyValue(myFuzzyValue);
      anEFInt.SetRange(aPB->Pave1().Parameter(), aPB->Pave2().Parameter());
      anEFInt.SetContext(myContext);
      anEFInt.UseQuickCoincidenceCheck(Standard_True);
      anEFInt.Perform();
      //
      const IntTools_SequenceOfCommonPrts& aCPrts = anEFInt.CommonParts();
      if ((aCPrts.Length() == 1) && (aCPrts(1).Type() == TopAbs_EDGE)) {
        Handle(BOPDS_CommonBlock) aCB;
        if (myDS->IsCommonBlock(aPB)) {
          aCB = myDS->CommonBlock(aPB);
        }
        else {
          aCB = new BOPDS_CommonBlock;
          aCB->AddPaveBlock(aPB);
        }
        //
        aCB->AddFace(i);
        //
        aMFPBIn.Add(aPB);
      }
    }
  }
}
