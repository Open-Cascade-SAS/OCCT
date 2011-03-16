#include <BOP_WireSolidHistoryCollector.ixx>

#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_IndexedDataMapOfShapeInteger.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>
#include <BOPTools_PShapeShapeInterference.hxx>
#include <BOPTools_VVInterference.hxx>

#include <BOP_BuilderTools.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp.hxx>

static Standard_Boolean AddNewShape(const TopoDS_Shape&                 theKey,
				    const TopoDS_Shape&                 theItem,
				    TopTools_DataMapOfShapeListOfShape& theMap);

static void GetAncestorFaces(const Standard_Integer theShapeIndex,
			     const BooleanOperations_ShapesDataStructure& theDS,
			     TopTools_IndexedMapOfShape& theFaces);

static Standard_Integer GetShapeWire(const BooleanOperations_ShapesDataStructure& theDS,
				     const TopoDS_Shape& S1,
				     const TopoDS_Shape& S2,
				     TopoDS_Shape& WS);

// ====================================================================================================
// function: Constructor
// purpose:  
// ====================================================================================================
BOP_WireSolidHistoryCollector::BOP_WireSolidHistoryCollector(const TopoDS_Shape& theShape1,
							     const TopoDS_Shape& theShape2,
							     const BOP_Operation theOperation) :
BOP_HistoryCollector(theShape1, theShape2, theOperation)
{
}

// ====================================================================================================
// function: SetResult
// purpose:  
// ====================================================================================================
void BOP_WireSolidHistoryCollector::SetResult(const TopoDS_Shape&       theResult,
					      const BOPTools_PDSFiller& theDSFiller) 
{
  myResult = theResult;

  FillSection(theDSFiller);
  FillEdgeHistory(theDSFiller);

  myHasDeleted = Standard_False;

  TopExp_Explorer anExp(myS1, TopAbs_FACE);

  if(!anExp.More()) {
    anExp.Init(myS2, TopAbs_FACE);

    if(anExp.More()) {
      myHasDeleted = Standard_True;
      return;
    }
  }
  else {
    myHasDeleted = Standard_True;
    return;
  }

  Standard_Integer i = 0;

  for(i = (Standard_Integer)TopAbs_EDGE; !myHasDeleted && (i < (Standard_Integer)TopAbs_VERTEX); i++) {
    TopAbs_ShapeEnum aType = (TopAbs_ShapeEnum) i;
    TopTools_IndexedMapOfShape aMap;
    TopExp::MapShapes(myResult, aType, aMap);

    TopExp_Explorer anExpObj(myS1, aType);

    for(; anExpObj.More(); anExpObj.Next()) {
      const TopoDS_Shape& aS = anExpObj.Current();

      if(!aMap.Contains(aS)) {

	if((!myModifMap.IsBound(aS) || myModifMap(aS).IsEmpty()) &&
	   (!myGenMap.IsBound(aS) || myGenMap(aS).IsEmpty())) {
	  myHasDeleted = Standard_True;
	  break;
	}
      }
    }
    TopExp_Explorer anExpTool(myS2, aType);

    for(; anExpTool.More(); anExpTool.Next()) {
      const TopoDS_Shape& aS = anExpTool.Current();

      if(!aMap.Contains(aS)) {
	if((!myModifMap.IsBound(aS) || myModifMap(aS).IsEmpty()) &&
	   (!myGenMap.IsBound(aS) || myGenMap(aS).IsEmpty())) {
	  myHasDeleted = Standard_True;
	  break;
	}
      }
    }
  }
}

