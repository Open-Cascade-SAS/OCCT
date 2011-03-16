// File:	BOP_BuilderTools.cxx
// Created:	Fri Nov  2 11:40:53 2001
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <BOP_BuilderTools.ixx>

#include <TColStd_IndexedMapOfInteger.hxx>

#include <gp_Dir.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>

#include <TopAbs_State.hxx>

#include <TopoDS_Shape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BooleanOperations_StateOfShape.hxx>

#include <BOPTools_Tools3D.hxx>
#include <BOPTools_SSInterference.hxx>

#include <BOP_ConnexityBlock.hxx>
#include <BOP_ListOfConnexityBlock.hxx> 

//=======================================================================
// function: StateToCompare
// purpose: 
//=======================================================================
  BooleanOperations_StateOfShape 
    BOP_BuilderTools::StateToCompare(const Standard_Integer iRank,
				     const BOP_Operation anOp)
{
  BooleanOperations_StateOfShape aSt=BooleanOperations_UNKNOWN;
  
  if (iRank==1) {
    if (anOp==BOP_COMMON) {
      aSt=BooleanOperations_IN;
    }
    if (anOp==BOP_FUSE) {
      aSt=BooleanOperations_OUT;
    }
    if (anOp==BOP_CUT) {
      aSt=BooleanOperations_OUT;
    }
    if (anOp==BOP_CUT21) {
      aSt=BooleanOperations_IN;
    }
  }

  if (iRank==2) {
    if (anOp==BOP_COMMON) {
      aSt=BooleanOperations_IN;
    }
    if (anOp==BOP_FUSE) {
      aSt=BooleanOperations_OUT;
    }
    if (anOp==BOP_CUT) {
      aSt=BooleanOperations_IN;
    }
    if (anOp==BOP_CUT21) {
      aSt=BooleanOperations_OUT;
    }
  }
  return aSt;
}
//=======================================================================
// function: ToReverseSection
// purpose: 
//=======================================================================
  Standard_Boolean BOP_BuilderTools::ToReverseSection(const Standard_Integer iRank,
						      const BOP_Operation anOp)
{
  Standard_Boolean bFlag=Standard_False;

  if (iRank==1) {
    if (anOp==BOP_FUSE || anOp==BOP_CUT) {
      bFlag=Standard_True;
    }
  }

  if (iRank==2) {
    if (anOp==BOP_FUSE || anOp==BOP_CUT21) {
       bFlag=Standard_True;
    }
  }
  return bFlag;
}
//=======================================================================
// function: ToReverseFace
// purpose: 
//=======================================================================
  Standard_Boolean BOP_BuilderTools::ToReverseFace(const Standard_Integer iRank,
						   const BOP_Operation anOp)
{
  Standard_Boolean bFlag=Standard_False;

  if (iRank==1) {
    if (anOp==BOP_CUT21) {
      bFlag=Standard_True;
    }
  }

  if (iRank==2) {
    if (anOp==BOP_CUT) {
       bFlag=Standard_True;
    }
  }
  return bFlag;
}

//=======================================================================
// function: OrientSectionEdgeOnF1
// purpose: 
//=======================================================================
  void BOP_BuilderTools::OrientSectionEdgeOnF1(const TopoDS_Face& aF1,
					       const TopoDS_Face& aF2,
					       const Standard_Integer iRankF1,
					       const BOP_Operation anOperation,
					       TopoDS_Edge& aE)
{
  TopAbs_Orientation anOr;
  Standard_Boolean bToReverseSection;
  
  anOr=BOPTools_Tools3D::EdgeOrientation(aE, aF1, aF2);
  aE.Orientation(anOr);
  bToReverseSection=BOP_BuilderTools::ToReverseSection(iRankF1, anOperation);
  if (bToReverseSection) {
    aE.Reverse();
  }
}

