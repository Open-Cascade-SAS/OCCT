// File:	BOP_SolidSolid_1.cxx
// Created:	Tue Jun 29 12:29:57 2004
// Author:	Mikhail KLOKOV
//		<mkk@kurox>


#include <BOP_SolidSolid.ixx>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_StateOfShape.hxx>
#include <BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>

#include <BOPTools_SSInterference.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_Curve.hxx>

#include <BOPTools_Tools3D.hxx>

#include <IntTools_Context.hxx>

#include <BOP_BuilderTools.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>

#include <TopTools_DataMapOfShapeInteger.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>
#include <BOPTools_StateFiller.hxx>
#include <gp_Dir.hxx>
#include <BRep_Builder.hxx>

static
Standard_Integer GetIndex(const TopoDS_Shape& theShape, 
			  const BooleanOperations_ShapesDataStructure& theDS);

static
void GetAttachedFaces(const Standard_Integer   theEdgeIndex,
		      const Standard_Integer   theFaceIndex,
		      const BOPTools_DSFiller&        theDSFiller,
		      TColStd_ListOfInteger&   theListOfFaces);

static
void GetStatesOfAdjacentFaces(const TColStd_ListOfInteger& theListOfFacesToCheck,
			      const BOPTools_DSFiller&     theDSFiller,
			      TColStd_MapOfInteger&        theMapOfUsedIndices,
			      Standard_Boolean&            bFoundINOUT,
			      Standard_Boolean&            bFoundININ,
			      Standard_Boolean&            bFoundOUTOUT);

static
Standard_Boolean ComputeStateForAnalyticalSurfaces(const Standard_Integer theFaceIndex,
						   const Standard_Integer theBaseFaceIndex,
						   const BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& theFFMap,
						   const BOPTools_DSFiller& theDSFiller,
						   TopAbs_State& theState);

static 
Standard_Boolean IsEdgeValidForFace(const Standard_Integer theEdgeIndex,
				    const Standard_Integer theFaceIndex,
				    BOPTools_SSInterference& theFF,
				    const BOPTools_DSFiller& theDSFiller);

