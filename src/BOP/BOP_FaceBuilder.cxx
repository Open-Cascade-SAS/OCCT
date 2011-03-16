// File:	BOP_FaceBuilder.cxx
// 
// Author:	Mister Open CAS.CADE
// 

#include <BOP_FaceBuilder.ixx>

#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <TopAbs_Orientation.hxx>

#include <TopLoc_Location.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopExp.hxx>

#include <IntTools_Tools.hxx>
#include <IntTools_FClass2d.hxx>
#include <IntTools_Context.hxx>

#include <BOP_WireEdgeClassifier.hxx>
#include <BOP_Loop.hxx>
#include <BOP_BlockBuilder.hxx>
#include <BOP_LoopSet.hxx>
#include <BOP_WESCorrector.hxx>
#include <BOPTools_Tools2D.hxx>
#include <BOPTools_Tools3D.hxx>

#include <BOP_ListOfConnexityBlock.hxx>
#include <BOP_BuilderTools.hxx>
#include <BOP_ListIteratorOfListOfConnexityBlock.hxx>
#include <BOP_ConnexityBlock.hxx>

static void DoTopologicalVerification(TopoDS_Face& F);

//=======================================================================
//function : BOP_FaceBuilder
//purpose  : 
//=======================================================================
  BOP_FaceBuilder::BOP_FaceBuilder():
  myTreatment(0),
  myManifoldFlag(Standard_True),
  myTreatSDScales(0)
{
}

//=======================================================================
//function : Do
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::Do(const BOP_WireEdgeSet& aWES,
			   const Standard_Boolean bForceClass) 
{
  myFace=aWES.Face();
  myWES=(BOP_WireEdgeSet*) &aWES;
  //
  // 
  BOP_WESCorrector aWESCor;
  aWESCor.SetWES(aWES);
  aWESCor.Do();
  BOP_WireEdgeSet& aNewWES=aWESCor.NewWES();
  //
  //Make Loops. Only Loops are allowed after WESCorrector 
  MakeLoops(aNewWES);
  //
  BOP_BlockBuilder& aBB = myBlockBuilder;
  BOP_WireEdgeClassifier WEC(myFace, aBB);
  BOP_LoopSet& LS = myLoopSet;
  //
  myFaceAreaBuilder.InitFaceAreaBuilder(LS, WEC, bForceClass);

  BuildNewFaces();
  
  
  if (myTreatment==0) {
    DoInternalEdges(); 
  }
  if (myTreatSDScales) {
    SDScales();
  }

  // do topological verification
  TopTools_ListIteratorOfListOfShape anIt;
  anIt.Initialize(myNewFaces);
  for(anIt.Initialize(myNewFaces); anIt.More(); anIt.Next()) {
    TopoDS_Face& aF = TopoDS::Face(anIt.Value());
    DoTopologicalVerification(aF);
  }
}


