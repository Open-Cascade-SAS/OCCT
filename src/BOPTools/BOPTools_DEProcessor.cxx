// File:	BOPTools_DEProcessor.cxx
// Created:	Wed Sep 12 12:10:52 2001
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <BOPTools_DEProcessor.ixx>


#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Solid.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <IntRes2d_IntersectionPoint.hxx>

#include <Precision.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <BRepExtrema_DistShapeShape.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>

#include <IntTools_Range.hxx>
#include <IntTools_Tools.hxx>
#include <IntTools_Context.hxx>

#include <BOPTools_DEInfo.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_PavePool.hxx>
#include <BOPTools_Pave.hxx>
#include <BOPTools_PaveSet.hxx>
#include <BOPTools_Tools3D.hxx>
#include <BOPTools_PaveBlockIterator.hxx>
#include <BOPTools_ListOfPave.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_StateFiller.hxx>

//=======================================================================
// function: BOPTools_DEProcessor::BOPTools_DEProcessor
// purpose: 
//=======================================================================
  BOPTools_DEProcessor::BOPTools_DEProcessor(const BOPTools_PaveFiller& aFiller,
					     const Standard_Integer aDim)
:
   myIsDone(Standard_False)
{
  myFiller=(BOPTools_PaveFiller*) &aFiller;
  myDS=myFiller->DS();
  
  myDim=aDim;
  if (aDim<2 || aDim>3) {
    myDim=3;
  }
  
}

//=======================================================================
// function: IsDone
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_DEProcessor::IsDone() const
{
  return myIsDone;
}
//=======================================================================
// function:  Do
// purpose: 
//=======================================================================
  void BOPTools_DEProcessor::Do()
{
  Standard_Integer aNbE;
  myIsDone=Standard_False;

  FindDegeneratedEdges();
  aNbE=myDEMap.Extent();
  
  if (!aNbE) {
    myIsDone=Standard_True;
    return;
  }
  
  DoPaves();
}

//=======================================================================
// function:  FindDegeneratedEdges
// purpose: 
//=======================================================================
  void BOPTools_DEProcessor::FindDegeneratedEdges()
{
  const BooleanOperations_ShapesDataStructure& aDS=*myDS;
  const BOPTools_PaveFiller& aPaveFiller=*myFiller;

  Standard_Integer i, aNbSourceShapes, nV, nF, nVx, ip, iRankE;
  TopAbs_ShapeEnum aType;

  const TopoDS_Shape& anObj=aDS.Object();
  const TopoDS_Shape& aTool=aDS.Tool();
  
  TopTools_IndexedDataMapOfShapeListOfShape aMEF;
  TopExp::MapShapesAndAncestors (anObj, TopAbs_EDGE, TopAbs_FACE, aMEF);
  TopExp::MapShapesAndAncestors (aTool, TopAbs_EDGE, TopAbs_FACE, aMEF);

  aNbSourceShapes=aDS.NumberOfSourceShapes();
  for (i=1; i<=aNbSourceShapes; i++) {
    const TopoDS_Shape& aS=aDS.Shape(i);
    aType=aS.ShapeType();
    if (aType==TopAbs_EDGE) {
      const TopoDS_Edge& aE=TopoDS::Edge(aS);
      if (BRep_Tool::Degenerated(aE)) {

	iRankE=aDS.Rank(i);

	TopoDS_Vertex aV=TopExp::FirstVertex(aE);

	nVx=aDS.ShapeIndex(aV, iRankE);
	//
	nV=nVx;
	ip=aPaveFiller.FindSDVertex(nV);
	if (ip) {
	  nV=ip;
	}
	//
	TColStd_ListOfInteger aLFn;
	const TopTools_ListOfShape& aLF=aMEF.FindFromKey(aE);
	TopTools_ListIteratorOfListOfShape anIt(aLF);
	for (; anIt.More(); anIt.Next()) {
	  const TopoDS_Shape& aF=anIt.Value();

	  nF=aDS.ShapeIndex(aF, iRankE);

	  aLFn.Append(nF);
	}
	BOPTools_DEInfo aDEInfo;
	aDEInfo.SetVertex(nV);
	aDEInfo.SetFaces(aLFn);

	myDEMap.Add (i, aDEInfo);

      }
    }
  }
  
}
//=======================================================================
// function:  DoPaves
// purpose: 
//=======================================================================
  void BOPTools_DEProcessor::DoPaves()
{

  Standard_Integer i, aNbE, nED, nVD, nFD=0;
  
  aNbE=myDEMap.Extent();
  for (i=1; i<=aNbE; i++) {
    nED=myDEMap.FindKey(i);
    
    const BOPTools_DEInfo& aDEInfo=myDEMap(i);
    nVD=aDEInfo.Vertex();
    // Fill PaveSet for the edge nED
    const TColStd_ListOfInteger& nLF=aDEInfo.Faces();
    TColStd_ListIteratorOfListOfInteger anIt(nLF);
    for (; anIt.More(); anIt.Next()) {
      nFD=anIt.Value();
      
      BOPTools_ListOfPaveBlock aLPB;
      FindPaveBlocks(nED, nVD, nFD, aLPB);
      FillPaveSet (nED, nVD, nFD, aLPB);
    }
    // 
    // Fill aSplitEdges for the edge nED
    FillSplitEdgesPool(nED);
    //
    // MakeSplitEdges
    MakeSplitEdges(nED, nFD);
    //
    // Compute States for Split parts
    if (myDim==3) {
      DoStates(nED, nFD);
    }
    if (myDim==2) {
      DoStates2D(nED, nFD);
    }
  }// next nED
}

