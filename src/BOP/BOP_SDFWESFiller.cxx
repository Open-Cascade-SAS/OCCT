// File:	BOP_SDFWESFiller.cxx
// Created:	Wed Jun  6 12:48:58 2001
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <BOP_SDFWESFiller.ixx>

#include <gp_Pnt.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>

#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopAbs_Orientation.hxx>

#include <BRep_Tool.hxx>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_StateOfShape.hxx>

#include <IntTools_Tools.hxx>
#include <IntTools_Context.hxx>

#include <BOPTools_Tools2D.hxx>
#include <BOPTools_Tools3D.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_CommonBlockPool.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_Tools.hxx>
#include <BOPTools_PointBetween.hxx>
#include <BOPTools_SolidStateFiller.hxx>

#include <BOPTColStd_Dump.hxx>
#include <TopTools_MapOfShape.hxx>
//
static Standard_Boolean IsValidSeam(const TopoDS_Edge& aE,
				    const TopoDS_Face& aF,
				    const Standard_Real aT,
				    IntTools_Context& aContext);

static void CorrespondantSeam(const TopoDS_Edge& aSpE1Seam11,
			      const Standard_Real aT1,
			      const TopoDS_Edge& aSpE1Seam21,
			      const TopoDS_Edge& aSpE1Seam22,
			      const Standard_Real aT2,
			      const TopoDS_Face& aF1FWD,
			      TopoDS_Edge& aSS);

static void TreatSDSeams (const TopoDS_Edge& aSpE1Seam11,
			  const Standard_Real aT1,
			  const TopoDS_Face& aF1FWD,
			  const TopoDS_Edge& aSpE1Seam21,
			  const Standard_Real aT2,
			  const TopoDS_Face& aF2FWD,
			  const Standard_Boolean bIsTakenSp1,
			  BOP_WireEdgeSet& aWES,
			  IntTools_Context& aContext);


//=======================================================================
// function: BOP_SDFWESFiller::BOP_SDFWESFiller
// purpose: 
//=======================================================================
  BOP_SDFWESFiller::BOP_SDFWESFiller()
:
  myDSFiller(NULL),
  myOperation(BOP_UNKNOWN),
  myNF1(0),
  myNF2(0),
  mySenseFlag(0)
{}

//=======================================================================
// function: BOP_SDFWESFiller::BOP_SDFWESFiller
// purpose: 
//=======================================================================
  BOP_SDFWESFiller::BOP_SDFWESFiller(const Standard_Integer nF1,
				     const Standard_Integer nF2,
				     const BOPTools_DSFiller& aDSFiller)
:
  myNF1(nF1),
  myNF2(nF2),
  mySenseFlag(0)
{
  myDSFiller=(BOPTools_DSFiller*) &aDSFiller;
  Prepare();
}
//=======================================================================
// function: SetStatesMap
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::SetStatesMap (const BOPTools_IndexedDataMapOfIntegerState& aMap)
{
  myStatesMap.Clear();
  myStatesMap=aMap;
}
//=======================================================================
// function: StatesMap
// purpose: 
//=======================================================================
  const BOPTools_IndexedDataMapOfIntegerState& BOP_SDFWESFiller::StatesMap ()const
{
  return myStatesMap;
}

//=======================================================================
// function: SetFaces
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::SetFaces (const Standard_Integer nF1,
				   const Standard_Integer nF2)
{
   myNF1=nF1;
   myNF2=nF2;
}
//=======================================================================
// function: SetSenseFlag
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::SetSenseFlag (const Standard_Integer iFlag)
				 
{
   mySenseFlag=iFlag;
}

//=======================================================================
// function: SenseFlag
// purpose: 
//=======================================================================
  Standard_Integer BOP_SDFWESFiller::SenseFlag () const
				 
{
   return mySenseFlag;
}

//=======================================================================
// function: Faces
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::Faces (Standard_Integer& nF1,
				Standard_Integer& nF2) const
{
   nF1=myNF1;
   nF2=myNF2;
}
//=======================================================================
// function: SetDSFiller
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::SetDSFiller(const BOPTools_DSFiller& aDSFiller)
{
   myDSFiller=(BOPTools_DSFiller*) &aDSFiller;
}

//=======================================================================
// function: DSFiller
// purpose: 
//=======================================================================
  const BOPTools_DSFiller& BOP_SDFWESFiller::DSFiller()const
{
  return *myDSFiller;
}

//=======================================================================
// function: SetOperation
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::SetOperation(const BOP_Operation anOp)
{
  myOperation=anOp;
}
//=======================================================================
// function: Operation
// purpose: 
//=======================================================================
  BOP_Operation BOP_SDFWESFiller::Operation()const
{
  return myOperation;
}
//xf
//=======================================================================
//function : RejectedOnParts
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& BOP_SDFWESFiller::RejectedOnParts()const
{
  return myRejectedOnParts;
}
//xt
//=======================================================================
// function: Prepare
// purpose: 
//======================================================================= 
  void BOP_SDFWESFiller::Prepare()
{
  if (!myNF1 || !myNF2) {
    return;
  }
  //
  // 1. Prepare States 2D for the Faces' entities  (myStatesMap)
  AssignStates(myNF1, myNF2);
  AssignStates(myNF2, myNF1);
  //
  AssignDEStates(myNF1, myNF2);
  AssignDEStates(myNF2, myNF1);
  //
  //  
  // 2.
  PrepareOnParts();
  //
}

