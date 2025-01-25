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
#include <Bnd_Box.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <BOPAlgo_Tools.hxx>
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
#include <BOPTools_BoxSelector.hxx>
#include <BOPTools_Parallel.hxx>
#include <Bnd_Tools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
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
#include <IntTools_Tools.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_ListOfShape.hxx>

//
static Standard_Real ToleranceFF(const BRepAdaptor_Surface& aBAS1,
                                 const BRepAdaptor_Surface& aBAS2);

/////////////////////////////////////////////////////////////////////////
//=================================================================================================

class BOPAlgo_FaceFace : public IntTools_FaceFace, public BOPAlgo_ParallelAlgo
{

public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_FaceFace()
      : IntTools_FaceFace(),
        BOPAlgo_ParallelAlgo(),
        myIF1(-1),
        myIF2(-1),
        myTolFF(1.e-7)
  {
  }

  //
  virtual ~BOPAlgo_FaceFace() {}

  //
  void SetIndices(const Standard_Integer nF1, const Standard_Integer nF2)
  {
    myIF1 = nF1;
    myIF2 = nF2;
  }

  //
  void Indices(Standard_Integer& nF1, Standard_Integer& nF2) const
  {
    nF1 = myIF1;
    nF2 = myIF2;
  }

  //
  void SetFaces(const TopoDS_Face& aF1, const TopoDS_Face& aF2)
  {
    myF1 = aF1;
    myF2 = aF2;
  }

  //
  void SetBoxes(const Bnd_Box& theBox1, const Bnd_Box& theBox2)
  {
    myBox1 = theBox1;
    myBox2 = theBox2;
  }

  //
  const TopoDS_Face& Face1() const { return myF1; }

  //
  const TopoDS_Face& Face2() const { return myF2; }

  //
  void SetTolFF(const Standard_Real aTolFF) { myTolFF = aTolFF; }

  //
  Standard_Real TolFF() const { return myTolFF; }

  //
  void SetFuzzyValue(const Standard_Real theFuzz) { IntTools_FaceFace::SetFuzzyValue(theFuzz); }

  //
  const gp_Trsf& Trsf() const { return myTrsf; }

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

      gp_Trsf     aTrsf;
      TopoDS_Face aF1 = myF1, aF2 = myF2;
      if (BOPAlgo_Tools::TrsfToPoint(myBox1, myBox2, aTrsf))
      {
        // Shapes are located far from origin, move the shapes to the origin,
        // to increase the accuracy of intersection.
        TopLoc_Location aLoc(aTrsf);
        aF1.Move(aLoc);
        aF2.Move(aLoc);

        // The starting point is initialized only with the UV parameters
        // on the faces - 3D point is not set (see GetEFPnts method),
        // so no need to transform anything.
        // for (IntSurf_ListOfPntOn2S::Iterator it (myListOfPnts); it.More(); it.Next())
        //{
        //  IntSurf_PntOn2S& aP2S = it.ChangeValue();
        //  aP2S.SetValue (aP2S.Value().Transformed (aTrsf));
        //}

        myTrsf = aTrsf.Inverted();
      }

      IntTools_FaceFace::Perform(aF1, aF2, myRunParallel);
    }
    catch (Standard_Failure const&)
    {
      AddError(new BOPAlgo_AlertIntersectionFailed);
    }
  }

  //
  void ApplyTrsf()
  {
    if (IsDone())
    {
      // Update curves
      for (Standard_Integer i = 1; i <= mySeqOfCurve.Length(); ++i)
      {
        IntTools_Curve& aIC = mySeqOfCurve(i);
        aIC.Curve()->Transform(myTrsf);
      }
      // Update points
      for (Standard_Integer i = 1; i <= myPnts.Length(); ++i)
      {
        IntTools_PntOn2Faces& aP2F   = myPnts(i);
        IntTools_PntOnFace    aPOnF1 = aP2F.P1(), aPOnF2 = aP2F.P2();
        aPOnF1.SetPnt(aPOnF1.Pnt().Transformed(myTrsf));
        aPOnF2.SetPnt(aPOnF2.Pnt().Transformed(myTrsf));
        aP2F.SetP1(aPOnF1);
        aP2F.SetP2(aPOnF2);
      }
    }
  }

  //
protected:
  Standard_Integer myIF1;
  Standard_Integer myIF2;
  Standard_Real    myTolFF;
  TopoDS_Face      myF1;
  TopoDS_Face      myF2;
  Bnd_Box          myBox1;
  Bnd_Box          myBox2;
  gp_Trsf          myTrsf;
};

//
//=======================================================================
typedef NCollection_Vector<BOPAlgo_FaceFace> BOPAlgo_VectorOfFaceFace;

/////////////////////////////////////////////////////////////////////////
//=================================================================================================

void BOPAlgo_PaveFiller::PerformFF(const Message_ProgressRange& theRange)
{
  // Update face info for all Face/Face intersection pairs
  // and also for the rest of the faces with FaceInfo already initialized,
  // i.e. anyhow touched faces.
  myIterator->Initialize(TopAbs_FACE, TopAbs_FACE);
  Standard_Integer iSize = myIterator->ExpectedLength();

  // Collect faces from intersection pairs
  TColStd_MapOfInteger aMIFence;
  Standard_Integer     nF1, nF2;
  for (; myIterator->More(); myIterator->Next())
  {
    myIterator->Value(nF1, nF2);
    aMIFence.Add(nF1);
    aMIFence.Add(nF2);
  }
  // Collect the rest of the touched faces
  for (Standard_Integer i = 0; i < myDS->NbSourceShapes(); ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() == TopAbs_FACE && aSI.HasReference())
    {
      aMIFence.Add(i);
    }
  }
  // Update face info
  myDS->UpdateFaceInfoOn(aMIFence);
  myDS->UpdateFaceInfoIn(aMIFence);

  if (!iSize)
  {
    // no intersection pairs found
    return;
  }

  Message_ProgressScope aPSOuter(theRange, NULL, 1);

  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  aFFs.SetIncrement(iSize);
  //
  // Options for the intersection algorithm
  Standard_Boolean bApprox   = mySectionAttribute.Approximation(),
                   bCompC2D1 = mySectionAttribute.PCurveOnS1(),
                   bCompC2D2 = mySectionAttribute.PCurveOnS2();
  Standard_Real anApproxTol  = 1.e-7;
  // Post-processing options
  Standard_Boolean bSplitCurve = Standard_False;
  //
  // Collect all pairs of Edge/Edge interferences to check if
  // some faces have to be moved to obtain more precise intersection
  NCollection_DataMap<BOPDS_Pair, TColStd_ListOfInteger> aEEMap;
  const BOPDS_VectorOfInterfEE&                          aVEEs = myDS->InterfEE();
  for (Standard_Integer iEE = 0; iEE < aVEEs.Size(); ++iEE)
  {
    const BOPDS_Interf& aEE = aVEEs(iEE);
    if (!aEE.HasIndexNew())
    {
      continue;
    }
    Standard_Integer nE1, nE2;
    aEE.Indices(nE1, nE2);

    const Standard_Integer nVN = aEE.IndexNew();

    BOPDS_Pair             aPair(nE1, nE2);
    TColStd_ListOfInteger* pPoints = aEEMap.ChangeSeek(aPair);
    if (pPoints)
    {
      pPoints->Append(nVN);
    }
    else
    {
      pPoints = aEEMap.Bound(BOPDS_Pair(nE1, nE2), TColStd_ListOfInteger());
      pPoints->Append(nVN);
    }
  }

  // Prepare the pairs of faces for intersection
  BOPAlgo_VectorOfFaceFace aVFaceFace;
  myIterator->Initialize(TopAbs_FACE, TopAbs_FACE);
  for (; myIterator->More(); myIterator->Next())
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    myIterator->Value(nF1, nF2);

    if (myGlue == BOPAlgo_GlueOff)
    {
      const TopoDS_Face& aF1 = (*(TopoDS_Face*)(&myDS->Shape(nF1)));
      const TopoDS_Face& aF2 = (*(TopoDS_Face*)(&myDS->Shape(nF2)));
      //
      const BRepAdaptor_Surface& aBAS1 = myContext->SurfaceAdaptor(aF1);
      const BRepAdaptor_Surface& aBAS2 = myContext->SurfaceAdaptor(aF2);
      if (aBAS1.GetType() == GeomAbs_Plane && aBAS2.GetType() == GeomAbs_Plane)
      {
        // Check if the planes are really interfering
        Standard_Boolean bToIntersect = CheckPlanes(nF1, nF2);
        if (!bToIntersect)
        {
          BOPDS_InterfFF& aFF = aFFs.Appended();
          aFF.SetIndices(nF1, nF2);
          aFF.Init(0, 0);
          continue;
        }
      }

      // Check if there is an intersection between edges of the faces.
      // If there is an intersection, check if there is a shift between the edges
      // (intersection point is on some distance from the edges), and move one of
      // the faces to the point of exact edges intersection. This should allow
      // obtaining more precise intersection curves between the faces
      // (at least the curves should reach the boundary).
      // Note, that currently this check considers only closed edges (seam edges).
      TopoDS_Face aFShifted1 = aF1, aFShifted2 = aF2;
      // Keep shift value to use it as the tolerance for intersection curves
      Standard_Real aShiftValue = 0.;

      if (aBAS1.GetType() != GeomAbs_Plane || aBAS2.GetType() != GeomAbs_Plane)
      {

        Standard_Boolean isFound = Standard_False;
        for (TopExp_Explorer aExp1(aF1, TopAbs_EDGE); !isFound && aExp1.More(); aExp1.Next())
        {
          const TopoDS_Edge&     aE1 = TopoDS::Edge(aExp1.Current());
          const Standard_Integer nE1 = myDS->Index(aE1);

          for (TopExp_Explorer aExp2(aF2, TopAbs_EDGE); !isFound && aExp2.More(); aExp2.Next())
          {
            const TopoDS_Edge&     aE2 = TopoDS::Edge(aExp2.Current());
            const Standard_Integer nE2 = myDS->Index(aE2);

            Standard_Boolean bIsClosed1 = BRep_Tool::IsClosed(aE1, aF1);
            Standard_Boolean bIsClosed2 = BRep_Tool::IsClosed(aE2, aF2);
            if (!bIsClosed1 && !bIsClosed2)
            {
              continue;
            }

            const TColStd_ListOfInteger* pPoints = aEEMap.Seek(BOPDS_Pair(nE1, nE2));
            if (!pPoints)
            {
              continue;
            }

            for (TColStd_ListOfInteger::Iterator itEEP(*pPoints); itEEP.More(); itEEP.Next())
            {
              const Standard_Integer& nVN  = itEEP.Value();
              const TopoDS_Vertex&    aVN  = TopoDS::Vertex(myDS->Shape(nVN));
              const gp_Pnt&           aPnt = BRep_Tool::Pnt(aVN);

              // Compute points exactly on the edges
              GeomAPI_ProjectPointOnCurve& aProjPC1 = myContext->ProjPC(aE1);
              GeomAPI_ProjectPointOnCurve& aProjPC2 = myContext->ProjPC(aE2);
              aProjPC1.Perform(aPnt);
              aProjPC2.Perform(aPnt);
              if (!aProjPC1.NbPoints() && !aProjPC2.NbPoints())
              {
                continue;
              }
              gp_Pnt aP1 = aProjPC1.NbPoints() > 0 ? aProjPC1.NearestPoint() : aPnt;
              gp_Pnt aP2 = aProjPC2.NbPoints() > 0 ? aProjPC2.NearestPoint() : aPnt;

              Standard_Real aShiftDist = aP1.Distance(aP2);
              if (aShiftDist > BRep_Tool::Tolerance(aVN))
              {
                // Move one of the faces to the point of exact intersection of edges
                gp_Trsf aTrsf;
                aTrsf.SetTranslation(bIsClosed1 ? gp_Vec(aP1, aP2) : gp_Vec(aP2, aP1));
                TopLoc_Location aLoc(aTrsf);
                (bIsClosed1 ? &aFShifted1 : &aFShifted2)->Move(aLoc);
                aShiftValue = aShiftDist;
                isFound     = Standard_True;
              }
            }
          }
        }
      }
      //
      BOPAlgo_FaceFace& aFaceFace = aVFaceFace.Appended();
      //
      aFaceFace.SetRunParallel(myRunParallel);
      aFaceFace.SetIndices(nF1, nF2);
      aFaceFace.SetFaces(aFShifted1, aFShifted2);
      aFaceFace.SetBoxes(myDS->ShapeInfo(nF1).Box(), myDS->ShapeInfo(nF2).Box());
      // Note: in case of faces with closed edges it should not be less than value of the shift
      Standard_Real aTolFF = Max(aShiftValue, ToleranceFF(aBAS1, aBAS2));
      aFaceFace.SetTolFF(aTolFF);
      //
      IntSurf_ListOfPntOn2S aListOfPnts;
      GetEFPnts(nF1, nF2, aListOfPnts);
      Standard_Integer aNbLP = aListOfPnts.Extent();
      if (aNbLP)
      {
        aFaceFace.SetList(aListOfPnts);
      }
      //
      aFaceFace.SetParameters(bApprox, bCompC2D1, bCompC2D2, anApproxTol);
      aFaceFace.SetFuzzyValue(myFuzzyValue);
    }
    else
    {
      // for the Glue mode just add all interferences of that type
      BOPDS_InterfFF& aFF = aFFs.Appended();
      aFF.SetIndices(nF1, nF2);
      aFF.SetTangentFaces(Standard_False);
      aFF.Init(0, 0);
    }
  } // for (; myIterator->More(); myIterator->Next()) {
  //
  Standard_Integer k, aNbFaceFace = aVFaceFace.Length();
  ;
  Message_ProgressScope aPS(aPSOuter.Next(), "Performing Face-Face intersection", aNbFaceFace);
  for (k = 0; k < aNbFaceFace; k++)
  {
    BOPAlgo_FaceFace& aFaceFace = aVFaceFace.ChangeValue(k);
    aFaceFace.SetProgressRange(aPS.Next());
  }
  //======================================================
  // Perform intersection
  BOPTools_Parallel::Perform(myRunParallel, aVFaceFace);
  if (UserBreak(aPSOuter))
  {
    return;
  }
  //======================================================
  // Treatment of the results

  for (k = 0; k < aNbFaceFace; ++k)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    BOPAlgo_FaceFace& aFaceFace = aVFaceFace(k);
    aFaceFace.Indices(nF1, nF2);
    if (!aFaceFace.IsDone() || aFaceFace.HasErrors())
    {
      BOPDS_InterfFF& aFF = aFFs.Appended();
      aFF.SetIndices(nF1, nF2);
      aFF.Init(0, 0);
      // Warn about failed intersection of faces
      AddIntersectionFailedWarning(aFaceFace.Face1(), aFaceFace.Face2());
      continue;
    }
    //
    Standard_Boolean bTangentFaces = aFaceFace.TangentFaces();
    Standard_Real    aTolFF        = aFaceFace.TolFF();
    //
    aFaceFace.PrepareLines3D(bSplitCurve);
    //
    aFaceFace.ApplyTrsf();
    //
    const IntTools_SequenceOfCurves&      aCvsX  = aFaceFace.Lines();
    const IntTools_SequenceOfPntOn2Faces& aPntsX = aFaceFace.Points();
    //
    Standard_Integer aNbCurves = aCvsX.Length();
    Standard_Integer aNbPoints = aPntsX.Length();
    //
    if (aNbCurves || aNbPoints)
    {
      myDS->AddInterf(nF1, nF2);
    }
    //
    BOPDS_InterfFF& aFF = aFFs.Appended();
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
    for (Standard_Integer i = 1; i <= aNbCurves; ++i)
    {
      if (UserBreak(aPSOuter))
      {
        return;
      }
      Bnd_Box               aBox;
      const IntTools_Curve& aIC      = aCvsX(i);
      Standard_Boolean      bIsValid = IntTools_Tools::CheckCurve(aIC, aBox);
      if (bIsValid)
      {
        BOPDS_Curve& aNC = aVNC.Appended();
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
    for (Standard_Integer i = 1; i <= aNbPoints; ++i)
    {
      const IntTools_PntOn2Faces& aPi = aPntsX(i);
      const gp_Pnt&               aP  = aPi.P1().Pnt();
      //
      BOPDS_Point& aNP = aVNP.Appended();
      aNP.SetPnt(aP);
    }
  }
}

