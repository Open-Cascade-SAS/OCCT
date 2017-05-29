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
//
#include <BOPAlgo_BuilderSolid.hxx>
#include <BOPAlgo_ShellSplitter.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPCol_BoxBndTree.hxx>
#include <BOPCol_DataMapOfShapeListOfShape.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeShape.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfOrientedShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_CoupleOfShape.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <TColStd_MapIntegerHasher.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>

//
static
  Standard_Boolean IsGrowthShell(const TopoDS_Shape& ,
                                 const BOPCol_IndexedMapOfShape& );
static
  Standard_Boolean IsHole(const TopoDS_Shape& ,
                          Handle(IntTools_Context)& );
static
  Standard_Boolean IsInside(const TopoDS_Shape& ,
                            const TopoDS_Shape& ,
                            Handle(IntTools_Context)& );
static
  void MakeInternalShells(const BOPCol_IndexedMapOfShape& ,
                          BOPCol_ListOfShape& );

//=======================================================================
//class     : BOPAlgo_BuilderSolid_ShapeBox
//purpose   : Auxiliary class
//=======================================================================
class BOPAlgo_BuilderSolid_ShapeBox {
 public:
  BOPAlgo_BuilderSolid_ShapeBox() {
    myIsHole=Standard_False;
  };
  //
  ~BOPAlgo_BuilderSolid_ShapeBox() {
  };
  //
  void SetShape(const TopoDS_Shape& aS) {
    myShape=aS;
  };
  //
  const TopoDS_Shape& Shape()const {
    return myShape;
  };
  //
  void SetBox(const Bnd_Box& aBox) {
    myBox=aBox;
  };
  //
  const Bnd_Box& Box()const {
    return myBox;
  };
  //
  void SetIsHole(const Standard_Boolean bFlag) {
    myIsHole=bFlag;
  };
  //
  Standard_Boolean IsHole()const {
    return myIsHole;
  };
  //
 protected:
  Standard_Boolean myIsHole;
  TopoDS_Shape myShape;
  Bnd_Box myBox;
};
//
typedef NCollection_DataMap
  <Standard_Integer, 
  BOPAlgo_BuilderSolid_ShapeBox, 
  TColStd_MapIntegerHasher> BOPAlgo_DataMapOfIntegerBSSB; 
//
typedef BOPAlgo_DataMapOfIntegerBSSB::Iterator 
  BOPAlgo_DataMapIteratorOfDataMapOfIntegerBSSB; 
//
//=======================================================================
//function : BOPAlgo_FacePnt
//purpose  : 
//=======================================================================
class BOPAlgo_FacePnt {
 public:
  BOPAlgo_FacePnt() {
  }
  //
  virtual ~BOPAlgo_FacePnt() {
  }
  //
  void SetFace(const TopoDS_Face& aFace) {
    myFace=aFace;
  }
  //
  const TopoDS_Face& Face()const {
    return myFace;
  }
  // 
  void SetPnt(const gp_Pnt& aPnt) {
    myPnt=aPnt;
  }
  //
  const gp_Pnt& Pnt()const {
    return myPnt;
  }
  //
 protected:
  gp_Pnt myPnt;
  TopoDS_Face myFace;
};
//
typedef BOPCol_NCVector
  <BOPAlgo_FacePnt> BOPAlgo_VectorOfFacePnt; 