//=======================================================================
// function: Do
// purpose: 
//======================================================================= 
  void BOP_SDFWESFiller::Do(const BOP_WireEdgeSet& pWES)
{
  
  myWES=(BOP_WireEdgeSet*) &pWES;

  if (!myNF1 || !myNF2) {
    return;
  }
  
  //
  // WES
  switch (myOperation) {
    case BOP_COMMON: {
      PrepareWESForZone (myNF1, myNF2);
      break;
    }
    case BOP_CUT: {
      PrepareWESForCut (myNF1, myNF2);
      break;
    }
    case BOP_CUT21: {
      PrepareWESForCut (myNF2, myNF1);
      break;
    }
    default: {
      return;
    }
  }
}

//=======================================================================
// function: AssignStates
// purpose: 
//======================================================================= 
  void BOP_SDFWESFiller::AssignStates(const Standard_Integer nF1,
				      const Standard_Integer nF2) 
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*) &aPaveFiller;
  const BOPTools_SplitShapesPool& aSplitShapesPool=aPaveFiller.SplitShapesPool();
  //
  Standard_Integer aBid=0, nSplit, nE, nW, nSp, anEdgeFlag, aWireFlag, aNbPB, iRankF1;
  TColStd_ListOfInteger aList1IN2, aList1ON2;
  TColStd_IndexedMapOfInteger aSpMapIN, aSpMapON;
  TColStd_ListIteratorOfListOfInteger anIt;
  //
  iRankF1=aDS.Rank(nF1);
  //
  const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
  // 
  //  Splits that are from nF2 and are IN2D for nF2
  pPaveFiller->SplitsInFace (aBid, nF1, nF2, aList1IN2);
  
  anIt.Initialize(aList1IN2);
  for (; anIt.More();  anIt.Next()) {
    nSplit=anIt.Value();
    aSpMapIN.Add(nSplit);
  }
  //
  // that are from nF2 and are ON2D for nF2
  pPaveFiller->SplitsOnFace (aBid, nF1, nF2, aList1ON2);
  
  anIt.Initialize(aList1ON2);
  for (; anIt.More();  anIt.Next()) {
    nSplit=anIt.Value();
    aSpMapON.Add(nSplit);
  }
  //
  // Treatment of the Face's entities
  aWireFlag=1;
  TopExp_Explorer anExpWire(aF1, TopAbs_WIRE);
  for (; anExpWire.More(); anExpWire.Next()) {
    const TopoDS_Shape& aWire=anExpWire.Current();
    nW=aDS.ShapeIndex(aWire, iRankF1);
    anEdgeFlag=1;

    TopExp_Explorer anExpEdge (aWire, TopAbs_EDGE);
    for (; anExpEdge.More(); anExpEdge.Next()) {
      const TopoDS_Shape& anEdge=anExpEdge.Current();
      nE=aDS.ShapeIndex(anEdge, iRankF1);
      
      const BOPTools_ListOfPaveBlock& aLPB=aSplitShapesPool(aDS.RefEdge(nE));

      aNbPB=aLPB.Extent();
      if (!aNbPB) {
	// the whole edge is OUT
	myStatesMap.Add(nE, BooleanOperations_OUT);
	continue;
      }

      BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aLPB);

      for (;  aPBIt.More();  aPBIt.Next()) {
	const BOPTools_PaveBlock& aPB=aPBIt.Value();
	nSp=aPB.Edge();
	
	if (aSpMapIN.Contains(nSp)) {// IN
	  myStatesMap.Add(nSp, BooleanOperations_IN);
	  anEdgeFlag=0;
	}
	else if (aSpMapON.Contains(nSp)) {// ON
	  myStatesMap.Add(nSp, BooleanOperations_ON);
	  anEdgeFlag=0;
	}
	else {// if (nSp!=nE) {// OUT
	  myStatesMap.Add(nSp, BooleanOperations_OUT);
	}
      } 
    } //  enf of for (; anExpEdge.More(); anExpEdge.Next()) 
     
    if (anEdgeFlag) {// all Wire is out
      myStatesMap.Add(nW, BooleanOperations_OUT);
    }
    else {
      aWireFlag=0;
    }
  } // enf of for (; anExpEdge.More(); anExpEdge.Next())
  
  if (aWireFlag) { // all Face is out of nF2
    myStatesMap.Add(nF1, BooleanOperations_OUT);
  }
}

