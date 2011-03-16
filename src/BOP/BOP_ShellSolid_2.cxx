// File:	BOP_ShellSolid_2.cxx
// Created:	Fri Nov  2 14:07:05 2001
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <BOP_ShellSolid.ixx>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <TopAbs_State.hxx>
#include <TopAbs_Orientation.hxx>

#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>

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

#include <BOPTools_Tools3D.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_CommonBlockPool.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>

#include <BOPTools_SSInterference.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_CommonBlock.hxx>

#include <BOP_BuilderTools.hxx>
#include <BOP_SDFWESFiller.hxx>


static
  void AddPartsEF (const TopoDS_Edge& aSSin,
		   const TopoDS_Edge& aEF2,
		   const TopoDS_Face& aF2,
		   const TopoDS_Face& myFace,
		   const Standard_Integer iRankF1,
		   const BOP_Operation myOperation,
		   const TopTools_IndexedDataMapOfShapeListOfShape& aMEF,
		   BOP_WireEdgeSet& aWES);
static
  void AddPartEF1INF2 (TopoDS_Edge& aSS,
		       const TopoDS_Face& myFace,
		       const TopoDS_Face& aF2,
		       const Standard_Integer iRankF1,
		       const BOP_Operation myOperation,
		       BOP_WireEdgeSet& aWES);
static
  void BothInternals (TopoDS_Edge& aSS,
		      const TopoDS_Face& myFace,
		      const TopoDS_Face& aF2,
		      const Standard_Integer iRankF1,
		      const BOP_Operation myOperation,
		      BOP_WireEdgeSet& aWES);
static
  void FirstInternal(TopoDS_Edge& aSS,
		     const TopoDS_Face& myFace,
		     const TopoDS_Face& aF2,
		     const TopoDS_Edge& anEF2,
		     TopoDS_Edge& aSSx,
		     const Standard_Integer iRankF1,
		     const BOP_Operation myOperation,
		     const TopTools_IndexedDataMapOfShapeListOfShape& aMEF,
		     BOP_WireEdgeSet& aWES,
		     IntTools_Context& aContext);

static
  void SecondInternal(TopoDS_Edge& aSS,
		      const TopoDS_Face& aF1,
		      const TopoDS_Face& aF2,
		      const TopoDS_Edge& anEF1,
		      TopoDS_Edge& aSSx,
		      const TopAbs_Orientation anOrEF1,
		      const Standard_Integer iRankF1,
		      const BOP_Operation myOperation,
		      BOP_WireEdgeSet& aWES,
		      IntTools_Context& aContext);


