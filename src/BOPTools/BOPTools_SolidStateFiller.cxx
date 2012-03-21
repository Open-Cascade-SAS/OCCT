// Created on: 2001-05-28
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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



#include <BOPTools_SolidStateFiller.ixx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_State.hxx>

#include <Geom_Curve.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <gp_Pnt.hxx>

#include <IntTools_Tools.hxx>

#include <BooleanOperations_StateOfShape.hxx>
#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_IndexedDataMapOfShapeInteger.hxx>

#include <BOPTools_ListOfShapeEnum.hxx>
#include <BOPTools_ListIteratorOfListOfShapeEnum.hxx>
#include <BOPTools_IndexedDataMapOfShapeWithState.hxx>

#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_CArray1OfESInterference.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfESInterference.hxx>
#include <BOPTools_ESInterference.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_CommonBlockPool.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_StateFiller.hxx>

static
  void IntersectionStates(const TopoDS_Shape& aE,
			  const TopTools_IndexedDataMapOfShapeListOfShape& aM,
			  TopTools_IndexedMapOfShape& anIntersectedShapes);
static
  void PropagateState(const TopoDS_Shape& aS,
		      const BooleanOperations_StateOfShape aState,
		      BooleanOperations_ShapesDataStructure* pDS,
		      const Standard_Integer iRank,
		      BOPTools_IndexedDataMapOfShapeWithState& aSWS,
		      TopTools_IndexedMapOfShape& aProcessedShapes);
static
  void PropagateState(const TopoDS_Shape& aF,
		      const BooleanOperations_StateOfShape aState,
		      BooleanOperations_ShapesDataStructure* pDS,
		      const Standard_Integer iRank,
		      BOPTools_IndexedDataMapOfShapeWithState& aSWS,
		      const TopTools_IndexedDataMapOfShapeListOfShape& aEFMap,
		      TopTools_IndexedMapOfShape& aProcessedShapes);
static
  Standard_Boolean HasConnexity(const TopoDS_Shape& aS,
			       const BOPTools_IndexedDataMapOfShapeWithState& aSWS,
			       const TopTools_IndexedDataMapOfShapeListOfShape& aMVE,
			       BooleanOperations_StateOfShape& aState);

// Peter KURNEV
// p-kurnev@opencascade.com

//=======================================================================
// function: BOPTools_SolidStateFiller::BOPTools_SolidStateFiller
// purpose: 
//=======================================================================
  BOPTools_SolidStateFiller::BOPTools_SolidStateFiller(const BOPTools_PaveFiller& aFiller)
:
  BOPTools_StateFiller(aFiller)
{
}

//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOPTools_SolidStateFiller::Do()
{
  const TopoDS_Shape& anObj=myDS->Object();
  const TopoDS_Shape& aTool=myDS->Tool();
  //
  myIsDone=Standard_True;
  //
  TopAbs_ShapeEnum aT1, aT2;
  aT1=anObj.ShapeType();
  aT2=aTool.ShapeType();
  
  if (aT1==TopAbs_FACE) {
    aT1=TopAbs_SHELL;
  }
  if (aT2==TopAbs_FACE) {
    aT2=TopAbs_SHELL;
  }
  
  if (aT1==TopAbs_SOLID && aT2==TopAbs_SOLID) {
    DoNonSections(1);
    DoNonSections(2);
    DoSections();
  }

  else if (aT1==TopAbs_SHELL && aT2==TopAbs_SHELL) {
    DoShellNonSections(1);
    DoShellNonSections(2);
    DoSections();
  }
  
  else if (aT1==TopAbs_SHELL && aT2==TopAbs_SOLID) {
    DoNonSections(1);
    DoShellNonSections(2);
    DoSections();
  }
  else if (aT1==TopAbs_SOLID && aT2==TopAbs_SHELL) {
    DoShellNonSections(1);
    DoNonSections(2);
    DoSections();
  }
  else {
    myIsDone=!myIsDone;
  }
}

