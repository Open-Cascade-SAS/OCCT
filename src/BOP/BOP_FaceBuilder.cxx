// Created by: Mister Open CAS.CADE
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


#include <BOP_FaceBuilder.ixx>

#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>

#include <Geom2dInt_Geom2dCurveTool.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <BRepLib.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve2d.hxx>

#include <TopAbs_Orientation.hxx>

#include <TopLoc_Location.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>

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
#include <BRepBuilderAPI_Copy.hxx>


static
  Standard_Boolean IsGrowthWire(const TopoDS_Shape& theWire,
				const TopTools_IndexedMapOfShape& theMHE);
static
  Standard_Boolean IsHole(const TopoDS_Shape& aW,
			  const TopoDS_Shape& aFace);
static
  Standard_Boolean IsInside(const TopoDS_Shape& theHole,
			    const TopoDS_Shape& theF2,
			    const Handle(IntTools_Context)& theContext);
static 
  void DoTopologicalVerification(TopoDS_Face& F);

//=======================================================================
//function : BOP_FaceBuilder
//purpose  : 
//=======================================================================
BOP_FaceBuilder::BOP_FaceBuilder():
  myTreatment(0),
//  myManifoldFlag(Standard_True),
  myTreatSDScales(0)
{
}
//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================
void BOP_FaceBuilder::SetContext(const Handle(IntTools_Context)& aCtx)
{
  myContext=aCtx;
}
//=======================================================================
//function : Context
//purpose  : 
//=======================================================================
const Handle(IntTools_Context)& BOP_FaceBuilder::Context()const
{
  return myContext;
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
//function : Do
//purpose  : 
//=======================================================================
void BOP_FaceBuilder::Do(const BOP_WireEdgeSet& aWES,
			 const Standard_Boolean bForceClass) 
{
  myFace=aWES.Face();
  myWES=(BOP_WireEdgeSet*) &aWES;
  //
  if (myContext.IsNull()) {
    myContext=new IntTools_Context;
  }
  //
  BOP_WESCorrector aWESCor;
  aWESCor.SetWES(aWES);
  aWESCor.Do();
  BOP_WireEdgeSet& aNewWES=aWESCor.NewWES();
  // 
  PerformAreas(aNewWES);
  //
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
	bIsPointInOnFace=myContext->IsPointInOnFace(aF, aP2D);
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
// function: TreatSDScales 
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
//=======================================================================
//function : PerformAreas
//purpose  : 
//=======================================================================
void BOP_FaceBuilder::PerformAreas(BOP_WireEdgeSet& aWES)
{
  Standard_Boolean bIsGrowth, bIsHole;
  Standard_Real aTol;
  TopTools_ListOfShape aNewFaces, aHoleWires, aLoops; 
  TopoDS_Shape anInfinitePointShape;
  TopTools_DataMapOfShapeShape aInOutMap;
  TopTools_DataMapOfShapeListOfShape aMSH;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItMSH;
  TopTools_ListIteratorOfListOfShape aIt1, aIt2;
  TopTools_IndexedMapOfShape aMHE;
  BRep_Builder aBB;
  Handle(Geom_Surface) aS;
  TopLoc_Location aLoc;
  //
  aTol=BRep_Tool::Tolerance(myFace);
  aS=BRep_Tool::Surface(myFace, aLoc);
  //
  myNewFaces.Clear();
  //
  for(aWES.InitShapes();  aWES.MoreShapes(); aWES.NextShape()) {
    const TopoDS_Shape& aW=aWES.Shape();
    aLoops.Append(aW);
  }
  //
  //  Draft faces [aNewFaces]
  aIt1.Initialize(aLoops);
  for ( ; aIt1.More(); aIt1.Next()) {
    const TopoDS_Shape& aWire=aIt1.Value();
    //
    bIsGrowth=IsGrowthWire(aWire, aMHE);
    if (bIsGrowth) {
      // make a growth face from a wire
      TopoDS_Face aFace;
      aBB.MakeFace(aFace, aS, aLoc, aTol);
      aBB.Add (aFace, aWire);
      //
      aNewFaces.Append (aFace);
    }
    else{
      // check if a wire is a hole 
      bIsHole=IsHole(aWire, myFace);
      //XX
      if (bIsHole) {
        aHoleWires.Append(aWire);
        TopExp::MapShapes(aWire, TopAbs_EDGE, aMHE);
      }
      else {
        // make a growth face from a wire
        TopoDS_Face aFace;
        aBB.MakeFace(aFace, aS, aLoc, aTol);
        aBB.Add (aFace, aWire);
        //
        aNewFaces.Append (aFace);
      }
    }
  }
  //
  // 2. Find outer growth shell that is most close to each hole shell
  aIt2.Initialize(aHoleWires);
  for (; aIt2.More(); aIt2.Next()) {
    const TopoDS_Shape& aHole = aIt2.Value();
    //
    aIt1.Initialize(aNewFaces);
    for ( ; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aF=aIt1.Value();
      //
      if (!IsInside(aHole, aF, myContext)){
        continue;
      }
      //
      if ( aInOutMap.IsBound (aHole)){
        const TopoDS_Shape& aF2=aInOutMap(aHole);
        if (IsInside(aF, aF2, myContext)) {
          aInOutMap.UnBind(aHole);
          aInOutMap.Bind (aHole, aF);
        }
      }
      else{
        aInOutMap.Bind (aHole, aF);
      }
    }
    //
    // Add aHole to a map Face/ListOfHoles [aMSH]
    if (aInOutMap.IsBound(aHole)){
      const TopoDS_Shape& aF=aInOutMap(aHole);
      if (aMSH.IsBound(aF)) {
        TopTools_ListOfShape& aLH=aMSH.ChangeFind(aF);
        aLH.Append(aHole);
      }
      else {
        TopTools_ListOfShape aLH;
        aLH.Append(aHole);
        aMSH.Bind(aF, aLH);
      }
    }
  }// for (; aIt2.More(); aIt2.Next())
  //
  // 3. Add aHoles to Faces
  aItMSH.Initialize(aMSH);
  for (; aItMSH.More(); aItMSH.Next()) {
    TopoDS_Face aF=TopoDS::Face(aItMSH.Key());
    //
    const TopTools_ListOfShape& aLH=aItMSH.Value();
    aIt2.Initialize(aLH);
    for (; aIt2.More(); aIt2.Next()) {
      const TopoDS_Shape& aHole = aIt2.Value();
      aBB.Add (aF, aHole);
    }
    //
    // update classifier 
    aTol=BRep_Tool::Tolerance(aF);
    IntTools_FClass2d& aClsf=myContext->FClass2d(aF);
    aClsf.Init(aF, aTol);
  }
  //
  // These aNewFaces are draft faces that 
  // do not contain any internal shapes
  //
  Standard_Boolean bIsValidIn2D, bNegativeFlag;
  Standard_Integer iNegativeFlag;
  //
  myNewFaces.Clear();
  myNegatives.Clear();
  //
  aIt1.Initialize(aNewFaces);
  for ( ; aIt1.More(); aIt1.Next()) {
    const TopoDS_Face& aFx=TopoDS::Face(aIt1.Value());
    bIsValidIn2D=BOPTools_Tools3D::IsValidArea (aFx, bNegativeFlag);
    if(bIsValidIn2D) {
      myNewFaces.Append (aFx);
      iNegativeFlag=(Standard_Integer)bNegativeFlag;
      myNegatives.Append(iNegativeFlag);
    }
  }
  //
}
//=======================================================================
//function : IsGrowthWire
//purpose  : 
//=======================================================================
Standard_Boolean IsGrowthWire(const TopoDS_Shape& theWire,
                              const TopTools_IndexedMapOfShape& theMHE)
{
  Standard_Boolean bRet;
  TopoDS_Iterator aIt;
  // 
  bRet=Standard_False;
  if (theMHE.Extent()) {
    aIt.Initialize(theWire);
    for(; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE=aIt.Value();
      if (theMHE.Contains(aE)) {
        return !bRet;
      }
    }
  }
  return bRet;
}
//=======================================================================
//function : IsHole
//purpose  : 
//=======================================================================
Standard_Boolean IsHole(const TopoDS_Shape& aW,
			const TopoDS_Shape& aFace)
{
  Standard_Boolean bIsHole;
  Standard_Real aTolF;
  TopoDS_Face aFF, aFC;
  BRep_Builder aBB;
  IntTools_FClass2d aFClass2d;
  //
  aFF=TopoDS::Face(aFace.EmptyCopied());
  aFF.Orientation(TopAbs_FORWARD);
  aBB.Add(aFF, aW);
  //
  BRepBuilderAPI_Copy aBC;
  //
  aBC.Perform(aFF);
  aFC=TopoDS::Face(aBC.Shape());
  aFF=aFC;
  //
  aTolF=BRep_Tool::Tolerance(aFF);
  //modified by NIZNHY-PKV Thu Aug 23 09:18:05 2012f
  BRepLib::SameParameter(aFF, aTolF, Standard_True);
  //modified by NIZNHY-PKV Thu Aug 23 09:18:08 2012t
  //
  aFClass2d.Init(aFF, aTolF);
  //
  bIsHole=aFClass2d.IsHole();
  //
  return bIsHole;
}
//=======================================================================
//function : IsInside
//purpose  : 
//=======================================================================
Standard_Boolean IsInside(const TopoDS_Shape& theHole,
                          const TopoDS_Shape& theF2,
                          const Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bRet;
  Standard_Real aT, aU, aV;
  
  TopAbs_State aState;
  TopExp_Explorer aExp;
  TopTools_IndexedMapOfShape aME2;
  gp_Pnt2d aP2D;
  //
  bRet=Standard_False;
  aState=TopAbs_UNKNOWN;
  const TopoDS_Face& aF2=TopoDS::Face(theF2);
  //
  TopExp::MapShapes(aF2, TopAbs_EDGE, aME2);
  //
  aExp.Init(theHole, TopAbs_EDGE);
  if (aExp.More()) {
    const TopoDS_Edge& aE = TopoDS::Edge(aExp.Current());
    if (aME2.Contains(aE)) {
      return bRet;
    }
    //
    aT=BOPTools_Tools2D::IntermediatePoint(aE);
    BOPTools_Tools2D::PointOnSurface(aE, aF2, aT, aU, aV);
    aP2D.SetCoord(aU, aV);
    //
    IntTools_FClass2d& aClsf=theContext->FClass2d(aF2);
    aState=aClsf.Perform(aP2D);
    bRet=(aState==TopAbs_IN);
  }
  //
  return bRet;
}
//=======================================================================
//function : DoTopologicalVerification
//purpose  : 
//=======================================================================
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
  }
}

