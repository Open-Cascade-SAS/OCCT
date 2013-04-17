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
#include <Bnd_Box.hxx>

#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>

#include <TopExp_Explorer.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <BRepBndLib.hxx>
#include <BRepTools.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <IntTools_FaceFace.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_SequenceOfPntOn2Faces.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_PntOn2Faces.hxx>
#include <IntTools_Tools.hxx>

#include <IntSurf_ListOfPntOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>

#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>

#include <BOPCol_MapOfInteger.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_DataMapOfShapeInteger.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPCol_IndexedMapOfInteger.hxx>

#include <BOPInt_Context.hxx>
#include <BOPInt_Tools.hxx>

#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_Point.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_VectorOfCurve.hxx>
#include <BOPDS_VectorOfPoint.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ListOfPave.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_CommonBlock.hxx>

#include <BOPAlgo_Tools.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopExp.hxx>
#include <BOPInt_ShrunkRange.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfPaveBlock.hxx>

static void ToleranceFF(const TopoDS_Face& aF1,
                        const TopoDS_Face& aF2,
                        Standard_Real& aTolFF);

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
  Standard_Boolean bToSplit;
  Standard_Integer nF1, nF2, aNbCurves, aNbPoints, iX, i, iP, iC, aNbLP;
  Standard_Real aApproxTol, aTolR3D, aTolR2D, aTolFF;
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aFFs.SetStartSize(iSize);
  aFFs.SetIncrement(iSize);
  aFFs.Init();
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
    IntTools_FaceFace aFaceFace;
    //
    IntSurf_ListOfPntOn2S aListOfPnts;
    GetEFPnts(nF1, nF2, aListOfPnts);
    aNbLP = aListOfPnts.Extent();
    if (aNbLP) {
      aFaceFace.SetList(aListOfPnts);
    }

    aFaceFace.SetParameters(bApp, bCompC2D1, bCompC2D2, aApproxTol);
    //
    aFaceFace.Perform(aF1, aF2);
    //
    bIsDone=aFaceFace.IsDone();
    if (bIsDone) {
      aTolR3D=aFaceFace.TolReached3d();
      aTolR2D=aFaceFace.TolReached2d();
      //
      ToleranceFF(aF1, aF2, aTolFF);
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
      myDS->AddInterf(nF1, nF2);
      //
      iX=aFFs.Append()-1;
      BOPDS_InterfFF& aFF=aFFs(iX);
      aFF.SetIndices(nF1, nF2);
      //
      aFF.SetTolR3D(aTolR3D);
      aFF.SetTolR2D(aTolR2D);
      //
      // Curves, Points 
      aFF.Init(aNbCurves, aNbPoints);
      //
      // Curves
      BOPDS_VectorOfCurve& aVNC=aFF.ChangeCurves();
      for (i=1; i<=aNbCurves; ++i) {
        Bnd_Box aBox;
        //
        const IntTools_Curve& aIC=aCvsX(i);
        const Handle(Geom_Curve)& aC3D= aIC.Curve();
        bValid=BOPInt_Tools::CheckCurve(aC3D, aTolR3D, aBox);
        if (bValid) {
          iC=aVNC.Append()-1;
          BOPDS_Curve& aNC=aVNC(iC);
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
        iP=aVNP.Append()-1;
        BOPDS_Point& aNP=aVNP(iP);
        aNP.SetPnt(aP);
      }
    //}// if (aNbCs || aNbPs)
    }// if (bIsDone) {
    else {// 904/L1
      iX=aFFs.Append()-1;
      BOPDS_InterfFF& aFF=aFFs(iX);
      aFF.SetIndices(nF1, nF2);
      aNbCurves=0;
      aNbPoints=0;
      aFF.Init(aNbCurves, aNbPoints);
    }
  }// for (; myIterator->More(); myIterator->Next()) {
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
  Standard_Real aTolR3D, aTolR2D, aT1, aT2;
  Handle(NCollection_IncAllocator) aAllocator;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  TopoDS_Edge aES;
  Handle(BOPDS_PaveBlock) aPBOut;
  //
  //-----------------------------------------------------scope f
  aAllocator=new NCollection_IncAllocator();
  //
  BOPCol_ListOfInteger aLSE(aAllocator);
  BOPCol_MapOfInteger aMVOnIn(100, aAllocator), aMF(100, aAllocator),
                      aMVStick(100,aAllocator), aMVEF(100, aAllocator),
                      aMVB(100, aAllocator);
  BOPDS_MapOfPaveBlock aMPBOnIn(100, aAllocator),
                       aMPBAdd(100, aAllocator);
  BOPDS_ListOfPaveBlock aLPB(aAllocator);
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks aMSCPB(100, aAllocator); 
  BOPCol_DataMapOfShapeInteger aMVI(100, aAllocator);
  BOPDS_DataMapOfPaveBlockListOfPaveBlock aDMExEdges;
  //
  for (i=0; i<aNbFF; ++i) {
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
    aTolR2D=aFF.TolR2D();
    //
    // Update face info
    if (aMF.Add(nF1)) {
      myDS->UpdateFaceInfoOn(nF1);
    }
    if (aMF.Add(nF2)) {
      myDS->UpdateFaceInfoOn(nF2);
    }
   
    BOPDS_FaceInfo& aFI1=myDS->ChangeFaceInfo(nF1);
    BOPDS_FaceInfo& aFI2=myDS->ChangeFaceInfo(nF2);
    //
    aMVOnIn.Clear();
    aMPBOnIn.Clear();
    aMVB.Clear();
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
    GetStickVertices(nF1, nF2, aMVStick, aMVEF);
    //
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVC.ChangeValue(j);
      const IntTools_Curve& aIC=aNC.Curve();
      // DEBf
      const Handle(Geom_Curve)& aC3D=aIC.Curve();
      // DEBt
      aNC.InitPaveBlock1();
      //
      PutPaveOnCurve(aMVOnIn, aTolR3D, aNC, nF1, nF2, aMVEF);
      //
      PutStickPavesOnCurve(nF1, nF2, aNC, aMVStick);
      //pkv/904/F7
      if (aNbC == 1) {
        PutEFPavesOnCurve(nF1, nF2, aNC, aMVEF);
      }
      //
      if (aIC.HasBounds()) {
        PutBoundPaveOnCurve(aF1, aF2, aTolR3D, aNC, aMVOnIn, aMVB);
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
        bValid2D=myContext->IsValidBlockForFaces(aT1, aT2, aIC, aF1, aF2, aTolR3D);
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
            if (!aPBOut->IsCommonBlock()) {
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
              PreparePostTreatFF(i, aPBOut, aMSCPB, aMVI, aVC);
            }
          }
          continue;
        }
        //
        // Make Edge
        const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
        const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
        //
        BOPTools_AlgoTools::MakeEdge (aIC, aV1, aT1, aV2, aT2, aTolR3D, aES);
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
      }
      //
      aLPBC.RemoveFirst();
    }//for (j=0; j<aNbC; ++j) {
    ProcessExistingPaveBlocks(i, aMPBOnIn, aMSCPB, aMVI, aMVB, aMPBAdd);
  }//for (i=0; i<aNbFF; ++i) {
  // 
  // post treatment
  myErrorStatus=PostTreatFF(aMSCPB, aMVI, aDMExEdges, aAllocator);
  if (myErrorStatus) {
    return;
  }
  //
  // update face info
  UpdateFaceInfo(aDMExEdges);
  //-----------------------------------------------------scope t
  aMF.Clear();
  aMVStick.Clear();
  aMPBOnIn.Clear();
  aMVOnIn.Clear();
  aDMExEdges.Clear();
  aAllocator.Nullify();
}

