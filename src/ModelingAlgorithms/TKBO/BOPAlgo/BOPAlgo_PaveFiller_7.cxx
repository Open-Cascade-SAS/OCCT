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

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_MapOfCommonBlock.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_VectorOfCurve.hxx>
#include <BOPDS_VectorOfFaceInfo.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPDS_VectorOfListOfPaveBlock.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRepLib.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_Vector.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

static Standard_Boolean IsBasedOnPlane(const TopoDS_Face& aF);

static void UpdateVertices(const TopoDS_Edge& aE, const TopoDS_Face& aF);

//=================================================================================================

class BOPAlgo_SplitEdge : public BOPAlgo_ParallelAlgo
{

public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_SplitEdge()
      : BOPAlgo_ParallelAlgo()
  {
    myT1  = 0.;
    myT2  = 0.;
    myTol = 0.;
  }

  //
  virtual ~BOPAlgo_SplitEdge() {}

  //
  void SetData(const TopoDS_Edge&   aE,
               const TopoDS_Vertex& aV1,
               const Standard_Real  aT1,
               const TopoDS_Vertex& aV2,
               const Standard_Real  aT2)
  {
    myE   = aE;
    myV1  = aV1;
    myT1  = aT1;
    myV2  = aV2;
    myT2  = aT2;
    myESp = aE;
  }

  //
  void SetPaveBlock(const Handle(BOPDS_PaveBlock)& aPB) { myPB = aPB; }

  //
  Handle(BOPDS_PaveBlock)& PaveBlock() { return myPB; }

  //
  void SetCommonBlock(const Handle(BOPDS_CommonBlock)& aCB) { myCB = aCB; }

  //
  Handle(BOPDS_CommonBlock)& CommonBlock() { return myCB; }

  //
  const TopoDS_Edge& SplitEdge() const { return myESp; }

  //
  const Bnd_Box Box() { return myBox; }

  //
  Standard_Real Tolerance() const { return myTol; }

  //
  void SetDS(const BOPDS_PDS theDS) { myDS = theDS; }

  //
  void SetContext(const Handle(IntTools_Context)& aContext) { myContext = aContext; }

  //
  virtual void Perform()
  {
    Message_ProgressScope aPS(myProgressRange, NULL, 1);
    if (UserBreak(aPS))
    {
      return;
    }
    myTol = BOPAlgo_Tools::ComputeToleranceOfCB(myCB, myDS, myContext);
    BOPTools_AlgoTools::MakeSplitEdge(myE, myV1, myT1, myV2, myT2, myESp);
    BRepBndLib::Add(myESp, myBox);
    myBox.SetGap(myBox.GetGap() + Precision::Confusion());
  }

  //
protected:
  // ->
  TopoDS_Edge   myE;
  TopoDS_Vertex myV1;
  Standard_Real myT1;
  TopoDS_Vertex myV2;
  Standard_Real myT2;
  // <->
  Handle(BOPDS_PaveBlock)   myPB;
  Handle(BOPDS_CommonBlock) myCB;
  // <-
  TopoDS_Edge   myESp;
  Bnd_Box       myBox;
  Standard_Real myTol;
  //
  BOPDS_PDS                myDS;
  Handle(IntTools_Context) myContext;
};

//
//=======================================================================
typedef NCollection_Vector<BOPAlgo_SplitEdge> BOPAlgo_VectorOfSplitEdge;

//=================================================================================================

class BOPAlgo_MPC : public BOPAlgo_ParallelAlgo
{

public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_MPC()
      : BOPAlgo_ParallelAlgo(),
        myFlag(Standard_False) {};
  //
  virtual ~BOPAlgo_MPC() {};

  //
  void SetEdge(const TopoDS_Edge& aE) { myE = aE; }

  //
  const TopoDS_Edge& Edge() const { return myE; }

  //
  void SetFace(const TopoDS_Face& aF) { myF = aF; }

  //
  const TopoDS_Face& Face() const { return myF; }

  //
  void SetFlag(const Standard_Boolean bFlag) { myFlag = bFlag; }

  //
  Standard_Boolean Flag() const { return myFlag; }

  //
  void SetData(const TopoDS_Edge&   aEz,
               const TopoDS_Vertex& aV1,
               const Standard_Real  aT1,
               const TopoDS_Vertex& aV2,
               const Standard_Real  aT2)
  {
    myEz = aEz;
    myV1 = aV1;
    myT1 = aT1;
    myV2 = aV2;
    myT2 = aT2;
  }

