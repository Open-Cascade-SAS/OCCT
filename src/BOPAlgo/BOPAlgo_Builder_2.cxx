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
#include <BOPAlgo_BuilderFace.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPCol_DataMapOfIntegerListOfShape.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfInteger.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_VectorOfCurve.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPDS_VectorOfPoint.hxx>
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_CoupleOfShape.hxx>
#include <BOPTools_DataMapOfShapeSet.hxx>
#include <BOPTools_ListOfCoupleOfShape.hxx>
#include <BOPTools_MapOfSet.hxx>
#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <BRep_Tool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Precision.hxx>
#include <IntTools_Context.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <algorithm>
//
static
  TopoDS_Face BuildDraftFace(const TopoDS_Face& theFace,
                             const BOPCol_DataMapOfShapeListOfShape& theImages,
                             Handle(IntTools_Context)& theCtx);

//=======================================================================
//class    : BOPAlgo_PairOfShapeBoolean
//purpose  : 
//=======================================================================
class BOPAlgo_PairOfShapeBoolean : public BOPAlgo_Algo {

 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_PairOfShapeBoolean() : 
    BOPAlgo_Algo(),
    myFlag(Standard_False) {
  }
  //
  virtual ~BOPAlgo_PairOfShapeBoolean() {
  }
  //
  TopoDS_Shape& Shape1() {
    return myShape1;
  }
  //
  TopoDS_Shape& Shape2() {
    return myShape2;
  }
  //
  Standard_Boolean& Flag() {
    return myFlag;
  }
  //
  void SetContext(const Handle(IntTools_Context)& aContext) {
    myContext=aContext;
  }
  //
  const Handle(IntTools_Context)& Context()const {
    return myContext;
  }
  //
  virtual void Perform() {
    BOPAlgo_Algo::UserBreak();
    //  
    const TopoDS_Face& aFj=*((TopoDS_Face*)&myShape1);
    const TopoDS_Face& aFk=*((TopoDS_Face*)&myShape2);
    myFlag=BOPTools_AlgoTools::AreFacesSameDomain(aFj, aFk, myContext, myFuzzyValue);
  }
  //
 protected: 
  Standard_Boolean myFlag;
  TopoDS_Shape myShape1;
  TopoDS_Shape myShape2;
  Handle(IntTools_Context) myContext;
};
//
typedef BOPCol_NCVector<BOPAlgo_PairOfShapeBoolean> \
  BOPAlgo_VectorOfPairOfShapeBoolean;
//
typedef BOPCol_ContextFunctor 
  <BOPAlgo_PairOfShapeBoolean,
  BOPAlgo_VectorOfPairOfShapeBoolean,
  Handle(IntTools_Context), 
  IntTools_Context> BOPCol_BuilderSDFaceFunctor;
//
typedef BOPCol_ContextCnt 
  <BOPCol_BuilderSDFaceFunctor,
  BOPAlgo_VectorOfPairOfShapeBoolean,
  Handle(IntTools_Context)> BOPAlgo_BuilderSDFaceCnt;
//
//=======================================================================
// BuilderFace
//
typedef BOPCol_NCVector<BOPAlgo_BuilderFace> BOPAlgo_VectorOfBuilderFace;
//
typedef BOPCol_Functor 
  <BOPAlgo_BuilderFace,
  BOPAlgo_VectorOfBuilderFace> BOPAlgo_BuilderFaceFunctor;
//
typedef BOPCol_Cnt 
  <BOPAlgo_BuilderFaceFunctor,
  BOPAlgo_VectorOfBuilderFace> BOPAlgo_BuilderFaceCnt;
//
//=======================================================================
//class    : BOPAlgo_VFI
//purpose  : 
//=======================================================================
class BOPAlgo_VFI : public BOPAlgo_Algo {

 public:
  DEFINE_STANDARD_ALLOC
  
