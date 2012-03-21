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

#include <QANewModTopOpe_Tools.ixx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BOPTools_VEInterference.hxx>
#include <BOPTools_VSInterference.hxx>
#include <BOPTools_EEInterference.hxx>
#include <BOPTools_ESInterference.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_CArray1OfESInterference.hxx>
#include <BOPTools_CArray1OfEEInterference.hxx>
#include <BOPTools_CArray1OfVVInterference.hxx>
#include <BOPTools_CArray1OfVEInterference.hxx>
#include <BOPTools_CArray1OfVSInterference.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BOPTools_PCurveMaker.hxx>
#include <BOPTools_DEProcessor.hxx>
#include <BOPTools_Tools3D.hxx>
#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_CommonBlockPool.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_StateFiller.hxx>
#include <BOPTools_Curve.hxx>

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Common.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <BOP_WireEdgeSet.hxx>
#include <BOP_SDFWESFiller.hxx>
#include <BOP_FaceBuilder.hxx>

#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <Geom_Surface.hxx>
#include <IntTools_Context.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>

#include <TCollection_CompareOfReal.hxx>
#include <SortTools_QuickSortOfReal.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <TColStd_IndexedMapOfReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

static Standard_Boolean CheckSameDomainFaceInside(const TopoDS_Face& theFace1,
						  const TopoDS_Face& theFace2);

static Standard_Boolean AddShapeToHistoryMap(const TopoDS_Shape& theOldShape,
					     const TopoDS_Shape& theNewShape,
					     TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap);

static void FillEdgeHistoryMap(BRepAlgoAPI_BooleanOperation&              theBOP,
			       TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap);

static void SortVertexOnEdge(const TopoDS_Edge&          theEdge,
			     const TopTools_ListOfShape& theListOfVertex,
			     TopTools_ListOfShape&       theListOfVertexSorted);

// ========================================================================================
// function: NbPoints
// purpose:
// ========================================================================================
Standard_Integer QANewModTopOpe_Tools::NbPoints(const BOPTools_PDSFiller& theDSFiller) 
{
  Standard_Integer i = 0, anbpoints = 0;
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  BOPTools_InterferencePool* anIntrPool = (BOPTools_InterferencePool*)&theDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs = anIntrPool->SSInterferences();
  Standard_Integer aNb=aFFs.Extent();

  for (i = 1; i <= aNb; i++) {
    BOPTools_SSInterference& aFFi = aFFs(i);
    TColStd_ListOfInteger& anAloneVertices = aFFi.AloneVertices();
    anbpoints += anAloneVertices.Extent();
  }
  BOPTools_CArray1OfESInterference& aEFs = anIntrPool->ESInterferences();
  aNb = aEFs.Extent();

  for (i = 1; i <= aNb; i++) {
    BOPTools_ESInterference& aESInterf = aEFs(i);
    Standard_Integer anIndex = aESInterf.NewShape();

    if(anIndex == 0)
      continue;

    if(aDS.GetShapeType(anIndex) == TopAbs_VERTEX)
      anbpoints++;
  }

  BOPTools_CArray1OfEEInterference& aEEs = anIntrPool->EEInterferences();
  aNb = aEEs.Extent();

  for (i = 1; i <= aNb; i++) {
    BOPTools_EEInterference& aEEInterf = aEEs(i);
    Standard_Integer anIndex = aEEInterf.NewShape();

    if(anIndex == 0)
      continue;

    if(aDS.GetShapeType(anIndex) == TopAbs_VERTEX)
      anbpoints++;
  }
  return anbpoints;
}

// ========================================================================================
// function: NewVertex
// purpose:
// ========================================================================================
TopoDS_Shape QANewModTopOpe_Tools::NewVertex(const BOPTools_PDSFiller& theDSFiller, 
					const Standard_Integer    theIndex) 
{
  TopoDS_Shape aVertex;

  Standard_Integer i = 0, anbpoints = 0;
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  BOPTools_InterferencePool* anIntrPool = (BOPTools_InterferencePool*)&theDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs = anIntrPool->SSInterferences();
  Standard_Integer aNb=aFFs.Extent();

  for (i = 1; i <= aNb; i++) {
    BOPTools_SSInterference& aFFi = aFFs(i);
    TColStd_ListOfInteger& anAloneVertices = aFFi.AloneVertices();
    TColStd_ListIteratorOfListOfInteger anIt(anAloneVertices);

    for(; anIt.More(); anIt.Next()) {
      anbpoints++;

      if(theIndex == anbpoints) {
	return aDS.Shape(anIt.Value());
      }
    }
  }
  BOPTools_CArray1OfESInterference& aEFs = anIntrPool->ESInterferences();
  aNb = aEFs.Extent();

  for (i = 1; i <= aNb; i++) {
    BOPTools_ESInterference& aESInterf = aEFs(i);
    Standard_Integer anIndex = aESInterf.NewShape();

    if(anIndex == 0)
      continue;

    if(aDS.GetShapeType(anIndex) == TopAbs_VERTEX) {
      anbpoints++;

      if(theIndex == anbpoints) {
	return aDS.Shape(anIndex);
      }
    }
  }

  BOPTools_CArray1OfEEInterference& aEEs = anIntrPool->EEInterferences();
  aNb = aEEs.Extent();

  for (i = 1; i <= aNb; i++) {
    BOPTools_EEInterference& aEEInterf = aEEs(i);
    Standard_Integer anIndex = aEEInterf.NewShape();

    if(anIndex == 0)
      continue;

    if(aDS.GetShapeType(anIndex) == TopAbs_VERTEX) {
      anbpoints++;

      if(theIndex == anbpoints) {
	return aDS.Shape(anIndex);
      }
    }
  }
  return aVertex;
}

