#include <BOP_SectionHistoryCollector.ixx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_PavePool.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BOPTools_VEInterference.hxx>
#include <BOPTools_VSInterference.hxx>
#include <TopoDS.hxx>
#include <TopTools_MapOfShape.hxx>

static Standard_Boolean AddNewShape(const TopoDS_Shape&                 theKey,
				    const TopoDS_Shape&                 theItem,
				    TopTools_DataMapOfShapeListOfShape& theMap);

// ============================================================================================
// function: Constructor
// purpose: 
// ============================================================================================
BOP_SectionHistoryCollector::BOP_SectionHistoryCollector(const TopoDS_Shape& theShape1,
							 const TopoDS_Shape& theShape2) :
BOP_HistoryCollector(theShape1,theShape2, BOP_SECTION)
{
}

// ============================================================================================
// function: SetResult
// purpose: 
// ============================================================================================
void BOP_SectionHistoryCollector::SetResult(const TopoDS_Shape&       theResult,
					    const BOPTools_PDSFiller& theDSFiller) 
{
  myResult = theResult;

  if(myResult.IsNull())
    return;

  TopAbs_ShapeEnum aResultType = TopAbs_EDGE;
  Standard_Boolean bcontainsface1 = Standard_False;
  Standard_Boolean bcontainsface2 = Standard_False;

  Standard_Boolean bcontainsedge1 = Standard_False;
  Standard_Boolean bcontainsedge2 = Standard_False;

  TopExp_Explorer anExp(myS1, TopAbs_FACE);

  if(anExp.More()) {
    bcontainsface1 = Standard_True;
    bcontainsedge1 = Standard_True;
  }
  else {
    anExp.Init(myS1, TopAbs_EDGE);

    if(anExp.More()) {
      bcontainsedge1 = Standard_True;
    }
  }
  anExp.Init(myS2, TopAbs_FACE);

  if(anExp.More()) {
    bcontainsface2 = Standard_True;
    bcontainsedge2 = Standard_True;
  }
  else {
    anExp.Init(myS2, TopAbs_EDGE);

    if(anExp.More()) {
      bcontainsedge2 = Standard_True;
    }
  }

  if(bcontainsface1 && bcontainsface2)
    aResultType = TopAbs_EDGE;
  else if(bcontainsedge1 && bcontainsedge2)
    aResultType = TopAbs_VERTEX;
  else
    return;

  myHasDeleted = Standard_True;


  TopTools_IndexedMapOfShape aMap;
  TopExp::MapShapes(myResult, aResultType, aMap);

  if(aResultType == TopAbs_EDGE) {
    FillFaceSection(theDSFiller, aMap);

    TopTools_IndexedDataMapOfShapeListOfShape aEFMap;
    TopTools_IndexedDataMapOfShapeListOfShape aVEMap;

    TopExp::MapShapesAndAncestors(myResult, TopAbs_VERTEX, TopAbs_EDGE, aVEMap);
    TopExp::MapShapesAndAncestors(myS1,       TopAbs_EDGE, TopAbs_FACE, aEFMap);
    TopExp::MapShapesAndAncestors(myS2,       TopAbs_EDGE, TopAbs_FACE, aEFMap);

    TopTools_IndexedMapOfShape aResultMap, aFreeBoundaryMap;
    Standard_Integer i = 0;

    for(i = 1; i <= aEFMap.Extent(); i++) {
      if(aEFMap.FindFromIndex(i).Extent() < 2)
	aFreeBoundaryMap.Add(aEFMap.FindKey(i));
    }

    for(i = 1; i <= aFreeBoundaryMap.Extent(); i++) {
      const TopoDS_Shape& anEdge = aFreeBoundaryMap.FindKey(i);
      FillEdgeSection(anEdge, theDSFiller, aMap, aVEMap, aEFMap);
    }
  }
}

