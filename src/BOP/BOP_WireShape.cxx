// File:	BOP_WireShape.cxx
// Created:	Mon Feb  4 12:02:05 2002
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <BOP_WireShape.ixx>

#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <BOPTools_DSFiller.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_CommonBlockPool.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>

#include <BOP_CorrectTolerances.hxx>
#include <BOP_BuilderTools.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_StateOfShape.hxx>

#include <BOP_BuilderTools.hxx>
#include <BOP_ConnexityBlock.hxx>
#include <BOP_ListOfConnexityBlock.hxx>
#include <BOP_ListIteratorOfListOfConnexityBlock.hxx>


static
  Standard_Integer InOrOut(const TopoDS_Vertex& ,
			   const TopoDS_Edge& );
static
  TopAbs_Orientation Orientation(const TopoDS_Vertex& ,
				 const TopoDS_Edge& );
static
  void OrientEdgesOnWire(const TopoDS_Wire& , 
			 TopoDS_Wire& );

//=======================================================================
// function: BOP_WireShape::BOP_WireShape
// purpose: 
//=======================================================================
  BOP_WireShape::BOP_WireShape()
{
}
//=======================================================================
// function: MakeResult
// purpose: 
//=======================================================================
  void BOP_WireShape::MakeResult()
{
  BRep_Builder aBB;
  TopoDS_Compound aCompound;

  aBB.MakeCompound(aCompound);
  
  TopoDS_Wire aWNew;
  BOP_ListOfConnexityBlock aLCB;
  BOP_BuilderTools::MakeConnexityBlocks(myLS, TopAbs_EDGE, aLCB);
  BOP_ListIteratorOfListOfConnexityBlock aLCBIt(aLCB);
  for (; aLCBIt.More(); aLCBIt.Next()) {
    const BOP_ConnexityBlock& aCB=aLCBIt.Value();
    const TopTools_ListOfShape& aLE=aCB.Shapes();
    TopoDS_Wire aW;
    aBB.MakeWire(aW);
    TopTools_ListIteratorOfListOfShape anIt(aLE);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Edge& aE=TopoDS::Edge(anIt.Value());
      aBB.Add(aW, aE);
    }
    OrientEdgesOnWire(aW, aWNew);
    aBB.Add(aCompound, aWNew);
  }
  myResult=aCompound;
}

//=======================================================================
// function: AddSplitPartsINOUT
// purpose: 
//=======================================================================
  void BOP_WireShape::AddSplitPartsINOUT()
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  const BOPTools_SplitShapesPool& aSplitShapesPool=aPaveFiller.SplitShapesPool();
  //
  Standard_Integer i, aNbPB, iRank, nSp, iBeg, iEnd;
  TopAbs_ShapeEnum aType, aTypeArg1, aTypeArg2;
  BooleanOperations_StateOfShape aState, aStateCmp;
  //
  aTypeArg1=aDS.Object().ShapeType();
  aTypeArg2=aDS.Tool().ShapeType();
  
  iBeg=1;
  iEnd=aDS.NumberOfShapesOfTheObject();

  if (aTypeArg1!=TopAbs_WIRE && aTypeArg2==TopAbs_WIRE) {
    iBeg=iEnd+1;
    iEnd=aDS.NumberOfSourceShapes();
  }
  else if (aTypeArg1==TopAbs_WIRE && aTypeArg2==TopAbs_WIRE){
    iBeg=1;
    iEnd=aDS.NumberOfSourceShapes();
  }
  //

  for (i=iBeg; i<=iEnd; ++i) {
    aType=aDS.GetShapeType(i);
    if (aType!=TopAbs_EDGE) {
      continue;
    }
    const TopoDS_Edge& aE=TopoDS::Edge(aDS.Shape(i));
    
    iRank=aDS.Rank(i);
    aStateCmp=BOP_BuilderTools::StateToCompare(iRank, myOperation);

    const BOPTools_ListOfPaveBlock& aLPB=aSplitShapesPool(aDS.RefEdge(i));
    aNbPB=aLPB.Extent();
    //
    if (!aNbPB) {
      aState=aDS.GetState(i);
      if (aState==aStateCmp) {
	myLS.Append(aE);
      }
    }
    //
    else {
      BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aLPB);
      for (; aPBIt.More(); aPBIt.Next()) {
	const BOPTools_PaveBlock& aPB=aPBIt.Value();
	nSp=aPB.Edge();
	const TopoDS_Edge& aSS=TopoDS::Edge(aDS.Shape(nSp));
	aState=aDS.GetState(nSp);
	if (aState==aStateCmp) {
	  myLS.Append(aSS);
	}
      }
    }
  }
}