// ========================================================================================
// function: HasSameDomain
// purpose:
// ========================================================================================
Standard_Boolean QANewModTopOpe_Tools::HasSameDomain(const BOPTools_PDSFiller& theDSFiller,
						const TopoDS_Shape&       theFace) 
{
  if(theFace.IsNull() || (theFace.ShapeType() != TopAbs_FACE))
    return Standard_False;
  const BOPTools_PaveFiller& aPaveFiller = theDSFiller->PaveFiller();
  BOPTools_PCurveMaker aPCurveMaker(aPaveFiller);
  aPCurveMaker.Do();

  BOPTools_DEProcessor aDEProcessor(aPaveFiller);
  aDEProcessor.Do();

  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  BOPTools_InterferencePool* pIntrPool = (BOPTools_InterferencePool*)&theDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs = pIntrPool->SSInterferences();
  //
  Standard_Boolean bFlag;
  Standard_Integer i, aNb, nF1, nF2, iZone, aNbSps, iSenseFlag;
  gp_Dir aDNF1, aDNF2;

  aNb=aFFs.Extent();

  for (i = 1; i <= aNb; i++) {
    bFlag=Standard_False;
    
    BOPTools_SSInterference& aFF=aFFs(i);
    
    nF1=aFF.Index1();
    nF2=aFF.Index2();
    const TopoDS_Face& aF1 = TopoDS::Face(aDS.Shape(nF1));
    const TopoDS_Face& aF2 = TopoDS::Face(aDS.Shape(nF2));

    if(!theFace.IsSame(aF1) && !theFace.IsSame(aF2))
      continue;

    const BOPTools_ListOfPaveBlock& aLPB = aFF.PaveBlocks();
    aNbSps = aLPB.Extent();

    if (!aNbSps) {
      continue;
    }
    const BOPTools_PaveBlock& aPB = aLPB.First();
    const TopoDS_Edge& aSpE = TopoDS::Edge(aDS.Shape(aPB.Edge()));
    
    BOPTools_Tools3D::GetNormalToFaceOnEdge (aSpE, aF1, aDNF1); 
    BOPTools_Tools3D::GetNormalToFaceOnEdge (aSpE, aF2, aDNF2);
    iSenseFlag=BOPTools_Tools3D::SenseFlag (aDNF1, aDNF2);

    if (iSenseFlag==1 || iSenseFlag==-1) {
    //
    //
      TopoDS_Face aF1FWD=aF1;
      aF1FWD.Orientation (TopAbs_FORWARD);
      
      BOP_WireEdgeSet aWES (aF1FWD);
      BOP_SDFWESFiller aWESFiller(nF1, nF2, *theDSFiller);
      aWESFiller.SetSenseFlag(iSenseFlag);
      aWESFiller.SetOperation(BOP_COMMON);
      aWESFiller.Do(aWES);
      
      BOP_FaceBuilder aFB;
      aFB.Do(aWES);
      const TopTools_ListOfShape& aLF=aFB.NewFaces();

      iZone = 0;
      TopTools_ListIteratorOfListOfShape anIt(aLF);

      for (; anIt.More(); anIt.Next()) {
	const TopoDS_Shape& aFR=anIt.Value();

	if (aFR.ShapeType()==TopAbs_FACE) {
	  const TopoDS_Face& aFaceResult=TopoDS::Face(aFR);
	  //
	  Standard_Boolean bIsValidIn2D, bNegativeFlag;
	  bIsValidIn2D=BOPTools_Tools3D::IsValidArea (aFaceResult, bNegativeFlag);

	  if (bIsValidIn2D) { 

	    if(CheckSameDomainFaceInside(aFaceResult, aF2)) {
	      iZone = 1;
	      break;
	    }
	  }
	}
      }

      if (iZone) { 
	bFlag = Standard_True;
	aFF.SetStatesMap(aWESFiller.StatesMap());
      }
    }
    aFF.SetTangentFacesFlag(bFlag);
    aFF.SetSenseFlag (iSenseFlag);

    if(bFlag) {
      return Standard_True;
    }
  }
  return Standard_False;
}

