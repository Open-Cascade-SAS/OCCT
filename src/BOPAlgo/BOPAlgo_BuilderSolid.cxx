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

#include <BOPAlgo_BuilderSolid.ixx>
//
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_UBTreeFiller.hxx>
//
#include <gp_Pnt2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
//
#include <TColStd_MapIntegerHasher.hxx>
//
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_Curve.hxx>
//
#include <TopAbs.hxx>
//
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
//
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
//
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
//
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfOrientedShape.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_DataMapOfShapeListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_BoxBndTree.hxx>
#include <BOPCol_ListOfInteger.hxx>
//
#include <BOPTools.hxx>
#include <BOPTools_CoupleOfShape.hxx>
#include <BOPTools_AlgoTools.hxx>
//
#include <BOPInt_Context.hxx>
//
#include <BOPAlgo_ShellSplitter.hxx>

static
  Standard_Boolean IsGrowthShell(const TopoDS_Shape& ,
                                 const BOPCol_IndexedMapOfShape& );
static
  Standard_Boolean IsHole(const TopoDS_Shape& ,
                          Handle(BOPInt_Context)& );
static
  Standard_Boolean IsInside(const TopoDS_Shape& ,
                            const TopoDS_Shape& ,
                            Handle(BOPInt_Context)& );
static
  void MakeInternalShells(const BOPCol_MapOfShape& ,
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
typedef NCollection_DataMap\
  <Standard_Integer, BOPAlgo_BuilderSolid_ShapeBox, TColStd_MapIntegerHasher> \
  BOPAlgo_DataMapOfIntegerBSSB; 
//
typedef BOPAlgo_DataMapOfIntegerBSSB::Iterator \
  BOPAlgo_DataMapIteratorOfDataMapOfIntegerBSSB; 
//
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
  myErrorStatus=0;
  //
  if (myContext.IsNull()) {
    myContext=new BOPInt_Context;
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
  //
  PerformShapesToAvoid();
  if (myErrorStatus) {
    return;
  }
  //
  PerformLoops();
  if (myErrorStatus) {
    return;
  }
  PerformAreas();
  if (myErrorStatus) {
    return;
  }
  PerformInternalShapes();
  if (myErrorStatus) {
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
        BOPTools::MapShapesAndAncestors(aF, TopAbs_EDGE, TopAbs_FACE, aMEF);
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
  Standard_Integer iErr;
  BOPCol_ListIteratorOfListOfShape aIt;
  TopoDS_Iterator aItS;
  BOPCol_MapIteratorOfMapOfOrientedShape aItM;
  BOPAlgo_ShellSplitter aSSp;
  // 
  myErrorStatus=0;
  myLoops.Clear();
  //
  // 1. Shells Usual
  aIt.Initialize (myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF=aIt.Value();
    if (!myShapesToAvoid.Contains(aF)) {
      aSSp.AddStartElement(aF);
    }
  }
  //
  aSSp.SetRunParallel(myRunParallel);
  aSSp.Perform();
  iErr=aSSp.ErrorStatus();
  if (iErr) {
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
  Standard_Integer aNbFA;
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
  aItM.Initialize(myShapesToAvoid);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aF=aItM.Key();
    aMP.Add(aF);
  }
  //
  // c. add all edges that are not processed to myShapesToAvoid
  aIt.Initialize (myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF=aIt.Value();
    if (!aMP.Contains(aF)) {
      myShapesToAvoid.Add(aF);
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
  aNbFA=myShapesToAvoid.Extent();
  //
  aItM.Initialize(myShapesToAvoid);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aFF=aItM.Key();
    BOPTools::MapShapesAndAncestors(aFF, 
				    TopAbs_EDGE, TopAbs_FACE, 
				    aEFMap);
  }
  //
  aItM.Initialize(myShapesToAvoid);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aFF=aItM.Key();
    if (!AddedFacesMap.Add(aFF)) {
      continue;
    }
    //
    // make a new shell
    TopoDS_Shell aShell;
    aBB.MakeShell(aShell);
    aBB.Add(aShell, aFF);
    //
    TopoDS_Iterator aItAddedF (aShell);
    for (; aItAddedF.More(); aItAddedF.Next()) {
      const TopoDS_Face& aF = (*(TopoDS_Face*)(&aItAddedF.Value()));
      //
      TopExp_Explorer aEdgeExp(aF, TopAbs_EDGE);
      for (; aEdgeExp.More(); aEdgeExp.Next()) {
        const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aEdgeExp.Current()));
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
  Standard_Integer k,aNbHoles;
  BRep_Builder aBB; 
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPCol_ListOfShape aNewSolids, aHoleShells; 
  BOPCol_DataMapOfShapeShape aInOutMap;
  BOPCol_IndexedMapOfShape aMHF;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  BOPCol_BoxBndTreeSelector aSelector;
  BOPCol_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  BOPAlgo_DataMapOfIntegerBSSB aDMISB(100);
  BOPAlgo_DataMapIteratorOfDataMapOfIntegerBSSB aItDMISB;
  BOPCol_DataMapOfShapeListOfShape aMSH;
  BOPCol_DataMapIteratorOfDataMapOfShapeShape aItDMSS;
  BOPCol_DataMapIteratorOfDataMapOfShapeListOfShape aItMSH;
  //
  myErrorStatus=0;
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
    aNbHoles=aBBTree.Select(aSelector);
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
      if (aInOutMap.IsBound (aHole)){
	const TopoDS_Shape& aHole2=aInOutMap(aHole);
	if (IsInside(aHole, aHole2, myContext)) {
          aInOutMap.UnBind(aHole);
          aInOutMap.Bind (aHole, aSolid);
        }
      }
      else{
        aInOutMap.Bind(aHole, aSolid);
      }
    }
  }//for (; aItDMISB.More(); aItDMISB.Next()) {
  //
  // 5. Map [Solid/Holes] -> aMSH 
  aItDMSS.Initialize(aInOutMap);
  for (; aItDMSS.More(); aItDMSS.Next()) {
    const TopoDS_Shape& aHole=aItDMSS.Key();
    const TopoDS_Shape& aSolid=aItDMSS.Value();
    //
    if (aMSH.IsBound(aSolid)) {
      BOPCol_ListOfShape& aLH=aMSH.ChangeFind(aSolid);
      aLH.Append(aHole);
    }
    else {
      BOPCol_ListOfShape aLH;
      aLH.Append(aHole);
      aMSH.Bind(aSolid, aLH);
    }
  }
  //
  // 6. Add aHoles to Solids
  aItMSH.Initialize(aMSH);
  for (; aItMSH.More(); aItMSH.Next()) {
    TopoDS_Solid aSolid=(*(TopoDS_Solid*)(&aItMSH.Key()));
    //
    const BOPCol_ListOfShape& aLH=aItMSH.Value();
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
    if (!aInOutMap.IsBound(aHole)){
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
  myErrorStatus=0;
  //
  Standard_Integer aNbFI=myLoopsInternal.Extent();
  if (!aNbFI) {// nothing to do
    return;
  }
  // 
  BRep_Builder aBB;
  TopoDS_Iterator aIt;
  BOPCol_ListIteratorOfListOfShape aShellIt, aSolidIt;
  BOPCol_MapIteratorOfMapOfShape aItMF;
  //
  BOPCol_MapOfShape aMF, aMFP, aMFx;
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF;
  BOPCol_ListOfShape aLSI;
  //
  // 1. All internal faces
  aShellIt.Initialize(myLoopsInternal);
  for (; aShellIt.More(); aShellIt.Next()) {
    const TopoDS_Shape& aShell=aShellIt.Value();
    aIt.Initialize(aShell);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aF=aIt.Value();
      aMF.Add(aF);
    }
  }
  aNbFI=aMF.Extent();
  //
  // 2 Process solids
  aSolidIt.Initialize(myAreas);
  for ( ; aSolidIt.More(); aSolidIt.Next()) {
    TopoDS_Solid& aSolid=(*(TopoDS_Solid*)(&aSolidIt.Value()));
    //
    TopExp_Explorer anExpSol(aSolid, TopAbs_FACE);;
    for (; anExpSol.More(); anExpSol.Next()) {
      const TopoDS_Shape& aF = anExpSol.Current();
      TopoDS_Shape aFF=aF;
      //
      aFF.Orientation(TopAbs_FORWARD);
      aMFx.Add(aFF);
      aFF.Orientation(TopAbs_REVERSED);
      aMFx.Add(aFF);
    }
    aMEF.Clear();
    BOPTools::MapShapesAndAncestors(aSolid, 
				    TopAbs_EDGE, TopAbs_FACE, 
				    aMEF);
    //
    // 2.1 Separate faces to process aMFP
    aMFP.Clear();
    aItMF.Initialize(aMF);
    for (; aItMF.More(); aItMF.Next()) {
      const TopoDS_Face& aF=(*(TopoDS_Face*)(&aItMF.Key()));
      if (!aMFx.Contains(aF)) {
        if (BOPTools_AlgoTools::IsInternalFace(aF, 
					       aSolid, 
					       aMEF, 
					       1.e-14, 
					       myContext)) {
          aMFP.Add(aF);
        }
      }
    }
    aMFx.Clear();
    //
    // 2.2 Make Internal Shells
    aLSI.Clear();
    MakeInternalShells(aMFP, aLSI);
    //
    // 2.3 Add them to aSolid
    aShellIt.Initialize(aLSI);
    for (; aShellIt.More(); aShellIt.Next()) {
      const TopoDS_Shape& aSI=aShellIt.Value();
      aBB.Add (aSolid, aSI);
    }
    //
    // 2.4 Remove faces aMFP from aMF
    aItMF.Initialize(aMFP);
    for (; aItMF.More(); aItMF.Next()) {
      const TopoDS_Shape& aF=aItMF.Key();
      aMF.Remove(aF);
    }
    //
    aNbFI=aMF.Extent();
    if (!aNbFI) {
      break;
    }
  } //for ( ; aSolidIt.More(); aSolidIt.Next()) {
  if (aNbFI) {
    TopoDS_Solid aSolid;
    aBB.MakeSolid(aSolid);
    //
    MakeInternalShells(aMF, aLSI);
    aShellIt.Initialize(aLSI);
    for (; aShellIt.More(); aShellIt.Next()) {
      const TopoDS_Shape& aSI=aShellIt.Value();
      aBB.Add (aSolid, aSI);
    }
    myAreas.Append(aSolid);
  }
}

//=======================================================================
//function : MakeInternalShells
//purpose  : 
//=======================================================================
void MakeInternalShells(const BOPCol_MapOfShape& theMF,
                        BOPCol_ListOfShape& theShells)
{
  BOPCol_ListIteratorOfListOfShape aItF;
  BRep_Builder aBB;
  //
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF;
  BOPCol_MapIteratorOfMapOfShape aItM;
  BOPCol_MapOfShape aAddedFacesMap;
  //
  aItM.Initialize(theMF);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aF=aItM.Key();
    BOPTools::MapShapesAndAncestors(aF, 
				    TopAbs_EDGE, TopAbs_FACE, 
				    aMEF);
  }
  //
  aItM.Initialize(theMF);
  for (; aItM.More(); aItM.Next()) {
    TopoDS_Shape aFF=aItM.Key();
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
    theShells.Append(aShell);
  }
}
//=======================================================================
//function : IsHole
//purpose  : 
//=======================================================================
Standard_Boolean IsHole(const TopoDS_Shape& theS2,
                        Handle(BOPInt_Context)& theContext)
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
                          Handle(BOPInt_Context)& theContext)
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
    aState=BOPTools_AlgoTools::ComputeState(aF, *pS2, 1.e-14, 
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