//=======================================================================
// 
// the WES components for a solid
//
//=======================================================================
// function: AddSectionPartsSo
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddSectionPartsSo (const Standard_Integer nF1, 
					  const Standard_Integer iFF,
					  BOP_WireEdgeSet& aWES)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  
  BOPTools_InterferencePool* pIntrPool=
    (BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  //
  Standard_Integer i, aNbCurves, nF2,  nE, iRankF1;
  Standard_Boolean bIsTouchCase;
  //
  iRankF1=aDS.Rank(nF1);
  //
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  //
  const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));
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
      bIsTouchCase=BOPTools_Tools3D::IsTouchCase(aES, myFace, aF2);
      if (bIsTouchCase) {
	aWES.AddStartElement (aES);
	aES.Reverse();
	aWES.AddStartElement (aES);
      }
      else {
	BOP_BuilderTools::OrientSectionEdgeOnF1
	  (myFace, aF2, iRankF1, myOperation, aES);
	aWES.AddStartElement (aES);
      }
    }
  }
}
//=======================================================================
// function: AddSplitPartsON3DSo
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddSplitPartsON3DSo(const Standard_Integer nF1,
					   const Standard_Integer iFF,
					   BOP_WireEdgeSet& aWES)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*)&aPaveFiller;
  BOPTools_CommonBlockPool& aCBPool=pPaveFiller->ChangeCommonBlockPool();

  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  //
  IntTools_Context& aContext=pPaveFiller->ChangeContext();
  //
  Standard_Boolean bIsToKeep;
  Standard_Integer nEF1, nF2, nF2x, nSpF1, iRankF1;
  TopAbs_Orientation anOrEF1;
  TopExp_Explorer anExp;
  //
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  
  const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
  const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));
  
  iRankF1=aDS.Rank(nF1);
  //
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
      //
      nF2x=aCB.Face();
      
      if (nF2x==nF2) { 
	// Splits that are ON (IN 2D) for other Face (aF2)
	nSpF1=aPBEF1.Edge();
	const TopoDS_Shape& aSplit=aDS.Shape(nSpF1);
	TopoDS_Edge aSS=TopoDS::Edge(aSplit);
	//
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	// Internal edges treatment
	{
	  if (anOrEF1==TopAbs_INTERNAL) {
	    AddPartEF1INF2(aSS, myFace, aF2, iRankF1, myOperation, aWES);
	    continue;
	  }
	}
	//
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	//
	aSS.Orientation(anOrEF1);
	//
	TopAbs_State aState1=
	  BOPTools_Tools3D::GetStatePartIN2D(aSS, anEF1, aF1, aF2, aContext);
	//
	bIsToKeep= 
	  BOP_BuilderTools::IsPartIN2DToKeep(aState1, iRankF1, myOperation);
	//
	if (bIsToKeep) {
	  aWES.AddStartElement (aSS);
	}
      }
    }
  }
}
//=======================================================================
// function: AddSplitPartsONSo
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddSplitPartsONSo(const Standard_Integer nF1,
					 const TopTools_IndexedDataMapOfShapeListOfShape& aMEFObj,
					 const TopTools_IndexedDataMapOfShapeListOfShape& aMEFTool,
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
  TopAbs_Orientation anOrEF1, anOrEF2;
  TopExp_Explorer anExp;
  TopTools_IndexedMapOfShape aM;
  //
  const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
  iRankF1=aDS.Rank(nF1);
  //
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
	Standard_Boolean bIsToKeep;

	const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));

	nSpF1=aPBEF1.Edge();
	const TopoDS_Shape& aSplit=aDS.Shape(nSpF1);
	TopoDS_Edge aSS=TopoDS::Edge(aSplit);
	//
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	// Internal edges treatment
	{
	  if (anOrEF1==TopAbs_INTERNAL) {
	    AddPartEF1INF2(aSS, myFace, aF2, iRankF1, myOperation, aWES);
	    continue;
	  }
	}
	//
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	//
	aSS.Orientation(anOrEF1);
	//
	TopAbs_State aState1=
	  BOPTools_Tools3D::GetStatePartIN2D(aSS, anEF1, aF1, aF2, aContext);
	
	bIsToKeep=
	  BOP_BuilderTools::IsPartIN2DToKeep(aState1, iRankF1, myOperation);
	//
	if (bIsToKeep) {
	  aWES.AddStartElement (aSS);
	}
	//
      }
      else {// else x
	// Splits that are ON other Edge from other Face
	nSpF1=aPBEF1.Edge();
	nSpF2=aPBEF2.Edge();
	nEF2=aPBEF2.OriginalEdge();
	//
	// Pave Block from which new edge will be taken
	const BOPTools_PaveBlock& aPB=aCB.PaveBlock1();
	nSpTaken=aPB.Edge();
	//
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	// Internal edges treatment
	{
	  Standard_Boolean bInternal1, bInternal2;
	  
	  const TopoDS_Edge& anEF2=TopoDS::Edge(aDS.Shape(nEF2));
	  anOrEF2=anEF2.Orientation();

	  const TopTools_IndexedDataMapOfShapeListOfShape& aMEF=
	    (iRankF1==1) ? aMEFTool : aMEFObj;
	  const TopTools_ListOfShape& aLF2=aMEF.FindFromKey(anEF2);
	  const TopoDS_Face& aF2=TopoDS::Face(aLF2.First());
	  //
	  bInternal1=(anOrEF1==TopAbs_INTERNAL);
	  bInternal2=(anOrEF2==TopAbs_INTERNAL);
	  //
	  if (bInternal1 || bInternal2) {
	    Standard_Integer nSpNotTaken;
	    //
	    nSpNotTaken=(nSpF1==nSpTaken) ? nSpF2 : nSpF1;
	    //
	    TopoDS_Edge aSS =TopoDS::Edge(aDS.Shape(nSpTaken));
	    TopoDS_Edge aSSx=TopoDS::Edge(aDS.Shape(nSpNotTaken));
	    
	    // a. Both parts are interlal edges
	    if (bInternal1 && bInternal2) {
	      BothInternals(aSS, myFace, aF2, iRankF1, myOperation, aWES); 
	    }
	    
	    // b. The first is internal , the second is not 
	    else if (bInternal1 && !bInternal2) {
	      FirstInternal(aSS, myFace, aF2, anEF2, aSSx, 
			    iRankF1, myOperation, aMEF, aWES, aContext);
	    }
	    
	    // c. The first is no1 internal , the second is  internal
	    else if (!bInternal1 && bInternal2) {
	      SecondInternal (aSS, aF1, aF2, anEF1, aSSx, 
			      anOrEF1, iRankF1, myOperation, aWES, aContext);
	    }
	    continue;
	  }
	}
	//
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	//
	Standard_Boolean bIsON2DToKeep, bIsON2DToKeepSimm=Standard_False;
	{
	  const TopoDS_Edge& anEFx=TopoDS::Edge(aDS.Shape(nEF2));
	  TopoDS_Edge aSpFx=TopoDS::Edge(aDS.Shape(nSpF2));
	  const TopTools_IndexedDataMapOfShapeListOfShape& aMEFx=
	    (iRankF1==1) ? aMEFTool : aMEFObj;
	  //
	  // anEF1
	  TopAbs_State aST1;
	  BOPTools_Tools3D::GetPlanes(aSpFx, anEFx, aMEFx, anEF1, aF1, aST1, aContext);
	  bIsON2DToKeep=
	    BOP_BuilderTools::IsPartOn2dToKeep(aST1, iRankF1, myOperation);
	  
	  if (BRep_Tool::IsClosed(anEF1, aF1)) {
	    TopoDS_Edge anEF1Seam;
	    BOPTools_Tools3D::GetSeam (aF1, anEF1, anEF1Seam);
	    //
	    if (!anEF1Seam.IsNull()) {
	      TopAbs_State aSTSeam;
	    
	      BOPTools_Tools3D::GetPlanes(aSpFx, anEFx, aMEFx, anEF1Seam, aF1, aSTSeam, aContext);
	      bIsON2DToKeepSimm=
		BOP_BuilderTools::IsPartOn2dToKeep(aSTSeam, iRankF1, myOperation);
	    }
	    bIsON2DToKeep=bIsON2DToKeep || bIsON2DToKeepSimm;
	  }
        }
	//

	if (nSpTaken==nSpF1) {
	  // Common Edge is from nEF1
	  const TopoDS_Shape& aSp1=aDS.Shape(nSpF1);
	  TopoDS_Edge aSS=TopoDS::Edge(aSp1);
	  aSS.Orientation(anOrEF1);
	  //
	  if (bIsON2DToKeep) {
	    aWES.AddStartElement (aSS);
	  }
	}
	else /*if (nSpTaken==nSpF2)*/ {
	  // Common Edge is from nEF2
	  
	  const TopoDS_Shape& aSp1=aDS.Shape(nSpF1);
	  TopoDS_Edge aSpF1=TopoDS::Edge(aSp1);
	  aSpF1.Orientation(anOrEF1);
	  
	  const TopoDS_Shape& aSp2=aDS.Shape(nSpF2);
	  TopoDS_Edge aSpF2=TopoDS::Edge(aSp2);
	  
	  Standard_Boolean bToReverse=
	    BOPTools_Tools3D::IsSplitToReverse1 (aSpF1, aSpF2, aContext);
	  if (bToReverse) {
	    aSpF2.Reverse();
	  }
	  //
	  if (BRep_Tool::IsClosed(aSpF1, myFace)) {
	    if (aM.Contains(aSpF2)){
	      continue;
	    }
	    aM.Add(aSpF2);
	    //
	    if (bIsON2DToKeep) {
	      if (!BRep_Tool::IsClosed(aSpF2, myFace)) {
		// modified by NIZHNY-MKK  Mon Feb 17 15:12:22 2003.BEGIN
		// 		BOPTools_Tools3D::DoSplitSEAMOnFace (aSpF2, myFace);
		Standard_Boolean bIsReversed = Standard_False;

		if(!BOPTools_Tools3D::DoSplitSEAMOnFace(aSpF2, aSpF1, myFace, bIsReversed))
		  BOPTools_Tools3D::DoSplitSEAMOnFace (aSpF2, myFace);
		// modified by NIZHNY-MKK  Mon Feb 17 15:12:25 2003.END
	      }

	      aWES.AddStartElement (aSpF2);
	      aSpF2.Reverse();
	      aWES.AddStartElement (aSpF2);
	    }
	    continue;  
	  }
	  //
	  if (bIsON2DToKeep) {
	    aWES.AddStartElement (aSpF2);
	  }
	}
      }// else x
    }// for (; anItCB.More(); anItCB.Next())
  }// for (; anExp.More(); anExp.Next())
}
//=======================================================================
// function: AddPartsEFSo
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddPartsEFSo (const Standard_Integer nF1, 
				     const Standard_Integer iFF,
				     const TopTools_IndexedDataMapOfShapeListOfShape& aMEFObj,
				     const TopTools_IndexedDataMapOfShapeListOfShape& aMEFTool,
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
  Standard_Boolean bIsKeepTwice, bIsAdjExists, bIsTouchCase;
  TopExp_Explorer anExp2, anExp1;
  //
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  
  
  const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
  const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));

  iRankF1=aDS.Rank(nF1);
  iRankF2=aDS.Rank(nF2);
  
  const TopTools_IndexedDataMapOfShapeListOfShape& aMEF=(iRankF2==1) ?  aMEFObj : aMEFTool;
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
	
	if (anEMap.Contains(aSpEF2)) {
	  continue;// next CB
	}
	anEMap.Add(aSpEF2);
	
	TopoDS_Edge aSS=TopoDS::Edge(aSpEF2);
	//
	TopoDS_Face aF2Adj;
	bIsAdjExists=BOPTools_Tools3D::GetAdjacentFace(aF2, aEF2, aMEF, aF2Adj);
	
	if (bIsAdjExists) {
	  bIsKeepTwice=BOPTools_Tools3D::IsKeepTwice(aF1, aF2, aF2Adj, aSS);
	  if (bIsKeepTwice) {
	    aWES.AddStartElement(aSS);
	    aSS.Reverse();
	    aWES.AddStartElement(aSS);
	  }
	  else {
	    aSS.Orientation(TopAbs_FORWARD);
	    BOP_BuilderTools::OrientSectionEdgeOnF1 
	      (myFace, aF2, iRankF1, myOperation, aSS);

	    aWES.AddStartElement(aSS);
	  }
	}
	else {
	  // No Adjacents
	  bIsTouchCase=BOPTools_Tools3D::IsTouchCase(aSS, myFace, aF2);
	  if (bIsTouchCase) {
	    aWES.AddStartElement(aSS);
	    aSS.Reverse();
	    aWES.AddStartElement(aSS);
	  }
	  else {
	    aSS.Orientation(TopAbs_FORWARD);
	    BOP_BuilderTools::OrientSectionEdgeOnF1
	      (myFace, aF2, iRankF1, myOperation, aSS);

	    aWES.AddStartElement(aSS);
	  }
	}
	//continue;
      }
      
    } // next CB on nEF2
  }
}
//=======================================================================
// function: AddINON2DPartsSo
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddINON2DPartsSo(const Standard_Integer iFF,
					const Standard_Integer nF1,
					const Standard_Integer nF2,
					BOP_WireEdgeSet& aWES) 
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  //
  Standard_Integer iSenseFlag, iRankF1;
  //
  iRankF1=aDS.Rank(nF1);
  //
  BOPTools_SSInterference& aFF=aFFs(iFF);
  //
  iSenseFlag=aFF.SenseFlag();
  //
  BOP_SDFWESFiller aWESFiller;
  aWESFiller.SetDSFiller(*myDSFiller);
  aWESFiller.SetFaces(nF1, nF2);
  aWESFiller.SetStatesMap(aFF.StatesMap());
  aWESFiller.SetSenseFlag(iSenseFlag);

  if (iSenseFlag==1) {
    switch (myOperation) {
  
    case BOP_FUSE: 
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
  }
  
  else if (iSenseFlag==-1) { // iSenseFlag<0
    switch (myOperation) {
    
    case BOP_FUSE:
      aWESFiller.SetOperation(BOP_CUT);
      aWESFiller.Do(aWES);
      break;
	
      case BOP_COMMON: 
	break;
	
      case BOP_CUT: 
      if (iRankF1==1) {
	aWESFiller.SetOperation(BOP_CUT);
	aWESFiller.Do(aWES);
	aWESFiller.SetOperation(BOP_COMMON);
	aWESFiller.Do(aWES);
      }
      break;
	
    case BOP_CUT21:
      if (iRankF1==2) {
	aWESFiller.SetOperation(BOP_CUT);
	aWESFiller.Do(aWES);
	aWESFiller.SetOperation(BOP_COMMON);
	aWESFiller.Do(aWES);
      }
    default:
      break;
    }
  }
}
//=======================================================================
// function: AddPartsEFSDSo
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddPartsEFSDSo (const Standard_Integer nF1, 
				       const Standard_Integer iFF,
				       const TopTools_IndexedDataMapOfShapeListOfShape& aMEFObj,
				       const TopTools_IndexedDataMapOfShapeListOfShape& aMEFTool,
				       BOP_WireEdgeSet& aWES)
{
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();

  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  
  const BOPTools_PaveFiller& aPF=myDSFiller->PaveFiller();
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*)&aPF;
  //
  Standard_Integer iRankF1, iRankF2, nF2, bid=0, nSpEF2, nEF2, iSenseFlag;
  Standard_Boolean bIsAdjExists, bKeptTwice;
  TopExp_Explorer anExp2, anExp1;
  //
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  iSenseFlag=aFF.SenseFlag();

  const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
  const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));

  iRankF1=aDS.Rank(nF1);
  iRankF2=aDS.Rank(nF2);
  
  const TopTools_IndexedDataMapOfShapeListOfShape& aMEF=(iRankF2==1) ?  aMEFObj : aMEFTool;
  //
  BOPTools_ListOfPaveBlock aLPB;
  BOPTools_ListIteratorOfListOfPaveBlock anIt;

  pPaveFiller->SplitsInFace(bid, nF2, nF1, aLPB);
  //
  anIt.Initialize(aLPB);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_PaveBlock& aPBF2=anIt.Value();
    nEF2=aPBF2.OriginalEdge();
    nSpEF2=aPBF2.Edge();
    const TopoDS_Edge& aEF2=TopoDS::Edge(aDS.Shape(nEF2));
    const TopoDS_Edge& aSpEF2=TopoDS::Edge(aDS.Shape(nSpEF2));
    //
    bKeptTwice=aWES.KeptTwice(aSpEF2);
    
    TopoDS_Face aF2Adj;
    bIsAdjExists=BOPTools_Tools3D::GetAdjacentFace(aF2, aEF2, aMEF, aF2Adj);
    
    TopAbs_State aSt;
    
    if (bIsAdjExists) {
      BOPTools_Tools3D::GetPointState(aSpEF2, aEF2, aF2Adj, aF1, aSt);
      TopoDS_Edge aSS=aSpEF2;
      aSS.Orientation(TopAbs_FORWARD);
      Standard_Boolean bIsTouchCase;

      if (iSenseFlag==-1) {
	if (aSt==TopAbs_IN) {
	  if (myOperation!=BOP_FUSE) {
	    if (bKeptTwice) {
	      aWES.RemoveEdgeFromWES(aSpEF2);
	      TopoDS_Edge aSpEF2R=TopoDS::Edge(aSpEF2.Reversed());
	      aWES.RemoveEdgeFromWES(aSpEF2R);
	    }
	    
	    bIsTouchCase=BOPTools_Tools3D::IsTouchCase(aSS, myFace, aF2Adj);
	    if (bIsTouchCase) {
	      //
	      aWES.AddStartElement(aSS);
	      aSS.Reverse();
	      aWES.AddStartElement(aSS);
	      //
	    }
	    else {
	      BOP_BuilderTools::OrientSectionEdgeOnF1 
		(myFace, aF2Adj, iRankF1, myOperation, aSS);
	      aWES.AddStartElement(aSS);
	    }
	  }
	  else {//myOperation==BOP_FUSE && aSt==TopAbs_IN
	    aWES.RemoveEdgeFromWES(aSpEF2);
	    TopoDS_Edge aSpEF2R=TopoDS::Edge(aSpEF2.Reversed());
	    aWES.RemoveEdgeFromWES(aSpEF2R);
	  }
	}
	
      }
      else {// if (iSenseFlag==1)
	if (aSt==TopAbs_OUT) {
	  if (myOperation!=BOP_FUSE) {
	    if (bKeptTwice) {
	      aWES.RemoveEdgeFromWES(aSpEF2);
	      TopoDS_Edge aSpEF2R=TopoDS::Edge(aSpEF2.Reversed());
	      aWES.RemoveEdgeFromWES(aSpEF2R);
	    }
	    bIsTouchCase=BOPTools_Tools3D::IsTouchCase(aSS, myFace, aF2Adj);
	    if (bIsTouchCase) {
	      //
	      aWES.AddStartElement(aSS);
	      aSS.Reverse();
	      aWES.AddStartElement(aSS);
	      //
	    }
	    else {
	      BOP_BuilderTools::OrientSectionEdgeOnF1
		(myFace, aF2Adj, iRankF1, myOperation, aSS);
	      aWES.AddStartElement(aSS);
	    }
	    //
	    if ((myOperation==BOP_CUT   && iRankF1==1)||
		(myOperation==BOP_CUT21 && iRankF1==2)) {
	      aWES.RemoveEdgeFromWES(aSpEF2);
	      TopoDS_Edge aSpEF2R=TopoDS::Edge(aSpEF2.Reversed());
	      aWES.RemoveEdgeFromWES(aSpEF2R);
	    }
	  }
	  else {//myOperation!=BOP_FUSE at aSt==TopAbs_OUT
	    bIsTouchCase=BOPTools_Tools3D::IsTouchCase(aSS, myFace, aF2Adj);
	    if (!bIsTouchCase) {
	      aWES.RemoveEdgeFromWES(aSpEF2);
	      TopoDS_Edge aSpEF2R=TopoDS::Edge(aSpEF2.Reversed());
	      aWES.RemoveEdgeFromWES(aSpEF2R);
	      
	      BOP_BuilderTools::OrientSectionEdgeOnF1
		(myFace, aF2Adj, iRankF1, myOperation, aSS);
	      aWES.AddStartElement(aSS);
	    }
	  }
	}
      }
    }
  }
}
//=======================================================================
// function: AddPartsEFNonSDSo
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddPartsEFNonSDSo (const Standard_Integer nF1, 
					  const Standard_Integer iFF,
					  const TopTools_IndexedDataMapOfShapeListOfShape& aMEFObj,
					  const TopTools_IndexedDataMapOfShapeListOfShape& aMEFTool,
					  const TColStd_IndexedMapOfInteger& aFFIndicesMap, 
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
  Standard_Boolean bIsKeepTwice, bIsAdjExists, bIsTouchCase;
  TopExp_Explorer anExp2, anExp1;
  //
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  
  const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
  const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));

  iRankF1=aDS.Rank(nF1);
  iRankF2=aDS.Rank(nF2);
  
  const TopTools_IndexedDataMapOfShapeListOfShape& aMEF=(iRankF2==1) ?  aMEFObj : aMEFTool;
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
	
	if (anEMap.Contains(aSpEF2)) {
	  continue;// next CB
	}
	anEMap.Add(aSpEF2);
	
	TopoDS_Edge aSS=TopoDS::Edge(aSpEF2);
	//
	TopoDS_Face aF2Adj;
	bIsAdjExists=BOPTools_Tools3D::GetAdjacentFace(aF2, aEF2, aMEF, aF2Adj);
	
	if (bIsAdjExists) {
	  //
	  
	  Standard_Integer nF2Adj;

	  nF2Adj=aDS.ShapeIndex(aF2Adj, iRankF2);

	  Standard_Boolean bIsSameDomainFaceWithF1;
	  
	  bIsSameDomainFaceWithF1=
	    BOP_BuilderTools::IsSameDomainFaceWithF1(nF1, nF2Adj, aFFIndicesMap, aFFs); 
	  
	  if (bIsSameDomainFaceWithF1) {
	    continue;
	  }
	  //
	  bIsKeepTwice=BOPTools_Tools3D::IsKeepTwice(aF1, aF2, aF2Adj, aSS);
	  if (bIsKeepTwice) {
	    aWES.AddStartElement(aSS);
	    aSS.Reverse();
	    aWES.AddStartElement(aSS);
	  }
	  else {
	    aSS.Orientation(TopAbs_FORWARD);
	    BOP_BuilderTools::OrientSectionEdgeOnF1
	      (myFace, aF2, iRankF1, myOperation, aSS);

	    aWES.AddStartElement(aSS);
	  }
	}
	else {
	  // No Adjacents
	  bIsTouchCase=BOPTools_Tools3D::IsTouchCase(aSS, myFace, aF2);
	  if (bIsTouchCase) {
	    aWES.AddStartElement(aSS);
	    aSS.Reverse();
	    aWES.AddStartElement(aSS);
	  }
	  else {
	    aSS.Orientation(TopAbs_FORWARD);
	    BOP_BuilderTools::OrientSectionEdgeOnF1 
	      (myFace, aF2, iRankF1, myOperation, aSS);

	    aWES.AddStartElement(aSS);
	  }
	}
      } //if (nFace==nF1) {
    } // next CB on nEF2
  }
}