// ========================================================================================
// function: SameDomain
// purpose:
// ========================================================================================
void QANewModTopOpe_Tools::SameDomain(const BOPTools_PDSFiller& theDSFiller,
				 const TopoDS_Shape&       theFace,
				 TopTools_ListOfShape&     theResultList) 
{
  theResultList.Clear();

  if(theFace.IsNull() || (theFace.ShapeType() != TopAbs_FACE))
    return;
  const BOPTools_PaveFiller& aPaveFiller = theDSFiller->PaveFiller();
  BOPTools_PCurveMaker aPCurveMaker(aPaveFiller);
  aPCurveMaker.Do();

  BOPTools_DEProcessor aDEProcessor(aPaveFiller);
  aDEProcessor.Do();

  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  BOPTools_InterferencePool* pIntrPool = (BOPTools_InterferencePool*)&theDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs = pIntrPool->SSInterferences();
  //
  Standard_Integer i, aNb, nF1, nF2,  aNbSps, iSenseFlag;
  gp_Dir aDNF1, aDNF2;

  aNb=aFFs.Extent();

  for (i = 1; i <= aNb; i++) {
    BOPTools_SSInterference& aFF=aFFs(i);
    
    nF1=aFF.Index1();
    nF2=aFF.Index2();
    const TopoDS_Face& aF1 = TopoDS::Face(aDS.Shape(nF1));
    const TopoDS_Face& aF2 = TopoDS::Face(aDS.Shape(nF2));

    if(!theFace.IsSame(aF1) && !theFace.IsSame(aF2))
      continue;

    if(aFF.IsTangentFaces()) {
      if(theFace.IsSame(aF1))
	theResultList.Append(aF2);
      else
	theResultList.Append(aF1);
      continue;
    }

    const BOPTools_ListOfPaveBlock& aLPB = aFF.PaveBlocks();
    aNbSps = aLPB.Extent();

    if (!aNbSps) {
      continue;
    }
    const BOPTools_PaveBlock& aPB = aLPB.First();
    const TopoDS_Edge& aSpE = TopoDS::Edge(aDS.Shape(aPB.Edge()));
    
    BOPTools_Tools3D::GetNormalToFaceOnEdge (aSpE, aF1, aDNF1); 
    BOPTools_Tools3D::GetNormalToFaceOnEdge (aSpE, aF2, aDNF2);
    iSenseFlag=BOPTools_Tools3D::SenseFlag (aDNF1, aDNF2);

    if (iSenseFlag==1 || iSenseFlag==-1) {
    //
    //
      TopoDS_Face aF1FWD=aF1;
      aF1FWD.Orientation (TopAbs_FORWARD);
      
      BOP_WireEdgeSet aWES (aF1FWD);
      BOP_SDFWESFiller aWESFiller(nF1, nF2, *theDSFiller);
      aWESFiller.SetSenseFlag(iSenseFlag);
      aWESFiller.SetOperation(BOP_COMMON);
      aWESFiller.Do(aWES);
      
      BOP_FaceBuilder aFB;
      aFB.Do(aWES);
      const TopTools_ListOfShape& aLF=aFB.NewFaces();

      TopTools_ListIteratorOfListOfShape anIt(aLF);

      for (; anIt.More(); anIt.Next()) {
	const TopoDS_Shape& aFR=anIt.Value();

	if (aFR.ShapeType()==TopAbs_FACE) {
	  const TopoDS_Face& aFaceResult=TopoDS::Face(aFR);
	  //
	  Standard_Boolean bIsValidIn2D, bNegativeFlag;
	  bIsValidIn2D=BOPTools_Tools3D::IsValidArea (aFaceResult, bNegativeFlag);

	  if (bIsValidIn2D) { 

	    if(CheckSameDomainFaceInside(aFaceResult, aF2)) {
	      if(theFace.IsSame(aF1))
		theResultList.Append(aF2);
	      else
		theResultList.Append(aF1);
	      break;
	    }
	  }
	}
      }
    }
  }
}

// ========================================================================================
// function: IsSplit
// purpose:
// ========================================================================================
Standard_Boolean QANewModTopOpe_Tools::IsSplit(const BOPTools_PDSFiller& theDSFiller,
					  const TopoDS_Shape&       theEdge,
					  const TopAbs_State        theState) 
{
  if(theEdge.IsNull() || (theEdge.ShapeType() != TopAbs_EDGE))
    return Standard_False;
  const BOPTools_SplitShapesPool& aSplitShapesPool = theDSFiller->SplitShapesPool();
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();

  const BooleanOperations_IndexedDataMapOfShapeInteger& aMap1 = aDS.ShapeIndexMap(1);
  const BooleanOperations_IndexedDataMapOfShapeInteger& aMap2 = aDS.ShapeIndexMap(2);
  Standard_Integer anIndex = 0;

  if(aMap1.Contains(theEdge))
    anIndex = aDS.ShapeIndex(theEdge, 1);
  else if(aMap2.Contains(theEdge))
    anIndex = aDS.ShapeIndex(theEdge, 2);
  else
    return Standard_False;

  const BOPTools_ListOfPaveBlock& aSplits = aSplitShapesPool(aDS.RefEdge(anIndex));
  BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSplits);

  for (; aPBIt.More(); aPBIt.Next()) {
    BOPTools_PaveBlock& aPB = aPBIt.Value();
    Standard_Integer    nSp = aPB.Edge();
    TopAbs_State aSplitState = BOPTools_StateFiller::ConvertState(aDS.GetState(nSp));

    if(aSplitState == theState)
      return Standard_True;
  }