//=======================================================================
// function: PrepareOnParts
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::PrepareOnParts ()
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*) &aPaveFiller;
  BOPTools_CommonBlockPool& aCBPool=pPaveFiller->ChangeCommonBlockPool();
  //
  Standard_Integer aBid=0, nE1, nE2, aNbSpON, nSp1, nSp2, iRankF1;
  Standard_Real aT1, aT2, aT, aTs; /*, U, V;*/ 
  Standard_Boolean aFlag;
  TColStd_ListOfInteger aLs;
  TColStd_IndexedMapOfInteger aMap;
  TopExp_Explorer anExpF1, anExpF2;
  //
  iRankF1=aDS.Rank(myNF1);

  gp_Pnt aPx1;
  //
  TopoDS_Face aF1FWD, aF2FWD;
  PrepareFaces(myNF1, myNF2, aF1FWD, aF2FWD);
  //
  // Process 
  anExpF1.Init(aF1FWD, TopAbs_EDGE);
  for (; anExpF1.More(); anExpF1.Next()) {
    const TopoDS_Edge& anE1=TopoDS::Edge(anExpF1.Current());
    //
    if (BRep_Tool::Degenerated(anE1)){
      continue;
    }
    //
    nE1=aDS.ShapeIndex(anE1, iRankF1);

    aLs.Clear();
    pPaveFiller->SplitsOnFace(nE1, myNF2, aLs);
    
    aNbSpON=aLs.Extent();
    if (!aNbSpON) {
      continue;
    }

    aMap.Clear();
    TColStd_ListIteratorOfListOfInteger anItLs(aLs);
    for (; anItLs.More(); anItLs.Next()) {
      aBid=anItLs.Value();
      aMap.Add(aBid);
    }

    BOPTools_ListOfCommonBlock& aLCB=aCBPool(aDS.RefEdge(nE1));
    BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
    for (; anItCB.More(); anItCB.Next()) {
      BOPTools_CommonBlock& aCB=anItCB.Value();
      BOPTools_PaveBlock& aPB1=aCB.PaveBlock1(nE1);
      nSp1=aPB1.Edge();
      if (aMap.Contains(nSp1)) {
	//
	// aPB1
	aPB1.Parameters(aT1, aT2);
	aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
	BOPTools_Tools::PointOnEdge(anE1, aT, aPx1);

	BOPTools_PointBetween aPointBetween;
	aPointBetween.SetParameter(aT);
	aPointBetween.SetPnt(aPx1);
	
	aPB1.SetPointBetween(aPointBetween);
	//
	// aPB2
	BOPTools_PaveBlock& aPB2=aCB.PaveBlock2(nE1);
	nE2=aPB2.OriginalEdge();
	nSp2=aPB2.Edge();
	const TopoDS_Edge& anE2=TopoDS::Edge(aDS.GetShape(nE2));
	//
	IntTools_Context& aContext=pPaveFiller->ChangeContext();
	aFlag=aContext.ProjectPointOnEdge(aPx1, anE2, aTs);
	//
	if (!aFlag) {
	  BOPTColStd_Dump::PrintMessage(" BOP_SDFWESFiller::PrepareOnParts() failed\n");
	  return;
	}

	aPointBetween.SetParameter(aTs);
	aPointBetween.SetPnt(aPx1);
	
	aPB2.SetPointBetween(aPointBetween);
	//
	BOPTools_ListOfCommonBlock& aLCB2=aCBPool(aDS.RefEdge(nE2));
	BOPTools_ListIteratorOfListOfCommonBlock anItCB2(aLCB2);
	for (; anItCB2.More(); anItCB2.Next()){
	  BOPTools_CommonBlock& aCB2=anItCB2.Value();
	  BOPTools_PaveBlock& aPB21=aCB2.PaveBlock1(nE2);
	  BOPTools_PaveBlock& aPB22=aCB2.PaveBlock2(nE2);
	  
	  if ((aPB21.IsEqual(aPB1) && aPB22.IsEqual(aPB2)) 
	      ||
	      (aPB21.IsEqual(aPB2) && aPB22.IsEqual(aPB1))) {
	    
	    aPointBetween.SetPnt(aPx1);
	    
	    aPointBetween.SetParameter(aTs);
	    aPB21.SetPointBetween(aPointBetween);
	    
	    aPointBetween.SetParameter(aT);
	    aPB22.SetPointBetween(aPointBetween);
	    
	    break;
	  }
	}
	//
      }
    }
  }
}

//=======================================================================
// function: PrepareWESForZone
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::PrepareWESForZone (const Standard_Integer nF1, 
					    const Standard_Integer nF2)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  const BOPTools_SplitShapesPool& aSplitShapesPool=aPaveFiller.SplitShapesPool();
  BOP_WireEdgeSet& aWES=*myWES;
  //
  Standard_Integer nE, nSp, iRankF1, iRankF2; 
  
  TopAbs_Orientation anOr;
  BooleanOperations_StateOfShape aState;
  TopTools_IndexedMapOfOrientedShape aMap;
  //
  iRankF1=aDS.Rank(nF1);
  iRankF2=aDS.Rank(nF2);
  // W E S 
  TopoDS_Face aF1FWD, aF2FWD; 
  PrepareFaces(nF1, nF2, aF1FWD, aF2FWD);

  TopExp_Explorer anExp;
  //
  // aF1FWD
  anExp.Init(aF1FWD, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape& anE=anExp.Current();
    anOr=anE.Orientation();

    nE=aDS.ShapeIndex(anE, iRankF1);

    const BOPTools_ListOfPaveBlock& aLPB=aSplitShapesPool(aDS.RefEdge(nE));
    BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aLPB);
    for (; aPBIt.More(); aPBIt.Next()) {
      const BOPTools_PaveBlock& aPB=aPBIt.Value();
      nSp=aPB.Edge();
      //>>>
      if (!myStatesMap.Contains(nSp)) {
	continue;
      }
      //>>>
      aState=myStatesMap.FindFromKey(nSp);
      
      if (aState==BooleanOperations_IN) {
	
	const TopoDS_Shape& aSplit=aDS.Shape(nSp);
	TopoDS_Edge aSS=TopoDS::Edge(aSplit);
	aSS.Orientation(anOr);
	//
	if (aMap.Contains(aSS)) {
	  continue;
	}
	//
	aWES.AddStartElement (aSS);
	aMap.Add(aSS);  

	if (BRep_Tool::IsClosed(aSS, aF1FWD)){
	  TopoDS_Shape EE=aSS.Reversed();
	  aWES.AddStartElement (EE);
	  aMap.Add(EE);
	}
      }
    }
  }
  //
  // aF2FWD
  aMap.Clear();
  anExp.Init(aF2FWD, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape& anE=anExp.Current();

    anOr=anE.Orientation();

    nE=aDS.ShapeIndex(anE, iRankF2);

    const BOPTools_ListOfPaveBlock& aLPB=aSplitShapesPool(aDS.RefEdge(nE));
    
    BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aLPB);
    for (; aPBIt.More(); aPBIt.Next()) {
      const BOPTools_PaveBlock& aPB=aPBIt.Value();
      nSp=aPB.Edge();
      //>>>
      if (!myStatesMap.Contains(nSp)) {
	continue;
      }
      //>>>
      aState=myStatesMap.FindFromKey(nSp);
      
      if (aState==BooleanOperations_IN) {
	const TopoDS_Shape& aSplit=aDS.Shape(nSp);
	TopoDS_Edge aSS=TopoDS::Edge(aSplit);
	//
	if (!BOPTools_Tools2D::HasCurveOnSurface(aSS, aF1FWD)) {
	  continue;
	}
	//
	aSS.Orientation(anOr);
	//
	if (aMap.Contains(aSS)) {
	  continue;
	}
	//	
	aWES.AddStartElement (aSS);
	aMap.Add(aSS);

	if (BRep_Tool::IsClosed(aSS, aF2FWD)){
	  TopoDS_Shape EE=aSS.Reversed();
	  aWES.AddStartElement (EE);
	  aMap.Add(EE);
	}
      }
    }
  }

  PrepareOnParts(nF1, nF2, BOP_COMMON);
}