//=======================================================================
// function:  DoStates
// purpose: 
//=======================================================================
  void BOPTools_DEProcessor::DoStates (const Standard_Integer nED, 
				       const Standard_Integer nFD)
{
  
  
  const BOPTools_SplitShapesPool& aSplitShapesPool=myFiller->SplitShapesPool();
  const BOPTools_ListOfPaveBlock& aSplitEdges=aSplitShapesPool(myDS->RefEdge(nED));

  const TopoDS_Edge& aDE=TopoDS::Edge(myDS->Shape(nED));
  const TopoDS_Face& aDF=TopoDS::Face(myDS->Shape(nFD));
  
  Standard_Integer nSp, iRank;
  Standard_Real aT, aT1, aT2, aTol=1e-7;
  TopoDS_Face aF;
  gp_Pnt2d aPx2DNear;
  gp_Pnt aPxNear;

  iRank=myDS->Rank(nED);
  const TopoDS_Shape& aReference=(iRank==1) ? myDS->Tool() : myDS->Object();

  BRepExtrema_DistShapeShape aDSS;
  aDSS.LoadS1(aReference);

  aF=aDF;
  aF.Orientation(TopAbs_FORWARD);

  BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSplitEdges);

  for (; aPBIt.More(); aPBIt.Next()) {
    BOPTools_PaveBlock& aPB=aPBIt.Value();
    
    nSp=aPB.Edge();
    const TopoDS_Edge& aSp=TopoDS::Edge(myDS->Shape(nSp));
    
    aPB.Parameters(aT1, aT2);
    aT=IntTools_Tools::IntermediatePoint(aT1, aT2);

    TopoDS_Edge aDERight, aSpRight;
    aDERight=aDE;
    aSpRight=aSp;

    BOPTools_Tools3D::OrientEdgeOnFace (aDE, aF, aDERight);
    aSpRight.Orientation(aDERight.Orientation());
    //
    {
      BRepAdaptor_Surface aBAS;
      aBAS.Initialize (aDF, Standard_False);

      if (aBAS.GetType()==GeomAbs_Sphere) {
	Standard_Real aDt2D, aR, aDelta=1.e-14;
	
	gp_Sphere aSphere=aBAS.Sphere();
	aR=aSphere.Radius();
	//
	aDt2D=acos (1.-4.*aTol/aR)+aDelta ;
	//
	BOPTools_Tools3D::PointNearEdge(aSpRight, aF, aT, aDt2D, aPx2DNear, aPxNear);
      }
      else {
	BOPTools_Tools3D::PointNearEdge(aSpRight, aF, aT, aPx2DNear, aPxNear);
      }
    }
    // 
    TopAbs_State aState;
    //
    TopAbs_ShapeEnum aTypeReference;
    aTypeReference=aReference.ShapeType();

    if (aTypeReference==TopAbs_SOLID) {
      // ... \ Solid processing 
      IntTools_Context& aContext=myFiller->ChangeContext();
      const TopoDS_Solid& aReferenceSolid=TopoDS::Solid(aReference);
      BRepClass3d_SolidClassifier& SC=aContext.SolidClassifier(aReferenceSolid);
      //
      SC.Perform(aPxNear, aTol);
      //
      aState=SC.State();
    }
    //
    
    else if (aTypeReference==TopAbs_SHELL || 
	     aTypeReference==TopAbs_FACE) {
      // ... \ Shell processing 
      TopoDS_Vertex aVxNear;
      BRep_Builder BB;
      
      BB.MakeVertex(aVxNear, aPxNear, aTol);

      aDSS.LoadS2(aVxNear);
      aDSS.Perform();
      
      aState=TopAbs_OUT;
      if (aDSS.IsDone()) {
	Standard_Real aDist=aDSS.Value();
	if (aDist < aTol) {
	  aState=TopAbs_ON;
	}
      }
    }
    else {
      // unknown aTypeReference
      aState=TopAbs_OUT;
    }
    //
    BooleanOperations_StateOfShape aSt;

    aSt=BOPTools_StateFiller::ConvertState(aState);

    myDS->SetState(nSp, aSt);
  }
}
//=======================================================================
// function:  DoStates2D
// purpose: 
//=======================================================================
  void BOPTools_DEProcessor::DoStates2D (const Standard_Integer nED, 
					 const Standard_Integer nFD)
{
  
  
  const BOPTools_SplitShapesPool& aSplitShapesPool=myFiller->SplitShapesPool();
  const BOPTools_ListOfPaveBlock& aSplitEdges=aSplitShapesPool(myDS->RefEdge(nED));

  const TopoDS_Edge& aDE=TopoDS::Edge(myDS->Shape(nED));
  const TopoDS_Face& aDF=TopoDS::Face(myDS->Shape(nFD));
  
  Standard_Integer nSp, iRank;
  Standard_Real aT, aT1, aT2;
  TopoDS_Face aF;
  gp_Pnt2d aPx2DNear;
  gp_Pnt aPxNear;

  iRank=myDS->Rank(nED);
  const TopoDS_Shape& aReference=(iRank==1) ? myDS->Tool() : myDS->Object();
  const TopoDS_Face& aFaceReference=TopoDS::Face(aReference);

  aF=aDF;
  aF.Orientation(TopAbs_FORWARD);

  BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSplitEdges);

  for (; aPBIt.More(); aPBIt.Next()) {
    BOPTools_PaveBlock& aPB=aPBIt.Value();
    
    nSp=aPB.Edge();
    const TopoDS_Edge& aSp=TopoDS::Edge(myDS->Shape(nSp));
    
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
    Standard_Boolean bIsValidPoint;
    TopAbs_State aState=TopAbs_OUT;
    //
    IntTools_Context& aContext=myFiller->ChangeContext();
    bIsValidPoint=aContext.IsValidPointForFace(aPxNear, aFaceReference, 1.e-3);
    //
    if (bIsValidPoint) {
      aState=TopAbs_IN;
    }
    //
    BooleanOperations_StateOfShape aSt;
    
    aSt=BOPTools_StateFiller::ConvertState(aState);

    myDS->SetState(nSp, aSt);
  }
}