//   if(theState == TopAbs_ON) {
//     const BOPTools_CommonBlockPool& aCommonBlockPool= theDSFiller->CommonBlockPool();
//     const BOPTools_ListOfCommonBlock& aCBlocks = aCommonBlockPool(aDS.RefEdge(anIndex));

//     if(!aCBlocks.IsEmpty()) 
//       return Standard_True;
//   }
  return Standard_False;
}

// ========================================================================================
// function: Splits
// purpose:
// ========================================================================================
void QANewModTopOpe_Tools::Splits(const BOPTools_PDSFiller& theDSFiller,
			     const TopoDS_Shape&       theEdge,
			     const TopAbs_State        theState,
			     TopTools_ListOfShape&     theResultList) 
{
  theResultList.Clear();

  if(theEdge.IsNull() || (theEdge.ShapeType() != TopAbs_EDGE))
    return;
  const BOPTools_SplitShapesPool& aSplitShapesPool = theDSFiller->SplitShapesPool();
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();

  const BooleanOperations_IndexedDataMapOfShapeInteger& aMap1 = aDS.ShapeIndexMap(1);
  const BooleanOperations_IndexedDataMapOfShapeInteger& aMap2 = aDS.ShapeIndexMap(2);
  Standard_Integer anIndex = 0;

  if(aMap1.Contains(theEdge))
    anIndex = aDS.ShapeIndex(theEdge, 1);
  else if(aMap2.Contains(theEdge))
    anIndex = aDS.ShapeIndex(theEdge, 2);
  else
    return;

  const BOPTools_ListOfPaveBlock& aSplits = aSplitShapesPool(aDS.RefEdge(anIndex));
  BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSplits);
//   TopTools_MapOfShape aMapE;

  for (; aPBIt.More(); aPBIt.Next()) {
    BOPTools_PaveBlock& aPB = aPBIt.Value();
    Standard_Integer    nSp = aPB.Edge();
    TopAbs_State aSplitState = BOPTools_StateFiller::ConvertState(aDS.GetState(nSp));

    if(aSplitState == theState) {
      TopoDS_Shape aSplit = aDS.Shape(nSp);
      theResultList.Append(aSplit);
//       aMapE.Add(aSplit);
    }
  }

//   if(theState == TopAbs_ON) {
//     const BOPTools_CommonBlockPool& aCommonBlockPool= theDSFiller->CommonBlockPool();
//     const BOPTools_ListOfCommonBlock& aCBlocks = aCommonBlockPool(aDS.RefEdge(anIndex));
//     BOPTools_ListIteratorOfListOfCommonBlock anIt(aCBlocks);
    
//     for(; anIt.More(); anIt.Next()) {
//       const BOPTools_CommonBlock& aCB = anIt.Value();
//       BOPTools_CommonBlock* pCB=(BOPTools_CommonBlock*) &aCB;
//       BOPTools_PaveBlock& aPB = pCB->PaveBlock1(anIndex);
//       Standard_Integer    nSp = aPB.Edge();
//       TopoDS_Shape aSplit = aDS.Shape(nSp);

//       if(aMapE.Contains(aSplit))
// 	continue;
//       theResultList.Append(aSplit);
//       aMapE.Add(aSplit);
//     }
//   }
}

// ========================================================================================
// function: SplitE
// purpose:
// ========================================================================================
Standard_Boolean QANewModTopOpe_Tools::SplitE(const TopoDS_Edge&    theEdge,
					 TopTools_ListOfShape& theSplits) 
{
  // prequesitory : <Eanc> is a valid edge.
  TopAbs_Orientation oEanc = theEdge.Orientation();
  TopoDS_Shape aLocalShape = theEdge.Oriented(TopAbs_FORWARD);
  TopoDS_Edge EFOR = TopoDS::Edge(aLocalShape);
  TopTools_ListOfShape aListOfVertex;
  TopExp_Explorer exv(EFOR,TopAbs_VERTEX);  

  for (;exv.More(); exv.Next()) {
    const TopoDS_Shape& v = exv.Current();
    aListOfVertex.Append(v);
  }
  Standard_Integer nv = aListOfVertex.Extent();

  if (nv <= 2) return Standard_False;
  TopTools_ListOfShape aListOfVertexSorted;

  SortVertexOnEdge(EFOR, aListOfVertex, aListOfVertexSorted);

  TopoDS_Vertex v0;
  TopTools_ListIteratorOfListOfShape anIt(aListOfVertexSorted);

  if (anIt.More()) {
    v0 = TopoDS::Vertex(anIt.Value()); 
    anIt.Next();
  }
  else return Standard_False;

  for (; anIt.More(); anIt.Next()) {
    TopoDS_Vertex v = TopoDS::Vertex(anIt.Value());
    
    // prequesitory: par0 < par
    Standard_Real par0 = BRep_Tool::Parameter(v0, EFOR);
    Standard_Real par  = BRep_Tool::Parameter(v, EFOR);

    // here, ed has the same geometries than Ein, but with no subshapes.
    TopoDS_Edge ed = TopoDS::Edge(EFOR.EmptyCopied());
    BRep_Builder BB;
    v0.Orientation(TopAbs_FORWARD); 
    BB.Add(ed, v0);
    v.Orientation(TopAbs_REVERSED); 
    BB.Add(ed, v);
    BB.Range(ed, par0, par);

    theSplits.Append(ed.Oriented(oEanc));
    v0 = v;
  }
  return Standard_True;
}