//=======================================================================
// function: PrepareWESForCut
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::PrepareWESForCut (const Standard_Integer nF1, 
					   const Standard_Integer nF2)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  const BOPTools_SplitShapesPool& aSplitShapesPool=aPaveFiller.SplitShapesPool();
  BOP_WireEdgeSet& aWES=*myWES;
  //
  Standard_Integer  nE, nSp, nPB, iRankF2;
  TopAbs_Orientation anOr;
  BooleanOperations_StateOfShape aState;

  iRankF2=aDS.Rank(nF2);
  //
  // W E S 
  TopoDS_Face aF1FWD, aF2FWD; 
  PrepareFaces(nF1, nF2, aF1FWD, aF2FWD);

  aF2FWD.Reverse();
  
  TopExp_Explorer anExp;
  //
  //  aF2FWD
  anExp.Init(aF2FWD, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape& anE=anExp.Current();
    anOr=anE.Orientation();

    nE=aDS.ShapeIndex(anE, iRankF2);

    const BOPTools_ListOfPaveBlock& aLPB=aSplitShapesPool(aDS.RefEdge(nE));

    nPB=aLPB.Extent();
    if (nPB) {
      BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aLPB);
      for (; aPBIt.More(); aPBIt.Next()) {
	const BOPTools_PaveBlock& aPB=aPBIt.Value();
	nSp=aPB.Edge();
	//>>>
	if (!myStatesMap.Contains(nSp)) {
	  continue;
	}
	//>>>
	aState=myStatesMap.FindFromKey(nSp);
	
	if (aState==BooleanOperations_IN) {
	  const TopoDS_Shape& aSplit=aDS.Shape(nSp);
	  TopoDS_Edge aSS=TopoDS::Edge(aSplit);
	  //
	  if (!BOPTools_Tools2D::HasCurveOnSurface(aSS, aF1FWD)) {
	    continue;
	  }
	  //
	  aSS.Orientation(anOr);
	  //
	  aWES.AddStartElement (aSS);
	  //
	  if (BRep_Tool::IsClosed(aSS, aF2FWD)){
	    TopoDS_Shape EE=aSS.Reversed();
	    aWES.AddStartElement (EE);
	  }
	}
      }
    }
    else {
      //>>>
      if (!myStatesMap.Contains(nE)) {
	continue;
      }
      //>>>
      aState=myStatesMap.FindFromKey(nE);
      if (aState==BooleanOperations_IN) {
	TopoDS_Edge aSS=TopoDS::Edge(anE);
	//
	aWES.AddStartElement (aSS);
      }
    }
  } // end of for (; anExp.More(); anExp.Next()) {
  
  PrepareOnParts(nF1, nF2, BOP_CUT);
}
//=======================================================================
// function: PrepareOnParts
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::PrepareOnParts (const Standard_Integer nF1, 
					 const Standard_Integer nF2,
					 const BOP_Operation anOperation)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*) &aPaveFiller;
  BOPTools_CommonBlockPool& aCBPool=pPaveFiller->ChangeCommonBlockPool();
  BOP_WireEdgeSet& aWES=*myWES;
  //
  IntTools_Context& aContext=pPaveFiller->ChangeContext();
  //
  Standard_Integer nE1, nE2, aNbSpON, nSp1, aBid, nSpTaken, nSp2, iRankF1, iRankF2;
  Standard_Real aT1, aT2, aU, aV, aScPr;
  Standard_Boolean bToReverse, bInternal1, bInternal2, bAdded;
  
  TopoDS_Edge aSS, aSSx;
  TopoDS_Face aF1FWD, aF2FWD;
  TopAbs_Orientation anOr, anOr2;

  TColStd_ListOfInteger aLs;
  TColStd_IndexedMapOfInteger aMap;
  TopTools_IndexedMapOfShape aM;
  TopTools_MapOfShape aMFence;//xft
  //
  gp_Vec aN1, aN2, aTau1, aTau2, aBiN1, aBiN2;
  TopExp_Explorer anExpF1, anExpF2;
  //
  // Source faces
  iRankF1=aDS.Rank(nF1);
  iRankF2=aDS.Rank(nF2);
  //
  PrepareFaces(nF1, nF2, aF1FWD, aF2FWD);
  //
  myRejectedOnParts.Clear();//xft
  //
  anExpF1.Init(aF1FWD, TopAbs_EDGE);
  for (; anExpF1.More(); anExpF1.Next()) {
    const TopoDS_Edge& anE1=TopoDS::Edge(anExpF1.Current());
    anOr=anE1.Orientation();
    //
    if (BRep_Tool::Degenerated(anE1)){
      continue;
    }
    //
    nE1=aDS.ShapeIndex(anE1, iRankF1); 

    anExpF2.Init(aF2FWD, TopAbs_EDGE);
    for (; anExpF2.More(); anExpF2.Next()) {
      const TopoDS_Edge& anE2=TopoDS::Edge(anExpF2.Current());
      anOr2=anE2.Orientation();
      //
      if (BRep_Tool::Degenerated(anE2)){
	continue;
      }
      //
      nE2=aDS.ShapeIndex(anE2, iRankF2);
      aLs.Clear();
      pPaveFiller->SplitsOnEdge(nE1, nE2, aLs);
      
      aNbSpON=aLs.Extent();
      if (!aNbSpON) {
	continue;
      }

      aMap.Clear();
      TColStd_ListIteratorOfListOfInteger anItLs(aLs);
      for (; anItLs.More(); anItLs.Next()) {
	aBid=anItLs.Value();
	aMap.Add(aBid);
      }

      BOPTools_ListOfCommonBlock& aLCB=aCBPool(aDS.RefEdge(nE1));
      BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
      for (; anItCB.More(); anItCB.Next()) {
	bAdded=Standard_False;//xft
	BOPTools_CommonBlock& aCB=anItCB.Value();
	// Pave Block from which new edge will be taken
	const BOPTools_PaveBlock& aPB=aCB.PaveBlock1();
	nSpTaken=aPB.Edge();
	const TopoDS_Shape& aSpTaken=aDS.Shape(nSpTaken);
	//
	BOPTools_PaveBlock& aPB1=aCB.PaveBlock1(nE1);
	nSp1=aPB1.Edge();
	
	if (aMap.Contains(nSp1)) {
	  BOPTools_PaveBlock& aPB2=aCB.PaveBlock2(nE1);
	  nSp2=aPB2.Edge();
	  //
	  //
	  //iiiiiiiiiiiiiiiii Tue Dec 25 15:10:09 2001 iiiiii
	  //
	  // Internal eges' processing
	  bInternal1=(anOr ==TopAbs_INTERNAL);
	  bInternal2=(anOr2==TopAbs_INTERNAL);
	  //
	  if (bInternal1 || bInternal2) {
	    aSS=TopoDS::Edge(aDS.Shape(nSpTaken));
	    // a.
	    if (bInternal1 && bInternal2) {
	      if (anOperation==BOP_COMMON) {
		aWES.AddStartElement (aSS);
		bAdded=Standard_True;//xft
	      }
	    }
	    // b.
	    else { // else x 
	      if (bInternal1 && !bInternal2) {
		if (nSpTaken==nSp1) {
		  aSS.Orientation(TopAbs_FORWARD);
		  aSSx=TopoDS::Edge(aDS.Shape(nSp2));
		  aSSx.Orientation(anOr2);
		  //
		  bToReverse=BOPTools_Tools3D::IsSplitToReverse1 (aSSx, aSS, aContext);
		  //
		  if (bToReverse) {
		    aSS.Reverse();
		  }
		}
		else {//nSpTaken!=nSp1
		  aSS.Orientation(anOr2);
		}
	      }
	      // c.
	      else if (!bInternal1 && bInternal2) {
		if (nSpTaken==nSp2) {
		  aSS.Orientation(TopAbs_FORWARD);
		  aSSx=TopoDS::Edge(aDS.Shape(nSp1));
		  aSSx.Orientation(anOr);
		  //
		  bToReverse=BOPTools_Tools3D::IsSplitToReverse1 (aSSx, aSS, aContext);
		  //
		  if (bToReverse) {
		    aSS.Reverse();
		  }
		}
		else {//nSpTaken!=nSp2
		  aSS.Orientation(anOr);
		}
	      }
	      // writting
	      if (anOperation==BOP_COMMON) {
		aWES.AddStartElement (aSS);
		bAdded=Standard_True;//xft
	      }
	      if (anOperation==BOP_CUT) { 
		aSS.Reverse();
		aWES.AddStartElement (aSS);
		bAdded=Standard_True;//xft
	      }
	    } // else x 
	    continue;
	  }  
	  //
	  //iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	  //
	  //
	  const BOPTools_PointBetween &aPbn1=aPB1.PointBetween();
	  aT1=aPbn1.Parameter();
	  BOPTools_Tools2D::EdgeTangent(anE1, aT1, aTau1);
	  
	  BOPTools_Tools2D::PointOnSurface(anE1, aF1FWD, aT1, aU, aV);
	  BOPTools_Tools2D::FaceNormal(aF1FWD, aU, aV, aN1);
	  aBiN1=aN1^aTau1;
	  
	  const BOPTools_PointBetween &aPbn2=aPB2.PointBetween();
	  aT2=aPbn2.Parameter();
	  BOPTools_Tools2D::EdgeTangent(anE2, aT2, aTau2);
	  
	  BOPTools_Tools2D::PointOnSurface(anE2, aF2FWD, aT2, aU, aV);
	  BOPTools_Tools2D::FaceNormal(aF2FWD, aU, aV, aN2);
	  aBiN2=aN2^aTau2;

	  aScPr=aBiN1*aBiN2;
	  //
	  if (aScPr < 0.) {
	    if (anOperation==BOP_CUT) { 
	      
	      if (nSpTaken==nSp1) {
		aSS=TopoDS::Edge(aDS.GetShape(nSp1));
		aSS.Orientation(anOr);
	      }
	      
	      else {
		const TopoDS_Shape& aSp1=aDS.Shape(nSp1);
		TopoDS_Edge aSpE1=TopoDS::Edge(aSp1);
		aSpE1.Orientation(anOr);

		const TopoDS_Shape& aSp2=aDS.Shape(nSp2);
		TopoDS_Edge aSpE2=TopoDS::Edge(aSp2);
		//
		bToReverse=BOPTools_Tools3D::IsSplitToReverse1 (aSpE1, aSpE2, aContext);
		//
		if (bToReverse) {
		  aSpE2.Reverse();
		}
		aSS=aSpE2;
	      }
	      // modified by NIZHNY-MKK  Thu May 29 12:39:32 2003.BEGIN
	      if (BRep_Tool::IsClosed(anE1, aF1FWD) && (!aSS.IsSame(anE1))) {
		Standard_Boolean areverse = Standard_False;
		BOPTools_Tools3D::DoSplitSEAMOnFace(aSS, anE1, aF1FWD, areverse);
	      }

	      if(BRep_Tool::IsClosed(anE2, aF2FWD) && (!aSS.IsSame(anE2))) {
		Standard_Boolean areverse = Standard_False;
		BOPTools_Tools3D::DoSplitSEAMOnFace(aSS, anE2, aF2FWD, areverse);
	      }
	      // modified by NIZHNY-MKK  Thu May 29 12:39:35 2003.END
	      //
	      if (BRep_Tool::IsClosed(anE1, aF1FWD) &&
		  BRep_Tool::IsClosed(anE2, aF2FWD)){
		//
		Standard_Boolean bIsTakenSp1;
		TopoDS_Edge aSpE1Seam11, aSpE1Seam21;

		bIsTakenSp1=(nSpTaken==nSp1);
		aSpE1Seam11=TopoDS::Edge(aDS.Shape(nSp1));
		aSpE1Seam21=TopoDS::Edge(aDS.Shape(nSp2));
		//
		if (aM.Contains(aSpE1Seam11)){
		  continue;
		}
		aM.Add(aSpE1Seam11);
		//
		if (aM.Contains(aSpE1Seam21)){
		  continue;
		}
		aM.Add(aSpE1Seam21);
		//
		TreatSDSeams (aSpE1Seam11, aT1, aF1FWD, 
			      aSpE1Seam21, aT2, aF2FWD,
			      bIsTakenSp1, aWES,
			      aContext);
		//
		continue;
	      }
	      //
	      aWES.AddStartElement (aSS);
	      bAdded=Standard_True;//xft
	    } 
	    
	  }
	  
	  else {
	    if (anOperation==BOP_COMMON) {
	      
	      if (nSpTaken==nSp1) {
		aSS=TopoDS::Edge(aDS.GetShape(nSp1));
		aSS.Orientation(anOr);
	      }
	      
	      else {
		const TopoDS_Shape& aSp1=aDS.Shape(nSp1);
		TopoDS_Edge aSpE1=TopoDS::Edge(aSp1);
		aSpE1.Orientation(anOr);

		const TopoDS_Shape& aSp2=aDS.Shape(nSp2);
		TopoDS_Edge aSpE2=TopoDS::Edge(aSp2);
		//
		bToReverse=BOPTools_Tools3D::IsSplitToReverse1 (aSpE1, aSpE2, aContext);
		//
		if (bToReverse) {
		  aSpE2.Reverse();
		}
		aSS=aSpE2;
		//
		if (BRep_Tool::IsClosed(aSpE1, aF1FWD)) {
		  //
		  if (aM.Contains(aSpE2)){
		    continue;
		  }
		  aM.Add(aSpE2);
		  //
		  if (!BRep_Tool::IsClosed(aSpE2, aF1FWD)) {
		    BOPTools_Tools3D::DoSplitSEAMOnFace (aSpE2, aF1FWD);
		  }
		  aWES.AddStartElement (aSpE2);
		  aSpE2.Reverse();
		  aWES.AddStartElement (aSpE2);
		  bAdded=Standard_True;	//xft
		  continue;  
		}
		//
	      }
	      //
	      aWES.AddStartElement (aSS);
	      bAdded=Standard_True; //xft
	    }// if (anOperation==BOP_COMMON) {
	  }// else {
	  if (!bAdded) {
	    if(aMFence.Add(aSpTaken)) { 
	      myRejectedOnParts.Append(aSpTaken);
	    }
	  }
	}// if (aMap.Contains(nSp1)) { 
      }// for (; anItCB.More(); anItCB.Next()) {
    }// for (; anExpF2.More(); anExpF2.Next()) {
  }//for (; anExpF1.More(); anExpF1.Next()) {
}

