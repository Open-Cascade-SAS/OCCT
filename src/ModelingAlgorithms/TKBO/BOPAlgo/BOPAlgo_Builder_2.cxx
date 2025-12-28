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

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_BuilderFace.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <NCollection_Vector.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_Set.hxx>
#include <NCollection_Map.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <BRep_Tool.hxx>
#include <GeomLib.hxx>
#include <NCollection_IncAllocator.hxx>
#include <IntTools_Context.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <algorithm>
//
static TopoDS_Face BuildDraftFace(
  const TopoDS_Face& theFace,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                     theImages,
  occ::handle<IntTools_Context>&     theCtx,
  const occ::handle<Message_Report>& theReport);

//=================================================================================================

class BOPAlgo_PairOfShapeBoolean : public BOPAlgo_ParallelAlgo
{

public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_PairOfShapeBoolean()
      : BOPAlgo_ParallelAlgo(),
        myFlag(false)
  {
  }

  //
  virtual ~BOPAlgo_PairOfShapeBoolean() {}

  //
  TopoDS_Shape& Shape1() { return myShape1; }

  //
  TopoDS_Shape& Shape2() { return myShape2; }

  //
  bool& Flag() { return myFlag; }

  //
  void SetContext(const occ::handle<IntTools_Context>& aContext) { myContext = aContext; }

  //
  const occ::handle<IntTools_Context>& Context() const { return myContext; }

  //
  virtual void Perform()
  {
    Message_ProgressScope aPS(myProgressRange, NULL, 1);
    if (UserBreak(aPS))
    {
      return;
    }

    const TopoDS_Face& aFj = *((TopoDS_Face*)&myShape1);
    const TopoDS_Face& aFk = *((TopoDS_Face*)&myShape2);
    myFlag = BOPTools_AlgoTools::AreFacesSameDomain(aFj, aFk, myContext, myFuzzyValue);
  }

  //
protected:
  bool                          myFlag;
  TopoDS_Shape                  myShape1;
  TopoDS_Shape                  myShape2;
  occ::handle<IntTools_Context> myContext;
};

//
typedef NCollection_Vector<BOPAlgo_PairOfShapeBoolean> BOPAlgo_VectorOfPairOfShapeBoolean;

//=======================================================================
// class   : BOPAlgo_SplitFace
// purpose : Auxiliary class to extend BOPAlgo_BuilderFace with progress support
//=======================================================================
class BOPAlgo_SplitFace : public BOPAlgo_BuilderFace
{
public:
  //! Sets progress range
  void SetProgressRange(const Message_ProgressRange& theRange) { myRange = theRange; }

  // New perform method, using own progress range
  void Perform()
  {
    Message_ProgressScope aPS(myRange, NULL, 1);
    if (!aPS.More())
    {
      return;
    }
    BOPAlgo_BuilderFace::Perform(aPS.Next());
  }

private:
  //! Disable the range enabled method
  virtual void Perform(const Message_ProgressRange& /*theRange*/) {};

private:
  Message_ProgressRange myRange;
};

typedef NCollection_Vector<BOPAlgo_SplitFace> BOPAlgo_VectorOfBuilderFace;

//=================================================================================================

class BOPAlgo_VFI : public BOPAlgo_ParallelAlgo
{

public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_VFI()
      : BOPAlgo_ParallelAlgo(),
        myIsInternal(false)
  {
  }

  //
  virtual ~BOPAlgo_VFI() {}

  //
  void SetVertex(const TopoDS_Vertex& aV) { myV = aV; }

  //
  TopoDS_Vertex& Vertex() { return myV; }

  //
  void SetFace(const TopoDS_Face& aF) { myF = aF; }

  //
  TopoDS_Face& Face() { return myF; }

  //
  bool IsInternal() const { return myIsInternal; }

  //
  void SetContext(const occ::handle<IntTools_Context>& aContext) { myContext = aContext; }

  //
  const occ::handle<IntTools_Context>& Context() const { return myContext; }

  //
  virtual void Perform()
  {
    Message_ProgressScope aPS(myProgressRange, NULL, 1);
    if (UserBreak(aPS))
    {
      return;
    }

    double aT1, aT2, dummy;
    //
    int iFlag    = myContext->ComputeVF(myV, myF, aT1, aT2, dummy, myFuzzyValue);
    myIsInternal = (iFlag == 0);
  }