//=======================================================================
// function: AddSplitPartsON
// purpose: 
//=======================================================================
  void BOP_WireShape::AddSplitPartsON()
{
  if (myOperation==BOP_CUT || myOperation==BOP_CUT21) {
    return;
  }

  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  const BOPTools_CommonBlockPool& aCommonBlockPool=aPaveFiller.CommonBlockPool();

  Standard_Integer i, aNbCB, nSpTaken, iBeg, iEnd;
  TopAbs_ShapeEnum aType, aTypeArg1, aTypeArg2;
  BOPTools_ListIteratorOfListOfCommonBlock anItCB;
  //
  aTypeArg1=aDS.Object().ShapeType();
  aTypeArg2=aDS.Tool().ShapeType();
  
  iBeg=1;
  iEnd=aDS.NumberOfShapesOfTheObject();
  if (aTypeArg1!=TopAbs_WIRE && aTypeArg2==TopAbs_WIRE) {
    iBeg=iEnd+1;
    iEnd=aDS.NumberOfSourceShapes();
  }
  else if (aTypeArg1==TopAbs_WIRE && aTypeArg2==TopAbs_WIRE){
    iEnd=aDS.NumberOfSourceShapes();
  }
  //
  for (i=iBeg; i<=iEnd; ++i) {
    aType=aDS.GetShapeType(i);
    if (aType!=TopAbs_EDGE) {
      continue;
    }
    //
    const BOPTools_ListOfCommonBlock& aLCB=aCommonBlockPool(aDS.RefEdge(i));
    aNbCB=aLCB.Extent();

    anItCB.Initialize(aLCB);
    for (; anItCB.More(); anItCB.Next()) {
      BOPTools_CommonBlock& aCB=anItCB.Value();
      const BOPTools_PaveBlock& aPB=aCB.PaveBlock1();
      nSpTaken=aPB.Edge();
      const TopoDS_Edge& aSS=TopoDS::Edge(aDS.Shape(nSpTaken));
      myLS.Append(aSS);
    }
  }
}

//=======================================================================
// function: OrientEdgesOnWire
// purpose: 
//=======================================================================
void OrientEdgesOnWire(const TopoDS_Wire& aWire, 
		       TopoDS_Wire& aWireNew)
{
  Standard_Integer i, aNbV, aNbE, j, iCnt, iInOrOut, aNbRest;

  TopTools_IndexedDataMapOfShapeListOfShape aVEMap;
  TopTools_IndexedMapOfShape aProcessedEdges, aRestEdges, aEMap;
  TopTools_ListIteratorOfListOfShape anIt;
  BRep_Builder aBB;

  aBB.MakeWire(aWireNew);
  
  TopExp::MapShapesAndAncestors(aWire, TopAbs_VERTEX, TopAbs_EDGE, aVEMap);
  
  aNbV=aVEMap.Extent();
  //
  // Do
  for (i=1; i<=aNbV; i++) {
    const TopoDS_Vertex& aV=TopoDS::Vertex(aVEMap.FindKey(i));
    
    const TopTools_ListOfShape& aLE=aVEMap.FindFromIndex(i);
    aNbE=aLE.Extent();
    
    if (aNbE>=2) {
      iCnt=0;
      anIt.Initialize(aLE);
      for(; anIt.More(); anIt.Next()) {
	const TopoDS_Edge& aE=TopoDS::Edge(anIt.Value());
	if (aProcessedEdges.Contains(aE)) {
	  iInOrOut=InOrOut(aV, aE);
	  iCnt+=iInOrOut;
	}
	else {
	  aRestEdges.Add(aE);
	}
      }

      TopoDS_Edge* pE;
      aNbRest=aRestEdges.Extent();
      for (j=1; j<=aNbRest; j++) {
	const TopoDS_Edge& aE=TopoDS::Edge(aRestEdges(j));
	pE=(TopoDS_Edge*)&aE;
	
	iInOrOut=InOrOut(aV, aE);
	if (iCnt>0) {
	  if (iInOrOut>0) {
	    pE->Reverse();
	  }
	  --iCnt;
	}
	else if (iCnt<=0){
	  if (iInOrOut<0) {
	    pE->Reverse();
	  }
	  ++iCnt;
	}
	aProcessedEdges.Add(*pE);
      }
    }//if (aNbE>=2) 
  }
  //
  //
  aNbE=aProcessedEdges.Extent();
  for (i=1; i<=aNbE; i++) {
    const TopoDS_Edge& aE=TopoDS::Edge(aProcessedEdges(i));
    aBB.Add(aWireNew, aE);
  }

  TopExp::MapShapes(aWire, TopAbs_EDGE, aEMap);
  
  aNbE=aEMap.Extent();
  for (i=1; i<=aNbE; i++) {
    const TopoDS_Edge& aE=TopoDS::Edge(aEMap(i));
    if (!aProcessedEdges.Contains(aE)) {
      aProcessedEdges.Add(aE);
      aBB.Add(aWireNew, aE);
    }
  }
}

//=======================================================================
//function : Orientation
//purpose  :
//=======================================================================
  TopAbs_Orientation Orientation(const TopoDS_Vertex& aV,
				 const TopoDS_Edge& aE)
{
  TopAbs_Orientation anOr=TopAbs_INTERNAL;

  TopExp_Explorer anExp;
  anExp.Init(aE, TopAbs_VERTEX);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Vertex& aVE1=TopoDS::Vertex(anExp.Current());
    if (aVE1.IsSame(aV)) {
      anOr=aVE1.Orientation();
      break;
    }
  }
  return anOr;
}

///=======================================================================
//function : InOrOut
//purpose  :
//=======================================================================
  Standard_Integer InOrOut(const TopoDS_Vertex& aV,
			   const TopoDS_Edge& aE)
{
  TopAbs_Orientation anOrV, anOrE;
  anOrV=aV.Orientation();
  anOrE=aE.Orientation();
  if (anOrV==TopAbs_INTERNAL){
    return 0;
  }
  
  anOrV=Orientation(aV, aE);
  
  if (anOrV==anOrE) {
    return -1; // escape
  }
  else {
    return 1; // entry
  }
}
