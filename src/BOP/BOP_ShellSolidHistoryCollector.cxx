// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <BOP_ShellSolidHistoryCollector.ixx>

#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
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
#include <BOP_BuilderTools.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopExp.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

// ====================================================================================================
// function: Constructor
// purpose:
// ====================================================================================================
BOP_ShellSolidHistoryCollector::BOP_ShellSolidHistoryCollector(const TopoDS_Shape& theShape1,
							       const TopoDS_Shape& theShape2,
							       const BOP_Operation theOperation) :
BOP_HistoryCollector(theShape1, theShape2, theOperation)
{
}

// ====================================================================================================
// function: AddNewFace
// purpose:
// ====================================================================================================
void BOP_ShellSolidHistoryCollector::AddNewFace(const TopoDS_Shape&       theOldShape,
						const TopoDS_Shape&       theNewShape,
						const BOPTools_PDSFiller& theDSFiller) 
{
  Standard_Boolean bAddAsGenerated = Standard_True;

  if(theOldShape.ShapeType() != theNewShape.ShapeType())
    return;

  const BooleanOperations_IndexedDataMapOfShapeInteger& aMap = theDSFiller->DS().ShapeIndexMap(1);
  Standard_Boolean bIsObject = (myS1.ShapeType() == TopAbs_SHELL) ? aMap.Contains(theOldShape) : !aMap.Contains(theOldShape);

  if(bIsObject) {
    bAddAsGenerated = Standard_False;
  }

  TopTools_DataMapOfShapeListOfShape& aHistoryMap = (bAddAsGenerated) ? myGenMap : myModifMap;

  if(aHistoryMap.IsBound(theOldShape)) {
    aHistoryMap.ChangeFind(theOldShape).Append(theNewShape);
  }
  else {
    TopTools_ListOfShape aShapeList;
    aShapeList.Append(theNewShape);
    aHistoryMap.Bind(theOldShape, aShapeList);
  }
}

