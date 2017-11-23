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
#include <BOPDS_Curve.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_SubIterator.hxx>
#include <BOPDS_VectorOfInterfVF.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_Vector.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//class    : BOPAlgo_VertexFace
//purpose  : 
//=======================================================================
class BOPAlgo_VertexFace : public BOPAlgo_Algo {
 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_VertexFace() : 
    BOPAlgo_Algo(),
    myIV(-1), myIF(-1),
    myFlag(-1), myT1(-1.),  myT2(-1.), myTolVNew(-1.) {
  }
  //
  virtual ~BOPAlgo_VertexFace(){
  }
  //
  void SetIndices(const Standard_Integer nV,
                  const Standard_Integer nF) {
    myIV=nV;
    myIF=nF;
  }
  //
  void Indices(Standard_Integer& nV,
               Standard_Integer& nF) const {
    nV=myIV;
    nF=myIF;
  }
  //
  void SetVertex(const TopoDS_Vertex& aV) {
    myV=aV;
  }
  //
  const TopoDS_Vertex& Vertex()const {
    return myV;
  }
  //
  void SetFace(const TopoDS_Face& aF) {
    myF=aF;
  }
  //
  const TopoDS_Face& Face()const {
    return myF;
  }
  //
  Standard_Integer Flag()const {
    return myFlag;
  }
  //
  void Parameters(Standard_Real& aT1,
                  Standard_Real& aT2)const {
    aT1=myT1;
    aT2=myT2;
  }
  //
  Standard_Real VertexNewTolerance()const {
    return myTolVNew;
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
    try
    {
      OCC_CATCH_SIGNALS

      myFlag=myContext->ComputeVF(myV, myF, myT1, myT2, myTolVNew, myFuzzyValue);
    }
    catch (Standard_Failure)
    {
      AddError(new BOPAlgo_AlertIntersectionFailed);
    }
  }
  //
 protected:
  Standard_Integer myIV;
  Standard_Integer myIF;
  Standard_Integer myFlag;
  Standard_Real myT1;
  Standard_Real myT2;
  Standard_Real myTolVNew;
  TopoDS_Vertex myV;
  TopoDS_Face myF;
  Handle(IntTools_Context) myContext;
};
//=======================================================================
typedef NCollection_Vector<BOPAlgo_VertexFace>
  BOPAlgo_VectorOfVertexFace; 
//
typedef BOPTools_ContextFunctor 
  <BOPAlgo_VertexFace,
  BOPAlgo_VectorOfVertexFace,
  Handle(IntTools_Context), 
  IntTools_Context> BOPAlgo_VertexFaceFunctor;
//
typedef BOPTools_ContextCnt 
  <BOPAlgo_VertexFaceFunctor,
  BOPAlgo_VectorOfVertexFace,
  Handle(IntTools_Context)> BOPAlgo_VertexFaceCnt;
