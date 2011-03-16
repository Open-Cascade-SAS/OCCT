// File:	BOPTools_WireStateFiller.cxx
// Created:	Mon Feb  4 10:18:15 2002
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_WireStateFiller.ixx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <BRep_Tool.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_IndexedDataMapOfShapeInteger.hxx>
#include <BooleanOperations_StateOfShape.hxx>

#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_CommonBlockPool.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_InterferencePool.hxx>


//=======================================================================
// function:  BOPTools_WireStateFiller::BOPTools_WireStateFiller
// purpose: 
//=======================================================================
  BOPTools_WireStateFiller::BOPTools_WireStateFiller(const BOPTools_PaveFiller& aFiller)
:
  BOPTools_StateFiller(aFiller)
{
}

//=======================================================================
// function: Do 
// purpose: 
//=======================================================================
  void BOPTools_WireStateFiller::Do()
{
  TopAbs_ShapeEnum aT1, aT2;

  aT1=(myDS->Object()).ShapeType();
  aT2=(myDS->Tool()).ShapeType();

  myIsDone=Standard_True;
  
  if (aT1==TopAbs_WIRE && aT2==TopAbs_WIRE){
    DoWires(1);
    DoWires(2);
  }
  else if (aT1==TopAbs_WIRE  && aT2==TopAbs_SHELL){
    DoWires(1);
  }
  else if (aT2==TopAbs_WIRE  && aT1==TopAbs_SHELL){
    DoWires(2);
  }
  else if (aT1==TopAbs_WIRE && aT2==TopAbs_SOLID){
    DoWireSolid(1);
  }
  else if (aT2==TopAbs_WIRE && aT1==TopAbs_SOLID){
    DoWireSolid(2);
  }
  else {
    myIsDone=!myIsDone;
  }
  
}

//=======================================================================
// function: DoWires
// purpose: 
//=======================================================================
  void BOPTools_WireStateFiller::DoWires (const Standard_Integer iRankObj)
{
  const TopoDS_Shape& anObj=(iRankObj==1) ? myDS->Object() : myDS->Tool(); 
  //
  const BooleanOperations_IndexedDataMapOfShapeInteger& aDSMap=myDS->ShapeIndexMap(iRankObj);
  const BOPTools_SplitShapesPool& aSplitShapesPool=myFiller->SplitShapesPool();
  const BOPTools_CommonBlockPool& aCommonBlockPool=myFiller->CommonBlockPool();
  //
  Standard_Integer i, aNbPaveBlocks, nSp, aNbE, nE;
  BooleanOperations_StateOfShape aSt;
  BOPTools_ListIteratorOfListOfPaveBlock anItPB;
  BOPTools_ListIteratorOfListOfCommonBlock anItCB;
  TopTools_IndexedMapOfShape aEM;
  //
  TopExp::MapShapes(anObj, TopAbs_EDGE, aEM);
  aNbE=aEM.Extent();
  //
  // 1
  for (i=1; i<=aNbE; i++) {
    const TopoDS_Edge& aE=TopoDS::Edge(aEM(i));
    nE=aDSMap.FindFromKey(aE);
    //
    if (BRep_Tool::Degenerated(aE)){
      continue;
    }
    //
    const BOPTools_ListOfCommonBlock& aLCB=aCommonBlockPool(myDS->RefEdge(nE));
    //
    anItCB.Initialize(aLCB);
    for (; anItCB.More(); anItCB.Next()) {
      BOPTools_CommonBlock& aCB=anItCB.Value();
      BOPTools_PaveBlock& aPB=aCB.PaveBlock1(nE);
      nSp=aPB.Edge();
      myDS->SetState(nSp, BooleanOperations_ON);
    }
  }
  //
  // 2
  for (i=1; i<=aNbE; i++) {
    const TopoDS_Edge& aE=TopoDS::Edge(aEM(i));
    nE=aDSMap.FindFromKey(aE);
    //
    if (BRep_Tool::Degenerated(aE)){
      continue;
    }
    //
    const BOPTools_ListOfPaveBlock& aLPB=aSplitShapesPool(myDS->RefEdge(nE));
    //
    aNbPaveBlocks=aLPB.Extent();
    
    if (!aNbPaveBlocks) {
      myDS->SetState(nE, BooleanOperations_OUT);
      continue;
    }
    //
    anItPB.Initialize(aLPB);
    for (; anItPB.More(); anItPB.Next()) {
      const BOPTools_PaveBlock& aPB=anItPB.Value();
      nSp=aPB.Edge();
      aSt=myDS-> GetState(nSp);
      if (aSt!=BooleanOperations_ON) {
	myDS->SetState(nSp, BooleanOperations_OUT);
      }
    }
  }
}
//=======================================================================