// ========================================================================================
// function: EdgeCurveAncestors
// purpose:
// ========================================================================================
 Standard_Boolean QANewModTopOpe_Tools::EdgeCurveAncestors(const BOPTools_PDSFiller& theDSFiller,
						      const TopoDS_Shape&       theEdge,
						      TopoDS_Shape&             theFace1,
						      TopoDS_Shape&             theFace2) 
{
  theFace1.Nullify();
  theFace2.Nullify();

  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  BOPTools_InterferencePool* anIntrPool = (BOPTools_InterferencePool*)&theDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs = anIntrPool->SSInterferences();
  Standard_Integer aNb = aFFs.Extent();
  Standard_Integer i = 0, j = 0;

  for (i = 1; i <= aNb; i++) {
    BOPTools_SSInterference& aFFi = aFFs(i);
    BOPTools_SequenceOfCurves& aBCurves = aFFi.Curves();
    Standard_Integer aNbCurves = aBCurves.Length();

    for (j = 1; j <= aNbCurves; j++) {
      BOPTools_Curve& aBC = aBCurves(j);
      const BOPTools_ListOfPaveBlock& aSectEdges = aBC.NewPaveBlocks();
      
      BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSectEdges);

      for (; aPBIt.More(); aPBIt.Next()) {
	BOPTools_PaveBlock& aPB=aPBIt.Value();
	Standard_Integer nSect = aPB.Edge();

	if(theEdge.IsSame(aDS.Shape(nSect))) {
	  Standard_Integer nF1 = aFFi.Index1();
	  Standard_Integer nF2 = aFFi.Index2();
	  theFace1 = aDS.Shape(nF1);
	  theFace2 = aDS.Shape(nF2);
	  return Standard_True;
	}
      }
    }
  }
  return Standard_False;
}

// ========================================================================================
// function: EdgeSectionAncestors
// purpose:
// ========================================================================================
Standard_Boolean QANewModTopOpe_Tools::EdgeSectionAncestors(const BOPTools_PDSFiller& theDSFiller,
						       const TopoDS_Shape&       theEdge,
						       TopTools_ListOfShape&     LF1,
						       TopTools_ListOfShape&     LF2,
						       TopTools_ListOfShape&     LE1,
						       TopTools_ListOfShape&     LE2) 
{
  if(theEdge.ShapeType() != TopAbs_EDGE)
    return Standard_False;
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  Standard_Integer i = 0, nb = 0;
  nb = aDS.NumberOfSourceShapes();

  for(i = 1; i <= nb; i++) {
    if(aDS.GetShapeType(i) != TopAbs_EDGE)
      continue;

    const BOPTools_CommonBlockPool&   aCommonBlockPool = theDSFiller->CommonBlockPool();
    const BOPTools_ListOfCommonBlock& aCBlocks         = aCommonBlockPool(aDS.RefEdge(i));
    BOPTools_ListIteratorOfListOfCommonBlock anIt(aCBlocks);
    
    for(; anIt.More(); anIt.Next()) {
      const BOPTools_CommonBlock& aCB    = anIt.Value();
      BOPTools_CommonBlock*       pCB    = (BOPTools_CommonBlock*) &aCB;
      BOPTools_PaveBlock&         aPB    = pCB->PaveBlock1(i);
      Standard_Integer            nSp    = aPB.Edge();
      TopoDS_Shape                aSplit = aDS.Shape(nSp);

      if(!theEdge.IsSame(aSplit))
	continue;

      if(aDS.Rank(i) == 1)
	LE1.Append(aDS.Shape(i));
      else
	LE2.Append(aDS.Shape(i));
      Standard_Integer nFace = aCB.Face();

      if(aCB.Face()) {
	if(aDS.Rank(nFace) == 1)
	  LF1.Append(aDS.Shape(nFace));
	else
	  LF2.Append(aDS.Shape(nFace));
      }
      // find edge ancestors.begin
      TopTools_IndexedMapOfShape aMapF;
      Standard_Integer j = 0, k = 0;

      for(j = 1; j <= aDS.NumberOfAncestors(i); j++) {
	Standard_Integer aAncestor1 = aDS.GetAncestor(i, j);

	for(k = 1; k <= aDS.NumberOfAncestors(aAncestor1); k++) {
	  Standard_Integer aAncestor2 = aDS.GetAncestor(aAncestor1, k);

	  if(aDS.GetShapeType(aAncestor2) == TopAbs_FACE) {
	    const TopoDS_Shape& aFace = aDS.Shape(aAncestor2);

	    if(aMapF.Contains(aFace))
	      continue;

	    if(aDS.Rank(i) == 1)
	      LF1.Append(aFace);
	    else
	      LF2.Append(aFace);
	    aMapF.Add(aFace);
	  }
	}
      }
      // find edge ancestors.end
    }
  }
  Standard_Boolean r = (!LF1.IsEmpty() && !LF2.IsEmpty());
  r = r && (!LE1.IsEmpty() || !LE2.IsEmpty());
  return r;
}