//=======================================================================
// function: IsSameDomainFaceWithF1
// purpose: 
//=======================================================================
  Standard_Boolean
    BOP_BuilderTools::IsSameDomainFaceWithF1(const Standard_Integer nF1,
					     const Standard_Integer nF2,
					     const TColStd_IndexedMapOfInteger& aFFIndicesMap,
					     BOPTools_CArray1OfSSInterference& aFFs)
{
  Standard_Boolean bIsSDF, bYes;
  Standard_Integer j, aNbj, iFFx, nFx;
  
  bYes=Standard_False;
  aNbj=aFFIndicesMap.Extent();
  
  for (j=1; j<=aNbj; j++) {
    iFFx=aFFIndicesMap(j);
    BOPTools_SSInterference& aFFx=aFFs(iFFx);
    bIsSDF=aFFx.IsTangentFaces();
    if (bIsSDF) {
      nFx=aFFx.OppositeIndex(nF1);
      if (nFx==nF2) {
	return !bYes;
      }
    }
  }
  return bYes;
}

//=======================================================================
// function: IsPartIN2DToKeep
// purpose: 
//=======================================================================
  Standard_Boolean 
    BOP_BuilderTools::IsPartIN2DToKeep(const TopAbs_State aST1, 
				       const Standard_Integer iRankF1,
				       const BOP_Operation anOp)
{
  

  Standard_Boolean bRetFlag= (aST1==TopAbs_OUT);
  //
  // Same oriented 
  if (bRetFlag) {
    if (anOp==BOP_CUT && iRankF1==2) {
      return !bRetFlag;
    }

    if (anOp==BOP_CUT21 && iRankF1==1) {
      return !bRetFlag;
    }
    
    if (anOp==BOP_COMMON && iRankF1==2) {
      return !bRetFlag;
    }

    if (anOp==BOP_COMMON && iRankF1==1) {
      return !bRetFlag;
    }

    return bRetFlag;
  }
  //
  // Different oriented
  else if (!bRetFlag) {
    if (anOp==BOP_FUSE) {
      return bRetFlag;
    }
  
    if (anOp==BOP_CUT && iRankF1==1) {
      return bRetFlag;
    }
  
    if (anOp==BOP_CUT21 && iRankF1==2) {
      return bRetFlag;
    }
    return !bRetFlag;
  }
  return !bRetFlag;
}
//=======================================================================
// function: IsPartOn2dToKeep
// purpose: 
//=======================================================================
  Standard_Boolean 
    BOP_BuilderTools::IsPartOn2dToKeep(const TopAbs_State aST1,
				       const Standard_Integer iRankF1,
				       const BOP_Operation anOp)
{
  Standard_Boolean bRetFlag=Standard_False;
  //
  // 1 OUT
  if (aST1==TopAbs_OUT) {
    if (anOp==BOP_FUSE) {
      bRetFlag=Standard_True;
    }
    if (iRankF1==1) {
      if (anOp==BOP_CUT) {
	bRetFlag=Standard_True;
      }
    }
    if (iRankF1==2) {
      if (anOp==BOP_CUT21) {
	bRetFlag=Standard_True;
      }
    }
  }
  //
  // 2 IN
  if (aST1==TopAbs_IN) {
    if (anOp==BOP_COMMON) {
      bRetFlag=Standard_True;
    }
    if (iRankF1==2) {
      if (anOp==BOP_CUT) {
	bRetFlag=Standard_True;
      }
    }
    if (iRankF1==1) {
      if (anOp==BOP_CUT21) {
	bRetFlag=Standard_True;
      }
    }
  }
  return bRetFlag;
}

//=======================================================================
// function: DoMap
// purpose: 
//=======================================================================
  void BOP_BuilderTools::DoMap
    (BOPTools_CArray1OfSSInterference& aFFs,
     BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aFFMap)
{
  Standard_Integer i, aNb, nF1, nF2;
  //
  // Intersection Map for Faces
  
  aFFMap.Clear();
  TColStd_IndexedMapOfInteger aFFIndices;

  aNb=aFFs.Extent();
  for (i=1; i<=aNb; i++) {
    BOPTools_SSInterference& aFF=aFFs(i);
    // Faces
    nF1=aFF.Index1();
    nF2=aFF.Index2();
    // nF1
    if (!aFFMap.Contains(nF1)) {
      aFFIndices.Clear();
      aFFIndices.Add(i);
      aFFMap.Add(nF1, aFFIndices);
    }
    else {
      TColStd_IndexedMapOfInteger& aMF=aFFMap.ChangeFromKey(nF1);
      aMF.Add(i);
    }
    // nF2
    if (!aFFMap.Contains(nF2)) {
      aFFIndices.Clear();
      aFFIndices.Add(i);
      aFFMap.Add(nF2, aFFIndices);
    }
    else {
      TColStd_IndexedMapOfInteger& aMF=aFFMap.ChangeFromKey(nF2);
      aMF.Add(i);
    }
  }
}