//=======================================================================
// function: PrepareFaces
// purpose: 
//=======================================================================
  void BOP_SDFWESFiller::PrepareFaces   (const Standard_Integer nF1, 
					 const Standard_Integer nF2, 
					 TopoDS_Face& aF1FWD,
					 TopoDS_Face& aF2FWD) const
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  TopAbs_Orientation anOr1, anOr2;

  const TopoDS_Shape& aF1=aDS.GetShape(nF1); 
  aF1FWD=TopoDS::Face(aF1);
  anOr1=aF1.Orientation();

  const TopoDS_Shape& aF2=aDS.GetShape(nF2);
  aF2FWD=TopoDS::Face(aF2);
  anOr2=aF2.Orientation();

  aF1FWD.Orientation(TopAbs_FORWARD);

  if (mySenseFlag==1) {
    if      (anOr1==TopAbs_FORWARD   && anOr2==TopAbs_FORWARD) {
      aF2FWD.Orientation(TopAbs_FORWARD);
    }
    else if (anOr1==TopAbs_REVERSED  && anOr2==TopAbs_REVERSED) {
      aF2FWD.Orientation(TopAbs_FORWARD);
    }
    else if (anOr1==TopAbs_FORWARD  && anOr2==TopAbs_REVERSED) {
      aF2FWD.Orientation(TopAbs_REVERSED);
    }
    else if (anOr1==TopAbs_REVERSED  && anOr2==TopAbs_FORWARD) {
      aF2FWD.Orientation(TopAbs_REVERSED);
    }
  }

  else{
    if      (anOr1==TopAbs_FORWARD   && anOr2==TopAbs_FORWARD) {
      aF2FWD.Orientation(TopAbs_REVERSED);
    }
    else if (anOr1==TopAbs_REVERSED  && anOr2==TopAbs_REVERSED) {
      aF2FWD.Orientation(TopAbs_REVERSED);
    }
    else if (anOr1==TopAbs_FORWARD  && anOr2==TopAbs_REVERSED) {
      aF2FWD.Orientation(TopAbs_FORWARD);
    }
    else if (anOr1==TopAbs_REVERSED  && anOr2==TopAbs_FORWARD) {
      aF2FWD.Orientation(TopAbs_FORWARD);
    }
  }
}
//
//=======================================================================
// function: AssignDEStates
// purpose: 
//======================================================================= 
  void BOP_SDFWESFiller::AssignDEStates(const Standard_Integer nF1,
					const Standard_Integer nF2) 
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  
  Standard_Integer nE1, iRankF1;
  TopExp_Explorer anExpF1;

  iRankF1=aDS.Rank(nF1);

  const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));

  anExpF1.Init(aF1, TopAbs_EDGE);
  for (; anExpF1.More(); anExpF1.Next()) {
    const TopoDS_Edge& anE1=TopoDS::Edge(anExpF1.Current());
    //
    if (!BRep_Tool::Degenerated(anE1)){
      continue;
    }
    //
    nE1=aDS.ShapeIndex(anE1, iRankF1);
    AssignDEStates (nF1, nE1, nF2); 
  }
}