//
//=======================================================================
// function: AddPartsEENonSDSo
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddPartsEENonSDSo (const Standard_Integer nF1, 
					  const Standard_Integer iFF,
					  const TopTools_IndexedDataMapOfShapeListOfShape& aMEFObj,
					  const TopTools_IndexedDataMapOfShapeListOfShape& aMEFTool,
					  const TColStd_IndexedMapOfInteger& aFFIndicesMap, 
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
  Standard_Integer nEF1, nF2, nSpF1, nSpF2, nEF2, nSpTaken, iRankF1, nF2x, iRankF2;
  TopAbs_Orientation anOrEF1, anOrEF2;
  TopExp_Explorer anExp;
  TopTools_IndexedMapOfShape aM;
  TColStd_ListOfInteger aSplitsOnF1;
  TColStd_ListIteratorOfListOfInteger anIt;
  TColStd_IndexedMapOfInteger aMSplitsOnF1;
  //
  // nF1
  const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
  iRankF1=aDS.Rank(nF1);
  const TopTools_IndexedDataMapOfShapeListOfShape& aMEFx=(iRankF1==1) ? aMEFTool : aMEFObj;
  //
  // nF2
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);

  iRankF2=aDS.Rank(nF2);

  const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));
  //
  pPaveFiller->SplitsOnFace(0, nF1, nF2, aSplitsOnF1);
  anIt.Initialize(aSplitsOnF1);
  for (; anIt.More(); anIt.Next()) {
    nSpF1=anIt.Value();
    aMSplitsOnF1.Add(nSpF1);
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
      //
      const TopoDS_Edge& aEF2=TopoDS::Edge(aDS.Shape(nEF2));
      TopoDS_Face aF2Adj;

      Standard_Boolean bIsAdjExists;
      bIsAdjExists=BOPTools_Tools3D::GetAdjacentFace(aF2, aEF2, aMEFx, aF2Adj);
      
      if (bIsAdjExists) {
	Standard_Boolean bIsSameDomainFaceWithF1;
	Standard_Integer nF2Adj;

	nF2Adj=aDS.ShapeIndex(aF2Adj, iRankF2);

	bIsSameDomainFaceWithF1=
	  BOP_BuilderTools::IsSameDomainFaceWithF1(nF1, nF2Adj, aFFIndicesMap, aFFs);
	
	if (bIsSameDomainFaceWithF1) {
	  continue;
	}
      }
      //
      const TopoDS_Shape& aSp1=aDS.Shape(nSpF1);
      const TopoDS_Shape& aSp2=aDS.Shape(nSpF2);
      
      //
      if (anEMap.Contains(aSp1)) {
	continue;// next CB
      }
      anEMap.Add(aSp1);
      //
      if (anEMap.Contains(aSp2)) {
	continue;// next CB
      }
      anEMap.Add(aSp2);
      //
      
      // Pave Block from which new edge will be taken
      const BOPTools_PaveBlock& aPB=aCB.PaveBlock1();
      nSpTaken=aPB.Edge();
      //
      //iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
      // Internal edges treatment
      {
	Standard_Boolean bInternal1, bInternal2;
	
	const TopoDS_Edge& anEF2=TopoDS::Edge(aDS.Shape(nEF2));
	anOrEF2=anEF2.Orientation();
	
	const TopTools_IndexedDataMapOfShapeListOfShape& aMEF=
	  (iRankF1==1) ? aMEFTool : aMEFObj;
	
	bInternal1=(anOrEF1==TopAbs_INTERNAL);
	bInternal2=(anOrEF2==TopAbs_INTERNAL);
	
	if (bInternal1 || bInternal2) {
	  Standard_Integer nSpNotTaken;
	  //
	  nSpNotTaken=(nSpF1==nSpTaken) ? nSpF2 : nSpF1;
	  
	  TopoDS_Edge aSS =TopoDS::Edge(aDS.Shape(nSpTaken));
	  TopoDS_Edge aSSx=TopoDS::Edge(aDS.Shape(nSpNotTaken));
	  // a. Both parts are interlal edges
	  if (bInternal1 && bInternal2) {
	    BothInternals(aSS, myFace, aF2, iRankF1, myOperation, aWES); 
	  }
	  // b. The first is internal, the second is not 
	  else if (bInternal1 && !bInternal2) {
	    FirstInternal(aSS, myFace, aF2, anEF2, aSSx, 
			  iRankF1, myOperation, aMEF, aWES, aContext);
	  }
	  
	  // c. The first is no1 internal , the second is  internal
	  else if (!bInternal1 && bInternal2) {
	    SecondInternal (aSS, aF1, aF2, anEF1, aSSx, 
			    anOrEF1, iRankF1, myOperation, aWES, aContext);
	  }
	  continue;
	}
      }
      //
      //iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
      //
      Standard_Boolean bIsON2DToKeep, bIsON2DToKeepSimm=Standard_False;
      {
	const TopoDS_Edge& anEFx=TopoDS::Edge(aDS.Shape(nEF2));
	TopoDS_Edge aSpFx=TopoDS::Edge(aDS.Shape(nSpF2));
	//
	// anEF1
	TopAbs_State aST1;
	BOPTools_Tools3D::GetPlanes(aSpFx, anEFx, aMEFx, anEF1, aF1, aST1, aContext);
	bIsON2DToKeep=
	  BOP_BuilderTools::IsPartOn2dToKeep(aST1, iRankF1, myOperation);
	
	if (BRep_Tool::IsClosed(anEF1, aF1)) {
	  TopoDS_Edge anEF1Seam;
	  BOPTools_Tools3D::GetSeam (aF1, anEF1, anEF1Seam);
	  //
	  if (!anEF1Seam.IsNull()) {
	    TopAbs_State aSTSeam;
	    
	    BOPTools_Tools3D::GetPlanes(aSpFx, anEFx, aMEFx, anEF1Seam, aF1, aSTSeam, aContext);
	    bIsON2DToKeepSimm=
	      BOP_BuilderTools::IsPartOn2dToKeep(aSTSeam, iRankF1, myOperation);
	  }
	  bIsON2DToKeep=bIsON2DToKeep || bIsON2DToKeepSimm;
	}
      }
      //
      
      if (nSpTaken==nSpF1) {
	// Common Edge is from nEF1
	
	TopoDS_Edge aSS=TopoDS::Edge(aSp1);
	aSS.Orientation(anOrEF1);
	//
	if (bIsON2DToKeep) {
	  aWES.AddStartElement (aSS);
	}
      }
      else /*if (nSpTaken==nSpF2)*/ {
	// Common Edge is from nEF2
	TopoDS_Edge aSpF1=TopoDS::Edge(aSp1);
	aSpF1.Orientation(anOrEF1);
	
	TopoDS_Edge aSpF2=TopoDS::Edge(aSp2);
	
	Standard_Boolean bToReverse= BOPTools_Tools3D::IsSplitToReverse1 (aSpF1, aSpF2, aContext);
	if (bToReverse) {
	  aSpF2.Reverse();
	}
	//
	if (BRep_Tool::IsClosed(aSpF1, myFace)) {
	  if (aM.Contains(aSpF2)){
	    continue;
	  }
	  aM.Add(aSpF2);
	  //
	  if (bIsON2DToKeep) {
	    if (!BRep_Tool::IsClosed(aSpF2, myFace)) {
	      BOPTools_Tools3D::DoSplitSEAMOnFace (aSpF2, myFace);
	      }
	    
	    aWES.AddStartElement (aSpF2);
	    aSpF2.Reverse();
	    aWES.AddStartElement (aSpF2);
	  }
	  continue;  
	}
	//
	if (bIsON2DToKeep) {
	  aWES.AddStartElement (aSpF2);
	}

      }// else /*if (nSpTaken==nSpF2)*/ {
    }// for (; anItCB.More(); anItCB.Next())
  }// for (; anExp.More(); anExp.Next())
}