//=======================================================================
// function: DoNonSections
// purpose: 
//=======================================================================
  void BOPTools_SolidStateFiller::DoNonSections(const Standard_Integer iRankObj)
{
  //
  // 0. Restore data and preparing 

  const TopoDS_Shape& anObj=(iRankObj==1) ? myDS->Object() : myDS->Tool();
  const TopoDS_Shape& aTool=(iRankObj==1) ? myDS->Tool()   : myDS->Object();
 
  const BooleanOperations_IndexedDataMapOfShapeInteger& aDSMap=myDS->ShapeIndexMap(iRankObj);
  const BOPTools_SplitShapesPool& aSplitShapesPool=myFiller->SplitShapesPool();
  const BOPTools_CommonBlockPool& aCommonBlockPool=myFiller->CommonBlockPool();

  Standard_Integer i, nE, nF, aNbPaveBlocks, aNb;
  BooleanOperations_StateOfShape aState;
  TopTools_IndexedMapOfShape aEM, anIntersectedShapes, aNonIntersectedShapes;
  TopTools_IndexedDataMapOfShapeListOfShape aM, aMVE, aMEF;
  //
  // aM Map
  TopExp::MapShapesAndAncestors (anObj, TopAbs_EDGE , TopAbs_WIRE , aM);
  TopExp::MapShapesAndAncestors (anObj, TopAbs_WIRE , TopAbs_FACE , aM);
  TopExp::MapShapesAndAncestors (anObj, TopAbs_FACE , TopAbs_SHELL, aM);
  TopExp::MapShapesAndAncestors (anObj, TopAbs_SHELL, TopAbs_SOLID, aM);
  //
  // VE Map
  TopExp::MapShapesAndAncestors (anObj, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  //
  // 1. anIntersectedShapes 
  //
  // 1.1. Check EF FF interferences with nF 
  TopExp::MapShapes(anObj, TopAbs_FACE, aEM);
  aNb=aEM.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aF=aEM(i);
    nF=aDSMap.FindFromKey(aF);
    Standard_Boolean bExists=IsFaceIntersected(nF);
    if (bExists) {
      anIntersectedShapes.Add(aF);
      IntersectionStates (aF, aM, anIntersectedShapes);
    }
  }
  //
  aEM.Clear();
  TopExp::MapShapes(anObj, TopAbs_EDGE, aEM);
  //
  // 1.2. Edges that have Split parts
  aNb=aEM.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aE=aEM(i);
    nE=aDSMap.FindFromKey(aE);
    const BOPTools_ListOfPaveBlock& aSplitEdges=aSplitShapesPool(myDS->RefEdge(nE));
    aNbPaveBlocks=aSplitEdges.Extent();
    //
    if (!aNbPaveBlocks) {
      continue;
    }
    //
    if (aNbPaveBlocks==1) {
      const BOPTools_ListOfCommonBlock& aCBlocks=aCommonBlockPool(myDS->RefEdge(nE));
      if (!aCBlocks.Extent()) {
	const BOPTools_PaveBlock& aPB=aSplitEdges.First();
	Standard_Integer nEB=aPB.Edge();
	if (nEB==aPB.OriginalEdge()) {
	  Standard_Boolean bHasInterference=//;
	    Standard_False; //LNX
	  Standard_Integer j, aNbSuc, nV;
	  
	  aNbSuc=myDS->NumberOfSuccessors(nEB);
	  for (j=1; j<=aNbSuc; j++) {
	    nV=myDS->GetSuccessor(nE, j);
	    bHasInterference=myIntrPool->HasInterference(nV);
	    if (bHasInterference) {
	      break;
	    }
	  }
	  if (!bHasInterference) {
	    continue;
	  }
	}
      }
    }
    //
    anIntersectedShapes.Add(aE);
    IntersectionStates (aE, aM, anIntersectedShapes);
  } // for (i=1; i<=aNb; i++)

  //
  // 1.3. Write Intersected state for anIntersectedShapes to the DS
  aNb=anIntersectedShapes.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=anIntersectedShapes(i);
    nE=aDSMap.FindFromKey(aS);
    myDS->SetState(nE, BooleanOperations_INTERSECTED);
  }
  //
  // 2. aNonIntersectedShapes
  //
  aNb=aM.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aM.FindKey(i);
    if (!anIntersectedShapes.Contains(aS)) {
      aNonIntersectedShapes.Add(aS);
    }
  }
  //
  // 2.1. Processing of Non-intersected shapes 
  BRep_Builder BB;
  TopoDS_Compound aCompound;
  BB.MakeCompound(aCompound);
  aNb=aNonIntersectedShapes.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aNonIntersectedShapes(i);
    BB.Add(aCompound, aS);
  }
  
  TopExp::MapShapesAndAncestors (aCompound, TopAbs_EDGE, TopAbs_FACE, aMEF);
  //
  TopTools_IndexedMapOfShape aProcessedShapes;
  BOPTools_IndexedDataMapOfShapeWithState aSWS;
  Standard_Boolean bHasConnexity;
  //
  BOPTools_ListOfShapeEnum aEnumList;
  aEnumList.Append(TopAbs_SHELL);
  aEnumList.Append(TopAbs_FACE);
  aEnumList.Append(TopAbs_WIRE);
  aEnumList.Append(TopAbs_EDGE);
  
  BOPTools_ListIteratorOfListOfShapeEnum anIt(aEnumList);
  for (; anIt.More(); anIt.Next()) {
    TopAbs_ShapeEnum anEnum=anIt.Value();
    aEM.Clear(); 
    TopExp::MapShapes(aCompound, anEnum, aEM);
    aNb=aEM.Extent();
    for (i=1; i<=aNb; i++) {
      const TopoDS_Shape& aS=aEM(i);
      //
      // DEBUG
      //nE=aDSMap.FindFromKey(aS);
      //
      if (!aProcessedShapes.Contains(aS)) {
	bHasConnexity=HasConnexity(aS, aSWS, aMVE, aState);
	if (!bHasConnexity) {
	  aState=BOPTools_StateFiller::ClassifyShapeByRef (aS, aTool);
	}
	aSWS.Add(aS, aState);
	aProcessedShapes.Add(aS);
	if (anEnum==TopAbs_FACE) {
	  PropagateState(aS, aState, myDS, iRankObj, aSWS, aMEF, aProcessedShapes); 
	}
	else {
	  PropagateState(aS, aState, myDS, iRankObj, aSWS, aProcessedShapes); 
	}
      }
    }
  }
  //
  // 2.2. Write Stats for Non-intersected Shapes to the DS
  aNb=aSWS.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aSWS.FindKey(i);
    aState=aSWS.FindFromIndex(i);
    nE=aDSMap.FindFromKey(aS);
    myDS->SetState(nE, aState);
  }

  //---------------------------------------------------
  //
  // 3.  Intersected Edges' Processing
  //
  //---------------------------------------------------
  Standard_Integer nSp, aNBVertices, nV1, nV2;
  BooleanOperations_StateOfShape aStV1, aStV2;

  aNb=anIntersectedShapes.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=anIntersectedShapes(i);
    if (aS.ShapeType()==TopAbs_EDGE) {
      nE=aDSMap.FindFromKey(aS);
      //
      // 3.1. On Parts Processing
      const BOPTools_ListOfCommonBlock& aLCB=aCommonBlockPool(myDS->RefEdge(nE));
      BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
      for (; anItCB.More(); anItCB.Next()) {
	const BOPTools_CommonBlock& aCB=anItCB.Value();
	BOPTools_CommonBlock* pCB=(BOPTools_CommonBlock*) &aCB;
	BOPTools_PaveBlock& aPB=pCB->PaveBlock1(nE);
	nSp=aPB.Edge();
	myDS->SetState(nSp, BooleanOperations_ON);
      }
      //
      // 3.2. IN, OUT Parts Processing
      const BOPTools_ListOfPaveBlock& aSplits=aSplitShapesPool(myDS->RefEdge(nE));
      BOPTools_ListIteratorOfListOfPaveBlock anItPB(aSplits);
      for (; anItPB.More(); anItPB.Next()) {
	const BOPTools_PaveBlock& aPB=anItPB.Value();
	nSp=aPB.Edge();
	const TopoDS_Shape& aSplit=myDS->Shape(nSp);

	aState=myDS->GetState(nSp);
	if (aState==BooleanOperations_UNKNOWN|| aState==BooleanOperations_INTERSECTED){
	  aNBVertices=myDS->NumberOfSuccessors(nE);
	  if (aNBVertices==2) {
	    nV1=myDS->GetSuccessor(nSp, 1);
	    aStV1=myDS->GetState(nV1);
	    nV2=myDS->GetSuccessor(nSp, 2);
	    aStV2=myDS->GetState(nV2);
	    if      ((aStV1==BooleanOperations_IN || aStV1==BooleanOperations_OUT) 
		     && (aStV2==BooleanOperations_ON)) {
	      myDS->SetState(nSp, aStV1);
	    }
	    else if ((aStV2==BooleanOperations_IN || aStV2==BooleanOperations_OUT)
		     && (aStV1==BooleanOperations_ON)) {
	      myDS->SetState(nSp, aStV2);
	    }
	    else {
	      aState=BOPTools_StateFiller::ClassifyShapeByRef(aSplit, aTool);
	      myDS->SetState(nSp, aState);
	      if (aStV1==BooleanOperations_UNKNOWN) {
		myDS->SetState(nV1, aState);
	      }
	      if (aStV2==BooleanOperations_UNKNOWN) {
		myDS->SetState(nV2, aState);
	      }
	    }
	  }// if (aNBVertices==2)
	  else {
	    aState=BOPTools_StateFiller::ClassifyShapeByRef(aSplit, aTool);
	    myDS->SetState(nSp, aState);
	  }

	}// if (aState==BooleanOperations_UNKNOWN || BooleanOperations_INTERSECTED)
      }//for (; anItPB.More(); anItPB.Next())
    }// if (aS.ShapeType()==TopAbs_EDGE)
  }// next "Intersected" Edge
}
//
//=======================================================================
// function: DoShellNonSections
// purpose: 
//=======================================================================
  void BOPTools_SolidStateFiller::DoShellNonSections(const Standard_Integer iRankObj)
{

  const TopoDS_Shape& anObj=(iRankObj==1) ? myDS->Object() : myDS->Tool(); 
  //
  // 0. Restore data and preparing 
  const BooleanOperations_IndexedDataMapOfShapeInteger& aDSMap=myDS->ShapeIndexMap(iRankObj);

  const BOPTools_SplitShapesPool& aSplitShapesPool=myFiller->SplitShapesPool();
  const BOPTools_CommonBlockPool& aCommonBlockPool=myFiller->CommonBlockPool();

  Standard_Integer i, nE,  aNbPaveBlocks, aNb, nF1, nF2, iRank, nFx;
  BooleanOperations_StateOfShape aState;
  TopTools_IndexedMapOfShape aEM, anIntersectedShapes, aNonIntersectedShapes;
  TopTools_IndexedDataMapOfShapeListOfShape aM;
  //
  // aM Map
  TopExp::MapShapesAndAncestors (anObj, TopAbs_EDGE , TopAbs_WIRE , aM);
  TopExp::MapShapesAndAncestors (anObj, TopAbs_WIRE , TopAbs_FACE , aM);
  TopExp::MapShapesAndAncestors (anObj, TopAbs_FACE , TopAbs_SHELL, aM);
  TopExp::MapShapesAndAncestors (anObj, TopAbs_SHELL, TopAbs_SOLID, aM);
  //
  // 1. anIntersectedShapes 
  //
  // 1.1. Check EF FF interferences with nF 
  const BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  
  aNb=aFFs.Extent();
  for (i=1; i<=aNb; i++) {
    const BOPTools_SSInterference& aFF=aFFs(i);
    // nF1
    nF1=aFF.Index1();
    nF2=aFF.Index2();

    iRank=myDS->Rank(nF1);
    nFx=(iRank==myDS->Rank(nF1)) ? nF1 : nF2;

    const TopoDS_Shape& aFx=myDS->Shape(nFx);
    anIntersectedShapes.Add(aFx);
    IntersectionStates (aFx, aM, anIntersectedShapes);
  }
  
  //
  TopExp::MapShapes(anObj, TopAbs_EDGE, aEM);
  //
  // 1.2. Edges that have Split parts
  aNb=aEM.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aE=aEM(i);
    nE=aDSMap.FindFromKey(aE);
    const BOPTools_ListOfPaveBlock& aSplitEdges=aSplitShapesPool(myDS->RefEdge(nE));
    aNbPaveBlocks=aSplitEdges.Extent();
    //
    if (!aNbPaveBlocks) {
      continue;
    }
    //
    if (aNbPaveBlocks==1) {
      const BOPTools_ListOfCommonBlock& aCBlocks=aCommonBlockPool(myDS->RefEdge(nE));
      if (!aCBlocks.Extent()) {
	const BOPTools_PaveBlock& aPB=aSplitEdges.First();
	Standard_Integer nEB=aPB.Edge();
	if (nEB==aPB.OriginalEdge()) {
	  Standard_Boolean bHasInterference=//; LNX
	    Standard_False;//LNX
	  Standard_Integer j, aNbSuc, nV;
	  
	  aNbSuc=myDS->NumberOfSuccessors(nEB);
	  for (j=1; j<=aNbSuc; j++) {
	    nV=myDS->GetSuccessor(nE, j);
	    bHasInterference=myIntrPool->HasInterference(nV);
	    if (bHasInterference) {
	      break;
	    }
	  }
	  if (!bHasInterference) {
	    continue;
	  }
	}
      }
    }
    //
    anIntersectedShapes.Add(aE);
    IntersectionStates (aE, aM, anIntersectedShapes);
  } // for (i=1; i<=aNb; i++)
  //
  // 1.3. Write Intersected state for anIntersectedShapes to the DS
  aNb=anIntersectedShapes.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=anIntersectedShapes(i);
    if (aDSMap.Contains(aS)) {
      nE=aDSMap.FindFromKey(aS);
      myDS->SetState(nE, BooleanOperations_INTERSECTED);
    }
  }
  //
  // 2. aNonIntersectedShapes
  //
  aNb=aM.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aM.FindKey(i);
    if (!anIntersectedShapes.Contains(aS)) {
      aNonIntersectedShapes.Add(aS);
    }
  }
  //
  // 2.1. Write Stats for Non-intersected Shapes to the DS
  aNb=aNonIntersectedShapes.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aNonIntersectedShapes(i);
    nE=aDSMap.FindFromKey(aS);
    myDS->SetState(nE, BooleanOperations_OUT);
  }
  //---------------------------------------------------
  //
  // 3.  Intersected Edges' Processing
  //
  //---------------------------------------------------
  Standard_Integer nSp;
  

  aNb=anIntersectedShapes.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=anIntersectedShapes(i);
    if (aS.ShapeType()==TopAbs_EDGE) {
      nE=aDSMap.FindFromKey(aS);
      //
      // 3.1. On Parts Processing
      const BOPTools_ListOfCommonBlock& aLCB=aCommonBlockPool(myDS->RefEdge(nE));
      BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
      for (; anItCB.More(); anItCB.Next()) {
	const BOPTools_CommonBlock& aCB=anItCB.Value();
	BOPTools_CommonBlock* pCB=(BOPTools_CommonBlock*) &aCB;
	BOPTools_PaveBlock& aPB=pCB->PaveBlock1(nE);
	nSp=aPB.Edge();
	myDS->SetState(nSp, BooleanOperations_ON);
      }
      //
      // 3.2. IN, OUT Parts Processing
      const BOPTools_ListOfPaveBlock& aSplits=aSplitShapesPool(myDS->RefEdge(nE));
      BOPTools_ListIteratorOfListOfPaveBlock anItPB(aSplits);
      for (; anItPB.More(); anItPB.Next()) {
	const BOPTools_PaveBlock& aPB=anItPB.Value();
	nSp=aPB.Edge();
	//const TopoDS_Shape& aSplit=myDS->Shape(nSp);//LNX

	aState=myDS->GetState(nSp);
	if (aState==BooleanOperations_UNKNOWN|| aState==BooleanOperations_INTERSECTED){
	  myDS->SetState(nSp, BooleanOperations_OUT);
	
	}// if (aState==BooleanOperations_UNKNOWN || BooleanOperations_INTERSECTED)
      }//for (; anItPB.More(); anItPB.Next())
    }// if (aS.ShapeType()==TopAbs_EDGE)
  }// next "Intersected" Edge
}
//
//=======================================================================
// function: DoSections
// purpose: 
//=======================================================================
  void BOPTools_SolidStateFiller::DoSections()
{
  Standard_Integer i, j, aNb, aNbCurves,  n1, n2, nE;
  
  BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  
  aNb=aFFs.Extent();
  for (i=1; i<=aNb; i++) {
    BOPTools_SSInterference& aFF=aFFs(i);
    n1=aFF.Index1();
    n2=aFF.Index2();
    BOPTools_SequenceOfCurves& aSC=aFF.Curves();
    aNbCurves=aSC.Length();
    for (j=1; j<=aNbCurves; j++) {
      const BOPTools_Curve& aBC=aSC(j);
      const BOPTools_ListOfPaveBlock& aLPB=aBC.NewPaveBlocks();
      BOPTools_ListIteratorOfListOfPaveBlock anIt(aLPB);
      for (; anIt.More(); anIt.Next()) {
	const BOPTools_PaveBlock& aPB=anIt.Value();
	nE=aPB.Edge();
	myDS->SetState(nE, BooleanOperations_ON);
      }
    }
  } 
}