  //
  void SetContext(const Handle(IntTools_Context)& aContext) { myContext = aContext; }

  //
  const Handle(IntTools_Context)& Context() const { return myContext; }

  //
  virtual void Perform()
  {
    Message_ProgressScope aPS(myProgressRange, NULL, 1);
    if (UserBreak(aPS))
    {
      return;
    }
    try
    {
      OCC_CATCH_SIGNALS

      // Check if edge has pcurve. If no then make its copy to avoid data races,
      // and use it to build pcurve.
      TopoDS_Edge          aCopyE = myE;
      Standard_Real        f, l;
      Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(aCopyE, myF, f, l);
      if (aC2d.IsNull())
      {
        aCopyE = BOPTools_AlgoTools::CopyEdge(aCopyE);

        Standard_Integer iErr = 1;
        if (!myEz.IsNull())
        {
          // Attach pcurve from the original edge
          TopoDS_Edge aSpz;
          BOPTools_AlgoTools::MakeSplitEdge(myEz, myV1, myT1, myV2, myT2, aSpz);
          iErr = BOPTools_AlgoTools2D::AttachExistingPCurve(aSpz, aCopyE, myF, myContext);
        }
        if (iErr)
          BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aCopyE, myF, myContext);

        myNewC2d = BRep_Tool::CurveOnSurface(aCopyE, myF, f, l);
        if (myNewC2d.IsNull())
        {
          AddError(new BOPAlgo_AlertBuildingPCurveFailed(TopoDS_Shape()));
          return;
        }
        else
          myNewTol = BRep_Tool::Tolerance(aCopyE);
      }
      else
      {
        const BRepAdaptor_Surface& aBAS = myContext->SurfaceAdaptor(myF);
        if (aBAS.IsUPeriodic() || aBAS.IsVPeriodic())
        {
          // The curve already exists. Adjust it for periodic cases.
          BOPTools_AlgoTools2D::AdjustPCurveOnSurf(myContext->SurfaceAdaptor(myF),
                                                   f,
                                                   l,
                                                   aC2d,
                                                   myNewC2d);
          if (myNewC2d != aC2d)
            myNewTol = BRep_Tool::Tolerance(aCopyE);
          else
            myNewC2d.Nullify();
        }
      }

      if (myFlag)
      {
        UpdateVertices(aCopyE, myF);
      }
    }
    catch (Standard_Failure const&)
    {
      AddError(new BOPAlgo_AlertBuildingPCurveFailed(TopoDS_Shape()));
    }
  }

  const Handle(Geom2d_Curve)& GetNewPCurve() const { return myNewC2d; }

  Standard_Real GetNewTolerance() const { return myNewTol; }

protected:
  Standard_Boolean     myFlag;
  TopoDS_Edge          myE;
  TopoDS_Face          myF;
  TopoDS_Edge          myEz;
  TopoDS_Vertex        myV1;
  Standard_Real        myT1;
  TopoDS_Vertex        myV2;
  Standard_Real        myT2;
  Handle(Geom2d_Curve) myNewC2d;
  Standard_Real        myNewTol;
  //
  Handle(IntTools_Context) myContext;
};

//
//=======================================================================
typedef NCollection_Vector<BOPAlgo_MPC> BOPAlgo_VectorOfMPC;

//=================================================================================================

class BOPAlgo_BPC
{
public:
  BOPAlgo_BPC() {};
  //
  ~BOPAlgo_BPC() {};

  //
  void SetFace(const TopoDS_Face& aF) { myF = aF; }

  //
  void SetEdge(const TopoDS_Edge& aE) { myE = aE; }

  //
  const TopoDS_Edge& GetEdge() const { return myE; }

  const TopoDS_Face& GetFace() const { return myF; }

  const Handle(Geom2d_Curve)& GetCurve2d() const { return myCurve; }

  Standard_Boolean IsToUpdate() const { return myToUpdate; }

  void SetRange(const Message_ProgressRange& theRange) { myRange = theRange; }

  //
  void Perform()
  {
    Message_ProgressScope aPS(myRange, NULL, 1);
    if (!aPS.More())
    {
      return;
    }
    BRepLib::BuildPCurveForEdgeOnPlane(myE, myF, myCurve, myToUpdate);
  };