// ====================================================================================================
// function: FillSection
// purpose:  
// ====================================================================================================
void BOP_WireSolidHistoryCollector::FillSection(const BOPTools_PDSFiller& theDSFiller) 
{
  // for edges
  BOPTools_InterferencePool* pIntrPool = (BOPTools_InterferencePool*)&theDSFiller->InterfPool();
  const BOPTools_PavePool& aPavePool = theDSFiller->PaveFiller().PavePool();
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  Standard_Integer nb = aDS.NumberOfShapesOfTheObject() + aDS.NumberOfShapesOfTheTool();
  Standard_Integer i = 0;
  TopTools_IndexedMapOfShape aResultMap;

  if(!myResult.IsNull()) {
    TopExp::MapShapes(myResult, TopAbs_VERTEX, aResultMap);
  }

  for(i = 1; i <= nb; i++) {
    if(aDS.GetShapeType(i) != TopAbs_EDGE)
      continue;
    const BOPTools_PaveSet& aPaveSet = aPavePool.Value(aDS.RefEdge(i));
    const BOPTools_ListOfPave& aListOfPave = aPaveSet.Set();
    BOPTools_ListIteratorOfListOfPave anIt(aListOfPave);
    TopTools_IndexedMapOfShape aMapOfVertex;

    for(; anIt.More(); anIt.Next()) {
      const BOPTools_Pave& aPave = anIt.Value();

      if((aPave.Type() == BooleanOperations_EdgeSurface) ||
	 (aPave.Type() == BooleanOperations_EdgeEdge) ||
	 (aPave.Type() == BooleanOperations_VertexEdge)) {
	BOPTools_PShapeShapeInterference anInter = pIntrPool->GetInterference(aPave.Interference(), aPave.Type());

	if(anInter != NULL) {
	  const TopoDS_Shape& aS1 = aDS.Shape(anInter->Index1());
	  const TopoDS_Shape& aS2 = aDS.Shape(anInter->Index2());
	  const TopoDS_Shape& aNewShape = aDS.Shape(anInter->NewShape());

	  if(aNewShape.ShapeType() != TopAbs_VERTEX)
	    continue;

	  Standard_Boolean addfirst = Standard_False;
	  Standard_Boolean addsecond = Standard_False;

	  if(aResultMap.Contains(aNewShape)) {
	    if(i == anInter->Index1()) {
	      addfirst = Standard_True;

	      if(aS1.IsSame(aNewShape)) {
		aMapOfVertex.Add(aNewShape);
		continue;
	      }

	      if(aPave.Type() != BooleanOperations_EdgeEdge) {
		addsecond = Standard_True;

		if(aS2.IsSame(aNewShape)) {
		  aMapOfVertex.Add(aNewShape);
		  continue;
		}
	      }
	    }
	    else {
	      addsecond = Standard_True;

	      if(aS2.IsSame(aNewShape)) {
		aMapOfVertex.Add(aNewShape);
		continue;
	      }

	      if(aPave.Type() != BooleanOperations_EdgeEdge) {
		addfirst = Standard_True;

		if(aS1.IsSame(aNewShape)) {
		  aMapOfVertex.Add(aNewShape);
		  continue;
		}
	      }
	    }
            TopTools_ListOfShape thelist;
	    if(addfirst) {
	      // modified by NIZHNY-3643  Thu Nov 13 17:33:38 2003 .b
	      if( aPave.Type() == BooleanOperations_EdgeEdge) {
		TopTools_IndexedMapOfShape aSharedFaces;
		aSharedFaces.Clear();
		GetAncestorFaces(i,aDS,aSharedFaces);
		Standard_Integer anbFaces = aSharedFaces.Extent();                
		if( anbFaces != 0 ) {
		  Standard_Integer iFace = 0;
		  for( iFace = 1; iFace <= anbFaces; iFace++ ) {
		    if(!myGenMap.IsBound(aSharedFaces.FindKey(iFace)))
		      myGenMap.Bind(aSharedFaces.FindKey(iFace), thelist);
		    myGenMap.ChangeFind(aSharedFaces.FindKey(iFace)).Append(aNewShape);
		  }
		}
		else {
		  if(!myGenMap.IsBound(aS1))
		    myGenMap.Bind(aS1, thelist);
		  myGenMap.ChangeFind(aS1).Append(aNewShape);
		}
	      }
	      else {
		if(!myGenMap.IsBound(aS1))
		  myGenMap.Bind(aS1, thelist);
		myGenMap.ChangeFind(aS1).Append(aNewShape);
	      }
	      // modified by NIZHNY-3643  Thu Nov 13 17:34:45 2003 .e
	    }

	    if(addsecond) {
	      // modified by NIZHNY-3643  Thu Nov 13 17:33:38 2003 .b
	      if( aPave.Type() == BooleanOperations_EdgeEdge) {
		TopTools_IndexedMapOfShape aSharedFaces;
		aSharedFaces.Clear();
		GetAncestorFaces(i,aDS,aSharedFaces);
		Standard_Integer anbFaces = aSharedFaces.Extent();
		if( anbFaces != 0 ) {
		  Standard_Integer iFace = 0;
		  for( iFace = 1; iFace <= anbFaces; iFace++ ) {
		    if(!myGenMap.IsBound(aSharedFaces.FindKey(iFace)))
		      myGenMap.Bind(aSharedFaces.FindKey(iFace), thelist);
		    myGenMap.ChangeFind(aSharedFaces.FindKey(iFace)).Append(aNewShape);
		  }
		}
		else {
		  if(!myGenMap.IsBound(aS2))
		    myGenMap.Bind(aS2, thelist);
		  myGenMap.ChangeFind(aS2).Append(aNewShape);
		}
	      }
	      else {
		if(!myGenMap.IsBound(aS2))
		  myGenMap.Bind(aS2, thelist);
		myGenMap.ChangeFind(aS2).Append(aNewShape);
	      }
	      // modified by NIZHNY-3643  Thu Nov 13 19:01:19 2003 .e
	    }
	  }
	}
      }
    }
    // end for(; anIt.More...

    // fill history for edges which vertices are on other shape.begin
    TopTools_IndexedDataMapOfShapeListOfShape aMapOfOldNewVertex, aVEMap;
    TopExp::MapShapesAndAncestors(myResult, TopAbs_VERTEX, TopAbs_EDGE, aVEMap);
    Standard_Integer j = 0, k = 0;
    TopTools_ListOfShape thelist1;
    for(j = 1; j <= aDS.NumberOfSuccessors(i); j++) {
      Standard_Integer avindex = aDS.GetSuccessor(i, j);

      BOPTools_CArray1OfVVInterference& VVs = pIntrPool->VVInterferences();
      Standard_Integer aNb = VVs.Extent();
    
      for (k = 1; k <= aNb; k++) {
	BOPTools_VVInterference& VV=VVs(k);
	Standard_Integer anIndex1 = VV.Index1();
	Standard_Integer anIndex2 = VV.Index2();

	if((avindex == anIndex1) || (avindex == anIndex2)) {
	  Standard_Integer aNewShapeIndex = VV.NewShape();
	  TopoDS_Shape aNewShape = aDS.Shape(aNewShapeIndex);

	  if(!aVEMap.Contains(aNewShape))
	    continue;

	  if(aVEMap.FindFromKey(aNewShape).Extent() >= 2)
	    continue;

	  for(Standard_Integer vit = 0; vit < 2; vit++) {
	    TopoDS_Shape aShape = (vit == 0) ? aDS.Shape(anIndex1) : aDS.Shape(anIndex2);

	    if(!aMapOfOldNewVertex.Contains(aShape))
	      aMapOfOldNewVertex.Add(aShape, thelist1);
	    aMapOfOldNewVertex.ChangeFromKey(aShape).Append(aNewShape);
	  }
	  break;
	}
      }

      for(Standard_Integer aninterit = 0; aninterit < 2; aninterit++) {

	if(aninterit == 0)
	  aNb = pIntrPool->VEInterferences().Extent();
	else
	  aNb = pIntrPool->VSInterferences().Extent();
    
	for (k = 1; k <= aNb; k++) {
	  BOPTools_ShapeShapeInterference* anInterference = NULL;
	
	  if(aninterit == 0)
	    anInterference = (BOPTools_ShapeShapeInterference*)(&pIntrPool->VEInterferences().Value(k));
	  else
	    anInterference = (BOPTools_ShapeShapeInterference*)(&pIntrPool->VSInterferences().Value(k));
	  Standard_Integer anIndex1 = anInterference->Index1();
	  Standard_Integer anIndex2 = anInterference->Index2();

	  if((avindex == anIndex1) || (avindex == anIndex2)) {
	    Standard_Integer aNewShapeIndex = anInterference->NewShape();
	    TopoDS_Shape aNewShape = aDS.Shape(aNewShapeIndex);

	    if(!aVEMap.Contains(aNewShape))
	      continue;

	    if(aVEMap.FindFromKey(aNewShape).Extent() >= 2)
	      continue;
	    TopoDS_Shape aShape1 = aDS.Shape(avindex);
	    TopoDS_Shape aShape2 = (avindex == anIndex1) ? aDS.Shape(anIndex2) : aDS.Shape(anIndex1);

	    if(!aMapOfOldNewVertex.Contains(aShape1))
	      aMapOfOldNewVertex.Add(aShape1, thelist1);
	    aMapOfOldNewVertex.ChangeFromKey(aShape1).Append(aNewShape);

	    AddNewShape(aShape2, aNewShape, myGenMap);
	  }
	}
      }
    }

    // modified by NIZHNY-3643  Fri Nov 14 09:59:47 2003 .b
    TopoDS_Shape WShape;
    Standard_Integer wRank = GetShapeWire(aDS,myS1,myS2,WShape);
    // modified by NIZHNY-3643  Fri Nov 14 09:59:53 2003 .e

    if(!aMapOfOldNewVertex.IsEmpty()) {
      TopTools_IndexedDataMapOfShapeListOfShape aMapVE;
      
      // modified by NIZHNY-3643  Fri Nov 14 10:01:32 2003 .b
      Standard_Integer iSh = 0;
      TopoDS_Shape aCShape;
      for( iSh = 1; iSh <= 2; iSh ++ ) {

	if( iSh == 1 ) {
	  TopExp::MapShapesAndAncestors(myS1, TopAbs_VERTEX, TopAbs_EDGE, aMapVE);
	  aCShape = myS1;
	}
	else {
	  aMapVE.Clear();
	  TopExp::MapShapesAndAncestors(myS2, TopAbs_VERTEX, TopAbs_EDGE, aMapVE);
	  aCShape = myS2;
	}

	Standard_Integer vit = 0;

	for(vit = 1; vit <= aMapOfOldNewVertex.Extent(); vit++) {
	  const TopoDS_Shape& aV = aMapOfOldNewVertex.FindKey(vit);

	  if(!aMapVE.Contains(aV))
	    continue;
	  TopTools_ListIteratorOfListOfShape aEIt(aMapVE.FindFromKey(aV));
	  const TopTools_ListOfShape& aNewVList = aMapOfOldNewVertex.FindFromIndex(vit);

	  if(aNewVList.IsEmpty())
	    continue;
	  TopoDS_Shape aNewShape = aNewVList.First();

	  // for wire old
	  if( aCShape.IsSame(WShape) ) {
	    for(; aEIt.More(); aEIt.Next()) {
	      const TopoDS_Shape& aE = aEIt.Value();
	      AddNewShape(aE, aNewShape, myGenMap);
	    }
	    // avoid shared edges from solids here
	    Standard_Integer eRank = aDS.Rank(i);
	    if( eRank != wRank ) {
	      TopTools_IndexedMapOfShape aSharedFaces;
	      aSharedFaces.Clear();
	      GetAncestorFaces(i,aDS,aSharedFaces);
	      if( aSharedFaces.Extent() == 1 ) {
		TopoDS_Shape aE = aDS.Shape(i);
		AddNewShape(aE, aNewShape, myGenMap);
	      }
	    }
	    else {
	      TopoDS_Shape aE = aDS.Shape(i);
	      AddNewShape(aE, aNewShape, myGenMap);
	    }
	  }
	  else {
	    // for solid new
	    TopTools_IndexedMapOfShape aSharedFaces;
	    aSharedFaces.Clear();
	    Standard_Integer sRank = (wRank == 1) ? 2 : 1;
	    for(; aEIt.More(); aEIt.Next()) {
	      const TopoDS_Shape& aE = aEIt.Value();
	      Standard_Integer iE = aDS.ShapeIndex(aE,sRank);
	      if( iE != 0 )
		GetAncestorFaces(iE,aDS,aSharedFaces);
	    }
	    TopoDS_Shape aE = aDS.Shape(i);
	    Standard_Integer iE = aDS.ShapeIndex(aE,sRank);
	    if( iE != 0 )
	      GetAncestorFaces(iE,aDS,aSharedFaces);
	    if( aSharedFaces.Extent() != 0 ) {
	      Standard_Integer anbFaces = aSharedFaces.Extent();
	      Standard_Integer iFace = 0;
	      for( iFace = 1; iFace <= anbFaces; iFace++ ) {
		AddNewShape(aSharedFaces.FindKey(iFace), aNewShape, myGenMap);
	      }
	    }
	  }
	}
      }
      // modified by NIZHNY-3643  Fri Nov 14 10:02:19 2003 .e
    }
    // fill history for edges which vertices are on other shape.end

    if(!aMapOfVertex.IsEmpty()) {
      if(!myResult.IsNull()) {
	TopTools_IndexedDataMapOfShapeListOfShape aMapVE;
	TopExp::MapShapesAndAncestors(myS1, TopAbs_VERTEX, TopAbs_EDGE, aMapVE);
	TopExp::MapShapesAndAncestors(myS2, TopAbs_VERTEX, TopAbs_EDGE, aMapVE);
	Standard_Integer vit = 0;

	for(vit = 1; vit <= aMapOfVertex.Extent(); vit++) {
	  const TopoDS_Shape& aV = aMapOfVertex(vit);

	  if(!aMapVE.Contains(aV))
	    continue;
	  TopTools_ListIteratorOfListOfShape aEIt(aMapVE.FindFromKey(aV));

	  for(; aEIt.More(); aEIt.Next()) {
	    const TopoDS_Shape& aE = aEIt.Value();
	    AddNewShape(aE, aV, myGenMap);
	  }
	  TopoDS_Shape aE = aDS.Shape(i);
	  AddNewShape(aE, aV, myGenMap);
	}
      }
    }
  }
}