//=======================================================================
//function : DoInternalEdges
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::DoInternalEdges()
{
  Standard_Integer i, aNbE, aNbSE, aNb, aNbF;
  TopTools_IndexedDataMapOfShapeListOfShape aDifferenceMap, aFLEMap;
  TopTools_IndexedMapOfOrientedShape aStartElementsMap, anEdgesMap;
  TopTools_IndexedMapOfShape anInternalEdges;
  //
  const TopTools_ListOfShape& aStartElements=myWES->StartElements();

  TopTools_ListIteratorOfListOfShape anIt(aStartElements);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aE=anIt.Value();
    aStartElementsMap.Add(aE);
  }

  anIt.Initialize(myNewFaces);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aF=anIt.Value();
    TopExp_Explorer anExp (aF, TopAbs_EDGE);
    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aE=anExp.Current();
      anEdgesMap.Add(aE);
    }   
  }

  

  aNbSE=aStartElementsMap.Extent();
  aNbE=anEdgesMap.Extent();

  if (aNbE==aNbSE) {
    return;
  }

  for (i=1; i<=aNbSE; i++) {
    const TopoDS_Shape& aE=aStartElementsMap(i);
    if (!anEdgesMap.Contains(aE)) {
      if (!aDifferenceMap.Contains(aE)) {
	TopTools_ListOfShape aLEx;
	aLEx.Append(aE);
	aDifferenceMap.Add(aE, aLEx);
      }
      else {
	TopTools_ListOfShape& aLEx=aDifferenceMap.ChangeFromKey(aE);
	aLEx.Append(aE);
      }
    }
  }

  aNbE=aDifferenceMap.Extent();
  if(!aNbE) {
    return;
  }

  for (i=1; i<=aNbE; i++) {
    const TopoDS_Shape& aE=aDifferenceMap.FindKey(i);
    const TopTools_ListOfShape& aLE=aDifferenceMap(i);
    aNb=aLE.Extent();
    if (aNb==2) {
      const TopoDS_Edge& anEdge=TopoDS::Edge(aE);
      if (!BRep_Tool::IsClosed(anEdge, myFace)) {
	anInternalEdges.Add(aE);
      }
    }
    //
    if (aNb==1) {
      const TopoDS_Edge& anEdge=TopoDS::Edge(aE);
      if (anEdge.Orientation()==TopAbs_INTERNAL) {
	anInternalEdges.Add(aE);
      }
    }
    //
  }
  
  aNbE=anInternalEdges.Extent();
  if(!aNbE) {
    return;
  }

  aFLEMap.Clear();

  for (i=1; i<=aNbE; i++) {
    const TopoDS_Edge& aEx=TopoDS::Edge(anInternalEdges(i));
    TopoDS_Edge aE=aEx;

    Standard_Real aT, aT1, aT2, aToler;
    Standard_Boolean bHasCurveOnSurface, bIsPointInOnFace;
    Handle(Geom2d_Curve)aC2D;

    bHasCurveOnSurface=
      BOPTools_Tools2D::HasCurveOnSurface(aE, myFace, aC2D, aT1, aT2, aToler);

    if (bHasCurveOnSurface) {
      aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
      gp_Pnt2d aP2D;
      aC2D->D0(aT, aP2D);
      //
      anIt.Initialize(myNewFaces);
      
      for (; anIt.More(); anIt.Next()) {
	TopoDS_Face& aF=TopoDS::Face(anIt.Value());
	//
	IntTools_Context aCtx;
	bIsPointInOnFace=aCtx.IsPointInOnFace(aF, aP2D);
	//
	if (bIsPointInOnFace) {
	  //
	  if (!aFLEMap.Contains(aF)) {
	    TopTools_ListOfShape aLE;
	    aLE.Append(aEx);
	    aFLEMap.Add(aF, aLE);
	  }
	  else {
	    TopTools_ListOfShape& aLE=aFLEMap.ChangeFromKey(aF);
	    aLE.Append(aEx);
	  }
	  break;
	}
      } //for (; anIt.More(); anIt.Next()) 
    }// if (bHasCurveOnSurface)
  } // for (i=1; i<=aNbE; i++)
  //
  // Make Wires from Internal Edges and Add the Wires to the faces
  aNbF=aFLEMap.Extent();
  BRep_Builder aBB;
  for (i=1; i<=aNbF; i++) {
    const TopoDS_Face& aF=TopoDS::Face(aFLEMap.FindKey(i));
    TopoDS_Face* pF=(TopoDS_Face*)&aF;

    const TopTools_ListOfShape& aLE=aFLEMap(i);
    //
    BOP_ListOfConnexityBlock aLConBlks;

    BOP_BuilderTools::MakeConnexityBlocks(aLE, TopAbs_EDGE, aLConBlks);

    BOP_ListIteratorOfListOfConnexityBlock aConBlkIt(aLConBlks);
    for (; aConBlkIt.More(); aConBlkIt.Next()) {
      BOP_ConnexityBlock& aConnexityBlock=aConBlkIt.Value();
      const TopTools_ListOfShape& aLECB=aConnexityBlock.Shapes();
      
      aNbE=aLECB.Extent();
      if (aNbE) {
	TopoDS_Wire aW;
	aBB.MakeWire(aW);

	anIt.Initialize(aLECB);
	for (; anIt.More(); anIt.Next()) {
	  TopoDS_Edge& aE=TopoDS::Edge(anIt.Value());
	  aE.Orientation(TopAbs_INTERNAL);
	  aBB.Add(aW, aE);
	}
	
	aBB.Add(*pF, aW);
      }
    }
    //
  }
}