// ============================================================================================
// function: FillFaceSection
// purpose: 
// ============================================================================================
void BOP_SectionHistoryCollector::FillFaceSection(const BOPTools_PDSFiller&         theDSFiller,
						  const TopTools_IndexedMapOfShape& theResultMap) 
{
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  const BOPTools_InterferencePool& anInterfPool = theDSFiller->InterfPool();
  BOPTools_InterferencePool* pInterfPool = (BOPTools_InterferencePool*) &anInterfPool;
  BOPTools_CArray1OfSSInterference& aFFs = pInterfPool->SSInterferences();
  //
  TopTools_IndexedMapOfShape aMap;
  Standard_Integer aNbFFs = aFFs.Extent();
  Standard_Integer i = 0, j = 0;

  for (i=1; i<=aNbFFs; ++i) {
    BOPTools_SSInterference& aFFi = aFFs(i);
    //
    Standard_Integer nF1 = aFFi.Index1();
    Standard_Integer nF2 = aFFi.Index2();
    TopoDS_Shape aF1 = aDS.Shape(nF1);
    TopoDS_Shape aF2 = aDS.Shape(nF2);
    Standard_Integer nSect = 0;

    // Old Section Edges
    const BOPTools_ListOfPaveBlock& aSectList = aFFi.PaveBlocks();
    BOPTools_ListIteratorOfListOfPaveBlock anIt(aSectList);

    for (; anIt.More();anIt.Next()) {
      const BOPTools_PaveBlock& aPB=anIt.Value();
      nSect = aPB.Edge();
      const TopoDS_Shape& aS = aDS.GetShape(nSect);

      if(theResultMap.Contains(aS)) {
	TopTools_ListOfShape thelist;
	if(!myGenMap.IsBound(aF1))
	  myGenMap.Bind(aF1, thelist );

	if(!myGenMap.IsBound(aF2))
	  myGenMap.Bind(aF2, thelist);

	for(Standard_Integer fit = 0; fit < 2; fit++) {
	  if(fit == 0)
	    AddNewShape(aF1, aS, myGenMap);
	  else
	    AddNewShape(aF2, aS, myGenMap);
	}
      }
    }

    // New Section Edges
    BOPTools_SequenceOfCurves& aBCurves = aFFi.Curves();
    Standard_Integer aNbCurves = aBCurves.Length();

    for (j = 1; j <= aNbCurves; j++) {
      BOPTools_Curve& aBC = aBCurves(j);
      const BOPTools_ListOfPaveBlock& aSectEdges = aBC.NewPaveBlocks();

      BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSectEdges);

      for (; aPBIt.More(); aPBIt.Next()) {
	BOPTools_PaveBlock& aPB = aPBIt.Value();
	nSect = aPB.Edge();
	const TopoDS_Shape& aS = aDS.GetShape(nSect);

	if(theResultMap.Contains(aS) && !aMap.Contains(aS)) {
          TopTools_ListOfShape thelist1;

	  if(!myGenMap.IsBound(aF1)) {
	    myGenMap.Bind(aF1, thelist1);
	  }
	  myGenMap.ChangeFind(aF1).Append(aS);

	  if(!myGenMap.IsBound(aF2))
	    myGenMap.Bind(aF2, thelist1);
	  myGenMap.ChangeFind(aF2).Append(aS);
	  aMap.Add(aS);
	}
      }
    }
  }
}

//  Modified by skv - Wed Nov  5 15:52:48 2003 OCC3644 Begin
// ============================================================================================
// function: IsEdgeToAdd
// purpose: 
// ============================================================================================

static Standard_Boolean IsEdgeToAdd
       (const TopoDS_Shape                              &theEdge,
	const TopTools_IndexedMapOfShape                &theResultMap,
	const TopTools_IndexedDataMapOfShapeListOfShape &theVEMapRes,
	const BOPTools_PDSFiller                        &theDSFiller)
{
  if (theEdge.ShapeType() != TopAbs_EDGE)
    return Standard_True;

  const BooleanOperations_ShapesDataStructure &aDS=theDSFiller->DS();
  const BOPTools_PaveFiller      &aPvFiller=theDSFiller->PaveFiller();
  const BOPTools_SplitShapesPool &aSplitShapesPool=aPvFiller.SplitShapesPool();
  Standard_Integer                aNbE1 = aDS.ShapeIndex(theEdge, 1);
  Standard_Integer                aNbE2 = aDS.ShapeIndex(theEdge, 2);
  Standard_Integer                aNbE  = (aNbE1 == 0) ? aNbE2 : aNbE1;

  if (aNbE == 0)
    return Standard_False;

  const BOPTools_ListOfPaveBlock         &aLPB=aSplitShapesPool(aDS.RefEdge(aNbE));
  BOPTools_ListIteratorOfListOfPaveBlock  aPBIt(aLPB);

  for (; aPBIt.More(); aPBIt.Next()) {
    const BOPTools_PaveBlock& aPB      = aPBIt.Value();
    Standard_Integer          aSplitNb = aPB.Edge();
    const TopoDS_Shape&       aSplit   = aDS.Shape(aSplitNb);

    if (theResultMap.Contains(aSplit)) {
      TopoDS_Vertex aVf;
      TopoDS_Vertex aVl;
      TopoDS_Edge   anEdge = TopoDS::Edge(aSplit);

      TopExp::Vertices(anEdge, aVf, aVl);

      if (theVEMapRes.FindFromKey(aVf).Extent() < 2 ||
	  theVEMapRes.FindFromKey(aVl).Extent() < 2)
	return Standard_False;
    }
  }

  return Standard_True;
}
//  Modified by skv - Wed Nov  5 15:52:50 2003 OCC3644 End