//=======================================================================
//function : PostTreatFF
//purpose  : 
//=======================================================================
  Standard_Integer BOPAlgo_PaveFiller::PostTreatFF
    (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
     BOPCol_DataMapOfShapeInteger& aMVI,
     BOPDS_DataMapOfPaveBlockListOfPaveBlock& aDMExEdges,
     Handle(NCollection_BaseAllocator)& theAllocator)
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
  Standard_Integer iErr, nSx, nVSD, iX, iP, iC, j, nV, iV, iE, k;
  Standard_Integer jx;
  Standard_Real aT;
  Standard_Integer aNbLPBx;
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
  // <-DEB f
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
  aPF.SetArguments(aLS);
  aPF.Perform();
  iErr=aPF.ErrorStatus();
  if (iErr) {
    iRet=1;
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
        } else {
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
          Handle(BOPDS_PaveBlock) aPBC=new BOPDS_PaveBlock();
          //
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
            const Handle(BOPDS_PaveBlock) aPBRx=aPBx->RealPaveBlock();
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
                if (!aMVI.IsBound(aV)) {// index of new vertex in theDS -> iV
                  aSI.SetShapeType(TopAbs_VERTEX);
                  aSI.SetShape(aV);
                  iV=myDS->Append(aSI);
                  aMVI.Bind(aV, iV);
                }
                else {
                  iV=aMVI.Find(aV);
                }
              }
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
  void BOPAlgo_PaveFiller::UpdateFaceInfo(
       BOPDS_DataMapOfPaveBlockListOfPaveBlock& theDME)
{
  Standard_Integer i, j, nV1, nF1, nF2, 
                   aNbFF, aNbC, aNbP, aNbS, aNbPBIn;
  BOPDS_IndexedMapOfPaveBlock aMPBCopy;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
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
    BOPDS_VectorOfCurve& aVNC=aFF.ChangeCurves();
    aNbC=aVNC.Extent();
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVNC(j);
      BOPDS_ListOfPaveBlock& aLPBC=aNC.ChangePaveBlocks();
      aItLPB.Initialize(aLPBC);
      //
      if (aItLPB.More() && theDME.IsBound(aLPBC.First())) {
        const Handle(BOPDS_PaveBlock)& aPB=aLPBC.First();
        BOPDS_ListOfPaveBlock& aLPB = theDME.ChangeFind(aPB);
        UpdateExistingPaveBlocks(aPB, aLPB, nF1, nF2);
        aLPBC.Clear();
        continue;
      }
      //
      for(; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB=aItLPB.Value();
        aFI1.ChangePaveBlocksSc().Add(aPB);
        aFI2.ChangePaveBlocksSc().Add(aPB);
      }
    }
    // VerticesSc
    const BOPDS_VectorOfPoint& aVNP=aFF.Points();
    aNbP=aVNP.Extent();
    for (j=0; j<aNbP; ++j) {
      const BOPDS_Point& aNP=aVNP(j);
      nV1=aNP.Index();
      aFI1.ChangeVerticesSc().Add(nV1);
      aFI2.ChangeVerticesSc().Add(nV1);
    }
  }
  //
  //2. PaveBlocksIn
  if (theDME.IsEmpty()) {
    return;
  }
  //
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    if (aSI.ShapeType()!=TopAbs_FACE) {
      continue;
    }
    //
    const TopoDS_Face& aF=(*(TopoDS_Face*)(&aSI.Shape()));
    //
    if(!myDS->HasFaceInfo(i)) {
      continue;
    }
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(i);
    //
    BOPDS_IndexedMapOfPaveBlock& aMPBIn=aFI.ChangePaveBlocksIn();
    aMPBCopy.Assign(aMPBIn);
    aMPBIn.Clear();
    //
    aNbPBIn=aMPBCopy.Extent();
    for (j=1; j<=aNbPBIn; ++j) {
      const Handle(BOPDS_PaveBlock)& aPB = aMPBCopy(j);
      if (theDME.IsBound(aPB)) {
        const BOPDS_ListOfPaveBlock& aLPB = theDME.Find(aPB);
        aItLPB.Initialize(aLPB);
        for (; aItLPB.More(); aItLPB.Next()) {
          const Handle(BOPDS_PaveBlock)& aPB1 = aItLPB.Value();
          aMPBIn.Add(aPB1);
        }
      } else {
        aMPBIn.Add(aPB);
      }
    }//for (j=1; j<=aNbPBIn; ++j) {
  }//for (i=0; i<aNbS; ++i) {
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
     const BOPDS_MapOfPaveBlock& theMPBOnIn,
     Handle(BOPDS_PaveBlock&) aPBOut)
{
  Standard_Boolean bRet;
  Standard_Real aT1, aT2, aTm, aTx;
  Standard_Integer nSp, iFlag, nV11, nV12, nV21, nV22;
  gp_Pnt aP1, aPm, aP2;
  Bnd_Box aBoxP1, aBoxPm, aBoxP2;
  BOPDS_MapIteratorOfMapOfPaveBlock aIt;
  //
  bRet=Standard_True;
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
  aIt.Initialize(theMPBOnIn);
  for (; aIt.More(); aIt.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB=aIt.Value();
    aPB->Indices(nV21, nV22);
    nSp=aPB->Edge();
    const BOPDS_ShapeInfo& aSISp=myDS->ChangeShapeInfo(nSp);
    const Bnd_Box& aBoxSp=aSISp.Box();
    if (!aBoxSp.IsOut(aBoxP1) && !aBoxSp.IsOut(aBoxPm) && !aBoxSp.IsOut(aBoxP2)) {
      const TopoDS_Edge& aSp=(*(TopoDS_Edge *)(&aSISp.Shape()));
      iFlag=(nV11 == nV21 || nV11 == nV22) ? 0 : 
                     myContext->ComputePE(aP1, theTolR3D, aSp, aTx);
      if (!iFlag) {
        iFlag=(nV12 == nV21 || nV12 == nV22) ? 0 :
                       myContext->ComputePE(aP2, theTolR3D, aSp, aTx);
        if (!iFlag) {
          iFlag=myContext->ComputePE(aPm, theTolR3D, aSp, aTx);
          if (!iFlag) {
            aPBOut = aPB;
            return bRet;
          }
        }
      }
    }
  }
  return !bRet;
}