  BOPAlgo_VFI() :
    BOPAlgo_Algo(),
    myFlag(-1) {
  }
  //
  virtual ~BOPAlgo_VFI(){
  }
  //
  void SetVertex(const TopoDS_Vertex& aV) {
    myV=aV;
  }
  //
  TopoDS_Vertex& Vertex() {
    return myV;
  }
  //
  void SetFace(const TopoDS_Face& aF) {
    myF=aF;
  }
  //
  TopoDS_Face& Face() {
    return myF;
  }
  //
  Standard_Integer Flag()const {
    return myFlag;
  }
  //
  void SetContext(const Handle(IntTools_Context)& aContext) {
    myContext=aContext;
  }
  //
  const Handle(IntTools_Context)& Context()const {
    return myContext;
  }
  //
  virtual void Perform() {
    Standard_Real aT1, aT2, dummy;
    //
    BOPAlgo_Algo::UserBreak();
    myFlag = myContext->ComputeVF(myV, myF, aT1, aT2, dummy, myFuzzyValue);
  }
  //
 protected:
  Standard_Integer myFlag;
  TopoDS_Vertex myV;
  TopoDS_Face myF;
  Handle(IntTools_Context) myContext;
};
//
typedef BOPCol_NCVector<BOPAlgo_VFI> BOPAlgo_VectorOfVFI; 
//
typedef BOPCol_ContextFunctor 
  <BOPAlgo_VFI,
  BOPAlgo_VectorOfVFI,
  Handle(IntTools_Context), 
  IntTools_Context> BOPAlgo_VFIFunctor;
//
typedef BOPCol_ContextCnt 
  <BOPAlgo_VFIFunctor,
  BOPAlgo_VectorOfVFI,
  Handle(IntTools_Context)> BOPAlgo_VFICnt;
