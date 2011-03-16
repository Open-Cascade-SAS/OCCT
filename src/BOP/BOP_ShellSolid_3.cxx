// File:	BOP_ShellSolid_3.cxx
// Created:	Wed Jun  9 15:16:44 2004
// Author:	Mikhail KLOKOV
//		<mkk@kurox>


#include <BOP_ShellSolid.ixx>

#include <TopAbs_Orientation.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopExp.hxx>

#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

#include <BRepClass3d_SolidClassifier.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>

#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>

#include <BOPTools_Tools3D.hxx>

#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_CommonBlockPool.hxx>

#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_CommonBlock.hxx>

#include <BOP_BuilderTools.hxx>

#include <BOP_FaceBuilder.hxx>
#include <BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>

// =====================================================================================================================
//  function: SplitFace
//  purpose:
// =====================================================================================================================
Standard_Boolean BOP_ShellSolid::SplitFace(const Standard_Integer theFaceIndex,
					   TopTools_DataMapOfShapeInteger& theMapOfEdgeIndex,
					   TopTools_ListOfShape& theListOfFace) const
{
  theListOfFace.Clear();

  const BooleanOperations_ShapesDataStructure& aDS              = myDSFiller->DS();
  const BOPTools_PaveFiller&                   aPaveFiller      = myDSFiller->PaveFiller();
  BOPTools_PaveFiller*                         pPaveFiller      = (BOPTools_PaveFiller*)&aPaveFiller;
  BOPTools_CommonBlockPool&                    aCBPool          = pPaveFiller->ChangeCommonBlockPool();
  const BOPTools_SplitShapesPool&              aSplitShapesPool = aPaveFiller.SplitShapesPool();

  BOPTools_InterferencePool* pIntrPool=
    (BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  
  BOPTools_CArray1OfSSInterference& aFFs = pIntrPool->SSInterferences();

  TopoDS_Face aFace = TopoDS::Face(aDS.Shape(theFaceIndex));
  aFace.Orientation(TopAbs_FORWARD);

  BOP_WireEdgeSet aWES (aFace);


  Standard_Integer i = 0;

  for(i = 1; i <= aFFs.Length(); i++) {
    BOPTools_SSInterference& aFF = aFFs(i);

    if((aFF.Index1() != theFaceIndex) && (aFF.Index2() != theFaceIndex))
      continue;

    BOPTools_SequenceOfCurves& aSC = aFF.Curves();
    Standard_Integer aNbCurves = aSC.Length();
    Standard_Integer j = 0;

    for(j = 1; j <= aNbCurves; j++) {
      const BOPTools_Curve& aBC = aSC(j);
      const BOPTools_ListOfPaveBlock& aLPB = aBC.NewPaveBlocks();
      BOPTools_ListIteratorOfListOfPaveBlock anIt(aLPB);

      for (; anIt.More(); anIt.Next()) {
	const BOPTools_PaveBlock& aPB = anIt.Value();
	Standard_Integer nE = aPB.Edge();
	const TopoDS_Shape& aE = aDS.Shape(nE);
	TopoDS_Edge aES = TopoDS::Edge(aE);
	aWES.AddStartElement(aES);
	aES.Reverse();
	aWES.AddStartElement(aES);

	if(!theMapOfEdgeIndex.IsBound(aES)) {
	  theMapOfEdgeIndex.Bind(aES, nE);
	}
      }
    }
  }

  Standard_Integer aFaceRank = aDS.Rank(theFaceIndex);
  TopExp_Explorer anExpE(aFace, TopAbs_EDGE);

  for(; anExpE.More(); anExpE.Next()) {
    const TopoDS_Shape& anOldEdge = anExpE.Current();
    TopAbs_Orientation anOr = anOldEdge.Orientation();
    Standard_Integer anEdgeIndex = aDS.ShapeIndex(anOldEdge, aFaceRank);

    if(anEdgeIndex <= 0)
      continue;
    
    const BOPTools_ListOfPaveBlock& aLPB = aSplitShapesPool(aDS.RefEdge(anEdgeIndex));

    if(aLPB.IsEmpty()) {
      TopoDS_Edge aSS = TopoDS::Edge(anOldEdge);
      //
      aWES.AddStartElement (aSS);

      if(!theMapOfEdgeIndex.IsBound(aSS)) {
	theMapOfEdgeIndex.Bind(aSS, anEdgeIndex);
      }
    }
    else {
      BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aLPB);

      for (; aPBIt.More(); aPBIt.Next()) {
	const BOPTools_PaveBlock& aPB = aPBIt.Value();
	Standard_Integer nSp = aPB.Edge();
	Standard_Integer nSpToAdd = nSp;

	BooleanOperations_StateOfShape aState = aDS.GetState(nSp);
	Standard_Boolean bAddToWes = ((aState == BooleanOperations_IN) ||
				      (aState == BooleanOperations_OUT));

	if(aState == BooleanOperations_ON) {
	  bAddToWes = Standard_True;

	  BOPTools_ListOfCommonBlock& aLCB = aCBPool(aDS.RefEdge(anEdgeIndex));

	  BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
    
	  for (; anItCB.More(); anItCB.Next()) {
	    BOPTools_CommonBlock& aCB = anItCB.Value();
	    Standard_Integer iCBFace = aCB.Face();

	    if(iCBFace == 0) {
	      BOPTools_PaveBlock& aPBEF1 = aCB.PaveBlock1(anEdgeIndex);
	      BOPTools_PaveBlock& aPBEF2 = aCB.PaveBlock2(anEdgeIndex);

	      const BOPTools_PaveBlock& aPB1 = aCB.PaveBlock1();
	      Standard_Integer nSpTaken = aPB1.Edge();

	      if((nSp == aPBEF1.Edge()) || (nSp == aPBEF2.Edge())) {
		nSpToAdd = nSpTaken;
		break;
	      }
	    }
	  }
	}
	
	if(bAddToWes) {
	  const TopoDS_Shape& aSplit = aDS.Shape(nSpToAdd);
	  TopoDS_Edge aSS=TopoDS::Edge(aSplit);
	  aSS.Orientation(anOr);

	  if(nSpToAdd != nSp) {
	    const TopoDS_Shape& aSp1=aDS.Shape(nSp);
	    TopoDS_Edge aSpF1=TopoDS::Edge(aSp1);
	    aSpF1.Orientation(anOr);
	  
	    const TopoDS_Shape& aSp2 = aDS.Shape(nSpToAdd);
	    TopoDS_Edge aSpF2 = TopoDS::Edge(aSp2);
	  
	    Standard_Boolean bToReverse=
	      BOPTools_Tools3D::IsSplitToReverse1 (aSpF1, aSpF2, pPaveFiller->ChangeContext());

	    if (bToReverse) {
	      aSpF2.Reverse();
	    }
	    
	    if (BRep_Tool::IsClosed(TopoDS::Edge(anOldEdge), aFace) && 
		!BRep_Tool::IsClosed(aSpF2, aFace)) {
	      Standard_Boolean bIsReversed = Standard_False;

	      if(!BOPTools_Tools3D::DoSplitSEAMOnFace(aSpF2, aSpF1, aFace, bIsReversed))
		BOPTools_Tools3D::DoSplitSEAMOnFace (aSpF2, aFace);
	    }


	    aSS = aSpF2;
	  }
	  //
	  aWES.AddStartElement (aSS);

	  if(!theMapOfEdgeIndex.IsBound(aSS)) {
	    theMapOfEdgeIndex.Bind(aSS, nSpToAdd);
	  }
	}
      }
    }
  }

  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger aFFMap;
  BOP_BuilderTools::DoMap(aFFs, aFFMap);

  Standard_Integer aNb = aFFMap.Extent();

  for (i = 1; i <= aNb; i++) {
    Standard_Integer nF1 = aFFMap.FindKey(i);

    if(nF1 != theFaceIndex)
      continue;

    const TColStd_IndexedMapOfInteger& aFFIndicesMap=aFFMap.FindFromIndex(i);
    Standard_Integer aNbj = aFFIndicesMap.Extent();
    Standard_Integer j = 0;

    for (j = 1; j <= aNbj; j++) {
      Standard_Integer iFF = aFFIndicesMap(j);
      BOPTools_SSInterference& aFF = aFFs(iFF);
      //
      Standard_Integer nF2 = aFF.OppositeIndex(nF1);

      if(nF2 <= 0)
	continue;
      //
      Standard_Integer iRankF2 = aDS.Rank(nF2);
      TopoDS_Shape aFace2 = aDS.Shape(nF2);

      anExpE.Init(aFace2, TopAbs_EDGE);

      for(; anExpE.More(); anExpE.Next()) {
	Standard_Integer anEdgeIndex2 = aDS.ShapeIndex(anExpE.Current(), iRankF2);

	if(anEdgeIndex2 <= 0)
	  continue;

	BOPTools_ListOfCommonBlock& aLCB = aCBPool(aDS.RefEdge(anEdgeIndex2));
    
	BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
    
	for (; anItCB.More(); anItCB.Next()) {
	  BOPTools_CommonBlock& aCB = anItCB.Value();
	  Standard_Integer iCBFace = aCB.Face();

	  if(iCBFace == theFaceIndex) {
	    BOPTools_PaveBlock& aPB = aCB.PaveBlock1(anEdgeIndex2);
	    Standard_Integer nSpEF2 = aPB.Edge();
	    const TopoDS_Shape& aSpEF2 = aDS.Shape(nSpEF2);

	    TopoDS_Edge aEF = TopoDS::Edge(aSpEF2);

	    if(!theMapOfEdgeIndex.IsBound(aEF)) {
	      theMapOfEdgeIndex.Bind(aEF, nSpEF2);
	    }

	    if(aEF.Orientation() == TopAbs_INTERNAL) {
	      aWES.AddStartElement(aEF);
	    }
	    else {
	      aEF.Orientation(TopAbs_FORWARD);
	      aWES.AddStartElement(aEF);
	      aEF.Reverse();
	      aWES.AddStartElement(aEF);
	    }
	  }
	}
      }
    }
  }

  // process internal edges
  BOP_WireEdgeSet aFilteredWES (aFace);
  TopoDS_Compound aComp;
  BRep_Builder aBB;
  aBB.MakeCompound(aComp);

  for(aWES.InitStartElements(); aWES.MoreStartElements(); aWES.NextStartElement()) {
    aBB.Add(aComp, aWES.StartElement());
  }
  TopTools_IndexedDataMapOfShapeListOfShape aMapVE;
  TopExp::MapShapesAndAncestors(aComp, TopAbs_VERTEX, TopAbs_EDGE, aMapVE);

  for(aWES.InitStartElements(); aWES.MoreStartElements(); aWES.NextStartElement()) {
    const TopoDS_Shape& anEdge = aWES.StartElement();

    if(anEdge.Orientation() != TopAbs_INTERNAL) {
      aFilteredWES.AddStartElement(anEdge);
    }
    else {
      TopoDS_Vertex aV1, aV2, aV;

      Standard_Boolean bFound1 = Standard_False;
      Standard_Boolean bFound2 = Standard_False;
      Standard_Boolean bIsClosed = Standard_False;
      Standard_Integer vcounter = 0;
      TopTools_IndexedMapOfShape aMapOfEInternal;
      aMapOfEInternal.Add(anEdge);

      TopExp::Vertices(TopoDS::Edge(anEdge), aV1, aV2);

      if(!aV1.IsNull() && aV1.IsSame(aV2)) {
	bIsClosed = Standard_True;
      }
      else {
	TopExp_Explorer anExpV(anEdge, TopAbs_VERTEX);

	for(; anExpV.More(); anExpV.Next(), vcounter++) {
	  aV = TopoDS::Vertex(anExpV.Current());
	  TopTools_MapOfShape aMapOfV;

	  while(!aV.IsNull()) {
	    if(!aMapVE.Contains(aV))
	      aV.Nullify();

	    if(aMapOfV.Contains(aV))
	      break;
	    aMapOfV.Add(aV);
	    const TopTools_ListOfShape& aListOfE = aMapVE.FindFromKey(aV);
	    TopTools_ListIteratorOfListOfShape anIt(aListOfE);
	    Standard_Boolean bFound = Standard_False;
	    aV.Nullify();

	    for(; anIt.More(); anIt.Next()) {
	      const TopoDS_Shape& anAdjE = anIt.Value();

	      if(anEdge.IsSame(anAdjE))
		continue;
	  
	      if(anAdjE.Orientation() != TopAbs_INTERNAL) {
		if(vcounter == 0)
		  bFound1 = Standard_True;
		else
		bFound2 = Standard_True;
		bFound = Standard_True;
	      }
	      else {
		aMapOfEInternal.Add(anAdjE);
	      }
	    } // end for

	    if(!bFound) {
	      anIt.Initialize(aListOfE);

	      for(; anIt.More(); anIt.Next()) {
		const TopoDS_Shape& anAdjE = anIt.Value();

		if(anEdge.IsSame(anAdjE))
		  continue;
	  
		if(anAdjE.Orientation() == TopAbs_INTERNAL) {
		  //
		  aMapOfEInternal.Add(anAdjE);
		  //

		  TopExp_Explorer anExpV2(anAdjE, TopAbs_VERTEX);

		  for(; anExpV2.More(); anExpV2.Next()) {
		    if(aMapOfV.Contains(anExpV2.Current()))
		      continue;
		    aV = TopoDS::Vertex(anExpV2.Current());
		  }
		}
	      }
	    }
	    // end if
	  }
	  // end while
	}
      }

      if(bIsClosed ||
	 ((vcounter > 1) && bFound1 && bFound2) ||
	 ((vcounter == 1) && bFound1)) {
	TopoDS_Shape anOrientedEdge = anEdge;
	anOrientedEdge.Orientation(TopAbs_FORWARD);

	aFilteredWES.AddStartElement(anOrientedEdge);
	anOrientedEdge.Reverse();
	aFilteredWES.AddStartElement(anOrientedEdge);
      }
      else {
	Standard_Integer aMapIt = 1, nbv = 0, nbe = 0;
	TopTools_IndexedMapOfShape aMapOfFoundV;

	for(aMapIt = 1; aMapIt <= aMapOfEInternal.Extent(); aMapIt++) {
	  const TopoDS_Shape& aShape = aMapOfEInternal(aMapIt);
	  TopExp::MapShapes(aShape, TopAbs_VERTEX, aMapOfFoundV);
	}
	nbe = aMapOfEInternal.Extent();
	nbv = aMapOfFoundV.Extent();

	if((nbe != 0) && (nbe == nbv)) {
	  TopoDS_Shape anOrientedEdge = anEdge;
	  anOrientedEdge.Orientation(TopAbs_FORWARD);

	  aFilteredWES.AddStartElement(anOrientedEdge);
	  anOrientedEdge.Reverse();
	  aFilteredWES.AddStartElement(anOrientedEdge);
	}
	else {
	  aFilteredWES.AddStartElement(anEdge);
	}
      }
    }
  }

  BOP_FaceBuilder aFB;
  aFB.Do(aFilteredWES);
  const TopTools_ListOfShape& aLF = aFB.NewFaces();

  theListOfFace = aLF;

  Standard_Integer nbstartedges = aFilteredWES.StartElements().Extent();
  Standard_Integer nbedgeused = 0;
  TopTools_ListIteratorOfListOfShape anItNewF(aLF);

  // was:
  // -----------------------------------------------------------
//  for(; anItNewF.More(); anItNewF.Next()) {
//    TopExp_Explorer anExp(anItNewF.Value(), TopAbs_EDGE);
//    for(; anExp.More(); anExp.Next(), nbedgeused++);
//  }
//  if(nbedgeused != nbstartedges) {
//    return Standard_False;
//  }
  // -----------------------------------------------------------
  //
  // modified 6841:
  //
  if(aLF.Extent() != 2) {
    for(; anItNewF.More(); anItNewF.Next()) {
      TopExp_Explorer anExp(anItNewF.Value(), TopAbs_EDGE);
      for(; anExp.More(); anExp.Next(), nbedgeused++);
    }
    if(nbedgeused != nbstartedges) {
      return Standard_False;
    }
  }
  else { // two faces analysis
    TopTools_IndexedDataMapOfShapeListOfShape amFLIE;
    TopTools_ListOfShape aLIntAll;
    for(; anItNewF.More(); anItNewF.Next()) {
      const TopoDS_Face & anF = TopoDS::Face(anItNewF.Value());
      TopExp_Explorer anExp(anItNewF.Value(), TopAbs_EDGE);
      TopTools_ListOfShape aLInt;
      for(; anExp.More(); anExp.Next(), nbedgeused++) {
        const TopoDS_Edge & anE = TopoDS::Edge(anExp.Current());
        if(anE.Orientation() == TopAbs_INTERNAL) {
          aLIntAll.Append(anE);
          aLInt.Append(anE);
        }
      }
      amFLIE.Add(anF,aLInt);
    }
    if(nbedgeused != nbstartedges) {
      if(!aLIntAll.IsEmpty()) {
        Standard_Integer nbUsed = nbedgeused;
        TopTools_ListIteratorOfListOfShape anItInt(aLIntAll);
        for(; anItInt.More(); anItInt.Next()) {
          const TopoDS_Edge & aEInt = TopoDS::Edge(anItInt.Value());
          Standard_Integer nbFRE = 0;
          TopTools_ListIteratorOfListOfShape anItWes(aFilteredWES.StartElements());
          for(; anItWes.More(); anItWes.Next()) {
            const TopoDS_Edge & aEWes = TopoDS::Edge(anItWes.Value());
            if(aEWes.Orientation() != TopAbs_INTERNAL) {
              if(aEInt.IsSame(aEWes))
                nbFRE++;
            }
          }
          if(nbFRE > 1)
            nbUsed += (nbFRE - 1);
        }
        if(nbUsed == nbstartedges && Abs(nbstartedges-nbedgeused) == 1) {
          Standard_Integer iF = 0, nbFI = 0;
          for(iF = 1; iF <= amFLIE.Extent(); iF++) {
            const TopTools_ListOfShape& aLOfIE = amFLIE.FindFromIndex(iF);
            if(!aLOfIE.IsEmpty()) nbFI++;
          }
          if(nbFI == 1) {
            anItNewF.Initialize(aLF);
            Standard_Boolean checkOk = Standard_False;
            for(; anItNewF.More(); anItNewF.Next()) {
              const TopoDS_Face & anF = TopoDS::Face(anItNewF.Value());
              const TopTools_ListOfShape& aLOfIE = amFLIE.FindFromKey(anF);
              if(!aLOfIE.IsEmpty() && aLOfIE.Extent() == 1) {
                const TopoDS_Edge & anIntE = TopoDS::Edge(aLOfIE.First());
                TopTools_IndexedDataMapOfShapeListOfShape aMapVE;
                TopExp::MapShapesAndAncestors(anF,TopAbs_VERTEX,TopAbs_EDGE,aMapVE);
                TopoDS_Vertex v1,v2; 
                TopExp::Vertices(anIntE,v1,v2);
                Standard_Boolean hasSingle = Standard_False;
                if(!v1.IsNull() && !v2.IsNull()) {
                  const TopTools_ListOfShape& aL1E = aMapVE.FindFromKey(v1);
                  const TopTools_ListOfShape& aL2E = aMapVE.FindFromKey(v2);
                  if((aL1E.Extent() == 1 && aL2E.Extent() != 1) ||
                     (aL2E.Extent() == 1 && aL1E.Extent() != 1))
                    hasSingle = Standard_True;
                }
                if(hasSingle) {
                  checkOk = Standard_True;
                  break;
                }
              }
            }
            if(checkOk) {
              return Standard_True;
            }
          }
        } // all edges are really used
      } // has internals
      return Standard_False;
    }
  } // two faces analysis

  return Standard_True;
}