// ========================================================================================
// function: BoolOpe
// purpose:
// ========================================================================================
Standard_Boolean QANewModTopOpe_Tools::BoolOpe(const TopoDS_Shape& theFace1,
					  const TopoDS_Shape& theFace2,
					  Standard_Boolean&   IsCommonFound,
					  TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap) 
{
  IsCommonFound = Standard_False;
  theHistoryMap.Clear();

  BOPTools_DSFiller aDSFiller;
  aDSFiller.SetShapes(theFace1, theFace2);

  if (!aDSFiller.IsDone()) {
    return Standard_False;
  }
  aDSFiller.Perform();
  const BooleanOperations_ShapesDataStructure& aDS = aDSFiller.DS();
  BOPTools_InterferencePool* anIntrPool = (BOPTools_InterferencePool*)&aDSFiller.InterfPool();
  Standard_Integer aNb = 0;
  Standard_Integer i = 0, j = 0;
  TopTools_IndexedMapOfShape aMapV;
  {
    BRepAlgoAPI_Common aCommon(theFace1, theFace2, aDSFiller);

    if(!aCommon.IsDone()) {
      return Standard_False;
    }
    TopExp_Explorer anExp(aCommon.Shape(), TopAbs_FACE);
    
    if(!anExp.More()) {
      IsCommonFound = Standard_False;
      return Standard_True;
    }
    IsCommonFound = Standard_True;
    TopExp::MapShapes(aCommon.Shape(), TopAbs_VERTEX, aMapV);
    // fill edge history.begin
    FillEdgeHistoryMap(aCommon, theHistoryMap);
    // fill edge history.end

    // fill face history.begin
    BOPTools_CArray1OfSSInterference& aFFs = anIntrPool->SSInterferences();
    aNb = aFFs.Extent();
    Standard_Boolean bReverseFlag = Standard_True;
    Standard_Boolean fillhistory = Standard_True;

    for (i=1; i<=aNb; i++) {
      BOPTools_SSInterference& aFF = aFFs(i);

      if(aFF.SenseFlag() == 1) {
	fillhistory = Standard_True;
	bReverseFlag = Standard_False;
      }
      else if(aFF.SenseFlag() == -1) {
	fillhistory = Standard_True;
	bReverseFlag = Standard_True;
      }
      else
	fillhistory = Standard_False;
    }

    if(fillhistory) {

      for(; anExp.More(); anExp.Next()) {
	TopoDS_Shape aResShape = anExp.Current();

	if(theFace1.Orientation() == aResShape.Orientation()) {
	  AddShapeToHistoryMap(theFace1, aResShape, theHistoryMap);

	  if(bReverseFlag)
	    aResShape.Reverse();
	  AddShapeToHistoryMap(theFace2, aResShape, theHistoryMap);
	}
	else if(theFace2.Orientation() == aResShape.Orientation()) {
	  AddShapeToHistoryMap(theFace2, aResShape, theHistoryMap);

	  if(bReverseFlag)
	    aResShape.Reverse();
	  AddShapeToHistoryMap(theFace1, aResShape, theHistoryMap);
	}
	else {
	  aResShape.Orientation(theFace1.Orientation());
	  AddShapeToHistoryMap(theFace1, aResShape, theHistoryMap);
	  aResShape.Orientation(theFace2.Orientation());

	  if(bReverseFlag)
	    aResShape.Reverse();
	  AddShapeToHistoryMap(theFace2, aResShape, theHistoryMap);
	}
      }
    }
    // fill face history.end
  }
  {
    BRepAlgoAPI_Cut aCut1(theFace1, theFace2, aDSFiller);

    if(!aCut1.IsDone())
      return Standard_False;
    TopExp::MapShapes(aCut1.Shape(), TopAbs_VERTEX, aMapV);
    // fill edge history.begin
    FillEdgeHistoryMap(aCut1, theHistoryMap);
    // fill edge history.end

    // fill face history.begin
    TopExp_Explorer anExp(aCut1.Shape(), TopAbs_FACE);

    for(; anExp.More(); anExp.Next()) {
      TopoDS_Shape aResShape = anExp.Current();
      aResShape.Orientation(theFace1.Orientation());
      AddShapeToHistoryMap(theFace1, aResShape, theHistoryMap);
    }
    // fill face history.end
  }

  {
    BRepAlgoAPI_Cut aCut2(theFace1, theFace2, aDSFiller, Standard_False);

    if(!aCut2.IsDone())
      return Standard_False;
    TopExp::MapShapes(aCut2.Shape(), TopAbs_VERTEX, aMapV);
    // fill edge history.begin
    FillEdgeHistoryMap(aCut2, theHistoryMap);
    // fill edge history.end

    // fill face history.begin
    TopExp_Explorer anExp(aCut2.Shape(), TopAbs_FACE);

    for(; anExp.More(); anExp.Next()) {
      TopoDS_Shape aResShape = anExp.Current();
      aResShape.Orientation(theFace2.Orientation());
      AddShapeToHistoryMap(theFace2, aResShape, theHistoryMap);
    }
    // fill face history.end
  }
  
  // fill vertex history.begin
  BOPTools_CArray1OfVVInterference& aVVs = anIntrPool->VVInterferences();
  aNb = aVVs.Extent();

  for (i = 1; i <= aNb; i++) {
    BOPTools_VVInterference& aVVi = aVVs(i);
    Standard_Integer aNewShapeIndex = aVVi.NewShape();

    if(aNewShapeIndex == 0)
      continue;
    const TopoDS_Shape& aNewVertex = aDS.Shape(aNewShapeIndex);

    if(!aMapV.Contains(aNewVertex))
      continue;
    const TopoDS_Shape& aV1 = aDS.Shape(aVVi.Index1());
    const TopoDS_Shape& aV2 = aDS.Shape(aVVi.Index2());
    AddShapeToHistoryMap(aV1, aNewVertex, theHistoryMap);
    AddShapeToHistoryMap(aV2, aNewVertex, theHistoryMap);
  }
  BOPTools_CArray1OfVEInterference& aVEs = anIntrPool->VEInterferences();
  aNb = aVEs.Extent();

  for (i = 1; i <= aNb; i++) {
    BOPTools_VEInterference& aVEi = aVEs(i);
    Standard_Integer aNewShapeIndex = aVEi.NewShape();

    if(aNewShapeIndex == 0)
      continue;
    const TopoDS_Shape& aNewVertex = aDS.Shape(aNewShapeIndex);

    if(!aMapV.Contains(aNewVertex))
      continue;
    Standard_Integer anIndex = 0;

    if(aDS.GetShapeType(aVEi.Index1()) == TopAbs_VERTEX)
      anIndex = aVEi.Index1();
    else if(aDS.GetShapeType(aVEi.Index2()) == TopAbs_VERTEX)
      anIndex = aVEi.Index2();
    else
      continue;
    const TopoDS_Shape& aV = aDS.Shape(anIndex);
    AddShapeToHistoryMap(aV, aNewVertex, theHistoryMap);
  }
  BOPTools_CArray1OfVSInterference& aVSs = anIntrPool->VSInterferences();
  aNb = aVSs.Extent();

  for (i = 1; i <= aNb; i++) {
    BOPTools_VSInterference& aVSi = aVSs(i);
    Standard_Integer aNewShapeIndex = aVSi.NewShape();

    if(aNewShapeIndex == 0)
      continue;
    const TopoDS_Shape& aNewVertex = aDS.Shape(aNewShapeIndex);

    if(!aMapV.Contains(aNewVertex))
      continue;
    Standard_Integer anIndex = 0;

    if(aDS.GetShapeType(aVSi.Index1()) == TopAbs_VERTEX)
      anIndex = aVSi.Index1();
    else if(aDS.GetShapeType(aVSi.Index2()) == TopAbs_VERTEX)
      anIndex = aVSi.Index2();
    else
      continue;
    const TopoDS_Shape& aV = aDS.Shape(anIndex);
    AddShapeToHistoryMap(aV, aNewVertex, theHistoryMap);
  }
  // fill vertex history.end
  return Standard_True;
}