  //
protected:
  TopoDS_Edge          myE;
  TopoDS_Face          myF;
  Handle(Geom2d_Curve) myCurve;
  Standard_Boolean     myToUpdate;

private:
  Message_ProgressRange myRange;
};

//=======================================================================
typedef NCollection_Vector<BOPAlgo_BPC> BOPAlgo_VectorOfBPC;

//=================================================================================================

void BOPAlgo_PaveFiller::MakeSplitEdges(const Message_ProgressRange& theRange)
{
  BOPDS_VectorOfListOfPaveBlock& aPBP   = myDS->ChangePaveBlocksPool();
  Standard_Integer               aNbPBP = aPBP.Length();
  Message_ProgressScope          aPSOuter(theRange, NULL, 1);
  if (!aNbPBP)
  {
    return;
  }
  //
  Standard_Integer                    i, nE, nV1, nV2, nSp, aNbVBSE, k;
  Standard_Real                       aT1, aT2;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  Handle(BOPDS_PaveBlock)             aPB;
  BOPDS_MapOfCommonBlock              aMCB(100);
  TopoDS_Vertex                       aV1, aV2;
  TopoDS_Edge                         aE;
  BOPAlgo_VectorOfSplitEdge           aVBSE;

  //
  UpdateCommonBlocksWithSDVertices();
  //
  aNbPBP = aPBP.Length();
  //
  for (i = 0; i < aNbPBP; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    //
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next())
    {
      aPB                         = aItPB.Value();
      nE                          = aPB->OriginalEdge();
      const BOPDS_ShapeInfo& aSIE = myDS->ShapeInfo(nE);
      if (aSIE.HasFlag())
      {
        // Skip degenerated edges
        continue;
      }

      const Handle(BOPDS_CommonBlock)& aCB = myDS->CommonBlock(aPB);
      Standard_Boolean                 bCB = !aCB.IsNull();
      if (bCB && !aMCB.Add(aCB))
        continue;

      aPB->Indices(nV1, nV2);
      // Check if it is necessary to make the split of the edge
      {
        Standard_Boolean bV1 = myDS->IsNewShape(nV1);
        Standard_Boolean bV2 = myDS->IsNewShape(nV2);

        Standard_Boolean bToSplit = Standard_True;
        if (!bV1 && !bV2) // no new vertices here
        {
          if (!myNonDestructive || !bCB)
          {
            if (bCB)
            {
              // Find the edge with these vertices
              BOPDS_ListIteratorOfListOfPaveBlock it(aCB->PaveBlocks());
              for (; it.More(); it.Next())
              {
                nE = it.Value()->OriginalEdge();
                if (myDS->PaveBlocks(nE).Extent() == 1)
                  break;
              }
              if (it.More())
              {
                // The pave block is found
                bToSplit = Standard_False;
                aCB->SetRealPaveBlock(it.Value());
                aCB->SetEdge(nE);
                // Compute tolerance of the common block and update the edge
                Standard_Real aTol = BOPAlgo_Tools::ComputeToleranceOfCB(aCB, myDS, myContext);
                UpdateEdgeTolerance(nE, aTol);
              }
            }
            else if (aLPB.Extent() == 1)
            {
              bToSplit = Standard_False;
              aPB->SetEdge(nE);
            }
            if (!bToSplit)
              continue;
          }
        }
      }

      // Split the edge
      if (bCB)
      {
        aPB = aCB->PaveBlock1();
        nE  = aPB->OriginalEdge();
        aPB->Indices(nV1, nV2);
      }
      aPB->Range(aT1, aT2);
      //
      aE = (*(TopoDS_Edge*)(&myDS->Shape(nE)));
      aE.Orientation(TopAbs_FORWARD);
      //
      aV1 = (*(TopoDS_Vertex*)(&myDS->Shape(nV1)));
      aV1.Orientation(TopAbs_FORWARD);
      //
      aV2 = (*(TopoDS_Vertex*)(&myDS->Shape(nV2)));
      aV2.Orientation(TopAbs_REVERSED);
      //
      BOPAlgo_SplitEdge& aBSE = aVBSE.Appended();
      //
      aBSE.SetData(aE, aV1, aT1, aV2, aT2);
      aBSE.SetPaveBlock(aPB);
      if (bCB)
      {
        aBSE.SetCommonBlock(aCB);
      }
      aBSE.SetDS(myDS);
    } // for (; aItPB.More(); aItPB.Next()) {
  } // for (i=0; i<aNbPBP; ++i) {
  //
  aNbVBSE = aVBSE.Length();
  Message_ProgressScope aPS(aPSOuter.Next(), "Splitting edges", aNbVBSE);
  for (k = 0; k < aNbVBSE; k++)
  {
    BOPAlgo_SplitEdge& aBSE = aVBSE.ChangeValue(k);
    aBSE.SetProgressRange(aPS.Next());
  }
  //======================================================
  BOPTools_Parallel::Perform(myRunParallel, aVBSE, myContext);
  //======================================================
  if (HasErrors())
  {
    return;
  }
  //
  for (k = 0; k < aNbVBSE; ++k)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    BOPAlgo_SplitEdge& aBSE = aVBSE(k);
    //
    const TopoDS_Edge& aSp  = aBSE.SplitEdge();
    const Bnd_Box&     aBox = aBSE.Box();
    //
    Handle(BOPDS_PaveBlock)    aPBk = aBSE.PaveBlock();
    Handle(BOPDS_CommonBlock)& aCBk = aBSE.CommonBlock();
    //
    BOPDS_ShapeInfo aSI;
    aSI.SetShapeType(TopAbs_EDGE);
    aSI.SetShape(aSp);
    aSI.ChangeBox()                   = aBox;
    TColStd_ListOfInteger& aSubShapes = aSI.ChangeSubShapes();
    aSubShapes.Append(aPBk->Pave1().Index());
    aSubShapes.Append(aPBk->Pave2().Index());
    //
    nSp = myDS->Append(aSI);
    //
    if (!aCBk.IsNull())
    {
      UpdateEdgeTolerance(nSp, aBSE.Tolerance());
      aCBk->SetEdge(nSp);
    }
    else
    {
      aPBk->SetEdge(nSp);
    }
  }
}