//=======================================================================
// function: AssignDEStates
// purpose: 
//======================================================================= 
  void BOP_SDFWESFiller::AssignDEStates(const Standard_Integer nFD,
					const Standard_Integer nED, 
					const Standard_Integer nF2) 
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*) &aPaveFiller;
  const BOPTools_SplitShapesPool& aSplitShapesPool=aPaveFiller.SplitShapesPool();
  //
  const BOPTools_ListOfPaveBlock& aSplitEdges=aSplitShapesPool(aDS.RefEdge(nED));
  //
  IntTools_Context& aContext=pPaveFiller->ChangeContext();
  const TopoDS_Edge& aDE=TopoDS::Edge(aDS.Shape(nED));
  const TopoDS_Face& aDF=TopoDS::Face(aDS.Shape(nFD));
  const TopoDS_Face& aFaceReference=TopoDS::Face(aDS.Shape(nF2));
  //
  Standard_Boolean bIsValidPoint;
  Standard_Integer nSp;
  Standard_Real aT, aT1, aT2;
  TopAbs_State aState;
  TopoDS_Face aF;
  gp_Pnt2d aPx2DNear;
  gp_Pnt aPxNear;
  //
  aF=aDF;
  aF.Orientation(TopAbs_FORWARD);
  
  BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSplitEdges);
  for (; aPBIt.More(); aPBIt.Next()) {
    BOPTools_PaveBlock& aPB=aPBIt.Value();
    
    nSp=aPB.Edge();
    const TopoDS_Edge& aSp=TopoDS::Edge(aDS.Shape(nSp));
    
    aPB.Parameters(aT1, aT2);
    aT=IntTools_Tools::IntermediatePoint(aT1, aT2);

    TopoDS_Edge aDERight, aSpRight;
    aDERight=aDE;
    aSpRight=aSp;

    BOPTools_Tools3D::OrientEdgeOnFace (aDE, aF, aDERight);
    aSpRight.Orientation(aDERight.Orientation());
    //
    BOPTools_Tools3D::PointNearEdge(aSpRight, aDF, aT, aPx2DNear, aPxNear);
    //
    aState=TopAbs_OUT;
    //
    bIsValidPoint=aContext.IsValidPointForFace(aPxNear, aFaceReference, 1.e-3);
    //
    if (bIsValidPoint) {
      aState=TopAbs_IN;
    }
    //
    BooleanOperations_StateOfShape aSt;
    
    aSt=BOPTools_SolidStateFiller::ConvertState(aState);
    if (myStatesMap.Contains(nSp)) {
      BooleanOperations_StateOfShape& aBooState=myStatesMap.ChangeFromKey(nSp);
      aBooState=aSt;
    }
    else {
      myStatesMap.Add(nSp, aSt);
    }
  }
}