//=======================================================================
//function : PutBoundPaveOnCurve
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutBoundPaveOnCurve(const TopoDS_Face& aF1,
                                               const TopoDS_Face& aF2,
                                               const Standard_Real aTolR3D,
                                               BOPDS_Curve& aNC,
                                               BOPCol_MapOfInteger& aMVOnIn,
                                               BOPCol_MapOfInteger& aMVB)
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
      aMVOnIn.Add(nVn);
      aMVB.Add(nVn);
    }
  }
}

//=======================================================================
//function : PutPaveOnCurve
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutPaveOnCurve(const BOPCol_MapOfInteger& aMVOnIn,
                                          const Standard_Real aTolR3D,
                                          BOPDS_Curve& aNC,
                                          const Standard_Integer nF1,
                                          const Standard_Integer nF2,
                                          const BOPCol_MapOfInteger& aMVEF)
{
  Standard_Boolean bIsVertexOnLine, bInBothFaces;
  Standard_Integer nV;
  Standard_Real aT;
  BOPDS_Pave aPave;
  //
  BOPCol_MapIteratorOfMapOfInteger aIt;
  //
  Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
  const IntTools_Curve& aIC=aNC.Curve();
  const Bnd_Box& aBoxC=aNC.Box();
  //
  aIt.Initialize(aMVOnIn);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    const BOPDS_ShapeInfo& aSIV=myDS->ShapeInfo(nV);
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&aSIV.Shape()));
    //
    if (!aMVEF.Contains(nV)) {
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
    }
    //
    bIsVertexOnLine=myContext->IsVertexOnLine(aV, aIC, aTolR3D, aT);
    if (!bIsVertexOnLine) {
      Standard_Real aTolVExt;
      BOPCol_MapOfInteger aMI;
      //
      aTolVExt = BRep_Tool::Tolerance(aV);
      //
      GetFullFaceMap(nF1, aMI);
      GetFullFaceMap(nF2, aMI);
      //
      ExtendedTolerance(nV, aMI, aTolVExt);
      bIsVertexOnLine=myContext->IsVertexOnLine(aV, aTolVExt, aIC, aTolR3D, aT);
    }   
    //
    if (bIsVertexOnLine) {
      aPave.SetIndex(nV);
      aPave.SetParameter(aT);
      //
      aPB->AppendExtPave(aPave);
      //
      BOPTools_AlgoTools::UpdateVertex (aIC, aT, aV);
      // 
      BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nV);
      Bnd_Box& aBoxDS=aSIDS.ChangeBox();
      BRepBndLib::Add(aV, aBoxDS);
    }
  }
}