//=======================================================================
//function : BuildNewFaces
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::BuildNewFaces() 
{
  Standard_Integer nF, nW, nE;
  Standard_Real    aTol;
  TopLoc_Location aLoc;
  TopoDS_Face newFace;
  TopoDS_Wire newWire;
  BRep_Builder aBB;
  Standard_Boolean bValidWire, bValidFace;
  
  Handle(Geom_Surface) aSurface=BRep_Tool::Surface(myFace, aLoc);
  aTol=BRep_Tool::Tolerance(myFace);

  myNewFaces.Clear();
  myNegatives.Clear();

  nF=InitFace();
  for (; MoreFace(); NextFace()) {
    bValidFace=Standard_False;
    aBB.MakeFace (newFace, aSurface, aLoc, aTol);

    nW=InitWire();
    for (; MoreWire(); NextWire()) {
      if (IsOldWire()) {
	newWire=TopoDS::Wire(OldWire());
      }
      else {
	aBB.MakeWire(newWire);
	nE=InitEdge();
	for (; MoreEdge(); NextEdge()) {
	  const TopoDS_Edge& newEdge=Edge();
	  aBB.Add(newWire, newEdge);
	}
      }
      
      bValidWire=BOPTools_Tools3D::IsConvexWire(newWire);
      if (bValidWire) {
	bValidFace=Standard_True;
	aBB.Add(newFace, newWire);
      }

      else {
	if (!myManifoldFlag && myTreatment==1) {
	  myNewFaces.Append (newWire);
	}
      }
    } // end of for (; MoreWire(); NextWire())
    
    if (bValidFace) {
      
      Standard_Boolean bIsValidIn2D, bNegativeFlag;
      Standard_Integer iNegativeFlag;

      bIsValidIn2D=BOPTools_Tools3D::IsValidArea (newFace, bNegativeFlag);
      if(bIsValidIn2D) {
	myNewFaces.Append (newFace);
	iNegativeFlag=(Standard_Integer)bNegativeFlag;
	myNegatives.Append(iNegativeFlag);
      }
      
    }
  }
}

//=======================================================================
//function : MakeLoops
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::MakeLoops(BOP_WireEdgeSet& SS)
{
  //BOP_BlockBuilder& BB = myBlockBuilder;
  BOP_ListOfLoop& LL = myLoopSet.ChangeListOfLoop();
  
  // Build blocks on elements of SS [ Ready to remove this invocation]
  // make list of loop (LL) of the LoopSet
  // - on shapes of the ShapeSet (SS)
  // - on blocks of the BlockBuilder (BB)
  //
  // Add shapes of SS as shape loops
  LL.Clear();
  for(SS.InitShapes();SS.MoreShapes();SS.NextShape()) {
    const TopoDS_Shape& S = SS.Shape();
    Handle(BOP_Loop) ShapeLoop = new BOP_Loop(S);
    LL.Append(ShapeLoop);
  }
  
}

//=======================================================================
//function : SetTreatment
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::SetTreatment(const Standard_Integer aTreatment)
{
  myTreatment=aTreatment;
}

//=======================================================================
//function : Treatment
//purpose  : 
//=======================================================================
  Standard_Integer BOP_FaceBuilder::Treatment()const 
{
  return myTreatment;
}
//=======================================================================
//function : SetTreatSDScales
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::SetTreatSDScales(const Standard_Integer aTreatment)
{
  myTreatSDScales=aTreatment;
}

//=======================================================================
//function : TreatSDScales
//purpose  : 
//=======================================================================
  Standard_Integer BOP_FaceBuilder::TreatSDScales()const 
{
  return myTreatSDScales;
}
//=======================================================================
//function : SetManifoldFlag
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::SetManifoldFlag(const Standard_Boolean aManifoldFlag)
{
  myManifoldFlag=aManifoldFlag;
}


//=======================================================================
//function : ManifoldFlag
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_FaceBuilder::ManifoldFlag()const
{
  return myManifoldFlag;
}