//=======================================================================
// function: AddPartsEF
// purpose: 
//=======================================================================
void AddPartsEF (const TopoDS_Edge& aSSin,
		 const TopoDS_Edge& aEF2,
		 const TopoDS_Face& aF2,
		 const TopoDS_Face& myFace,
		 const Standard_Integer iRankF1,
		 const BOP_Operation myOperation,
		 const TopTools_IndexedDataMapOfShapeListOfShape& aMEF,
		 BOP_WireEdgeSet& aWES)
{
  Standard_Boolean bIsKeepTwice, bIsAdjExists, bIsTouchCase;
  TopoDS_Face aF2Adj;
  TopoDS_Edge aSS=aSSin;
  // 
  bIsAdjExists=BOPTools_Tools3D::GetAdjacentFace(aF2, aEF2, aMEF, aF2Adj);
  //
  if (bIsAdjExists) {
    bIsKeepTwice=BOPTools_Tools3D::IsKeepTwice(myFace, aF2, aF2Adj, aSS);
    if (bIsKeepTwice) {
      aWES.AddStartElement(aSS);
      aSS.Reverse();
      aWES.AddStartElement(aSS);
    }
    else {
      aSS.Orientation(TopAbs_FORWARD);
      BOP_BuilderTools::OrientSectionEdgeOnF1 (myFace, aF2, iRankF1, myOperation, aSS);
      aWES.AddStartElement(aSS);
    }
  }
  else {
    // No Adjacents
    bIsTouchCase=BOPTools_Tools3D::IsTouchCase(aSS, myFace, aF2);
    //
    if (bIsTouchCase) {
      aWES.AddStartElement(aSS);
      aSS.Reverse();
      aWES.AddStartElement(aSS);
    }
    else {
      aSS.Orientation(TopAbs_FORWARD);
      BOP_BuilderTools::OrientSectionEdgeOnF1 (myFace, aF2, iRankF1, myOperation, aSS);
      aWES.AddStartElement(aSS);
    }
  }
}