// ============================================================================================
// function: FillEdgeSection
// purpose: 
// ============================================================================================
void BOP_SectionHistoryCollector::FillEdgeSection
                  (const TopoDS_Shape                              &theEdge,
		   const BOPTools_PDSFiller                        &theDSFiller,
		   const TopTools_IndexedMapOfShape                &theResultMap,
		   const TopTools_IndexedDataMapOfShapeListOfShape &theVEMapRes,
		   const TopTools_IndexedDataMapOfShapeListOfShape &theEFMap)
{
  if(myResult.IsNull())
    return;

  TopTools_IndexedDataMapOfShapeListOfShape aMapOfOldNewVertex;

  BOPTools_InterferencePool* pIntrPool = (BOPTools_InterferencePool*)&theDSFiller->InterfPool();
  const BOPTools_PavePool& aPavePool = theDSFiller->PaveFiller().PavePool();
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();

  Standard_Integer anIndex = 0;
  const BooleanOperations_IndexedDataMapOfShapeInteger& aMap1 = theDSFiller->DS().ShapeIndexMap(1);
  const BooleanOperations_IndexedDataMapOfShapeInteger& aMap2 = theDSFiller->DS().ShapeIndexMap(2);

  if(aMap1.Contains(theEdge))
    anIndex = aMap1.FindFromKey(theEdge);
  else if(aMap2.Contains(theEdge))
    anIndex = aMap2.FindFromKey(theEdge);
  else
    return;

  const BOPTools_PaveSet& aPaveSet = aPavePool.Value(aDS.RefEdge(anIndex));
  const BOPTools_ListOfPave& aListOfPave = aPaveSet.Set();
  BOPTools_ListIteratorOfListOfPave anIt(aListOfPave);

  for(; anIt.More(); anIt.Next()) {
    const BOPTools_Pave& aPave = anIt.Value();
    const BooleanOperations_KindOfInterference aPaveType = aPave.Type();

    if((aPaveType == BooleanOperations_EdgeSurface)) {
      BOPTools_PShapeShapeInterference anInter = pIntrPool->GetInterference(aPave.Interference(), aPaveType);

      if(anInter == NULL)
	continue;

      const TopoDS_Shape& aS1 = aDS.Shape(anInter->Index1());
      const TopoDS_Shape& aS2 = aDS.Shape(anInter->Index2());
      const TopoDS_Shape& aNewShape = aDS.Shape(anInter->NewShape());

      if((aNewShape.ShapeType() != TopAbs_VERTEX) ||
	 (!theVEMapRes.Contains(aNewShape)))
	continue;

      if(theVEMapRes.FindFromKey(aNewShape).Extent() >= 2)
	continue;

      if(aS1.IsSame(aNewShape) || aS2.IsSame(aNewShape)) {
        TopTools_ListOfShape thelist;
	if(!aMapOfOldNewVertex.Contains(aNewShape))
	  aMapOfOldNewVertex.Add(aNewShape, thelist);
	aMapOfOldNewVertex.ChangeFromKey(aNewShape).Append(aNewShape);
	continue;
      }
      Standard_Boolean addfirst = Standard_True;
      Standard_Boolean addsecond = Standard_True;

      for(Standard_Integer sit = 0; sit < 2; sit++) {
	if(((sit == 0) && !addfirst) || ((sit != 0) && !addsecond))
	  continue;
	const TopoDS_Shape& aS = (sit == 0) ? aS1 : aS2;
//  Modified by skv - Wed Nov  5 17:11:41 2003 OCC3644 Begin
// 	AddNewShape(aS, aNewShape, myGenMap);
 	if (IsEdgeToAdd(aS, theResultMap, theVEMapRes, theDSFiller))
	  AddNewShape(aS, aNewShape, myGenMap);
//  Modified by skv - Wed Nov  5 17:11:49 2003 OCC3644 End
      }
    } else if (aPaveType == BooleanOperations_EdgeEdge ||
	       aPaveType == BooleanOperations_VertexEdge) {
      // Special treatment of case Edge-Edge and Edge-Vertex interference.
      BOPTools_PShapeShapeInterference anInter = pIntrPool->GetInterference(aPave.Interference(), aPaveType);

      if(anInter == NULL)
	continue;

      const TopoDS_Shape& aS1 = aDS.Shape(anInter->Index1());
      const TopoDS_Shape& aS2 = aDS.Shape(anInter->Index2());
      const TopoDS_Shape& aNewShape = aDS.Shape(anInter->NewShape());

      if((aNewShape.ShapeType() != TopAbs_VERTEX) ||
	 (!theVEMapRes.Contains(aNewShape)))
	continue;

      if(theVEMapRes.FindFromKey(aNewShape).Extent() >= 2)
	continue;

      Standard_Boolean isAddObj  = IsEdgeToAdd(aS1, theResultMap, theVEMapRes, theDSFiller);
      Standard_Boolean isAddTool = IsEdgeToAdd(aS2, theResultMap, theVEMapRes, theDSFiller);

      if (!isAddObj) {
	if (!theEFMap.Contains(aS1))
	  continue;

	AddNewShape(aS2, aNewShape, myGenMap);

	const TopTools_ListOfShape         &aFaces = theEFMap.FindFromKey(aS1);
	TopTools_ListIteratorOfListOfShape  aFIter(aFaces);

	for (; aFIter.More(); aFIter.Next()) {
	  const TopoDS_Shape &anAncFace = aFIter.Value();

	  AddNewShape(anAncFace, aNewShape, myGenMap);
	}
      } else if (!isAddTool) {
	if (!theEFMap.Contains(aS2))
	  continue;

	AddNewShape(aS1, aNewShape, myGenMap);

	const TopTools_ListOfShape         &aFaces = theEFMap.FindFromKey(aS2);
	TopTools_ListIteratorOfListOfShape  aFIter(aFaces);

	for (; aFIter.More(); aFIter.Next()) {
	  const TopoDS_Shape &anAncFace = aFIter.Value();

	  AddNewShape(anAncFace, aNewShape, myGenMap);
	}
      } else {
	if (!theEFMap.Contains(aS1) || !theEFMap.Contains(aS2))
	  continue;

	AddNewShape(aS1, aNewShape, myGenMap);
	AddNewShape(aS2, aNewShape, myGenMap);

	const TopTools_ListOfShape         &aFaces1 = theEFMap.FindFromKey(aS1);
	const TopTools_ListOfShape         &aFaces2 = theEFMap.FindFromKey(aS1);
	TopTools_ListIteratorOfListOfShape  aFIter(aFaces1);

	for (; aFIter.More(); aFIter.Next()) {
	  const TopoDS_Shape &anAncFace = aFIter.Value();

	  AddNewShape(anAncFace, aNewShape, myGenMap);
	}

	for (aFIter.Initialize(aFaces2); aFIter.More(); aFIter.Next()) {
	  const TopoDS_Shape &anAncFace = aFIter.Value();

	  AddNewShape(anAncFace, aNewShape, myGenMap);
	}
      }
    }
  }

  Standard_Integer i = 0, j = 0;

  for(j = 1; j <= aDS.NumberOfSuccessors(anIndex); j++) {
    Standard_Integer avindex = aDS.GetSuccessor(anIndex, j);

    BOPTools_CArray1OfVVInterference& VVs = pIntrPool->VVInterferences();
    Standard_Integer aNb = VVs.Extent();
    
    for (i = 1; i <= aNb; i++) {
      BOPTools_VVInterference& VV=VVs(i);
      Standard_Integer anIndex1 = VV.Index1();
      Standard_Integer anIndex2 = VV.Index2();

      if((avindex == anIndex1) || (avindex == anIndex2)) {
	Standard_Integer aNewShapeIndex = VV.NewShape();
	TopoDS_Shape aNewShape = aDS.Shape(aNewShapeIndex);

	if(!theVEMapRes.Contains(aNewShape))
	  continue;

	if(theVEMapRes.FindFromKey(aNewShape).Extent() >= 2)
	  continue;

	for(Standard_Integer vit = 0; vit < 2; vit++) {
	  TopoDS_Shape aShape = (vit == 0) ? aDS.Shape(anIndex1) : aDS.Shape(anIndex2);
          TopTools_ListOfShape thelist1;
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
    
      for (i = 1; i <= aNb; i++) {
	BOPTools_ShapeShapeInterference* anInterference = NULL;
	
	if(aninterit == 0)
	  anInterference = (BOPTools_ShapeShapeInterference*)(&pIntrPool->VEInterferences().Value(i));
	else
	  anInterference = (BOPTools_ShapeShapeInterference*)(&pIntrPool->VSInterferences().Value(i));
	Standard_Integer anIndex1 = anInterference->Index1();
	Standard_Integer anIndex2 = anInterference->Index2();

	if((avindex == anIndex1) || (avindex == anIndex2)) {
	  Standard_Integer aNewShapeIndex = anInterference->NewShape();
	  TopoDS_Shape aNewShape = aDS.Shape(aNewShapeIndex);

	  if(!theVEMapRes.Contains(aNewShape))
	    continue;

	  if(theVEMapRes.FindFromKey(aNewShape).Extent() >= 2)
	    continue;
	  TopoDS_Shape aShape1 = aDS.Shape(avindex);
	  TopoDS_Shape aShape2 = (avindex == anIndex1) ? aDS.Shape(anIndex2) : aDS.Shape(anIndex1);

	  if(aninterit == 0) {
	    TopTools_ListOfShape thelist2;
	    if(!aMapOfOldNewVertex.Contains(aShape1))
	      aMapOfOldNewVertex.Add(aShape1, thelist2);
	    aMapOfOldNewVertex.ChangeFromKey(aShape1).Append(aNewShape);
	  } else {
	    Standard_Integer aRank     = 1;
	    Standard_Integer aVtxIndex = aDS.ShapeIndex(aShape1, aRank);

	    if (aVtxIndex == 0) {
	      aRank     = 2;
	      aVtxIndex = aDS.ShapeIndex(aShape1, aRank);
	    }

	    if (aVtxIndex != 0) {
	      Standard_Integer    aNbEdges = aDS.NumberOfAncestors(aVtxIndex);
	      Standard_Integer    anEdgeInd;
	      TopTools_MapOfShape anAddedFaces;

	      for (anEdgeInd = 1; anEdgeInd <= aNbEdges; anEdgeInd++) {
		Standard_Integer    anEdgeId = aDS.GetAncestor(aVtxIndex, anEdgeInd);
		const TopoDS_Shape &anEdge   = aDS.GetShape(anEdgeId);

		if (IsEdgeToAdd(anEdge, theResultMap, theVEMapRes, theDSFiller))
		  AddNewShape(anEdge, aNewShape, myGenMap);
	      }
	    }
	  }

//  Modified by skv - Wed Nov  5 17:11:41 2003 OCC3644 Begin
// 	  AddNewShape(aShape2, aNewShape, myGenMap);
 	  if (IsEdgeToAdd(aShape2, theResultMap, theVEMapRes, theDSFiller))
	    AddNewShape(aShape2, aNewShape, myGenMap);
//  Modified by skv - Wed Nov  5 17:11:49 2003 OCC3644 End
	}
      }
    }
  }

  if(!aMapOfOldNewVertex.IsEmpty()) {
    Standard_Integer vit = 0;
    
    for(vit = 1; vit <= aMapOfOldNewVertex.Extent(); vit++) {
      const TopoDS_Shape& aV        = aMapOfOldNewVertex.FindKey(vit);
      Standard_Integer    aRank     = 1;
      Standard_Integer    aVtxIndex = aDS.ShapeIndex(aV, aRank);

      if (aVtxIndex == 0) {
	aRank     = 2;
	aVtxIndex = aDS.ShapeIndex(aV, aRank);
      }

      if (aVtxIndex == 0)
	continue;

      Standard_Integer    aNbEdges = aDS.NumberOfAncestors(aVtxIndex);
      Standard_Integer    anEdgeInd;
      TopTools_MapOfShape anAddedFaces;
      const TopTools_ListOfShape& aNewVList = aMapOfOldNewVertex.FindFromIndex(vit);

      if(aNewVList.IsEmpty())
	continue;

      TopoDS_Shape aNewShape = aNewVList.First();

      for (anEdgeInd = 1; anEdgeInd <= aNbEdges; anEdgeInd++) {
	Standard_Integer            anEdgeId = aDS.GetAncestor(aVtxIndex, anEdgeInd);
	const TopoDS_Shape         &anEdge   = aDS.GetShape(anEdgeId);
	const TopTools_ListOfShape &aFaces   = theEFMap.FindFromKey(anEdge);

	TopTools_ListIteratorOfListOfShape aFaceIter(aFaces);

	for (; aFaceIter.More(); aFaceIter.Next()) {
	  const TopoDS_Shape &aFace = aFaceIter.Value();

	  if (!anAddedFaces.Add(aFace))
	    continue;

	  AddNewShape(aFace, aNewShape, myGenMap);
	}
      }
    }
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