//=======================================================================
// function: IsFaceIntersected
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_SolidStateFiller::IsFaceIntersected(const Standard_Integer nF) 
{
  Standard_Boolean bFlag=Standard_False;
  Standard_Integer i, aNb, n1, n2;
  
  const BOPTools_CArray1OfESInterference& aEFs=myIntrPool->ESInterferences();
  aNb=aEFs.Extent();
  for (i=1; i<=aNb; i++) {
    const BOPTools_ESInterference& aEF=aEFs(i);
    n1=aEF.Index1();
    n2=aEF.Index2();
    if (n1==nF || n2==nF) {
      return !bFlag;
    }
  }

  const BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  aNb=aFFs.Extent();
  for (i=1; i<=aNb; i++) {
    const BOPTools_SSInterference& aFF=aFFs(i);
    n1=aFF.Index1();
    n2=aFF.Index2();
    if (n1==nF || n2==nF) {
      return !bFlag;
    }
  }
  return bFlag;
}





//=======================================================================
// function: PropagateState [for faces]
// purpose: 
//=======================================================================
  void PropagateState(const TopoDS_Shape& aF,
		      const BooleanOperations_StateOfShape aState,
		      BooleanOperations_ShapesDataStructure* pDS,
		      const Standard_Integer iRank,
		      BOPTools_IndexedDataMapOfShapeWithState& aSWS,
		      const TopTools_IndexedDataMapOfShapeListOfShape& aEFMap,
		      TopTools_IndexedMapOfShape& aProcessedShapes)
{
  // PropagateState for Sub-Shapes
  PropagateState (aF, aState, pDS, iRank, aSWS, aProcessedShapes);
  // PropagateState for connected Faces;
  TopTools_IndexedMapOfShape anEdgeMap;
  TopExp::MapShapes(aF, TopAbs_EDGE, anEdgeMap);
  
  Standard_Integer i, aNbE;
  aNbE=anEdgeMap.Extent();
  
  for (i=1; i<=aNbE; i++) {
    const TopoDS_Shape& aE=anEdgeMap(i);
    //
    const TopTools_ListOfShape& aFacesList=aEFMap.FindFromKey(aE);
    //
    TopTools_ListIteratorOfListOfShape anIt(aFacesList);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aFi=anIt.Value();
      if (!aProcessedShapes.Contains(aFi)) {
	if (!aFi.IsSame(aF)) {
	  aSWS.Add(aFi, aState);
	  aProcessedShapes.Add(aFi);
	  PropagateState(aFi, aState, pDS, iRank, aSWS, aEFMap, aProcessedShapes);
	}
      }
    }
  }
}