//
//=======================================================================
//function : FillImagesFaces
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::FillImagesFaces()
{
  BuildSplitFaces();
  FillSameDomainFaces();
  FillImagesFaces1();
}
//=======================================================================
//function : BuildSplitFaces
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::BuildSplitFaces()
{
  Standard_Boolean bHasFaceInfo, bIsClosed, bIsDegenerated, bToReverse;
  Standard_Integer i, j, k, aNbS, aNbPBIn, aNbPBOn, aNbPBSc, aNbAV, nSp;
  TopoDS_Face aFF, aFSD;
  TopoDS_Edge aSp, aEE;
  TopAbs_Orientation anOriF, anOriE;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPCol_ListOfInteger aLIAV;
  BOPCol_MapOfShape aMFence;
  Handle(NCollection_BaseAllocator) aAllocator;
  BOPAlgo_VectorOfBuilderFace aVBF;
  //
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~scope f
  aAllocator=
    NCollection_BaseAllocator::CommonBaseAllocator();
  //
  BOPCol_ListOfShape aLE(aAllocator);
  BOPCol_MapOfShape aMDE(100, aAllocator);
  //
  // Build temporary map of faces images to avoid rebuilding
  // of the faces without any IN or section edges
  NCollection_IndexedDataMap<Standard_Integer, BOPCol_ListOfShape> aFacesIm;
  //
  aNbS=myDS->NbSourceShapes();
  //
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    if (aSI.ShapeType()!=TopAbs_FACE) {
      continue;
    }
    //
    const TopoDS_Face& aF=(*(TopoDS_Face*)(&aSI.Shape()));
    Standard_Boolean isUClosed = Standard_False,
                     isVClosed = Standard_False,
                     isChecked = Standard_False;
    //
    bHasFaceInfo=myDS->HasFaceInfo(i);
    if(!bHasFaceInfo) {
      continue;
    }
    //
    const BOPDS_FaceInfo& aFI=myDS->FaceInfo(i);
    //
    const BOPDS_IndexedMapOfPaveBlock& aMPBIn=aFI.PaveBlocksIn();
    const BOPDS_IndexedMapOfPaveBlock& aMPBOn=aFI.PaveBlocksOn();
    const BOPDS_IndexedMapOfPaveBlock& aMPBSc=aFI.PaveBlocksSc();
    aLIAV.Clear();
    myDS->AloneVertices(i, aLIAV);
    
    aNbPBIn=aMPBIn.Extent();
    aNbPBOn=aMPBOn.Extent();
    aNbPBSc=aMPBSc.Extent();
    aNbAV=aLIAV.Extent();
    if (!aNbPBIn && !aNbPBOn && !aNbPBSc && !aNbAV) { // not compete
      continue;
    }

    if (!aNbPBIn && !aNbPBSc)
    {
      // No internal parts for the face, so just build the draft face
      // and keep it to pass directly into result.
      // If the original face has any internal edges, the draft face
      // will be null, as the internal edges may split the face on parts
      // (as in the case "bugs modalg_5 bug25245_1").
      // The BuilderFace algorithm will be called in this case.
      TopoDS_Face aFD = BuildDraftFace(aF, myImages, myContext);
      if (!aFD.IsNull())
      {
        aFacesIm(aFacesIm.Add(i, BOPCol_ListOfShape())).Append(aFD);
        continue;
      }
    }

    aMFence.Clear();
    //
    anOriF=aF.Orientation();
    aFF=aF;
    aFF.Orientation(TopAbs_FORWARD);
    //
    // 1. Fill the edges set for the face aFF -> LE
    aLE.Clear();

    // 1.1 Bounding edges
    aExp.Init(aFF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aExp.Current()));
      anOriE=aE.Orientation();
      //
      if (!myImages.IsBound(aE)) {
        if (anOriE==TopAbs_INTERNAL) {
          aEE=aE;
          aEE.Orientation(TopAbs_FORWARD);
          aLE.Append(aEE);
          aEE.Orientation(TopAbs_REVERSED);
          aLE.Append(aEE);
        }
        else {
          aLE.Append(aE);
        }

        continue;
      }

      if(!isChecked)
      {
        const Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aF);
        GeomLib::IsClosed(aSurf, BRep_Tool::Tolerance(aE),
          isUClosed, isVClosed);

        isChecked = Standard_True;
      }

      bIsClosed = Standard_False;

      if((isUClosed || isVClosed) && BRep_Tool::IsClosed(aE, aF)) 
      {

        Standard_Boolean isUIso = Standard_False, isVIso = Standard_False;
        BOPTools_AlgoTools2D::IsEdgeIsoline(aE, aF, isUIso, isVIso);

        bIsClosed = ((isUClosed && isUIso) || (isVClosed && isVIso));
      }

      bIsDegenerated=BRep_Tool::Degenerated(aE);

      const BOPCol_ListOfShape& aLIE=myImages.Find(aE);
      aIt.Initialize(aLIE);
      for (; aIt.More(); aIt.Next()) {
        aSp=(*(TopoDS_Edge*)(&aIt.Value()));
        if (bIsDegenerated) {
          aSp.Orientation(anOriE);
          aLE.Append(aSp);
          continue;
        }
        //
        if (anOriE==TopAbs_INTERNAL) {
          aSp.Orientation(TopAbs_FORWARD);
          aLE.Append(aSp);
          aSp.Orientation(TopAbs_REVERSED);
          aLE.Append(aSp);
          continue;
        }
          //
        if (bIsClosed) {
          if (aMFence.Add(aSp)) {
            if (!BRep_Tool::IsClosed(aSp, aF)){
              BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSp, aF);
            }
            //
            aSp.Orientation(TopAbs_FORWARD);
            aLE.Append(aSp);
            aSp.Orientation(TopAbs_REVERSED);
            aLE.Append(aSp);
          }// if (aMFence.Add(aSp))
          continue;
        }// if (bIsClosed){
        //
        aSp.Orientation(anOriE);
        bToReverse=BOPTools_AlgoTools::IsSplitToReverse(aSp, aE, myContext);
        if (bToReverse) {
          aSp.Reverse();
        }
        aLE.Append(aSp);
      }// for (; aIt.More(); aIt.Next()) {
    }// for (; aExp.More(); aExp.Next()) {
    // 
    //
    // 1.2 In edges
    for (j=1; j<=aNbPBIn; ++j) {
      const Handle(BOPDS_PaveBlock)& aPB=aMPBIn(j);
      nSp=aPB->Edge();
      aSp=(*(TopoDS_Edge*)(&myDS->Shape(nSp)));
      //
      aSp.Orientation(TopAbs_FORWARD);
      aLE.Append(aSp);
      aSp.Orientation(TopAbs_REVERSED);
      aLE.Append(aSp);
    }
    //
    //
    // 1.3 Section edges
    for (j=1; j<=aNbPBSc; ++j) {
      const Handle(BOPDS_PaveBlock)& aPB=aMPBSc(j);
      nSp=aPB->Edge();
      aSp=(*(TopoDS_Edge*)(&myDS->Shape(nSp)));
      //
      aSp.Orientation(TopAbs_FORWARD);
      aLE.Append(aSp);
      aSp.Orientation(TopAbs_REVERSED);
      aLE.Append(aSp);
    }
    //
    if (!myPaveFiller->NonDestructive()) {
      // speed up for planar faces
      BRepLib::BuildPCurveForEdgesOnPlane(aLE, aFF);
    }
    // 3 Build split faces
    BOPAlgo_BuilderFace& aBF=aVBF.Append1();
    aBF.SetFace(aF);
    aBF.SetShapes(aLE);
    aBF.SetRunParallel(myRunParallel);
    aBF.SetProgressIndicator(myProgressIndicator);
    //
  }// for (i=0; i<aNbS; ++i) {
  //
  //===================================================
  BOPAlgo_BuilderFaceCnt::Perform(myRunParallel, aVBF);
  //===================================================
  //
  Standard_Integer aNbBF = aVBF.Extent();
  for (k = 0; k < aNbBF; ++k)
  {
    BOPAlgo_BuilderFace& aBF = aVBF(k);
    aFacesIm.Add(myDS->Index(aBF.Face()), aBF.Areas());
  }

  aNbBF = aFacesIm.Extent();
  for (k = 1; k <= aNbBF; ++k)
  {
    const TopoDS_Face& aF = TopoDS::Face(myDS->Shape(aFacesIm.FindKey(k)));
    anOriF = aF.Orientation();
    const BOPCol_ListOfShape& aLFR = aFacesIm(k);
    //
    BOPCol_ListOfShape* pLFIm = mySplits.Bound(aF, BOPCol_ListOfShape());
    aIt.Initialize(aLFR);
    for (; aIt.More(); aIt.Next()) {
      TopoDS_Shape& aFR=aIt.ChangeValue();
      if (anOriF==TopAbs_REVERSED)
        aFR.Orientation(TopAbs_REVERSED);
      pLFIm->Append(aFR);
    }
  }
  //
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~scope t
}