// ====================================================================================================
// function: FillEdgeHistory
// purpose:  Fills modified map for edges, 
//           fills generated map for edges (vertex type of intersection, edge type of intersection)
// ====================================================================================================
void BOP_WireSolidHistoryCollector::FillEdgeHistory(const BOPTools_PDSFiller& theDSFiller) 
{
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller           = theDSFiller->PaveFiller();
  const BOPTools_SplitShapesPool& aSplitShapesPool = aPaveFiller.SplitShapesPool();
  TopTools_IndexedMapOfShape aResultMap;

  if(!myResult.IsNull()) {
    TopExp::MapShapes(myResult, TopAbs_EDGE, aResultMap);
  }
  
  Standard_Integer iRank = 1;

  for(; iRank <= 2; iRank++) {
    BooleanOperations_StateOfShape aStateCmp, aState;
    aStateCmp = BOP_BuilderTools::StateToCompare(iRank, myOp);
    Standard_Integer i = 0;
    Standard_Integer nb = aDS.NumberOfShapesOfTheObject();
    nb = (iRank == 1) ? nb : (nb + aDS.NumberOfShapesOfTheTool());
    Standard_Integer startindex = (iRank == 1) ? 1 : (aDS.NumberOfShapesOfTheObject() + 1);

    for(i = startindex; i <= nb; i++) {
      if(aDS.GetShapeType(i) != TopAbs_EDGE)
	continue;
      const BOPTools_ListOfPaveBlock& aLPB = aSplitShapesPool(aDS.RefEdge(i));
      const TopoDS_Shape& anOldShape = aDS.Shape(i);

      if(!aLPB.IsEmpty()) {
	BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aLPB);

	for(; aPBIt.More(); aPBIt.Next()) {
	  const BOPTools_PaveBlock& aPB = aPBIt.Value();
	  Standard_Integer nSp = aPB.Edge();

	  if(nSp == i)
	    continue;
	  aState=aDS.GetState(nSp);

	  if ((aState == aStateCmp) || (myOp==BOP_COMMON && (aState == BooleanOperations_ON))) {
	    const TopoDS_Shape& aNewShape = aDS.Shape(nSp);

	    if(aResultMap.Contains(aNewShape)) {
	      Standard_Integer bIsGenerated = Standard_True;

	      if((myOp == BOP_CUT) || (myOp == BOP_FUSE)) {
		bIsGenerated = (iRank == 2);
	      }
	      else {
		if(myOp == BOP_CUT21) {
		  bIsGenerated = (iRank == 1);
		}
		else if(myOp == BOP_COMMON) {
		  bIsGenerated = Standard_False;
		}
	      }
	      TopTools_DataMapOfShapeListOfShape& aHistoryMap = (bIsGenerated) ? myGenMap : myModifMap;

	      if(aHistoryMap.IsBound(anOldShape)) {
		aHistoryMap.ChangeFind(anOldShape).Append(aNewShape);
	      }
	      else {
		TopTools_ListOfShape aShapeList;
		aShapeList.Append(aNewShape);
		aHistoryMap.Bind(anOldShape, aShapeList);
	      }
	    }
	  }
	}
      }
      //end if(!aLPB.IsEmpty...

      const BOPTools_CommonBlockPool& aCBPool = aPaveFiller.CommonBlockPool();
      const BOPTools_ListOfCommonBlock& aLCB = aCBPool(aDS.RefEdge(i));
      BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
      Standard_Boolean bWireIsObject = (myS1.ShapeType() == TopAbs_WIRE);
      Standard_Boolean bFillWithCommonPart = bWireIsObject ? (iRank == 1) : (iRank == 2);

      for (; bFillWithCommonPart && anItCB.More(); anItCB.Next()) {
	const BOPTools_CommonBlock& aCB = anItCB.Value();
	const BOPTools_PaveBlock& aPB = aCB.PaveBlock1();
	Standard_Integer nSp = aPB.Edge();
	TopoDS_Shape aNewShape = aDS.Shape(nSp);

	Standard_Boolean found = Standard_True;

	if(!aResultMap.Contains(aNewShape)) {
	  nSp = aCB.PaveBlock2().Edge();
	  found = Standard_False;

	  if(nSp) {
	    aNewShape = aDS.Shape(nSp);
	    found = aResultMap.Contains(aNewShape);
	  }
	}

	if(found) {
	  TopTools_DataMapOfShapeListOfShape& aHistoryMap = myModifMap;

	  if(aHistoryMap.IsBound(anOldShape)) {
	    aHistoryMap.ChangeFind(anOldShape).Append(aNewShape);
	  }
	  else {
	    TopTools_ListOfShape aShapeList;
	    aShapeList.Append(aNewShape);
	    aHistoryMap.Bind(anOldShape, aShapeList);
	  }
	}
      }
    }
    // end for(i = startindex...
  }
}