  //
protected:
  bool                          myIsInternal;
  TopoDS_Vertex                 myV;
  TopoDS_Face                   myF;
  occ::handle<IntTools_Context> myContext;
};

//
typedef NCollection_Vector<BOPAlgo_VFI> BOPAlgo_VectorOfVFI;

//=================================================================================================

void BOPAlgo_Builder::FillImagesFaces(const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPS(theRange, "Filling splits of faces", 10);
  BuildSplitFaces(aPS.Next(9));
  if (HasErrors())
  {
    return;
  }
  FillSameDomainFaces(aPS.Next(0.5));
  if (HasErrors())
  {
    return;
  }
  FillInternalVertices(aPS.Next(0.5));
}

//=================================================================================================

void BOPAlgo_Builder::BuildSplitFaces(const Message_ProgressRange& theRange)
{
  bool                                     bHasFaceInfo, bIsClosed, bIsDegenerated, bToReverse;
  int                                      i, j, k, aNbS, aNbPBIn, aNbPBOn, aNbPBSc, aNbAV, nSp;
  TopoDS_Face                              aFF, aFSD;
  TopoDS_Edge                              aSp, aEE;
  TopAbs_Orientation                       anOriF, anOriE;
  TopExp_Explorer                          aExp;
  NCollection_List<TopoDS_Shape>::Iterator aIt;
  NCollection_List<int>                    aLIAV;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
  occ::handle<NCollection_BaseAllocator>                 aAllocator;
  BOPAlgo_VectorOfBuilderFace                            aVBF;
  //
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~scope f
  aAllocator = NCollection_BaseAllocator::CommonBaseAllocator();
  //
  NCollection_List<TopoDS_Shape>                         aLE(aAllocator);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMDE(100, aAllocator);
  //
  Message_ProgressScope aPSOuter(theRange, NULL, 10);
  // Build temporary map of faces images to avoid rebuilding
  // of the faces without any IN or section edges
  NCollection_IndexedDataMap<int, NCollection_List<TopoDS_Shape>> aFacesIm;
  //
  aNbS = myDS->NbSourceShapes();
  //
  for (i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
    {
      continue;
    }
    if (UserBreak(aPSOuter))
    {
      return;
    }
    //
    const TopoDS_Face& aF        = (*(TopoDS_Face*)(&aSI.Shape()));
    bool               isUClosed = false, isVClosed = false, isChecked = false;
    //
    bHasFaceInfo = myDS->HasFaceInfo(i);
    if (!bHasFaceInfo)
    {
      continue;
    }
    //
    const BOPDS_FaceInfo& aFI = myDS->FaceInfo(i);
    //
    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aMPBIn = aFI.PaveBlocksIn();
    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aMPBOn = aFI.PaveBlocksOn();
    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aMPBSc = aFI.PaveBlocksSc();
    aLIAV.Clear();
    myDS->AloneVertices(i, aLIAV);

    aNbPBIn = aMPBIn.Extent();
    aNbPBOn = aMPBOn.Extent();
    aNbPBSc = aMPBSc.Extent();
    aNbAV   = aLIAV.Extent();
    if (!aNbPBIn && !aNbPBOn && !aNbPBSc && !aNbAV)
    { // not compete
      continue;
    }

    if (!aNbPBIn && !aNbPBSc)
    {
      // If there are any alone vertices to be put in the face,
      // the new face has to be created even if the wires of the
      // face have not been modified.

      // It is also necessary to check if the face contains any internal edges,
      // as such edges may split the face on parts and it is better
      // to send the face be treated by the BuilderFace algorithm.
      // In case of alone vertices the check for internals will be performed
      // in the BuildDraftFace method.
      bool hasInternals = false;
      if (!aNbAV)
      {
        // Check if any wires of the face have been modified.
        // If no modified and internal wires present in the face
        // there is no need to create the new face.
        bool hasModified = false;

        TopoDS_Iterator aItW(aF);
        for (; aItW.More(); aItW.Next())
        {
          TopoDS_Iterator itE(aItW.Value());
          hasInternals = (itE.More() && (itE.Value().Orientation() == TopAbs_INTERNAL));
          if (hasInternals)
            break;

          hasModified |= myImages.IsBound(aItW.Value());
        }

        if (!hasInternals && !hasModified)
          continue;
      }

      if (!hasInternals)
      {
        // No internal parts for the face, so just build the draft face
        // and keep it to pass directly into result.
        // If the original face has any internal edges or multi-connected vertices,
        // the draft face will be null, as such sub-shapes may split the face on parts
        // (as in the case "bugs modalg_5 bug25245_1").
        // The BuilderFace algorithm will be called in this case.
        TopoDS_Face aFD = BuildDraftFace(aF, myImages, myContext, myReport);
        if (!aFD.IsNull())
        {
          aFacesIm(aFacesIm.Add(i, NCollection_List<TopoDS_Shape>())).Append(aFD);
          continue;
        }
      }
    }

    aMFence.Clear();
    //
    anOriF = aF.Orientation();
    aFF    = aF;
    aFF.Orientation(TopAbs_FORWARD);
    //
    // 1. Fill the edges set for the face aFF -> LE
    aLE.Clear();

    // 1.1 Bounding edges
    aExp.Init(aFF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aExp.Current()));
      anOriE                = aE.Orientation();
      //
      if (!myImages.IsBound(aE))
      {
        if (anOriE == TopAbs_INTERNAL)
        {
          aEE = aE;
          aEE.Orientation(TopAbs_FORWARD);
          aLE.Append(aEE);
          aEE.Orientation(TopAbs_REVERSED);
          aLE.Append(aEE);
        }
        else
        {
          aLE.Append(aE);
        }

        continue;
      }

      if (!isChecked)
      {
        const occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aF);
        GeomLib::IsClosed(aSurf, BRep_Tool::Tolerance(aE), isUClosed, isVClosed);

        isChecked = true;
      }

      bIsClosed = false;

      if ((isUClosed || isVClosed) && BRep_Tool::IsClosed(aE, aF))
      {

        bool isUIso = false, isVIso = false;
        BOPTools_AlgoTools2D::IsEdgeIsoline(aE, aF, isUIso, isVIso);

        bIsClosed = ((isUClosed && isUIso) || (isVClosed && isVIso));
      }

      bIsDegenerated = BRep_Tool::Degenerated(aE);

      const NCollection_List<TopoDS_Shape>& aLIE = myImages.Find(aE);
      aIt.Initialize(aLIE);
      for (; aIt.More(); aIt.Next())
      {
        aSp = (*(TopoDS_Edge*)(&aIt.Value()));
        if (bIsDegenerated)
        {
          aSp.Orientation(anOriE);
          aLE.Append(aSp);
          continue;
        }
        //
        if (anOriE == TopAbs_INTERNAL)
        {
          aSp.Orientation(TopAbs_FORWARD);
          aLE.Append(aSp);
          aSp.Orientation(TopAbs_REVERSED);
          aLE.Append(aSp);
          continue;
        }
        //
        if (bIsClosed)
        {
          if (aMFence.Add(aSp))
          {
            if (!BRep_Tool::IsClosed(aSp, aF))
            {
              if (!BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSp, aF))
              {
                // try different approach
                if (!BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aE, aSp, aF))
                {
                  TopoDS_Compound aWS;
                  BRep_Builder().MakeCompound(aWS);
                  BRep_Builder().Add(aWS, aF);
                  BRep_Builder().Add(aWS, aSp);
                  AddWarning(new BOPAlgo_AlertUnableToMakeClosedEdgeOnFace(aWS));
                }
              }
            }
            //
            aSp.Orientation(TopAbs_FORWARD);
            aLE.Append(aSp);
            aSp.Orientation(TopAbs_REVERSED);
            aLE.Append(aSp);
          } // if (aMFence.Add(aSp))
          continue;
        } // if (bIsClosed){
        //
        aSp.Orientation(anOriE);
        bToReverse = BOPTools_AlgoTools::IsSplitToReverseWithWarn(aSp, aE, myContext, myReport);
        if (bToReverse)
        {
          aSp.Reverse();
        }
        aLE.Append(aSp);
      } // for (; aIt.More(); aIt.Next()) {
    } // for (; aExp.More(); aExp.Next()) {
    //
    //
    // 1.2 In edges
    for (j = 1; j <= aNbPBIn; ++j)
    {
      const occ::handle<BOPDS_PaveBlock>& aPB = aMPBIn(j);
      nSp                                     = aPB->Edge();
      Standard_ASSERT(nSp >= 0, "Face information is not up to date", continue);
      aSp = (*(TopoDS_Edge*)(&myDS->Shape(nSp)));
      //
      aSp.Orientation(TopAbs_FORWARD);
      aLE.Append(aSp);
      aSp.Orientation(TopAbs_REVERSED);
      aLE.Append(aSp);
    }
    //
    //
    // 1.3 Section edges
    for (j = 1; j <= aNbPBSc; ++j)
    {
      const occ::handle<BOPDS_PaveBlock>& aPB = aMPBSc(j);
      nSp                                     = aPB->Edge();
      aSp                                     = (*(TopoDS_Edge*)(&myDS->Shape(nSp)));
      //
      aSp.Orientation(TopAbs_FORWARD);
      aLE.Append(aSp);
      aSp.Orientation(TopAbs_REVERSED);
      aLE.Append(aSp);
    }
    //
    if (!myPaveFiller->NonDestructive())
    {
      // speed up for planar faces
      BRepLib::BuildPCurveForEdgesOnPlane(aLE, aFF);
    }
    // 3 Build split faces
    BOPAlgo_SplitFace& aBF = aVBF.Appended();
    aBF.SetFace(aF);
    aBF.SetShapes(aLE);
    aBF.SetRunParallel(myRunParallel);
    //
  } // for (i=0; i<aNbS; ++i) {

  // close preparation task
  aPSOuter.Next();
  //
  int aNbBF = aVBF.Length();
  // Set progress range for each task to be run in parallel
  Message_ProgressScope aPSParallel(aPSOuter.Next(9), "Splitting faces", aNbBF);
  for (int iF = 0; iF < aNbBF; iF++)
  {
    BOPAlgo_SplitFace& aBF = aVBF.ChangeValue(iF);
    aBF.SetProgressRange(aPSParallel.Next());
  }
  //===================================================
  BOPTools_Parallel::Perform(myRunParallel, aVBF);
  //===================================================
  if (UserBreak(aPSOuter))
  {
    return;
  }
  for (k = 0; k < aNbBF; ++k)
  {
    BOPAlgo_BuilderFace& aBF = aVBF(k);
    aFacesIm.Add(myDS->Index(aBF.Face()), aBF.Areas());
    myReport->Merge(aBF.GetReport());
  }

  aNbBF = aFacesIm.Extent();
  for (k = 1; k <= aNbBF; ++k)
  {
    const TopoDS_Face& aF                      = TopoDS::Face(myDS->Shape(aFacesIm.FindKey(k)));
    anOriF                                     = aF.Orientation();
    const NCollection_List<TopoDS_Shape>& aLFR = aFacesIm(k);
    //
    NCollection_List<TopoDS_Shape>* pLFIm = myImages.Bound(aF, NCollection_List<TopoDS_Shape>());
    aIt.Initialize(aLFR);
    for (; aIt.More(); aIt.Next())
    {
      TopoDS_Shape& aFR = aIt.ChangeValue();
      if (anOriF == TopAbs_REVERSED)
        aFR.Orientation(TopAbs_REVERSED);
      pLFIm->Append(aFR);
    }
  }
  //
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~scope t
}