//=======================================================================
//function : InitFace
//purpose  : 
//=======================================================================
  Standard_Integer BOP_FaceBuilder::InitFace()
{
  Standard_Integer n = myFaceAreaBuilder.InitArea();
  return n;
}
//=======================================================================
//function : MoreFace
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_FaceBuilder::MoreFace() const
{
  Standard_Boolean b = myFaceAreaBuilder.MoreArea();
  return b;
}
//=======================================================================
//function : NextFace
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::NextFace()
{
  myFaceAreaBuilder.NextArea();
}
//=======================================================================
//function : InitWire
//purpose  : 
//=======================================================================
  Standard_Integer BOP_FaceBuilder::InitWire()
{
  Standard_Integer n = myFaceAreaBuilder.InitLoop();
  return n;
}
//=======================================================================
//function : MoreWire
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_FaceBuilder::MoreWire() const
{
  Standard_Boolean b = myFaceAreaBuilder.MoreLoop();
  return b;
}
//=======================================================================
//function : NextWire
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::NextWire()
{
  myFaceAreaBuilder.NextLoop();
}
//=======================================================================
//function : IsOldWire
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_FaceBuilder::IsOldWire() const
{
  const Handle(BOP_Loop)& L = myFaceAreaBuilder.Loop();
  Standard_Boolean b = L->IsShape();
  return b;
}
//=======================================================================
//function : OldWire
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BOP_FaceBuilder::OldWire() const
{
  const Handle(BOP_Loop)& L = myFaceAreaBuilder.Loop();
  const TopoDS_Shape& B = L->Shape();
  return B;
}
//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================
  const TopoDS_Wire& BOP_FaceBuilder::Wire() const
{
  return TopoDS::Wire(OldWire());
}

//=======================================================================
//function : InitEdge
//purpose  : 
//=======================================================================
  Standard_Integer BOP_FaceBuilder::InitEdge()
{
  const Handle(BOP_Loop)& L = myFaceAreaBuilder.Loop();
  if ( L->IsShape() ){
    Standard_DomainError::Raise("BOP_FaceBuilder:InitEdge");
  }
  else {
    myBlockIterator = L->BlockIterator();
    myBlockIterator.Initialize();
    FindNextValidElement();
  }
  Standard_Integer n = myBlockIterator.Extent();
  return n;
}
//=======================================================================
//function : FindNextValidElement
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::FindNextValidElement()
{
  // prerequisites : myBlockIterator.Initialize
  Standard_Boolean found = Standard_False;

  while ( myBlockIterator.More()) {
    const Standard_Integer i = myBlockIterator.Value();
    found = myBlockBuilder.ElementIsValid(i);
    if (found) break;
    else myBlockIterator.Next();
  }
}
//=======================================================================
//function : MoreEdge
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_FaceBuilder::MoreEdge() const
{
  Standard_Boolean b = myBlockIterator.More();
  return b;
}
//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================
  void BOP_FaceBuilder::NextEdge()
{
  myBlockIterator.Next();
  FindNextValidElement();
}
//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
  const TopoDS_Edge& BOP_FaceBuilder::Edge() const
{
  if (!myBlockIterator.More()) {
    Standard_Failure::Raise("OutOfRange");
  }
  
  Standard_Integer i = myBlockIterator.Value();
  
  Standard_Boolean isvalid = myBlockBuilder.ElementIsValid(i);
 
  if (!isvalid) {
    Standard_Failure::Raise("Edge not Valid");
  }

  const TopoDS_Shape& E = myBlockBuilder.Element(i);
  const TopoDS_Edge& anEdge = TopoDS::Edge(E);
  
  return anEdge;
}

//=======================================================================
//function : WES
//purpose  : 
//=======================================================================
  const BOP_WireEdgeSet& BOP_FaceBuilder::WES() const
{
  return *myWES;
}

//=======================================================================
//function : NewFaces
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& BOP_FaceBuilder::NewFaces() const
{
  return myNewFaces;
}