// -----------------------------------------------------------------
// static function: CheckSameDomainFaceInside
// purpose: Check if distance between several points of theFace1 and
//          theFace2 is not more than sum of maximum of tolerances of
//          theFace1's edges and tolerance of theFace2
// -----------------------------------------------------------------
Standard_Boolean CheckSameDomainFaceInside(const TopoDS_Face& theFace1,
					   const TopoDS_Face& theFace2) {

  Standard_Real umin = 0., umax = 0., vmin = 0., vmax = 0.;
  BRepTools::UVBounds(theFace1, umin, umax, vmin, vmax);
  IntTools_Context aContext;
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(theFace1);
  Standard_Real aTolerance = BRep_Tool::Tolerance(theFace1);

  TopExp_Explorer anExpE(theFace1, TopAbs_EDGE);

  for(; anExpE.More(); anExpE.Next()) {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExpE.Current());
    Standard_Real anEdgeTol = BRep_Tool::Tolerance(anEdge);
    aTolerance = (aTolerance < anEdgeTol) ? anEdgeTol : aTolerance;
  }
  aTolerance += BRep_Tool::Tolerance(theFace2);

  Standard_Integer nbpoints = 5;
  Standard_Real adeltau = (umax - umin) / (nbpoints + 1);
  Standard_Real adeltav = (vmax - vmin) / (nbpoints + 1);
  Standard_Real U = umin + adeltau;
  GeomAPI_ProjectPointOnSurf& aProjector = aContext.ProjPS(theFace2);

  for(Standard_Integer i = 1; i <= nbpoints; i++, U+=adeltau) {
    Standard_Real V = vmin + adeltav;

    for(Standard_Integer j = 1; j <= nbpoints; j++, V+=adeltav) {
      gp_Pnt2d aPoint(U,V);

      if(aContext.IsPointInFace(theFace1, aPoint)) {
	gp_Pnt aP3d = aSurface->Value(U, V);
	aProjector.Perform(aP3d);

	if(aProjector.IsDone()) {

	  if(aProjector.LowerDistance() > aTolerance)
	    return Standard_False;
	}
      }
    }
  }

  return Standard_True;
}