//=================================================================================================

typedef NCollection_IndexedDataMap<BOPTools_Set, NCollection_List<TopoDS_Shape>>
  BOPAlgo_IndexedDataMapOfSetListOfShape;

static void AddEdgeSet(const TopoDS_Shape&                           theS,
                       BOPAlgo_IndexedDataMapOfSetListOfShape&       theMap,
                       const occ::handle<NCollection_BaseAllocator>& theAllocator)
{
  // Make set
  BOPTools_Set aSE;
  aSE.Add(theS, TopAbs_EDGE);
  // Add set to the map, keeping connection to the shape
  NCollection_List<TopoDS_Shape>* pLF = theMap.ChangeSeek(aSE);
  if (!pLF)
    pLF = &theMap(theMap.Add(aSE, NCollection_List<TopoDS_Shape>(theAllocator)));
  pLF->Append(theS);
}

//=================================================================================================

void BOPAlgo_Builder::FillSameDomainFaces(const Message_ProgressRange& theRange)
{
  // It is necessary to analyze all Face/Face intersections
  // and find all faces with equal sets of edges
  const NCollection_Vector<BOPDS_InterfFF>& aFFs   = myDS->InterfFF();
  int                                       aNbFFs = aFFs.Length();
  if (!aNbFFs)
    return;

  Message_ProgressScope aPSOuter(theRange, NULL, 10);

  occ::handle<NCollection_BaseAllocator> aAllocator = new NCollection_IncAllocator;

  // Vector to store the indices of faces for future sorting
  // for making the SD face for the group from the face with
  // smallest index in Data structure
  NCollection_Vector<int> aFIVec(256, aAllocator);
  // Fence map to avoid repeated checks of the same face.
  NCollection_Map<int> aMFence(1, aAllocator);

  // Fill the vector with indices of faces
  for (int i = 0; i < aNbFFs; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    const BOPDS_InterfFF& aFF = aFFs(i);
    // get indices
    int nF[2];
    aFF.Indices(nF[0], nF[1]);
    // store indices to the vector
    for (int j = 0; j < 2; ++j)
    {
      if (!myDS->HasFaceInfo(nF[j]))
        continue;

      if (!aMFence.Add(nF[j]))
        continue;

      aFIVec.Appended() = nF[j];
    }
  }

  // Sort the indices
  std::sort(aFIVec.begin(), aFIVec.end());

  // Data map of set of edges with all faces having this set
  NCollection_IndexedDataMap<BOPTools_Set, NCollection_List<TopoDS_Shape>> anESetFaces(1,
                                                                                       aAllocator);
  // Map of planar bounded faces. If such faces have the same Edge set
  // they are considered Same domain, without additional check.
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFPlanar(1, aAllocator);

  int aNbF = aFIVec.Length();
  for (int i = 0; i < aNbF; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    const int              nF  = aFIVec(i);
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(nF);
    const TopoDS_Shape&    aF  = aSI.Shape();

    bool bCheckPlanar = false;
    {
      // At this stage, context should contain adaptor for all intersected faces,
      // so getting a type of the underlying surface should be done at no cost.
      if (myContext->SurfaceAdaptor(TopoDS::Face(aF)).GetType() == GeomAbs_Plane)
      {
        // Check bounding box of the face - it should not be open in any side
        const Bnd_Box& aBox = aSI.Box();
        bCheckPlanar        = !(aBox.IsOpenXmin() || aBox.IsOpenXmax() || aBox.IsOpenYmin()
                         || aBox.IsOpenYmax() || aBox.IsOpenZmin() || aBox.IsOpenZmax());
      }
    }

    const NCollection_List<TopoDS_Shape>* pLFSp = myImages.Seek(aF);
    if (pLFSp)
    {
      NCollection_List<TopoDS_Shape>::Iterator aItLF(*pLFSp);
      for (; aItLF.More(); aItLF.Next())
      {
        AddEdgeSet(aItLF.Value(), anESetFaces, aAllocator);
        if (bCheckPlanar)
          aMFPlanar.Add(aItLF.Value());
      }
    }
    else
    {
      AddEdgeSet(aF, anESetFaces, aAllocator);
      if (bCheckPlanar)
        aMFPlanar.Add(aF);
    }
  }

  // Store pairs of faces with equal set of edges to check if they are really Same Domain
  BOPAlgo_VectorOfPairOfShapeBoolean aVPSB;

  // Back and forth map of SD faces to make the blocks
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMSLS(1, aAllocator);

  int aNbSets = anESetFaces.Extent();
  for (int i = 1; i <= aNbSets; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    const NCollection_List<TopoDS_Shape>& aLF = anESetFaces(i);
    if (aLF.Extent() < 2)
      continue;

    // All possible pairs from <aLF> should be checked
    NCollection_List<TopoDS_Shape>::Iterator aIt1(aLF);
    for (; aIt1.More(); aIt1.Next())
    {
      const TopoDS_Shape& aF1          = aIt1.Value();
      bool                bCheckPlanar = aMFPlanar.Contains(aF1);

      NCollection_List<TopoDS_Shape>::Iterator aIt2 = aIt1;
      for (aIt2.Next(); aIt2.More(); aIt2.Next())
      {
        const TopoDS_Shape& aF2 = aIt2.Value();
        if (bCheckPlanar && aMFPlanar.Contains(aF2))
        {
          // Consider planar bounded faces as Same Domain without additional check
          BOPAlgo_Tools::FillMap(aF1, aF2, aDMSLS, aAllocator);
          continue;
        }
        // Add pair for analysis
        BOPAlgo_PairOfShapeBoolean& aPSB = aVPSB.Appended();
        aPSB.Shape1()                    = aF1;
        aPSB.Shape2()                    = aF2;
        aPSB.SetFuzzyValue(myFuzzyValue);
      }
    }
  }

  aPSOuter.Next();

  // Set progress range for each task to be run in parallel
  Message_ProgressScope aPSParallel(aPSOuter.Next(6), "Checking SD faces", aVPSB.Size());
  for (int iPSB = 0; iPSB < aVPSB.Size(); ++iPSB)
  {
    aVPSB.ChangeValue(iPSB).SetProgressRange(aPSParallel.Next());
  }
  //================================================================
  // Perform analysis
  BOPTools_Parallel::Perform(myRunParallel, aVPSB, myContext);
  //================================================================
  if (UserBreak(aPSOuter))
  {
    return;
  }

  NCollection_List<NCollection_List<TopoDS_Shape>> aMBlocks(aAllocator);
  // Fill map with SD faces to make the blocks
  int aNbPairs = aVPSB.Length();
  for (int i = 0; i < aNbPairs; ++i)
  {
    BOPAlgo_PairOfShapeBoolean& aPSB = aVPSB(i);
    if (aPSB.Flag())
      BOPAlgo_Tools::FillMap(aPSB.Shape1(), aPSB.Shape2(), aDMSLS, aAllocator);
  }
  aVPSB.Clear();

  // Make blocks of SD faces using the back and forth map
  BOPAlgo_Tools::MakeBlocks(aDMSLS, aMBlocks, aAllocator);

  Message_ProgressScope aPS(aPSOuter.Next(3), "Filling same domain faces map", aMBlocks.Size());
  // Fill same domain faces map
  NCollection_List<NCollection_List<TopoDS_Shape>>::Iterator aItB(aMBlocks);
  for (; aItB.More(); aItB.Next(), aPS.Next())
  {
    if (UserBreak(aPS))
    {
      return;
    }
    const NCollection_List<TopoDS_Shape>& aLSD = aItB.Value();
    // If the group contains some original faces, the one with minimal
    // index in the DS will be chosen as the SD for the whole group.
    // If there are no original faces in the group, the first face from
    // the group will be used as the SD face.
    // Such SD face will be representative of the whole group in the result.
    TopoDS_Face*                             pFSD  = NULL;
    int                                      nFMin = ::IntegerLast();
    NCollection_List<TopoDS_Shape>::Iterator aItLF(aLSD);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aF = aItLF.Value();
      // Check the index of the face in DS
      const int nF = myDS->Index(aF);
      if (nF >= 0)
      {
        // The fact that the face is found in the DS, means that
        // the face has not been change, and thus it is original one.
        //
        // Such face does not have any splits, but have an SD face.
        // Consider it being split.
        myImages.Bound(aF, NCollection_List<TopoDS_Shape>())->Append(aF);

        // For the SD face chose the one with minimal index
        if (nF < nFMin)
        {
          nFMin = nF;
          pFSD  = (TopoDS_Face*)&aF;
        }
      }
    }

    if (!pFSD)
    {
      // No original faces in the group, take the first one
      pFSD = (TopoDS_Face*)&aLSD.First();
    }

    // Save all SD connections
    aItLF.Initialize(aLSD);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aF = aItLF.Value();
      myShapesSD.Bind(aF, *pFSD);
    }
  }

  // Update the map of images with SD faces and
  // fill the map of origins.
  int aNbS = myDS->NbSourceShapes();
  for (int i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
      continue;

    const TopoDS_Shape&             aF    = aSI.Shape();
    NCollection_List<TopoDS_Shape>* pLFIm = myImages.ChangeSeek(aF);
    if (!pLFIm)
      continue;

    NCollection_List<TopoDS_Shape>::Iterator aItLFIm(*pLFIm);
    for (; aItLFIm.More(); aItLFIm.Next())
    {
      TopoDS_Shape&       aFIm = aItLFIm.ChangeValue();
      const TopoDS_Shape* pFSD = myShapesSD.Seek(aFIm);
      if (pFSD)
        // Update image with SD face
        aFIm = *pFSD;

      // Fill the map of origins
      NCollection_List<TopoDS_Shape>* pLFOr = myOrigins.ChangeSeek(aFIm);
      if (!pLFOr)
        pLFOr = myOrigins.Bound(aFIm, NCollection_List<TopoDS_Shape>());
      pLFOr->Append(aF);
    }
  }

  aMBlocks.Clear();
  aDMSLS.Clear();
}