//=======================================================================
// function:  FillSplitEdgesPool
// purpose: 
//=======================================================================
  void BOPTools_DEProcessor::FillSplitEdgesPool (const Standard_Integer nED)
{
  BOPTools_SplitShapesPool& aSplitShapesPool=myFiller->ChangeSplitShapesPool();
  //
  BOPTools_ListOfPaveBlock& aSplitEdges=aSplitShapesPool.ChangeValue(myDS->RefEdge(nED));
  //
  aSplitEdges.Clear();
  //
  const BOPTools_PavePool& aPavePool=myFiller->PavePool();
  BOPTools_PavePool* pPavePool=(BOPTools_PavePool*) &aPavePool;
  BOPTools_PaveSet& aPaveSet= pPavePool->ChangeValue(myDS->RefEdge(nED));
  
  BOPTools_PaveBlockIterator aPBIt(nED, aPaveSet);
  for (; aPBIt.More(); aPBIt.Next()) {
    BOPTools_PaveBlock& aPB=aPBIt.Value();
    aSplitEdges.Append(aPB);
  }
}

//=======================================================================
// function:  MakeSplitEdges
// purpose: 
//=======================================================================
  void BOPTools_DEProcessor::MakeSplitEdges (const Standard_Integer nED,
					     const Standard_Integer nFD)
{
  const BOPTools_SplitShapesPool& aSplitShapesPool=myFiller->SplitShapesPool();
  const BOPTools_ListOfPaveBlock& aSplitEdges=aSplitShapesPool(myDS->RefEdge(nED));

  Standard_Integer nV1, nV2, aNewShapeIndex;
  Standard_Real    t1, t2;
  TopoDS_Edge aE, aESplit;
  TopoDS_Vertex aV1, aV2;

  const TopoDS_Edge aDE=TopoDS::Edge(myDS->Shape(nED));
  const TopoDS_Face aDF=TopoDS::Face(myDS->Shape(nFD));

  BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSplitEdges);

  for (; aPBIt.More(); aPBIt.Next()) {
    BOPTools_PaveBlock& aPB=aPBIt.Value();
    
    const BOPTools_Pave& aPave1=aPB.Pave1();
    nV1=aPave1.Index();
    t1=aPave1.Param();
    aV1=TopoDS::Vertex(myDS->GetShape(nV1));
    aV1.Orientation(TopAbs_FORWARD);
    
    const BOPTools_Pave& aPave2=aPB.Pave2();
    nV2=aPave2.Index();
    t2=aPave2.Param();
    aV2=TopoDS::Vertex(myDS->GetShape(nV2));
    aV2.Orientation(TopAbs_REVERSED);
    
    MakeSplitEdge(aDE, aDF, aV1, t1, aV2, t2, aESplit); 
    //
    // Add Split Part of the Original Edge to the DS
    BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
    
    anASSeq.SetNewSuccessor(nV1);
    anASSeq.SetNewOrientation(aV1.Orientation());
    
    anASSeq.SetNewSuccessor(nV2);
    anASSeq.SetNewOrientation(aV2.Orientation());
    
    myDS->InsertShapeAndAncestorsSuccessors(aESplit, anASSeq);
    aNewShapeIndex=myDS->NumberOfInsertedShapes();
    myDS->SetState(aNewShapeIndex, BooleanOperations_UNKNOWN);
    //
    // Fill Split Set for the Original Edge
    aPB.SetEdge(aNewShapeIndex);
    //
  }
}
//=======================================================================
// function:  MakeSplitEdge
// purpose: 
//=======================================================================
  void BOPTools_DEProcessor::MakeSplitEdge (const TopoDS_Edge&   aE,
					    const TopoDS_Face&   aF,
					    const TopoDS_Vertex& aV1,
					    const Standard_Real  aP1,
					    const TopoDS_Vertex& aV2,
					    const Standard_Real  aP2,
					    TopoDS_Edge& aNewEdge)
{
  Standard_Real aTol=1.e-7;

  TopoDS_Edge E=aE;

  E.EmptyCopy();
  BRep_Builder BB;
  BB.Add  (E, aV1);
  BB.Add  (E, aV2);

  BB.Range(E, aF, aP1, aP2);

  BB.Degenerated(E, Standard_True);

  BB.UpdateEdge(E, aTol);
  aNewEdge=E;
}


			
//=======================================================================
// function:  FillPaveSet
// purpose: 
//=======================================================================
  void BOPTools_DEProcessor::FillPaveSet (const Standard_Integer nED,
					  const Standard_Integer nVD,
					  const Standard_Integer nFD,
					  BOPTools_ListOfPaveBlock& aLPB)
{
  Standard_Boolean bIsDone, bXDir;
  Standard_Integer nE, aNbPoints, j;
  Standard_Real aTD1, aTD2, aT1, aT2, aTolInter, aX, aDT;
  //
  aDT=Precision::PConfusion();
  //
  BOPTools_PaveSet& aPaveSet= (myFiller->ChangePavePool()).ChangeValue(myDS->RefEdge(nED));
  //
  // Clear aPaveSet, aSplitEdges
  aPaveSet.ChangeSet().Clear();
  //
  const TopoDS_Edge& aDE=TopoDS::Edge(myDS->Shape(nED));
  const TopoDS_Face& aDF=TopoDS::Face(myDS->Shape(nFD));
  //
  // 2D Curve of degenerated edge on the face aDF
  Handle(Geom2d_Curve) aC2DDE=BRep_Tool::CurveOnSurface(aDE, aDF, aTD1, aTD2);
  //
  // Choose direction for Degenerated Edge
  gp_Pnt2d aP2d1, aP2d2;
  aC2DDE->D0(aTD1, aP2d1);
  aC2DDE->D0(aTD2, aP2d2);

  bXDir=Standard_False;
  if (fabs(aP2d1.Y()-aP2d2.Y()) < aDT){
    bXDir=!bXDir;
  }
  //
  // Prepare bounding Paves
  BOPTools_Pave aPave1 (nVD, aTD1, BooleanOperations_UnknownInterference);
  aPaveSet.Append(aPave1);
  BOPTools_Pave aPave2 (nVD, aTD2, BooleanOperations_UnknownInterference);
  aPaveSet.Append(aPave2);
  //
  // Fill other paves 
  BOPTools_ListIteratorOfListOfPaveBlock anIt(aLPB);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_PaveBlock& aPB=anIt.Value();
    nE=aPB.Edge();
    const TopoDS_Edge& aE=TopoDS::Edge(myDS->Shape(nE));
    
    Handle(Geom2d_Curve) aC2D=BRep_Tool::CurveOnSurface(aE, aDF, aT1, aT2);
    //
    // Intersection
    aTolInter=0.001;
    
    
    Geom2dAdaptor_Curve aGAC1, aGAC2;
    
    aGAC1.Load(aC2DDE, aTD1, aTD2);
    Handle(Geom2d_Line) aL2D= Handle(Geom2d_Line)::DownCast(aC2D);
    if (!aL2D.IsNull()) {
      aGAC2.Load(aC2D);
    }
    else {
      aGAC2.Load(aC2D, aT1, aT2);
    }
    
    Geom2dInt_GInter aGInter(aGAC1, aGAC2, aTolInter, aTolInter);
    
    bIsDone=aGInter.IsDone();
    if(bIsDone) {
      aNbPoints=aGInter.NbPoints();
      if (aNbPoints) { 
	for (j=1; j<=aNbPoints; ++j) {
	  gp_Pnt2d aP2D=aGInter.Point(j).Value();
	  //
	  aX=(bXDir) ? aP2D.X(): aP2D.Y();
	  //
	  if (fabs (aX-aTD1) < aDT || fabs (aX-aTD2) < aDT) {
	    continue; 
	  }
	  if (aX < aTD1 || aX > aTD2) {
	    continue; 
	  }
	  //
	  Standard_Boolean bRejectFlag=Standard_False;
	  const BOPTools_ListOfPave& aListOfPave=aPaveSet.Set();
	  BOPTools_ListIteratorOfListOfPave aPaveIt(aListOfPave);
	  for (; aPaveIt.More(); aPaveIt.Next()) {
	    const BOPTools_Pave& aPavex=aPaveIt.Value();
	    Standard_Real aXx=aPavex.Param();
	    if (fabs (aX-aXx) < aDT) {
	      bRejectFlag=Standard_True;
	      break;
	    }
	  }
	  if (bRejectFlag) {
	    continue; 
	  }
	  //
	  BOPTools_Pave aPave(nVD, aX, BooleanOperations_UnknownInterference);
	  aPaveSet.Append(aPave);
	}
      }
    }
  }
}