//=======================================================================
//function : ExtendedTolerance
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_PaveFiller::ExtendedTolerance(const Standard_Integer nV,
                                                         const BOPCol_MapOfInteger& aMI,
                                                         Standard_Real& aTolVExt)
{
  Standard_Boolean bFound = Standard_False;
  if (!(myDS->IsNewShape(nV))) {
    return bFound;
  }

  Standard_Integer i, k, aNbLines;
  Standard_Real aT11, aT12, aD1, aD2, aD;
  TopoDS_Vertex aV;
  gp_Pnt aPV, aP11, aP12;

  aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV)));
  aPV=BRep_Tool::Pnt(aV);
  //
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();

  for (k=0; k<2; ++k) {
    aNbLines = !k ? aEEs.Extent() : aEFs.Extent();
    for (i = 0; i < aNbLines; ++i) {
      BOPDS_Interf *aInt = !k ? (BOPDS_Interf*) (&aEEs(i)) :
                                (BOPDS_Interf*) (&aEFs(i));
      if (aInt->IndexNew() == nV) {
        if (aMI.Contains(aInt->Index1()) && aMI.Contains(aInt->Index2())) {
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
  BOPCol_MapOfInteger aMIF1, aMIF2;
  //
  //collect indexes of all shapes from nF1 and nF2.
  GetFullFaceMap(nF1, aMIF1);
  GetFullFaceMap(nF2, aMIF2);
  //
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  aNbEFs = aEFs.Extent();
  //
  for(i = 0; i < aNbEFs; ++i) {
    const BOPDS_InterfEF& aEF = aEFs(i);
    nE = aEF.Index1();
    nFOpposite = aEF.Index2();
    if(aMIF1.Contains(nE) && aMIF2.Contains(nFOpposite) ||
       aMIF1.Contains(nFOpposite) && aMIF2.Contains(nE)) {
      //
      IntTools_CommonPrt aCP = aEF.CommonPart();
      if(aCP.Type() == TopAbs_VERTEX) {
        Standard_Real aPar = aCP.VertexParameter1();
        const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&myDS->Shape(nE)));
        const TopoDS_Face& aFOpposite = (*(TopoDS_Face*)(&myDS->Shape(nFOpposite)));
        //
        const Handle(Geom_Curve)& aCurve = BRep_Tool::Curve(aE, f, l);
        //
        nF = (nFOpposite == nF1) ? nF2 : nF1;
        const TopoDS_Face& aF = (*(TopoDS_Face*)(&myDS->Shape(nF)));
        Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(aE, aF, f, l);
        //
        GeomAPI_ProjectPointOnSurf& aProj = myContext->ProjPS(aFOpposite);
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
  void BOPAlgo_PaveFiller::PutEFPavesOnCurve(const Standard_Integer nF1,
                                             const Standard_Integer nF2,
                                             BOPDS_Curve& aNC,
                                             const BOPCol_MapOfInteger& aMVEF)
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
  Handle(BOPDS_PaveBlock)& aPB = aNC.ChangePaveBlock1();

  GeomAPI_ProjectPointOnCurve& aProjPT = myContext->ProjPT(aC3D);
  
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
      PutPaveOnCurve(nV, aDist, aNC, aPB);
    }
  }
}

//=======================================================================
//function : ProcessUnUsedVertices
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutStickPavesOnCurve(const Standard_Integer nF1,
                                                const Standard_Integer nF2,
                                                BOPDS_Curve& aNC,
                                                const BOPCol_MapOfInteger& aMVStick)
{
  BOPCol_MapOfInteger aMV;
  aMV.Assign(aMVStick);
  RemoveUsedVertices(aNC, aMV);
  //
  if (!aMV.Extent()) {
    return;
  }
  //
  GeomAbs_SurfaceType aType1, aType2;
  const TopoDS_Face& aF1 = (*(TopoDS_Face*)(&myDS->Shape(nF1)));  
  const TopoDS_Face& aF2 = (*(TopoDS_Face*)(&myDS->Shape(nF2)));  
  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  Handle(Geom_Surface) aS2=BRep_Tool::Surface(aF2);
  GeomAdaptor_Surface aGAS1(aS1);
  GeomAdaptor_Surface aGAS2(aS2);
  
  //
  aType1=aGAS1.GetType();
  aType2=aGAS2.GetType();
  //
  if(aType1==GeomAbs_Torus  || aType2==GeomAbs_Torus) {
    Standard_Integer nV, m, n;
    Standard_Real aTC[2], aD, aD2, u, v, aDT2, aScPr, aDScPr;
    GeomAbs_CurveType aTypeC;
    gp_Pnt aPC[2], aPV;
    gp_Dir aDN[2];
    gp_Pnt2d aP2D;
    
    Handle(Geom2d_Curve) aC2D[2];
    //
    aDT2=2e-7;     // the rich criteria
    aDScPr=5.e-9;  // the creasing criteria
    //
    const IntTools_Curve& aIC=aNC.Curve();
    //Handle(Geom_Curve) aC3D=aIC.Curve(); //DEB
    aTypeC=aIC.Type();
    if (aTypeC==GeomAbs_BezierCurve || aTypeC==GeomAbs_BSplineCurve) {
      //
      aIC.Bounds(aTC[0], aTC[1], aPC[0], aPC[1]);
      aC2D[0]=aIC.FirstCurve2d();
      aC2D[1]=aIC.SecondCurve2d();
      if (!aC2D[0].IsNull() && !aC2D[1].IsNull()) {
        BOPCol_MapIteratorOfMapOfInteger aItMI, aItMI1;
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
            Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
            PutPaveOnCurve(nV, aD, aNC, aPB);
          }
        }//for (jVU=1; jVU=aNbVU; ++jVU) {
      }
    }//if (aTypeC==GeomAbs_BezierCurve || aTypeC==GeomAbs_BSplineCurve) {
  }//if(aType1==GeomAbs_Torus  || aType2==GeomAbs_Torus) {
}