//=================================================================================================

void BOPAlgo_Builder::FillInternalVertices(const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPSOuter(theRange, NULL, 1);

  // Vector of pairs of Vertex/Face for classification of the vertices
  // relatively faces, and adding them as internal into the faces
  BOPAlgo_VectorOfVFI aVVFI;

  int aNbS = myDS->NbSourceShapes();
  for (int i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
      continue;

    if (UserBreak(aPSOuter))
    {
      return;
    }

    const TopoDS_Shape&                   aF    = aSI.Shape();
    const NCollection_List<TopoDS_Shape>* pLFIm = myImages.Seek(aF);
    if (!pLFIm)
      continue;

    // Find vertices to add as internal into the splits
    NCollection_List<int> aLIAV;
    myDS->AloneVertices(i, aLIAV);

    // Add vertices and faces for classification
    NCollection_List<int>::Iterator aItLV(aLIAV);
    for (; aItLV.More(); aItLV.Next())
    {
      TopoDS_Vertex aV = TopoDS::Vertex(myDS->Shape(aItLV.Value()));
      aV.Orientation(TopAbs_INTERNAL);

      NCollection_List<TopoDS_Shape>::Iterator aItLFIm(*pLFIm);
      for (; aItLFIm.More(); aItLFIm.Next())
      {
        const TopoDS_Face& aFIm = TopoDS::Face(aItLFIm.Value());
        // Make the pair
        BOPAlgo_VFI& aVFI = aVVFI.Appended();
        aVFI.SetVertex(aV);
        aVFI.SetFace(aFIm);
        aVFI.SetFuzzyValue(myFuzzyValue);
      }
    }
  }

  // Set progress range for each task to be run in parallel
  Message_ProgressScope aPSParallel(aPSOuter.Next(), "Looking for internal shapes", aVVFI.Size());
  for (int iVFI = 0; iVFI < aVVFI.Size(); ++iVFI)
  {
    aVVFI.ChangeValue(iVFI).SetProgressRange(aPSParallel.Next());
  }
  // Perform classification
  //================================================================
  BOPTools_Parallel::Perform(myRunParallel, aVVFI, myContext);
  //================================================================
  if (UserBreak(aPSOuter))
  {
    return;
  }

  int aNbVFI = aVVFI.Length();
  for (int i = 0; i < aNbVFI; ++i)
  {
    BOPAlgo_VFI& aVFI = aVVFI(i);
    if (aVFI.IsInternal())
    {
      TopoDS_Vertex& aV = aVFI.Vertex();
      TopoDS_Face&   aF = aVFI.Face();
      BRep_Builder().Add(aF, aV);
    }
  }
}

