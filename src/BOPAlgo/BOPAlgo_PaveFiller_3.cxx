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
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_VectorOfInterfEE.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Parallel.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BRep_Builder.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <IntTools_Tools.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Vector.hxx>
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
  public BOPAlgo_ParallelAlgo {
 
 public:

  DEFINE_STANDARD_ALLOC
  //
  BOPAlgo_EdgeEdge(): 
    IntTools_EdgeEdge(),
    BOPAlgo_ParallelAlgo() {
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
  void SetBoxes (const Bnd_Box& theBox1,
                 const Bnd_Box& theBox2)
  {
    myBox1 = theBox1;
    myBox2 = theBox2;
  }
  //
  void SetFuzzyValue(const Standard_Real theFuzz) {
    IntTools_EdgeEdge::SetFuzzyValue(theFuzz);
  }
  //
  virtual void Perform() {
    Message_ProgressScope aPS(myProgressRange, NULL, 1);
    if (UserBreak(aPS))
    {
      return;
    }
    TopoDS_Edge anE1 = myEdge1, anE2 = myEdge2;
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
        myEdge1.Move (aLoc);
        myEdge2.Move (aLoc);
        hasTrsf = Standard_True;
      }

      IntTools_EdgeEdge::Perform();
    }
    catch (Standard_Failure const&)
    {
      AddError(new BOPAlgo_AlertIntersectionFailed);
    }

    myEdge1 = anE1;
    myEdge2 = anE2;
    if (hasTrsf)
    {
      for (Standard_Integer i = 1; i <= myCommonParts.Length(); ++i)
      {
        IntTools_CommonPrt& aCPart = myCommonParts (i);
        aCPart.SetEdge1 (myEdge1);
        aCPart.SetEdge2 (myEdge2);
      }
    }

  }
  //
 protected:
  Handle(BOPDS_PaveBlock) myPB1;
  Handle(BOPDS_PaveBlock) myPB2;
  Bnd_Box myBox1;
  Bnd_Box myBox2;
};
//
//=======================================================================
typedef NCollection_Vector<BOPAlgo_EdgeEdge> BOPAlgo_VectorOfEdgeEdge;

