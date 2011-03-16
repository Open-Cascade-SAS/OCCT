// File:	BOP_Builder.cxx
// Created:	Thu Mar 29 11:12:53 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOP_Builder.ixx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>

#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_CommonBlockPool.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>

//=======================================================================
// function: BOP_Builder::BOP_Builder
// purpose: 
//=======================================================================
  BOP_Builder::BOP_Builder() 
: myOperation(BOP_UNKNOWN),
  myManifoldFlag(Standard_False),
  myIsDone(Standard_False),
  myErrorStatus(1),
  myDraw(0)
{
}
//=======================================================================
// function: Destroy
// purpose: 
//=======================================================================
  void BOP_Builder::Destroy()
{
}

//=======================================================================
// function: SetShapes
// purpose: 
//=======================================================================
  void BOP_Builder::SetShapes (const TopoDS_Shape& aS1,
			       const TopoDS_Shape& aS2)
{
  myShape1=aS1;
  myShape2=aS2;
}
//=======================================================================
// function: SetOperation
// purpose: 
//=======================================================================
  void BOP_Builder::SetOperation (const BOP_Operation anOp) 
{
  myOperation=anOp;
}
	 

//=======================================================================
// function: SetManifoldFlag
// purpose: 
//=======================================================================
  void BOP_Builder::SetManifoldFlag (const Standard_Boolean aFlag)
{
  myManifoldFlag=aFlag;
}
//=======================================================================
// function: Shape1
// purpose: 
//=======================================================================
  const TopoDS_Shape& BOP_Builder::Shape1()const
{
  return myShape1;
}

//=======================================================================
// function: Shape2
// purpose: 
//=======================================================================
  const TopoDS_Shape& BOP_Builder::Shape2()const
{
  return myShape2;
}
//=======================================================================
// function: Operation
// purpose: 
//=======================================================================
  BOP_Operation BOP_Builder::Operation () const
{
  return myOperation;
}
	
//=======================================================================
// function: ManifoldFlag
// purpose: 
//=======================================================================
  Standard_Boolean BOP_Builder::ManifoldFlag () const 
{
  return myManifoldFlag;
}
//=======================================================================
// function: IsDone
// purpose: 
//=======================================================================
  Standard_Boolean BOP_Builder::IsDone() const 
{
  return myIsDone;
}
//=======================================================================
// function: ErrorStatus
// purpose: 
//=======================================================================
  Standard_Integer BOP_Builder::ErrorStatus() const 
{
  return myErrorStatus;
}
//=======================================================================
// function: Result
// purpose: 
//=======================================================================
  const TopoDS_Shape& BOP_Builder::Result()const
{
  return myResult;
}

//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_Builder::Do()
{
}
//=======================================================================
// function: DoDoWithFiller
// purpose: 
//=======================================================================
  void BOP_Builder::DoWithFiller(const BOPTools_DSFiller& )
{
} 