//=======================================================================
//function : GetStickVertices
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::GetStickVertices(const Standard_Integer nF1,
                                            const Standard_Integer nF2,
                                            BOPCol_MapOfInteger& aMVStick,
                                            BOPCol_MapOfInteger& aMVEF)
{
  BOPCol_MapOfInteger aMIF1, aMIF2;
  Standard_Integer nV1, nV2, nE1, nE2, nV, nE, nF, nVNew, i;
  //
  BOPDS_VectorOfInterfVV& aVVs=myDS->InterfVV();
  BOPDS_VectorOfInterfVE& aVEs=myDS->InterfVE();
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  //
  Standard_Integer aNbVVs, aNbVEs, aNbEEs, aNbVFs, aNbEFs;
  aNbVVs = aVVs.Extent();
  aNbVEs = aVEs.Extent();
  aNbEEs = aEEs.Extent();
  aNbVFs = aVFs.Extent();
  aNbEFs = aEFs.Extent();
  //
  //collect indexes of all shapes from nF1 and nF2.
  GetFullFaceMap(nF1, aMIF1);
  GetFullFaceMap(nF2, aMIF2);
  //collect VV interferences
  for(i = 0; i < aNbVVs; ++i) {
    const BOPDS_InterfVV& aVV = aVVs(i);
    nV1 = aVV.Index1();
    nV2 = aVV.Index2();
    if(aMIF1.Contains(nV1) && aMIF2.Contains(nV2) ||
       aMIF1.Contains(nV2) && aMIF2.Contains(nV1)) {
      if (aVV.HasIndexNew()) {
        nVNew = aVV.IndexNew();
        aMVStick.Add(nVNew);
      }
    }
  }
  //collect VE interferences
  for(i = 0; i < aNbVEs; ++i) {
    const BOPDS_InterfVE& aVE = aVEs(i);
    nV = aVE.Index1();
    nE = aVE.Index2();
    if(aMIF1.Contains(nV) && aMIF2.Contains(nE) ||
       aMIF1.Contains(nE) && aMIF2.Contains(nV)) {
      aMVStick.Add(nV);
    }
  }
  //collect EE interferences
  for(i = 0; i < aNbEEs; ++i) {
    const BOPDS_InterfEE& aEE = aEEs(i);
    nE1 = aEE.Index1();
    nE2 = aEE.Index2();
    if(aMIF1.Contains(nE1) && aMIF2.Contains(nE2) ||
       aMIF1.Contains(nE2) && aMIF2.Contains(nE1)) {
      IntTools_CommonPrt aCP = aEE.CommonPart();
      if(aCP.Type() == TopAbs_VERTEX) {
        nVNew = aEE.IndexNew();
        aMVStick.Add(nVNew);
      }
    }
  }
  //collect VF interferences
  for(i = 0; i < aNbVFs; ++i) {
    const BOPDS_InterfVF& aVF = aVFs(i);
    nV = aVF.Index1();
    nF = aVF.Index2();
    if(aMIF1.Contains(nV) && aMIF2.Contains(nF) ||
       aMIF1.Contains(nF) && aMIF2.Contains(nV)) {
      aMVStick.Add(nV);
    }
  }
  //collect EF interferences
  for(i = 0; i < aNbEFs; ++i) {
    const BOPDS_InterfEF& aEF = aEFs(i);
    nE = aEF.Index1();
    nF = aEF.Index2();
    if(aMIF1.Contains(nE) && aMIF2.Contains(nF) ||
       aMIF1.Contains(nF) && aMIF2.Contains(nE)) {
      IntTools_CommonPrt aCP = aEF.CommonPart();
      if(aCP.Type() == TopAbs_VERTEX) {
        nVNew = aEF.IndexNew();
        aMVStick.Add(nVNew);
        aMVEF.Add(nVNew);
      }
    }
  }
}