//=================================================================================
// function: PrepareFaceSplits
// purpose: 
//=================================================================================
void BOP_SolidSolid::PrepareFaceSplits() 
{
  const BooleanOperations_ShapesDataStructure& aDS = myDSFiller->DS();
  BooleanOperations_ShapesDataStructure*       pDS = (BooleanOperations_ShapesDataStructure*)&aDS;
  BOPTools_InterferencePool* pIntrPool = (BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs = pIntrPool->SSInterferences();

  Standard_Integer i, aNb, nF1, iRank;
  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger aFFMap;
  BOP_BuilderTools::DoMap(aFFs, aFFMap);
  TopTools_ListOfShape aListOfNewFaces;
  TopAbs_Orientation anOriF1 = TopAbs_FORWARD;

  TColStd_DataMapOfIntegerListOfInteger& aMapOfFaceSplits = myDSFiller->ChangeSplitFacePool();
  
  aNb=aFFMap.Extent();

  for (i=1; i<=aNb; i++) {
    // 

    nF1 = aFFMap.FindKey(i);
    TopoDS_Face aF1 = TopoDS::Face(aDS.Shape(nF1));
    
    anOriF1 = aF1.Orientation();
    iRank   = aDS.Rank(nF1);

    TopTools_DataMapOfShapeInteger aMapOfEdgeIndex;
    TopTools_ListOfShape aListOfSplits;

    if(SplitFace(nF1, aMapOfEdgeIndex, aListOfSplits)) {

      DoInternalVertices(nF1, aListOfSplits);

      TopTools_ListIteratorOfListOfShape aFaceIt(aListOfSplits);

      for(; aFaceIt.More(); aFaceIt.Next()) {
	TopoDS_Shape aShapeF = aFaceIt.Value();
	BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
	pDS->InsertShapeAndAncestorsSuccessors(aShapeF, anASSeq);
	//
	Standard_Integer aNewFaceIndex = pDS->NumberOfInsertedShapes();
	//
	pDS->SetState(aNewFaceIndex, BooleanOperations_ON);

	if(!aMapOfFaceSplits.IsBound(nF1)) {
          TColStd_ListOfInteger thelist;
	  aMapOfFaceSplits.Bind(nF1, thelist);
	}
	aMapOfFaceSplits.ChangeFind(nF1).Append(aNewFaceIndex);

	TopAbs_State aState = TopAbs_ON;
	Standard_Boolean bFoundFaceState = Standard_False;

	if(PropagateFaceStateByEdges(aShapeF, aMapOfEdgeIndex, aState)) {

	  if(aState != TopAbs_ON) { // can not determine correctly ON state
	    BooleanOperations_StateOfShape aConvertedState = BOPTools_StateFiller::ConvertState(aState);

	    pDS->SetState(aNewFaceIndex, aConvertedState);

	    bFoundFaceState = Standard_True;
	  }
	}

	if(!bFoundFaceState) {
	  // 
	  if(ComputeStateByInsidePoints(aNewFaceIndex, nF1, iRank, aFFMap, aState)) {

	    if(aState != TopAbs_ON) {
	      BooleanOperations_StateOfShape aConvertedState = BOPTools_StateFiller::ConvertState(aState);

	      pDS->SetState(aNewFaceIndex, aConvertedState);
	    }
	    bFoundFaceState = Standard_True;
	  }
	}

	if(!bFoundFaceState) {

	  if(ComputeStateForAnalyticalSurfaces(aNewFaceIndex, nF1, aFFMap, *myDSFiller, aState)) {
	    if(aState != TopAbs_ON) {
	      BooleanOperations_StateOfShape aConvertedState = BOPTools_StateFiller::ConvertState(aState);

	      pDS->SetState(aNewFaceIndex, aConvertedState);
	      bFoundFaceState = Standard_True;
	    }
	  }
	}
      }
    }
  }
  // end for
}

// =====================================================================================================================
//  function: PropagateFaceStateByEdges
//  purpose:
// =====================================================================================================================
Standard_Boolean BOP_SolidSolid::PropagateFaceStateByEdges(const TopoDS_Shape& theFace,
							   const TopTools_DataMapOfShapeInteger& theMapOfEdgeIndex,
							   TopAbs_State& theState) 
{
  TopAbs_State aState = TopAbs_UNKNOWN;

  const BooleanOperations_ShapesDataStructure& aDS              = myDSFiller->DS();

  if(theFace.IsNull() || (theFace.ShapeType() != TopAbs_FACE))
    return Standard_False;
  TopoDS_Face aF1 = TopoDS::Face(theFace);

  Standard_Boolean bFoundNotON = Standard_False;
  BooleanOperations_StateOfShape aFoundState = BooleanOperations_ON;
  Standard_Boolean bIsINOUT = Standard_False;
  
  TopExp_Explorer anExpE(aF1, TopAbs_EDGE);

  for(; anExpE.More(); anExpE.Next()) {
    const TopoDS_Shape& anEdge = anExpE.Current();

    Standard_Integer nE = 0;

    if(theMapOfEdgeIndex.IsBound(anEdge)) {
      nE = theMapOfEdgeIndex(anEdge);
    }
    else {
      nE = aDS.ShapeIndex(anEdge, 1);
      nE = (nE == 0) ? aDS.ShapeIndex(anEdge, 2) : nE;
    }

    if(nE == 0)
      continue;

    BooleanOperations_StateOfShape anEdgeState = aDS.GetState(nE);

    if((anEdgeState == BooleanOperations_IN) ||
       (anEdgeState == BooleanOperations_OUT)) {

      if(!bFoundNotON) {
	bFoundNotON = Standard_True;
	aFoundState = anEdgeState;
      }
	
      if(aFoundState != anEdgeState) {
	bIsINOUT = Standard_True;
	break;
      }
    }
  }

  if(!bIsINOUT && bFoundNotON) {
    if(aFoundState == BooleanOperations_IN)
      aState = TopAbs_IN;
    else if(aFoundState == BooleanOperations_OUT)
      aState = TopAbs_OUT;     
  }

  if(aState == TopAbs_UNKNOWN)
    return Standard_False;

  theState = aState;

  return Standard_True;
}

// =====================================================================================================================
//  function: ComputeStateByInsidePoints
//  purpose:
// =====================================================================================================================
Standard_Boolean BOP_SolidSolid::ComputeStateByInsidePoints(const Standard_Integer theFaceIndex,
							    const Standard_Integer theBaseFaceIndex,
							    const Standard_Integer theFaceRank,
							    const BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& theFFMap,
							    TopAbs_State& theState) 

{
  TopAbs_State aState = TopAbs_ON;
  const BooleanOperations_ShapesDataStructure& aDS = myDSFiller->DS();
  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  const BOPTools_PaveFiller& aPaveFiller = myDSFiller->PaveFiller();
  BOPTools_PaveFiller*       pPaveFiller = (BOPTools_PaveFiller*)&aPaveFiller;
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();

  if(theFaceIndex == 0)
    return Standard_False;

  const TopoDS_Shape& aS = aDS.Shape(theFaceIndex);

  if(aS.IsNull())
    return Standard_False;
  TopoDS_Face aFace = TopoDS::Face(aS);
  //
  //
  Standard_Integer i = 0, j = 0, aNb = 0;
  aNb = theFFMap.Extent();

  for (i=1; i<=aNb; i++) {
    // 
    Standard_Integer nF1 = theFFMap.FindKey(i);

    if(nF1 != theBaseFaceIndex)
      continue;

    const TColStd_IndexedMapOfInteger& aFFIndicesMap=theFFMap.FindFromIndex(i);
    Standard_Integer aNbj = aFFIndicesMap.Extent();

    for (j=1; j<=aNbj; j++) {
      Standard_Integer iFF = aFFIndicesMap(j);
      BOPTools_SSInterference& aFF = aFFs(iFF);
      Standard_Boolean bIsTouchCase = aFF.IsTangentFaces();

      if (bIsTouchCase) {
	Standard_Integer nF2 = aFF.OppositeIndex(nF1);
	const TopoDS_Face& aF2 = TopoDS::Face(aDS.Shape(nF2));

	if(BOPTools_Tools3D::CheckSameDomainFaceInside(aFace, aF2, pPaveFiller->ChangeContext())) {
	  theState = TopAbs_ON;
	  return Standard_True;
	}
      }
    }
  }
  const TopoDS_Shape& aTool = (theFaceRank == 1) ? aDS.Tool() : aDS.Object();
  TopoDS_Solid aRefSolid;

  if(aTool.ShapeType() == TopAbs_SOLID)
    aRefSolid = TopoDS::Solid(aTool);
  else {
    BRep_Builder aBB;
    aBB.MakeSolid(aRefSolid);

    TopExp_Explorer anExpSH(aTool, TopAbs_SHELL);

    for(; anExpSH.More(); anExpSH.Next()) {
      TopoDS_Shape aShell = anExpSH.Current();
      aBB.Add(aShell, aRefSolid);
    }
  }

  if(!BOPTools_Tools3D::ComputeFaceState(aFace, aRefSolid, pPaveFiller->ChangeContext(), aState)) {
    return Standard_False;
  }
  theState = aState;

  return Standard_True;
}

// =====================================================================================================================
//  function: TakeOnSplit
//  purpose:
// =====================================================================================================================
Standard_Boolean BOP_SolidSolid::TakeOnSplit(const Standard_Integer theFaceIndex,
					     const Standard_Integer theBaseFaceIndex) const
{
  Standard_Boolean bTake = Standard_False;

  Standard_Boolean binout = Standard_False;
  Standard_Boolean binin = Standard_False;
  Standard_Boolean boutout  = Standard_False;

  TColStd_MapOfInteger aMapOfUsedIndices;
  TColStd_ListOfInteger aListOfFacesToCheck;
  aListOfFacesToCheck.Append(theFaceIndex);

  GetStatesOfAdjacentFaces(aListOfFacesToCheck, *myDSFiller, aMapOfUsedIndices, binout, binin, boutout);
  
  switch(myOperation) {
  case BOP_FUSE: {
    if(binout || (!binin && !boutout)) {
      bTake = Standard_True;
    }
    break;
  }
  case BOP_COMMON: {
    if(binout || (!binin && !boutout)) {
      bTake = Standard_True;
    }
    break;
  }
  case BOP_CUT: {
    if((binin || boutout) && !binout) {
      bTake = Standard_True;
    }
    break;
  }
  case BOP_CUT21: {
    if((binin || boutout) && !binout) {
      bTake = Standard_True;
    }
    break;
  }
  default: {
    break;
  }
  }
  return bTake;
}

// ------------------------------------------------------------------------------------
// static function: GetIndex
// purpose:
// ------------------------------------------------------------------------------------
Standard_Integer GetIndex(const TopoDS_Shape& theShape, 
			  const BooleanOperations_ShapesDataStructure& theDS)
{
  Standard_Integer anIndex = 0, i = 0;

  anIndex = theDS.ShapeIndex(theShape, 1);
  anIndex = (anIndex == 0) ? theDS.ShapeIndex(theShape, 2) : anIndex;

  if(anIndex == 0) {

    for (i = theDS.NumberOfSourceShapes() + 1; i <= theDS.NumberOfInsertedShapes(); i++) {
      if(theShape.IsSame(theDS.Shape(i))) {
	anIndex = i;
	break;
      }
    }
  }

  return anIndex;
}

// ------------------------------------------------------------------------------------
// static function: GetAttachedFaces
// purpose:
// ------------------------------------------------------------------------------------
void GetAttachedFaces(const Standard_Integer   theEdgeIndex,
		      const Standard_Integer   theFaceIndex,
		      const BOPTools_DSFiller&        theDSFiller,
		      TColStd_ListOfInteger&   theListOfFaces)
{
  theListOfFaces.Clear();
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller.DS();
  const TColStd_DataMapOfIntegerListOfInteger& aMap = theDSFiller.SplitFacePool();

  Standard_Integer i = 0;

  for(i = 1; i <= aDS.NumberOfInsertedShapes(); i++) {

    if(aDS.GetShapeType(i) == TopAbs_FACE) {
      TColStd_ListOfInteger aListOfFaceIndex;

      if(!aMap.IsBound(i)) {
	if(theFaceIndex == i)
	  continue;
	aListOfFaceIndex.Append(i);
      }
      else {
	TColStd_ListIteratorOfListOfInteger anIttmp(aMap.Find(i));

	for(; anIttmp.More(); anIttmp.Next()) {
	  if(theFaceIndex == anIttmp.Value())
	    continue;
	  aListOfFaceIndex.Append(anIttmp.Value());
	}
      }

      TColStd_ListIteratorOfListOfInteger anIt(aListOfFaceIndex);
      
      for(; anIt.More(); anIt.Next()) {
	if(anIt.Value() <= 0)
	  continue;
	const TopoDS_Shape& aFace = aDS.Shape(anIt.Value());
	TopExp_Explorer anExpE(aFace, TopAbs_EDGE);

	for(; anExpE.More(); anExpE.Next()) {
	  const TopoDS_Shape& anEdge = anExpE.Current();
	  Standard_Integer nE = GetIndex(anEdge, aDS);

	  if(theEdgeIndex == nE) {
	    theListOfFaces.Append(anIt.Value());
	    break;
	  }
	}
      }
    }
  }  
}

// ------------------------------------------------------------------------------------
// static function: GetStatesOfAdjacentFaces
// purpose:
// ------------------------------------------------------------------------------------
void GetStatesOfAdjacentFaces(const TColStd_ListOfInteger& theListOfFacesToCheck,
			      const BOPTools_DSFiller&     theDSFiller,
			      TColStd_MapOfInteger&        theMapOfUsedIndices,
			      Standard_Boolean&            bFoundINOUT,
			      Standard_Boolean&            bFoundININ,
			      Standard_Boolean&            bFoundOUTOUT)
{

  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller.DS();
  TColStd_ListOfInteger aLisOfON;
  TColStd_ListIteratorOfListOfInteger anItF(theListOfFacesToCheck);

  for(; anItF.More(); anItF.Next()) {
    Standard_Integer nF = anItF.Value();

    if(theMapOfUsedIndices.Contains(nF)) {
      continue;
    }
    theMapOfUsedIndices.Add(nF);

    TopoDS_Shape aFace = aDS.Shape(nF);

    TopExp_Explorer anExpE(aFace, TopAbs_EDGE);

    for(; anExpE.More(); anExpE.Next()) {
      const TopoDS_Shape& anEdge = anExpE.Current();
      Standard_Integer nE = 0;
      nE = GetIndex(anEdge, aDS);

      if(nE <= 0)
	continue;

      if(theMapOfUsedIndices.Contains(nE))
	continue;
      theMapOfUsedIndices.Add(nE);
      TColStd_ListOfInteger aListOfFaces, aListOfIN, aListOfOUT;
      GetAttachedFaces(nE, nF, theDSFiller, aListOfFaces);
    
      TColStd_ListIteratorOfListOfInteger anIt(aListOfFaces);

      for(; anIt.More(); anIt.Next()) {
	if(theMapOfUsedIndices.Contains(anIt.Value()))
	    continue;

	// 	if((aDS.GetState(anIt.Value()) != BooleanOperations_IN) &&
	// 	   (aDS.GetState(anIt.Value()) != BooleanOperations_OUT))
	if(aDS.GetState(anIt.Value()) == BooleanOperations_ON)
	  aLisOfON.Append(anIt.Value());

	if(aDS.GetState(anIt.Value()) == BooleanOperations_IN)
	  aListOfIN.Append(anIt.Value());
	else if(aDS.GetState(anIt.Value()) == BooleanOperations_OUT)
	  aListOfOUT.Append(anIt.Value());
      }
      bFoundINOUT  = bFoundINOUT || (!aListOfIN.IsEmpty() && !aListOfOUT.IsEmpty());
      bFoundININ   = bFoundININ  || (!aListOfIN.IsEmpty() && aListOfOUT.IsEmpty());
      bFoundOUTOUT = bFoundOUTOUT || (aListOfIN.IsEmpty() && !aListOfOUT.IsEmpty());
    }
  }

  if(!aLisOfON.IsEmpty() && (theMapOfUsedIndices.Extent() <= aDS.NumberOfInsertedShapes())) {
    GetStatesOfAdjacentFaces(aLisOfON, theDSFiller, theMapOfUsedIndices, bFoundINOUT, bFoundININ, bFoundOUTOUT);
  }
}

// ------------------------------------------------------------------------------------
// static function: ComputeStateForAnalyticalSurfaces
// purpose:
// ------------------------------------------------------------------------------------
Standard_Boolean ComputeStateForAnalyticalSurfaces(const Standard_Integer theFaceIndex,
						   const Standard_Integer theBaseFaceIndex,
						   const BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& theFFMap,
						   const BOPTools_DSFiller& theDSFiller,
						   TopAbs_State& theState)
{
  TopAbs_State aState = TopAbs_ON;
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller.DS();
  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&theDSFiller.InterfPool();
  const BOPTools_PaveFiller& aPaveFiller = theDSFiller.PaveFiller();
  BOPTools_PaveFiller*       pPaveFiller = (BOPTools_PaveFiller*)&aPaveFiller;
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();

  if(theFaceIndex == 0)
    return Standard_False;

  const TopoDS_Shape& aS = aDS.Shape(theFaceIndex);

  if(aS.IsNull())
    return Standard_False;
  TopoDS_Face aFace = TopoDS::Face(aS);

  Standard_Integer j = 0;
  Standard_Boolean bFound = Standard_False;

  if (theFFMap.Contains(theBaseFaceIndex)) {
    // 
    Standard_Integer nF1 = theBaseFaceIndex;

    const TColStd_IndexedMapOfInteger& aFFIndicesMap=theFFMap.FindFromKey(theBaseFaceIndex);
    Standard_Integer aNbj = aFFIndicesMap.Extent();

    for (j=1; (!bFound) && (j<=aNbj); j++) {
      Standard_Integer iFF = aFFIndicesMap(j);
      BOPTools_SSInterference& aFF = aFFs(iFF);
      Standard_Boolean bIsTouchCase = aFF.IsTangentFaces();

      if (!bIsTouchCase) {
	Standard_Integer nF2 = aFF.OppositeIndex(nF1);
	const TopoDS_Face& aF2 = TopoDS::Face(aDS.Shape(nF2));
	//
	TopExp_Explorer anExpE(aFace, TopAbs_EDGE);

	for(; anExpE.More(); anExpE.Next()) {
	  TopoDS_Edge aSp = TopoDS::Edge(anExpE.Current());

	  Standard_Boolean bTestEdge = Standard_False;

	  Standard_Integer  nE = GetIndex(aSp, aDS);
	  bTestEdge = IsEdgeValidForFace(nE, nF2, aFF, theDSFiller);

	  if(bTestEdge) {
	    TopAbs_State aStPF = TopAbs_ON;

	    Standard_Boolean bAnalytic = Standard_False;
	    Standard_Real aTolTangent, aTolR;
	    //
	    aTolTangent=0.002;
	    aTolR=0.0000001;

	    bAnalytic = BOPTools_Tools3D::TreatedAsAnalytic(aF2, aSp, aFace, 
							    aTolTangent, aTolR, 
							    aStPF, pPaveFiller->ChangeContext());
	    if(bAnalytic) {
	      aState = aStPF;
	      bFound = Standard_True;
	      break;
	    }
	    else {
	      gp_Dir aDBF1, aDNF2;

	      BOPTools_Tools3D::GetBiNormal (aSp, aFace, aDBF1);
	      BOPTools_Tools3D::GetNormalToFaceOnEdge (aSp, aF2, aDNF2);
	      
	      Standard_Real aTolScPr, aScPr;
	    
	      aTolScPr=1.e-7;
	      aScPr=aDBF1*aDNF2;

	      if (fabs(aScPr) > aTolScPr) {
		aStPF=TopAbs_OUT;
	      
		if (aScPr<0.) {
		  aStPF=TopAbs_IN;
		}
		aState = aStPF;
		bFound = Standard_True;
		break;
	      }
	    }
	  }
	  // end if(bTestEdge)
	}
      }
    }
  }

  if(!bFound) {
    return Standard_False;
  }
  theState = aState;
  return Standard_True;
}

// ------------------------------------------------------------------------------------
// static function: IsEdgeValidForFace
// purpose:
// ------------------------------------------------------------------------------------
Standard_Boolean IsEdgeValidForFace(const Standard_Integer theEdgeIndex,
				    const Standard_Integer theFaceIndex,
				    BOPTools_SSInterference& theFF,
				    const BOPTools_DSFiller& theDSFiller)
{

  const BOPTools_PaveFiller&                   aPaveFiller      = theDSFiller.PaveFiller();
  BOPTools_PaveFiller*                         pPaveFiller      = (BOPTools_PaveFiller*)&aPaveFiller;
  BOPTools_CommonBlockPool&                    aCBPool          = pPaveFiller->ChangeCommonBlockPool();

  BOPTools_SequenceOfCurves& aSCvs=theFF.Curves();
  Standard_Integer aNbCurves=aSCvs.Length();
  Standard_Integer i = 0;

  for (i=1; i<=aNbCurves; i++) {
    BOPTools_Curve& aBC=aSCvs(i);
    const BOPTools_ListOfPaveBlock& aSectEdges=aBC.NewPaveBlocks();

    BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSectEdges);

    for (; aPBIt.More(); aPBIt.Next()) {
      BOPTools_PaveBlock& aPB = aPBIt.Value();

      if(theEdgeIndex == aPB.Edge())
	return Standard_True;
    }
  }

  
  for(i = 1; i <= aCBPool.Length(); i++) {
    const BOPTools_ListOfCommonBlock& aCBList = aCBPool.Value(i);
    BOPTools_ListIteratorOfListOfCommonBlock anItCB(aCBList);

    for (; anItCB.More(); anItCB.Next()) {
      BOPTools_CommonBlock& aCB=anItCB.Value();
      Standard_Integer nFace = aCB.Face();

      if(nFace == theFaceIndex) {
	const BOPTools_PaveBlock& aPB1 = aCB.PaveBlock1();
	const BOPTools_PaveBlock& aPB2 = aCB.PaveBlock2();

	if((theEdgeIndex == aPB1.Edge()) ||
	   (theEdgeIndex == aPB2.Edge())) {
	  return Standard_True;
	}
      }
    }
  }
  

  return Standard_False;
}