//=======================================================================
// function: UpdateDEStates3D
// purpose: 
//======================================================================= 
  void BOP_SDFWESFiller::UpdateDEStates3D()
{
  BooleanOperations_ShapesDataStructure* pDS=
    (BooleanOperations_ShapesDataStructure*)&myDSFiller->DS();
  
  Standard_Integer i, aNb, nSp;
  BooleanOperations_StateOfShape aSt;

  aNb=myStatesMap.Extent();
  for (i=1; i<=aNb; i++) {
    nSp=myStatesMap.FindKey(i);
    aSt=pDS->GetState(nSp);
    aSt=BooleanOperations_UNKNOWN;
    pDS->SetState(nSp, aSt);
  }
}
//
#include <Geom2d_Curve.hxx>
#include <gp_Pnt2d.hxx>

//=======================================================================
// function: TreatSDSeams
// purpose: 
//=======================================================================
void TreatSDSeams (const TopoDS_Edge& aSpE1Seam11,
		   const Standard_Real aT1,
		   const TopoDS_Face& aF1FWD,
	
		   const TopoDS_Edge& aSpE1Seam21,
		   const Standard_Real aT2,
		   const TopoDS_Face& aF2FWD,
		   
		   const Standard_Boolean bIsTakenSp1,
		   BOP_WireEdgeSet& aWES,
		   IntTools_Context& aContext)
{
  Standard_Boolean bIsValidSeam11, bIsValidSeam12, 
                   bIsValidSeam21, bIsValidSeam22;
  Standard_Real aScPr;
  TopoDS_Edge aSS, aSpE1Seam12,aSpE1Seam22;
  gp_Dir aDB11, aDB12, aDB21, aDB22;
	    
  aSpE1Seam12=TopoDS::Edge(aSpE1Seam11.Reversed());
  aSpE1Seam22=TopoDS::Edge(aSpE1Seam21.Reversed());
	    //
  if (!bIsTakenSp1) {
    BOPTools_Tools3D::DoSplitSEAMOnFace (aSpE1Seam21, aF1FWD);
    aSpE1Seam22=TopoDS::Edge(aSpE1Seam21.Reversed());
  }
  //
  bIsValidSeam11=IsValidSeam(aSpE1Seam11, aF1FWD, aT1, aContext);
  bIsValidSeam12=IsValidSeam(aSpE1Seam12, aF1FWD, aT1, aContext);
  bIsValidSeam21=IsValidSeam(aSpE1Seam21, aF2FWD, aT2, aContext);
  bIsValidSeam22=IsValidSeam(aSpE1Seam22, aF2FWD, aT2, aContext);
  // 1
  if (bIsValidSeam11 && bIsValidSeam12) {
    BOPTools_Tools3D::GetBiNormal(aSpE1Seam11, aF1FWD, aT1, aDB11);
    BOPTools_Tools3D::GetBiNormal(aSpE1Seam12, aF1FWD, aT1, aDB12);
    // 1.1
    if (bIsValidSeam21 && !bIsValidSeam22) {
      BOPTools_Tools3D::GetBiNormal(aSpE1Seam21, aF2FWD, aT2, aDB21);
      aScPr=aDB11*aDB21;
      if (aScPr<0.) {
	if (bIsTakenSp1) {
	  aSS=aSpE1Seam11;
	}
	else {
	  //aSS=aSpE1Seam21;
	  CorrespondantSeam(aSpE1Seam11, aT1, 
			    aSpE1Seam21, aSpE1Seam22, aT2, 
			    aF1FWD, aSS);
	}
      }
      else { //if (aScPr>0.)
	if (bIsTakenSp1) {
	  aSS=aSpE1Seam12;
	}
	else {
	  //aSS=aSpE1Seam22;
	  CorrespondantSeam(aSpE1Seam12, aT1, 
			    aSpE1Seam21, aSpE1Seam22, aT2, 
			    aF1FWD, aSS);
	}
      }
      aWES.AddStartElement (aSS);
    } //if (bIsValidSeam21 && !bIsValidSeam22)
    //  
    // 1.2	
    if (!bIsValidSeam21 && bIsValidSeam22) {
      BOPTools_Tools3D::GetBiNormal(aSpE1Seam22, aF2FWD, aT2, aDB22);
      aScPr=aDB11*aDB22;
      if (aScPr<0.) {
	if (bIsTakenSp1) {
	  aSS=aSpE1Seam11;
	}
	else {
	  //aSS=aSpE1Seam22;
	  CorrespondantSeam(aSpE1Seam11, aT1, 
			    aSpE1Seam21, aSpE1Seam22, aT2, 
			    aF1FWD, aSS);
	}
      }
      else {//if (aScPr>0.)
	if (bIsTakenSp1) {
	  aSS=aSpE1Seam12;
	}
	else {
	  //aSS=aSpE1Seam21;
	  CorrespondantSeam(aSpE1Seam12, aT1, 
			    aSpE1Seam21, aSpE1Seam22, aT2, 
			    aF1FWD, aSS);
	}
      }
      aWES.AddStartElement (aSS);
    }// if (!bIsValidSeam21 && bIsValidSeam22)
  } //if (bIsValidSeam11 && bIsValidSeam12)
}