// --------------------------------------------------------------------------------
// static function: AddNewShape
// purpose:
// --------------------------------------------------------------------------------
Standard_Boolean AddNewShape(const TopoDS_Shape&                 theKey,
			     const TopoDS_Shape&                 theItem,
			     TopTools_DataMapOfShapeListOfShape& theMap) {


  if(!theMap.IsBound(theKey)) {
    TopTools_ListOfShape aList;
    aList.Append(theItem);
    theMap.Bind(theKey, aList);
    return Standard_True;
  }

  Standard_Boolean found = Standard_False;
  TopTools_ListOfShape& aList = theMap.ChangeFind(theKey);
  TopTools_ListIteratorOfListOfShape aVIt(aList);

  for(; aVIt.More(); aVIt.Next()) {
    if(theItem.IsSame(aVIt.Value())) {
      found = Standard_True;
      break;
    }
  }

  if(!found) {
    aList.Append(theItem);
  }
  return !found;
}

void GetAncestorFaces(const Standard_Integer theShapeIndex,
		      const BooleanOperations_ShapesDataStructure& theDS,
		      TopTools_IndexedMapOfShape& theFaces)
{
  // find ancestors for the edge
  Standard_Integer nbeA = theDS.NumberOfAncestors(theShapeIndex);
  if( nbeA == 0 ) return;
  Standard_Integer ieA = 0;
  for( ieA = 1; ieA <= nbeA; ieA++ ) {
    Standard_Integer indxA = theDS.GetAncestor(theShapeIndex,ieA);
    TopAbs_ShapeEnum aShapeType = theDS.GetShapeType(indxA);
    if( aShapeType == TopAbs_FACE ) {
      theFaces.Add(theDS.Shape(indxA));
    }
    else if( aShapeType == TopAbs_WIRE ) {
      GetAncestorFaces(indxA,theDS,theFaces);
    }
    else {
      return;
    }
  }
}

Standard_Integer GetShapeWire(const BooleanOperations_ShapesDataStructure& theDS,
			      const TopoDS_Shape& S1,
			      const TopoDS_Shape& S2,
			      TopoDS_Shape& WS)
{
  Standard_Integer wRank = 0;
  if( S1.ShapeType() == TopAbs_WIRE ) {
    if( S1.IsSame(theDS.Object()) )
      wRank = 1;
    else
      wRank = 2;
    WS = S1;
  }
  else {
    if( S2.IsSame(theDS.Object()) )
      wRank = 1;
    else
      wRank = 2;
    WS = S2;
  }
  return wRank;
}