//=======================================================================
// function: TreatSDScales Jan 15 12:25:34 2002
// purpose :  
//=======================================================================
  void BOP_FaceBuilder::SDScales()
{
  
  Standard_Integer iNegativeFlag, aNbFR, i, aNbEFOpen, iCnt;
  
  TopTools_ListOfShape aLFR;
  TopTools_ListIteratorOfListOfShape anIt, anItFR;
  TopTools_IndexedMapOfShape aMFR;
  //
  iCnt=myNewFaces.Extent();
  if (iCnt<2){
    return;
  }
  //
  // 1. Collect all faces with negative (infinite) area 
  anIt.Initialize(myNewFaces);
  for (i=1; anIt.More(); anIt.Next(), ++i) {
    const TopoDS_Face& aF=TopoDS::Face(anIt.Value());
    
    iNegativeFlag=myNegatives(i);
    if (iNegativeFlag) {
      aLFR.Append(aF);
    }
  }
  //
  aNbFR=aLFR.Extent();
  //
  if (!aNbFR) {
    return;
  }
  //
  //
  BOP_ListOfConnexityBlock aLCB;
  BOP_ListIteratorOfListOfConnexityBlock aLCBIt;
  //
  BOP_BuilderTools::MakeConnexityBlocks (myNewFaces, TopAbs_FACE, aLCB);
  //
  anItFR.Initialize(aLFR);
  for (; anItFR.More(); anItFR.Next()) {
    const TopoDS_Face& aFR=TopoDS::Face(anItFR.Value());
    //
    iCnt=1;
    TopTools_IndexedMapOfShape aMEFOpen;
    BOP_ConnexityBlock* pCBR=NULL;
    //
    TopExp::MapShapes(aFR, TopAbs_EDGE, aMEFOpen);
    aNbEFOpen=aMEFOpen.Extent();
    //
    // Look for ConnexityBlock to which aFR belongs to (pCBR)
    aLCBIt.Initialize(aLCB);
    for (; aLCBIt.More() && iCnt; aLCBIt.Next()) {
      const BOP_ConnexityBlock& aCB=aLCBIt.Value();
    
      const TopTools_ListOfShape& aLCF=aCB.Shapes();
      anIt.Initialize(aLCF);
      for (; anIt.More() && iCnt; anIt.Next()) {
	const TopoDS_Face& aF=TopoDS::Face(anIt.Value());
	TopTools_IndexedMapOfShape aMECB;
	TopExp::MapShapes(aF, TopAbs_EDGE, aMECB);
	
	for (i=1; i<=aNbEFOpen; ++i) {
	  const TopoDS_Shape& aEFOpen= aMEFOpen(i);
	  if (aMECB.Contains(aEFOpen)) {
	    iCnt=0;
	    pCBR=(BOP_ConnexityBlock*) &aCB;
	    break;
	  }
	}
      }
    }
    //
    if (iCnt) {
      // it is strange
      continue;
    }
    //
    // Collect Faces to remove in the map aMFR
    const TopTools_ListOfShape& aLCR=pCBR->Shapes();
    anIt.Initialize(aLCR);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Face& aF=TopoDS::Face(anIt.Value());
      aMFR.Add(aF);
    }
  } // for (; anItFR.More(); anItFR.Next())
  //
  //
  iCnt=aMFR.Extent();
  if (!iCnt) {
    // Nothing to remove
    return;
  }
  //
  TopTools_ListOfShape aLFOut;
  anIt.Initialize(myNewFaces);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Face& aF=TopoDS::Face(anIt.Value());
    if (!aMFR.Contains(aF)) {
      aLFOut.Append(aF);
    }
  }
  //
  myNewFaces.Clear();
  anIt.Initialize(aLFOut);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Face& aF=TopoDS::Face(anIt.Value());
    myNewFaces.Append(aF);
  }
 
}