//=======================================================================
// function:  PropagateState
// purpose: 
//=======================================================================
void PropagateState(const TopoDS_Shape& aSS,
		    const BooleanOperations_StateOfShape aState,
		    BooleanOperations_ShapesDataStructure* pDS,
		    const Standard_Integer iRank,
		    BOPTools_IndexedDataMapOfShapeWithState& aSWS,
		    TopTools_IndexedMapOfShape& aProcessedShapes)
{
  TopAbs_ShapeEnum aSubType;

  aSubType=BOPTools_StateFiller::SubType(aSS);
  
  if (aSubType==TopAbs_SHAPE) {
    return;
  }

  const BooleanOperations_IndexedDataMapOfShapeInteger& aDSMap= pDS->ShapeIndexMap(iRank);

  TopTools_IndexedMapOfShape aSubMap;
  TopExp::MapShapes(aSS, aSubType, aSubMap);

  Standard_Integer i, aNb, nV;
  aNb=aSubMap.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aSubMap(i);
    if (!aProcessedShapes.Contains(aS)) {
      if (aSubType==TopAbs_VERTEX) {
	nV=aDSMap.FindFromKey(aS);
	BooleanOperations_StateOfShape aSt=pDS->GetState(nV);
	if (aSt!=BooleanOperations_UNKNOWN){
	  aProcessedShapes.Add(aS);
	  continue;
	}
      }
      aSWS.Add(aS, aState);
      aProcessedShapes.Add(aS);
      PropagateState (aS, aState, pDS, iRank, aSWS, aProcessedShapes);
    }
  }
}	    
//=======================================================================
// function: IntersectionStates 
// purpose: 
//=======================================================================
void IntersectionStates(const TopoDS_Shape& aE,
			const TopTools_IndexedDataMapOfShapeListOfShape& aM,
			TopTools_IndexedMapOfShape& anIntersectedShapes)
{
  if (aM.Contains(aE)) {
    const TopTools_ListOfShape& anAncesstors=aM.FindFromKey(aE);
    TopTools_ListIteratorOfListOfShape anIt(anAncesstors);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& anAnc=anIt.Value();
      anIntersectedShapes.Add(anAnc);
      IntersectionStates(anAnc, aM, anIntersectedShapes);
    }
  }
}
//=======================================================================
// function:  HasConnexity
// purpose: 
//=======================================================================
 Standard_Boolean HasConnexity(const TopoDS_Shape& aS,
			       const BOPTools_IndexedDataMapOfShapeWithState& aSWS,
			       const TopTools_IndexedDataMapOfShapeListOfShape& aMVE,
			       BooleanOperations_StateOfShape& aState)
{
  TopAbs_ShapeEnum aType;
  BooleanOperations_StateOfShape aSt;
  aType=aS.ShapeType();
  if (aType!=TopAbs_EDGE) {
    Standard_Integer i, aNb;
    TopTools_IndexedMapOfShape aME;
    TopExp::MapShapes(aS, TopAbs_EDGE, aME);
    aNb=aME.Extent();
    for (i=1; i<=aNb; i++) {
      const TopoDS_Shape& aE=aME(i);
      if (aSWS.Contains(aE)){
	aSt=aSWS.FindFromKey(aE);
	aState=aSt;
	return Standard_True;
      }
    }
  }
  else {
    TopExp_Explorer anExp (aS, TopAbs_VERTEX);
    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aV=anExp.Current();
      if (aMVE.Contains(aV)) {
	const TopTools_ListOfShape& anEdgesList=aMVE.FindFromKey(aV);
	TopTools_ListIteratorOfListOfShape anIt(anEdgesList);
	for (; anIt.More(); anIt.Next()) {
	  const TopoDS_Shape& aEx=anIt.Value();
	  if (aSWS.Contains(aEx)) {
	    aSt=aSWS.FindFromKey(aEx);
	    aState=aSt;
	    return Standard_True;
	  }
	}
      }
    }
  }
  
  aState=BooleanOperations_UNKNOWN;
  return Standard_False;
}