//=======================================================================
// function: GetFullFaceMap
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::GetFullFaceMap(const Standard_Integer nF,
                                        BOPCol_MapOfInteger& aMI)
{
  Standard_Integer nV, nE;
  TopoDS_Edge aE;
  TopoDS_Vertex aV;
  //face
  aMI.Add(nF);
  //edges
  const TopoDS_Face& aF = (*(TopoDS_Face*)(&myDS->Shape(nF)));
  TopExp_Explorer anExp(aF, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    aE = (*(TopoDS_Edge *)(&anExp.Current()));
    nE = myDS->Index(aE);
    aMI.Add(nE);
  }
  //vertices
  TopExp_Explorer anExpV(aF, TopAbs_VERTEX);
  for (; anExpV.More(); anExpV.Next()) {
    aV = (*(TopoDS_Vertex *)(&anExpV.Current()));
    nV = myDS->Index(aV);
    aMI.Add(nV);
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
  void BOPAlgo_PaveFiller::PutPaveOnCurve(const Standard_Integer nV,
                                          const Standard_Real aTolR3D,
                                          const BOPDS_Curve& aNC, 
                                          Handle(BOPDS_PaveBlock)& aPB)
{
  Standard_Boolean bIsVertexOnLine;
  Standard_Real aT;
  BOPDS_Pave aPave;
  //
  const TopoDS_Vertex aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV)));
  const IntTools_Curve& aIC = aNC.Curve();
  //
  bIsVertexOnLine=myContext->IsVertexOnLine(aV, aIC, aTolR3D, aT);
  if (bIsVertexOnLine) {
    aPave.SetIndex(nV);
    aPave.SetParameter(aT);
    //
    aPB->AppendExtPave(aPave);
    //
    BOPTools_AlgoTools::UpdateVertex (aIC, aT, aV);
    // 
    BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nV);
    Bnd_Box& aBoxDS=aSIDS.ChangeBox();
    BRepBndLib::Add(aV, aBoxDS);
  }
}

