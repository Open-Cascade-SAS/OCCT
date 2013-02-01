// Created on: 2004-06-29
// Created by: Mikhail KLOKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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



#include <BOP_SolidSolid.ixx>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>

#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Surface.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <HatchGen_Domain.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
//
#include <TopExp_Explorer.hxx>
#include <BRepTools.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <TopTools_DataMapOfShapeInteger.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_StateOfShape.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>
#include <BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>

#include <IntTools_Tools.hxx>
#include <IntTools_Context.hxx>

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
#include <BOPTools_Tools2D.hxx>
#include <BOPTools_Tools3D.hxx>
#include <BOPTools_StateFiller.hxx>

#include <BOP_BuilderTools.hxx>

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
  Standard_Boolean ComputeStateForAnalyticalSurfaces
  (const Standard_Integer theFaceIndex,
   const Standard_Integer theBaseFaceIndex,
   const BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& theFFMap,
   const BOPTools_DSFiller& theDSFiller,
   TopAbs_State& theState);

static 
Standard_Boolean IsEdgeValidForFace(const Standard_Integer theEdgeIndex,
				    const Standard_Integer theFaceIndex,
				    BOPTools_SSInterference& theFF,
				    const BOPTools_DSFiller& theDSFiller);

static
  TopAbs_State ComputeState(const TopoDS_Face& theF,
			  const TopoDS_Solid& theRef,
			  const Standard_Real theTol,
			  const Handle(IntTools_Context)& theCtx);

static
  Standard_Integer PntInFace(const TopoDS_Face& aF,
			     gp_Pnt& theP,
			     gp_Pnt2d& theP2D);

static
  Standard_Integer PntHoverFace(const TopoDS_Face& aF,
				gp_Pnt& theP);

static
  TopAbs_State ComputeState(const gp_Pnt& theP,
			    const TopoDS_Solid& theRef,
			    const Standard_Real theTol,
			    const Handle(IntTools_Context)& theCtx);


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