//=======================================================================
// function: BuildResult
// purpose: 
//=======================================================================
  void BOP_Builder::BuildResult()
{
}
//
//
//=======================================================================
// function: FillModified
// purpose: 
//=======================================================================
  void BOP_Builder::FillModified()
{
  //
  // Prepare myResultMap
  myResultMap.Clear();
  TopExp::MapShapes(myResult, TopAbs_FACE, myResultMap);
  TopExp::MapShapes(myResult, TopAbs_EDGE, myResultMap);
  //
  // Fill Modified for Edges
  Standard_Integer i, aNbSources, aNbPaveBlocks, nSp, nFace;
  //
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_SplitShapesPool& aSplitShapesPool= myDSFiller->SplitShapesPool();
  const BOPTools_CommonBlockPool& aCommonBlockPool= myDSFiller->CommonBlockPool();
  //
  aNbSources=aDS.NumberOfSourceShapes();
  for (i=1; i<=aNbSources; i++) {
    
    if (aDS.GetShapeType(i) != TopAbs_EDGE){
      continue;
    }
    //
    const TopoDS_Shape& aE=aDS.Shape(i);
    //
    const BOPTools_ListOfPaveBlock& aSplitEdges=aSplitShapesPool(aDS.RefEdge(i));
    const BOPTools_ListOfCommonBlock& aCBlocks =aCommonBlockPool(aDS.RefEdge(i));
    //
    aNbPaveBlocks=aSplitEdges.Extent();
    if (!aNbPaveBlocks) {
      continue;
    }
    //
    TopTools_IndexedMapOfShape aM;
    
    BOPTools_ListIteratorOfListOfCommonBlock aCBIt(aCBlocks);
    for (; aCBIt.More(); aCBIt.Next()) {
      BOPTools_CommonBlock& aCB=aCBIt.Value();
      //
      nFace=aCB.Face();
      //
      if (nFace) {
	BOPTools_PaveBlock& aPB=aCB.PaveBlock1(i);
	nSp=aPB.Edge();
	const TopoDS_Shape& aSp=aDS.Shape(nSp);
	//
	if (aM.Contains(aSp)) {
	  continue;
	}
	aM.Add(aSp);
	//
	if (myResultMap.Contains(aSp)) {
	  FillModified(aE, aSp);
	}
      }
      //
      else {
	const BOPTools_PaveBlock& aPB=aCB.PaveBlock1();
	nSp=aPB.Edge();
	const TopoDS_Shape& aSp=aDS.Shape(nSp);
	//
	if (aM.Contains(aSp)) {
	  continue;
	}
	aM.Add(aSp);
	//
	if (myResultMap.Contains(aSp)) {
	  FillModified(aE, aSp);
	}
      }
    } // for (; aCBIt.More(); aCBIt.Next())
    
    
    BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSplitEdges);
    for (; aPBIt.More(); aPBIt.Next()) {
      const BOPTools_PaveBlock& aPB=aPBIt.Value();
      nSp=aPB.Edge();
      const TopoDS_Shape& aSp=aDS.Shape(nSp);
      //
      if (aM.Contains(aSp)) {
	continue;
      }
      aM.Add(aSp);
      //
      if (myResultMap.Contains(aSp)) {
	FillModified(aE, aSp);
      }
    }// for (; aPBIt.More(); aPBIt.Next())
  }
}
//=======================================================================
// function: FillModified
// purpose: 
//=======================================================================
  void BOP_Builder::FillModified(const TopoDS_Shape& aE,
				 const TopoDS_Shape& aSp)
{
  if (myModifiedMap.Contains(aE)) {
    TopTools_ListOfShape& aLM=myModifiedMap.ChangeFromKey(aE);
    aLM.Append(aSp);
  }
  else {
    TopTools_ListOfShape aLM;
    aLM.Append(aSp);
    myModifiedMap.Add(aE, aLM);
  }
}
//=======================================================================
// function: FillModified
// purpose: 
//=======================================================================
  void BOP_Builder::FillModified(const TopoDS_Shape& aS,
				 const TopTools_ListOfShape& aLFx)
{
  TopTools_ListIteratorOfListOfShape anIt(aLFx);
  //
  if (myModifiedMap.Contains(aS)) {
    TopTools_ListOfShape& aLM=myModifiedMap.ChangeFromKey(aS);
    anIt.Initialize(aLFx);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aFx=anIt.Value();
      aLM.Append(aFx);
    }
  }
  
  else {
    TopTools_ListOfShape aLM;
    anIt.Initialize(aLFx);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aFx=anIt.Value();
      aLM.Append(aFx);
    }
    myModifiedMap.Add(aS, aLM);
  }
}
//=======================================================================
// function: Modified
// purpose: 
//=======================================================================
  const TopTools_ListOfShape& BOP_Builder::Modified(const TopoDS_Shape& aS)const
{
  if (myModifiedMap.Contains(aS)) {
    const TopTools_ListOfShape& aLM=myModifiedMap.FindFromKey(aS);
    return aLM;
  }
  else {
    return myEmptyList;
  }
}
//=======================================================================
// function: IsDeleted
// purpose: 
//=======================================================================
  Standard_Boolean BOP_Builder::IsDeleted(const TopoDS_Shape& aS)const
{
  Standard_Boolean bFlag=Standard_False;
  //
  if (myResultMap.Contains(aS)) {
    return bFlag;
  }
  //
  const TopTools_ListOfShape& aLM=Modified(aS);
  if (aLM.Extent()) {
    return bFlag;
  }
  //
  return !bFlag;
}


//=======================================================================
//function : SortTypes
//purpose  : 
//=======================================================================
  void BOP_Builder::SortTypes(TopAbs_ShapeEnum& aType1,
			      TopAbs_ShapeEnum& aType2)
{ 
  Standard_Integer iT1, iT2;

  if (aType1==aType2)
    return;
  
  iT1=(Standard_Integer) aType1;
  iT2=(Standard_Integer) aType2;
  
  if (iT1 < iT2) {
    aType1=(TopAbs_ShapeEnum) iT2;
    aType2=(TopAbs_ShapeEnum) iT1;
  }
}

//=======================================================================
// function: SectionEdges
// purpose: 
//=======================================================================
  const TopTools_ListOfShape& BOP_Builder::SectionEdges()const
{
  return mySectionEdges;
}


//=======================================================================
// function: SetHistoryCollector
// purpose: 
//=======================================================================
void BOP_Builder::SetHistoryCollector(const Handle(BOP_HistoryCollector)& theHistory) 
{
  myHistory = theHistory;
}

//=======================================================================
// function: GetHistoryCollector
// purpose: 
//=======================================================================
Handle(BOP_HistoryCollector) BOP_Builder::GetHistoryCollector() const
{
  return myHistory;
}