// ====================================================================================================
// function: SetResult
// purpose:
// ====================================================================================================
void BOP_ShellSolidHistoryCollector::SetResult(const TopoDS_Shape&       theResult,
					       const BOPTools_PDSFiller& theDSFiller) 
{
  myResult = theResult;
  FillSection(theDSFiller);
  FillEdgeHistory(theDSFiller);

  myHasDeleted = Standard_False;
  Standard_Integer i = 0;

  TopTools_IndexedMapOfShape aFreeBoundaryMap;
  TopTools_IndexedDataMapOfShapeListOfShape aEFMap;

  if(myS1.ShapeType() == TopAbs_SHELL) {
    TopExp::MapShapesAndAncestors(myS1, TopAbs_EDGE, TopAbs_FACE, aEFMap);
  }
  else {
    TopExp::MapShapesAndAncestors(myS2, TopAbs_EDGE, TopAbs_FACE, aEFMap);
  }

  for(i = 1; i <= aEFMap.Extent(); i++) {
    if(aEFMap.FindFromIndex(i).Extent() < 2)
      aFreeBoundaryMap.Add(aEFMap.FindKey(i));
  }
  aEFMap.Clear();
  
  for(i = (Standard_Integer)TopAbs_FACE; !myHasDeleted && (i < (Standard_Integer)TopAbs_EDGE); i++) {
    if(i == (Standard_Integer)TopAbs_WIRE)
      continue;

    TopAbs_ShapeEnum aType = (TopAbs_ShapeEnum) i;
    TopTools_IndexedMapOfShape aMap;
    TopExp::MapShapes(myResult, aType, aMap);

    TopExp_Explorer anExpObj(myS1, aType);

    for(; anExpObj.More(); anExpObj.Next()) {
      const TopoDS_Shape& aS = anExpObj.Current();

      if((i == (Standard_Integer)TopAbs_EDGE) && !aFreeBoundaryMap.Contains(aS))
	continue;

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
void BOP_ShellSolidHistoryCollector::FillSection(const BOPTools_PDSFiller& theDSFiller) 
{
  BOPTools_InterferencePool* pIntrPool = (BOPTools_InterferencePool*)&theDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs = pIntrPool->SSInterferences();
  Standard_Integer i = 0;
  TopTools_IndexedDataMapOfShapeListOfShape aResultMap;

  if(!myResult.IsNull()) {
    TopExp::MapShapesAndAncestors(myResult, TopAbs_EDGE, TopAbs_FACE, aResultMap);
  }

  for(i = 1; i <= aFFs.Extent(); i++) {
    BOPTools_SSInterference& aFF = aFFs(i);
    Standard_Integer nF1 = aFF.Index1();
    Standard_Integer nF2 = aFF.Index2();

    if(aFF.IsTangentFaces())
      continue;
    TopoDS_Shape aF1 = theDSFiller->DS().Shape(nF1);
    TopoDS_Shape aF2 = theDSFiller->DS().Shape(nF2);


    BOPTools_SequenceOfCurves& aSeqOfCurves = aFF.Curves();
    Standard_Integer j = 0;

    for(j = 1; j <= aSeqOfCurves.Length(); j++) {
      const BOPTools_Curve& aCurve = aSeqOfCurves.Value(j);

      const BOPTools_ListOfPaveBlock& aPBList = aCurve.NewPaveBlocks();
      BOPTools_ListIteratorOfListOfPaveBlock anIt(aPBList);

      for (; anIt.More();anIt.Next()) {
	const BOPTools_PaveBlock& aPB = anIt.Value();
	Standard_Integer anindex = aPB.Edge();
	const TopoDS_Shape& aS = theDSFiller->DS().GetShape(anindex);

	if(aResultMap.Contains(aS)) {
          TopTools_ListOfShape thelist;
	  if(!myGenMap.IsBound(aF1))
	    myGenMap.Bind(aF1, thelist);
	  myGenMap.ChangeFind(aF1).Append(aS);

	  if(!myGenMap.IsBound(aF2))
	    myGenMap.Bind(aF2, thelist);
	  myGenMap.ChangeFind(aF2).Append(aS);
	}
      }
    }

    // Old Section Edges
    const BOPTools_ListOfPaveBlock& aSectList = aFF.PaveBlocks();
    BOPTools_ListIteratorOfListOfPaveBlock anIt(aSectList);

    for (; anIt.More();anIt.Next()) {
      const BOPTools_PaveBlock& aPB=anIt.Value();
      Standard_Integer nSect = aPB.Edge();
      const TopoDS_Shape& aS = theDSFiller->DS().GetShape(nSect);

      if(aResultMap.Contains(aS)) {
	if(aResultMap.FindFromKey(aS).Extent() >= 2)
	  continue;

	Standard_Boolean add1 = Standard_True;	
        TopTools_ListOfShape thelist1;
	if(!myGenMap.IsBound(aF1))
	  myGenMap.Bind(aF1, thelist1);
	else {
	  TopTools_ListOfShape& aList = myGenMap.ChangeFind(aF1);
	  TopTools_ListIteratorOfListOfShape anItF1(aList);

	  for(; anItF1.More(); anItF1.Next()) {
	    if(aS.IsSame(anItF1.Value())) {
	      add1 = Standard_False;
	      break;
	    }
	  }
	}

	if(add1) {
	  myGenMap.ChangeFind(aF1).Append(aS);
	}
	Standard_Boolean add2 = Standard_True;	
        TopTools_ListOfShape thelist2;
	if(!myGenMap.IsBound(aF2))
	  myGenMap.Bind(aF2, thelist2);
	else {
	  TopTools_ListOfShape& aList = myGenMap.ChangeFind(aF2);
	  TopTools_ListIteratorOfListOfShape anItF2(aList);

	  for(; anItF2.More(); anItF2.Next()) {
	    if(aS.IsSame(anItF2.Value())) {
	      add2 = Standard_False;
	      break;
	    }
	  }
	}

	if(add2) {
	  myGenMap.ChangeFind(aF2).Append(aS);
	}
      }
    }
  }
  aResultMap.Clear();
  TopTools_IndexedMapOfShape aResultMapV;

  if(!myResult.IsNull()) {
    TopExp::MapShapes(myResult, TopAbs_VERTEX, aResultMapV);
  }

  for(i = 1; i <= aFFs.Extent(); i++) {
    BOPTools_SSInterference& aFF = aFFs(i);
    Standard_Integer nF1 = aFF.Index1();
    Standard_Integer nF2 = aFF.Index2();

    if(aFF.IsTangentFaces())
      continue;
    TopoDS_Shape aF1 = theDSFiller->DS().Shape(nF1);
    TopoDS_Shape aF2 = theDSFiller->DS().Shape(nF2);

    TColStd_ListIteratorOfListOfInteger anIt(aFF.AloneVertices());

    for(; anIt.More(); anIt.Next()) {
      TopoDS_Shape aNewVertex = theDSFiller->DS().Shape(anIt.Value());

      if(aResultMapV.Contains(aNewVertex)) {
        TopTools_ListOfShape thelist3;
	if(!myGenMap.IsBound(aF1))
	  myGenMap.Bind(aF1, thelist3);
	myGenMap.ChangeFind(aF1).Append(aNewVertex);

	if(!myGenMap.IsBound(aF2))
	  myGenMap.Bind(aF2, thelist3);
	myGenMap.ChangeFind(aF2).Append(aNewVertex);
      }
    }
  }
}

// ====================================================================================================
// function: FillEdgeHistory
// purpose:  
// ====================================================================================================
void BOP_ShellSolidHistoryCollector::FillEdgeHistory(const BOPTools_PDSFiller& theDSFiller) 
{
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller           = theDSFiller->PaveFiller();
  const BOPTools_SplitShapesPool& aSplitShapesPool = aPaveFiller.SplitShapesPool();
  TopTools_IndexedMapOfShape aResultMap, aFreeBoundaryMap;

  if(!myResult.IsNull()) {
    TopExp::MapShapes(myResult, TopAbs_EDGE, aResultMap);

    TopTools_IndexedDataMapOfShapeListOfShape aEFMap;

    if(myS1.ShapeType() == TopAbs_SHELL) {
      TopExp::MapShapesAndAncestors(myS1, TopAbs_EDGE, TopAbs_FACE, aEFMap);
    }
    else {
      TopExp::MapShapesAndAncestors(myS2, TopAbs_EDGE, TopAbs_FACE, aEFMap);
    }
    Standard_Integer i = 0;

    for(i = 1; i <= aEFMap.Extent(); i++) {
      if(aEFMap.FindFromIndex(i).Extent() < 2)
	aFreeBoundaryMap.Add(aEFMap.FindKey(i));
    }
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

      if(!aFreeBoundaryMap.Contains(anOldShape)) {
	continue;
      }

      if(!aLPB.IsEmpty()) {
	BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aLPB);

	for(; aPBIt.More(); aPBIt.Next()) {
	  const BOPTools_PaveBlock& aPB = aPBIt.Value();
	  Standard_Integer nSp = aPB.Edge();

	  if(nSp == i)
	    continue;
	  aState=aDS.GetState(nSp);

	  if (aState == aStateCmp) {
	    const TopoDS_Shape& aNewShape = aDS.Shape(nSp);

	    if(aResultMap.Contains(aNewShape)) {
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
      }
      //end if(!aLPB.IsEmpty...

      const BOPTools_CommonBlockPool& aCBPool = aPaveFiller.CommonBlockPool();
      const BOPTools_ListOfCommonBlock& aLCB = aCBPool(aDS.RefEdge(i));
      BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);

      for (; anItCB.More(); anItCB.Next()) {
	const BOPTools_CommonBlock& aCB = anItCB.Value();
	const BOPTools_PaveBlock& aPB = aCB.PaveBlock1();
	Standard_Integer nSp = aPB.Edge();
	TopoDS_Shape aNewShape = aDS.Shape(nSp);

	if(aResultMap.Contains(aNewShape)) {
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
      // end for (; anItCB.More...
    }
  }
}