//=======================================================================
// function : UpdateSavedTolerance
// purpose  : Updates the saved tolerance of the vertices of the edge
//           with new tolerance of edge
//=======================================================================
static void UpdateSavedTolerance(const BOPDS_PDS&              theDS,
                                 const Standard_Integer        theNE,
                                 const Standard_Real           theTolNew,
                                 TColStd_DataMapOfIntegerReal& theMVTol)
{
  const TColStd_ListOfInteger&        aSubShapes = theDS->ShapeInfo(theNE).SubShapes();
  TColStd_ListIteratorOfListOfInteger itSS(aSubShapes);
  for (; itSS.More(); itSS.Next())
  {
    const Standard_Integer nV        = itSS.Value();
    Standard_Real*         pTolSaved = theMVTol.ChangeSeek(nV);
    if (pTolSaved && *pTolSaved < theTolNew)
      *pTolSaved = theTolNew;
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::MakeBlocks(const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPSOuter(theRange, NULL, 4);
  if (myGlue != BOPAlgo_GlueOff)
  {
    return;
  }
  //
  BOPDS_VectorOfInterfFF& aFFs  = myDS->InterfFF();
  Standard_Integer        aNbFF = aFFs.Length();
  Message_ProgressScope   aPS(aPSOuter.Next(), "Building section edges", aNbFF);
  if (!aNbFF)
  {
    return;
  }
  //
  Standard_Boolean                    bExist, bValid2D;
  Standard_Integer                    i, nF1, nF2, aNbC, aNbP, j;
  Standard_Integer                    nV1, nV2;
  Standard_Real                       aT1, aT2;
  Handle(NCollection_BaseAllocator)   aAllocator = new NCollection_IncAllocator;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  TopoDS_Edge                         aES;
  Handle(BOPDS_PaveBlock)             aPBOut;
  //
  //-----------------------------------------------------scope f
  //
  TColStd_ListOfInteger aLSE(aAllocator), aLBV(aAllocator);
  TColStd_MapOfInteger  aMVOnIn(100, aAllocator), aMVCommon(100, aAllocator),
    aMVStick(100, aAllocator), aMVEF(100, aAllocator), aMI(100, aAllocator),
    aMVBounds(100, aAllocator);
  BOPDS_IndexedMapOfPaveBlock                   aMPBOnIn(100, aAllocator);
  BOPDS_MapOfPaveBlock                          aMPBAdd(100, aAllocator), aMPBCommon;
  BOPDS_ListOfPaveBlock                         aLPB(aAllocator);
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks aMSCPB(100, aAllocator);
  TopTools_DataMapOfShapeInteger                aMVI(100, aAllocator);
  BOPDS_DataMapOfPaveBlockListOfPaveBlock       aDMExEdges(100, aAllocator);
  TColStd_DataMapOfIntegerReal                  aMVTol(100, aAllocator);
  TColStd_DataMapOfIntegerInteger               aDMNewSD(100, aAllocator);
  TColStd_DataMapOfIntegerListOfInteger         aDMVLV;
  TColStd_DataMapOfIntegerListOfInteger         aDMBV(100, aAllocator);
  TColStd_DataMapIteratorOfDataMapOfIntegerReal aItMV;
  BOPDS_IndexedMapOfPaveBlock                   aMicroPB(100, aAllocator);
  TopTools_IndexedMapOfShape                    aVertsOnRejectedPB;
  // Map of PaveBlocks with the faces to which it has to be added
  BOPAlgo_DataMapOfPaveBlockListOfInteger aPBFacesMap;
  //
  // The vector aFFToRecheck contains indices of potentially problematic Face-Face intersections
  NCollection_Vector<Standard_Integer> aFFToRecheck;
  // aNbFF may be increased while processing this loop, because it is necessary to recheck
  // some of Face-Face intersections to avoid missing section edges
  // aNbFF will be increased to the number of potentially problematic Face-Face intersections
  const Standard_Integer aNbFFPrev = aNbFF;
  for (i = 0; i < aNbFF; ++i, aPS.Next())
  {
    if (UserBreak(aPS))
    {
      return;
    }
    // after passing through all of Face-Face intersections it is necessary to return
    // to potentially problematic Face-Face intersections and process them one more time
    const Standard_Integer aCurInd = i < aNbFFPrev ? i : aFFToRecheck[i - aNbFFPrev];
    //
    BOPDS_InterfFF& aFF = aFFs(aCurInd);
    aFF.Indices(nF1, nF2);
    //
    BOPDS_VectorOfPoint& aVP = aFF.ChangePoints();
    aNbP                     = aVP.Length();
    BOPDS_VectorOfCurve& aVC = aFF.ChangeCurves();
    aNbC                     = aVC.Length();
    if (!aNbP && !aNbC)
    {
      continue;
    }
    //
    const TopoDS_Face& aF1 = (*(TopoDS_Face*)(&myDS->Shape(nF1)));
    const TopoDS_Face& aF2 = (*(TopoDS_Face*)(&myDS->Shape(nF2)));
    //
    Standard_Real aTolFF = Max(BRep_Tool::Tolerance(aF1), BRep_Tool::Tolerance(aF2));
    //
    BOPDS_FaceInfo& aFI1 = myDS->ChangeFaceInfo(nF1);
    BOPDS_FaceInfo& aFI2 = myDS->ChangeFaceInfo(nF2);
    //
    aMVOnIn.Clear();
    aMVCommon.Clear();
    aMPBOnIn.Clear();
    aMPBCommon.Clear();
    aDMBV.Clear();
    aMVTol.Clear();
    aLSE.Clear();
    //
    myDS->SubShapesOnIn(nF1, nF2, aMVOnIn, aMVCommon, aMPBOnIn, aMPBCommon);
    myDS->SharedEdges(nF1, nF2, aLSE, aAllocator);
    //
    // 1. Treat Points
    for (j = 0; j < aNbP; ++j)
    {
      TopoDS_Vertex            aV;
      BOPDS_CoupleOfPaveBlocks aCPB;
      //
      BOPDS_Point&  aNP = aVP.ChangeValue(j);
      const gp_Pnt& aP  = aNP.Pnt();
      //
      bExist = IsExistingVertex(aP, aTolFF, aMVOnIn);
      if (!bExist)
      {
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
    for (j = 0; j < aNbC; ++j)
    {
      BOPDS_Curve& aNC = aVC.ChangeValue(j);
      // DEBt
      aNC.InitPaveBlock1();
      //
      // In order to avoid problems connected with
      // extending tolerance of vertex while putting
      // (e.g. see "bugs modalg_6 bug26789_1" test case),
      // all not-common vertices will be checked by
      // BndBoxes before putting. For common-vertices,
      // filtering by BndBoxes is not necessary.
      PutPavesOnCurve(aMVOnIn, aMVCommon, aNC, aMI, aMVEF, aMVTol, aDMVLV);
    }

    // if some E-F vertex was put on a curve due to large E-F intersection range,
    // and it also was put on another curve correctly then remove this vertex from
    // the first curve. Detect such case if the distance to curve exceeds aTolR3D.
    FilterPavesOnCurves(aVC, aMVTol);

    for (j = 0; j < aNbC; ++j)
    {
      BOPDS_Curve&          aNC = aVC.ChangeValue(j);
      const IntTools_Curve& aIC = aNC.Curve();
      //
      PutStickPavesOnCurve(aF1, aF2, aMI, aVC, j, aMVStick, aMVTol, aDMVLV);
      // 904/F7
      if (aNbC == 1)
      {
        PutEFPavesOnCurve(aVC, j, aMI, aMVEF, aMVTol, aDMVLV);
      }
      //
      if (aIC.HasBounds())
      {
        aLBV.Clear();
        //
        PutBoundPaveOnCurve(aF1, aF2, aNC, aLBV);
        //
        if (!aLBV.IsEmpty())
        {
          aDMBV.Bind(j, aLBV);
          TColStd_ListIteratorOfListOfInteger aItI(aLBV);
          for (; aItI.More(); aItI.Next())
          {
            aMVBounds.Add(aItI.Value());
          }
        }
      }
    } // for (j=0; j<aNbC; ++j) {

    // Put closing pave if needed
    for (j = 0; j < aNbC; ++j)
    {
      BOPDS_Curve& aNC = aVC.ChangeValue(j);
      PutClosingPaveOnCurve(aNC);
    }
    //

    BOPTools_BoxTree aPBTree;
    {
      // Fill the tree with boxes of pave blocks ON/IN
      // Tree will be build on first selection from the tree.
      const Standard_Integer aNbPB = aMPBOnIn.Extent();
      aPBTree.SetSize(aNbPB);
      for (Standard_Integer iPB = 1; iPB <= aNbPB; ++iPB)
      {
        const Handle(BOPDS_PaveBlock)& aPB = aMPBOnIn(iPB);
        if (!aPB->HasEdge())
          continue;

        if (myDS->ShapeInfo(aPB->OriginalEdge()).HasFlag())
          continue;

        aPBTree.Add(iPB, Bnd_Tools::Bnd2BVH(myDS->ShapeInfo(aPB->Edge()).Box()));
      }
    }

    // Added additional check of Face-Face intersection to avoid missing section edges
    // because of sequence of Face-Face interference processing
    Standard_Boolean isToRecheck = aNbC > 0 && i < aNbFFPrev;
    //
    // 3. Make section edges
    for (j = 0; j < aNbC; ++j)
    {
      BOPDS_Curve&          aNC     = aVC.ChangeValue(j);
      const IntTools_Curve& aIC     = aNC.Curve();
      Standard_Real         aTolR3D = Max(aNC.Tolerance(), aNC.TangentialTolerance());
      //
      BOPDS_ListOfPaveBlock&   aLPBC = aNC.ChangePaveBlocks();
      Handle(BOPDS_PaveBlock)& aPB1  = aNC.ChangePaveBlock1();
      //
      aLPB.Clear();
      aPB1->Update(aLPB, Standard_False);
      //
      if (aLPB.Extent() != 0)
      {
        isToRecheck = false;
      }

      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next())
      {
        Handle(BOPDS_PaveBlock)& aPB = aItLPB.ChangeValue();
        aPB->Indices(nV1, nV2);
        aPB->Range(aT1, aT2);
        //
        if (fabs(aT1 - aT2) < Precision::PConfusion())
        {
          continue;
        }

        // Check validity of the block for the faces:
        // classify bounding and middle points on the curve
        // relatively both faces
        bValid2D = myContext->IsValidBlockForFaces(aT1, aT2, aIC, aF1, aF2, aTolR3D);
        if (!bValid2D)
        {
          continue;
        }
        //
        Standard_Integer nEOut;
        Standard_Real    aTolNew = -1.;
        bExist                   = IsExistingPaveBlock(aPB, aNC, aLSE, nEOut, aTolNew);
        if (bExist)
        {
          // Update edge with new tolerance
          UpdateEdgeTolerance(nEOut, aTolNew);
          // Update aMVTol map with new tolerances of vertices
          UpdateSavedTolerance(myDS, nEOut, aTolNew, aMVTol);
          continue;
        }
        //
        const TopoDS_Vertex& aV1 = (*(TopoDS_Vertex*)(&myDS->Shape(nV1)));
        const TopoDS_Vertex& aV2 = (*(TopoDS_Vertex*)(&myDS->Shape(nV2)));
        //
        // check if the pave block has a valid range
        Standard_Real aFirst, aLast;
        if (!BRepLib::FindValidRange(GeomAdaptor_Curve(aIC.Curve()),
                                     aTolR3D,
                                     aT1,
                                     BRep_Tool::Pnt(aV1),
                                     Max(aTolR3D, BRep_Tool::Tolerance(aV1)),
                                     aT2,
                                     BRep_Tool::Pnt(aV2),
                                     Max(aTolR3D, BRep_Tool::Tolerance(aV2)),
                                     aFirst,
                                     aLast))
        {
          // If the pave block does not have valid range, i.e. it is completely
          // covered by the tolerance spheres of its vertices, it will be
          // passed into post treatment process to fuse its vertices.
          // The pave block itself will not be kept.
          if (!aMVBounds.Contains(nV1) && !aMVBounds.Contains(nV2))
          {
            aMicroPB.Add(aPB);
            // keep vertices for post treatment
            aMVI.Bind(aV1, nV1);
            aMVI.Bind(aV2, nV2);
          }
          continue;
        }
        //
        bExist =
          IsExistingPaveBlock(aPB, aNC, aTolR3D, aMPBOnIn, aPBTree, aMPBCommon, aPBOut, aTolNew);
        if (bExist)
        {
          Standard_Boolean bInF1 =
            (aFI1.PaveBlocksOn().Contains(aPBOut) || aFI1.PaveBlocksIn().Contains(aPBOut));
          Standard_Boolean bInF2 =
            (aFI2.PaveBlocksOn().Contains(aPBOut) || aFI2.PaveBlocksIn().Contains(aPBOut));
          if (!bInF1 || !bInF2)
          {
            // Update edge to touch both faces
            Standard_Integer   nE    = aPBOut->Edge();
            const TopoDS_Edge& aE    = *(TopoDS_Edge*)&myDS->Shape(nE);
            Standard_Real      aTolE = BRep_Tool::Tolerance(aE);
            if (aTolNew < aNC.Tolerance())
              aTolNew = aNC.Tolerance(); // use real tolerance of intersection
            if (aTolNew > aTolE)
            {
              UpdateEdgeTolerance(nE, aTolNew);
              // Update aMVTol map with new tolerances of vertices
              UpdateSavedTolerance(myDS, nE, aTolNew, aMVTol);
            }

            // Face without pave block
            const Standard_Integer nF     = bInF1 ? nF2 : nF1;
            TColStd_ListOfInteger* pFaces = aPBFacesMap.ChangeSeek(aPBOut);
            if (!pFaces)
              pFaces = aPBFacesMap.Bound(aPBOut, TColStd_ListOfInteger());
            // List is expected to be short, so we allow the check here
            if (pFaces->IsEmpty() || !pFaces->Contains(nF))
              pFaces->Append(nF);

            // Try fusing the vertices of the existing pave block
            // with the vertices put on the real section curve (except
            // for technological vertices, which will be removed)
            Standard_Integer nVOut1, nVOut2;
            aPBOut->Indices(nVOut1, nVOut2);
            if (nV1 != nVOut1 && nV1 != nVOut2 && !aMVBounds.Contains(nV1))
            {
              aVertsOnRejectedPB.Add(aV1);
            }
            if (nV2 != nVOut1 && nV2 != nVOut2 && !aMVBounds.Contains(nV2))
            {
              aVertsOnRejectedPB.Add(aV2);
            }

            if (aMPBAdd.Add(aPBOut))
            {
              // Add edge for processing as the section edge
              PreparePostTreatFF(aCurInd, j, aPBOut, aMSCPB, aMVI, aLPBC);
            }
          }
          continue;
        }
        //
        // Make Edge
        BOPTools_AlgoTools::MakeEdge(aIC, aV1, aT1, aV2, aT2, aTolR3D, aES);
        // Make p-curves
        BOPTools_AlgoTools::MakePCurve(aES,
                                       aF1,
                                       aF2,
                                       aIC,
                                       mySectionAttribute.PCurveOnS1(),
                                       mySectionAttribute.PCurveOnS2(),
                                       myContext);
        //
        // Append the Pave Block to the Curve j
        aLPBC.Append(aPB);
        //
        // Keep info for post treatment
        BOPDS_CoupleOfPaveBlocks aCPB;
        aCPB.SetIndexInterf(aCurInd);
        aCPB.SetIndex(j);
        aCPB.SetPaveBlock1(aPB);
        //
        aMSCPB.Add(aES, aCPB);
        aMVI.Bind(aV1, nV1);
        aMVI.Bind(aV2, nV2);
        //
        aMVTol.UnBind(nV1);
        aMVTol.UnBind(nV2);

        // Add existing pave blocks for post treatment
        ProcessExistingPaveBlocks(aCurInd,
                                  j,
                                  nF1,
                                  nF2,
                                  aES,
                                  aMPBOnIn,
                                  aPBTree,
                                  aMSCPB,
                                  aMVI,
                                  aLPBC,
                                  aPBFacesMap,
                                  aMPBAdd);
      }
      //
      aLPBC.RemoveFirst();
    } // for (j=0; j<aNbC; ++j) {
    if (isToRecheck)
    {
      aFFToRecheck.Append(aCurInd);
      ++aNbFF;
    }
    //
    // back to previous tolerance values for unused vertices
    // and forget about SD groups of such vertices
    aItMV.Initialize(aMVTol);
    for (; aItMV.More(); aItMV.Next())
    {
      nV1                = aItMV.Key();
      Standard_Real aTol = aItMV.Value();
      //
      const TopoDS_Vertex&        aV = *(TopoDS_Vertex*)&myDS->Shape(nV1);
      const Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*)&aV.TShape());
      TV->Tolerance(aTol);
      // reset bnd box
      BOPDS_ShapeInfo& aSIDS  = myDS->ChangeShapeInfo(nV1);
      Bnd_Box&         aBoxDS = aSIDS.ChangeBox();
      aBoxDS                  = Bnd_Box();
      BRepBndLib::Add(aV, aBoxDS);
      aBoxDS.SetGap(aBoxDS.GetGap() + Precision::Confusion());
      //
      if (aDMVLV.IsBound(nV1))
        aDMVLV.UnBind(nV1);
    }
    //
    ProcessExistingPaveBlocks(aCurInd,
                              nF1,
                              nF2,
                              aMPBOnIn,
                              aPBTree,
                              aDMBV,
                              aMSCPB,
                              aMVI,
                              aPBFacesMap,
                              aMPBAdd);
  } // for (i=0; i<aNbFF; ++i) {

  // Remove "micro" section edges
  RemoveMicroSectionEdges(aMSCPB, aMicroPB);

  // post treatment
  MakeSDVerticesFF(aDMVLV, aDMNewSD);
  PostTreatFF(aMSCPB,
              aDMExEdges,
              aDMNewSD,
              aMicroPB,
              aVertsOnRejectedPB,
              aAllocator,
              aPSOuter.Next(2));
  if (HasErrors())
  {
    return;
  }
  // reduce tolerances of section edges where it is appropriate
  CorrectToleranceOfSE();
  //
  // update face info
  UpdateFaceInfo(aDMExEdges, aDMNewSD, aPBFacesMap);
  // Update all pave blocks
  UpdatePaveBlocks(aDMNewSD);
  //
  // Treat possible common zones by trying to put each section edge
  // into all faces, not participated in creation of that edge, as IN edge

  PutSEInOtherFaces(aPSOuter.Next());
  //
  //-----------------------------------------------------scope t
  aMVStick.Clear();
  aMPBOnIn.Clear();
  aMVOnIn.Clear();
  aMVCommon.Clear();
  aDMExEdges.Clear();
  aMI.Clear();
  aDMNewSD.Clear();
}

//=================================================================================================

void BOPAlgo_PaveFiller::MakeSDVerticesFF(const TColStd_DataMapOfIntegerListOfInteger& theDMVLV,
                                          TColStd_DataMapOfIntegerInteger&             theDMNewSD)
{
  // Create a new SD vertex for each group of coinciding vertices
  // and put new substitutions to theDMNewSD.
  TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger aItG(theDMVLV);
  for (; aItG.More(); aItG.Next())
  {
    const TColStd_ListOfInteger& aList = aItG.Value();
    // make SD vertices w/o creation of interfs
    Standard_Integer nSD = MakeSDVertices(aList, Standard_False);
    // update theDMNewSD
    TColStd_ListIteratorOfListOfInteger aItL(aList);
    for (; aItL.More(); aItL.Next())
    {
      Standard_Integer nV = aItL.Value();
      theDMNewSD.Bind(nV, nSD);
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::PostTreatFF(BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
                                     BOPDS_DataMapOfPaveBlockListOfPaveBlock&       aDMExEdges,
                                     TColStd_DataMapOfIntegerInteger&               aDMNewSD,
                                     const BOPDS_IndexedMapOfPaveBlock&             theMicroPB,
                                     const TopTools_IndexedMapOfShape&        theVertsOnRejectedPB,
                                     const Handle(NCollection_BaseAllocator)& theAllocator,
                                     const Message_ProgressRange&             theRange)
{
  Standard_Integer aNbS = theMSCPB.Extent();
  if (!aNbS)
  {
    return;
  }
  //
  Standard_Boolean                    bHasPaveBlocks, bOld;
  Standard_Integer                    nSx, nVSD, iX, iP, iC, j, nV, iV = 0, iE, k;
  Standard_Integer                    aNbLPBx;
  TopAbs_ShapeEnum                    aType;
  TopoDS_Shape                        aV, aE;
  TopTools_ListIteratorOfListOfShape  aItLS;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  BOPDS_PDS                           aPDS;
  Handle(BOPDS_PaveBlock)             aPB1;
  BOPDS_Pave                          aPave[2];
  BOPDS_ShapeInfo                     aSI;
  //
  TopTools_ListOfShape aLS(theAllocator);
  BOPAlgo_PaveFiller   aPF(theAllocator);
  aPF.SetIsPrimary(Standard_False);
  aPF.SetNonDestructive(myNonDestructive);
  //
  BOPDS_VectorOfInterfFF& aFFs  = myDS->InterfFF();
  Standard_Integer        aNbFF = aFFs.Length();
  //

  // Find unused vertices
  TopTools_IndexedMapOfShape VertsUnused;
  TColStd_MapOfInteger       IndMap;
  for (Standard_Integer i = 0; i < aNbFF; i++)
  {
    BOPDS_InterfFF&  aFF = aFFs(i);
    Standard_Integer nF1, nF2;
    aFF.Indices(nF1, nF2);

    TColStd_MapOfInteger aMV, aMVEF, aMI;
    GetStickVertices(nF1, nF2, aMV, aMVEF, aMI);
    BOPDS_VectorOfCurve& aVC = aFF.ChangeCurves();
    RemoveUsedVertices(aVC, aMV);

    TColStd_MapIteratorOfMapOfInteger itmap(aMV);
    for (; itmap.More(); itmap.Next())
    {
      Standard_Integer    indV    = itmap.Value();
      const TopoDS_Shape& aVertex = myDS->Shape(indV);
      if (IndMap.Add(indV))
        VertsUnused.Add(aVertex);
      else
        VertsUnused.RemoveKey(aVertex);
    }
  }
  /////////////////////

  Standard_Integer aNbME     = theMicroPB.Extent();
  Standard_Integer aNbVOnRPB = theVertsOnRejectedPB.Extent();
  // 0
  if (aNbS == 1 && (aNbME == 0) && (aNbVOnRPB == 0) && VertsUnused.IsEmpty())
  {
    const TopoDS_Shape&             aS   = theMSCPB.FindKey(1);
    const BOPDS_CoupleOfPaveBlocks& aCPB = theMSCPB.FindFromIndex(1);
    //
    aType = aS.ShapeType();
    if (aType == TopAbs_VERTEX)
    {
      aSI.SetShapeType(aType);
      aSI.SetShape(aS);
      iV = myDS->Append(aSI);
      //
      iX                        = aCPB.IndexInterf();
      iP                        = aCPB.Index();
      BOPDS_InterfFF&      aFF  = aFFs(iX);
      BOPDS_VectorOfPoint& aVNP = aFF.ChangePoints();
      BOPDS_Point&         aNP  = aVNP(iP);
      aNP.SetIndex(iV);
    }
    else if (aType == TopAbs_EDGE)
    {
      aPB1 = aCPB.PaveBlock1();
      //
      if (aPB1->HasEdge())
      {
        BOPDS_ListOfPaveBlock aLPBx;
        aLPBx.Append(aPB1);
        aDMExEdges.Bind(aPB1, aLPBx);
      }
      else
      {
        aSI.SetShapeType(aType);
        aSI.SetShape(aS);
        iE = myDS->Append(aSI);
        //
        aPB1->SetEdge(iE);
      }
    }
    return;
  }
  //
  // 1 prepare arguments
  // Avoid intersection of existing edges among themselves
  TopoDS_Compound anExistingEdges;
  BRep_Builder().MakeCompound(anExistingEdges);
  TopTools_MapOfShape anAddedSD;
  for (k = aNbS; k > 0; --k)
  {
    const TopoDS_Shape&            aS  = theMSCPB.FindKey(k);
    const Handle(BOPDS_PaveBlock)& aPB = theMSCPB(k).PaveBlock1();
    if (!aPB.IsNull() && aPB->HasEdge())
      BRep_Builder().Add(anExistingEdges, aS);
    else
      aLS.Append(aS);
    // add vertices-candidates for SD from the map aDMNewSD,
    // so that they took part in fuse operation.
    TopoDS_Iterator itV(aS);
    for (; itV.More(); itV.Next())
    {
      const TopoDS_Shape&     aVer = itV.Value();
      Standard_Integer        iVer = myDS->Index(aVer);
      const Standard_Integer* pSD  = aDMNewSD.Seek(iVer);
      if (pSD)
      {
        const TopoDS_Shape& aVSD = myDS->Shape(*pSD);
        if (anAddedSD.Add(aVSD))
          aLS.Append(aVSD);
      }
    }
  }
  if (anExistingEdges.NbChildren() > 0)
    aLS.Append(anExistingEdges);
  //
  // The section edges considered as a micro should be
  // specially treated - their vertices should be united and
  // the edge itself should be removed. Thus, we add only
  // its vertices into operation.
  //
  BRep_Builder aBB;
  for (k = 1; k <= aNbME; ++k)
  {
    Standard_Integer nVerts[2];
    theMicroPB(k)->Indices(nVerts[0], nVerts[1]);
    TopoDS_Vertex aVerts[2];
    for (Standard_Integer i = 0; i < 2; ++i)
    {
      const Standard_Integer* pSD = aDMNewSD.Seek(nVerts[i]);
      aVerts[i]                   = TopoDS::Vertex(myDS->Shape(pSD ? *pSD : nVerts[i]));
      if (anAddedSD.Add(aVerts[i]))
        aLS.Append(aVerts[i]);
    }
    //
    if (aVerts[0].IsSame(aVerts[1]))
    {
      continue;
    }
    //
    // make sure these vertices will be united
    const gp_Pnt& aP1 = BRep_Tool::Pnt(aVerts[0]);
    const gp_Pnt& aP2 = BRep_Tool::Pnt(aVerts[1]);
    //
    Standard_Real aDist  = aP1.Distance(aP2);
    Standard_Real aTolV1 = BRep_Tool::Tolerance(aVerts[0]);
    Standard_Real aTolV2 = BRep_Tool::Tolerance(aVerts[1]);
    //
    aDist -= (aTolV1 + aTolV2);
    if (aDist > 0.)
    {
      aDist /= 2.;
      aBB.UpdateVertex(aVerts[0], aTolV1 + aDist);
      aBB.UpdateVertex(aVerts[1], aTolV2 + aDist);
    }
  }

  // Add vertices put on the real section curves to unify them with the
  // vertices of the edges, by which these sections curves have been rejected
  // and with unused vertices
  const TopTools_IndexedMapOfShape* VerMap[2] = {&theVertsOnRejectedPB, &VertsUnused};
  for (Standard_Integer imap = 0; imap < 2; imap++)
  {
    Standard_Integer NbVer = VerMap[imap]->Extent();
    for (Standard_Integer i = 1; i <= NbVer; ++i)
    {
      TopoDS_Shape            aVer = VerMap[imap]->FindKey(i);
      Standard_Integer        iVer = myDS->Index(aVer);
      const Standard_Integer* pSD  = aDMNewSD.Seek(iVer);
      if (pSD)
        aVer = myDS->Shape(*pSD);

      if (anAddedSD.Add(aVer))
        aLS.Append(aVer);
    }
  }
  //
  Message_ProgressScope aPS(theRange, "Intersection of section edges", 1);

  // 2 Fuse shapes
  aPF.SetRunParallel(myRunParallel);
  aPF.SetArguments(aLS);
  aPF.Perform(aPS.Next());
  if (aPF.HasErrors())
  {
    AddError(new BOPAlgo_AlertPostTreatFF);
    return;
  }
  aPDS = aPF.PDS();
  //
  // Map to store the real tolerance of the common block
  // and avoid repeated computation of it
  NCollection_DataMap<Handle(BOPDS_CommonBlock), Standard_Real> aMCBTol;
  // Map to avoid creation of different pave blocks for
  // the same intersection edge
  NCollection_DataMap<Standard_Integer, Handle(BOPDS_PaveBlock)> aMEPB;
  //
  aItLS.Initialize(aLS);
  for (; aItLS.More(); aItLS.Next())
  {
    const TopoDS_Shape& aSx = aItLS.Value();
    if (aSx.ShapeType() == TopAbs_COMPOUND)
    {
      for (TopoDS_Iterator itC(aSx); itC.More(); itC.Next())
        aLS.Append(itC.Value());
      continue;
    }
    nSx                         = aPDS->Index(aSx);
    const BOPDS_ShapeInfo& aSIx = aPDS->ShapeInfo(nSx);
    //
    aType = aSIx.ShapeType();
    //
    if (aType == TopAbs_VERTEX)
    {
      Standard_Boolean bIntersectionPoint = theMSCPB.Contains(aSx);
      //
      if (aPDS->HasShapeSD(nSx, nVSD))
      {
        aV = aPDS->Shape(nVSD);
      }
      else
      {
        aV = aSx;
      }
      // index of new vertex in theDS -> iV
      iV = myDS->Index(aV);
      if (iV < 0)
      {
        aSI.SetShapeType(aType);
        aSI.SetShape(aV);
        iV = myDS->Append(aSI);
      }
      //
      if (!bIntersectionPoint)
      {
        // save SD connection
        nSx = myDS->Index(aSx);
        if (nSx != iV)
        {
          aDMNewSD.Bind(nSx, iV);
          myDS->AddShapeSD(nSx, iV);
        }
      }
      else
      {
        // update FF interference
        const BOPDS_CoupleOfPaveBlocks& aCPB = theMSCPB.FindFromKey(aSx);
        iX                                   = aCPB.IndexInterf();
        iP                                   = aCPB.Index();
        BOPDS_InterfFF&      aFF             = aFFs(iX);
        BOPDS_VectorOfPoint& aVNP            = aFF.ChangePoints();
        BOPDS_Point&         aNP             = aVNP(iP);
        aNP.SetIndex(iV);
      }
    } // if (aType==TopAbs_VERTEX) {
    //
    else if (aType == TopAbs_EDGE)
    {
      bHasPaveBlocks                       = aPDS->HasPaveBlocks(nSx);
      const BOPDS_CoupleOfPaveBlocks& aCPB = theMSCPB.FindFromKey(aSx);
      iX                                   = aCPB.IndexInterf();
      iC                                   = aCPB.Index();
      aPB1                                 = aCPB.PaveBlock1();
      //
      bOld = aPB1->HasEdge();
      if (bOld)
      {
        BOPDS_ListOfPaveBlock aLPBx;
        aDMExEdges.Bind(aPB1, aLPBx);
      }
      //
      if (!bHasPaveBlocks)
      {
        if (bOld)
        {
          aDMExEdges.ChangeFind(aPB1).Append(aPB1);
        }
        else
        {
          aSI.SetShapeType(aType);
          aSI.SetShape(aSx);
          iE = myDS->Append(aSI);
          //
          aPB1->SetEdge(iE);
        }
      }
      else
      {
        BOPDS_InterfFF&        aFF   = aFFs(iX);
        BOPDS_VectorOfCurve&   aVNC  = aFF.ChangeCurves();
        BOPDS_Curve&           aNC   = aVNC(iC);
        BOPDS_ListOfPaveBlock& aLPBC = aNC.ChangePaveBlocks();
        //
        // check if edge occurred to be micro edge;
        // note we check not the edge aSx itself, but its image in aPDS
        const BOPDS_ListOfPaveBlock& aLPBx = aPDS->PaveBlocks(nSx);
        aNbLPBx                            = aLPBx.Extent();
        if (aNbLPBx == 0 || (aNbLPBx == 1 && !aLPBx.First()->HasShrunkData()))
        {
          BOPDS_ListIteratorOfListOfPaveBlock it(aLPBC);
          for (; it.More(); it.Next())
          {
            if (it.Value() == aPB1)
            {
              aLPBC.Remove(it);
              break;
            }
          }

          // The edge became micro edge, check vertices for SD
          TopoDS_Iterator itV(aSx);
          for (; itV.More(); itV.Next())
            aLS.Append(itV.Value());

          continue;
        }
        //
        if (bOld && !aNbLPBx)
        {
          aDMExEdges.ChangeFind(aPB1).Append(aPB1);
          continue;
        }
        //
        if (!bOld)
        {
          aItLPB.Initialize(aLPBC);
          for (; aItLPB.More(); aItLPB.Next())
          {
            const Handle(BOPDS_PaveBlock)& aPBC = aItLPB.Value();
            if (aPBC == aPB1)
            {
              aLPBC.Remove(aItLPB);
              break;
            }
          }
        }
        //
        if (aNbLPBx)
        {
          aItLPB.Initialize(aLPBx);
          for (; aItLPB.More(); aItLPB.Next())
          {
            const Handle(BOPDS_PaveBlock)& aPBx  = aItLPB.Value();
            const Handle(BOPDS_PaveBlock)  aPBRx = aPDS->RealPaveBlock(aPBx);
            //
            // update vertices of paves
            aPave[0] = aPBx->Pave1();
            aPave[1] = aPBx->Pave2();
            for (j = 0; j < 2; ++j)
            {
              nV = aPave[j].Index();
              aV = aPDS->Shape(nV);
              // index of new vertex in myDS -> iV
              iV = myDS->Index(aV);
              if (iV < 0)
              {
                aSI.SetShapeType(TopAbs_VERTEX);
                aSI.SetShape(aV);
                iV = myDS->Append(aSI);
              }
              const BOPDS_Pave& aP1 = !j ? aPB1->Pave1() : aPB1->Pave2();
              if (aP1.Index() != iV)
              {
                if (aP1.Parameter() == aPave[j].Parameter())
                {
                  aDMNewSD.Bind(aP1.Index(), iV);
                  myDS->AddShapeSD(aP1.Index(), iV);
                }
                else
                {
                  // check aPDS to have the SD connection between these vertices
                  const TopoDS_Shape& aVPave = myDS->Shape(aP1.Index());
                  Standard_Integer    nVnewSD, nVnew = aPDS->Index(aVPave);
                  if (aPDS->HasShapeSD(nVnew, nVnewSD))
                  {
                    if (nVnewSD == nV)
                    {
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
            aE = aPDS->Shape(aPBRx->Edge());
            iE = myDS->Index(aE);
            if (iE < 0)
            {
              aSI.SetShapeType(aType);
              aSI.SetShape(aE);
              iE = myDS->Append(aSI);
            }
            //
            // update real edge tolerance according to distances in common block if any
            if (aPDS->IsCommonBlock(aPBRx))
            {
              const Handle(BOPDS_CommonBlock)& aCB  = aPDS->CommonBlock(aPBRx);
              Standard_Real*                   pTol = aMCBTol.ChangeSeek(aCB);
              if (!pTol)
              {
                Standard_Real aTol = BOPAlgo_Tools::ComputeToleranceOfCB(aCB, aPDS, aPF.Context());
                pTol               = aMCBTol.Bound(aCB, aTol);
              }
              //
              if (aNC.Tolerance() < *pTol)
              {
                aNC.SetTolerance(*pTol);
              }
            }
            // append new PaveBlock to aLPBC
            Handle(BOPDS_PaveBlock)* pPBC = aMEPB.ChangeSeek(iE);
            if (!pPBC)
            {
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
            if (bOld)
            {
              (*pPBC)->SetOriginalEdge(aPB1->OriginalEdge());
              aDMExEdges.ChangeFind(aPB1).Append(*pPBC);
            }
            else
            {
              aLPBC.Append(*pPBC);
            }
          }
        }
      }
    } // else if (aType==TopAbs_EDGE)
  } // for (; aItLS.More(); aItLS.Next()) {

  // Update SD for vertices that did not participate in operation
  TColStd_DataMapOfIntegerInteger::Iterator itDM(aDMNewSD);
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

//=================================================================================================

void BOPAlgo_PaveFiller::UpdateFaceInfo(
  BOPDS_DataMapOfPaveBlockListOfPaveBlock&       theDME,
  const TColStd_DataMapOfIntegerInteger&         theDMV,
  const BOPAlgo_DataMapOfPaveBlockListOfInteger& thePBFacesMap)
{
  Standard_Integer                    i, j, nV1, nF1, nF2, aNbFF, aNbC, aNbP;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  TColStd_MapOfInteger                aMF;

  // Unify pave blocks of the existing edges united on the post-treat stage
  NCollection_DataMap<Standard_Integer, BOPDS_ListOfPaveBlock> anEdgeLPB;

  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  aNbFF                        = aFFs.Length();
  // 1. Sections (curves, points);
  for (i = 0; i < aNbFF; ++i)
  {
    BOPDS_InterfFF& aFF = aFFs(i);
    aFF.Indices(nF1, nF2);
    //
    BOPDS_FaceInfo& aFI1 = myDS->ChangeFaceInfo(nF1);
    BOPDS_FaceInfo& aFI2 = myDS->ChangeFaceInfo(nF2);
    //
    // 1.1. Section edges
    BOPDS_VectorOfCurve& aVNC = aFF.ChangeCurves();
    aNbC                      = aVNC.Length();
    for (j = 0; j < aNbC; ++j)
    {
      BOPDS_Curve&           aNC   = aVNC(j);
      BOPDS_ListOfPaveBlock& aLPBC = aNC.ChangePaveBlocks();
      //
      // Add section edges to face info
      aItLPB.Initialize(aLPBC);
      for (; aItLPB.More();)
      {
        const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
        //
        // Treat existing pave blocks
        if (theDME.IsBound(aPB))
        {
          BOPDS_ListOfPaveBlock& aLPB = theDME.ChangeFind(aPB);
          UpdateExistingPaveBlocks(aPB, aLPB, thePBFacesMap);

          BOPDS_ListIteratorOfListOfPaveBlock itLPB(aLPB);
          for (; itLPB.More(); itLPB.Next())
          {
            const Standard_Integer nE      = itLPB.Value()->Edge();
            BOPDS_ListOfPaveBlock* pLPBOnE = anEdgeLPB.ChangeSeek(nE);
            if (!pLPBOnE)
              pLPBOnE = anEdgeLPB.Bound(nE, BOPDS_ListOfPaveBlock());
            pLPBOnE->Append(itLPB.Value());
          }

          aLPBC.Remove(aItLPB);
          continue;
        }
        //
        aFI1.ChangePaveBlocksSc().Add(aPB);
        aFI2.ChangePaveBlocksSc().Add(aPB);
        // Add edge-PB connection
        const Standard_Integer nE      = aPB->Edge();
        BOPDS_ListOfPaveBlock* pLPBOnE = anEdgeLPB.ChangeSeek(nE);
        if (!pLPBOnE)
          pLPBOnE = anEdgeLPB.Bound(nE, BOPDS_ListOfPaveBlock());
        pLPBOnE->Append(aPB);

        aItLPB.Next();
      }
    }
    //
    // 1.2. Section vertices
    const BOPDS_VectorOfPoint& aVNP = aFF.Points();
    aNbP                            = aVNP.Length();
    for (j = 0; j < aNbP; ++j)
    {
      const BOPDS_Point& aNP = aVNP(j);
      nV1                    = aNP.Index();
      if (nV1 < 0)
      {
        continue;
      }
      aFI1.ChangeVerticesSc().Add(nV1);
      aFI2.ChangeVerticesSc().Add(nV1);
    }
    //
    aMF.Add(nF1);
    aMF.Add(nF2);
  }

  Standard_Boolean bNewCB = Standard_False;
  {
    // Unify pave blocks of the existing edges united on the post-treat stage
    // by making new Common blocks from them
    NCollection_DataMap<Standard_Integer, BOPDS_ListOfPaveBlock>::Iterator itDM(anEdgeLPB);
    for (; itDM.More(); itDM.Next())
    {
      const BOPDS_ListOfPaveBlock& aLPB = itDM.Value();
      if (aLPB.Extent() == 1)
        continue;

      bNewCB = Standard_True;

      // Find or create common block
      Handle(BOPDS_CommonBlock) aCB;
      // Collect all faces attached to common blocks
      TColStd_MapOfInteger aMFaces;
      // Collect all pave blocks attached to common blocks
      BOPDS_IndexedMapOfPaveBlock aMPaveBlocks;

      BOPDS_ListIteratorOfListOfPaveBlock itLPB(aLPB);
      for (; itLPB.More(); itLPB.Next())
      {
        const Handle(BOPDS_PaveBlock)& aPB = itLPB.Value();
        aMPaveBlocks.Add(aPB);

        if (myDS->IsCommonBlock(aPB))
        {
          Handle(BOPDS_CommonBlock) aPBCB = myDS->CommonBlock(aPB);
          // Get pave blocks
          const BOPDS_ListOfPaveBlock& aLPBOnCB = aPBCB->PaveBlocks();
          for (BOPDS_ListOfPaveBlock::Iterator it(aLPBOnCB); it.More(); it.Next())
            aMPaveBlocks.Add(it.Value());

          // Get faces
          const TColStd_ListOfInteger& aLFacesOnCB = aPBCB->Faces();
          for (TColStd_ListOfInteger::Iterator it(aLFacesOnCB); it.More(); it.Next())
            aMFaces.Add(it.Value());

          if (aCB.IsNull())
            aCB = aPBCB;
        }
      }

      if (aCB.IsNull())
      {
        // None of the pave blocks in the list is a common block,
        // so create the new one.
        aCB = new BOPDS_CommonBlock;
        aCB->SetPaveBlocks(aLPB);
        itLPB.Initialize(aLPB);
        for (; itLPB.More(); itLPB.Next())
        {
          const Handle(BOPDS_PaveBlock)& aPB = itLPB.Value();
          myDS->SetCommonBlock(aPB, aCB);
        }
      }
      else
      {
        // Update common block with new pave blocks
        BOPDS_ListOfPaveBlock aLPBNew;
        {
          const Standard_Integer aNbPB = aMPaveBlocks.Extent();
          for (Standard_Integer iPB = 1; iPB <= aNbPB; ++iPB)
          {
            const Handle(BOPDS_PaveBlock)& aPB = aMPaveBlocks(iPB);
            myDS->SetCommonBlock(aPB, aCB);
            aLPBNew.Append(aPB);
          }
        }
        aCB->SetPaveBlocks(aLPBNew);

        // Update faces of the common block
        TColStd_ListOfInteger aLFaces;
        for (TColStd_MapOfInteger::Iterator it(aMFaces); it.More(); it.Next())
          aLFaces.Append(it.Value());
        aCB->SetFaces(aLFaces);
      }
    }
  }

  Standard_Boolean bVerts = theDMV.Extent() > 0;
  Standard_Boolean bEdges = theDME.Extent() > 0 || bNewCB;
  //
  if (!bVerts && !bEdges)
  {
    return;
  }
  //
  // 2. Update Face Info information with new vertices and new
  //    pave blocks created in PostTreatFF from existing ones
  Standard_Integer                                 nV2;
  TColStd_MapIteratorOfMapOfInteger                aItMF;
  TColStd_DataMapIteratorOfDataMapOfIntegerInteger aItMV;
  //
  aItMF.Initialize(aMF);
  for (; aItMF.More(); aItMF.Next())
  {
    nF1 = aItMF.Value();
    //
    BOPDS_FaceInfo& aFI = myDS->ChangeFaceInfo(nF1);
    //
    // 2.1. Update information about vertices
    if (bVerts)
    {
      TColStd_MapOfInteger& aMVOn = aFI.ChangeVerticesOn();
      TColStd_MapOfInteger& aMVIn = aFI.ChangeVerticesIn();
      //
      aItMV.Initialize(theDMV);
      for (; aItMV.More(); aItMV.Next())
      {
        nV1 = aItMV.Key();
        nV2 = aItMV.Value();
        //
        if (aMVOn.Remove(nV1))
          aMVOn.Add(nV2);
        //
        if (aMVIn.Remove(nV1))
          aMVIn.Add(nV2);
      } // for (; aItMV.More(); aItMV.Next()) {
    } // if (bVerts) {
    //
    // 2.2. Update information about pave blocks
    if (bEdges)
    {
      BOPDS_MapOfPaveBlock         aMPBFence;
      BOPDS_IndexedMapOfPaveBlock* pMPB[] = {&aFI.ChangePaveBlocksOn(),
                                             &aFI.ChangePaveBlocksIn(),
                                             &aFI.ChangePaveBlocksSc()};
      for (i = 0; i < 3; ++i)
      {
        BOPDS_IndexedMapOfPaveBlock aMPBCopy = *pMPB[i];
        pMPB[i]->Clear();
        const Standard_Integer aNbPB = aMPBCopy.Extent();
        for (j = 1; j <= aNbPB; ++j)
        {
          const Handle(BOPDS_PaveBlock)& aPB  = aMPBCopy(j);
          const BOPDS_ListOfPaveBlock*   pLPB = theDME.Seek(aPB);
          if (pLPB && !pLPB->IsEmpty())
          {
            aItLPB.Initialize(*pLPB);
            for (; aItLPB.More(); aItLPB.Next())
            {
              const Handle(BOPDS_PaveBlock)& aPB1 = aItLPB.Value();
              const Handle(BOPDS_PaveBlock)& aPBR = myDS->RealPaveBlock(aPB1);
              if (aMPBFence.Add(aPBR))
                pMPB[i]->Add(aPBR);
            }
          }
          else
          {
            const Handle(BOPDS_PaveBlock)& aPBR = myDS->RealPaveBlock(aPB);
            if (aMPBFence.Add(aPBR))
              pMPB[i]->Add(aPBR);
          }
        } // for (j = 1; j <= aNbPB; ++j) {
      } // for (i = 0; i < 2; ++i) {
    } // if (bEdges) {
  }
}

//=================================================================================================

Standard_Boolean BOPAlgo_PaveFiller::IsExistingVertex(const gp_Pnt&               aP,
                                                      const Standard_Real         theTolR3D,
                                                      const TColStd_MapOfInteger& aMVOnIn) const
{
  Standard_Boolean                  bRet;
  Standard_Integer                  nV, iFlag;
  Standard_Real                     aTolCheck;
  gp_Pnt                            aPV;
  Bnd_Box                           aBoxP;
  TColStd_MapIteratorOfMapOfInteger aIt;
  //
  aTolCheck = theTolR3D + myFuzzyValue;
  bRet      = Standard_True;
  //
  aBoxP.Add(aP);
  aBoxP.Enlarge(theTolR3D);
  //
  aIt.Initialize(aMVOnIn);
  for (; aIt.More(); aIt.Next())
  {
    nV                           = aIt.Value();
    const BOPDS_ShapeInfo& aSIV  = myDS->ShapeInfo(nV);
    const TopoDS_Vertex&   aV    = (*(TopoDS_Vertex*)(&aSIV.Shape()));
    const Bnd_Box&         aBoxV = aSIV.Box();
    //
    if (!aBoxP.IsOut(aBoxV))
    {
      iFlag = BOPTools_AlgoTools::ComputeVV(aV, aP, aTolCheck);
      if (!iFlag)
      {
        return bRet;
      }
    }
  }
  return !bRet;
}

//=================================================================================================

Standard_Boolean BOPAlgo_PaveFiller::IsExistingPaveBlock(const Handle(BOPDS_PaveBlock)& thePB,
                                                         const BOPDS_Curve&             theNC,
                                                         const TColStd_ListOfInteger&   theLSE,
                                                         Standard_Integer&              theNEOut,
                                                         Standard_Real&                 theTolNew)
{
  if (theLSE.IsEmpty())
    return Standard_False;

  Standard_Real                       aT1, aT2, aTm, aTx, aTolE, aTolCheck, aTol, aDist;
  Standard_Integer                    nE, iFlag, nV1, nV2;
  gp_Pnt                              aPm;
  Bnd_Box                             aBoxPm;
  TColStd_ListIteratorOfListOfInteger aItLI;
  //
  thePB->Range(aT1, aT2);
  thePB->Indices(nV1, nV2);
  const TopoDS_Vertex &aV1   = TopoDS::Vertex(myDS->Shape(nV1)),
                      &aV2   = TopoDS::Vertex(myDS->Shape(nV2));
  const Standard_Real aTolV1 = BRep_Tool::Tolerance(aV1), aTolV2 = BRep_Tool::Tolerance(aV2);

  aTol = Max(aTolV1, aTolV2);

  aTm = IntTools_Tools::IntermediatePoint(aT1, aT2);
  theNC.Curve().D0(aTm, aPm);
  aBoxPm.Add(aPm);
  aBoxPm.Enlarge(aTol);
  //
  aItLI.Initialize(theLSE);
  for (; aItLI.More(); aItLI.Next())
  {
    nE = aItLI.Value();
    if (nE < 0)
      continue;
    const BOPDS_ShapeInfo& aSIE  = myDS->ChangeShapeInfo(nE);
    const Bnd_Box&         aBoxE = aSIE.Box();
    if (!aBoxE.IsOut(aBoxPm))
    {
      const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aSIE.Shape()));
      aTolE                 = BRep_Tool::Tolerance(aE);
      aTolCheck             = Max(aTolE, aTol) + myFuzzyValue;
      iFlag                 = myContext->ComputePE(aPm, aTolCheck, aE, aTx, aDist);
      if (!iFlag)
      {
        theNEOut  = nE;
        theTolNew = aDist;
        return Standard_True;
      }
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean BOPAlgo_PaveFiller::IsExistingPaveBlock(
  const Handle(BOPDS_PaveBlock)&     thePB,
  const BOPDS_Curve&                 theNC,
  const Standard_Real                theTolR3D,
  const BOPDS_IndexedMapOfPaveBlock& theMPBOnIn,
  BOPTools_BoxTree&                  thePBTree,
  const BOPDS_MapOfPaveBlock&        theMPBCommon,
  Handle(BOPDS_PaveBlock)&           aPBOut,
  Standard_Real&                     theTolNew)
{
  const IntTools_Curve& aIC = theNC.Curve();

  Standard_Real aT1, aT2;
  thePB->Range(aT1, aT2);

  Standard_Integer nV11, nV12;
  thePB->Indices(nV11, nV12);

  // first point
  Bnd_Box aBoxP1;
  gp_Pnt  aP1;
  aIC.D0(aT1, aP1);
  aBoxP1.Add(aP1);
  const Standard_Real aTolV11 = BRep_Tool::Tolerance(TopoDS::Vertex(myDS->Shape(nV11)));
  aBoxP1.Enlarge(aTolV11);

  // Find edges intersecting by AABB with the first point
  BOPTools_BoxTreeSelector aSelector;
  aSelector.SetBox(Bnd_Tools::Bnd2BVH(aBoxP1));
  aSelector.SetBVHSet(&thePBTree);
  if (!aSelector.Select())
    return Standard_False;

  // intermediate point
  Bnd_Box                   aBoxPm;
  Standard_Real             aTm = IntTools_Tools::IntermediatePoint(aT1, aT2);
  gp_Pnt                    aPm;
  gp_Vec                    aVTgt1;
  const Handle(Geom_Curve)& aC3d = aIC.Curve();
  aC3d->D1(aTm, aPm, aVTgt1);
  aBoxPm.Add(aPm);
  Standard_Boolean isVtgt1Valid = aVTgt1.SquareMagnitude() > gp::Resolution();
  if (isVtgt1Valid)
    aVTgt1.Normalize();

  // last point
  Bnd_Box aBoxP2;
  gp_Pnt  aP2;
  aIC.D0(aT2, aP2);
  aBoxP2.Add(aP2);
  const Standard_Real aTolV12 = BRep_Tool::Tolerance(TopoDS::Vertex(myDS->Shape(nV12)));
  aBoxP2.Enlarge(aTolV12);

  const Standard_Real aTolV1 = Max(aTolV11, aTolV12) + myFuzzyValue;

  Standard_Real aTolCheck = theTolR3D + myFuzzyValue;

  // Some limit values to define "thin" face when iflag1=iflag2=2 and
  // edge has no common block with any face
  Standard_Real       aMaxTolAdd   = 0.001; // Maximal tolerance of edge allowed
  const Standard_Real aCoeffTolAdd = 10.;   // Coeff to define max. tolerance with help of aTolCheck
  aMaxTolAdd                       = Min(aMaxTolAdd, aCoeffTolAdd * aTolCheck);

  // Look for the existing pave block closest to the section curve
  Standard_Boolean bFound = Standard_False;
  theTolNew               = ::RealLast();

  for (TColStd_ListOfInteger::Iterator it(aSelector.Indices()); it.More(); it.Next())
  {
    const Handle(BOPDS_PaveBlock)& aPB = theMPBOnIn(it.Value());

    Standard_Integer nV21, nV22;
    aPB->Indices(nV21, nV22);

    const Standard_Real aTolV21 = BRep_Tool::Tolerance(TopoDS::Vertex(myDS->Shape(nV21)));
    const Standard_Real aTolV22 = BRep_Tool::Tolerance(TopoDS::Vertex(myDS->Shape(nV22)));
    const Standard_Real aTolV2  = Max(aTolV21, aTolV22) + myFuzzyValue;

    const BOPDS_ShapeInfo& aSISp  = myDS->ChangeShapeInfo(aPB->Edge());
    const TopoDS_Edge&     aSp    = (*(TopoDS_Edge*)(&aSISp.Shape()));
    const Bnd_Box&         aBoxSp = aSISp.Box();

    Standard_Integer iFlag1 = (nV11 == nV21 || nV11 == nV22) ? 2 : 1;
    Standard_Integer iFlag2 = (nV12 == nV21 || nV12 == nV22) ? 2 : (!aBoxSp.IsOut(aBoxP2) ? 1 : 0);
    if (!iFlag2)
      continue;

    Standard_Real aDist  = 0.;
    Standard_Real aCoeff = 1.; // Coeff for taking in account deflections between edge and theNC
    // when aPB is not common block
    Standard_Real    aDistm1m2 = 0.;
    Standard_Integer aPEStatus = 1;

    Standard_Real aRealTol = aTolCheck;
    if (myDS->IsCommonBlock(aPB))
    {
      aRealTol = Max(aRealTol, Max(aTolV1, aTolV2));
      if (theMPBCommon.Contains(aPB))
        // for an edge, which is a common block with a face,
        // increase the chance to coincide with section curve
        aRealTol *= 2.;
    }
    else if (iFlag1 == 2 && iFlag2 == 2)
    {
      // Check, if edge could be common block with section curve
      //  and increase the chance to coincide with section curve
      // skip processing if one edge is closed, but other is not closed
      // such configurations can give iFlag1 == 2 && iFlag2 == 2
      Standard_Boolean bSkipProcessing =
        ((nV11 == nV12) && (nV21 != nV22)) || ((nV11 != nV12) && (nV21 == nV22));

      if (!bSkipProcessing)
      {

        if (isVtgt1Valid)
        {
          BRepAdaptor_Curve aBAC2(aSp);
          if (aIC.Type() != GeomAbs_Line || aBAC2.GetType() != GeomAbs_Line)
          {
            Standard_Real aTldp;
            Standard_Real aTolAdd = 2. * Min(aMaxTolAdd, Max(aRealTol, Max(aTolV1, aTolV2)));
            aPEStatus             = myContext->ComputePE(aPm, aTolAdd, aSp, aTldp, aDistm1m2);

            if (aPEStatus == 0)
            {
              gp_Pnt aPm2;
              gp_Vec aVTgt2;
              aBAC2.D1(aTldp, aPm2, aVTgt2);
              if (aVTgt2.SquareMagnitude() > gp::Resolution())
              {
                // The angle should be close to zero
                Standard_Real aCos = aVTgt1.Dot(aVTgt2.Normalized());
                if (Abs(aCos) >= 0.9063)
                {
                  aRealTol = aTolAdd;
                  aCoeff   = 2.;
                }
              }
            }
          }
        }
      }
    }

    Bnd_Box aBoxTmp = aBoxPm;
    aBoxTmp.Enlarge(aRealTol);

    Standard_Real aDistToSp = 0.;
    Standard_Real aTx;
    if (aBoxSp.IsOut(aBoxTmp) || aPEStatus < 0)
    {
      continue;
    }
    else if (aPEStatus == 0) // aPEStatus == 0 for case iflag1 == iflag2 == 2
    {
      aDistToSp = aDistm1m2;
    }
    else if (aPEStatus == 1) // Projection has not been done yet
    {
      aPEStatus = myContext->ComputePE(aPm, aRealTol, aSp, aTx, aDistToSp);
      if (aPEStatus < 0)
        continue;
    }
    //
    if (iFlag1 == 1)
    {
      iFlag1 = !myContext->ComputePE(aP1, aRealTol, aSp, aTx, aDist);
      if (iFlag1 && aDistToSp < aDist)
        aDistToSp = aDist;
    }
    //
    if (iFlag2 == 1)
    {
      iFlag2 = !myContext->ComputePE(aP2, aRealTol, aSp, aTx, aDist);
      if (iFlag2 && aDistToSp < aDist)
        aDistToSp = aDist;
    }
    //
    if (iFlag1 && iFlag2)
    {
      if (aDistToSp < theTolNew)
      {
        aPBOut    = aPB;
        theTolNew = aCoeff * aDistToSp;
        bFound    = Standard_True;
      }
    }
  }
  return bFound;
}

//=================================================================================================

static void getBoundPaves(const BOPDS_DS*    theDS,
                          const BOPDS_Curve& theNC,
                          Standard_Integer   theNV[2])
{
  theNV[0] = theNV[1] = -1;

  // get extreme paves
  const Handle(BOPDS_PaveBlock)& aPB   = theNC.PaveBlocks().First();
  const BOPDS_ListOfPave&        aLP   = aPB->ExtPaves();
  Standard_Integer               aNbEP = aLP.Extent();
  if (aNbEP == 0)
    return;
  Standard_Real aTmin = RealLast();
  Standard_Real aTmax = -aTmin;
  for (BOPDS_ListIteratorOfListOfPave aItLP(aLP); aItLP.More(); aItLP.Next())
  {
    const BOPDS_Pave& aPv = aItLP.Value();
    Standard_Integer  nV;
    Standard_Real     aTV;
    aPv.Contents(nV, aTV);
    if (aTV < aTmin)
    {
      theNV[0] = aPv.Index();
      aTmin    = aTV;
    }
    if (aTV > aTmax)
    {
      theNV[1] = aPv.Index();
      aTmax    = aTV;
    }
  }

  // compare extreme vertices with ends of the curve
  const IntTools_Curve& aIC = theNC.Curve();
  Standard_Real         aT[2];
  gp_Pnt                aP[2];
  aIC.Bounds(aT[0], aT[1], aP[0], aP[1]);
  Standard_Real aTol = Max(theNC.Tolerance(), theNC.TangentialTolerance());
  aTol += Precision::Confusion();
  for (Standard_Integer j = 0; j < 2; ++j)
  {
    const BOPDS_ShapeInfo& aSIV  = theDS->ShapeInfo(theNV[j]);
    const TopoDS_Vertex&   aV    = (*(TopoDS_Vertex*)(&aSIV.Shape()));
    Standard_Integer       iFlag = BOPTools_AlgoTools::ComputeVV(aV, aP[j], aTol);
    if (iFlag != 0)
      theNV[j] = -1;
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::PutBoundPaveOnCurve(const TopoDS_Face&     aF1,
                                             const TopoDS_Face&     aF2,
                                             BOPDS_Curve&           aNC,
                                             TColStd_ListOfInteger& aLVB)
{
  const IntTools_Curve& aIC = aNC.Curve();
  Standard_Real         aT[2];
  gp_Pnt                aP[2];
  aIC.Bounds(aT[0], aT[1], aP[0], aP[1]);
  Standard_Real            aTolR3D = Max(aNC.Tolerance(), aNC.TangentialTolerance());
  Handle(BOPDS_PaveBlock)& aPB     = aNC.ChangePaveBlock1();
  // Get numbers of vertices assigned to the ends of the curve
  Standard_Integer aBndNV[2];
  getBoundPaves(myDS, aNC, aBndNV);
  //
  Standard_Real    aTolVnew = Precision::Confusion();
  Standard_Boolean isClosed = aP[1].IsEqual(aP[0], aTolVnew);
  if (isClosed && (aBndNV[0] > 0 || aBndNV[1] > 0))
    return;

  for (Standard_Integer j = 0; j < 2; ++j)
  {
    if (aBndNV[j] < 0)
    {
      // no vertex on this end
      if (j && isClosed)
      {
        // if curve is closed, process only one bound
        continue;
      }
      Standard_Boolean bVF = myContext->IsValidPointForFaces(aP[j], aF1, aF2, aTolR3D);
      if (!bVF)
      {
        continue;
      }
      TopoDS_Vertex aVn;
      BOPTools_AlgoTools::MakeNewVertex(aP[j], aTolR3D, aVn);
      BOPTools_AlgoTools::UpdateVertex(aIC, aT[j], aVn);
      aTolVnew = BRep_Tool::Tolerance(aVn);

      BOPDS_ShapeInfo aSIVn;
      aSIVn.SetShapeType(TopAbs_VERTEX);
      aSIVn.SetShape(aVn);

      Bnd_Box& aBox = aSIVn.ChangeBox();
      BRepBndLib::Add(aVn, aBox);
      aBox.SetGap(aBox.GetGap() + Precision::Confusion());

      Standard_Integer nVn = myDS->Append(aSIVn);

      BOPDS_Pave aPn;
      aPn.SetIndex(nVn);
      aPn.SetParameter(aT[j]);
      aPB->AppendExtPave(aPn);

      aLVB.Append(nVn);
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::PutPavesOnCurve(const TColStd_MapOfInteger&            theMVOnIn,
                                         const TColStd_MapOfInteger&            theMVCommon,
                                         BOPDS_Curve&                           theNC,
                                         const TColStd_MapOfInteger&            theMI,
                                         const TColStd_MapOfInteger&            theMVEF,
                                         TColStd_DataMapOfIntegerReal&          theMVTol,
                                         TColStd_DataMapOfIntegerListOfInteger& theDMVLV)
{
  Standard_Integer                  nV;
  TColStd_MapIteratorOfMapOfInteger aIt;
  //
  const Bnd_Box&      aBoxC   = theNC.Box();
  const Standard_Real aTolR3D = Max(theNC.Tolerance(), theNC.TangentialTolerance());
  //
  // Put EF vertices first
  aIt.Initialize(theMVEF);
  for (; aIt.More(); aIt.Next())
  {
    nV = aIt.Value();
    PutPaveOnCurve(nV, aTolR3D, theNC, theMI, theMVTol, theDMVLV, 2);
  }

  // Put all other vertices
  aIt.Initialize(theMVOnIn);
  for (; aIt.More(); aIt.Next())
  {
    nV = aIt.Value();
    if (theMVEF.Contains(nV))
    {
      continue;
    }

    if (!theMVCommon.Contains(nV))
    {
      const BOPDS_ShapeInfo& aSIV  = myDS->ShapeInfo(nV);
      const Bnd_Box&         aBoxV = aSIV.Box();
      //
      if (aBoxC.IsOut(aBoxV))
      {
        continue;
      }
      if (!myDS->IsNewShape(nV))
      {
        continue;
      }
    }
    //
    PutPaveOnCurve(nV, aTolR3D, theNC, theMI, theMVTol, theDMVLV, 1);
  }
}

//=================================================================================================

namespace
{
struct PaveBlockDist
{
  Handle(BOPDS_PaveBlock) PB;
  Standard_Real           SquareDist; // square distance from vertex to the paveblock
  Standard_Real           SinAngle;   // sinus of angle between projection vector
  // and tangent at projection point
  Standard_Real Tolerance; // tolerance of the section curve
};
} // namespace

void BOPAlgo_PaveFiller::FilterPavesOnCurves(const BOPDS_VectorOfCurve&    theVNC,
                                             TColStd_DataMapOfIntegerReal& theMVTol)
{
  // For each vertex found in ExtPaves of pave blocks of section curves
  // collect list of pave blocks with distance to the curve
  NCollection_IndexedDataMap<Standard_Integer, NCollection_List<PaveBlockDist>> aIDMVertPBs;
  Standard_Integer                                                              i;
  const Standard_Real anEps = gp::Resolution();
  for (i = 0; i < theVNC.Length(); ++i)
  {
    const BOPDS_Curve&             aNC     = theVNC(i);
    const IntTools_Curve&          aIC     = aNC.Curve();
    const Standard_Real            aTolR3D = Max(aNC.Tolerance(), aNC.TangentialTolerance());
    GeomAdaptor_Curve              aGAC(aIC.Curve());
    const Handle(BOPDS_PaveBlock)& aPB    = aNC.PaveBlocks().First();
    const BOPDS_ListOfPave&        aPaves = aPB->ExtPaves();
    BOPDS_ListOfPave::Iterator     itPaves(aPaves);
    for (; itPaves.More(); itPaves.Next())
    {
      const BOPDS_Pave&    aPave = itPaves.Value();
      Standard_Integer     nV    = aPave.Index();
      const TopoDS_Vertex& aV    = TopoDS::Vertex(myDS->Shape(nV));
      // compute distance from vertex to the point on curve with vertex parameter
      gp_Pnt        aPV  = BRep_Tool::Pnt(aV);
      Standard_Real aPar = aPave.Parameter();
      gp_Pnt        aPonC;
      gp_Vec        aD1;
      aGAC.D1(aPar, aPonC, aD1);
      gp_Vec        aProjVec(aPV, aPonC);
      Standard_Real aSqDist  = aProjVec.SquareMagnitude();
      Standard_Real aSqD1Mod = aD1.SquareMagnitude();
      Standard_Real aSin     = aProjVec.CrossSquareMagnitude(aD1);
      if (aSqDist > anEps && aSqD1Mod > anEps)
        aSin = sqrt(aSin / aSqDist / aSqD1Mod);
      NCollection_List<PaveBlockDist>* pList = aIDMVertPBs.ChangeSeek(nV);
      if (!pList)
        pList =
          &aIDMVertPBs.ChangeFromIndex(aIDMVertPBs.Add(nV, NCollection_List<PaveBlockDist>()));
      PaveBlockDist aPBD = {aPB, aSqDist, aSin, aTolR3D};
      pList->Append(aPBD);
    }
  }

  // Process each vertex
  const Standard_Real aSinAngleMin = 0.5; // angle below which projection is suspicious
  for (i = 1; i <= aIDMVertPBs.Extent(); i++)
  {
    Standard_Integer                       nV    = aIDMVertPBs.FindKey(i);
    const NCollection_List<PaveBlockDist>& aList = aIDMVertPBs(i);
    // Find a pave with minimal distance
    Standard_Real                             aMinDist = RealLast();
    Handle(BOPDS_PaveBlock)                   aPBMinDist;
    NCollection_List<PaveBlockDist>::Iterator itL(aList);
    for (; itL.More(); itL.Next())
    {
      const PaveBlockDist& aPBD = itL.Value();
      if (aPBD.SquareDist < aMinDist)
      {
        aMinDist   = aPBD.SquareDist;
        aPBMinDist = aPBD.PB;
      }
    }
    // Remove a vertex from a pave block if the distance is greater than the tolerance
    // and there are other pave blocks for which the distance is less than the current.
    // Do not remove a vertex if it is projected on the curve with quite large angle
    // (see test bugs modalg_6 bug27761).

    // Reduce tolerance for the vertex to the value of maximal distance to
    // to section curve on which it will be kept.
    Standard_Real    aMaxDistKept = -1;
    Standard_Boolean isRemoved    = Standard_False;
    for (itL.Init(aList); itL.More(); itL.Next())
    {
      const PaveBlockDist& aPBD       = itL.Value();
      Standard_Real        aCheckDist = 100. * Max(aPBD.Tolerance * aPBD.Tolerance, aMinDist);
      if (aPBD.SquareDist > aCheckDist && aPBD.SinAngle < aSinAngleMin)
      {
        aPBD.PB->RemoveExtPave(nV);
        isRemoved = Standard_True;
      }
      else if (aPBD.SquareDist > aMaxDistKept)
        aMaxDistKept = aPBD.SquareDist;
    }

    if (isRemoved && aMaxDistKept > 0)
    {
      const Standard_Real* pTol = theMVTol.Seek(nV);
      if (pTol)
      {
        const TopoDS_Vertex& aV       = *(TopoDS_Vertex*)&myDS->Shape(nV);
        const Standard_Real  aRealTol = Max(*pTol, sqrt(aMaxDistKept) + Precision::Confusion());
        (*(Handle(BRep_TVertex)*)&aV.TShape())->Tolerance(aRealTol);
      }
    }
  }
}

//=================================================================================================

Standard_Boolean BOPAlgo_PaveFiller::ExtendedTolerance(const Standard_Integer      nV,
                                                       const TColStd_MapOfInteger& aMI,
                                                       Standard_Real&              aTolVExt,
                                                       const Standard_Integer      aType)
{
  Standard_Boolean bFound = Standard_False;
  if (!(myDS->IsNewShape(nV)))
  {
    return bFound;
  }
  //
  Standard_Integer i, k, aNbLines, aNbInt;
  Standard_Real    aT11, aT12, aD1, aD2, aD;
  TopoDS_Vertex    aV;
  gp_Pnt           aPV, aP11, aP12;
  //
  k      = 0;
  aNbInt = 2;
  if (aType == 1)
  {
    aNbInt = 1;
  }
  else if (aType == 2)
  {
    k = 1;
  }
  //
  aV  = (*(TopoDS_Vertex*)(&myDS->Shape(nV)));
  aPV = BRep_Tool::Pnt(aV);
  //
  BOPDS_VectorOfInterfEE& aEEs = myDS->InterfEE();
  BOPDS_VectorOfInterfEF& aEFs = myDS->InterfEF();
  //
  for (; k < aNbInt; ++k)
  {
    aNbLines = !k ? aEEs.Length() : aEFs.Length();
    for (i = 0; i < aNbLines; ++i)
    {
      BOPDS_Interf* aInt = !k ? (BOPDS_Interf*)(&aEEs(i)) : (BOPDS_Interf*)(&aEFs(i));
      if (aInt->IndexNew() == nV)
      {
        if (aMI.Contains(aInt->Index1()) && aMI.Contains(aInt->Index2()))
        {
          const IntTools_CommonPrt& aComPrt = !k ? aEEs(i).CommonPart() : aEFs(i).CommonPart();
          //
          const TopoDS_Edge& aE1 = aComPrt.Edge1();
          aComPrt.Range1(aT11, aT12);
          BOPTools_AlgoTools::PointOnEdge(aE1, aT11, aP11);
          BOPTools_AlgoTools::PointOnEdge(aE1, aT12, aP12);
          aD1 = aPV.Distance(aP11);
          aD2 = aPV.Distance(aP12);
          aD  = (aD1 > aD2) ? aD1 : aD2;
          if (aD > aTolVExt)
          {
            aTolVExt = aD;
          }
          return !bFound;
        } // if (aMI.Contains(aEF.Index1()) && aMI.Contains(aEF.Index2())) {
      } // if (aInt->IndexNew() == nV) {
    } // for (i = 0; i < aNbLines; ++i) {
  } // for (k=0; k<2; ++k) {
  return bFound;
}

//=================================================================================================

void BOPAlgo_PaveFiller::GetEFPnts(const Standard_Integer nF1,
                                   const Standard_Integer nF2,
                                   IntSurf_ListOfPntOn2S& aListOfPnts)
{
  Standard_Integer     nE, nF, nFOpposite, aNbEFs, i;
  Standard_Real        U1, U2, V1, V2, f, l;
  TColStd_MapOfInteger aMI;
  //
  // collect indexes of all shapes from nF1 and nF2.
  GetFullShapeMap(nF1, aMI);
  GetFullShapeMap(nF2, aMI);
  //
  BOPDS_VectorOfInterfEF& aEFs = myDS->InterfEF();
  aNbEFs                       = aEFs.Length();
  //
  for (i = 0; i < aNbEFs; ++i)
  {
    const BOPDS_InterfEF& aEF = aEFs(i);
    if (aEF.HasIndexNew())
    {
      aEF.Indices(nE, nFOpposite);
      if (aMI.Contains(nE) && aMI.Contains(nFOpposite))
      {
        const IntTools_CommonPrt& aCP        = aEF.CommonPart();
        Standard_Real             aPar       = aCP.VertexParameter1();
        const TopoDS_Edge&        aE         = (*(TopoDS_Edge*)(&myDS->Shape(nE)));
        const TopoDS_Face&        aFOpposite = (*(TopoDS_Face*)(&myDS->Shape(nFOpposite)));
        //
        const Handle(Geom_Curve)& aCurve = BRep_Tool::Curve(aE, f, l);
        //
        nF                           = (nFOpposite == nF1) ? nF2 : nF1;
        const TopoDS_Face&   aF      = (*(TopoDS_Face*)(&myDS->Shape(nF)));
        Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(aE, aF, f, l);
        //
        GeomAPI_ProjectPointOnSurf& aProj = myContext->ProjPS(aFOpposite);
        //
        gp_Pnt aPoint;
        aCurve->D0(aPar, aPoint);
        IntSurf_PntOn2S aPnt;
        if (!aPCurve.IsNull())
        {
          gp_Pnt2d aP2d = aPCurve->Value(aPar);
          aProj.Perform(aPoint);
          if (aProj.IsDone())
          {
            aProj.LowerDistanceParameters(U1, V1);
            if (nF == nF1)
            {
              aPnt.SetValue(aP2d.X(), aP2d.Y(), U1, V1);
            }
            else
            {
              aPnt.SetValue(U1, V1, aP2d.X(), aP2d.Y());
            }
            aListOfPnts.Append(aPnt);
          }
        }
        else
        {
          GeomAPI_ProjectPointOnSurf& aProj1 = myContext->ProjPS(aF);
          aProj1.Perform(aPoint);
          aProj.Perform(aPoint);
          if (aProj1.IsDone() && aProj.IsDone())
          {
            aProj1.LowerDistanceParameters(U1, V1);
            aProj.LowerDistanceParameters(U2, V2);
            if (nF == nF1)
            {
              aPnt.SetValue(U1, V1, U2, V2);
            }
            else
            {
              aPnt.SetValue(U2, V2, U1, V1);
            }
            aListOfPnts.Append(aPnt);
          }
        }
      }
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::PutEFPavesOnCurve(const BOPDS_VectorOfCurve&             theVC,
                                           const Standard_Integer                 theIndex,
                                           const TColStd_MapOfInteger&            aMI,
                                           const TColStd_MapOfInteger&            aMVEF,
                                           TColStd_DataMapOfIntegerReal&          aMVTol,
                                           TColStd_DataMapOfIntegerListOfInteger& aDMVLV)
{
  if (!aMVEF.Extent())
  {
    return;
  }
  //
  const BOPDS_Curve&    aNC = theVC.Value(theIndex);
  const IntTools_Curve& aIC = aNC.Curve();
  GeomAbs_CurveType     aTypeC;
  aTypeC = aIC.Type();
  if (!(aTypeC == GeomAbs_BezierCurve || aTypeC == GeomAbs_BSplineCurve))
  {
    return;
  }
  //
  Standard_Integer     nV;
  TColStd_MapOfInteger aMV;
  //
  aMV.Assign(aMVEF);
  RemoveUsedVertices(theVC, aMV);
  if (!aMV.Extent())
  {
    return;
  }
  //
  Standard_Real aDist;
  BOPDS_Pave    aPave;
  //
  const Handle(Geom_Curve)&    aC3D    = aIC.Curve();
  GeomAPI_ProjectPointOnCurve& aProjPT = myContext->ProjPT(aC3D);
  //
  TColStd_MapIteratorOfMapOfInteger aItMI;
  aItMI.Initialize(aMV);
  for (; aItMI.More(); aItMI.Next())
  {
    nV                       = aItMI.Value();
    const TopoDS_Vertex& aV  = (*(TopoDS_Vertex*)(&myDS->Shape(nV)));
    gp_Pnt               aPV = BRep_Tool::Pnt(aV);
    aProjPT.Perform(aPV);
    Standard_Integer aNbPoints = aProjPT.NbPoints();
    if (aNbPoints)
    {
      aDist = aProjPT.LowerDistance();
      PutPaveOnCurve(nV, aDist, aNC, aMI, aMVTol, aDMVLV);
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::PutStickPavesOnCurve(const TopoDS_Face&                     aF1,
                                              const TopoDS_Face&                     aF2,
                                              const TColStd_MapOfInteger&            aMI,
                                              const BOPDS_VectorOfCurve&             theVC,
                                              const Standard_Integer                 theIndex,
                                              const TColStd_MapOfInteger&            aMVStick,
                                              TColStd_DataMapOfIntegerReal&          aMVTol,
                                              TColStd_DataMapOfIntegerListOfInteger& aDMVLV)
{
  const BOPDS_Curve& aNC = theVC.Value(theIndex);
  // Get numbers of vertices assigned to the ends of the curve
  Standard_Integer aBndNV[2];
  getBoundPaves(myDS, aNC, aBndNV);
  if (aBndNV[0] >= 0 && aBndNV[1] >= 0)
  {
    // both curve ends already have assigned vertices
    return;
  }
  TColStd_MapOfInteger aMV;
  aMV.Assign(aMVStick);
  RemoveUsedVertices(theVC, aMV);
  //
  if (!aMV.Extent())
  {
    return;
  }
  //
  Handle(Geom_Surface) aS1 = BRep_Tool::Surface(aF1);
  Handle(Geom_Surface) aS2 = BRep_Tool::Surface(aF2);
  //
  const IntTools_Curve& aIC = aNC.Curve();
  Handle(Geom2d_Curve)  aC2D[2];
  //
  aC2D[0] = aIC.FirstCurve2d();
  aC2D[1] = aIC.SecondCurve2d();
  if (!aC2D[0].IsNull() && !aC2D[1].IsNull())
  {
    Standard_Integer                  nV, m, n;
    Standard_Real                     aTC[2], aD, aD2, u, v, aDT2, aScPr, aDScPr;
    gp_Pnt                            aPC[2], aPV;
    gp_Dir                            aDN[2];
    gp_Pnt2d                          aP2D;
    TColStd_MapIteratorOfMapOfInteger aItMI, aItMI1;
    //
    aDT2   = 2e-7;  // the rich criteria
    aDScPr = 5.e-9; // the creasing criteria
    aIC.Bounds(aTC[0], aTC[1], aPC[0], aPC[1]);
    //
    aItMI.Initialize(aMV);
    for (; aItMI.More(); aItMI.Next())
    {
      nV                      = aItMI.Value();
      const TopoDS_Vertex& aV = *((TopoDS_Vertex*)&myDS->Shape(nV));
      aPV                     = BRep_Tool::Pnt(aV);
      //
      for (m = 0; m < 2; ++m)
      {
        if (aBndNV[m] >= 0)
          continue;
        aD2 = aPC[m].SquareDistance(aPV);
        if (aD2 > aDT2)
        { // no rich
          continue;
        }
        //
        for (n = 0; n < 2; ++n)
        {
          Handle(Geom_Surface)& aS = (!n) ? aS1 : aS2;
          aC2D[n]->D0(aTC[m], aP2D);
          aP2D.Coord(u, v);
          BOPTools_AlgoTools3D::GetNormalToSurface(aS, u, v, aDN[n]);
        }
        //
        aScPr = aDN[0] * aDN[1];
        if (aScPr < 0.)
        {
          aScPr = -aScPr;
        }
        aScPr = 1. - aScPr;
        //
        if (aScPr > aDScPr)
        {
          continue;
        }
        //
        // The intersection curve aIC is vanishing curve (the crease)
        aD = sqrt(aD2);
        //
        PutPaveOnCurve(nV, aD, aNC, aMI, aMVTol, aDMVLV);
      }
    } // for (jVU=1; jVU=aNbVU; ++jVU) {
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::GetStickVertices(const Standard_Integer nF1,
                                          const Standard_Integer nF2,
                                          TColStd_MapOfInteger&  aMVStick,
                                          TColStd_MapOfInteger&  aMVEF,
                                          TColStd_MapOfInteger&  aMI)
{
  Standard_Integer nS1, nS2, nVNew, aTypeInt, i;
  //
  BOPDS_VectorOfInterfVV& aVVs = myDS->InterfVV();
  BOPDS_VectorOfInterfVE& aVEs = myDS->InterfVE();
  BOPDS_VectorOfInterfEE& aEEs = myDS->InterfEE();
  BOPDS_VectorOfInterfVF& aVFs = myDS->InterfVF();
  BOPDS_VectorOfInterfEF& aEFs = myDS->InterfEF();
  //
  Standard_Integer aNbLines[5] = {aVVs.Length(),
                                  aVEs.Length(),
                                  aEEs.Length(),
                                  aVFs.Length(),
                                  aEFs.Length()};
  // collect indices of all shapes from nF1 and nF2.
  aMI.Clear();
  GetFullShapeMap(nF1, aMI);
  GetFullShapeMap(nF2, aMI);
  //
  // collect VV, VE, EE, VF interferences
  for (aTypeInt = 0; aTypeInt < 4; ++aTypeInt)
  {
    for (i = 0; i < aNbLines[aTypeInt]; ++i)
    {
      BOPDS_Interf* aInt = (aTypeInt == 0)
                             ? (BOPDS_Interf*)(&aVVs(i))
                             : ((aTypeInt == 1) ? (BOPDS_Interf*)(&aVEs(i))
                                                : ((aTypeInt == 2) ? (BOPDS_Interf*)(&aEEs(i))
                                                                   : (BOPDS_Interf*)(&aVFs(i))));
      if (aInt->HasIndexNew())
      {
        aInt->Indices(nS1, nS2);
        if (aMI.Contains(nS1) && aMI.Contains(nS2))
        {
          nVNew = aInt->IndexNew();
          myDS->HasShapeSD(nVNew, nVNew);
          aMVStick.Add(nVNew);
        }
      }
    }
  }
  // collect EF interferences
  for (i = 0; i < aNbLines[4]; ++i)
  {
    const BOPDS_InterfEF& aInt = aEFs(i);
    if (aInt.HasIndexNew())
    {
      aInt.Indices(nS1, nS2);
      if (aMI.Contains(nS1) && aMI.Contains(nS2))
      {
        nVNew = aInt.IndexNew();
        myDS->HasShapeSD(nVNew, nVNew);
        aMVStick.Add(nVNew);
        aMVEF.Add(nVNew);
      }
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::GetFullShapeMap(const Standard_Integer nF, TColStd_MapOfInteger& aMI)
{
  TColStd_ListIteratorOfListOfInteger aIt;
  Standard_Integer                    nS;
  //
  const BOPDS_ShapeInfo&       aSI = myDS->ShapeInfo(nF);
  const TColStd_ListOfInteger& aLI = aSI.SubShapes();
  //
  aMI.Add(nF);
  aIt.Initialize(aLI);
  for (; aIt.More(); aIt.Next())
  {
    nS = aIt.Value();
    aMI.Add(nS);
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::RemoveUsedVertices(const BOPDS_VectorOfCurve& aVC,
                                            TColStd_MapOfInteger&      aMV)
{
  if (aMV.IsEmpty())
    return;

  for (Standard_Integer i = 0; i < aVC.Length(); ++i)
  {
    const BOPDS_Curve&                  aNC   = aVC.Value(i);
    const BOPDS_ListOfPaveBlock&        aLPBC = aNC.PaveBlocks();
    BOPDS_ListIteratorOfListOfPaveBlock itPB(aLPBC);
    for (; itPB.More(); itPB.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPB = itPB.Value();
      const BOPDS_ListOfPave&        aLP = aPB->ExtPaves();
      BOPDS_ListIteratorOfListOfPave itLP(aLP);
      for (; itLP.More(); itLP.Next())
        aMV.Remove(itLP.Value().Index());

      aMV.Remove(aPB->Pave1().Index());
      aMV.Remove(aPB->Pave2().Index());
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::PutPaveOnCurve(const Standard_Integer                 nV,
                                        const Standard_Real                    aTolR3D,
                                        const BOPDS_Curve&                     aNC,
                                        const TColStd_MapOfInteger&            aMI,
                                        TColStd_DataMapOfIntegerReal&          aMVTol,
                                        TColStd_DataMapOfIntegerListOfInteger& aDMVLV,
                                        const Standard_Integer                 iCheckExtend)
{
  Standard_Boolean bIsVertexOnLine;
  Standard_Real    aT;
  //
  const TopoDS_Vertex&           aV  = (*(TopoDS_Vertex*)(&myDS->Shape(nV)));
  const Handle(BOPDS_PaveBlock)& aPB = aNC.PaveBlocks().First();
  const IntTools_Curve&          aIC = aNC.Curve();
  //
  Standard_Real aTolV = (aMVTol.IsBound(nV) ? aMVTol(nV) : BRep_Tool::Tolerance(aV));

  bIsVertexOnLine = myContext->IsVertexOnLine(aV, aTolV, aIC, aTolR3D + myFuzzyValue, aT);
  if (!bIsVertexOnLine && iCheckExtend && !myVertsToAvoidExtension.Contains(nV))
  {
    Standard_Real anExtraTol = aTolV;
    if (ExtendedTolerance(nV, aMI, anExtraTol, iCheckExtend))
    {
      bIsVertexOnLine = myContext->IsVertexOnLine(aV, anExtraTol, aIC, aTolR3D + myFuzzyValue, aT);
      if (bIsVertexOnLine)
      {
        gp_Pnt aPOnC;
        aIC.D0(aT, aPOnC);
        aTolV = aPOnC.Distance(BRep_Tool::Pnt(aV));
      }
    }
  }
  //
  if (bIsVertexOnLine)
  {
    // check if aPB contains the parameter aT
    Standard_Boolean bExist;
    Standard_Integer nVUsed;
    Standard_Real    aPTol, aDTol;
    //
    aDTol = BOPTools_AlgoTools::DTolerance();
    //
    GeomAdaptor_Curve aGAC(aIC.Curve());
    aPTol = aGAC.Resolution(Max(aTolR3D, aTolV));
    //
    bExist = aPB->ContainsParameter(aT, aPTol, nVUsed);
    if (bExist)
    {
      // use existing pave
      TColStd_ListOfInteger* pList = aDMVLV.ChangeSeek(nVUsed);
      if (!pList)
      {
        pList = aDMVLV.Bound(nVUsed, TColStd_ListOfInteger());
        pList->Append(nVUsed);
        if (!aMVTol.IsBound(nVUsed))
        {
          const TopoDS_Vertex& aVUsed = (*(TopoDS_Vertex*)(&myDS->Shape(nVUsed)));
          aTolV                       = BRep_Tool::Tolerance(aVUsed);
          aMVTol.Bind(nVUsed, aTolV);
        }
      }
      // avoid repeated elements in the list
      TColStd_ListIteratorOfListOfInteger aItLI(*pList);
      for (; aItLI.More(); aItLI.Next())
      {
        if (aItLI.Value() == nV)
        {
          break;
        }
      }
      if (!aItLI.More())
      {
        pList->Append(nV);
      }
      // save initial tolerance for the vertex
      if (!aMVTol.IsBound(nV))
      {
        aTolV = BRep_Tool::Tolerance(aV);
        aMVTol.Bind(nV, aTolV);
      }
    }
    else
    {
      // add new pave
      BOPDS_Pave aPave;
      aPave.SetIndex(nV);
      aPave.SetParameter(aT);
      aPB->AppendExtPave(aPave);
      //
      gp_Pnt aP1          = aGAC.Value(aT);
      aTolV               = BRep_Tool::Tolerance(aV);
      gp_Pnt        aP2   = BRep_Tool::Pnt(aV);
      Standard_Real aDist = aP1.Distance(aP2);
      if (aTolV < aDist + aDTol)
      {
        BRep_Builder().UpdateVertex(aV, aDist + aDTol);
        //
        if (!aMVTol.IsBound(nV))
        {
          aMVTol.Bind(nV, aTolV);
        }
        //
        BOPDS_ShapeInfo& aSIDS  = myDS->ChangeShapeInfo(nV);
        Bnd_Box&         aBoxDS = aSIDS.ChangeBox();
        BRepBndLib::Add(aV, aBoxDS);
        aBoxDS.SetGap(aBoxDS.GetGap() + Precision::Confusion());
      }
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::ProcessExistingPaveBlocks(
  const Standard_Integer                         theInt,
  const Standard_Integer                         theCur,
  const Standard_Integer                         nF1,
  const Standard_Integer                         nF2,
  const TopoDS_Edge&                             theES,
  const BOPDS_IndexedMapOfPaveBlock&             theMPBOnIn,
  BOPTools_BoxTree&                              thePBTree,
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
  TopTools_DataMapOfShapeInteger&                theMVI,
  BOPDS_ListOfPaveBlock&                         theLPBC,
  BOPAlgo_DataMapOfPaveBlockListOfInteger&       thePBFacesMap,
  BOPDS_MapOfPaveBlock&                          theMPB)
{
  Bnd_Box aBoxES;
  BRepBndLib::Add(theES, aBoxES, false);

  BOPTools_BoxTreeSelector aSelector;
  aSelector.SetBox(Bnd_Tools::Bnd2BVH(aBoxES));
  aSelector.SetBVHSet(&thePBTree);
  if (!aSelector.Select())
    return;

  const Standard_Real aTolES = BRep_Tool::Tolerance(theES);

  const BOPDS_FaceInfo& aFI1 = myDS->FaceInfo(nF1);
  const BOPDS_FaceInfo& aFI2 = myDS->FaceInfo(nF2);

  for (TColStd_ListOfInteger::Iterator itPB(aSelector.Indices()); itPB.More(); itPB.Next())
  {
    const Handle(BOPDS_PaveBlock)& aPBF = theMPBOnIn(itPB.Value());
    if (theMPB.Contains(aPBF))
      continue;

    Standard_Boolean bInF1 =
      (aFI1.PaveBlocksOn().Contains(aPBF) || aFI1.PaveBlocksIn().Contains(aPBF));
    Standard_Boolean bInF2 =
      (aFI2.PaveBlocksOn().Contains(aPBF) || aFI2.PaveBlocksIn().Contains(aPBF));
    if (bInF1 && bInF2)
    {
      // Add all common edges for post treatment
      theMPB.Add(aPBF);
      PreparePostTreatFF(theInt, theCur, aPBF, theMSCPB, theMVI, theLPBC);
      continue;
    }

    const Standard_Integer                     nF = bInF1 ? nF2 : nF1;
    const NCollection_List<EdgeRangeDistance>* pList =
      myDistances.Seek(BOPDS_Pair(aPBF->OriginalEdge(), nF));
    if (!pList)
      continue;

    Standard_Real aT1, aT2;
    aPBF->Range(aT1, aT2);

    Standard_Real aDist = RealLast();
    for (NCollection_List<EdgeRangeDistance>::Iterator itR(*pList); itR.More(); itR.Next())
    {
      const EdgeRangeDistance& aRangeDist = itR.Value();
      if ((aT1 <= aRangeDist.First && aRangeDist.First <= aT2)
          || (aT1 <= aRangeDist.Last && aRangeDist.Last <= aT2)
          || (aRangeDist.First <= aT1 && aT1 <= aRangeDist.Last)
          || (aRangeDist.First <= aT2 && aT2 <= aRangeDist.Last))
      {
        aDist = aRangeDist.Distance;
        break;
      }
    }
    if (aDist < RealLast())
    {
      const TopoDS_Edge&  aEF     = TopoDS::Edge(myDS->Shape(aPBF->Edge()));
      const Standard_Real aTolSum = aTolES + BRep_Tool::Tolerance(aEF);

      if (aDist <= aTolSum)
      {
        theMPB.Add(aPBF);
        PreparePostTreatFF(theInt, theCur, aPBF, theMSCPB, theMVI, theLPBC);

        TColStd_ListOfInteger* pFaces = thePBFacesMap.ChangeSeek(aPBF);
        if (!pFaces)
          pFaces = thePBFacesMap.Bound(aPBF, TColStd_ListOfInteger());
        if (pFaces->IsEmpty() || !pFaces->Contains(nF))
          pFaces->Append(nF);
      }
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::ProcessExistingPaveBlocks(
  const Standard_Integer                         theInt,
  const Standard_Integer                         nF1,
  const Standard_Integer                         nF2,
  const BOPDS_IndexedMapOfPaveBlock&             aMPBOnIn,
  BOPTools_BoxTree&                              thePBTree,
  const TColStd_DataMapOfIntegerListOfInteger&   aDMBV,
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& aMSCPB,
  TopTools_DataMapOfShapeInteger&                aMVI,
  BOPAlgo_DataMapOfPaveBlockListOfInteger&       thePBFacesMap,
  BOPDS_MapOfPaveBlock&                          aMPB)
{
  if (aDMBV.IsEmpty())
  {
    return;
  }
  //
  Standard_Real                                          aT, dummy;
  Standard_Integer                                       nV, nE, iC, iFlag;
  TColStd_ListIteratorOfListOfInteger                    aItLI;
  TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger aItBV;
  //
  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  BOPDS_InterfFF&         aFF  = aFFs(theInt);
  BOPDS_VectorOfCurve&    aVC  = aFF.ChangeCurves();
  //
  const BOPDS_FaceInfo& aFI1 = myDS->FaceInfo(nF1);
  const BOPDS_FaceInfo& aFI2 = myDS->FaceInfo(nF2);
  //
  aItBV.Initialize(aDMBV);
  for (; aItBV.More(); aItBV.Next())
  {
    iC                                = aItBV.Key();
    const TColStd_ListOfInteger& aLBV = aItBV.Value();
    //
    BOPDS_Curve&           aNC   = aVC.ChangeValue(iC);
    BOPDS_ListOfPaveBlock& aLPBC = aNC.ChangePaveBlocks();
    //
    aItLI.Initialize(aLBV);
    for (; aItLI.More(); aItLI.Next())
    {
      nV                           = aItLI.Value();
      const BOPDS_ShapeInfo& aSIV  = myDS->ShapeInfo(nV);
      const Bnd_Box&         aBoxV = aSIV.Box();
      const TopoDS_Vertex&   aV    = *(TopoDS_Vertex*)&aSIV.Shape();
      if (!aMVI.IsBound(aV))
      {
        continue;
      }
      //
      BOPTools_BoxTreeSelector aSelector;
      aSelector.SetBox(Bnd_Tools::Bnd2BVH(aBoxV));
      aSelector.SetBVHSet(&thePBTree);
      if (!aSelector.Select())
        continue;

      for (TColStd_ListOfInteger::Iterator it(aSelector.Indices()); it.More(); it.Next())
      {
        const Handle(BOPDS_PaveBlock)& aPB = aMPBOnIn(it.Value());
        if (aPB->Pave1().Index() == nV || aPB->Pave2().Index() == nV)
        {
          continue;
        }
        //
        if (aMPB.Contains(aPB))
        {
          continue;
        }
        nE                          = aPB->Edge();
        const BOPDS_ShapeInfo& aSIE = myDS->ShapeInfo(nE);
        const TopoDS_Edge&     aE   = *(TopoDS_Edge*)&aSIE.Shape();
        //
        iFlag = myContext->ComputeVE(aV, aE, aT, dummy, myFuzzyValue);
        if (!iFlag)
        {
          aMPB.Add(aPB);
          PreparePostTreatFF(theInt, iC, aPB, aMSCPB, aMVI, aLPBC);

          // Add faces to PB
          Standard_Boolean bInF1 =
            (aFI1.PaveBlocksOn().Contains(aPB) || aFI1.PaveBlocksIn().Contains(aPB));
          Standard_Boolean bInF2 =
            (aFI2.PaveBlocksOn().Contains(aPB) || aFI2.PaveBlocksIn().Contains(aPB));
          if (!bInF1 || !bInF2)
          {
            // Face without pave block
            const Standard_Integer nF     = bInF1 ? nF2 : nF1;
            TColStd_ListOfInteger* pFaces = thePBFacesMap.ChangeSeek(aPB);
            if (!pFaces)
              pFaces = thePBFacesMap.Bound(aPB, TColStd_ListOfInteger());
            // List is expected to be short, so we allow the check here
            if (pFaces->IsEmpty() || !pFaces->Contains(nF))
              pFaces->Append(nF);
          }
        }
      }
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::UpdateExistingPaveBlocks(
  const Handle(BOPDS_PaveBlock)&                 aPBf,
  BOPDS_ListOfPaveBlock&                         aLPB,
  const BOPAlgo_DataMapOfPaveBlockListOfInteger& thePBFacesMap)
{
  if (!aLPB.Extent())
  {
    return;
  }
  //
  Standard_Integer                    nE;
  Standard_Boolean                    bCB;
  Handle(BOPDS_PaveBlock)             aPB, aPB1, aPB2, aPB2n;
  Handle(BOPDS_CommonBlock)           aCB;
  BOPDS_ListIteratorOfListOfPaveBlock aIt, aIt1, aIt2;
  //
  // 1. Remove old pave blocks
  const Handle(BOPDS_CommonBlock)& aCB1 = myDS->CommonBlock(aPBf);
  bCB                                   = !aCB1.IsNull();
  BOPDS_ListOfPaveBlock aLPB1;
  //
  if (bCB)
  {
    aLPB1.Assign(aCB1->PaveBlocks());
  }
  else
  {
    aLPB1.Append(aPBf);
  }
  aIt1.Initialize(aLPB1);
  for (; aIt1.More(); aIt1.Next())
  {
    aPB1 = aIt1.Value();
    nE   = aPB1->OriginalEdge();
    //
    BOPDS_ListOfPaveBlock& aLPB2 = myDS->ChangePaveBlocks(nE);
    aIt2.Initialize(aLPB2);
    for (; aIt2.More(); aIt2.Next())
    {
      aPB2 = aIt2.Value();
      if (aPB1 == aPB2)
      {
        aLPB2.Remove(aIt2);
        break;
      }
    }
  }
  //
  // 2. Update pave blocks
  if (bCB)
  {
    // Create new common blocks
    BOPDS_ListOfPaveBlock        aLPBNew;
    const TColStd_ListOfInteger& aFaces = aCB1->Faces();
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPBValue         = aIt.Value();
      BOPDS_Pave                     aPBValuePaves[2] = {aPBValue->Pave1(), aPBValue->Pave2()};
      //
      aCB = new BOPDS_CommonBlock;
      aIt1.Initialize(aLPB1);
      for (; aIt1.More(); aIt1.Next())
      {
        aPB2 = aIt1.Value();
        nE   = aPB2->OriginalEdge();
        //
        // Create new pave block
        aPB2n = new BOPDS_PaveBlock;
        if (aPBValue->OriginalEdge() == nE)
        {
          aPB2n->SetPave1(aPBValuePaves[0]);
          aPB2n->SetPave2(aPBValuePaves[1]);
        }
        else
        {
          // For the different original edge compute the parameters of paves
          BOPDS_Pave aPave[2];

          if (aPBValuePaves[0].Index() == aPBValuePaves[1].Index()
              && aPB2->Pave1().Index() == aPB2->Pave2().Index())
          {
            // still closed
            aPave[0].SetIndex(aPBValuePaves[0].Index());
            aPave[0].SetParameter(aPB2->Pave1().Parameter());
            aPave[1].SetIndex(aPBValuePaves[1].Index());
            aPave[1].SetParameter(aPB2->Pave2().Parameter());
          }
          else
          {
            for (Standard_Integer i = 0; i < 2; ++i)
            {
              Standard_Integer nV = aPBValuePaves[i].Index();
              aPave[i].SetIndex(nV);
              if (nV == aPB2->Pave1().Index())
              {
                aPave[i].SetParameter(aPB2->Pave1().Parameter());
              }
              else if (nV == aPB2->Pave2().Index())
              {
                aPave[i].SetParameter(aPB2->Pave2().Parameter());
              }
              else
              {
                // Compute the parameter by projecting the point
                const TopoDS_Vertex& aV   = TopoDS::Vertex(myDS->Shape(nV));
                const TopoDS_Edge&   aEOr = TopoDS::Edge(myDS->Shape(nE));
                Standard_Real        aTOut, aDist;
                Standard_Integer iErr = myContext->ComputeVE(aV, aEOr, aTOut, aDist, myFuzzyValue);
                if (!iErr)
                {
                  aPave[i].SetParameter(aTOut);
                }
                else
                {
                  // Unable to project - set the parameter of the closest boundary
                  const TopoDS_Vertex& aV1 = TopoDS::Vertex(myDS->Shape(aPB2->Pave1().Index()));
                  const TopoDS_Vertex& aV2 = TopoDS::Vertex(myDS->Shape(aPB2->Pave2().Index()));
                  //
                  gp_Pnt aP  = BRep_Tool::Pnt(aV);
                  gp_Pnt aP1 = BRep_Tool::Pnt(aV1);
                  gp_Pnt aP2 = BRep_Tool::Pnt(aV2);
                  //
                  Standard_Real aDist1 = aP.SquareDistance(aP1);
                  Standard_Real aDist2 = aP.SquareDistance(aP2);
                  //
                  aPave[i].SetParameter(aDist1 < aDist2 ? aPB2->Pave1().Parameter()
                                                        : aPB2->Pave2().Parameter());
                }
              }
            }
          }
          //
          if (aPave[1].Parameter() < aPave[0].Parameter())
          {
            BOPDS_Pave aPaveTmp = aPave[0];
            aPave[0]            = aPave[1];
            aPave[1]            = aPaveTmp;
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
  else
  {
    nE                           = aPBf->OriginalEdge();
    BOPDS_ListOfPaveBlock& aLPBE = myDS->ChangePaveBlocks(nE);
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next())
    {
      aPB = aIt.Value();
      aLPBE.Append(aPB);
    }
  }

  // Try to project the edge on the faces
  const TColStd_ListOfInteger* pLFaces = thePBFacesMap.Seek(aPBf);
  if (!pLFaces)
    return;
  TColStd_ListIteratorOfListOfInteger itLF(*pLFaces);
  for (; itLF.More(); itLF.Next())
  {
    const Standard_Integer nF  = itLF.Value();
    BOPDS_FaceInfo&        aFI = myDS->ChangeFaceInfo(nF);
    const TopoDS_Face&     aF  = TopoDS::Face(myDS->Shape(nF));

    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next())
    {
      aPB = aIt.ChangeValue();
      if (aFI.PaveBlocksOn().Contains(aPB) || aFI.PaveBlocksIn().Contains(aPB))
        continue;

      const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(aPB->Edge());
      //
      IntTools_EdgeFace anEF;
      anEF.SetEdge(aE);
      anEF.SetFace(aF);
      anEF.SetFuzzyValue(myFuzzyValue);
      anEF.SetRange(aPB->Pave1().Parameter(), aPB->Pave2().Parameter());
      anEF.SetContext(myContext);
      anEF.Perform();
      //
      const IntTools_SequenceOfCommonPrts& aCPrts = anEF.CommonParts();
      Standard_Boolean bCoincide = (aCPrts.Length() == 1 && aCPrts(1).Type() == TopAbs_EDGE);
      if (bCoincide)
      {
        aCB = myDS->CommonBlock(aPB);
        if (aCB.IsNull())
        {
          aCB = new BOPDS_CommonBlock;
          aCB->AddPaveBlock(aPB);
          myDS->SetCommonBlock(aPB, aCB);
        }
        aCB->AddFace(nF);
        aFI.ChangePaveBlocksIn().Add(aPB);
      }
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::PutClosingPaveOnCurve(BOPDS_Curve& aNC)
{
  const IntTools_Curve&     aIC  = aNC.Curve();
  const Handle(Geom_Curve)& aC3D = aIC.Curve();
  // check 3d curve
  if (aC3D.IsNull())
    return;

  // check bounds
  if (!aIC.HasBounds())
    return;

  // check closeness
  Standard_Real aT[2];
  gp_Pnt        aP[2];
  aIC.Bounds(aT[0], aT[1], aP[0], aP[1]);

  // Find the pave which has been put at one of the ends
  BOPDS_Pave aPave;
  // Index of the vertex put at one of the ends
  Standard_Integer nV = -1;
  // Keep the opposite parameter
  Standard_Real aTOp = 0.;
  // Keep the opposite bounding point
  gp_Pnt aPOp;

  Handle(BOPDS_PaveBlock)&       aPB = aNC.ChangePaveBlock1();
  BOPDS_ListOfPave&              aLP = aPB->ChangeExtPaves();
  BOPDS_ListIteratorOfListOfPave aItLP(aLP);
  for (; aItLP.More() && (nV < 0); aItLP.Next())
  {
    aPave             = aItLP.Value();
    Standard_Real aTC = aPave.Parameter();
    for (Standard_Integer j = 0; j < 2; ++j)
    {
      if (Abs(aTC - aT[j]) < Precision::PConfusion())
      {
        nV   = aPave.Index();
        aTOp = (!j) ? aT[1] : aT[0];
        aPOp = (!j) ? aP[1] : aP[0];
        break;
      }
    }
  }

  if (nV < 0)
    // No paves on the bounds of the curve
    return;

  // Check if the curve is closed using the tolerance
  // of found vertex
  const TopoDS_Vertex& aV    = TopoDS::Vertex(myDS->Shape(nV));
  Standard_Real        aTolV = BRep_Tool::Tolerance(aV);
  gp_Pnt               aPV   = BRep_Tool::Pnt(aV);
  // Tolerance for the point on the curve
  Standard_Real aTolP = Max(aNC.Tolerance(), aNC.TangentialTolerance());
  aTolP += Precision::Confusion();

  const Standard_Real aDistVP = aPV.Distance(aPOp);
  if (aDistVP > aTolV + aTolP)
  {
    // Curve is not closed
    return;
  }

  // Check if there will be valid range on the curve
  Standard_Real aFirst, aLast;
  Standard_Real aNewTolV = Max(aTolV, aDistVP + BOPTools_AlgoTools::DTolerance());
  if (!BRepLib::FindValidRange(GeomAdaptor_Curve(aIC.Curve()),
                               aIC.Tolerance(),
                               aT[0],
                               aP[0],
                               aNewTolV,
                               aT[1],
                               aP[1],
                               aNewTolV,
                               aFirst,
                               aLast))
  {
    // No valid range
    return;
  }

  if (aNewTolV > aTolV)
  {
    Standard_Integer nVn = UpdateVertex(nV, aNewTolV);
    if (nVn != nV)
    {
      aPave.SetIndex(nVn);
      nV = nVn;
    }
    aTolV = BRep_Tool::Tolerance(TopoDS::Vertex(myDS->Shape(nV)));
  }

  // Add closing pave to the curve
  BOPDS_Pave aNewPave;
  aNewPave.SetIndex(nV);
  aNewPave.SetParameter(aTOp);
  aLP.Append(aNewPave);
}

//=================================================================================================

void BOPAlgo_PaveFiller::PreparePostTreatFF(const Standard_Integer                         aInt,
                                            const Standard_Integer                         aCur,
                                            const Handle(BOPDS_PaveBlock)&                 aPB,
                                            BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& aMSCPB,
                                            TopTools_DataMapOfShapeInteger&                aMVI,
                                            BOPDS_ListOfPaveBlock&                         aLPBC)
{
  Standard_Integer nV1, nV2;
  //
  aLPBC.Append(aPB);
  //
  aPB->Indices(nV1, nV2);
  const TopoDS_Vertex& aV1 = (*(TopoDS_Vertex*)(&myDS->Shape(nV1)));
  const TopoDS_Vertex& aV2 = (*(TopoDS_Vertex*)(&myDS->Shape(nV2)));
  const TopoDS_Edge&   aE  = *(TopoDS_Edge*)&myDS->Shape(aPB->Edge());
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

//=================================================================================================

Standard_Boolean BOPAlgo_PaveFiller::CheckPlanes(const Standard_Integer nF1,
                                                 const Standard_Integer nF2) const
{
  Standard_Boolean                  bToIntersect;
  Standard_Integer                  i, nV2, iCnt;
  TColStd_MapIteratorOfMapOfInteger aIt;
  //
  bToIntersect = Standard_False;
  //
  const BOPDS_FaceInfo& aFI1 = myDS->ChangeFaceInfo(nF1);
  const BOPDS_FaceInfo& aFI2 = myDS->ChangeFaceInfo(nF2);
  //
  const TColStd_MapOfInteger& aMVIn1 = aFI1.VerticesIn();
  const TColStd_MapOfInteger& aMVOn1 = aFI1.VerticesOn();
  //
  iCnt = 0;
  for (i = 0; (i < 2 && !bToIntersect); ++i)
  {
    const TColStd_MapOfInteger& aMV2 = (!i) ? aFI2.VerticesIn() : aFI2.VerticesOn();
    //
    aIt.Initialize(aMV2);
    for (; aIt.More(); aIt.Next())
    {
      nV2 = aIt.Value();
      if (aMVIn1.Contains(nV2) || aMVOn1.Contains(nV2))
      {
        ++iCnt;
        if (iCnt > 1)
        {
          bToIntersect = !bToIntersect;
          break;
        }
      }
    }
  }
  //
  return bToIntersect;
}

//=================================================================================================

void BOPAlgo_PaveFiller::UpdatePaveBlocks(const TColStd_DataMapOfIntegerInteger& aDMNewSD)
{
  if (aDMNewSD.IsEmpty())
  {
    return;
  }
  //
  Standard_Integer                    nSp, aNbPBP, nV[2], i, j;
  Standard_Real                       aT[2];
  Standard_Boolean                    bCB, bRebuild;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  BOPDS_MapOfPaveBlock                aMPB;
  TColStd_MapOfInteger                aMicroEdges;
  //
  BOPDS_ListOfPaveBlock anAllPBs;

  // Get pave blocks of section edges
  BOPDS_VectorOfInterfFF& aFFs  = myDS->InterfFF();
  Standard_Integer        aNbFF = aFFs.Length();
  for (i = 0; i < aNbFF; ++i)
  {
    const BOPDS_InterfFF&      aFF  = aFFs(i);
    const BOPDS_VectorOfCurve& aVNC = aFF.Curves();
    Standard_Integer           aNbC = aVNC.Length();
    for (j = 0; j < aNbC; ++j)
    {
      const BOPDS_Curve&           aNC   = aVNC(j);
      const BOPDS_ListOfPaveBlock& aLPBC = aNC.PaveBlocks();
      aItPB.Initialize(aLPBC);
      for (; aItPB.More(); aItPB.Next())
        anAllPBs.Append(aItPB.Value());
    }
  }

  // Get pave blocks from the pool
  BOPDS_VectorOfListOfPaveBlock& aPBP = myDS->ChangePaveBlocksPool();
  aNbPBP                              = aPBP.Length();
  for (i = 0; i < aNbPBP; ++i)
  {
    BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next())
      anAllPBs.Append(aItPB.Value());
  }

  // Process all pave blocks
  aItPB.Initialize(anAllPBs);
  for (; aItPB.More(); aItPB.Next())
  {
    Handle(BOPDS_PaveBlock)          aPB = aItPB.Value();
    const Handle(BOPDS_CommonBlock)& aCB = myDS->CommonBlock(aPB);
    bCB                                  = !aCB.IsNull();
    if (bCB)
    {
      aPB = aCB->PaveBlock1();
    }
    //
    if (aMPB.Add(aPB))
    {
      bRebuild = Standard_False;
      aPB->Indices(nV[0], nV[1]);
      aPB->Range(aT[0], aT[1]);
      // remember the fact if the edge had different vertices before substitution
      Standard_Boolean wasRegularEdge = (nV[0] != nV[1]);
      //
      for (j = 0; j < 2; ++j)
      {
        if (aDMNewSD.IsBound(nV[j]))
        {
          BOPDS_Pave aPave;
          //
          nV[j] = aDMNewSD.Find(nV[j]);
          aPave.SetIndex(nV[j]);
          aPave.SetParameter(aT[j]);
          //
          bRebuild = Standard_True;
          if (!j)
          {
            aPB->SetPave1(aPave);
          }
          else
          {
            aPB->SetPave2(aPave);
          }
        }
      }
      //
      if (bRebuild)
      {
        Standard_Integer nE = aPB->Edge();
        // Check if the Pave Block has the edge set
        if (nE < 0)
        {
          // untouched edge
          nE = aPB->OriginalEdge();
        }
        Standard_Boolean isDegEdge = myDS->ShapeInfo(nE).HasFlag();
        if (wasRegularEdge && !isDegEdge && nV[0] == nV[1])
        {
          // now edge has the same vertex on both ends;
          // check if it is not a regular closed curve.
          FillShrunkData(aPB);
          if (!aPB->HasShrunkData())
          {
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
      } // if (bRebuild) {
    } // if (aMPB.Add(aPB)) {
  } // for (; aItPB.More(); aItPB.Next()) {
  aMPB.Clear();

  if (aMicroEdges.Extent())
    RemovePaveBlocks(aMicroEdges);
}

//=================================================================================================

void BOPAlgo_PaveFiller::RemovePaveBlocks(const TColStd_MapOfInteger& theEdges)
{
  // Remove all pave blocks referring to input edges:
  //
  // 1. from the Pave Blocks Pool
  BOPDS_VectorOfListOfPaveBlock& aPBP   = myDS->ChangePaveBlocksPool();
  Standard_Integer               aNbPBP = aPBP.Length(), i;
  for (i = 0; i < aNbPBP; ++i)
  {
    BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    //
    BOPDS_ListIteratorOfListOfPaveBlock aItPB(aLPB);
    while (aItPB.More())
    {
      const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
      if (theEdges.Contains(aPB->Edge()))
        aLPB.Remove(aItPB);
      else
        aItPB.Next();
    }
  }

  // 2. from section curves
  TColStd_MapOfInteger    aMPassed;
  BOPDS_VectorOfInterfFF& aFFs  = myDS->InterfFF();
  Standard_Integer        aNbFF = aFFs.Length(), j;
  for (i = 0; i < aNbFF; ++i)
  {
    BOPDS_InterfFF& aFF = aFFs(i);
    // remove from Section pave blocks
    BOPDS_VectorOfCurve& aVNC = aFF.ChangeCurves();
    Standard_Integer     aNbC = aVNC.Length();
    for (j = 0; j < aNbC; ++j)
    {
      BOPDS_Curve&                        aNC  = aVNC(j);
      BOPDS_ListOfPaveBlock&              aLPB = aNC.ChangePaveBlocks();
      BOPDS_ListIteratorOfListOfPaveBlock aItPB(aLPB);
      while (aItPB.More())
      {
        const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
        if (theEdges.Contains(aPB->Edge()))
          aLPB.Remove(aItPB);
        else
          aItPB.Next();
      }
    }
  }

  // 3. From Face Info
  for (i = 0; i < myDS->NbSourceShapes(); ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
      continue;
    if (!aSI.HasReference())
      continue;

    BOPDS_FaceInfo&              aFI     = myDS->ChangeFaceInfo(i);
    BOPDS_IndexedMapOfPaveBlock* aIMPB[] = {&aFI.ChangePaveBlocksIn(),
                                            &aFI.ChangePaveBlocksOn(),
                                            &aFI.ChangePaveBlocksSc()};
    for (Standard_Integer k = 0; k < 3; k++)
    {
      Standard_Integer aNbPB = aIMPB[k]->Extent(), m;
      for (m = 1; m <= aNbPB; ++m)
      {
        const Handle(BOPDS_PaveBlock)& aPB = aIMPB[k]->FindKey(m);
        if (theEdges.Contains(aPB->Edge()))
          break;
      }
      if (m <= aNbPB)
      {
        BOPDS_IndexedMapOfPaveBlock aMPBCopy = *aIMPB[k];
        aIMPB[k]->Clear();
        for (m = 1; m <= aNbPB; ++m)
        {
          const Handle(BOPDS_PaveBlock)& aPB = aMPBCopy(m);
          if (!theEdges.Contains(aPB->Edge()))
            aIMPB[k]->Add(aPB);
        }
      }
    }
  }
}

//=======================================================================
// function : ToleranceFF
// purpose  : Computes the TolFF according to the tolerance value and
//           types of the faces.
//=======================================================================
Standard_Real ToleranceFF(const BRepAdaptor_Surface& aBAS1, const BRepAdaptor_Surface& aBAS2)
{
  Standard_Real aTol1  = aBAS1.Tolerance();
  Standard_Real aTol2  = aBAS2.Tolerance();
  Standard_Real aTolFF = Max(aTol1, aTol2);
  //
  Standard_Boolean isAna1, isAna2;
  isAna1 = (aBAS1.GetType() == GeomAbs_Plane || aBAS1.GetType() == GeomAbs_Cylinder
            || aBAS1.GetType() == GeomAbs_Cone || aBAS1.GetType() == GeomAbs_Sphere
            || aBAS1.GetType() == GeomAbs_Torus);
  //
  isAna2 = (aBAS2.GetType() == GeomAbs_Plane || aBAS2.GetType() == GeomAbs_Cylinder
            || aBAS2.GetType() == GeomAbs_Cone || aBAS2.GetType() == GeomAbs_Sphere
            || aBAS2.GetType() == GeomAbs_Torus);
  //
  if (!isAna1 || !isAna2)
  {
    aTolFF = Max(aTolFF, 5.e-6);
  }
  return aTolFF;
}

//=================================================================================================

void BOPAlgo_PaveFiller::UpdateBlocksWithSharedVertices()
{
  if (!myNonDestructive)
  {
    return;
  }
  //
  Standard_Integer aNbFF;
  //
  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  aNbFF                        = aFFs.Length();
  if (!aNbFF)
  {
    return;
  }
  //
  Standard_Boolean     bOnCurve, bHasShapeSD;
  Standard_Integer     i, nF1, nF2, aNbC, j, nV, nVSD;
  Standard_Real        aTolV;
  TColStd_MapOfInteger aMF;
  //
  for (i = 0; i < aNbFF; ++i)
  {
    BOPDS_InterfFF& aFF = aFFs(i);
    //
    BOPDS_VectorOfCurve& aVC = aFF.ChangeCurves();
    aNbC                     = aVC.Length();
    if (!aNbC)
    {
      continue;
    }
    //
    aFF.Indices(nF1, nF2);
    //
    if (aMF.Add(nF1))
    {
      myDS->UpdateFaceInfoOn(nF1);
    }
    if (aMF.Add(nF2))
    {
      myDS->UpdateFaceInfoOn(nF2);
    }
    //
    // Collect old vertices that are shared for nF1, nF2 ->aMI;
    TColStd_MapOfInteger              aMI;
    TColStd_MapIteratorOfMapOfInteger aItMI;
    //
    BOPDS_FaceInfo& aFI1 = myDS->ChangeFaceInfo(nF1);
    BOPDS_FaceInfo& aFI2 = myDS->ChangeFaceInfo(nF2);
    //
    const TColStd_MapOfInteger& aMVOn1 = aFI1.VerticesOn();
    const TColStd_MapOfInteger& aMVIn1 = aFI1.VerticesIn();
    const TColStd_MapOfInteger& aMVOn2 = aFI2.VerticesOn();
    const TColStd_MapOfInteger& aMVIn2 = aFI2.VerticesIn();
    //
    for (j = 0; j < 2; ++j)
    {
      const TColStd_MapOfInteger& aMV1 = (!j) ? aMVOn1 : aMVIn1;
      aItMI.Initialize(aMV1);
      for (; aItMI.More(); aItMI.Next())
      {
        nV = aItMI.Value();
        if (myDS->IsNewShape(nV))
        {
          continue;
        }
        if (aMVOn2.Contains(nV) || aMVIn2.Contains(nV))
        {
          aMI.Add(nV);
        }
      }
    }
    //
    // Try to put vertices aMI on curves
    for (j = 0; j < aNbC; ++j)
    {
      BOPDS_Curve&  aNC     = aVC.ChangeValue(j);
      Standard_Real aTolR3D = Max(aNC.Tolerance(), aNC.TangentialTolerance());
      //
      aItMI.Initialize(aMI);
      for (; aItMI.More(); aItMI.Next())
      {
        nV = aItMI.Value();
        //
        bHasShapeSD = myDS->HasShapeSD(nV, nVSD);
        if (bHasShapeSD)
        {
          continue;
        }
        //
        bOnCurve = EstimatePaveOnCurve(nV, aNC, aTolR3D);
        if (!bOnCurve)
        {
          continue;
        }
        //
        const TopoDS_Vertex& aV = *((TopoDS_Vertex*)&myDS->Shape(nV));
        aTolV                   = BRep_Tool::Tolerance(aV);
        //
        UpdateVertex(nV, aTolV);
        myDS->InitPaveBlocksForVertex(nV);
      }
    } // for (j=0; j<aNbC; ++j) {
  } // for (i=0; i<aNbFF; ++i) {
  //
  UpdateCommonBlocksWithSDVertices();
}

//=================================================================================================

Standard_Boolean BOPAlgo_PaveFiller::EstimatePaveOnCurve(const Standard_Integer nV,
                                                         const BOPDS_Curve&     aNC,
                                                         const Standard_Real    aTolR3D)
{
  Standard_Boolean bIsVertexOnLine;
  Standard_Real    aT;
  //
  const TopoDS_Vertex&  aV  = *((TopoDS_Vertex*)&myDS->Shape(nV));
  const IntTools_Curve& aIC = aNC.Curve();
  //
  bIsVertexOnLine = myContext->IsVertexOnLine(aV, aIC, aTolR3D, aT);
  return bIsVertexOnLine;
}

//=================================================================================================

void BOPAlgo_PaveFiller::CorrectToleranceOfSE()
{
  BOPDS_VectorOfInterfFF&                                             aFFs = myDS->InterfFF();
  NCollection_IndexedDataMap<Standard_Integer, BOPDS_ListOfPaveBlock> aMVIPBs;
  TColStd_MapOfInteger                                                aMVIToReduce;
  // Fence map to avoid repeated checking of the same edge
  BOPDS_MapOfPaveBlock aMPB;
  //
  // 1. iterate on all sections F-F
  Standard_Integer aNb = aFFs.Length(), i;
  for (i = 0; i < aNb; ++i)
  {
    BOPDS_InterfFF& aFF = aFFs(i);
    //
    BOPDS_VectorOfCurve& aVNC = aFF.ChangeCurves();
    Standard_Integer     aNbC = aVNC.Length(), k;
    for (k = 0; k < aNbC; ++k)
    {
      BOPDS_Curve&                        aNC  = aVNC(k);
      BOPDS_ListOfPaveBlock&              aLPB = aNC.ChangePaveBlocks();
      BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
      for (; aItLPB.More();)
      {
        const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
        Standard_Integer               nE;
        if (!aPB->HasEdge(nE))
        {
          aLPB.Remove(aItLPB);
          continue;
        }
        //
        if (!aMPB.Add(aPB))
        {
          aItLPB.Next();
          continue;
        }
        //
        Standard_Boolean bIsReduced = Standard_False;
        if (aPB->OriginalEdge() < 0)
        {
          // It is possible that due to small angle between faces the
          // common zone between faces can be large and the tangential
          // tolerance of the curve will be large as well.
          // Here we're trying to reduce the tolerance of the section
          // edge using the valid tolerance of the edge.
          // Note, that if the pave block has created common block with
          // other edges its valid tolerance could have been changed to
          // cover all edges in common block (see PostTreatFF() method).
          Standard_Real aTolC    = aNC.Tolerance();
          Standard_Real aTolTang = aNC.TangentialTolerance();
          if (aTolC < aTolTang)
          {
            const TopoDS_Edge& aE    = TopoDS::Edge(myDS->Shape(nE));
            Standard_Real      aTolE = BRep_Tool::Tolerance(aE);
            if (aTolC < aTolE)
            {
              // reduce edge tolerance
              static_cast<BRep_TEdge*>(aE.TShape().get())->Tolerance(aTolC);
              bIsReduced = Standard_True;
            }
          }
        }
        //
        // fill in the map vertex index - pave blocks
        for (Standard_Integer j = 0; j < 2; j++)
        {
          Standard_Integer nV = (j == 0 ? aPB->Pave1().Index() : aPB->Pave2().Index());
          myDS->HasShapeSD(nV, nV);
          BOPDS_ListOfPaveBlock* pPBList = aMVIPBs.ChangeSeek(nV);
          if (!pPBList)
          {
            pPBList = &aMVIPBs.ChangeFromIndex(aMVIPBs.Add(nV, BOPDS_ListOfPaveBlock()));
          }
          pPBList->Append(aPB);
          if (bIsReduced)
          {
            aMVIToReduce.Add(nV);
          }
        }
        aItLPB.Next();
      }
    }
  }
  //
  if (aMVIToReduce.IsEmpty())
  {
    return;
  }
  //
  // 2. try to reduce tolerances of connected vertices
  // 2.1 find all other edges containing these connected vertices to avoid
  //     reducing the tolerance to the value less than the tolerances of edges,
  //     i.e. minimal tolerance for the vertex is the max tolerance of the
  //     edges containing this vertex
  TColStd_DataMapOfIntegerReal   aMVITol;
  BOPDS_VectorOfListOfPaveBlock& aPBP = myDS->ChangePaveBlocksPool();
  aNb                                 = aPBP.Length();
  for (i = 0; i < aNb; ++i)
  {
    const BOPDS_ListOfPaveBlock&        aLPB = aPBP(i);
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
    for (; aItLPB.More(); aItLPB.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
      Standard_Integer               nE;
      if (!aPB->HasEdge(nE))
      {
        continue;
      }
      const TopoDS_Edge& aE    = TopoDS::Edge(myDS->Shape(nE));
      Standard_Real      aTolE = BRep_Tool::Tolerance(aE);
      //
      Standard_Integer nV[2];
      aPB->Indices(nV[0], nV[1]);
      //
      for (Standard_Integer j = 0; j < 2; j++)
      {
        if (aMVIToReduce.Contains(nV[j]))
        {
          Standard_Real* aMaxTol = aMVITol.ChangeSeek(nV[j]);
          if (!aMaxTol)
          {
            aMVITol.Bind(nV[j], aTolE);
          }
          else if (aTolE > *aMaxTol)
          {
            *aMaxTol = aTolE;
          }
          BOPDS_ListOfPaveBlock& aPBList = aMVIPBs.ChangeFromKey(nV[j]);
          aPBList.Append(aPB);
        }
      }
    }
  }
  //
  // 2.2 reduce tolerances if possible
  aNb = aMVIPBs.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    Standard_Integer nV = aMVIPBs.FindKey(i);
    if (!aMVIToReduce.Contains(nV))
    {
      continue;
    }
    //
    const TopoDS_Vertex& aV      = TopoDS::Vertex(myDS->Shape(nV));
    Standard_Real        aTolV   = BRep_Tool::Tolerance(aV);
    Standard_Real        aMaxTol = aMVITol.IsBound(nV) ? aMVITol.Find(nV) : 0.;
    // it makes no sense to compute the real tolerance if it is
    // impossible to reduce the tolerance at least 0.1% of the current value
    if (aTolV - aMaxTol < 0.001 * aTolV)
    {
      continue;
    }
    //
    // compute the maximal distance from the vertex to the adjacent edges
    gp_Pnt aP = BRep_Tool::Pnt(aV);
    //
    // Avoid repeated checks
    BOPDS_MapOfPaveBlock aMPBFence;
    //
    const BOPDS_ListOfPaveBlock&        aLPB = aMVIPBs.FindFromIndex(i);
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
    for (; aItLPB.More(); aItLPB.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
      if (!aMPBFence.Add(aPB))
      {
        continue;
      }
      Standard_Integer   nE = aPB->Edge();
      const TopoDS_Edge& aE = TopoDS::Edge(myDS->Shape(nE));
      BRepAdaptor_Curve  aC(aE);
      for (Standard_Integer iPave = 0; iPave < 2; ++iPave)
      {
        const BOPDS_Pave& aPave = !iPave ? aPB->Pave1() : aPB->Pave2();
        Standard_Integer  nVSD  = aPave.Index();
        myDS->HasShapeSD(nVSD, nVSD);
        if (nVSD != nV)
        {
          continue;
        }
        //
        gp_Pnt        aPonE = aC.Value(aPave.Parameter());
        Standard_Real aDist = aP.Distance(aPonE);
        aDist += BRep_Tool::Tolerance(aE);
        if (aDist > aMaxTol)
        {
          aMaxTol = aDist;
        }
      }
    }
    //
    if (aMaxTol < aTolV)
    {
      static_cast<BRep_TVertex*>(aV.TShape().get())->Tolerance(aMaxTol);
    }
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::PutSEInOtherFaces(const Message_ProgressRange& theRange)
{
  // Try to intersect each section edge with the faces
  // not participated in its creation

  // Get all section edges
  BOPDS_IndexedMapOfPaveBlock aMPBScAll;

  BOPDS_VectorOfInterfFF& aFFs  = myDS->InterfFF();
  const Standard_Integer  aNbFF = aFFs.Length();
  Message_ProgressScope   aPS(theRange, NULL, 1);
  for (Standard_Integer i = 0; i < aNbFF; ++i)
  {
    const BOPDS_VectorOfCurve& aVNC = aFFs(i).Curves();
    const Standard_Integer     aNbC = aVNC.Length();
    for (Standard_Integer j = 0; j < aNbC; ++j)
    {
      const BOPDS_ListOfPaveBlock&        aLPBC = aVNC(j).PaveBlocks();
      BOPDS_ListIteratorOfListOfPaveBlock aItPB(aLPBC);
      for (; aItPB.More(); aItPB.Next())
        aMPBScAll.Add(aItPB.Value());
    }
  }
  // Perform intersection of collected pave blocks
  ForceInterfEF(aMPBScAll, aPS.Next(), Standard_False);
}

//=================================================================================================

void BOPAlgo_PaveFiller::RemoveMicroSectionEdges(
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
  BOPDS_IndexedMapOfPaveBlock&                   theMicroPB)
{
  if (theMSCPB.IsEmpty())
    // no section edges
    return;

  // Get all F/F interferences
  BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();

  // Build the new map of section edges avoiding the micro edges
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks aSEPBMap;
  // Analyze all section edges
  Standard_Integer aNbCPB = theMSCPB.Extent();
  for (Standard_Integer i = 1; i <= aNbCPB; ++i)
  {
    const TopoDS_Shape&             aSI  = theMSCPB.FindKey(i);
    const BOPDS_CoupleOfPaveBlocks& aCPB = theMSCPB(i);

    if (aSI.ShapeType() != TopAbs_EDGE)
    {
      // Not an edge
      aSEPBMap.Add(aSI, aCPB);
      continue;
    }

    // Get pave block for analysis
    const Handle(BOPDS_PaveBlock)& aPB = aCPB.PaveBlock1();
    if (aPB->HasEdge())
    {
      // Not a real section edge
      aSEPBMap.Add(aSI, aCPB);
      continue;
    }

    if (!BOPTools_AlgoTools::IsMicroEdge(TopoDS::Edge(aSI), myContext, Standard_False))
    {
      // Normal edge
      aSEPBMap.Add(aSI, aCPB);
      continue;
    }

    // Micro edge is found, avoid it in the <theMSCPB> map
    // and remove from the F/F Intersection info structure

    // Get F/F interference which created this micro edge
    BOPDS_InterfFF& aFF = aFFs(aCPB.IndexInterf());
    // Get curve from which this edge has been created
    BOPDS_Curve& aCurve = aFF.ChangeCurves().ChangeValue(aCPB.Index());
    // Get all section pave blocks created from this curve
    BOPDS_ListOfPaveBlock& aLPBC = aCurve.ChangePaveBlocks();
    // Remove pave block from the list
    for (BOPDS_ListIteratorOfListOfPaveBlock it(aLPBC); it.More(); it.Next())
    {
      if (it.Value() == aPB)
      {
        aLPBC.Remove(it);
        break;
      }
    }

    // Add the pave block of "micro" edge into outgoing map for
    // unification of its vertices in the PostTreatFF method
    theMicroPB.Add(aPB);
  }

  // Overwrite the old map if necessary
  if (aSEPBMap.Extent() != theMSCPB.Extent())
    theMSCPB = aSEPBMap;
}

//=================================================================================================

void BOPAlgo_PaveFiller::RemoveMicroEdges()
{
  // Fence map
  BOPDS_MapOfPaveBlock aMPBFence;
  // Resulting map of micro edges
  TColStd_MapOfInteger aMicroEdges;
  // Check all pave blocks from the pool to find the micro edges
  BOPDS_VectorOfListOfPaveBlock& aPBP   = myDS->ChangePaveBlocksPool();
  Standard_Integer               aNbPBP = aPBP.Length();
  for (Standard_Integer i = 0; i < aNbPBP; ++i)
  {
    BOPDS_ListOfPaveBlock& aLPB = aPBP(i);
    if (aLPB.Extent() < 2)
      // No splits
      continue;

    if (myDS->ShapeInfo(aLPB.First()->OriginalEdge()).HasFlag())
      continue;

    BOPDS_ListOfPaveBlock::Iterator it(aLPB);
    for (; it.More(); it.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPB  = it.Value();
      Handle(BOPDS_PaveBlock)        aPBR = myDS->RealPaveBlock(aPB);

      if (aMPBFence.Add(aPBR))
      {
        Standard_Integer nV1, nV2;
        aPBR->Indices(nV1, nV2);
        if (nV1 == nV2)
        {
          // Check if it has the valid range
          FillShrunkData(aPBR);
          if (!aPBR->HasShrunkData())
            aMicroEdges.Add(aPBR->Edge());
        }
      }
    }
  }
  RemovePaveBlocks(aMicroEdges);
}
