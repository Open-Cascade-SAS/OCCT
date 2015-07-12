// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// File: BOPAlgo_ShellSplitter.cxx
// Created: Thu Jan 16 08:33:50 2014

#include <BOPAlgo_ShellSplitter.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPCol_MapOfOrientedShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_CoupleOfShape.hxx>
#include <BRep_Builder.hxx>
#include <IntTools_Context.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>

//
//
//
//
//
static
  void MakeShell(const BOPCol_ListOfShape& , 
                 TopoDS_Shell& );
//
static
  void RefineShell(TopoDS_Shell& theShell,
                   BOPCol_ListOfShape& aLShX);
//
static
  void MapEdgesAndFaces
  (const TopoDS_Shape& aF,
   BOPCol_IndexedDataMapOfShapeListOfShape& aMEF,
   const Handle(NCollection_BaseAllocator)& theAllocator);

//=======================================================================
//class    : BOPAlgo_CBK
//purpose  : 
//=======================================================================
class BOPAlgo_CBK {
 public:
  BOPAlgo_CBK() : 
    myPCB (NULL) {
  }
  //
  ~BOPAlgo_CBK() {
  }
  //
  void SetConnexityBlock (const BOPTools_ConnexityBlock& aCB) {
    myPCB=(BOPTools_ConnexityBlock*)&aCB;
  }
  //
  BOPTools_ConnexityBlock& ConnexityBlock () {
    return *myPCB;
  }
  //
  void Perform() {
    BOPAlgo_ShellSplitter::SplitBlock(*myPCB);
  }
 protected:
  BOPTools_ConnexityBlock *myPCB;
};
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_CBK> BOPAlgo_VectorOfCBK; 
//
typedef BOPCol_Functor 
  <BOPAlgo_CBK,
  BOPAlgo_VectorOfCBK> BOPAlgo_CBKFunctor;
//
typedef BOPCol_Cnt 
  <BOPAlgo_CBKFunctor,
  BOPAlgo_VectorOfCBK> BOPAlgo_CBKCnt;