//=================================================================================================

Standard_Integer BOPAlgo_PaveFiller::SplitEdge(const Standard_Integer nE,
                                               const Standard_Integer nV1,
                                               const Standard_Real    aT1,
                                               const Standard_Integer nV2,
                                               const Standard_Real    aT2)
{
  Standard_Integer nSp;
  TopoDS_Vertex    aV1, aV2;
  TopoDS_Edge      aE, aSp;
  BOPDS_ShapeInfo  aSI;
  //
  aSI.SetShapeType(TopAbs_EDGE);
  //
  aE = (*(TopoDS_Edge*)(&myDS->Shape(nE)));
  aE.Orientation(TopAbs_FORWARD);
  //
  aV1 = (*(TopoDS_Vertex*)(&myDS->Shape(nV1)));
  aV1.Orientation(TopAbs_FORWARD);
  //
  aV2 = (*(TopoDS_Vertex*)(&myDS->Shape(nV2)));
  aV2.Orientation(TopAbs_REVERSED);
  //
  BOPTools_AlgoTools::MakeSplitEdge(aE, aV1, aT1, aV2, aT2, aSp);
  //
  aSI.SetShape(aSp);
  //
  Bnd_Box& aBox = aSI.ChangeBox();
  BRepBndLib::Add(aSp, aBox);
  aBox.SetGap(aBox.GetGap() + Precision::Confusion());
  //
  nSp = myDS->Append(aSI);
  return nSp;
}

//=================================================================================================