//=======================================================================
// function: PerformEE
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformEE(const Message_ProgressRange& theRange)
{
  FillShrunkData(TopAbs_EDGE, TopAbs_EDGE);
  //
  myIterator->Initialize(TopAbs_EDGE, TopAbs_EDGE);
  Standard_Integer iSize = myIterator->ExpectedLength();
  Message_ProgressScope aPSOuter(theRange, NULL, 10);
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
  TColStd_MapOfInteger aMEdges;
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
    if (UserBreak(aPSOuter))
    {
      return;
    }
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
      if (UserBreak(aPSOuter))
      {
        return;
      }
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
        BOPAlgo_EdgeEdge& anEdgeEdge=aVEdgeEdge.Appended();
        //
        anEdgeEdge.UseQuickCoincidenceCheck(bExpressCompute);
        //
        anEdgeEdge.SetPaveBlock1(aPB1);
        anEdgeEdge.SetPaveBlock2(aPB2);
        //
        anEdgeEdge.SetEdge1(aE1, aT11, aT12);
        anEdgeEdge.SetEdge2(aE2, aT21, aT22);
        anEdgeEdge.SetBoxes (aBB1, aBB2);
        anEdgeEdge.SetFuzzyValue(myFuzzyValue);
      }//for (; aIt2.More(); aIt2.Next()) {
    }//for (; aIt1.More(); aIt1.Next()) {
  }//for (; myIterator->More(); myIterator->Next()) {
  //
  aNbEdgeEdge=aVEdgeEdge.Length();

  Message_ProgressScope aPS(aPSOuter.Next(9), "Performing Edge-edge intersection", aNbEdgeEdge);
  for (k = 0; k < aNbEdgeEdge; k++)
  {
    BOPAlgo_EdgeEdge& anEdgeEdge = aVEdgeEdge.ChangeValue(k);
    anEdgeEdge.SetProgressRange(aPS.Next());
  }
  //======================================================
  BOPTools_Parallel::Perform (myRunParallel, aVEdgeEdge);
  //======================================================
  if (UserBreak(aPSOuter))
  {
    return;
  }
  //
  for (k = 0; k < aNbEdgeEdge; ++k) {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    Bnd_Box aBB1, aBB2;
    //
    BOPAlgo_EdgeEdge& anEdgeEdge=aVEdgeEdge(k);
    if (!anEdgeEdge.IsDone() || anEdgeEdge.HasErrors()) {
      // Warn about failed intersection of sub-shapes
      const TopoDS_Shape& aE1 = myDS->Shape(anEdgeEdge.PaveBlock1()->OriginalEdge());
      const TopoDS_Shape& aE2 = myDS->Shape(anEdgeEdge.PaveBlock2()->OriginalEdge());
      AddIntersectionFailedWarning(aE1, aE2);
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
      if (UserBreak(aPSOuter))
      {
        return;
      }
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
          Standard_Boolean bIsOnPave[4];
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
          Standard_Boolean isVExists = Standard_False;
          for (j = 0; j < 4; ++j)
          {
            if (bIsOnPave[j])
            {
              Handle(BOPDS_PaveBlock)& aPB = (j < 2) ? aPB2 : aPB1;
              bIsOnPave[j] = ForceInterfVE(nV[j], aPB, aMEdges);
              if (bIsOnPave[j]) isVExists = Standard_True;
            }
          }

          BOPTools_AlgoTools::MakeNewVertex(aE1, aT1, aE2, aT2, aVnew);
          const gp_Pnt aPnew = BRep_Tool::Pnt(aVnew);

          if (isVExists)
          {
            // The found intersection point is located closely to one of the
            // pave blocks bounds. So, do not create the new vertex in this point.
            // Check if this point is a real intersection point or just a touching point.
            // If it is a touching point, do nothing.
            // If it is an intersection point, update the existing vertex to cover the
            // intersection point.
            const gp_Pnt aPOnE1 = BRepAdaptor_Curve(aE1).Value(aT1);
            const gp_Pnt aPOnE2 = BRepAdaptor_Curve(aE2).Value(aT2);
            if (aPOnE1.Distance(aPOnE2) > Precision::Intersection())
              // No intersection point
              continue;

            // Real intersection is present.
            // Update the existing vertex to cover the intersection point.
            for (j = 0; j < 4; ++j)
            {
              if (bIsOnPave[j])
              {
                const TopoDS_Vertex& aV = TopoDS::Vertex(myDS->Shape(nV[j]));
                const gp_Pnt aP = BRep_Tool::Pnt(aV);
                Standard_Real aDistPP = aP.Distance(aPnew);
                // Just update the vertex
                UpdateVertex(nV[j], aDistPP);
                myVertsToAvoidExtension.Add(nV[j]);
              }
            }
          }

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
            TColStd_MapOfInteger aMV;
            gp_Pnt aPx;
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
          BOPDS_InterfEE& aEE=aEEs.Appended();
          iX=aEEs.Length()-1;
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
          BOPDS_InterfEE& aEE=aEEs.Appended();
          iX=aEEs.Length()-1;
          aEE.SetIndices(nE1, nE2);
          aEE.SetCommonPart(aCPart);
          // 2
          myDS->AddInterf(nE1, nE2);
          //
          BOPAlgo_Tools::FillMap<Handle(BOPDS_PaveBlock)>(aPB1, aPB2, aMPBLPB, aAllocator);
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
  BOPAlgo_Tools::PerformCommonBlocks(aMPBLPB, aAllocator, myDS, myContext);
  // Update vertices of common blocks with real CB tolerances
  UpdateVerticesOfCB();

  PerformNewVertices(aMVCPB, aAllocator, aPSOuter.Next());
  if (HasErrors())
  {
    return;
  }
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
   const Message_ProgressRange& theRange,
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
  TopTools_IndexedDataMapOfShapeListOfShape aImages;
  TreatNewVertices(theMVCPB, aImages);
  //
  // 2. Add new vertices to myDS and connect indices to CPB structure
  BOPDS_VectorOfInterfEE& aEEs = myDS->InterfEE();
  BOPDS_VectorOfInterfEF& aEFs = myDS->InterfEF();
  //
    // 4. Compute Extra Paves and split Pave blocks by the Extra paves
  Message_ProgressScope aPS(theRange, NULL, 2);
  Standard_Integer i, aNb = aImages.Extent();
  Message_ProgressScope aPS1(aPS.Next(), NULL, aNb + aNbV);
  for (i = 1; i <= aNb; ++i, aPS1.Next()) {
    if (UserBreak(aPS))
    {
      return;
    }
    const TopoDS_Vertex& aV = TopoDS::Vertex(aImages.FindKey(i));
    const TopTools_ListOfShape& aLVSD = aImages.FindFromIndex(i);
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
    TopTools_ListIteratorOfListOfShape aItLS(aLVSD);
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
  for (i = 1; i <= aNbV; ++i, aPS1.Next()) {
    if (UserBreak(aPS))
    {
      return;
    }
    const BOPDS_CoupleOfPaveBlocks& aCPB = theMVCPB.FindFromIndex(i);
    Standard_Integer iV = aCPB.Index();
    //
    Handle(BOPDS_PaveBlock) aPB[2];
    aCPB.PaveBlocks(aPB[0], aPB[1]);
    for (Standard_Integer j = 0; j < 2; ++j) {
      TColStd_ListOfInteger *pLI = aMPBLI.ChangeSeek(aPB[j]);
      if (!pLI) {
        pLI = &aMPBLI(aMPBLI.Add(aPB[j], TColStd_ListOfInteger(theAllocator)));
      }
      pLI->Append(iV);
      //
      if (aPB[0] == aPB[1]) {
        break;
      }
    }
  }
  // 4. Compute Extra Paves and split Pave blocks by the Extra paves
  IntersectVE(aMPBLI, aPS.Next(), Standard_False);
}
//=======================================================================
//function : TreatNewVertices
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::TreatNewVertices
  (const BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB,
   TopTools_IndexedDataMapOfShapeListOfShape& myImages)
{
  //
  // Prepare for intersection
  TopTools_IndexedDataMapOfShapeReal aVerts;
  Standard_Integer i, aNbV = theMVCPB.Extent();
  for (i = 1; i <= aNbV; ++i) {
    const TopoDS_Shape& aV = theMVCPB.FindKey(i);
    Standard_Real aTol = theMVCPB.FindFromIndex(i).Tolerance();
    aVerts.Add(aV, aTol);
  }
  //
  // Perform intersection
  TopTools_ListOfListOfShape aChains;
  BOPAlgo_Tools::IntersectVertices(aVerts, myFuzzyValue, aChains);
  //
  // Treat the results - make new vertices for each chain
  TopTools_ListOfListOfShape::Iterator aItC(aChains);
  for (; aItC.More(); aItC.Next()) {
    const TopTools_ListOfShape& aLVSD = aItC.Value();
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

  if (nV1 < 0 || nV2 < 0)
  {
    return;
  }

  const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1))); 
  const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2))); 
  // Get the edge
  Standard_Integer nE = -1;
  if (!thePB->HasEdge(nE))
  {
    nE = thePB->OriginalEdge();
    if (nE < 0)
      return;
  }

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
    if (bWholeEdge && thePB->OriginalEdge() >= 0) {
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
      Standard_Real aTS1, aTS2;
      theSR.ShrunkRange(aTS1, aTS2);
      thePB->SetShrunkData(aTS1, aTS2, Bnd_Box(), Standard_False);
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
Standard_Boolean BOPAlgo_PaveFiller::ForceInterfVE(const Standard_Integer nV,
                                                   Handle(BOPDS_PaveBlock)& aPB,
                                                   TColStd_MapOfInteger& theMEdges)
{
  Standard_Integer nE, nVx, nVSD, iFlag;
  Standard_Real aT, aTolVNew;
  //
  nE = aPB->OriginalEdge();
  //
  const BOPDS_ShapeInfo& aSIE=myDS->ShapeInfo(nE);
  if (aSIE.HasSubShape(nV)) {
    return Standard_True;
  }
  //
  if (myDS->HasInterf(nV, nE)) {
    return Standard_True;
  }
  //
  if (myDS->HasInterfShapeSubShapes(nV, nE)) {
    return Standard_True;
  }
  //
  if (aPB->Pave1().Index() == nV || 
      aPB->Pave2().Index() == nV) {
    return Standard_True;
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
    BOPDS_InterfVE& aVE=aVEs.Appended();
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
    return Standard_True;
  }
  return Standard_False;
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

//=======================================================================
//function : UpdateVerticesOfCB
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::UpdateVerticesOfCB()
{
  // Fence map to avoid checking same Common block twice
  BOPDS_MapOfPaveBlock aMPBFence;

  BOPDS_VectorOfListOfPaveBlock& aPBP = myDS->ChangePaveBlocksPool();
  const Standard_Integer aNbPBP = aPBP.Length();
  for (Standard_Integer i = 0; i < aNbPBP; ++i)
  {
    const BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    BOPDS_ListIteratorOfListOfPaveBlock itPB(aLPB);
    for (; itPB.More(); itPB.Next())
    {
      const Handle(BOPDS_CommonBlock)& aCB = myDS->CommonBlock(itPB.Value());
      if (aCB.IsNull())
        continue;

      const Handle(BOPDS_PaveBlock)& aPBR = aCB->PaveBlock1();
      if (!aMPBFence.Add(aPBR))
        continue;

      Standard_Real aTolCB = aCB->Tolerance();
      if (aTolCB > 0.)
      {
        UpdateVertex(aPBR->Pave1().Index(), aTolCB);
        UpdateVertex(aPBR->Pave2().Index(), aTolCB);
      }
    }
  }
}

//=======================================================================
//function : ForceInterfEE
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::ForceInterfEE(const Message_ProgressRange& theRange)
{
  // Now that we have vertices increased and unified, try to find additional
  // common blocks among the pairs of edges.
  // Since all real intersections should have already happened, here we
  // are interested in common blocks only, thus we need to check only
  // those pairs of pave blocks with the same bounding vertices.

  Handle(NCollection_IncAllocator) anAlloc = new NCollection_IncAllocator;
  Message_ProgressScope aPSOuter(theRange, NULL, 10);
  // Initialize pave blocks for all vertices which participated in intersections
  const Standard_Integer aNbS = myDS->NbSourceShapes();
  for (Standard_Integer i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() == TopAbs_VERTEX)
    {
      if (myDS->HasInterf(i))
        myDS->InitPaveBlocksForVertex(i);
    }
    if (UserBreak(aPSOuter))
    {
      return;
    }
  }
  // Fill the connection map from bounding vertices to pave blocks
  // having those bounding vertices
  NCollection_IndexedDataMap<BOPDS_Pair,
                             BOPDS_ListOfPaveBlock> aPBMap(1, anAlloc);
  // Fence map of pave blocks
  BOPDS_MapOfPaveBlock aMPBFence(1, anAlloc);

  for (Standard_Integer i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_EDGE)
      // Not an edge
      continue;

    if (!aSI.HasReference())
      // Edge has no pave blocks
      continue;

    if (aSI.HasFlag())
      // Degenerated edge
      continue;
    if (UserBreak(aPSOuter))
    {
      return;
    }
    const BOPDS_ListOfPaveBlock& aLPB = myDS->PaveBlocks(i);
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
    for (; aItLPB.More(); aItLPB.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
      const Handle(BOPDS_PaveBlock)& aPBR = myDS->RealPaveBlock(aPB);
      if (!aMPBFence.Add(aPBR))
        continue;

      // Get indices
      Standard_Integer nV1, nV2;
      aPBR->Indices(nV1, nV2);

      // Add pave block to a map
      BOPDS_Pair aPair(nV1, nV2);
      BOPDS_ListOfPaveBlock *pList = aPBMap.ChangeSeek(aPair);
      if (!pList)
        pList = &aPBMap(aPBMap.Add(aPair, BOPDS_ListOfPaveBlock(anAlloc)));
      pList->Append(aPBR);
    }
  }

  Standard_Integer aNbPB = aPBMap.Extent();
  if (!aNbPB)
    return;

  const Standard_Boolean bSICheckMode = (myArguments.Extent() == 1);

  // Prepare pave blocks with the same vertices for intersection.
  BOPAlgo_VectorOfEdgeEdge aVEdgeEdge;

  for (Standard_Integer i = 1; i <= aNbPB; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    const BOPDS_ListOfPaveBlock& aLPB = aPBMap(i);
    if (aLPB.Extent() < 2)
      continue;

    const BOPDS_Pair& aPair = aPBMap.FindKey(i);
    Standard_Integer nV1, nV2;
    aPair.Indices(nV1, nV2);

    const TopoDS_Vertex& aV1 = TopoDS::Vertex(myDS->Shape(nV1));
    const TopoDS_Vertex& aV2 = TopoDS::Vertex(myDS->Shape(nV2));

    // Use the max tolerance of vertices as Fuzzy value for intersection of edges.
    // In the Self-Interference check mode we are interested in real
    // intersections only, so use only the real tolerance of edges,
    // no need to use the extended tolerance.
    Standard_Real aTolAdd = (bSICheckMode ? myFuzzyValue :
      2 * Max(BRep_Tool::Tolerance(aV1), BRep_Tool::Tolerance(aV2)));

    // All possible pairs combined from the list <aLPB> should be checked
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB1(aLPB);
    for (; aItLPB1.More(); aItLPB1.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPB1 = aItLPB1.Value();
      const Handle(BOPDS_CommonBlock)& aCB1 = myDS->CommonBlock(aPB1);
      const Standard_Integer nE1 = aPB1->OriginalEdge();
      const Standard_Integer iR1 = myDS->Rank(nE1);
      const TopoDS_Edge& aE1 = TopoDS::Edge(myDS->Shape(nE1));
      Standard_Real aT11, aT12;
      aPB1->Range(aT11, aT12);
      BRepAdaptor_Curve aBAC1(aE1);
      gp_Pnt aPm;
      gp_Vec aVTgt1;
      aBAC1.D1((aT11 + aT12) * 0.5, aPm, aVTgt1);
      if (aVTgt1.SquareMagnitude() < gp::Resolution())
        continue;
      aVTgt1.Normalize();

      BOPDS_ListIteratorOfListOfPaveBlock aItLPB2 = aItLPB1;
      for (aItLPB2.Next(); aItLPB2.More(); aItLPB2.Next())
      {
        const Handle(BOPDS_PaveBlock)& aPB2 = aItLPB2.Value();
        const Handle(BOPDS_CommonBlock)& aCB2 = myDS->CommonBlock(aPB2);
        const Standard_Integer nE2 = aPB2->OriginalEdge();
        const Standard_Integer iR2 = myDS->Rank(nE2);

        // Check that the edges came from different arguments
        if (iR1 == iR2)
        {
          // If the sharing of the vertices is not original, but has been acquired
          // during the operation, check the coincidence of the edges even if
          // they came from the same argument
          if ((!myDS->IsNewShape(nV1) && (myDS->Rank(nV1) == iR1)) ||
              (!myDS->IsNewShape(nV2) && (myDS->Rank(nV2) == iR2)))
            continue;
        }

        // Check that the Pave blocks do not form the Common block already
        if (!aCB1.IsNull() && !aCB2.IsNull())
        {
          if (aCB1 == aCB2)
            continue;
        }

        const TopoDS_Edge& aE2 = TopoDS::Edge(myDS->Shape(nE2));
        Standard_Real aT21, aT22;
        aPB2->Range(aT21, aT22);

        // Check the angle between edges in the middle point.
        // If the angle is more than 25 degrees, do not use the additional
        // tolerance, as it may lead to undesired unification of edges
        Standard_Boolean bUseAddTol = Standard_True;
        {
          BRepAdaptor_Curve aBAC2(aE2);
          if (aBAC1.GetType() != GeomAbs_Line ||
              aBAC2.GetType() != GeomAbs_Line)
          {
            GeomAPI_ProjectPointOnCurve& aProjPC = myContext->ProjPC(aE2);
            aProjPC.Perform(aPm);
            if (!aProjPC.NbPoints())
              continue;

            gp_Pnt aPm2;
            gp_Vec aVTgt2;
            aBAC2.D1(aProjPC.LowerDistanceParameter(), aPm2, aVTgt2);
            if (aVTgt2.SquareMagnitude() < gp::Resolution())
              continue;

            // The angle should be close to zero
            Standard_Real aCos = aVTgt1.Dot (aVTgt2.Normalized());
            if (Abs(aCos) < 0.9063)
              bUseAddTol = Standard_False;
          }
        }

        // Add pair for intersection
        BOPAlgo_EdgeEdge& anEdgeEdge = aVEdgeEdge.Appended();
        anEdgeEdge.UseQuickCoincidenceCheck(Standard_True);
        anEdgeEdge.SetPaveBlock1(aPB1);
        anEdgeEdge.SetPaveBlock2(aPB2);
        anEdgeEdge.SetEdge1(aE1, aT11, aT12);
        anEdgeEdge.SetEdge2(aE2, aT21, aT22);
        anEdgeEdge.SetBoxes (myDS->ShapeInfo(nE1).Box(), myDS->ShapeInfo (nE2).Box());
        if (bUseAddTol)
        {
          anEdgeEdge.SetFuzzyValue(myFuzzyValue + aTolAdd);
        }
        else
        {
          anEdgeEdge.SetFuzzyValue(myFuzzyValue);
        }
      }
    }
  }

  Standard_Integer aNbPairs = aVEdgeEdge.Length();
  if (!aNbPairs)
    return;

  // close preparation step
  aPSOuter.Next(0.7);

  aPBMap.Clear();
  aMPBFence.Clear();
  anAlloc->Reset(false);

  Message_ProgressScope aPS(aPSOuter.Next(9), "Checking for coinciding edges", aNbPairs);
  for (Standard_Integer i = 0; i < aNbPairs; i++)
  {
    BOPAlgo_EdgeEdge& anEdgeEdge = aVEdgeEdge.ChangeValue(i);
    anEdgeEdge.SetProgressRange(aPS.Next());
  }

  // Perform intersection of the found pairs
  BOPTools_Parallel::Perform (myRunParallel, aVEdgeEdge);
  if (UserBreak(aPSOuter))
  {
    return;
  }
  BOPDS_VectorOfInterfEE& aEEs = myDS->InterfEE();
  if (aEEs.IsEmpty())
    aEEs.SetIncrement(10);

  // Analyze the results of intersection looking for TopAbs_EDGE
  // intersection type only.

  BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock aMPBLPB(1, anAlloc);
  for (Standard_Integer i = 0; i < aNbPairs; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    BOPAlgo_EdgeEdge& anEdgeEdge = aVEdgeEdge(i);
    if (!anEdgeEdge.IsDone() || anEdgeEdge.HasErrors())
    {
      // Warn about failed intersection of sub-shapes
      const TopoDS_Shape& aE1 = myDS->Shape(anEdgeEdge.PaveBlock1()->OriginalEdge());
      const TopoDS_Shape& aE2 = myDS->Shape(anEdgeEdge.PaveBlock2()->OriginalEdge());
      AddIntersectionFailedWarning(aE1, aE2);
      continue;
    }

    const IntTools_SequenceOfCommonPrts& aCParts = anEdgeEdge.CommonParts();
    if (aCParts.Length() != 1)
      continue;

    const IntTools_CommonPrt& aCP = aCParts(1);
    if (aCP.Type() != TopAbs_EDGE)
      continue;

    Handle(BOPDS_PaveBlock) aPB[] = {anEdgeEdge.PaveBlock1(), anEdgeEdge.PaveBlock2()};
    const Standard_Integer nE1 = aPB[0]->OriginalEdge();
    const Standard_Integer nE2 = aPB[1]->OriginalEdge();

    if (myDS->Rank(nE1) == myDS->Rank(nE2))
    {
      // Add acquired self-interference warning
      TopoDS_Compound aWC;
      BRep_Builder().MakeCompound(aWC);
      BRep_Builder().Add(aWC, myDS->Shape(nE1));
      BRep_Builder().Add(aWC, myDS->Shape(nE2));
      AddWarning(new BOPAlgo_AlertAcquiredSelfIntersection(aWC));
    }

    BOPDS_InterfEE& aEE = aEEs.Appended();
    aEE.SetIndices(nE1, nE2);
    aEE.SetCommonPart(aCP);
    myDS->AddInterf(nE1, nE2);

    // Fill map for common blocks creation
    for (Standard_Integer j = 0; j < 2; ++j)
    {
      if (myDS->IsCommonBlock(aPB[j]))
      {
        const BOPDS_ListOfPaveBlock& aLPBCB = myDS->CommonBlock(aPB[j])->PaveBlocks();
        BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPBCB);
        for (; aItLPB.More(); aItLPB.Next())
          BOPAlgo_Tools::FillMap<Handle(BOPDS_PaveBlock)>(aPB[j], aItLPB.Value(), aMPBLPB, anAlloc);
      }
    }
    BOPAlgo_Tools::FillMap<Handle(BOPDS_PaveBlock)>(aPB[0], aPB[1], aMPBLPB, anAlloc);
  }
  // Create new common blocks of coinciding pairs.
  BOPAlgo_Tools::PerformCommonBlocks(aMPBLPB, anAlloc, myDS);
}