#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <BRep_Builder.hxx>

#include <BooleanOperations_IndexedDataMapOfShapeInteger.hxx>

#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_CommonBlockPool.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_IndexedDataMapOfShapeWithState.hxx>
#include <BOPTools_StateFiller.hxx>

static
  void PropagateState(const TopoDS_Shape& aS,
		      const BooleanOperations_StateOfShape aState,
		      BooleanOperations_ShapesDataStructure* pDS,
		      const Standard_Integer iRank,
		      BOPTools_IndexedDataMapOfShapeWithState& aSWS,
		      TopTools_IndexedMapOfShape& aProcessedShapes);

static
  Standard_Boolean HasConnexity(const TopoDS_Shape& aS,
			       const BOPTools_IndexedDataMapOfShapeWithState& aSWS,
			       const TopTools_IndexedDataMapOfShapeListOfShape& aMVE,
			       BooleanOperations_StateOfShape& aState);

//=======================================================================
// function: DoWireSolid
// purpose: 
//=======================================================================
  void BOPTools_WireStateFiller::DoWireSolid (const Standard_Integer iRankObj)
{
  const TopoDS_Shape& anObj=(iRankObj==1) ? myDS->Object() : myDS->Tool();
  const TopoDS_Shape& aTool=(iRankObj==1) ? myDS->Tool()   : myDS->Object();
  //
  const BooleanOperations_IndexedDataMapOfShapeInteger& aDSMap=myDS->ShapeIndexMap(iRankObj);
  const BOPTools_SplitShapesPool& aSplitShapesPool=myFiller->SplitShapesPool();
  const BOPTools_CommonBlockPool& aCommonBlockPool=myFiller->CommonBlockPool();
  //
  Standard_Integer i, aNb, nE, aNbPB;
  BooleanOperations_StateOfShape aState;
  TopTools_IndexedMapOfShape aEM, anIntersectedShapes, aNonIntersectedShapes;
  TopTools_IndexedDataMapOfShapeListOfShape aM, aMVE;
  
  //
  // aM Map
  TopExp::MapShapesAndAncestors (anObj, TopAbs_EDGE , TopAbs_WIRE , aM);
  // VE Map
  TopExp::MapShapesAndAncestors (anObj, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  //
  // 1.2. Edges that have Split parts
  TopExp::MapShapes(anObj, TopAbs_EDGE, aEM);
  aNb=aEM.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aE=aEM(i);
    nE=aDSMap.FindFromKey(aE);
    const BOPTools_ListOfPaveBlock& aLPB=aSplitShapesPool(myDS->RefEdge(nE));
    aNbPB=aLPB.Extent();
    //
    if (!aNbPB) {
      continue;
    }
    //
    if (aNbPB==1) {
      const BOPTools_ListOfCommonBlock& aLCB=aCommonBlockPool(myDS->RefEdge(nE));
      if (!aLCB.Extent()) {
	const BOPTools_PaveBlock& aPB=aLPB.First();
	Standard_Integer nEB=aPB.Edge();
	if (nEB==aPB.OriginalEdge()) {
	  Standard_Boolean bHasInterference=Standard_False;// Wng in Gcc 3.0
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
    anIntersectedShapes.Add(aE);
  }// for (i=1; i<=aNb; i++)
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
  //
  TopTools_IndexedMapOfShape aProcessedShapes;
  BOPTools_IndexedDataMapOfShapeWithState aSWS;
  Standard_Boolean bHasConnexity;
  //
  aEM.Clear(); 
  TopExp::MapShapes(aCompound, TopAbs_EDGE, aEM);
  aNb=aEM.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aEM(i);
    if (!aProcessedShapes.Contains(aS)) {
      bHasConnexity=HasConnexity(aS, aSWS, aMVE, aState);
      if (!bHasConnexity) {
	aState=BOPTools_StateFiller::ClassifyShapeByRef (aS, aTool);
      }
      aSWS.Add(aS, aState);
      aProcessedShapes.Add(aS);
      PropagateState(aS, aState, myDS, iRankObj, aSWS, aProcessedShapes); 
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
