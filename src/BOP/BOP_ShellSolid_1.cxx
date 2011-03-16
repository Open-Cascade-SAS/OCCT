// File:	BOP_ShellSolid_1.cxx
// Created:	Fri Nov  2 12:36:51 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOP_ShellSolid.ixx>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

#include <TopAbs_Orientation.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>

#include <TopExp_Explorer.hxx>

#include <BRep_Tool.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_IndexedDataMapOfShapeInteger.hxx>

#include <IntTools_Context.hxx>

#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_SSInterference.hxx>

#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_CommonBlockPool.hxx>

#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_Tools3D.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>

#include <BOP_SDFWESFiller.hxx>

#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <TopExp.hxx>

static 
  Standard_Boolean CheckSplitToAvoid(const TopoDS_Edge&          theSplit,
				     const BOPTools_CommonBlock& theCB, 
				     const Standard_Integer      theEdgeIndex,
				     const Standard_Integer      theFaceIndex,
				     const BOPTools_PDSFiller&   theDSFiller, 
				     const BOP_Operation&        theOperation,
				     IntTools_Context&           theContext);

//=======================================================================
// 
// the WES components for a shell
//
//=======================================================================
// function: AddSectionPartsSh
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddSectionPartsSh (const Standard_Integer nF1, 
					  const Standard_Integer iFF,
					  BOP_WireEdgeSet& aWES)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  //
  Standard_Integer i, aNbCurves, nF2,  nE, iRankF1;
  //
  iRankF1=aDS.Rank(nF1);
  //
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  //
  BOPTools_SequenceOfCurves& aSC=aFF.Curves();
  aNbCurves=aSC.Length();
  for (i=1; i<=aNbCurves; i++) {
    const BOPTools_Curve& aBC=aSC(i);
    const BOPTools_ListOfPaveBlock& aLPB=aBC.NewPaveBlocks();
    BOPTools_ListIteratorOfListOfPaveBlock anIt(aLPB);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_PaveBlock& aPB=anIt.Value();
      nE=aPB.Edge();
      const TopoDS_Edge& aE=TopoDS::Edge(aDS.Shape(nE));
      
      TopoDS_Edge aES=aE;
      
      if (myOperation==BOP_FUSE) {
	aWES.AddStartElement (aES);
	aES.Reverse();
	aWES.AddStartElement (aES);
      }
	
    }
  }
}
//=======================================================================
// function: AddSplitPartsONSh
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddSplitPartsONSh(const Standard_Integer nF1,
					 BOP_WireEdgeSet& aWES)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*)&aPaveFiller;
  BOPTools_CommonBlockPool& aCBPool=pPaveFiller->ChangeCommonBlockPool();
  //
  IntTools_Context& aContext=pPaveFiller->ChangeContext();
  //
  Standard_Integer nEF1, nF2, nSpF1, nSpF2, nEF2, nSpTaken, iRankF1;
  Standard_Boolean bToReverse;
  TopAbs_Orientation anOrEF1, anOrEF2;
  TopExp_Explorer anExp;
  TopTools_IndexedMapOfShape aM;
  TopoDS_Edge aSSF1, aSSF2;
  //
  iRankF1=aDS.Rank(nF1);
  //
  anExp.Init(myFace, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Edge& anEF1=TopoDS::Edge(anExp.Current());
    anOrEF1=anEF1.Orientation();
    nEF1=aDS.ShapeIndex(anEF1, iRankF1);
    
    BOPTools_ListOfCommonBlock& aLCB=aCBPool(aDS.RefEdge(nEF1));
    
    BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
    for (; anItCB.More(); anItCB.Next()) {
      BOPTools_CommonBlock& aCB=anItCB.Value();

      BOPTools_PaveBlock& aPBEF1=aCB.PaveBlock1(nEF1);
      BOPTools_PaveBlock& aPBEF2=aCB.PaveBlock2(nEF1);
      nF2=aCB.Face();
      if (nF2) { 
	// Splits that are ON (IN 2D) for other Face (aF2)
	nSpF1=aPBEF1.Edge();
	const TopoDS_Shape& aSplit=aDS.Shape(nSpF1);
	aSSF1=TopoDS::Edge(aSplit);
	//
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	// Internal edges treatment
	{
	  if (anOrEF1==TopAbs_INTERNAL) {
	    if (myOperation==BOP_FUSE) {
	      aSSF1.Orientation(TopAbs_FORWARD);
	      aWES.AddStartElement (aSSF1);
	      aSSF1.Reverse();
	      aWES.AddStartElement (aSSF1);
	    }
	    else if (myOperation==BOP_CUT) {
	      if (iRankF1==1) {
		aWES.AddStartElement (aSSF1);
	      }
	    }
	    else if (myOperation==BOP_CUT21) {
	      if (iRankF1==2) {
		aWES.AddStartElement (aSSF1);
	      }
	    }
	    continue;
	  }
	}
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	//
	aSSF1.Orientation(anOrEF1);
	aWES.AddStartElement (aSSF1);
      }

      else {
	// Splits that are ON other Edge from other Face
	nSpF1=aPBEF1.Edge();
	nSpF2=aPBEF2.Edge();
	nEF2=aPBEF2.OriginalEdge();
	
	const TopoDS_Edge& anEF2=TopoDS::Edge(aDS.Shape(nEF2));
	anOrEF2=anEF2.Orientation();

	const TopoDS_Shape& aSpF1=aDS.Shape(nSpF1);
	const TopoDS_Shape& aSpF2=aDS.Shape(nSpF2);
	//
	// Pave Block from which new edge will be taken
	const BOPTools_PaveBlock& aPB=aCB.PaveBlock1();
	nSpTaken=aPB.Edge();
	//
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	// Internal edges treatment
	{
	  Standard_Boolean bInternal1, bInternal2;
	  bInternal1=(anOrEF1==TopAbs_INTERNAL);
	  bInternal2=(anOrEF2==TopAbs_INTERNAL);
	  
	  if (bInternal1) {
	    aSSF1=TopoDS::Edge(aDS.Shape(nSpTaken));

	    if (myOperation==BOP_FUSE) {
	      aSSF1.Orientation(TopAbs_FORWARD);
	      aWES.AddStartElement (aSSF1);
	      aSSF1.Reverse();
	      aWES.AddStartElement (aSSF1);
	      continue;
	    }

	    if (myOperation==BOP_CUT && iRankF1==1) {
	      aSSF1.Orientation(TopAbs_INTERNAL);
	      aWES.AddStartElement (aSSF1);
	      continue;
	    }

	    if (myOperation==BOP_CUT21 && iRankF1==2) {
	      aSSF1.Orientation(TopAbs_INTERNAL);
	      aWES.AddStartElement (aSSF1);
	      continue;
	    }
	  }
	  
	  else if (!bInternal1 && bInternal2) {
	    if (nSpTaken!=nSpF1) {
	      
	      if ((myOperation==BOP_FUSE)||
		  (myOperation==BOP_CUT && iRankF1==1) ||
		  (myOperation==BOP_CUT21 && iRankF1==2)) { 
	      
		aSSF1=TopoDS::Edge(aSpF1);
		aSSF1.Orientation(anOrEF1);
		
		aSSF2=TopoDS::Edge(aSpF2);
	      
		aSSF2.Orientation(TopAbs_FORWARD);
		bToReverse=BOPTools_Tools3D::IsSplitToReverse1 (aSSF1, aSSF2, aContext);
		if (bToReverse) {
		  aSSF2.Reverse();
		}
	      
		aWES.AddStartElement (aSSF2);
		continue;
	      }
	    }
	  }
	}
	//
	aSSF1=TopoDS::Edge(aSpF1);
	aSSF1.Orientation(anOrEF1);
	
	if (nSpTaken==nSpF1) {
	  // Common Edge is from nEF1
	  if(CheckSplitToAvoid(aSSF1, aCB, nEF1, nF1, myDSFiller, myOperation, aContext)){
	    continue;
	  }
	  aWES.AddStartElement (aSSF1);
	}
	
	else {
	  // Common Edge is from nEF2 nSpTaken!=nSpF2
	  aSSF2=TopoDS::Edge(aSpF2);
	  
	  bToReverse=BOPTools_Tools3D::IsSplitToReverse1 (aSSF1, aSSF2, aContext);
	  if (bToReverse) {
	    aSSF2.Reverse();
	  }
	  //
	  if (BRep_Tool::IsClosed(aSSF1, myFace)) {
	    if (aM.Contains(aSSF2)){
	      continue;
	    }
	    aM.Add(aSSF2);
	    //
	    if (!BRep_Tool::IsClosed(aSSF2, myFace)) {
	      BOPTools_Tools3D::DoSplitSEAMOnFace (aSSF2, myFace);
	    }

	    aWES.AddStartElement (aSSF2);
	    aSSF2.Reverse();
	    aWES.AddStartElement (aSSF2);
	    continue;  
	  }
	  //
	  if(CheckSplitToAvoid(aSSF2, aCB, nEF1, nF1, myDSFiller, myOperation, aContext)) {
	    continue;
	  }
	  aWES.AddStartElement (aSSF2);
	}
      }
    }
  }
}
//=======================================================================
// function: AddPartsEFSh
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddPartsEFSh (const Standard_Integer nF1, 
				     const Standard_Integer iFF,
				     TopTools_IndexedMapOfShape& anEMap,
				     BOP_WireEdgeSet& aWES)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  
  const BOPTools_PaveFiller& aPF=myDSFiller->PaveFiller();
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*)&aPF;
  BOPTools_CommonBlockPool& aCBPool=pPaveFiller->ChangeCommonBlockPool();
  //
  Standard_Integer iRankF1, iRankF2, nF2, nSpEF2, nEF2,  nFace;
  TopExp_Explorer anExp2;
  TopAbs_Orientation anOrEF2;
  //
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  //
  const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));
  //
  iRankF1=aDS.Rank(nF1);
  iRankF2=aDS.Rank(nF2);
  //
  // EF2\F1 Processing
  anExp2.Init (aF2, TopAbs_EDGE);
  for (; anExp2.More(); anExp2.Next()) {
    const TopoDS_Edge& aEF2= TopoDS::Edge(anExp2.Current());
    anOrEF2=aEF2.Orientation();

    nEF2=aDS.ShapeIndex (aEF2, iRankF2);

    BOPTools_ListOfCommonBlock& aLCB=aCBPool(aDS.RefEdge(nEF2));
    
    BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
    for (; anItCB.More(); anItCB.Next()) {
      BOPTools_CommonBlock& aCB=anItCB.Value();
      nFace=aCB.Face();
      if (nFace==nF1) {
	BOPTools_PaveBlock& aPB=aCB.PaveBlock1(nEF2);

	nSpEF2=aPB.Edge();
	const TopoDS_Shape& aSpEF2=aDS.Shape(nSpEF2);
	
	if (anEMap.Contains(aSpEF2)) {
	  continue;// next CB
	}
	anEMap.Add(aSpEF2);
	
	TopoDS_Edge aSS=TopoDS::Edge(aSpEF2);
	//
	//
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	// Internal edges treatment
	{
	  if (anOrEF2==TopAbs_INTERNAL) {
	    aSS.Orientation(TopAbs_FORWARD);
	  }
	}
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	//
	if (myOperation== BOP_FUSE) {
	  aWES.AddStartElement (aSS);
	  aSS.Reverse();
	  aWES.AddStartElement (aSS);
	}
      }
    } // next CB on nEF2
  }
}
//xf
//=======================================================================
// function: AddINON2DPartsSh
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddINON2DPartsSh(const Standard_Integer nF1,
					const Standard_Integer iFF,
					BOP_WireEdgeSet& aWES)
{
  TopTools_IndexedMapOfShape anEMap;
  AddINON2DPartsSh(nF1, iFF, aWES, anEMap);
}
//xt
//=======================================================================
// function: AddINON2DPartsSh
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddINON2DPartsSh(const Standard_Integer nF1,
					const Standard_Integer iFF,
					BOP_WireEdgeSet& aWES,
					TopTools_IndexedMapOfShape& anEMap) //xft
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  //
  Standard_Integer iRankF1, nF2, iSenseFlag;

  iRankF1=aDS.Rank(nF1);
  
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  //
  iSenseFlag=aFF.SenseFlag();
  //
  BOP_SDFWESFiller aWESFiller;
  
  aWESFiller.SetDSFiller(*myDSFiller);
  aWESFiller.SetFaces(nF1, nF2);
  aWESFiller.SetStatesMap(aFF.StatesMap());
  //
  aWESFiller.SetSenseFlag(iSenseFlag);
 
  switch (myOperation) {
  
  case BOP_FUSE: 
    if (myRank==2) {
      //shell\solid case when the solid is the first arg.
      iRankF1=1;
    }
    if (iRankF1==1) {
      aWESFiller.SetOperation(BOP_CUT);	
      aWESFiller.Do(aWES);
      aWESFiller.SetOperation(BOP_COMMON);	
      aWESFiller.Do(aWES);
    }
    else {
      aWESFiller.SetOperation(BOP_CUT);	
      aWESFiller.Do(aWES);
    }
    break;
    
  case BOP_COMMON:
    if (myRank==2) {
      //shell\solid case when the solid is the first arg.
      iRankF1=1;
    }
    if (iRankF1==1) {
      aWESFiller.SetOperation(BOP_COMMON);	
      aWESFiller.Do(aWES);
    }
    break;
    
  case BOP_CUT: 
    if (iRankF1==1) {
      aWESFiller.SetOperation(BOP_CUT);	
      aWESFiller.Do(aWES);
    }
    break;
    
  case BOP_CUT21: 
    if (iRankF1==2) {
      aWESFiller.SetOperation(BOP_CUT);	
      aWESFiller.Do(aWES);
    }
    break;
    
  default:
    break;
  }
  //
  //xf
  // Collect all split edges of nF1 that are CB with 
  // splis of all SD faces to nFx,
  // but not included in aWES (RejectedOnParts).
  // This is necessary to prevent inclusion these splits in 
  // AddPartsEENonSDSh(...) 
  // see BOP_SDFWESFiller,  BOP_ShellSolid::DoNewFaces()
  //  for more details;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  const TopTools_ListOfShape& aLRE=aWESFiller.RejectedOnParts();
  aIt.Initialize(aLRE);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE=aIt.Value();
    anEMap.Add(aE);
  }
  //xt
}
//=======================================================================
// function: AddPartsEFNonSDSh
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddPartsEFNonSDSh (const Standard_Integer nF1, 
					  const Standard_Integer iFF,
					  TopTools_IndexedMapOfShape& anEMap,
					  BOP_WireEdgeSet& aWES)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  
  const BOPTools_PaveFiller& aPF=myDSFiller->PaveFiller();
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*)&aPF;
  BOPTools_CommonBlockPool& aCBPool=pPaveFiller->ChangeCommonBlockPool();
  //
  Standard_Integer nF2, nSpEF2, nEF2,  nFace, iRankF2;
  TopExp_Explorer anExp2;
  TopAbs_Orientation anOrEF2 = TopAbs_FORWARD;
  //
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  //
  const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));

  iRankF2=aDS.Rank(nF2);
  //
  TopTools_IndexedMapOfOrientedShape aWESMap;
  {
    const TopTools_ListOfShape& aWESList=aWES.StartElements();
    TopTools_ListIteratorOfListOfShape anIt(aWESList);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aS=anIt.Value();
      aWESMap.Add(aS);
    }
  }
  //
  // EF2\F1 Processing
  anExp2.Init (aF2, TopAbs_EDGE);
  for (; anExp2.More(); anExp2.Next()) {
    const TopoDS_Edge& aEF2= TopoDS::Edge(anExp2.Current());

    nEF2=aDS.ShapeIndex(aEF2, iRankF2);

    BOPTools_ListOfCommonBlock& aLCB=aCBPool(aDS.RefEdge(nEF2));
    
    BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
    for (; anItCB.More(); anItCB.Next()) {
      BOPTools_CommonBlock& aCB=anItCB.Value();
      nFace=aCB.Face();
      
      if (nFace==nF1) {
	BOPTools_PaveBlock& aPB=aCB.PaveBlock1(nEF2);

	nSpEF2=aPB.Edge();
	const TopoDS_Shape& aSpEF2=aDS.Shape(nSpEF2);
	//
	if (anEMap.Contains(aSpEF2)) {
	  continue;// next CB
	}
	anEMap.Add(aSpEF2);
	//
	if (aWESMap.Contains(aSpEF2)) {
	  continue;// next CB
	}
	aWESMap.Add(aSpEF2);
	//
	TopoDS_Edge aSS=TopoDS::Edge(aSpEF2);
	//
	//
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	// Internal edges treatment
	{
	  if (anOrEF2==TopAbs_INTERNAL) {
	    aSS.Orientation(TopAbs_FORWARD);
	  }
	}
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	//
	if (myOperation==BOP_FUSE) {
	  aWES.AddStartElement(aSS);
	  aSS.Reverse();
	  aWES.AddStartElement(aSS);
	}
	//
      } //if (nFace==nF1) {
    } // next CB on nEF2
  }
}
//=======================================================================
// function: AddPartsEENonSDSh
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddPartsEENonSDSh (const Standard_Integer nF1, 
					  const Standard_Integer iFF,
					  TopTools_IndexedMapOfShape& anEMap,
					  BOP_WireEdgeSet& aWES)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  
  const BOPTools_PaveFiller& aPF=myDSFiller->PaveFiller();
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*)&aPF;
  BOPTools_CommonBlockPool& aCBPool=pPaveFiller->ChangeCommonBlockPool();
  //
  IntTools_Context& aContext=pPaveFiller->ChangeContext();
  //
  Standard_Integer nEF1, nF2, nSpF1, nSpF2, nEF2, nSpTaken, nF2x, iRankF1;
  Standard_Boolean bToReverse;
  TopAbs_Orientation anOrEF1, anOrEF2;
  TopExp_Explorer anExp;
  TopTools_IndexedMapOfShape aM;
  TColStd_ListOfInteger aSplitsOnF1;
  TColStd_ListIteratorOfListOfInteger anItSp;
  TColStd_IndexedMapOfInteger aMSplitsOnF1;
  TopoDS_Edge aSSF1, aSSF2;
  //
  // nF1
  iRankF1=aDS.Rank(nF1);
  //
  // nF2
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  //
  pPaveFiller->SplitsOnFace(0, nF1, nF2, aSplitsOnF1);
  anItSp.Initialize(aSplitsOnF1);
  for (; anItSp.More(); anItSp.Next()) {
    nSpF1=anItSp.Value();
    aMSplitsOnF1.Add(nSpF1);
  }
  //
  TopTools_IndexedMapOfOrientedShape aWESMap;
  {
    const TopTools_ListOfShape& aWESList=aWES.StartElements();
    TopTools_ListIteratorOfListOfShape anIt(aWESList);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aS=anIt.Value();
      aWESMap.Add(aS);
    }
  }
  //
  anExp.Init(myFace, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Edge& anEF1=TopoDS::Edge(anExp.Current());
    anOrEF1=anEF1.Orientation();
    nEF1=aDS.ShapeIndex(anEF1, iRankF1);
    
    BOPTools_ListOfCommonBlock& aLCB=aCBPool(aDS.RefEdge(nEF1));
    
    BOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
    for (; anItCB.More(); anItCB.Next()) {
      BOPTools_CommonBlock& aCB=anItCB.Value();
    
      BOPTools_PaveBlock& aPBEF1=aCB.PaveBlock1(nEF1);
      BOPTools_PaveBlock& aPBEF2=aCB.PaveBlock2(nEF1);
      
      nF2x=aCB.Face();
      if (nF2x) {
	continue;
      }
      // Splits that are ON other Edge from other Face
      nSpF1=aPBEF1.Edge();
      //
      if (!aMSplitsOnF1.Contains(nSpF1)) {
	continue;// next CB
      }
      //
      nSpF2=aPBEF2.Edge();
      nEF2=aPBEF2.OriginalEdge();

      const TopoDS_Edge& anEF2=TopoDS::Edge(aDS.Shape(nEF2));
      anOrEF2=anEF2.Orientation();
      
      const TopoDS_Shape& aSpF1=aDS.Shape(nSpF1);
      const TopoDS_Shape& aSpF2=aDS.Shape(nSpF2);
      
      //
      if (anEMap.Contains(aSpF1)) {
	continue;// next CB
      }
      anEMap.Add(aSpF1);
      //
      if (anEMap.Contains(aSpF2)) {
	continue;// next CB
      }
      anEMap.Add(aSpF2);
      //
      
      // Pave Block from which new edge will be taken
      const BOPTools_PaveBlock& aPB=aCB.PaveBlock1();
      nSpTaken=aPB.Edge();
      //
      //iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
      // Internal edges treatment
      {
	Standard_Boolean bInternal1, bInternal2;
	bInternal1=(anOrEF1==TopAbs_INTERNAL);
	bInternal2=(anOrEF2==TopAbs_INTERNAL);
	
	if (bInternal1) {
	  aSSF1=TopoDS::Edge(aDS.Shape(nSpTaken));
	  
	  if (myOperation==BOP_FUSE) {
	    aSSF1.Orientation(TopAbs_FORWARD);
	    aWES.AddStartElement (aSSF1);
	    aSSF1.Reverse();
	    aWES.AddStartElement (aSSF1);
	    continue;
	  }
	  
	  if (myOperation==BOP_CUT && iRankF1==1) {
	    aSSF1.Orientation(TopAbs_INTERNAL);
	    aWES.AddStartElement (aSSF1);
	    continue;
	  }
	  
	  if (myOperation==BOP_CUT21 && iRankF1==2) {
	    aSSF1.Orientation(TopAbs_INTERNAL);
	    aWES.AddStartElement (aSSF1);
	    continue;
	  }
	}
	  
	else if (!bInternal1 && bInternal2) {
	  if (nSpTaken!=nSpF1) {
	    
	    if ((myOperation==BOP_FUSE)||
		(myOperation==BOP_CUT && iRankF1==1) ||
		(myOperation==BOP_CUT21 && iRankF1==2)) { 
	      
	      aSSF1=TopoDS::Edge(aSpF1);
	      aSSF1.Orientation(anOrEF1);
	      
	      aSSF2=TopoDS::Edge(aSpF2);
	      
	      aSSF2.Orientation(TopAbs_FORWARD);
	      bToReverse=BOPTools_Tools3D::IsSplitToReverse1 (aSSF1, aSSF2, aContext);
	      if (bToReverse) {
		aSSF2.Reverse();
	      }
	      
	      aWES.AddStartElement (aSSF2);
	      continue;
	    }
	  }
	}
      }
      //iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
      //
      aSSF1=TopoDS::Edge(aSpF1);
      aSSF1.Orientation(anOrEF1);
      
      if (nSpTaken==nSpF1) {
	// Common Edge is from nEF1
	aWES.AddStartElement (aSSF1);
      }
      else  {
	// Common Edge is from nEF2 nSpTaken!=nSpF2
	aSSF2=TopoDS::Edge(aSpF2);
	
	bToReverse=BOPTools_Tools3D::IsSplitToReverse1 (aSSF1, aSSF2, aContext);
	if (bToReverse) {
	  aSSF2.Reverse();
	}
	//
	if (BRep_Tool::IsClosed(aSSF1, myFace)) {
	  if (aM.Contains(aSSF2)){
	    continue;
	  }
	  aM.Add(aSSF2);
	  //
	  if (!BRep_Tool::IsClosed(aSSF2, myFace)) {
	    BOPTools_Tools3D::DoSplitSEAMOnFace (aSSF2, myFace);
	  }
	  aWES.AddStartElement (aSSF2);
	  aSSF2.Reverse();
	  aWES.AddStartElement (aSSF2);
	  continue;  
	}
	//
	aWES.AddStartElement (aSSF2);
      }// else /*if (nSpTaken==nSpF2)*/ {
    }// for (; anItCB.More(); anItCB.Next())
  }// for (; anExp.More(); anExp.Next())
}
//=======================================================================
//function : CheckSplitToAvoid
//purpose  : 
//=======================================================================
Standard_Boolean CheckSplitToAvoid(const TopoDS_Edge&          theSplit,
				   const BOPTools_CommonBlock& theCB, 
				   const Standard_Integer      theEdgeIndex,
				   const Standard_Integer      theFaceIndex,
				   const BOPTools_PDSFiller&   theDSFiller, 
				   const BOP_Operation&        theOperation,
				   IntTools_Context&           theContext) {

  Standard_Integer anE = -1;

  if(theCB.PaveBlock1().OriginalEdge() == theEdgeIndex) {
    anE = theCB.PaveBlock2().OriginalEdge();
  }
  else if(theCB.PaveBlock2().OriginalEdge() == theEdgeIndex) {
    anE = theCB.PaveBlock1().OriginalEdge();
  }

  if(anE >= 0) {
    const TopoDS_Shape& anEdge = theDSFiller->DS().Shape(anE);
    TopoDS_Face aFaceCur = TopoDS::Face(theDSFiller->DS().Shape(theFaceIndex));
    aFaceCur.Orientation(TopAbs_FORWARD);

    TopTools_IndexedDataMapOfShapeListOfShape aMapEF;
    Standard_Integer aRank = theDSFiller->DS().Rank(anE);
    TopoDS_Shape aSource = (aRank == 1) ? theDSFiller->Shape1() : theDSFiller->Shape2();
    TopExp::MapShapesAndAncestors(aSource, TopAbs_EDGE, TopAbs_FACE, aMapEF);

    if(aMapEF.Contains(anEdge)) {
      const TopTools_ListOfShape& aLF = aMapEF.FindFromKey(anEdge);

      if(!aLF.IsEmpty()) {
	TopTools_ListIteratorOfListOfShape anIt(aLF);
	Standard_Boolean avoid = Standard_True;

	for(; anIt.More(); anIt.Next()) {
	  const TopoDS_Face& aFace = TopoDS::Face(anIt.Value());
	  Standard_Real f = 0., l = 0.;
	  Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface(theSplit, aFaceCur, f, l);

	  if(!aCurve.IsNull()) {
	    Standard_Real amidpar = (f + l) * 0.5;

	    if(theOperation == BOP_COMMON) {
	      gp_Pnt2d aPoint2d;
	      gp_Pnt aPoint3d;
	      Standard_Real aTolerance = BRep_Tool::Tolerance(theSplit); //???
	      BOPTools_Tools3D::PointNearEdge(theSplit, aFaceCur, amidpar, aTolerance, aPoint2d, aPoint3d);
	      GeomAPI_ProjectPointOnSurf& aProjector =  theContext.ProjPS(aFace);
	      aProjector.Perform(aPoint3d);

	      if(aProjector.IsDone()) {
		Standard_Real U = 0., V = 0.;
		Standard_Real adist = aProjector.LowerDistance();

		if(adist < BRep_Tool::Tolerance(aFace)) {
		  aProjector.LowerDistanceParameters(U, V);

		  if(theContext.IsPointInFace(aFace, gp_Pnt2d(U, V))) {
		    avoid = Standard_False;
		    break;
		  }
		  else {
		  }
		}
	      }
	    }
	    else if(theOperation == BOP_CUT) {
	      if(theDSFiller->DS().Rank(theFaceIndex) != 2) {
		avoid = Standard_False;
		continue;
	      }
	      gp_Pnt2d aPoint2d;
	      gp_Pnt aPoint3d;
	      Standard_Real aTolerance = BRep_Tool::Tolerance(theSplit); //???
	      BOPTools_Tools3D::PointNearEdge(theSplit, aFaceCur, amidpar, aTolerance, aPoint2d, aPoint3d);
	      GeomAPI_ProjectPointOnSurf& aProjector =  theContext.ProjPS(aFace);
	      aProjector.Perform(aPoint3d);

	      if(aProjector.IsDone()) {
		Standard_Real U = 0., V = 0.;
		Standard_Real adist = aProjector.LowerDistance();

		if(adist < BRep_Tool::Tolerance(aFace)) {
		  aProjector.LowerDistanceParameters(U, V);

		  if(theContext.IsPointInFace(aFace, gp_Pnt2d(U, V))) {
		    avoid = Standard_False;
		    break;
		  }
		  else {
		  }
		}
	      }
	    }
	    else if(theOperation == BOP_CUT21) {
	      if(theDSFiller->DS().Rank(theFaceIndex) != 1) {
		avoid = Standard_False;
		continue;
	      }
	      gp_Pnt2d aPoint2d;
	      gp_Pnt aPoint3d;
	      Standard_Real aTolerance = BRep_Tool::Tolerance(theSplit); //???
	      BOPTools_Tools3D::PointNearEdge(theSplit, aFaceCur, amidpar, aTolerance, aPoint2d, aPoint3d);
	      GeomAPI_ProjectPointOnSurf& aProjector =  theContext.ProjPS(aFace);
	      aProjector.Perform(aPoint3d);

	      if(aProjector.IsDone()) {
		Standard_Real U = 0., V = 0.;
		Standard_Real adist = aProjector.LowerDistance();

		if(adist < BRep_Tool::Tolerance(aFace)) {
		  aProjector.LowerDistanceParameters(U, V);

		  if(theContext.IsPointInFace(aFace, gp_Pnt2d(U, V))) {
		    avoid = Standard_False;
		    break;
		  }
		  else {
		  }
		}
	      }
	    }
	    // end if(theOperation == BOP_CUT21...
	    else {
	      avoid = Standard_False;
	      break;
	    }
	  }
	}

	if(avoid) {
	  return Standard_True;
	}
      }
      // end if(!aLF.IsEmpty...
    }
  }

  return Standard_False;
}
