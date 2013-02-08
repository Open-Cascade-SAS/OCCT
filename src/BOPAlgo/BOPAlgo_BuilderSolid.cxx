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


#include <BOPAlgo_BuilderSolid.ixx>

#include <gp_Pnt2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_Curve.hxx>

#include <TopAbs.hxx>

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
#include <BRepTools.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
//
#include <BOPTools_AlgoTools.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPTools.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfOrientedShape.hxx>
//
#include <NCollection_List.hxx> 
//
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_DataMapOfShapeListOfShape.hxx>
#include <BOPInt_Context.hxx>
#include <BOPTools_CoupleOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>

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

static
  Standard_Boolean IsClosedShell(const TopoDS_Shell& theShell);

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
  BOPAlgo_BuilderSolid::BOPAlgo_BuilderSolid(const Handle(NCollection_BaseAllocator)& theAllocator)
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
//function : Perform
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderSolid::Perform()
{
  myErrorStatus=0;
  //
  if (myContext.IsNull()) {
    //myErrorStatus=11;// Null Context
    //return;
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
  while (1) {
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
      else {
        int a=0;
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
  }//while (1) 
}  
//=======================================================================
//function : PerformLoops
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderSolid::PerformLoops()
{
  myErrorStatus=0;
  //
  myLoops.Clear();
  //
  Standard_Integer aNbLF, aNbOff, aNbFP, aNbFA;
  Standard_Integer i;
  TopAbs_Orientation anOr;
  TopoDS_Edge aEL;
  BRep_Builder aBB;
  TopoDS_Iterator aItS;
  //
  BOPCol_ListIteratorOfListOfShape aItF, aIt;
  BOPCol_MapIteratorOfMapOfOrientedShape aItM;
  BOPTools_CoupleOfShape aCSOff;
  //
  BOPCol_MapOfOrientedShape AddedFacesMap;
  BOPCol_IndexedDataMapOfShapeListOfShape aEFMap, aMEFP;
  //
  //=================================================
  //
  // 1. Shells Usual
  //
  aItF.Initialize (myShapes);
  for (; aItF.More(); aItF.Next()) {
    const TopoDS_Shape& aFF = aItF.Value();
    BOPTools::MapShapesAndAncestors(aFF, TopAbs_EDGE, TopAbs_FACE, aEFMap);
  }
  //
  aItF.Initialize (myShapes);
  for (i=1; aItF.More(); aItF.Next(), ++i) {
    const TopoDS_Shape& aFF = aItF.Value();
    if (myShapesToAvoid.Contains(aFF)) {
      continue;
    }
    if (!AddedFacesMap.Add(aFF)) {
      continue;
    }
    //
    // make a new shell
    TopoDS_Shell aShell;
    aBB.MakeShell(aShell);
    aBB.Add(aShell, aFF);
    //
    aMEFP.Clear();
    BOPTools::MapShapesAndAncestors(aFF, TopAbs_EDGE, TopAbs_FACE, aMEFP);
    //
    // loop on faces added to Shell; add their neighbor faces to Shell and so on
    TopoDS_Iterator aItAddedF (aShell);
    for (; aItAddedF.More(); aItAddedF.Next()) {
      const TopoDS_Face& aF = (*(TopoDS_Face*)(&aItAddedF.Value()));
      //
      // loop on edges of aF; find a good neighbor face of aF by aE
      TopExp_Explorer aEdgeExp(aF, TopAbs_EDGE);
      for (; aEdgeExp.More(); aEdgeExp.Next()) {
        const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aEdgeExp.Current()));
        //
        //1
        if (aMEFP.Contains(aE)) {
          const BOPCol_ListOfShape& aLFP=aMEFP.FindFromKey(aE);
          aNbFP=aLFP.Extent();
          if (aNbFP>1) { 
            continue;
          }
        }
        //2
        anOr=aE.Orientation();
        if (anOr==TopAbs_INTERNAL) {
          continue;
        }
        //3
        if (BRep_Tool::Degenerated(aE)) {
          continue;
        }
        //
        // candidate faces list
        const BOPCol_ListOfShape& aLF=aEFMap.FindFromKey(aE);
        aNbLF=aLF.Extent();
        if (!aNbLF) {
          continue;
        }
        //
        // try to select one of neighbors
        // check if a face already added to Shell shares E
        Standard_Boolean bFound;
        BOPCol_ListIteratorOfListOfShape aItLF;
        BOPTools_ListOfCoupleOfShape aLCSOff;
        //
        aItLF.Initialize(aLF);
        for (; aItLF.More(); aItLF.Next()) { 
          const TopoDS_Face& aFL=(*(TopoDS_Face*)(&aItLF.Value()));
          if (myShapesToAvoid.Contains(aFL)) {
            continue;
          }
          if (aF.IsSame(aFL)) {
            continue;
          } 
          if (AddedFacesMap.Contains(aFL)){
            continue;
          }
          //
          bFound=BOPTools_AlgoTools::GetEdgeOff(aE, aFL, aEL);
          if (!bFound) {
            continue;
          }
          //
          aCSOff.SetShape1(aEL);
          aCSOff.SetShape2(aFL);
          aLCSOff.Append(aCSOff);
        }//for (; aItLF.More(); aItLF.Next()) { 
        //
        aNbOff=aLCSOff.Extent();
        if (!aNbOff){
          continue;
        }
        //
        TopoDS_Face aSelF;
        if (aNbOff==1) {
          aSelF=(*(TopoDS_Face*)(&aLCSOff.First().Shape2()));
        }
        else if (aNbOff>1){
          BOPTools_AlgoTools::GetFaceOff(aE, aF, aLCSOff, aSelF, myContext);
          }
        //
        if (!aSelF.IsNull() && AddedFacesMap.Add(aSelF)) { 
          aBB.Add(aShell, aSelF);
          BOPTools::MapShapesAndAncestors(aSelF, TopAbs_EDGE, TopAbs_FACE, aMEFP);
        }
      } // for (; aEdgeExp.More(); aEdgeExp.Next()) { 
    } //for (; aItAddedF.More(); aItAddedF.Next()) {
    //
    if (IsClosedShell(aShell)) {
      myLoops.Append(aShell);
    }
  } // for (; aItF.More(); aItF.Next()) {
  
  //
  // Post Treatment
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
  // 2.Internal Shells
  //
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
    BOPTools::MapShapesAndAncestors(aFF, TopAbs_EDGE, TopAbs_FACE, aEFMap);
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
        aItF.Initialize(aLF);
        for (; aItF.More(); aItF.Next()) { 
          const TopoDS_Face& aFL=(*(TopoDS_Face*)(&aItF.Value()));
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
  myErrorStatus=0;
  //
  Standard_Boolean bIsGrowthShell, bIsHole;
  BRep_Builder aBB; 
  TopoDS_Shape anInfinitePointShape;
  BOPCol_DataMapIteratorOfDataMapOfShapeListOfShape aItMSH;
  BOPCol_ListIteratorOfListOfShape aShellIt, aSolidIt;
  //
  BOPCol_ListOfShape aNewSolids, aHoleShells; 
  BOPCol_DataMapOfShapeShape aInOutMap;
  BOPCol_DataMapOfShapeListOfShape aMSH;
  BOPCol_IndexedMapOfShape aMHF;
  //
  myAreas.Clear();
  //
  //  Draft solids [aNewSolids]
  aShellIt.Initialize(myLoops);
  for ( ; aShellIt.More(); aShellIt.Next()) {
    const TopoDS_Shape& aShell = aShellIt.Value();
    //
    bIsGrowthShell=IsGrowthShell(aShell, aMHF);
    if (bIsGrowthShell) {
      // make a growth solid from a shell
      TopoDS_Solid Solid;
      aBB.MakeSolid(Solid);
      aBB.Add (Solid, aShell);
      //
      aNewSolids.Append (Solid);
    }
    else{
      // check if a shell is a hole
      //XX
      bIsHole=IsHole(aShell, myContext);
      //XX
      if (bIsHole) {
        aHoleShells.Append(aShell);
        BOPTools::MapShapes(aShell, TopAbs_FACE, aMHF);
      }
      else {
        // make a growth solid from a shell
        TopoDS_Solid Solid;
        aBB.MakeSolid(Solid);
        aBB.Add (Solid, aShell);
        //
        aNewSolids.Append (Solid);
      }
    }
  }
  //
  // 2. Find outer growth shell that is most close to each hole shell
  aShellIt.Initialize(aHoleShells);
  for (; aShellIt.More(); aShellIt.Next()) {
    const TopoDS_Shape& aHole = aShellIt.Value();
    //
    aSolidIt.Initialize(aNewSolids);
    for ( ; aSolidIt.More(); aSolidIt.Next())    {
      const TopoDS_Shape& aSolid = aSolidIt.Value();
      //
      if (!IsInside(aHole, aSolid, myContext)){
        continue;
      }
      //
      if ( aInOutMap.IsBound (aHole)){
        const TopoDS_Shape& aSolid2 = aInOutMap(aHole);
        if (IsInside(aSolid, aSolid2, myContext)) {
          aInOutMap.UnBind(aHole);
          aInOutMap.Bind (aHole, aSolid);
        }
      }
      else{
        aInOutMap.Bind (aHole, aSolid);
      }
    }
    //
    // Add aHole to a map Solid/ListOfHoles [aMSH]
    if (aInOutMap.IsBound(aHole)){
      const TopoDS_Shape& aSolid=aInOutMap(aHole);
      if (aMSH.IsBound(aSolid)) {
        BOPCol_ListOfShape& aLH=aMSH.ChangeFind(aSolid);
        aLH.Append(aHole);
      }
      else {
        BOPCol_ListOfShape aLH;
        aLH.Append(aHole);
        aMSH.Bind(aSolid, aLH);
      }
      //aBB.Add (aSolid, aHole);
    }
  }// for (; aShellIt.More(); aShellIt.Next()) {
  //
  // 3. Add aHoles to Solids
  aItMSH.Initialize(aMSH);
  for (; aItMSH.More(); aItMSH.Next()) {
    TopoDS_Solid aSolid=(*(TopoDS_Solid*)(&aItMSH.Key()));
    //
    const BOPCol_ListOfShape& aLH=aItMSH.Value();
    aShellIt.Initialize(aLH);
    for (; aShellIt.More(); aShellIt.Next()) {
      const TopoDS_Shape& aHole = aShellIt.Value();
      aBB.Add (aSolid, aHole);
    }
    //
    // update classifier
    BRepClass3d_SolidClassifier& aSC=myContext->SolidClassifier(aSolid);
    aSC.Load(aSolid);
    //
  }
  //
  // These aNewSolids are draft solids that 
  // do not contain any internal shapes
  //
  aShellIt.Initialize(aNewSolids);
  for ( ; aShellIt.More(); aShellIt.Next()) {
    const TopoDS_Shape& aSx = aShellIt.Value();
    myAreas.Append(aSx);
  }

  // Add holes that outside the solids to myAreas
  aShellIt.Initialize(aHoleShells);
  for (; aShellIt.More(); aShellIt.Next()) {
    const TopoDS_Shape& aHole = aShellIt.Value();
    if (!aInOutMap.IsBound(aHole)){
      TopoDS_Solid aSolid;
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
    BOPTools::MapShapesAndAncestors(aSolid, TopAbs_EDGE, TopAbs_FACE, aMEF);
    //
    // 2.1 Separate faces to process aMFP
    aMFP.Clear();
    aItMF.Initialize(aMF);
    for (; aItMF.More(); aItMF.Next()) {
      const TopoDS_Face& aF=(*(TopoDS_Face*)(&aItMF.Key()));
      if (!aMFx.Contains(aF)) {
        if (BOPTools_AlgoTools::IsInternalFace(aF, aSolid, aMEF, 1.e-14, myContext)) {
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
    BOPTools::MapShapesAndAncestors(aF, TopAbs_EDGE, TopAbs_FACE, aMEF);
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
    aState=BOPTools_AlgoTools::ComputeState(aF, *pS2, 1.e-14, aBounds, theContext);
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
//=======================================================================
//function : IsClosedShell
//purpose  : 
//=======================================================================
Standard_Boolean IsClosedShell(const TopoDS_Shell& theShell)
{
  Standard_Integer aNbE;
  Standard_Boolean bRet;
  TopoDS_Iterator aIt;
  TopExp_Explorer aExp;
  //
  BOPCol_MapOfShape aM;
  // 
  bRet=Standard_False;
  aIt.Initialize(theShell);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Face& aF=(*(TopoDS_Face*)(&aIt.Value()));
    aExp.Init(aF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aExp.Current()));
      if (BRep_Tool::Degenerated(aE)) {
        continue;
      }
      //
      if (aE.Orientation()==TopAbs_INTERNAL) {
        continue;
      }
      if (!aM.Add(aE)) {
        aM.Remove(aE);
      }
    }
  }
  //
  aNbE=aM.Extent();
  if (!aNbE) {
    bRet=!bRet;
  }
  return bRet;
}