//
//=======================================================================
//function : BOPAlgo_FaceSolid
//purpose  : 
//=======================================================================
class BOPAlgo_FaceSolid : public BOPAlgo_Algo {
 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_FaceSolid() :
    myIsInternalFace(Standard_False) {
  }
  //
  virtual ~BOPAlgo_FaceSolid() {
  }
  //
  void SetFace(const TopoDS_Face& aFace) {
    myFace=aFace;
  }
  //
  const TopoDS_Face& Face()const {
    return myFace;
  }
  //
  void SetSolid(const TopoDS_Solid& aSolid) {
    mySolid=aSolid;
  }
  //
  const TopoDS_Solid& Solid()const {
    return mySolid;
  }
  //
  void SetPnt(const gp_Pnt& aPnt) {
    myPnt=aPnt;
  }
  //
  const gp_Pnt& Pnt()const {
    return myPnt;
  }
  void SetContext(const Handle(IntTools_Context)& aContext) {
    myContext=aContext;
  }
  //
  const Handle(IntTools_Context)& Context()const {
    return myContext;
  }
  //
  Standard_Boolean IsInternalFace() const {
    return myIsInternalFace;
  }
  //
  virtual void Perform () {
    TopAbs_State aState;
    //
    BOPAlgo_Algo::UserBreak();
    //
    aState=BOPTools_AlgoTools::ComputeState(myPnt, mySolid,
                                            Precision::Confusion(),
                                            myContext);
    //
    myIsInternalFace=(aState==TopAbs_IN);
  }
  //
 protected:
  Standard_Boolean myIsInternalFace;
  gp_Pnt myPnt;
  TopoDS_Face myFace;
  TopoDS_Solid mySolid;
  Handle(IntTools_Context) myContext;
};
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_FaceSolid> BOPAlgo_VectorOfFaceSolid; 
//
typedef BOPCol_ContextFunctor 
  <BOPAlgo_FaceSolid,
  BOPAlgo_VectorOfFaceSolid,
  Handle(IntTools_Context), 
  IntTools_Context> BOPAlgo_FaceSolidFunctor;
//
typedef BOPCol_ContextCnt 
  <BOPAlgo_FaceSolidFunctor,
  BOPAlgo_VectorOfFaceSolid,
  Handle(IntTools_Context)> BOPAlgo_FaceSolidCnt;