//=======================================================================
//function : AddEdgeSet
//purpose  : 
//=======================================================================
typedef
  NCollection_IndexedDataMap<BOPTools_Set,
                             BOPCol_ListOfShape,
                             BOPTools_SetMapHasher> BOPAlgo_IndexedDataMapOfSetListOfShape;

static void AddEdgeSet(const TopoDS_Shape& theS,
                       BOPAlgo_IndexedDataMapOfSetListOfShape& theMap,
                       const Handle(NCollection_BaseAllocator)& theAllocator)
{
  // Make set
  BOPTools_Set aSE;
  aSE.Add(theS, TopAbs_EDGE);
  // Add set to the map, keeping connection to the shape
  BOPCol_ListOfShape* pLF = theMap.ChangeSeek(aSE);
  if (!pLF)
    pLF = &theMap(theMap.Add(aSE, BOPCol_ListOfShape(theAllocator)));
  pLF->Append(theS);
}

//=======================================================================
//function : FillSameDomainFaces
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::FillSameDomainFaces()
{
  // It is necessary to analyze all Face/Face intersections
  // and find all faces with equal sets of edges
  const BOPDS_VectorOfInterfFF& aFFs = myDS->InterfFF();
  Standard_Integer aNbFFs = aFFs.Extent();
  if (!aNbFFs)
    return;

  Handle(NCollection_BaseAllocator) aAllocator = new NCollection_IncAllocator;

  // Vector to store the indices of faces for future sorting
  // for making the SD face for the group from the face with
  // smallest index in Data structure
  BOPCol_NCVector<Standard_Integer> aFIVec(256, aAllocator);
  // Fence map to avoid repeated checks of the same face.
  BOPCol_MapOfInteger aMFence(1, aAllocator);

  // Fill the vector with indices of faces
  for (Standard_Integer i = 0; i < aNbFFs; ++i)
  {
    const BOPDS_InterfFF& aFF = aFFs(i);
    // get indices
    Standard_Integer nF[2];
    aFF.Indices(nF[0], nF[1]);
    // store indices to the vector
    for (Standard_Integer j = 0; j < 2; ++j)
    {
      if (!myDS->HasFaceInfo(nF[j]))
        continue;

      if (!aMFence.Add(nF[j]))
        continue;

      aFIVec.Append1() = nF[j];
    }
  }

  // Sort the indices
  std::sort(aFIVec.begin(), aFIVec.end());

  // Data map of set of edges with all faces having this set
  NCollection_IndexedDataMap<BOPTools_Set,
                             BOPCol_ListOfShape,
                             BOPTools_SetMapHasher> anESetFaces(1, aAllocator);
  // Map of planar bounded faces. If such faces have the same Edge set
  // they are considered Same domain, without additional check.
  BOPCol_MapOfShape aMFPlanar(1, aAllocator);

  Standard_Integer aNbF = aFIVec.Extent();
  for (Standard_Integer i = 0; i < aNbF; ++i)
  {
    const Standard_Integer nF = aFIVec(i);
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(nF);
    const TopoDS_Shape& aF = aSI.Shape();

    Standard_Boolean bCheckPlanar = Standard_False;
    {
      // At this stage, context should contain adaptor for all intersected faces,
      // so getting a type of the underlying surface should be done at no cost.
      if (myContext->SurfaceAdaptor(TopoDS::Face(aF)).GetType() == GeomAbs_Plane)
      {
        // Check bounding box of the face - it should not be open in any side
        const Bnd_Box& aBox = aSI.Box();
        bCheckPlanar = !(aBox.IsOpenXmin() || aBox.IsOpenXmax() ||
                         aBox.IsOpenYmin() || aBox.IsOpenYmax() ||
                         aBox.IsOpenZmin() || aBox.IsOpenZmax());
      }
    }

    const BOPCol_ListOfShape* pLFSp = mySplits.Seek(aF);
    if (pLFSp)
    {
      BOPCol_ListIteratorOfListOfShape aItLF(*pLFSp);
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
  BOPCol_IndexedDataMapOfShapeListOfShape aDMSLS(1, aAllocator);

  Standard_Integer aNbSets = anESetFaces.Extent();
  for (Standard_Integer i = 1; i <= aNbSets; ++i)
  {
    const BOPCol_ListOfShape& aLF = anESetFaces(i);
    if (aLF.Extent() < 2)
      continue;

    // All possible pairs from <aLF> should be checked
    BOPCol_ListIteratorOfListOfShape aIt1(aLF);
    for (; aIt1.More(); aIt1.Next())
    {
      const TopoDS_Shape& aF1 = aIt1.Value();
      Standard_Boolean bCheckPlanar = aMFPlanar.Contains(aF1);

      BOPCol_ListIteratorOfListOfShape aIt2 = aIt1;
      for (aIt2.Next(); aIt2.More(); aIt2.Next())
      {
        const TopoDS_Shape& aF2 = aIt2.Value();
        if (bCheckPlanar && aMFPlanar.Contains(aF2))
        {
          // Consider planar bounded faces as Same Domain without additional check
          BOPAlgo_Tools::FillMap<TopoDS_Shape, TopTools_ShapeMapHasher>(aF1, aF2, aDMSLS, aAllocator);
          continue;
        }
        // Add pair for analysis
        BOPAlgo_PairOfShapeBoolean& aPSB = aVPSB.Append1();
        aPSB.Shape1() = aF1;
        aPSB.Shape2() = aF2;
        aPSB.SetFuzzyValue(myFuzzyValue);
        aPSB.SetProgressIndicator(myProgressIndicator);
      }
    }
  }

  //================================================================
  // Perform analysis
  BOPAlgo_BuilderSDFaceCnt::Perform(myRunParallel, aVPSB, myContext);
  //================================================================

  NCollection_List<BOPCol_ListOfShape> aMBlocks(aAllocator);
  // Fill map with SD faces to make the blocks
  Standard_Integer aNbPairs = aVPSB.Extent();
  for (Standard_Integer i = 0; i < aNbPairs; ++i)
  {
    BOPAlgo_PairOfShapeBoolean& aPSB = aVPSB(i);
    if (aPSB.Flag())
      BOPAlgo_Tools::FillMap<TopoDS_Shape, TopTools_ShapeMapHasher>
       (aPSB.Shape1(), aPSB.Shape2(), aDMSLS, aAllocator);
  }
  aVPSB.Clear();

  // Make blocks of SD faces using the back and forth map
  BOPAlgo_Tools::MakeBlocks<TopoDS_Shape, TopTools_ShapeMapHasher>
    (aDMSLS, aMBlocks, aAllocator);

  // Fill same domain faces map
  NCollection_List<BOPCol_ListOfShape>::Iterator aItB(aMBlocks);
  for (; aItB.More(); aItB.Next())
  {
    const BOPCol_ListOfShape& aLSD = aItB.Value();
    // First face will be SD face for all faces in the group
    const TopoDS_Shape& aFSD1 = aLSD.First();
    BOPCol_ListIteratorOfListOfShape aItLF(aLSD);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aFSD = aItLF.Value();
      myShapesSD.Bind(aFSD, aFSD1);
      // If the face has no splits but have an SD face, it is considered as being split
      if (!mySplits.IsBound(aFSD))
        mySplits.Bound(aFSD, BOPCol_ListOfShape())->Append(aFSD);
    }
  }
  aMBlocks.Clear();
  aDMSLS.Clear();
}
//=======================================================================
// function: FillImagesFaces1
// purpose: 
//=======================================================================
void BOPAlgo_Builder::FillImagesFaces1()
{
  Standard_Integer i, aNbS, iSense, nVx, aNbVFI, iFlag;
  TopoDS_Face aFSD;
  TopoDS_Vertex aVx;
  BRep_Builder aBB;
  BOPCol_ListOfInteger aLIAV;
  BOPCol_ListOfShape aLFIm;
  BOPCol_ListIteratorOfListOfInteger aItV;
  BOPCol_ListIteratorOfListOfShape aItLS, aItF;
  BOPAlgo_VectorOfVFI aVVFI;
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
    if (!mySplits.IsBound(aF)) {
      continue;
    }
    // 
    // 1.
    aLIAV.Clear();
    myDS->AloneVertices(i, aLIAV);
    aLFIm.Clear();
    //
    const BOPCol_ListOfShape& aLSp=mySplits.Find(aF);
    aItLS.Initialize(aLSp);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Face& aFSp=(*(TopoDS_Face*)(&aItLS.Value()));
      if (!myShapesSD.IsBound(aFSp)) {
        aLFIm.Append(aFSp);
      }
      else {
        aFSD=(*(TopoDS_Face*)(&myShapesSD.Find(aFSp)));
        iSense=BOPTools_AlgoTools::Sense(aFSp, aFSD, myContext);
        if (iSense<0) {
          aFSD.Reverse();
        }
        aLFIm.Append(aFSD);
      }
    }
    //
    //FillInternalVertices(aLFIm, aLIAV);
    //
    myImages.Bind(aF, aLFIm); 
    //
    // 2. fill myOrigins
    aItLS.Initialize(aLFIm);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Face& aFSp=(*(TopoDS_Face*)(&aItLS.Value()));
      //
      BOPCol_ListOfShape* pLOr = myOrigins.ChangeSeek(aFSp);
      if (!pLOr) {
        pLOr = myOrigins.Bound(aFSp, BOPCol_ListOfShape());
      }
      pLOr->Append(aF);
    }
    //
    // 3.
    aItV.Initialize(aLIAV);
    for (; aItV.More(); aItV.Next()) {
      nVx=aItV.Value();
      aVx=(*(TopoDS_Vertex*)(&myDS->Shape(nVx)));
      aVx.Orientation(TopAbs_INTERNAL);
      //
      aItF.Initialize(aLFIm);
      for (; aItF.More(); aItF.Next()) {
        TopoDS_Face& aFy=(*(TopoDS_Face*)(&aItF.Value()));
        //
        BOPAlgo_VFI& aVFI=aVVFI.Append1();
        aVFI.SetVertex(aVx);
        aVFI.SetFace(aFy);
        aVFI.SetFuzzyValue(myFuzzyValue);
        aVFI.SetProgressIndicator(myProgressIndicator);
      }
    }
  }// for (i=0; i<aNbS; ++i) {
  //
  // 4. 
  aNbVFI=aVVFI.Extent();
  //================================================================
  BOPAlgo_VFICnt::Perform(myRunParallel, aVVFI, myContext);
  //================================================================
  //
  for (i=0; i < aNbVFI; ++i) {
    BOPAlgo_VFI& aVFI=aVVFI(i);
    //
    iFlag=aVFI.Flag();
    if (!iFlag) {
      TopoDS_Vertex& aVertex=aVFI.Vertex();
      TopoDS_Face& aFy=aVFI.Face(); 
      aBB.Add(aFy, aVertex);
    }
  }
}
//=======================================================================
//function : BuildDraftFace
//purpose  : Build draft faces, updating the bounding edges,
//           according to the information stored into the <theImages> map
//=======================================================================
TopoDS_Face BuildDraftFace(const TopoDS_Face& theFace,
                           const BOPCol_DataMapOfShapeListOfShape& theImages,
                           Handle(IntTools_Context)& theCtx)
{
  BRep_Builder aBB;
  // Take the information from the original face
  TopLoc_Location aLoc;
  const Handle(Geom_Surface)& aS = BRep_Tool::Surface(theFace, aLoc);
  const Standard_Real aTol = BRep_Tool::Tolerance(theFace);
  // Make the new face, without any wires
  TopoDS_Face aDraftFace;
  aBB.MakeFace(aDraftFace, aS, aLoc, aTol);

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
        TopoDS_Face aNull;
        return aNull;
      }

      const BOPCol_ListOfShape* pLEIm = theImages.Seek(aE);
      if (!pLEIm)
      {
        aBB.Add(aNewWire, aE);
        continue;
      }

      // Check if the original edge is degenerated
      Standard_Boolean bIsDegenerated = BRep_Tool::Degenerated(aE);
      // Check if the original edge is closed on the face
      Standard_Boolean bIsClosed = BRep_Tool::IsClosed(aE, theFace);

      BOPCol_ListIteratorOfListOfShape aItLEIm(*pLEIm);
      for (; aItLEIm.More(); aItLEIm.Next())
      {
        TopoDS_Edge& aSp = TopoDS::Edge(aItLEIm.Value());

        aSp.Orientation(anOriE);
        if (bIsDegenerated)
        {
          aBB.Add(aNewWire, aSp);
          continue;
        }

        // Check closeness of the split edge and if it is not
        // make the second PCurve
        if (bIsClosed && !BRep_Tool::IsClosed(aSp, theFace))
          BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSp, theFace);

        // Check if the split should be reversed
        if (BOPTools_AlgoTools::IsSplitToReverse(aSp, aE, theCtx))
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