//=======================================================================
//function : ProcessOldPaveBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::ProcessExistingPaveBlocks
    (const Standard_Integer theInt,
     const BOPDS_MapOfPaveBlock& aMPBOnIn,
     BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& aMSCPB,
     BOPCol_DataMapOfShapeInteger& aMVI,
     const BOPCol_MapOfInteger& aMVB,
     BOPDS_MapOfPaveBlock& aMPB)
{
  Standard_Integer nV, nE, iFlag;
  Standard_Real aT;
  BOPCol_MapIteratorOfMapOfInteger aItB;
  BOPDS_MapIteratorOfMapOfPaveBlock aItPB;
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  BOPDS_InterfFF& aFF = aFFs(theInt);
  BOPDS_VectorOfCurve& aVC=aFF.ChangeCurves();
  //  
  aItB.Initialize(aMVB);
  for (; aItB.More(); aItB.Next()) {
    nV = aItB.Value();
    const BOPDS_ShapeInfo& aSIV=myDS->ShapeInfo(nV);
    const Bnd_Box& aBoxV=aSIV.Box();
    const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&aSIV.Shape();
    if (!aMVI.IsBound(aV)) {
      continue;
    }
    //
    aItPB.Initialize(aMPBOnIn);
    for (; aItPB.More(); aItPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
      if (aPB->Pave1().Index() == nV || aPB->Pave2().Index() == nV) {
        continue;
      }
      //
      if (aMPB.Contains(aPB)) {
        continue;
      }
      nE=aPB->Edge();
      const BOPDS_ShapeInfo& aSIE=myDS->ShapeInfo(nE);
      const Bnd_Box& aBoxE=aSIE.Box();
      //
      if (!aBoxV.IsOut(aBoxE)) {
        const TopoDS_Edge& aE = *(TopoDS_Edge*)&aSIE.Shape();
        //
        iFlag=myContext->ComputeVE (aV, aE, aT);
        if (!iFlag) {
          aMPB.Add(aPB);
          //
          PreparePostTreatFF(theInt, aPB, aMSCPB, aMVI, aVC);
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
  BOPDS_IndexedMapOfPaveBlock aMPB;
  //
  //remove micro edges from aLPB
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
  //update face info
  myDS->UpdateFaceInfoOn(nF1);
  //
  myDS->UpdateFaceInfoOn(nF2);
  //
  BOPDS_FaceInfo& aFI1 = myDS->ChangeFaceInfo(nF1);
  BOPDS_FaceInfo& aFI2 = myDS->ChangeFaceInfo(nF2);
  //
  BOPDS_IndexedMapOfPaveBlock& aMPBOn1 = aFI1.ChangePaveBlocksOn();
  BOPDS_IndexedMapOfPaveBlock& aMPBIn1 = aFI1.ChangePaveBlocksIn();
  BOPDS_IndexedMapOfPaveBlock& aMPBOn2 = aFI2.ChangePaveBlocksOn();
  BOPDS_IndexedMapOfPaveBlock& aMPBIn2 = aFI2.ChangePaveBlocksIn();
  //
  // remove old pave blocks
  const Handle(BOPDS_CommonBlock)& aCB1 = aPBf->CommonBlock();
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
  if (bCB) {
    //create new pave blocks
    const BOPCol_ListOfInteger& aFaces = aCB1->Faces();
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next()) {
      Handle(BOPDS_PaveBlock)& aPB = aIt.ChangeValue();
      //
      aCB = new BOPDS_CommonBlock;
      aIt1.Initialize(aLPB1);
      for (; aIt1.More(); aIt1.Next()) {
        aPB2 = aIt1.Value();
        nE = aPB2->OriginalEdge();
        //
        aPB2n = new BOPDS_PaveBlock;
        aPB2n->SetPave1(aPB->Pave1());
        aPB2n->SetPave2(aPB->Pave2());
        aPB2n->SetEdge(aPB->Edge());
        aPB2n->SetOriginalEdge(nE);
        aCB->AddPaveBlock(aPB2n);
        aPB2n->SetCommonBlock(aCB);
        myDS->ChangePaveBlocks(nE).Append(aPB2n);
      }
      aCB->AddFaces(aFaces);
      myDS->SortPaveBlocks(aCB);
      //
      aPB=aCB->PaveBlocks().First();
    }
  } 
  //
  aIt.Initialize(aLPB);
  for (; aIt.More(); aIt.Next()) {
    Handle(BOPDS_PaveBlock)& aPB = aIt.ChangeValue();
    nE = aPB->OriginalEdge();
    //
    Standard_Integer nF = (aMPBOn1.Contains(aPBf) || 
                           aMPBIn1.Contains(aPBf)) ? nF2 : nF1;
    const TopoDS_Face& aF = *(TopoDS_Face*)&myDS->Shape(nF);
    IntTools_Range aShrR(aPB->Pave1().Parameter(), aPB->Pave2().Parameter());
    const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(aPB->Edge());
    //
    Standard_Boolean bCom = BOPTools_AlgoTools::IsBlockInOnFace(aShrR, aF, aE, myContext);
    if (bCom) {
      if (bCB) {
        aCB = aPB->CommonBlock();
        aCB->AddFace(nF);
      } else {
        aCB = new BOPDS_CommonBlock;
        aCB->AddPaveBlock(aPB);
        aCB->AddFace(nF1);
        aCB->AddFace(nF2);
        //
        aPB->SetCommonBlock(aCB);
      }
      aMPB.Add(aPB);
    }
    if (!bCB) {
      myDS->ChangePaveBlocks(nE).Append(aPB);
    }
  }
  //
  Standard_Integer i, aNbPB;
  Standard_Boolean bIn1, bIn2;
  //
  bIn1 = aMPBOn1.Contains(aPBf) || aMPBIn1.Contains(aPBf);
  bIn2 = aMPBOn2.Contains(aPBf) || aMPBIn2.Contains(aPBf);
  //
  aNbPB=aMPB.Extent();
  for (i=1; i<=aNbPB; ++i) {
    aPB = aMPB(i);
    if (!bIn1) {
      aMPBIn1.Add(aPB);
    }
    //
    if (!bIn2) {
      aMPBIn2.Add(aPB);
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
     const Handle(BOPDS_PaveBlock)& aPB,
     BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& aMSCPB,
     BOPCol_DataMapOfShapeInteger& aMVI,
     BOPDS_VectorOfCurve& aVC) 
{
  Standard_Integer nV1, nV2;
  //
  Standard_Integer iC=aVC.Append()-1;
  BOPDS_ListOfPaveBlock& aLPBC = aVC(iC).ChangePaveBlocks();
  aLPBC.Append(aPB);
  //
  aPB->Indices(nV1, nV2);
  const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
  const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
  const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(aPB->Edge());
  // Keep info for post treatment 
  BOPDS_CoupleOfPaveBlocks aCPB;
  aCPB.SetIndexInterf(aInt);
  aCPB.SetIndex(iC);
  aCPB.SetPaveBlock1(aPB);
  //
  aMSCPB.Add(aE, aCPB);
  aMVI.Bind(aV1, nV1);
  aMVI.Bind(aV2, nV2);
}

//=======================================================================
//function : ToleranceFF
//purpose  : Computes the TolFF according to the tolerance value and 
//           types of the faces.
//=======================================================================
  void ToleranceFF(const TopoDS_Face& aF1,
                   const TopoDS_Face& aF2,
                   Standard_Real& aTolFF)
{
  Standard_Real aTol1, aTol2;
  Standard_Boolean isAna1, isAna2;
  //
  aTol1 = BRep_Tool::Tolerance(aF1);
  aTol2 = BRep_Tool::Tolerance(aF2);
  aTolFF = Max(aTol1, aTol2);
  //
  BRepAdaptor_Surface BAS1(aF1);
  BRepAdaptor_Surface BAS2(aF2);
  //
  isAna1 = (BAS1.GetType() == GeomAbs_Plane ||
            BAS1.GetType() == GeomAbs_Cylinder ||
            BAS1.GetType() == GeomAbs_Cone ||
            BAS1.GetType() == GeomAbs_Sphere ||
            BAS1.GetType() == GeomAbs_Torus);
  //
  isAna2 = (BAS2.GetType() == GeomAbs_Plane ||
            BAS2.GetType() == GeomAbs_Cylinder ||
            BAS2.GetType() == GeomAbs_Cone ||
            BAS2.GetType() == GeomAbs_Sphere ||
            BAS2.GetType() == GeomAbs_Torus);
  //
  aTolFF = (isAna1 && isAna2) ? aTolFF : Max(aTolFF, 5.e-6);
}


// DEB f
  /*
  {
    BOPDS_DataMapIteratorOfDataMapOfShapeCoupleOfPaveBlocks aItx;
    TopoDS_Compound aCx;
    //
    BRep_Builder aBBx;
    aBBx.MakeCompound(aCx);
    //
    aItx.Initialize(theMSCPB);
    for (; aItx.More(); aItx.Next()) {
      const TopoDS_Shape& aSx=aItx.Key();
      aBBx.Add(aCx, aSx);
    }
    int a=0;
    BRepTools::Write(aCx, "cx");
  }
  */
  // DEB t