//=======================================================================
//function : ComputeStateByInsidePoints
//purpose  : 
//=======================================================================
Standard_Boolean BOP_SolidSolid::
  ComputeStateByInsidePoints(const Standard_Integer theFaceIndex,
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

	if(BOPTools_Tools3D::CheckSameDomainFaceInside(aFace, aF2, pPaveFiller->Context())) {
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

  if(!BOPTools_Tools3D::ComputeFaceState(aFace, aRefSolid, pPaveFiller->Context(), aState)) {
    return Standard_False;
  }
  theState = aState;

  return Standard_True;
}
//=======================================================================
//function : TakeOnSplit
//purpose  : 
//=======================================================================
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
  //DEB
  Handle(IntTools_Context) aCtx;
  TopAbs_Orientation aOrF1;
  TopoDS_Face aF1, aFSp;
  //
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPF =myDSFiller->PaveFiller();
  BOPTools_PaveFiller* pPF = (BOPTools_PaveFiller*)&aPF;
  aCtx=pPF->Context();
  //
  aF1=TopoDS::Face(aDS.Shape(theBaseFaceIndex));
  aOrF1=aF1.Orientation();
  aFSp=TopoDS::Face(aDS.Shape(theFaceIndex));
  aFSp.Orientation(aOrF1);
  //
  GetStatesOfAdjacentFaces(aListOfFacesToCheck, 
			   *myDSFiller, 
			   aMapOfUsedIndices, 
			   binout, binin, boutout);
  
  //zzf
  if (!binout && !binin && !boutout) {
    Standard_Real aTol;
    TopAbs_State aState;
    //
    aTol=1.e-5;
    aState=ComputeState(aFSp, myRefTool, aTol, aCtx);
    //
    if (aState==TopAbs_IN) {
      if (myOperation==BOP_FUSE || myOperation==BOP_COMMON) {
	bTake=Standard_False;
      }
      else {
	bTake=Standard_True;
      }
    }
    //
    else if (aState==TopAbs_OUT) {
      if (myOperation==BOP_FUSE || myOperation==BOP_COMMON) {
	bTake=Standard_True;
      }
      else {
	bTake=Standard_False;
      }
    }
    //
    return bTake;
  }//if (!binout && !binin && !boutout) {
  //zzt
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
//=======================================================================
//function : GetStatesOfAdjacentFaces
//purpose  : 
//=======================================================================
void GetStatesOfAdjacentFaces(const TColStd_ListOfInteger& theListOfFacesToCheck,
			      const BOPTools_DSFiller&     theDSFiller,
			      TColStd_MapOfInteger&        theMapOfUsedIndices,
			      Standard_Boolean&            bFoundINOUT,
			      Standard_Boolean&            bFoundININ,
			      Standard_Boolean&            bFoundOUTOUT)
{
  TColStd_ListOfInteger aLisOfON;
  Standard_Integer nE, nF, nFa;
  BooleanOperations_StateOfShape aStFa;
  //
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller.DS();
  //
  TColStd_ListIteratorOfListOfInteger anItF(theListOfFacesToCheck);
  for(; anItF.More(); anItF.Next()) {
    nF = anItF.Value();
    const TopoDS_Shape& aF=aDS.Shape(nF);

    if(theMapOfUsedIndices.Contains(nF)) {
      continue;
    }
    //
    theMapOfUsedIndices.Add(nF);
    
    TopoDS_Shape aFace = aDS.Shape(nF);

    TopExp_Explorer anExpE(aFace, TopAbs_EDGE);

    for(; anExpE.More(); anExpE.Next()) {
      const TopoDS_Shape& anEdge = anExpE.Current();
      nE = GetIndex(anEdge, aDS);

      if(nE <= 0) {
	continue;
      }
      if(theMapOfUsedIndices.Contains(nE)) {
	continue;
      }
      //
      theMapOfUsedIndices.Add(nE);
      TColStd_ListOfInteger aListOfFaces, aListOfIN, aListOfOUT;
      GetAttachedFaces(nE, nF, theDSFiller, aListOfFaces);
    
      TColStd_ListIteratorOfListOfInteger anIt(aListOfFaces);

      for(; anIt.More(); anIt.Next()) {
	nFa=anIt.Value();
	aStFa=aDS.GetState(nFa);
	//
	const TopoDS_Shape& aFa=aDS.Shape(nFa);
	if(theMapOfUsedIndices.Contains(nFa)) {
	  continue;
	}
	//
	if(aStFa==BooleanOperations_ON) {
	  aLisOfON.Append(nFa);
	}
	if(aStFa==BooleanOperations_IN) {
	  aListOfIN.Append(nFa);
	}
	else if(aStFa==BooleanOperations_OUT) {
	  aListOfOUT.Append(nFa);
	}
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
							    aStPF, pPaveFiller->Context());
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
//=======================================================================
// function:  ComputeState
// purpose:
//=======================================================================
TopAbs_State ComputeState(const TopoDS_Face& theF,
			  const TopoDS_Solid& theRef,
			  const Standard_Real theTol,
			  const Handle(IntTools_Context)& theCtx)
{
  Standard_Integer iErr;
  TopAbs_State aRet;
  gp_Pnt aP;
  //
  aRet=TopAbs_UNKNOWN;
  //
  iErr=PntHoverFace(theF, aP);
  if (iErr) {
    return aRet;
  }
  //
  aRet=ComputeState(aP, theRef, theTol, theCtx);
  return aRet;
}

//=======================================================================
// function:  ComputeState
// purpose:
//=======================================================================
TopAbs_State ComputeState(const gp_Pnt& theP,
			  const TopoDS_Solid& theRef,
			  const Standard_Real theTol,
			  const Handle(IntTools_Context)& theCtx)
{
  TopAbs_State aState;
  //
  BRepClass3d_SolidClassifier& aSC=theCtx->SolidClassifier(theRef);
  aSC.Perform(theP, theTol);
  //
  aState=aSC.State();
  //
  return aState;
}
//=======================================================================
//function : PntHoverFace
//purpose  :
//=======================================================================
Standard_Integer PntHoverFace(const TopoDS_Face& aF,
			      gp_Pnt& theP)
{
  Standard_Integer iErr;
  Standard_Real aU, aV, aX;
  gp_Pnt aP;
  gp_Vec aDN;
  gp_Pnt2d aP2D;
  //
  iErr=PntInFace(aF, aP,  aP2D);
  if (iErr) {
    return iErr;
  }
  //
  aX=1e-4;
  //
  aP2D.Coord(aU, aV);
  BOPTools_Tools2D::FaceNormal(aF, aU, aV, aDN);
  //
  theP.SetXYZ(aP.XYZ()+aX*aDN.XYZ());
  //
  return iErr;
}
//=======================================================================
//function : PntInFace
//purpose  :
//=======================================================================
Standard_Integer PntInFace(const TopoDS_Face& aF,
			   gp_Pnt& theP,
			   gp_Pnt2d& theP2D)
{
  Standard_Boolean bIsDone, bHasFirstPoint, bHasSecondPoint;
  Standard_Integer iErr, aIx, aNbDomains, i;
  Standard_Real aUMin, aUMax, aVMin, aVMax;
  Standard_Real aVx, aUx, aV1, aV2, aU1, aU2, aEpsT;
  Standard_Real aTolArcIntr, aTolTangfIntr, aTolHatch2D, aTolHatch3D;
  gp_Dir2d aD2D (0., 1.);
  gp_Pnt2d aP2D;
  gp_Pnt aPx;
  Handle(Geom2d_Curve) aC2D;
  Handle(Geom2d_TrimmedCurve) aCT2D;
  Handle(Geom2d_Line) aL2D;
  Handle(Geom_Surface) aS;
  TopAbs_Orientation aOrE;
  TopoDS_Face aFF;
  TopExp_Explorer aExp;
  //
  aTolHatch2D=1.e-8;
  aTolHatch3D=1.e-8;
  aTolArcIntr=1.e-10;
  aTolTangfIntr=1.e-10;
  //
  Geom2dHatch_Intersector aIntr(aTolArcIntr, aTolTangfIntr);
  Geom2dHatch_Hatcher aHatcher(aIntr,
			       aTolHatch2D, aTolHatch3D,
			       Standard_True, Standard_False);
  //
  iErr=0;
  aEpsT=1.e-12;
  //
  aFF=aF;
  aFF.Orientation (TopAbs_FORWARD);
  //
  aS=BRep_Tool::Surface(aFF);
  BRepTools::UVBounds(aFF, aUMin, aUMax, aVMin, aVMax);
  //
  // 1
  aExp.Init (aFF, TopAbs_EDGE);
  for (; aExp.More() ; aExp.Next()) {
    const TopoDS_Edge& aE=*((TopoDS_Edge*)&aExp.Current());
    aOrE=aE.Orientation();
    //
    aC2D=BRep_Tool::CurveOnSurface (aE, aFF, aU1, aU2);
    if (aC2D.IsNull() ) {
      iErr=1;
      return iErr;
    }
    if (fabs(aU1-aU2) < aEpsT) {
      iErr=2;
      return iErr;
    }
    //
    aCT2D=new Geom2d_TrimmedCurve(aC2D, aU1, aU2);
    aHatcher.AddElement(aCT2D, aOrE);
  }// for (; aExp.More() ; aExp.Next()) {
  //
  // 2
  aUx=IntTools_Tools::IntermediatePoint(aUMin, aUMax);
  aP2D.SetCoord(aUx, 0.);
  aL2D=new Geom2d_Line (aP2D, aD2D);
  Geom2dAdaptor_Curve aHCur(aL2D);
  //
  aIx=aHatcher.AddHatching(aHCur) ;
  //
  // 3.
  aHatcher.Trim();
  bIsDone=aHatcher.TrimDone(aIx);
  if (!bIsDone) {
    iErr=3;
    return iErr;
  }
  //
  aHatcher.ComputeDomains(aIx);
  bIsDone=aHatcher.IsDone(aIx);
  if (!bIsDone) {
    iErr=4;
    return iErr;
  }
  //
  // 4.
  aNbDomains=aHatcher.NbDomains(aIx);
  for (i=1; i<=aNbDomains; ++i) {
    const HatchGen_Domain& aDomain=aHatcher.Domain (aIx, i) ;
    bHasFirstPoint=aDomain.HasFirstPoint();
    if (!bHasFirstPoint) {
      iErr=5;
      return iErr;
    }
    //
    aV1=aDomain.FirstPoint().Parameter();
    //
    bHasSecondPoint=aDomain.HasSecondPoint();
    if (!bHasSecondPoint) {
      iErr=6;
      return iErr;
    }
    //
    aV2=aDomain.SecondPoint().Parameter();
    //
    aVx=IntTools_Tools::IntermediatePoint(aV1, aV2);
    //
    break;
  }
  //
  aS->D0(aUx, aVx, aPx);
  //
  theP2D.SetCoord(aUx, aVx);
  theP=aPx;
  //
  return iErr;
}