//=======================================================================
// function : HasMultiConnected
// purpose  : Checks if the edge has multi-connected vertices.
//=======================================================================
static bool HasMultiConnected(
  const TopoDS_Edge& theEdge,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theMap)
{
  TopoDS_Iterator itV(theEdge);
  for (; itV.More(); itV.Next())
  {
    const TopoDS_Shape&             aV    = itV.Value();
    NCollection_List<TopoDS_Shape>* pList = theMap.ChangeSeek(aV);
    if (!pList)
    {
      pList = theMap.Bound(aV, NCollection_List<TopoDS_Shape>());
      pList->Append(theEdge);
    }
    else
    {
      // The list is expected to be 1-2 elements long,
      // thus using "Contains" is safe.
      if (!pList->Contains(theEdge))
        pList->Append(theEdge);

      if (pList->Extent() > 2)
        return true;
    }
  }
  return false;
}

//=======================================================================
// function : BuildDraftFace
// purpose  : Build draft faces, updating the bounding edges,
//           according to the information stored into the <theImages> map
//=======================================================================
TopoDS_Face BuildDraftFace(
  const TopoDS_Face& theFace,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                     theImages,
  occ::handle<IntTools_Context>&     theCtx,
  const occ::handle<Message_Report>& theReport)
{
  BRep_Builder aBB;
  // Take the information from the original face
  TopLoc_Location                  aLoc;
  const occ::handle<Geom_Surface>& aS   = BRep_Tool::Surface(theFace, aLoc);
  const double                     aTol = BRep_Tool::Tolerance(theFace);
  // Make the new face, without any wires
  TopoDS_Face aDraftFace;
  aBB.MakeFace(aDraftFace, aS, aLoc, aTol);

  // Check if the thin face can be split by a vertex - in this case
  // this vertex will be contained in more than two edges. Thus, count
  // the vertices appearance, and if the multi-connexity is met return
  // the null face to use the BuilderFace algorithm for checking the
  // possibility of split.
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aVerticesCounter;

  // Check that the edges of the initial face have not been unified during intersection.
  // Otherwise, it will be necessary to check validity of the new wires.
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEdges;

  // Update wires of the original face and add them to draft face
  TopoDS_Iterator aItW(theFace.Oriented(TopAbs_FORWARD));
  for (; aItW.More(); aItW.Next())
  {
    const TopoDS_Shape& aW = aItW.Value();
    if (aW.ShapeType() != TopAbs_WIRE)
      continue;

    // Rebuild wire using images of edges
    TopoDS_Iterator aItE(aW.Oriented(TopAbs_FORWARD));
    if (!aItE.More())
      continue;

    TopoDS_Wire aNewWire;
    aBB.MakeWire(aNewWire);

    for (; aItE.More(); aItE.Next())
    {
      const TopoDS_Edge& aE = TopoDS::Edge(aItE.Value());

      TopAbs_Orientation anOriE = aE.Orientation();
      if (anOriE == TopAbs_INTERNAL)
      {
        // The internal edges could split the original face on halves.
        // Thus, use the BuilderFace algorithm to build the new face.
        return TopoDS_Face();
      }

      // Check if the original edge is degenerated
      bool bIsDegenerated = BRep_Tool::Degenerated(aE);
      // Check if the original edge is closed on the face
      bool bIsClosed = BRep_Tool::IsClosed(aE, theFace);

      // Check for the splits of the edge
      const NCollection_List<TopoDS_Shape>* pLEIm = theImages.Seek(aE);
      if (!pLEIm)
      {
        // Check if the edge has multi-connected vertices
        if (!bIsDegenerated && HasMultiConnected(aE, aVerticesCounter))
          return TopoDS_Face();

        // Check edges unification
        if (!bIsClosed && !aMEdges.Add(aE))
          return TopoDS_Face();

        aBB.Add(aNewWire, aE);
        continue;
      }

      NCollection_List<TopoDS_Shape>::Iterator aItLEIm(*pLEIm);
      for (; aItLEIm.More(); aItLEIm.Next())
      {
        TopoDS_Edge& aSp = TopoDS::Edge(aItLEIm.ChangeValue());

        // Check if the split has multi-connected vertices
        if (!bIsDegenerated && HasMultiConnected(aSp, aVerticesCounter))
          return TopoDS_Face();

        // Check edges unification
        if (!bIsClosed && !aMEdges.Add(aSp))
          return TopoDS_Face();

        aSp.Orientation(anOriE);
        if (bIsDegenerated)
        {
          aBB.Add(aNewWire, aSp);
          continue;
        }

        // If the original edge is closed on the face check closeness
        // of the split edge and if it is not closed make the second PCurve
        if (bIsClosed && !BRep_Tool::IsClosed(aSp, theFace))
          BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSp, theFace);

        // Check if the split should be reversed
        if (BOPTools_AlgoTools::IsSplitToReverseWithWarn(aSp, aE, theCtx, theReport))
          aSp.Reverse();

        aBB.Add(aNewWire, aSp);
      }
    }

    aNewWire.Orientation(aW.Orientation());
    aNewWire.Closed(BRep_Tool::IsClosed(aNewWire));
    aBB.Add(aDraftFace, aNewWire);
  }

  if (theFace.Orientation() == TopAbs_REVERSED)
    aDraftFace.Reverse();

  return aDraftFace;
}