void BOPAlgo_PaveFiller::MakePCurves(const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPSOuter(theRange, NULL, 1);
  if (myAvoidBuildPCurve || (!mySectionAttribute.PCurveOnS1() && !mySectionAttribute.PCurveOnS2()))
    return;
  Standard_Boolean                    bHasPC;
  Standard_Integer                    i, nF1, aNbC, k, nE, aNbFF, aNbFI, nEx;
  Standard_Integer                    j, aNbPBIn, aNbPBOn;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  TopoDS_Face                         aF1F, aF2F;
  BOPAlgo_VectorOfMPC                 aVMPC;
  //
  // 1. Process Common Blocks
  const BOPDS_VectorOfFaceInfo& aFIP = myDS->FaceInfoPool();
  //
  aNbFI = aFIP.Length();
  for (i = 0; i < aNbFI; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    const BOPDS_FaceInfo& aFI = aFIP(i);
    nF1                       = aFI.Index();
    //
    aF1F = (*(TopoDS_Face*)(&myDS->Shape(nF1)));
    aF1F.Orientation(TopAbs_FORWARD);
    // In
    const BOPDS_IndexedMapOfPaveBlock& aMPBIn = aFI.PaveBlocksIn();
    aNbPBIn                                   = aMPBIn.Extent();
    for (j = 1; j <= aNbPBIn; ++j)
    {
      const Handle(BOPDS_PaveBlock)& aPB = aMPBIn(j);
      nE                                 = aPB->Edge();
      Standard_ASSERT(nE >= 0, "Face information is not up to date", continue);
      const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&myDS->Shape(nE)));
      //
      BOPAlgo_MPC& aMPC = aVMPC.Appended();
      aMPC.SetEdge(aE);
      aMPC.SetFace(aF1F);
    }
    //
    // On
    const BOPDS_IndexedMapOfPaveBlock& aMPBOn = aFI.PaveBlocksOn();
    aNbPBOn                                   = aMPBOn.Extent();
    for (j = 1; j <= aNbPBOn; ++j)
    {
      const Handle(BOPDS_PaveBlock)& aPB = aMPBOn(j);
      nE                                 = aPB->Edge();
      const TopoDS_Edge& aE              = (*(TopoDS_Edge*)(&myDS->Shape(nE)));
      bHasPC                             = BOPTools_AlgoTools2D::HasCurveOnSurface(aE, aF1F);
      if (bHasPC)
      {
        continue;
      }
      //
      BOPAlgo_MPC& aMPC = aVMPC.Appended();

      Handle(BOPDS_CommonBlock) aCB = myDS->CommonBlock(aPB);

      if (!aCB.IsNull())
      {
        const BOPDS_ListOfPaveBlock& aLPB = aCB->PaveBlocks();
        if (aLPB.Extent() >= 2)
        {
          aItLPB.Initialize(aLPB);
          for (; aItLPB.More(); aItLPB.Next())
          {
            const Handle(BOPDS_PaveBlock)& aPBx = aItLPB.Value();
            if (aPBx == aPB)
            {
              continue;
            }
            //
            nEx                    = aPBx->OriginalEdge();
            const TopoDS_Edge& aEx = (*(TopoDS_Edge*)(&myDS->Shape(nEx)));
            bHasPC                 = BOPTools_AlgoTools2D::HasCurveOnSurface(aEx, aF1F);
            if (!bHasPC)
            {
              continue;
            }
            //
            Standard_Integer nV1x, nV2x;
            Standard_Real    aT1x, aT2x;
            TopoDS_Vertex    aV1x, aV2x;
            TopoDS_Edge      aEz;
            //
            aEz = aEx;
            aEz.Orientation(TopAbs_FORWARD);
            //
            aPBx->Indices(nV1x, nV2x);
            aPBx->Range(aT1x, aT2x);
            //
            aV1x = (*(TopoDS_Vertex*)(&myDS->Shape(nV1x)));
            aV1x.Orientation(TopAbs_FORWARD);
            //
            aV2x = (*(TopoDS_Vertex*)(&myDS->Shape(nV2x)));
            aV2x.Orientation(TopAbs_REVERSED);
            //
            aMPC.SetData(aEz, aV1x, aT1x, aV2x, aT2x);
            //
            break;
          }
        }
      }

      aMPC.SetEdge(aE);
      aMPC.SetFace(aF1F);
    }
  } // for (i=0; i<aNbFI; ++i) {
  //
  // 2. Process section edges. P-curves on them must already be computed.
  // However, we must provide the call to UpdateVertices.
  Standard_Boolean bPCurveOnS[2];
  bPCurveOnS[0] = mySectionAttribute.PCurveOnS1();
  bPCurveOnS[1] = mySectionAttribute.PCurveOnS2();
  //
  if (bPCurveOnS[0] || bPCurveOnS[1])
  {
    // container to remember already added edge-face pairs
    BOPDS_MapOfPair         anEFPairs;
    BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
    aNbFF                        = aFFs.Length();
    for (i = 0; i < aNbFF; ++i)
    {
      const BOPDS_InterfFF&      aFF  = aFFs(i);
      const BOPDS_VectorOfCurve& aVNC = aFF.Curves();
      aNbC                            = aVNC.Length();
      if (aNbC == 0)
        continue;
      Standard_Integer nF[2];
      aFF.Indices(nF[0], nF[1]);
      //
      TopoDS_Face aFf[2];
      aFf[0] = (*(TopoDS_Face*)(&myDS->Shape(nF[0])));
      aFf[0].Orientation(TopAbs_FORWARD);
      //
      aFf[1] = (*(TopoDS_Face*)(&myDS->Shape(nF[1])));
      aFf[1].Orientation(TopAbs_FORWARD);
      //
      for (k = 0; k < aNbC; ++k)
      {
        const BOPDS_Curve&           aNC  = aVNC(k);
        const BOPDS_ListOfPaveBlock& aLPB = aNC.PaveBlocks();
        aItLPB.Initialize(aLPB);
        for (; aItLPB.More(); aItLPB.Next())
        {
          const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
          nE                                 = aPB->Edge();
          const TopoDS_Edge& aE              = (*(TopoDS_Edge*)(&myDS->Shape(nE)));
          //
          for (Standard_Integer m = 0; m < 2; ++m)
          {
            if (bPCurveOnS[m] && anEFPairs.Add(BOPDS_Pair(nE, nF[m])))
            {
              BOPAlgo_MPC& aMPC = aVMPC.Appended();
              aMPC.SetEdge(aE);
              aMPC.SetFace(aFf[m]);
              aMPC.SetFlag(Standard_True);
            }
          }
        }
      }
    } // for (i=0; i<aNbFF; ++i) {
  } // if (bPCurveOnS1 || bPCurveOnS2 ) {
  //
  Message_ProgressScope aPS(aPSOuter.Next(), "Projecting edges on faces", aVMPC.Length());
  for (i = 0; i < aVMPC.Length(); i++)
  {
    BOPAlgo_MPC& aMPC = aVMPC.ChangeValue(i);
    aMPC.SetProgressRange(aPS.Next());
  }
  //======================================================
  BOPTools_Parallel::Perform(myRunParallel, aVMPC, myContext);
  //======================================================
  if (HasErrors())
  {
    return;
  }

  // Add warnings of the failed projections and update edges with new pcurves
  Standard_Integer aNb = aVMPC.Length();
  for (i = 0; i < aNb; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    const BOPAlgo_MPC& aMPC = aVMPC(i);
    if (aMPC.HasErrors())
    {
      TopoDS_Compound aWC;
      BRep_Builder().MakeCompound(aWC);
      BRep_Builder().Add(aWC, aMPC.Edge());
      BRep_Builder().Add(aWC, aMPC.Face());
      AddWarning(new BOPAlgo_AlertBuildingPCurveFailed(aWC));
    }
    else
    {
      const Handle(Geom2d_Curve)& aNewPC = aMPC.GetNewPCurve();
      // if aNewPC is null we do not need to update the edge because it already contains
      // valid p-curve, and only vertices have been updated.
      if (!aNewPC.IsNull())
        BRep_Builder().UpdateEdge(aMPC.Edge(), aNewPC, aMPC.Face(), aMPC.GetNewTolerance());
    }
  }
}