//=======================================================================
// function:  FindPaveBlocks
// purpose: 
//=======================================================================
  void BOPTools_DEProcessor::FindPaveBlocks(const Standard_Integer ,
					    const Standard_Integer nVD,
					    const Standard_Integer nFD,
					    BOPTools_ListOfPaveBlock& aLPBOut)
{

  BOPTools_CArray1OfSSInterference& aFFs=(myFiller->InterfPool())->SSInterferences();
  
  BOPTools_ListIteratorOfListOfPaveBlock anIt;
  Standard_Integer i, aNb, nF2, nSp, nV;

  //ZZ const TopoDS_Edge& aDE=TopoDS::Edge(myDS->Shape(nED));
  
  aNb=aFFs.Extent();
  for (i=1; i<=aNb; i++) {
    BOPTools_SSInterference& aFF=aFFs(i);
    //
    nF2=aFF.OppositeIndex(nFD);
    if (!nF2) {
      continue;
    }
    //
    // Split Parts 
    const BOPTools_ListOfPaveBlock& aLPBSplits=aFF.PaveBlocks();
    anIt.Initialize(aLPBSplits);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_PaveBlock& aPBSp=anIt.Value();
      nSp=aPBSp.Edge();
      
      const BOPTools_Pave& aPave1=aPBSp.Pave1();
      nV=aPave1.Index();
      if (nV==nVD) {
	aLPBOut.Append(aPBSp);
	continue;
      }
      
      const BOPTools_Pave& aPave2=aPBSp.Pave2();
      nV=aPave2.Index();
      if (nV==nVD) {
	aLPBOut.Append(aPBSp);
	continue;
      }
    }
    //
    // Section Parts
    Standard_Integer j, aNbCurves;   
    BOPTools_SequenceOfCurves& aSC=aFF.Curves();
    aNbCurves=aSC.Length();
    
    for (j=1; j<=aNbCurves; j++) {
      const BOPTools_Curve& aBC=aSC(j);
      const BOPTools_ListOfPaveBlock& aLPBSe=aBC.NewPaveBlocks();

      anIt.Initialize(aLPBSe);
      for (; anIt.More(); anIt.Next()) {
	const BOPTools_PaveBlock& aPBSe=anIt.Value();
	
	const BOPTools_Pave& aPv1=aPBSe.Pave1();
	nV=aPv1.Index();
	if (nV==nVD) {
	  aLPBOut.Append(aPBSe);
	  continue;
	}
	
	const BOPTools_Pave& aPv2=aPBSe.Pave2();
	nV=aPv2.Index();
	if (nV==nVD) {
	  aLPBOut.Append(aPBSe);
	  continue;
	}
      }
    }

  } // for (i=1; i<=aNb; i++) Next FF interference
  
}