// --------------------------------------------------------------------------------------------
// static function: AddShapeToHistoryMap
// purpose: 
// --------------------------------------------------------------------------------------------
Standard_Boolean AddShapeToHistoryMap(const TopoDS_Shape& theOldShape,
				      const TopoDS_Shape& theNewShape,
				      TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap) {

  if(!theHistoryMap.Contains(theOldShape)) {
    TopTools_ListOfShape aList;
    aList.Append(theNewShape);
    theHistoryMap.Add(theOldShape, aList);
    return Standard_True;
  }
  
  Standard_Boolean found = Standard_False;
  TopTools_ListOfShape& aList = theHistoryMap.ChangeFromKey(theOldShape);
  TopTools_ListIteratorOfListOfShape aVIt(aList);

  for(; aVIt.More(); aVIt.Next()) {
    if(theNewShape.IsSame(aVIt.Value())) {
      found = Standard_True;
      break;
    }
  }

  if(!found) {
    aList.Append(theNewShape);
  }
  return !found;
}

// --------------------------------------------------------------------------------------------
// static function: FillEdgeHistoryMap
// purpose: 
// --------------------------------------------------------------------------------------------
void FillEdgeHistoryMap(BRepAlgoAPI_BooleanOperation&              theBOP,
			TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap) {

  TopExp_Explorer anExp;
  anExp.Init(theBOP.Shape1(), TopAbs_EDGE);

  for(; anExp.More(); anExp.Next()) {
    const TopTools_ListOfShape& aList = theBOP.Modified(anExp.Current());
    TopTools_ListIteratorOfListOfShape anIt(aList);

    for(; anIt.More(); anIt.Next()) {
      AddShapeToHistoryMap(anExp.Current(), anIt.Value(), theHistoryMap);
    }
  }

  anExp.Init(theBOP.Shape2(), TopAbs_EDGE);

  for(; anExp.More(); anExp.Next()) {
    const TopTools_ListOfShape& aList = theBOP.Modified(anExp.Current());
    TopTools_ListIteratorOfListOfShape anIt(aList);

    for(; anIt.More(); anIt.Next()) {
      AddShapeToHistoryMap(anExp.Current(), anIt.Value(), theHistoryMap);
    }
  }
}

// --------------------------------------------------------------------------------------------
// static function: SortVertexOnEdge
// purpose: 
// --------------------------------------------------------------------------------------------
void SortVertexOnEdge(const TopoDS_Edge&          theEdge,
		      const TopTools_ListOfShape& theListOfVertex,
		      TopTools_ListOfShape&       theListOfVertexSorted) {

  TopTools_DataMapOfIntegerShape mapiv;// mapiv.Find(iV) = V
  TColStd_IndexedMapOfReal mappar;     // mappar.FindIndex(parV) = iV
  TopTools_ListIteratorOfListOfShape itlove(theListOfVertex);
  
  for (; itlove.More(); itlove.Next()){
    const TopoDS_Vertex& v = TopoDS::Vertex(itlove.Value());
    Standard_Real par = BRep_Tool::Parameter(v, theEdge);
    Standard_Integer iv = mappar.Add(par);
    mapiv.Bind(iv,v);
  }
  Standard_Integer nv = mapiv.Extent();
  TColStd_Array1OfReal tabpar(1,nv);
  Standard_Integer i = 0;

  for ( i = 1; i <= nv; i++) {
    Standard_Real p = mappar.FindKey(i);
    tabpar.SetValue(i,p);
  }
  theListOfVertexSorted.Clear();
  TCollection_CompareOfReal compare;
  SortTools_QuickSortOfReal::Sort(tabpar, compare);

  for (i = 1; i <= nv; i++) {
    Standard_Real par = tabpar.Value(i);
    Standard_Integer iv = mappar.FindIndex(par);
    const TopoDS_Shape& v = mapiv.Find(iv);
    theListOfVertexSorted.Append(v);
  }
}