//=======================================================================
// function: AddPartEF1INF2
// purpose: 
//=======================================================================
void AddPartEF1INF2 (TopoDS_Edge& aSS,
		     const TopoDS_Face& myFace,
		     const TopoDS_Face& aF2,
		     const Standard_Integer iRankF1,
		     const BOP_Operation myOperation,
		     BOP_WireEdgeSet& aWES)
{
  Standard_Boolean bIsTouchCase;
  
  aSS.Orientation(TopAbs_FORWARD);
	    
  bIsTouchCase=BOPTools_Tools3D::IsTouchCase(aSS, myFace, aF2);
	    
  if (bIsTouchCase) {
    aWES.AddStartElement (aSS);
    aSS.Reverse();
    aWES.AddStartElement (aSS);
  }
  else {
    BOP_BuilderTools::OrientSectionEdgeOnF1(myFace, aF2, iRankF1, myOperation, aSS);
      aWES.AddStartElement (aSS);
  }
}     
//=======================================================================
// function: BothInternals
// purpose: 
//=======================================================================
void BothInternals (TopoDS_Edge& aSS,
		    const TopoDS_Face& myFace,
		    const TopoDS_Face& aF2,
		    const Standard_Integer iRankF1,
		    const BOP_Operation myOperation,
		    BOP_WireEdgeSet& aWES)
{
  Standard_Boolean bIsTouchCase;
  //
  aSS.Orientation(TopAbs_FORWARD);
  //
  bIsTouchCase=BOPTools_Tools3D::IsTouchCase(aSS, myFace, aF2);
  //
  if (bIsTouchCase) {
    aWES.AddStartElement (aSS);
    aSS.Reverse();
    aWES.AddStartElement (aSS);
  }
  else {
    BOP_BuilderTools::OrientSectionEdgeOnF1(myFace, aF2, iRankF1, myOperation, aSS);
    aWES.AddStartElement (aSS);
  }
}
//=======================================================================
// function: FirstInternal
// purpose: 
//=======================================================================
void FirstInternal(TopoDS_Edge& aSS,
		   const TopoDS_Face& myFace,
		   const TopoDS_Face& aF2,
		   const TopoDS_Edge& anEF2,
		   TopoDS_Edge& aSSx,
		   const Standard_Integer iRankF1,
		   const BOP_Operation myOperation,
		   const TopTools_IndexedDataMapOfShapeListOfShape& aMEF,
		   BOP_WireEdgeSet& aWES,
		   IntTools_Context& aContext)
{
  Standard_Boolean bToReverse;
  TopAbs_Orientation anOrEF2;
  TopoDS_Edge aEF2;
  
  BOPTools_Tools3D::OrientEdgeOnFace (anEF2, aF2, aEF2);
  anOrEF2=aEF2.Orientation();
  
  if (aSS.Orientation()==TopAbs_INTERNAL) {
    aSS.Orientation(TopAbs_FORWARD);
    aSSx.Orientation(anOrEF2);
    bToReverse=BOPTools_Tools3D::IsSplitToReverse1 (aSSx, aSS, aContext);
    if(bToReverse){
      aSS.Reverse();
    }
  }
  else {
    aSS.Orientation(anOrEF2);
  }
  
  AddPartsEF(aSS, aEF2, aF2, myFace, iRankF1, myOperation, aMEF, aWES);
}