//
//=======================================================================

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BuilderSolid::BOPAlgo_BuilderSolid()
:
  BOPAlgo_BuilderArea()
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BuilderSolid::BOPAlgo_BuilderSolid
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_BuilderArea(theAllocator)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_BuilderSolid::~BOPAlgo_BuilderSolid()
{
}
//=======================================================================
//function : SetSolid 
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::SetSolid(const TopoDS_Solid& aS)
{
  mySolid=aS;
}
//=======================================================================
//function : Solid 
//purpose  : 
//=======================================================================
const TopoDS_Solid& BOPAlgo_BuilderSolid::Solid()const
{
  return mySolid;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::Perform()
{
  GetReport()->Clear();
  //
  if (myContext.IsNull()) {
    myContext=new IntTools_Context;
  }
  //
  TopoDS_Compound aC;
  BRep_Builder aBB;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  aBB.MakeCompound(aC);
  aIt.Initialize(myShapes);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF=aIt.Value();
    aBB.Add(aC, aF);
  }
  //
  UserBreak();
  //
  PerformShapesToAvoid();
  if (HasErrors()) {
    return;
  }
  //
  UserBreak();
  //
  PerformLoops();
  if (HasErrors()) {
    return;
  }
  //
  UserBreak();
  //
  PerformAreas();
  if (HasErrors()) {
    return;
  }
  //
  UserBreak();
  //
  PerformInternalShapes();
  if (HasErrors()) {
    return;
  }
}
//=======================================================================
//function :PerformShapesToAvoid
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::PerformShapesToAvoid()
{
  Standard_Boolean bFound;
  Standard_Integer i, iCnt, aNbE, aNbF;
  TopAbs_Orientation aOrE;
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  myShapesToAvoid.Clear();
  //
  iCnt=0;
  for(;;) {
    ++iCnt;
    bFound=Standard_False;
    //
    // 1. MEF
    aMEF.Clear();
    aIt.Initialize (myShapes);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aF=aIt.Value();
      if (!myShapesToAvoid.Contains(aF)) {
        BOPTools::MapShapesAndAncestors(aF, 
                                        TopAbs_EDGE, 
                                        TopAbs_FACE, 
                                        aMEF);
      }
    }
    aNbE=aMEF.Extent();
    //
    // 2. myFacesToAvoid
    for (i=1; i<=aNbE; ++i) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aMEF.FindKey(i)));
      if (BRep_Tool::Degenerated(aE)) {
        continue;
      }
      //
      BOPCol_ListOfShape& aLF=aMEF.ChangeFromKey(aE);
      aNbF=aLF.Extent();
      if (!aNbF) {
        continue;
      }
      //
      aOrE=aE.Orientation();
      //
      const TopoDS_Face& aF1=(*(TopoDS_Face*)(&aLF.First()));
      if (aNbF==1) {
        if (aOrE==TopAbs_INTERNAL) {
          continue;
        }
        bFound=Standard_True;
        myShapesToAvoid.Add(aF1);
      }
      else if (aNbF==2) {
        const TopoDS_Face& aF2=(*(TopoDS_Face*)(&aLF.Last()));
        if (aF2.IsSame(aF1)) {
          if (BRep_Tool::IsClosed(aE, aF1)) {
            continue;
          }
          //
          if (aOrE==TopAbs_INTERNAL) {
            continue;
          }
          //
          bFound=Standard_True;
          myShapesToAvoid.Add(aF1);
          myShapesToAvoid.Add(aF2);
        }
      }
    }// for (i=1; i<=aNbE; ++i) {
    //
    if (!bFound) {
      break;
    }
    //
  }//for(;;) {
}  
//=======================================================================
//function : PerformLoops
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::PerformLoops()
{
  Standard_Integer i, aNbSh;
  BOPCol_ListIteratorOfListOfShape aIt;
  TopoDS_Iterator aItS;
  Handle(NCollection_BaseAllocator) aAlr;
  // 
  myLoops.Clear();
  //
  aAlr=
    NCollection_BaseAllocator::CommonBaseAllocator();
  BOPAlgo_ShellSplitter aSSp(aAlr);
  //
  // 1. Shells Usual
  aIt.Initialize (myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Face& aF=*((TopoDS_Face*)&aIt.Value());
    if (myContext->IsInfiniteFace(aF)) {
      TopoDS_Shell aSh;
      BRep_Builder aBB;
      //
      aBB.MakeShell(aSh);
      aBB.Add(aSh, aF);
      myLoops.Append(aSh);
      continue;
    }
    //
    if (!myShapesToAvoid.Contains(aF)) {
      aSSp.AddStartElement(aF);
    }
  }
  //
  aSSp.SetRunParallel(myRunParallel);
  aSSp.Perform();
  if (aSSp.HasErrors()) {
    // add warning status
    {
      TopoDS_Compound aFacesSp;
      BRep_Builder().MakeCompound(aFacesSp);
      BOPCol_ListIteratorOfListOfShape aItLF(aSSp.StartElements());
      for (; aItLF.More(); aItLF.Next()) {
        BRep_Builder().Add(aFacesSp, aItLF.Value());
      }
      AddWarning (new BOPAlgo_AlertShellSplitterFailed (aFacesSp));
    }
    return;
  }
  //
  const BOPCol_ListOfShape& aLSh=aSSp.Shells();
  aIt.Initialize (aLSh);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSh=aIt.Value();
    myLoops.Append(aSh);
  }
  //=================================================
  //
  // 2. Post Treatment
  BRep_Builder aBB;
  BOPCol_MapOfOrientedShape AddedFacesMap;
  BOPCol_IndexedDataMapOfShapeListOfShape aEFMap;
  BOPCol_MapOfOrientedShape aMP;
  //
  // a. collect all edges that are in loops
  aIt.Initialize (myLoops);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aItS.Initialize(aS);
    for (; aItS.More(); aItS.Next()) {
      const TopoDS_Shape& aF=aItS.Value();
      aMP.Add(aF);
    }
  }
  // 
  // b. collect all edges that are to avoid
  aNbSh = myShapesToAvoid.Extent();
  for (i = 1; i <= aNbSh; ++i) {
    const TopoDS_Shape& aF = myShapesToAvoid(i);
    aMP.Add(aF);
  }
  //
  // c. add all edges that are not processed to myShapesToAvoid
  aIt.Initialize (myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Face& aF=*((TopoDS_Face*)&aIt.Value());
    if (!myContext->IsInfiniteFace(aF)) {
      if (!aMP.Contains(aF)) {
        myShapesToAvoid.Add(aF);
      }
    }
  }
  //=================================================
  //
  // 3.Internal Shells
  myLoopsInternal.Clear();
  //
  aEFMap.Clear();
  AddedFacesMap.Clear();
  //
  aNbSh = myShapesToAvoid.Extent();
  for (i = 1; i <= aNbSh; ++i) {
    const TopoDS_Shape& aFF = myShapesToAvoid(i);
    BOPTools::MapShapesAndAncestors(aFF, 
        TopAbs_EDGE, TopAbs_FACE, 
        aEFMap);
  }
  //
  for (i = 1; i <= aNbSh; ++i) {
    const TopoDS_Shape& aFF = myShapesToAvoid(i);
    if (!AddedFacesMap.Add(aFF)) {
      continue;
    }
    //
    // make a new shell
    TopExp_Explorer aExp;
    TopoDS_Shell aShell;
    aBB.MakeShell(aShell);
    aBB.Add(aShell, aFF);
    //
    aItS.Initialize(aShell);
    for (; aItS.More(); aItS.Next()) {
      const TopoDS_Face& aF = (*(TopoDS_Face*)(&aItS.Value()));
      //
      aExp.Init(aF, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aExp.Current()));
        const BOPCol_ListOfShape& aLF=aEFMap.FindFromKey(aE);
        aIt.Initialize(aLF);
        for (; aIt.More(); aIt.Next()) { 
          const TopoDS_Face& aFL=(*(TopoDS_Face*)(&aIt.Value()));
          if (AddedFacesMap.Add(aFL)){
            aBB.Add(aShell, aFL);
          }
        }
      }
    }
    aShell.Closed (BRep_Tool::IsClosed (aShell));
    myLoopsInternal.Append(aShell);
  }
}
//=======================================================================
//function : PerformAreas
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::PerformAreas()
{
  Standard_Boolean bIsGrowth, bIsHole;
  Standard_Integer i, k, aNbInOut, aNbMSH;
  BRep_Builder aBB; 
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPCol_ListOfShape aNewSolids, aHoleShells; 
  BOPCol_IndexedDataMapOfShapeShape aInOutMap;
  BOPCol_IndexedMapOfShape aMHF;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  BOPCol_BoxBndTreeSelector aSelector;
  BOPCol_BoxBndTree aBBTree;
  NCollection_UBTreeFiller 
    <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  BOPAlgo_DataMapOfIntegerBSSB aDMISB(100);
  BOPCol_IndexedDataMapOfShapeListOfShape aMSH;
  BOPAlgo_DataMapIteratorOfDataMapOfIntegerBSSB aItDMISB;
  //
  myAreas.Clear();
  //
  //  Draft solids [aNewSolids]
  aItLS.Initialize(myLoops);
  for (k=0; aItLS.More(); aItLS.Next(), ++k) {
    TopoDS_Solid aSolid;
    Bnd_Box aBox;
    BOPAlgo_BuilderSolid_ShapeBox aSB;
    //
    const TopoDS_Shape& aShell = aItLS.Value();
    aSB.SetShape(aShell);
    //
    BRepBndLib::Add(aShell, aBox);
    bIsHole=Standard_False;
    //
    bIsGrowth=IsGrowthShell(aShell, aMHF);
    if (bIsGrowth) {
      // make a growth solid from a shell
      aBB.MakeSolid(aSolid);
      aBB.Add (aSolid, aShell);
      //
      aNewSolids.Append (aSolid);
      aSB.SetShape(aSolid);
    }
    else{
      // check if a shell is a hole
      bIsHole=IsHole(aShell, myContext);
      if (bIsHole) {
        aHoleShells.Append(aShell);
        BOPTools::MapShapes(aShell, TopAbs_FACE, aMHF);
        aSB.SetShape(aShell);
      }
      else {
        // make a growth solid from a shell
        aBB.MakeSolid(aSolid);
        aBB.Add (aSolid, aShell);
        //
        aNewSolids.Append (aSolid);
        aSB.SetShape(aSolid);
      }
    }
    //
    aSB.SetBox(aBox);
    aSB.SetIsHole(bIsHole);
    aDMISB.Bind(k, aSB);
  }  
  //
  // 2. Prepare TreeFiller
  aItDMISB.Initialize(aDMISB);
  for (; aItDMISB.More(); aItDMISB.Next()) {
    k=aItDMISB.Key();
    const BOPAlgo_BuilderSolid_ShapeBox& aSB=aItDMISB.Value();
    //
    bIsHole=aSB.IsHole();
    if (bIsHole) {
      const Bnd_Box& aBox=aSB.Box();
      aTreeFiller.Add(k, aBox);
    }
  }
  //
  // 3. Shake TreeFiller
  aTreeFiller.Fill();
  //
  // 4. Find outer growth shell that is most close 
  //    to each hole shell
  aItDMISB.Initialize(aDMISB);
  for (; aItDMISB.More(); aItDMISB.Next()) {
    k=aItDMISB.Key();
    const BOPAlgo_BuilderSolid_ShapeBox& aSB=aItDMISB.Value();
    bIsHole=aSB.IsHole();
    if (bIsHole) {
      continue;
    }
    //
    const TopoDS_Shape aSolid=aSB.Shape();
    const Bnd_Box& aBoxSolid=aSB.Box();
    //
    aSelector.Clear();
    aSelector.SetBox(aBoxSolid);
    //
    aBBTree.Select(aSelector);
    //
    const BOPCol_ListOfInteger& aLI=aSelector.Indices();
    //
    aItLI.Initialize(aLI);
    for (; aItLI.More(); aItLI.Next()) {
      k=aItLI.Value();
      const BOPAlgo_BuilderSolid_ShapeBox& aSBk=aDMISB.Find(k);
      const TopoDS_Shape& aHole=aSBk.Shape();
      //
      if (!IsInside(aHole, aSolid, myContext)){
        continue;
      }
      //
      if (aInOutMap.Contains (aHole)){
        const TopoDS_Shape& aSolidWas = aInOutMap.FindFromKey(aHole);
        if (IsInside(aSolid, aSolidWas, myContext)) {
          aInOutMap.ChangeFromKey(aHole) = aSolid;
        }
      }
      else{
        aInOutMap.Add(aHole, aSolid);
      }
    }
  }//for (i = 1; i <= aNbDMISB; ++i) {
  //
  // 5. Map [Solid/Holes] -> aMSH 
  aNbInOut = aInOutMap.Extent();
  for (i = 1; i <= aNbInOut; ++i) {
    const TopoDS_Shape& aHole = aInOutMap.FindKey(i);
    const TopoDS_Shape& aSolid = aInOutMap(i);
    //
    if (aMSH.Contains(aSolid)) {
      BOPCol_ListOfShape& aLH = aMSH.ChangeFromKey(aSolid);
      aLH.Append(aHole);
    }
    else {
      BOPCol_ListOfShape aLH;
      aLH.Append(aHole);
      aMSH.Add(aSolid, aLH);
    }
  }
  //
  // 6. Add aHoles to Solids
  aNbMSH = aMSH.Extent();
  for (i = 1; i <= aNbMSH; ++i) {
    TopoDS_Solid aSolid=(*(TopoDS_Solid*)(&(aMSH.FindKey(i))));
    const BOPCol_ListOfShape& aLH = aMSH(i);
    //
    aItLS.Initialize(aLH);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aHole = aItLS.Value();
      aBB.Add (aSolid, aHole);
    }
    //
    // update classifier
    BRepClass3d_SolidClassifier& aSC=
      myContext->SolidClassifier(aSolid);
    aSC.Load(aSolid);
    //
  }
  //
  // 7. These aNewSolids are draft solids that 
  // do not contain any internal shapes
  aItLS.Initialize(aNewSolids);
  for ( ; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aSx=aItLS.Value();
    myAreas.Append(aSx);
  }
  // Add holes that outside the solids to myAreas
  aItLS.Initialize(aHoleShells);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aHole = aItLS.Value();
    if (!aInOutMap.Contains(aHole)){
      TopoDS_Solid aSolid;
      //
      aBB.MakeSolid(aSolid);
      aBB.Add (aSolid, aHole);
      //
      myAreas.Append(aSolid);
    }
  }
}
//=======================================================================
//function : PerformInternalShapes
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::PerformInternalShapes()
{
  if (myAvoidInternalShapes) {
    return;
  }
  //
  Standard_Integer aNbFI=myLoopsInternal.Extent();
  if (!aNbFI) {// nothing to do
    return;
  }
  // 
  Standard_Boolean bIsInternalFace;
  Standard_Integer k, aNbVFS, aNbSLF, aNbVFP, aNbA;
  BRep_Builder aBB;
  TopoDS_Iterator aIt;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape  aItLS;
  BOPCol_IndexedMapOfShape aMFs;
  BOPCol_ListOfShape aLSI;
  BOPAlgo_VectorOfFaceSolid aVFS;
  BOPAlgo_VectorOfFacePnt aVFP;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  BOPCol_BoxBndTreeSelector aSelector;
  BOPCol_BoxBndTree aBBTree;
  NCollection_UBTreeFiller 
    <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  aNbA=myAreas.Extent();
  //
  // 1. aVFP
  aItLS.Initialize(myLoopsInternal);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aShell=aItLS.Value();
    aIt.Initialize(aShell);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Face& aF=*((TopoDS_Face*)&aIt.Value());
      //
      if (!aMFs.Contains(aF)) {
        aMFs.Add(aF);
        //
        gp_Pnt aP;
        gp_Pnt2d aP2D;
        //
        if (aNbA) {
          BOPTools_AlgoTools3D::PointInFace(aF, aP, aP2D, myContext);
        }
        //
        BOPAlgo_FacePnt& aFP=aVFP.Append1();
        aFP.SetFace(aF);
        aFP.SetPnt(aP);
      }
    }
  }
  //
  if (!aNbA) {
    // 7b. "Rest" faces treatment
    TopoDS_Solid aSolid;
    aBB.MakeSolid(aSolid);
    //
    MakeInternalShells(aMFs, aLSI);
    //
    aItLS.Initialize(aLSI);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aSI=aItLS.Value();
       aBB.Add (aSolid, aSI);
    }
    myAreas.Append(aSolid);
    //
    return; // =>
  }//if (!aNbA) {
  //
  // 2. Prepare TreeFiller 
  aNbVFP=aVFP.Extent();
  for(k=0; k<aNbVFP; ++k) {
    Bnd_Box aBox;
    //
    const BOPAlgo_FacePnt& aFP=aVFP(k);
    const TopoDS_Face& aF=aFP.Face();
    //
    BRepBndLib::Add(aF, aBox);
    aTreeFiller.Add(k, aBox);
  }
  //
  aTreeFiller.Fill();
  //
  // 3. Face/Solid candidates: aVFS
  aItLS.Initialize(myAreas);
  for (; aItLS.More(); aItLS.Next()) {
    Bnd_Box aBox;
    //
    TopoDS_Solid& aSolid=(*(TopoDS_Solid*)(&aItLS.Value()));
    BRepBndLib::Add(aSolid, aBox);
    //
    aMFs.Clear();
    aExp.Init(aSolid, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aFs=aExp.Current();
      aMFs.Add(aFs);
    }
    //
    aSelector.Clear();
    aSelector.SetBox(aBox);
    //
    aBBTree.Select(aSelector);
    //
    const BOPCol_ListOfInteger& aLI=aSelector.Indices();
    aItLI.Initialize(aLI);
    for (; aItLI.More(); aItLI.Next()) {
      k=aItLI.Value();
      const BOPAlgo_FacePnt& aFP=aVFP(k);
      const TopoDS_Face& aF=aFP.Face();
      if (aMFs.Contains(aF)) {
        continue;
      }
      //
      const gp_Pnt& aP=aFP.Pnt();
      //
      BOPAlgo_FaceSolid& aFS=aVFS.Append1();
      aFS.SetPnt(aP);
      aFS.SetFace(aF);
      aFS.SetSolid(aSolid);
    }
  }
  //
  aNbVFS=aVFS.Extent();
  if (!aNbVFS) {
    return;
  }
  // 4. Refine candidates
  //=============================================================
  BOPAlgo_FaceSolidCnt::Perform(myRunParallel, aVFS, myContext);
  //=============================================================
  // 
  // 5. Solid/Faces:  aMSLF
  BOPCol_IndexedDataMapOfShapeListOfShape aMSLF;
  BOPCol_MapOfShape aMFProcessed;
  //  
  for (k=0; k < aNbVFS; ++k) {
    const BOPAlgo_FaceSolid& aFS=aVFS(k);
    //
    const TopoDS_Solid& aSolid=aFS.Solid();
    const TopoDS_Face& aF=aFS.Face();
    //
    bIsInternalFace=aFS.IsInternalFace();
    if (!bIsInternalFace) {
      continue;
    }
    //
    if (aMSLF.Contains(aSolid)) {
      BOPCol_ListOfShape& aLF=aMSLF.ChangeFromKey(aSolid);
      aLF.Append(aF);
    }
    else {
      BOPCol_ListOfShape aLF;
      //
      aLF.Append(aF);
      aMSLF.Add(aSolid, aLF);
    }
  }// for (k=0; k < aNbVE; ++k) {
  //
  // 6. Update Solids by internal Faces
  aNbSLF=aMSLF.Extent();
  for (k=1; k <= aNbSLF; ++k) {
    const TopoDS_Shape& aSolid=aMSLF.FindKey(k);
    TopoDS_Shape *pSolid=(TopoDS_Shape*)&aSolid;
    //
    const BOPCol_ListOfShape& aLF=aMSLF(k);
    //
    aMFs.Clear();
    aItLS.Initialize(aLF);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aF=aItLS.Value();
      aMFs.Add(aF);
      aMFProcessed.Add(aF);
    }
    //
    aLSI.Clear();
    MakeInternalShells(aMFs, aLSI);
    //
    aItLS.Initialize(aLSI);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aSI=aItLS.Value();
      aBB.Add (*pSolid, aSI);
    }
  }
  // 
  // 7. "Rest" faces treatment (if there are)
  aMFs.Clear();
  for (k=0; k < aNbVFS; ++k) {
    const BOPAlgo_FaceSolid& aFS=aVFS(k);
    //
    const TopoDS_Face& aF=aFS.Face();
    if (!aMFProcessed.Contains(aF)) {
      aMFs.Add(aF);
    }
  }
  //
  aNbFI=aMFs.Extent();
  if (aNbFI) {
    TopoDS_Solid aSolid;
    aBB.MakeSolid(aSolid);
    //
    aLSI.Clear();
    MakeInternalShells(aMFs, aLSI);
    //
    aItLS.Initialize(aLSI);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aSI=aItLS.Value();
      aBB.Add (aSolid, aSI);
    }
    myAreas.Append(aSolid);
  }
}
//=======================================================================
//function : MakeInternalShells
//purpose  : 
//=======================================================================
void MakeInternalShells(const BOPCol_IndexedMapOfShape& theMF,
                        BOPCol_ListOfShape& theShells)
{
  Standard_Integer i, aNbF;
  BRep_Builder aBB;
  BOPCol_ListIteratorOfListOfShape aItF;
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF;
  BOPCol_MapOfShape aAddedFacesMap;
  //
  aNbF = theMF.Extent();
  for (i = 1; i <= aNbF; ++i) {
    TopoDS_Shape aF = theMF(i);
    BOPTools::MapShapesAndAncestors(aF, 
        TopAbs_EDGE, TopAbs_FACE, 
        aMEF);
  }
  //
  for (i = 1; i <= aNbF; ++i) {
    TopoDS_Shape aFF = theMF(i);
    if (!aAddedFacesMap.Add(aFF)) {
      continue;
    }
    //
    // make a new shell
    TopoDS_Shell aShell;
    aBB.MakeShell(aShell);    
    aFF.Orientation(TopAbs_INTERNAL);
    aBB.Add(aShell, aFF);
    //
    TopoDS_Iterator aItAddedF (aShell);
    for (; aItAddedF.More(); aItAddedF.Next()) {
      const TopoDS_Shape& aF =aItAddedF.Value();
      //
      TopExp_Explorer aEdgeExp(aF, TopAbs_EDGE);
      for (; aEdgeExp.More(); aEdgeExp.Next()) {
        const TopoDS_Shape& aE =aEdgeExp.Current();
        const BOPCol_ListOfShape& aLF=aMEF.FindFromKey(aE);
        aItF.Initialize(aLF);
        for (; aItF.More(); aItF.Next()) { 
          TopoDS_Shape aFL=aItF.Value();
          if (aAddedFacesMap.Add(aFL)){
            aFL.Orientation(TopAbs_INTERNAL);
            aBB.Add(aShell, aFL);
          }
        }
      }
    }
    aShell.Closed (BRep_Tool::IsClosed (aShell));
    theShells.Append(aShell);
  }
}
//=======================================================================
//function : IsHole
//purpose  : 
//=======================================================================
Standard_Boolean IsHole(const TopoDS_Shape& theS2,
                        Handle(IntTools_Context)& theContext)
{
  TopoDS_Solid *pS2=(TopoDS_Solid *)&theS2;
  BRepClass3d_SolidClassifier& aClsf=theContext->SolidClassifier(*pS2);
  //
  aClsf.PerformInfinitePoint(::RealSmall());
  //
  return (aClsf.State()==TopAbs_IN);
}
//=======================================================================
//function : IsInside
//purpose  : 
//=======================================================================
Standard_Boolean IsInside(const TopoDS_Shape& theS1,
                          const TopoDS_Shape& theS2,
                          Handle(IntTools_Context)& theContext)
{
  TopExp_Explorer aExp;
  TopAbs_State aState;
  //
  TopoDS_Solid *pS2=(TopoDS_Solid *)&theS2;
  //
  aExp.Init(theS1, TopAbs_FACE);
  if (!aExp.More()){
    BRepClass3d_SolidClassifier& aClsf=theContext->SolidClassifier(*pS2);
    aClsf.PerformInfinitePoint(::RealSmall());
    aState=aClsf.State();
  }
  else {
    BOPCol_IndexedMapOfShape aBounds;
    BOPTools::MapShapes(*pS2, TopAbs_EDGE, aBounds);
    const TopoDS_Face& aF = (*(TopoDS_Face*)(&aExp.Current()));
    aState=BOPTools_AlgoTools::ComputeState(aF, *pS2,
                                            Precision::Confusion(),
                                            aBounds, theContext);
  }
  return (aState==TopAbs_IN);
}
//=======================================================================
//function : IsGrowthShell
//purpose  : 
//=======================================================================
Standard_Boolean IsGrowthShell(const TopoDS_Shape& theShell,
                               const BOPCol_IndexedMapOfShape& theMHF)
{
  Standard_Boolean bRet;
  TopoDS_Iterator aIt;
  // 
  bRet=Standard_False;
  if (theMHF.Extent()) {
    aIt.Initialize(theShell);
    for(; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aF=aIt.Value();
      if (theMHF.Contains(aF)) {
        return !bRet;
      }
    }
  }
  return bRet;
}