//=======================================================================
// function : UpdateVertices
// purpose  : update tolerances of vertices comparing extremities of
//           3d and 2d curves
//=======================================================================
void UpdateVertices(const TopoDS_Edge& aE, const TopoDS_Face& aF)
{
  Standard_Integer     j;
  Standard_Real        aT[2], aUx, aVx, aTolV2, aD2, aD;
  gp_Pnt               aP3D, aP3Dx;
  gp_Pnt2d             aP2Dx;
  Handle(Geom_Surface) aS;
  Handle(Geom_Curve)   aC3D;
  Handle(Geom2d_Curve) aC2D;
  TopoDS_Edge          aEf;
  TopoDS_Vertex        aV[2];
  BRep_Builder         aBB;
  //
  aEf = aE;
  aEf.Orientation(TopAbs_FORWARD);
  //
  TopExp::Vertices(aEf, aV[0], aV[1]);
  //
  aS   = BRep_Tool::Surface(aF);
  aC3D = BRep_Tool::Curve(aEf, aT[0], aT[1]);
  aC2D = BRep_Tool::CurveOnSurface(aEf, aF, aT[0], aT[1]);
  //
  for (j = 0; j < 2; ++j)
  {
    aTolV2 = BRep_Tool::Tolerance(aV[j]);
    aTolV2 = aTolV2 * aTolV2;
    //
    aC3D->D0(aT[j], aP3D);
    aC2D->D0(aT[j], aP2Dx);
    aP2Dx.Coord(aUx, aVx);
    aS->D0(aUx, aVx, aP3Dx);
    aD2 = aP3D.SquareDistance(aP3Dx);
    if (aD2 > aTolV2)
    {
      aD = sqrt(aD2);
      aBB.UpdateVertex(aV[j], aD + BOPTools_AlgoTools::DTolerance());
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::Prepare(const Message_ProgressRange& theRange)
{
  if (myNonDestructive)
  {
    // do not allow storing pcurves in original edges if non-destructive mode is on
    return;
  }
  TopAbs_ShapeEnum           aType[] = {TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE};
  Standard_Boolean           bIsBasedOnPlane;
  Standard_Integer           i, aNb, n1, nF, aNbF;
  TopExp_Explorer            aExp;
  TopTools_IndexedMapOfShape aMF;
  //
  aNb = 3;
  Message_ProgressScope aPSOuter(theRange, NULL, 1);
  for (i = 0; i < aNb; ++i)
  {
    myIterator->Initialize(aType[i], aType[2]);
    for (; myIterator->More(); myIterator->Next())
    {
      myIterator->Value(n1, nF);
      const TopoDS_Face& aF = (*(TopoDS_Face*)(&myDS->Shape(nF)));
      //
      bIsBasedOnPlane = IsBasedOnPlane(aF);
      if (bIsBasedOnPlane)
      {
        aMF.Add(aF);
      }
    }
  }
  //
  aNbF = aMF.Extent();
  if (!aNbF)
  {
    return;
  }
  //
  // Build pcurves of edges on planes; first collect pairs edge-face.
  BOPAlgo_VectorOfBPC aVBPC;
  //
  for (i = 1; i <= aNbF; ++i)
  {
    const TopoDS_Face& aF = *(TopoDS_Face*)&aMF(i);
    aExp.Init(aF, aType[1]);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Edge& aE   = *((TopoDS_Edge*)&aExp.Current());
      BOPAlgo_BPC&       aBPC = aVBPC.Appended();
      aBPC.SetEdge(aE);
      aBPC.SetFace(aF);
    }
  }
  //
  Message_ProgressScope aPS(aPSOuter.Next(), "Building 2d curves on planar faces", aVBPC.Length());
  for (i = 0; i < aVBPC.Length(); i++)
  {
    BOPAlgo_BPC& aBPC = aVBPC.ChangeValue(i);
    aBPC.SetRange(aPS.Next());
  }
  //======================================================
  BOPTools_Parallel::Perform(myRunParallel, aVBPC);
  //======================================================
  if (UserBreak(aPS))
  {
    return;
  }
  // pcurves are built, and now update edges
  BRep_Builder aBB;
  TopoDS_Edge  E;
  for (i = 0; i < aVBPC.Length(); i++)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    const BOPAlgo_BPC& aBPC = aVBPC(i);
    if (aBPC.IsToUpdate())
    {
      Standard_Real aTolE = BRep_Tool::Tolerance(aBPC.GetEdge());
      aBB.UpdateEdge(aBPC.GetEdge(), aBPC.GetCurve2d(), aBPC.GetFace(), aTolE);
    }
  }
}

//=================================================================================================

Standard_Boolean IsBasedOnPlane(const TopoDS_Face& aF)
{
  TopLoc_Location                        aLoc;
  Handle(Geom_RectangularTrimmedSurface) aGRTS;
  Handle(Geom_Plane)                     aGP;

  const Handle(Geom_Surface)& aS = BRep_Tool::Surface(aF, aLoc);
  aGRTS                          = Handle(Geom_RectangularTrimmedSurface)::DownCast(aS);
  if (!aGRTS.IsNull())
  {
    aGP = Handle(Geom_Plane)::DownCast(aGRTS->BasisSurface());
  }
  else
  {
    aGP = Handle(Geom_Plane)::DownCast(aS);
  }
  //
  return (!aGP.IsNull());
}