//=======================================================================
//
static 
  void Pathx(const TopoDS_Shape& aVx,
	     const TopoDS_Shape& aEx,
	     const TopAbs_ShapeEnum aType2,
	     const TopTools_IndexedDataMapOfShapeListOfShape& aVEMap,
	     TopTools_IndexedMapOfOrientedShape& aProcessedEdges,
	     TopTools_IndexedMapOfOrientedShape& aCBMap);

//=======================================================================
// function: MakeConnexityBlocks
// purpose: 
//=======================================================================
  void BOP_BuilderTools::MakeConnexityBlocks(const TopTools_ListOfShape& aLEIn,
					     const TopAbs_ShapeEnum aType,
					     BOP_ListOfConnexityBlock& aLConBlks)
{
  Standard_Integer i, aNbV, aNbE;
  TopTools_IndexedDataMapOfShapeListOfShape aVEMap;
  TopTools_IndexedMapOfOrientedShape aProcessedEdges, aCBMap;
  TopAbs_ShapeEnum aType2;
  //
  if (aType==TopAbs_EDGE) {
    aType2=TopAbs_VERTEX;
  }
  else if (aType==TopAbs_FACE) {
    aType2=TopAbs_EDGE;
  }
  else {
    return;
  }
  //
  TopTools_ListIteratorOfListOfShape anIt(aLEIn);
  //
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aE=anIt.Value();
    TopExp::MapShapesAndAncestors(aE, aType2, aType, aVEMap);
  }
  //
  aNbV=aVEMap.Extent();
  for (i=1; i<=aNbV; i++) {
    aCBMap.Clear();
    
    const TopoDS_Shape& aV=aVEMap.FindKey(i);
    
    const TopTools_ListOfShape& aLE=aVEMap(i);
    anIt.Initialize(aLE);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aE=anIt.Value();
      if (!aProcessedEdges.Contains(aE)) {
	aProcessedEdges.Add(aE);
	aCBMap.Add(aE);
	Pathx (aV, aE, aType2, aVEMap, aProcessedEdges, aCBMap);
      }
    }
    aNbE=aCBMap.Extent();
    if (aNbE) {
      BOP_ConnexityBlock aConnexityBlock;
      aConnexityBlock.SetShapes(aCBMap);
      aLConBlks.Append(aConnexityBlock);
    }
  }
}

//=======================================================================
// function: Pathx
// purpose: 
//=======================================================================
void Pathx(const TopoDS_Shape& aVx,
	   const TopoDS_Shape& aEx,
	   const TopAbs_ShapeEnum aType2,
	   const TopTools_IndexedDataMapOfShapeListOfShape& aVEMap,
	   TopTools_IndexedMapOfOrientedShape& aProcessedEdges,
	   TopTools_IndexedMapOfOrientedShape& aCBMap)
{
  TopExp_Explorer anExp(aEx, aType2);
  for (; anExp.More(); anExp.Next()){
    const TopoDS_Shape& aV=anExp.Current();
    
    if (aV.IsSame(aVx)){
      continue;
    }
    
    const TopTools_ListOfShape& aLE=aVEMap.FindFromKey(aV);
    TopTools_ListIteratorOfListOfShape anIt(aLE);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aE=anIt.Value();
      if (!aProcessedEdges.Contains(aE)) {
	aProcessedEdges.Add(aE);
	aCBMap.Add(aE);
	
	Pathx (aV, aE, aType2, aVEMap, aProcessedEdges, aCBMap);
      }
    }
  }
}