//
//=======================================================================
// function: PerformVF
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformVF()
{
  myIterator->Initialize(TopAbs_VERTEX, TopAbs_FACE);
  Standard_Integer iSize = myIterator->ExpectedLength();
  //
  Standard_Integer nV, nF;
  //
  if (myGlue == BOPAlgo_GlueFull) {
    // there is no need to intersect vertices with faces in this mode
    // just initialize FaceInfo for all faces
    for (; myIterator->More(); myIterator->Next()) {
      myIterator->Value(nV, nF);
      if (!myDS->IsSubShape(nV, nF)) {
        myDS->ChangeFaceInfo(nF);
      }
    }
    return;
  }
  //
  BOPDS_VectorOfInterfVF& aVFs = myDS->InterfVF();
  if (!iSize) {
    iSize = 10;
    aVFs.SetIncrement(iSize);
    //
    TreatVerticesEE();
    return;
  }
  //
  Standard_Integer nVSD, iFlag, nVx, aNbVF, k;
  Standard_Real aT1, aT2;
  BOPAlgo_VectorOfVertexFace aVVF; 
  //
  aVFs.SetIncrement(iSize);
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nV, nF);
    //
    if (myDS->IsSubShape(nV, nF)) {
      continue;
    }
    //
    myDS->ChangeFaceInfo(nF);
    if (myDS->HasInterfShapeSubShapes(nV, nF)) {
      continue;
    }
    //
    nVx=nV;
    if (myDS->HasShapeSD(nV, nVSD)) {
      nVx=nVSD;
    }
    //
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nVx))); 
    const TopoDS_Face& aF=(*(TopoDS_Face *)(&myDS->Shape(nF))); 
    //
    BOPAlgo_VertexFace& aVertexFace=aVVF.Appended();
    //
    aVertexFace.SetIndices(nV, nF);
    aVertexFace.SetVertex(aV);
    aVertexFace.SetFace(aF);
    aVertexFace.SetFuzzyValue(myFuzzyValue);
    aVertexFace.SetProgressIndicator(myProgressIndicator);
  }//for (; myIterator->More(); myIterator->Next()) {
  //
  aNbVF=aVVF.Length();
  //================================================================
  BOPAlgo_VertexFaceCnt::Perform(myRunParallel, aVVF, myContext);
  //================================================================
  //
  for (k=0; k < aNbVF; ++k) {
    const BOPAlgo_VertexFace& aVertexFace=aVVF(k);
    // 
    iFlag=aVertexFace.Flag();
    if (iFlag != 0) {
      if (aVertexFace.HasErrors())
      {
        // Warn about failed intersection of sub-shapes
        AddIntersectionFailedWarning(aVertexFace.Vertex(), aVertexFace.Face());
      }
      continue;
    }
    //
    aVertexFace.Indices(nV, nF);
    aVertexFace.Parameters(aT1, aT2);
    // 1
    BOPDS_InterfVF& aVF=aVFs.Appended();
    aVF.SetIndices(nV, nF);
    aVF.SetUV(aT1, aT2);
    // 2
    myDS->AddInterf(nV, nF);
    //
    // 3 update vertex V/F if necessary
    Standard_Real aTolVNew = aVertexFace.VertexNewTolerance();
    nVx=UpdateVertex(nV, aTolVNew);
    //
    // 4
    if (myDS->IsNewShape(nVx)) {
      aVF.SetIndexNew(nVx);
    }
    // 5 update FaceInfo
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
    TColStd_MapOfInteger& aMVIn=aFI.ChangeVerticesIn();
    aMVIn.Add(nVx);
  }//for (k=0; k < aNbVF; ++k) {
  //
  TreatVerticesEE();
}
//=======================================================================
//function : TreatVerticesEE
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::TreatVerticesEE()
{
  Standard_Integer i, aNbS,aNbEEs, nF, nV, iFlag;
  Standard_Real aT1, aT2, dummy;
  TColStd_ListIteratorOfListOfInteger aItLI;
  Handle(NCollection_BaseAllocator) aAllocator;
  //
  aAllocator=
    NCollection_BaseAllocator::CommonBaseAllocator();
  TColStd_ListOfInteger aLIV(aAllocator), aLIF(aAllocator);
  TColStd_MapOfInteger aMI(100, aAllocator);
  BOPDS_MapOfPaveBlock aMPBF(100, aAllocator);
  //
  aNbS=myDS->NbSourceShapes();
  //
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  aNbEEs=aEEs.Length();
  for (i=0; i<aNbEEs; ++i) {
    BOPDS_InterfEE& aEE=aEEs(i);
    if (aEE.HasIndexNew()) {
      nV=aEE.IndexNew();
      if (aMI.Add(nV)) {
        aLIV.Append(nV);
      }   
    }   
  }
  if (!aLIV.Extent()) {
    aAllocator.Nullify();
    return;
  }
  //
  aNbS=myDS->NbSourceShapes();
  for (nF=0; nF<aNbS; ++nF) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(nF);
    if (aSI.ShapeType()==TopAbs_FACE) {
      aLIF.Append(nF);
    }
  }
  if (!aLIF.Extent()) {
    aAllocator.Nullify();
    return;
  }
  //-------------------------------------------------------------
  BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
  //
  BOPDS_SubIterator aIt(aAllocator);
  //
  aIt.SetDS(myDS);
  aIt.SetSubSet1(aLIF);
  aIt.SetSubSet2(aLIV);
  aIt.Prepare();
  aIt.Initialize();
  for (; aIt.More(); aIt.Next()) {
    aIt.Value(nV, nF);
    //
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
    const TColStd_MapOfInteger& aMVOn=aFI.VerticesOn();
    //
    if (!aMVOn.Contains(nV)) {
      const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nV))); 
      const TopoDS_Face& aF=(*(TopoDS_Face *)(&myDS->Shape(nF))); 
      iFlag = myContext->ComputeVF(aV, aF, aT1, aT2, dummy, myFuzzyValue);
      if (!iFlag) {
        // 1
        BOPDS_InterfVF& aVF=aVFs.Appended();
        i=aVFs.Length()-1;
        aVF.SetIndices(nV, nF);
        aVF.SetUV(aT1, aT2);
        // 2
        myDS->AddInterf(nV, nF);
        //
        TColStd_MapOfInteger& aMVIn=aFI.ChangeVerticesIn();
        aMVIn.Add(nV);
      }
    }
  }
}