//=======================================================================
// function: SecondInternal
// purpose: 
//=======================================================================
void SecondInternal(TopoDS_Edge& aSS,
		    const TopoDS_Face& aF1,
		    const TopoDS_Face& aF2,
		    const TopoDS_Edge& anEF1,
		    TopoDS_Edge& aSSx,
		    const TopAbs_Orientation anOrEF1,
		    const Standard_Integer iRankF1,
		    const BOP_Operation myOperation,
		    BOP_WireEdgeSet& aWES,
		    IntTools_Context& aContext)
{
  Standard_Boolean bToReverse, bIsToKeep;
  if (aSS.Orientation()==TopAbs_INTERNAL) {
    aSS.Orientation(TopAbs_FORWARD);
    aSSx.Orientation(anOrEF1);
    bToReverse=BOPTools_Tools3D::IsSplitToReverse1 (aSSx, aSS, aContext);
    if(bToReverse){
      aSS.Reverse();
    }
  }
  else {
    aSS.Orientation(anOrEF1);
  }
  //
  TopAbs_State aState1=
    BOPTools_Tools3D::GetStatePartIN2D(aSS, anEF1, aF1, aF2, aContext);
  //
  bIsToKeep=
    BOP_BuilderTools::IsPartIN2DToKeep(aState1, iRankF1, myOperation);
  //
  if (bIsToKeep) {
    aWES.AddStartElement (aSS);
  }
}