//
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_ShellSplitter::BOPAlgo_ShellSplitter()
:
  BOPAlgo_Algo(),
  myStartShapes(myAllocator),
  myShells(myAllocator),
  myLCB(myAllocator)
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_ShellSplitter::BOPAlgo_ShellSplitter
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Algo(theAllocator),
  myStartShapes(theAllocator),
  myShells(theAllocator),
  myLCB(myAllocator)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_ShellSplitter::~BOPAlgo_ShellSplitter()
{
}
//=======================================================================
//function : AddStartElement
//purpose  : 
//=======================================================================
void BOPAlgo_ShellSplitter::AddStartElement(const TopoDS_Shape& aE)
{
  myStartShapes.Append(aE);
}
//=======================================================================
//function : StartElements
//purpose  : 
//=======================================================================
const BOPCol_ListOfShape& BOPAlgo_ShellSplitter::StartElements()const
{
  return myStartShapes;
}
//=======================================================================
//function : Loops
//purpose  : 
//=======================================================================
const BOPCol_ListOfShape& BOPAlgo_ShellSplitter::Shells()const
{
  return myShells;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_ShellSplitter::Perform()
{
  myErrorStatus=0;
  //
  MakeConnexityBlocks();
  if (myErrorStatus) {
    return;
  }
  //
  MakeShells();
}
//=======================================================================
//function : MakeConnexityBlocks
//purpose  : 
//=======================================================================
void BOPAlgo_ShellSplitter::MakeConnexityBlocks()
{
  Standard_Boolean bRegular;
  Standard_Integer i, j, aNbE, aNbES, aNbEP, k, aNbCB;
  TopoDS_Shape aFR;
  TopoDS_Iterator aItF, aItW;
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF(100, myAllocator);
  BOPCol_IndexedMapOfShape aMEP(100, myAllocator);
  BOPCol_IndexedMapOfShape aMFC(100, myAllocator);
  BOPCol_MapOfShape aMER(100, myAllocator);
  BOPCol_MapOfShape aMFP(100, myAllocator);
  BOPCol_IndexedMapOfShape aMEAdd(100, myAllocator);
  BOPCol_MapOfShape aMES(100, myAllocator);
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  myErrorStatus=0;
  //
  myLCB.Clear();
  //
  const BOPCol_ListOfShape& aLSE=myStartShapes;
  aIt.Initialize(aLSE);
  for (i=1; aIt.More(); aIt.Next(), ++i) {
    const TopoDS_Shape& aSE=aIt.Value();
    if (!aMEP.Contains(aSE)) {
      aMEP.Add(aSE);
      MapEdgesAndFaces(aSE, aMEF, myAllocator);
    }
    else {
      aMER.Add(aSE);
    }
  }
  //
  // 2
  aNbE=aMEF.Extent();
  for (i=1; i<=aNbE; ++i) {
    aNbES=aMES.Extent();
    if (aNbES==aNbE) {
      break;
    }
    //
    const TopoDS_Shape& aE=aMEF.FindKey(i);
    //
    if (!aMES.Add(aE)) {
      continue;
    }
    // aMES - globally processed edges
    //
    //------------------------------------- goal: aMEC
    aMFC.Clear();    // aMEC - edges of CB
    aMEP.Clear();    // aMVP - edges to process right now 
    aMEAdd.Clear();  // aMVAdd edges to process on next step of for(;;) {
    //
    aMEP.Add(aE);
    //
    for(;;) {
      aNbEP=aMEP.Extent();
      for (k=1; k<=aNbEP; ++k) {
        const TopoDS_Shape& aEP=aMEP(k);
        const BOPCol_ListOfShape& aLF=aMEF.FindFromKey(aEP);
        aIt.Initialize(aLF);
        for (; aIt.More(); aIt.Next()) {
          const TopoDS_Shape& aF=aIt.Value();
          if (aMFC.Add(aF)) {
            aItF.Initialize(aF);
            while (aItF.More()) {
              const TopoDS_Shape& aW=aItF.Value();  
              if (aW.ShapeType()!=TopAbs_WIRE) {
                aItF.Next();
                continue;
              }
              //
              aItW.Initialize(aW);
              while (aItW.More()) {
                const TopoDS_Shape& aEF=aItW.Value();  
                //
                if (aMES.Add(aEF)) {
                  aMEAdd.Add(aEF);
                }
                //
                aItW.Next();
              }
              //
              aItF.Next();
            }
          }
        }
      }
      //
      aNbEP=aMEAdd.Extent();
      if (!aNbEP) {
        break; // from for(;;) {
      }
      //
      aMEP.Clear();
      //
      for (k=1; k<=aNbEP; ++k) {
        const TopoDS_Shape& aEF=aMEAdd(k);
        aMEP.Add(aEF);
      }
      aMEAdd.Clear();
    }// for(;;) {
    //
    //-------------------------------------
    BOPTools_ConnexityBlock aCB(myAllocator);
    //
    BOPCol_ListOfShape& aLECB=aCB.ChangeShapes();
    BOPCol_IndexedDataMapOfShapeListOfShape aMEFR(100, myAllocator);
    //
    bRegular=Standard_True;
    aNbCB = aMFC.Extent();
    for (j=1; j<=aNbCB; ++j) {
      aFR = aMFC(j);
      //
      if (aMER.Contains(aFR)) {
        aFR.Orientation(TopAbs_FORWARD);
        aLECB.Append(aFR);
        aFR.Orientation(TopAbs_REVERSED);
        aLECB.Append(aFR);
        bRegular=Standard_False;
      }
      else {
        aLECB.Append(aFR);
      }
      //
      if (bRegular) {
        MapEdgesAndFaces(aFR, aMEFR, myAllocator);
      }
    }
    //
    if (bRegular) {
      Standard_Integer aNbER, aNbFR; 
      //
      aNbER=aMEFR.Extent();
      for (k=1; k<=aNbER; ++k) {
        const BOPCol_ListOfShape& aLFR=aMEFR(k);
        aNbFR=aLFR.Extent();
        if (aNbFR>2) {
          bRegular=!bRegular;
          break;
        }
      }
    }
    //
    aCB.SetRegular(bRegular);
    myLCB.Append(aCB);
  }
}
//=======================================================================
//function : SplitBlock
//purpose  : 
//=======================================================================
void BOPAlgo_ShellSplitter::SplitBlock(BOPTools_ConnexityBlock& aCB)
{
  Standard_Integer aNbLF, aNbOff, aNbFP;
  Standard_Integer i;
  TopAbs_Orientation anOr;
  TopoDS_Edge aEL;
  BRep_Builder aBB;
  TopoDS_Iterator aItS;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape aItF;
  BOPTools_CoupleOfShape aCSOff;
  BOPCol_MapOfOrientedShape AddedFacesMap;
  BOPCol_IndexedDataMapOfShapeListOfShape aEFMap, aMEFP;
  Handle (IntTools_Context) aContext;
  // 
  aContext=new IntTools_Context;
  //
  const BOPCol_ListOfShape& myShapes=aCB.Shapes();
  //
  BOPCol_ListOfShape& myLoops=aCB.ChangeLoops();
  myLoops.Clear();
  //
  // 1. Shells Usual
  aItF.Initialize (myShapes);
  for (; aItF.More(); aItF.Next()) {
    const TopoDS_Shape& aFF = aItF.Value();
    BOPTools::MapShapesAndAncestors (aFF, 
                                     TopAbs_EDGE, 
                                     TopAbs_FACE, 
                                     aEFMap);
  }
  //
  aItF.Initialize (myShapes);
  for (i=1; aItF.More(); aItF.Next(), ++i) {
    const TopoDS_Shape& aFF = aItF.Value();
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
    BOPTools::MapShapesAndAncestors(aFF, 
                                    TopAbs_EDGE, 
                                    TopAbs_FACE, 
                                    aMEFP);
    //
    // loop on faces added to Shell; 
    // add their neighbor faces to Shell and so on
    aItS.Initialize (aShell);
    for (; aItS.More(); aItS.Next()) {
      const TopoDS_Face& aF = (*(TopoDS_Face*)(&aItS.Value()));
      //
      // loop on edges of aF; find a good neighbor face of aF by aE
      aExp.Init(aF, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aExp.Current()));
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
          BOPTools_AlgoTools::GetFaceOff(aE, 
                                         aF, 
                                         aLCSOff, 
                                         aSelF, 
                                         aContext);
        }
        //
        if (!aSelF.IsNull() && AddedFacesMap.Add(aSelF)) { 
          aBB.Add(aShell, aSelF);
          BOPTools::MapShapesAndAncestors(aSelF, 
                                          TopAbs_EDGE, 
                                          TopAbs_FACE, 
                                          aMEFP);
        }
      } // for (; aExp.More(); aExp.Next()) {
    } // for (; aItS.More(); aItS.Next()) {
    //
    BOPCol_ListOfShape aLShX; 
    BOPCol_ListIteratorOfListOfShape aItLShX;
    //
    RefineShell(aShell, aLShX);
    //
    aItLShX.Initialize(aLShX);
    for (; aItLShX.More(); aItLShX.Next()) { 
      TopoDS_Shell& aShX=*((TopoDS_Shell*)&aItLShX.Value());
      //
      if (BRep_Tool::IsClosed(aShX)) {
        aShX.Closed(Standard_True);
        myLoops.Append(aShX);
      }
    }
  } // for (; aItF.More(); aItF.Next()) {
}
//=======================================================================
//function : RefineShell
//purpose  : 
//=======================================================================
void RefineShell(TopoDS_Shell& theShell, 
                 BOPCol_ListOfShape& aLShX)
{
  TopoDS_Iterator aIt;
  //
  aIt.Initialize(theShell);
  if(!aIt.More()) {
    return;
  }
  //
  Standard_Integer i, aNbMEF, aNbF, aNbMFB;
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF; 
  TopoDS_Builder aBB;
  TopExp_Explorer aExp;
  BOPCol_IndexedMapOfShape aMFB;
  BOPCol_MapOfShape aMEStop, aMFProcessed;
  BOPCol_ListIteratorOfListOfShape aItLF, aItLFP;
  BOPCol_ListOfShape aLFP, aLFP1;
  //
  // Branch points
  BOPTools::MapShapesAndAncestors (theShell, 
                                   TopAbs_EDGE, 
                                   TopAbs_FACE, 
                                   aMEF);
  //
  aNbMEF=aMEF.Extent();
  for (i=1; i<=aNbMEF; ++i) {
    const TopoDS_Shape& aE=aMEF.FindKey(i);
    const BOPCol_ListOfShape& aLF=aMEF.FindFromIndex(i);
    aNbF=aLF.Extent();
    if (aNbF>2) {
      aMEStop.Add(aE);
    }
  }
  //
  if (aMEStop.IsEmpty()) {
    aLShX.Append(theShell);
    return;
  }
  //
  // The first Face 
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF1=aIt.Value();
    if (!aMFProcessed.Add(aF1)) {
      continue;
    }
    //
    aMFB.Clear();
    aLFP.Clear();
    //
    aMFB.Add(aF1);
    aLFP.Append(aF1);
    //
    // Trying to reach the branch point
    for (;;) {
      aItLFP.Initialize(aLFP);
      for (; aItLFP.More(); aItLFP.Next()) { 
        const TopoDS_Shape& aFP=aItLFP.Value();
        //
        aExp.Init(aFP, TopAbs_EDGE);
        for (; aExp.More(); aExp.Next()) {
          const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aExp.Current()));
          if (aMEStop.Contains(aE)) {
            continue;
          }
          //
          if (aE.Orientation() == TopAbs_INTERNAL) {
            continue;
          }
          //
          if (BRep_Tool::Degenerated(aE)) {
            continue;
          }
          //
          const BOPCol_ListOfShape& aLF=aMEF.FindFromKey(aE);
          //
          aItLF.Initialize(aLF);
          for (; aItLF.More(); aItLF.Next()) { 
            const TopoDS_Shape& aFP1=aItLF.Value();
            if (aFP1.IsSame(aFP)) {
              continue;
            }
            if (aMFB.Contains(aFP1)) {
              continue;
            }
            //
            aMFProcessed.Add(aFP1);
            aMFB.Add(aFP1);
            aLFP1.Append(aFP1);
          }// for (; aItLF.More(); aItLF.Next()) { 
        }// for (; aExp.More(); aExp.Next()) {
      } // for (; aItLFP.More(); aItLFP.Next()) { 
      //
      //
      if (aLFP1.IsEmpty()) {
        break;
      }
      //
      aLFP.Clear();
      aItLF.Initialize(aLFP1);
      for (; aItLF.More(); aItLF.Next()) { 
        const TopoDS_Shape& aFP1=aItLF.Value();
        aLFP.Append(aFP1);
      }
      aLFP1.Clear();
    }// for (;;) {
    //
    aNbMFB=aMFB.Extent();
    if (aNbMFB) {
      TopoDS_Shell aShX;
      aBB.MakeShell(aShX);
      //
      for (i=1; i<=aNbMFB; ++i) {
        const TopoDS_Shape& aFB=aMFB(i);
        aBB.Add(aShX, aFB);
      }
      aLShX.Append(aShX);
    }
  }//for (; aIt.More(); aIt.Next()) {
}
//=======================================================================
//function : MakeShells
//purpose  : 
//=======================================================================
void BOPAlgo_ShellSplitter::MakeShells()
{
  Standard_Boolean bIsRegular;
  Standard_Integer aNbVCBK, k;
  BOPTools_ListIteratorOfListOfConnexityBlock aItCB;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPAlgo_VectorOfCBK aVCBK;
  //
  myErrorStatus=0;
  myShells.Clear();
  //
  aItCB.Initialize(myLCB);
  for (; aItCB.More(); aItCB.Next()) {
    BOPTools_ConnexityBlock& aCB=aItCB.ChangeValue();
    bIsRegular=aCB.IsRegular();
    if (bIsRegular) {
      TopoDS_Shell aShell;
      //
      const BOPCol_ListOfShape& aLF=aCB.Shapes();
      MakeShell(aLF, aShell);
      aShell.Closed(Standard_True);
      myShells.Append(aShell);
    }
    else {
      BOPAlgo_CBK& aCBK=aVCBK.Append1();
      aCBK.SetConnexityBlock(aCB);
    }
  }
  //
  aNbVCBK=aVCBK.Extent();
  //===================================================
  BOPAlgo_CBKCnt::Perform(myRunParallel, aVCBK);
  //===================================================
  for (k=0; k<aNbVCBK; ++k) {
    BOPAlgo_CBK& aCBK=aVCBK(k);
    const BOPTools_ConnexityBlock& aCB=aCBK.ConnexityBlock();
    const BOPCol_ListOfShape& aLS=aCB.Loops();
    aIt.Initialize(aLS);
    for (; aIt.More(); aIt.Next()) {
      TopoDS_Shape& aShell=aIt.ChangeValue();
      aShell.Closed(Standard_True);
      myShells.Append(aShell);
    }
  }
}
//=======================================================================
//function : MakeShell
//purpose  : 
//=======================================================================
void MakeShell(const BOPCol_ListOfShape& aLS, 
               TopoDS_Shell& aShell)
{
  BRep_Builder aBB;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  aBB.MakeShell(aShell);
  //
  aIt.Initialize(aLS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF=aIt.Value();
    aBB.Add(aShell, aF);
  }
}
//=======================================================================
// function: MapEdgesAndFaces
// purpose: 
//=======================================================================
void MapEdgesAndFaces
  (const TopoDS_Shape& aF,
   BOPCol_IndexedDataMapOfShapeListOfShape& aMEF,
   const Handle(NCollection_BaseAllocator)& theAllocator)
{
  TopoDS_Iterator aItF, aItW;
  //
  aItF.Initialize(aF);
  while (aItF.More()) {
    const TopoDS_Shape& aW=aItF.Value();  
    if (aW.ShapeType()!=TopAbs_WIRE) {
      aItF.Next();
      continue;
    }
    //
    aItW.Initialize(aW);
    while (aItW.More()) {
      const TopoDS_Shape& aE=aItW.Value();  
      //
      if (aMEF.Contains(aE)) {
        BOPCol_ListOfShape& aLF=aMEF.ChangeFromKey(aE);
        aLF.Append(aF);
      }
      else {
        BOPCol_ListOfShape aLS(theAllocator);
        //
        aLS.Append(aF);
        aMEF.Add(aE, aLS);
      }
      //
      aItW.Next();
    }
    //
    aItF.Next();
  }
}