void DoTopologicalVerification(TopoDS_Face& F)
{
  TopTools_IndexedDataMapOfShapeListOfShape mapVE;
  mapVE.Clear();
  TopExp::MapShapesAndAncestors(F,TopAbs_VERTEX,TopAbs_EDGE,mapVE);

  Standard_Real pF1 = 0., pL1 = 0., pF2 = 0., pL2 = 0.;
  Standard_Integer nbKeys = mapVE.Extent(), iKey = 0;

  for( iKey = 1; iKey <= nbKeys; iKey++ ) {
    const TopoDS_Vertex& iV = TopoDS::Vertex(mapVE.FindKey(iKey));
    if( iV.IsNull() ) continue;

    Standard_Real TolV = BRep_Tool::Tolerance(iV);

    const TopTools_ListOfShape& iLE = mapVE.FindFromIndex(iKey);
    Standard_Integer nbE = iLE.Extent();
    if( nbE != 2 ) break;

    const TopoDS_Edge& iE1 = TopoDS::Edge(iLE.First());
    const TopoDS_Edge& iE2 = TopoDS::Edge(iLE.Last());

    if(BRep_Tool::Degenerated(iE1) || BRep_Tool::Degenerated(iE2) ) continue;

    Standard_Real iPE1 = BRep_Tool::Parameter(iV,iE1);
    Standard_Real iPE2 = BRep_Tool::Parameter(iV,iE2);
	
    Handle(Geom_Curve) aC3D1 = BRep_Tool::Curve(iE1,pF1,pL1);
    Handle(Geom_Curve) aC3D2 = BRep_Tool::Curve(iE2,pF2,pL2);
    if( aC3D1.IsNull() || aC3D2.IsNull() ) break;

    Standard_Boolean is1F = (fabs(iPE1-pF1) < fabs(iPE1-pL1));
    Standard_Boolean is2F = (fabs(iPE2-pF2) < fabs(iPE2-pL2));
    
    Standard_Real useP1 = iPE1;
    if( is1F ) {
      if( fabs(iPE1-pF1) > 1.e-7 ) useP1 = pF1;
    }
    else {
      if( fabs(iPE1-pL1) > 1.e-7 ) useP1 = pL1;
    }

    Standard_Real useP2 = iPE2;
    if( is2F ) {
      if( fabs(iPE2-pF2) > 1.e-7 ) useP2 = pF2;
    }
    else {
      if( fabs(iPE2-pL2) > 1.e-7 ) useP2 = pL2;
    }

    gp_Pnt aPnt1 = aC3D1->Value(useP1);
    gp_Pnt aPnt2 = aC3D2->Value(useP2);
    gp_Pnt aPntV = BRep_Tool::Pnt(iV);

    Standard_Real distV1 = aPntV.Distance(aPnt1);
    Standard_Real distV2 = aPntV.Distance(aPnt2);

    // update vertex tolerance checking 3D curves
    if( distV1 > TolV || distV2 > TolV ) {
      Standard_Real distMax = Max(distV1,distV2);
      Standard_Real delta = fabs(distMax-TolV);
      Standard_Real newTol = TolV + delta + 2.e-7;
      TopoDS_Vertex & aV = (TopoDS_Vertex &) iV;
      BRep_Builder bb;
      bb.UpdateVertex(aV,newTol);
      TolV = newTol;
    }

    gp_Pnt2d aPnt2dF, aPnt2dL;
    BRep_Tool::UVPoints(iE1,F,aPnt2dF, aPnt2dL);
    gp_Pnt2d aPnt2dE1 = (is1F) ? aPnt2dF : aPnt2dL;
    BRep_Tool::UVPoints(iE2,F,aPnt2dF, aPnt2dL);
    gp_Pnt2d aPnt2dE2 = (is2F) ? aPnt2dF : aPnt2dL;

    BRepAdaptor_Surface aFSurf (F,Standard_False);
    aPnt1 = aFSurf.Value(aPnt2dE1.X(), aPnt2dE1.Y());
    aPnt2 = aFSurf.Value(aPnt2dE2.X(), aPnt2dE2.Y());
    distV1 = aPntV.Distance(aPnt1);
    distV2 = aPntV.Distance(aPnt2);

    // update vertex tolerance checking 3D points on surface
    if( distV1 > TolV || distV2 > TolV ) {
      Standard_Real distMax = Max(distV1,distV2);
      Standard_Real delta = fabs(distMax-TolV);
      Standard_Real newTol = TolV + delta + 2.e-7;
      TopoDS_Vertex & aV = (TopoDS_Vertex &) iV;
      BRep_Builder bb;
      bb.UpdateVertex(aV,newTol);
      TolV = newTol;
    }

//     Standard_Real dist2d = aPnt2dE1.Distance(aPnt2dE2);
//     // update veretex tolerance checking distance in 2D
//     if( dist2d > TolV ) {
//       Standard_Real delta2d = fabs(dist2d-TolV);
//       Standard_Real newTolV = TolV + delta2d + 2.e-7;
//       TopoDS_Vertex & aV = (TopoDS_Vertex &) iV;
//       BRep_Builder bb;
//       bb.UpdateVertex(aV,newTolV);
//     }
  }
}