//=======================================================================
// function: AddPartsEESDSo
// purpose: 
//=======================================================================
  void BOP_ShellSolid::AddPartsEESDSo (const Standard_Integer nF1, 
				       const Standard_Integer iFF,
				       const TopTools_IndexedDataMapOfShapeListOfShape& aMEFObj,
				       const TopTools_IndexedDataMapOfShapeListOfShape& aMEFTool,
				       BOP_WireEdgeSet& aWES)
{
  if (myOperation==BOP_FUSE) {
    return;
  }
  //
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  BOPTools_InterferencePool* pIntrPool=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  
  const BOPTools_PaveFiller& aPF=myDSFiller->PaveFiller();
  BOPTools_PaveFiller* pPaveFiller=(BOPTools_PaveFiller*)&aPF;
  BOPTools_CommonBlockPool& aCBPool=pPaveFiller->ChangeCommonBlockPool();
  //
  IntTools_Context& aContext=pPaveFiller->ChangeContext();
  //
  Standard_Integer nEF1, nF2, nSpF1, nSpF2, nEF2, nSpTaken, iRankF1, nF2x, iRankF2, iSenseFlag;
  TopAbs_Orientation anOrEF1;
  TopExp_Explorer anExp;
  TopTools_IndexedMapOfShape aM;
  TColStd_ListOfInteger aSplitsOnF1;
  TColStd_ListIteratorOfListOfInteger anIt;
  TColStd_IndexedMapOfInteger aMSplitsOnF1;
  //
  // nF1
  const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
  iRankF1=aDS.Rank(nF1);
  const TopTools_IndexedDataMapOfShapeListOfShape& aMEFx=(iRankF1==1) ? aMEFTool : aMEFObj;
  //
  // nF2
  BOPTools_SSInterference& aFF=aFFs(iFF);
  nF2=aFF.OppositeIndex(nF1);
  iSenseFlag=aFF.SenseFlag();
  //
  if (iSenseFlag==1) {
    return ;
  }
  //
  iRankF2=aDS.Rank(nF2);
  //
  pPaveFiller->SplitsOnFace(0, nF1, nF2, aSplitsOnF1);
  //
  anIt.Initialize(aSplitsOnF1);
  for (; anIt.More(); anIt.Next()) {
    nSpF1=anIt.Value();
    aMSplitsOnF1.Add(nSpF1);
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
      //
      const TopoDS_Shape& aSp1=aDS.Shape(nSpF1);
      const TopoDS_Shape& aSp2=aDS.Shape(nSpF2);
      // Pave Block from which new edge will be taken
      const BOPTools_PaveBlock& aPB=aCB.PaveBlock1();
      nSpTaken=aPB.Edge();
      //
      Standard_Boolean bIsON2DToKeep; 
      {
	const TopoDS_Edge& anEFx=TopoDS::Edge(aDS.Shape(nEF2));
	TopoDS_Edge aSpFx=TopoDS::Edge(aDS.Shape(nSpF2));
	//
	// anEF1
	TopAbs_State aST1;
	BOPTools_Tools3D::GetPlanes(aSpFx, anEFx, aMEFx, anEF1, aF1, aST1, aContext);
	
	bIsON2DToKeep=BOP_BuilderTools::IsPartOn2dToKeep(aST1, iRankF1, myOperation);
	if (bIsON2DToKeep) {
	  bIsON2DToKeep=!bIsON2DToKeep;
	  if (aST1==TopAbs_IN) {
	    if (
		(iRankF1==2 && (myOperation==BOP_COMMON || myOperation==BOP_CUT)) ||
		(iRankF1==1 && (myOperation==BOP_COMMON || myOperation==BOP_CUT21))
		){
	      bIsON2DToKeep=!bIsON2DToKeep;
	    }
	  }
	}
      }
      if (bIsON2DToKeep) {
	//
	if (nSpTaken==nSpF1) {
	  // Common Edge is from nEF1
	  TopoDS_Edge aSS=TopoDS::Edge(aSp1);
	  aSS.Orientation(anOrEF1);
	  //
	  aWES.AddStartElement (aSS);
	}
	else /*if (nSpTaken==nSpF2)*/ {
	  // Common Edge is from nEF2
	  TopoDS_Edge aSpF1=TopoDS::Edge(aSp1);
	  aSpF1.Orientation(anOrEF1);
	  
	  TopoDS_Edge aSpF2=TopoDS::Edge(aSp2);
	  
	  Standard_Boolean bToReverse= BOPTools_Tools3D::IsSplitToReverse1 (aSpF1, aSpF2, aContext);
	  if (bToReverse) {
	    aSpF2.Reverse();
	  }
	  //
	  if (BRep_Tool::IsClosed(aSpF1, myFace)) {
	    if (aM.Contains(aSpF2)){
	      continue;
	    }
	    aM.Add(aSpF2);
	    //
	   
	    if (!BRep_Tool::IsClosed(aSpF2, myFace)) {
	      BOPTools_Tools3D::DoSplitSEAMOnFace (aSpF2, myFace);
	    }
	    
	    aWES.AddStartElement (aSpF2);
	    aSpF2.Reverse();
	    aWES.AddStartElement (aSpF2);
	    
	    continue;  
	  }
	  //
	    aWES.AddStartElement (aSpF2);
	}// else /*if (nSpTaken==nSpF2)*/ {
      }// if (bIsON2DToKeep) 
    }// for (; anItCB.More(); anItCB.Next())
  }// for (; anExp.More(); anExp.Next())
}