/*
//=======================================================================
// function:  ConvertState
// purpose: 
//=======================================================================
BooleanOperations_StateOfShape 
    BOPTools_SolidStateFiller::ConvertState(const TopAbs_State aSt)
{
  BooleanOperations_StateOfShape aState;
  switch (aSt) {
    case TopAbs_IN:
      aState=BooleanOperations_IN;
      break;
    case TopAbs_OUT:
      aState=BooleanOperations_OUT;
      break;  
    case TopAbs_ON:
      aState=BooleanOperations_ON;
      break;  
    case TopAbs_UNKNOWN:
      aState=BooleanOperations_UNKNOWN;
      break;  
    default:
      aState=BooleanOperations_UNKNOWN;
      break;  
  }
  return aState;
}

//=======================================================================
// function:  ConvertState
// purpose: 
//=======================================================================
TopAbs_State 
    BOPTools_SolidStateFiller::ConvertState(const BooleanOperations_StateOfShape aSt)
{
  TopAbs_State aState;
  
  switch (aSt) {
    case BooleanOperations_IN:
      aState=TopAbs_IN;
      break;
    case BooleanOperations_OUT:
      aState=TopAbs_OUT;
      break;  
    case BooleanOperations_ON:
      aState=TopAbs_ON;
      break;  
    case BooleanOperations_UNKNOWN:
      aState=TopAbs_UNKNOWN;
      break;  
    default:
      aState=TopAbs_UNKNOWN;
      break;  
  }
  return aState;
}					    
//=======================================================================
// function:  ClassifyShapeByRef
// purpose: 
//=======================================================================
  BooleanOperations_StateOfShape 
    BOPTools_SolidStateFiller::ClassifyShapeByRef (const TopoDS_Shape& aS,
						   const TopoDS_Shape& aRef)
{
  TopAbs_ShapeEnum aType;
  aType=aS.ShapeType();

  TopoDS_Edge aE;
  if (aType!=TopAbs_EDGE) {
    TopTools_IndexedMapOfShape aME;
    TopExp::MapShapes(aS, TopAbs_EDGE, aME);
    aE=TopoDS::Edge(aME(1));
  }
  else {
    aE=TopoDS::Edge(aS);
  }
  
  TopAbs_State aSt=BOPTools_SolidStateFiller::ClassifyEdgeToSolidByOnePoint(aE, aRef);
  BooleanOperations_StateOfShape aState=BOPTools_SolidStateFiller::ConvertState(aSt) ;
  
  return aState;
}

//=======================================================================
// function:  ClassifyEdgeToSolidByOnePoint
// purpose: 
//=======================================================================
TopAbs_State 
  BOPTools_SolidStateFiller::ClassifyEdgeToSolidByOnePoint(const TopoDS_Edge& E,
							   const TopoDS_Shape& Ref)
{
  Standard_Real f2 = 0., l2 = 0., par = 0.;
  
  Handle(Geom_Curve) C3D = BRep_Tool::Curve(E, f2, l2);
  gp_Pnt aP3d;

  

  if(C3D.IsNull()) {
    //it means that we are in degenerated edge
    const TopoDS_Vertex& fv = TopExp::FirstVertex(E);
    if(fv.IsNull()){
      return TopAbs_UNKNOWN;
    }
    aP3d = BRep_Tool::Pnt(fv);
  }
  else {//usual case
    par=IntTools_Tools::IntermediatePoint(f2, l2);
    C3D -> D0(par, aP3d);
  }
    
  BRepClass3d_SolidClassifier SC(Ref);
  SC.Perform(aP3d, 1e-7);
  
  TopAbs_State aState=SC.State();
  
  return aState;
}
//=======================================================================
// function:  SubType
// purpose: 
//=======================================================================
  TopAbs_ShapeEnum BOPTools_SolidStateFiller::SubType(const TopoDS_Shape& aS)
{
  TopAbs_ShapeEnum aSourceType, aReturnType;
  aSourceType=aS.ShapeType();

  switch (aSourceType) {
    case TopAbs_SOLID:
      aReturnType=TopAbs_SHELL;
      break;
    case TopAbs_SHELL:
      aReturnType=TopAbs_FACE;
      break;
    case TopAbs_FACE:
      aReturnType=TopAbs_WIRE;
      break;  
    case TopAbs_WIRE:
      aReturnType=TopAbs_EDGE;
      break;   
    case TopAbs_EDGE:
      aReturnType=TopAbs_VERTEX;
      break;     
    default:
      aReturnType=TopAbs_SHAPE;
      break;
  }
  return aReturnType;
}
*/