//=======================================================================
// function: IsValidSeam
// purpose: 
//======================================================================= 
  Standard_Boolean IsValidSeam(const TopoDS_Edge& aE,
			       const TopoDS_Face& aF,
			       const Standard_Real aT,
			       IntTools_Context& aContext)
{
  Standard_Boolean bIsPointInOnFace;
  gp_Pnt2d aPx2DNear;
  gp_Pnt aPxNear;

  BOPTools_Tools3D::PointNearEdge(aE, aF, aT, aPx2DNear, aPxNear);
  //
  bIsPointInOnFace=aContext.IsPointInOnFace(aF, aPx2DNear);
  return bIsPointInOnFace;
}
//=======================================================================
// function: CorrespondantSeam
// purpose: 
//======================================================================= 
  void CorrespondantSeam(const TopoDS_Edge& aSpE1Seam11,
			 const Standard_Real aT1,
			 const TopoDS_Edge& aSpE1Seam21,
			 const TopoDS_Edge& aSpE1Seam22,
			 const Standard_Real aT2,
			 const TopoDS_Face& aF1FWD,
			 TopoDS_Edge& aSS)
				      
{
  Standard_Real a, b, aD1121, aD1122, aTol=1.e-7;
  Handle(Geom2d_Curve) aC2DSeam11, aC2DSeam21, aC2DSeam22;
  gp_Pnt2d aP2D11, aP2D21, aP2D22;

  aC2DSeam11=BRep_Tool::CurveOnSurface(aSpE1Seam11, aF1FWD, a, b);
  aC2DSeam11->D0(aT1, aP2D11);

  aC2DSeam21=BRep_Tool::CurveOnSurface(aSpE1Seam21, aF1FWD, a, b);
  aC2DSeam21->D0(aT2, aP2D21);

  aC2DSeam22=BRep_Tool::CurveOnSurface(aSpE1Seam22, aF1FWD, a, b);
  aC2DSeam22->D0(aT2, aP2D22);

  aD1121=aP2D11.Distance(aP2D21);
  aD1122=aP2D11.Distance(aP2D22);
  
  aSS=aSpE1Seam22;
  if (aD1121<aTol) {
    aSS=aSpE1Seam21;
  }
}
