// Created on: 1999-09-29
// Created by: Maxim ZVEREV
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_Builder1.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_Tools.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepDS_ShapeWithState.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

//define parameter division number as 10*e^(-PI) = 0.43213918
const Standard_Real PAR_T = 0.43213918;

static TopTools_IndexedMapOfShape mySDEdgeMap;

static TopAbs_State ClassifyEdgeToFaceByOnePoint(const TopoDS_Edge& E,
						 const TopoDS_Face& F);

//modified by NIZHNY-MZV  Thu Apr 20 09:58:59 2000
/////////////////// 
//this variable used to separate old algo from the new one
//because new algo can not be used in LocOpe and Mechanical Features (for the moment)
//that's why we use new algo only in BRepAlgoAPI_BooleanOperation
//in all other cases old algo is called (see the methods GFillSolidSFS, GFillShellSFS, etc.);
Standard_Boolean GLOBAL_USE_NEW_BUILDER = Standard_False;

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
TopOpeBRepBuild_Builder1::TopOpeBRepBuild_Builder1(const TopOpeBRepDS_BuildTool& BT)
     : TopOpeBRepBuild_Builder(BT)
{
  mySameDomMap.Clear();
  myMapOfEdgeFaces.Clear();
  mySplitsONtoKeep.Clear();
  myProcessedPartsOut2d.Clear();
  myProcessedPartsON2d.Clear();
}

//modified by NIZNHY-PKV Mon Dec 16 11:37:59 2002 f
/*
//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::Destroy()
{
}
*/
//modified by NIZNHY-PKV Mon Dec 16 11:38:05 2002 t
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::Clear()
{
  TopOpeBRepBuild_Builder::Clear();

//  mySameDomMap.Clear();
//  myMapOfEdgeFaces.Clear();
//  mySplitsONtoKeep.Clear();
//  myProcessedPartsOut2d.Clear();
//  myProcessedPartsON2d.Clear();
//  myDataStructure -> ChangeDS().ChangeMapOfShapeWithStateObj().Clear();
//  myDataStructure -> ChangeDS().ChangeMapOfShapeWithStateTool().Clear();
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::Perform(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  TopOpeBRepBuild_Builder::Perform(HDS);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::Perform(const Handle(TopOpeBRepDS_HDataStructure)& HDS,
				       const TopoDS_Shape& S1, 
				       const TopoDS_Shape& S2)
{
  //modified by NIZHNY-MZV  Wed Apr 19 17:23:12 2000
  //see the comments at the top of file about this global variable
  if(!GLOBAL_USE_NEW_BUILDER) {
    TopOpeBRepBuild_Builder::Perform(HDS, S1, S2);
    return;
  }


  mySameDomMap.Clear();
  myMapOfEdgeFaces.Clear();
  mySplitsONtoKeep.Clear();
  myProcessedPartsOut2d.Clear();
  myProcessedPartsON2d.Clear();

  myShape1 = S1; myShape2 = S2;
  Perform(HDS);

  myIsKPart = FindIsKPart();
  if((myIsKPart == 1) || (myIsKPart == 5))
    myIsKPart=4;

  if (myIsKPart==4) {
    // For the moment States will be calculated in case SOLID/SOLID only
    PerformShapeWithStates();
  }
}

//=======================================================================
//function : MergeKPart
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::MergeKPart(const TopAbs_State TB1,
					 const TopAbs_State TB2)
{
  TopOpeBRepBuild_Builder::MergeKPart(TB1, TB2);
}

//=======================================================================
//function : MergeKPart
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::MergeKPart()
{
  if ( myIsKPart == 1 ) { // iskole
    MergeKPartiskole();
  }
  else if ( myIsKPart == 5 ) { // iskoletge
    MergeKPartiskoletge();
  }
  else if (myIsKPart == 2) { // isdisj
    MergeKPartisdisj();
  }
  else if ( myIsKPart == 3 ) { // isfafa
    MergeKPartisfafa();
  }
  else if ( myIsKPart == 4 ) { // issoso
    MergeKPartissoso();
   
    TopTools_ListIteratorOfListOfShape its(Merged(myShape1,myState1));
    for (; its.More(); its.Next()) {
      CorrectResult2d(its.Value());
    }
  }
    
  End(); 
    
}

//=======================================================================
//function : GFillSolidSFS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GFillSolidSFS(const TopoDS_Shape& SO1,
					     const TopTools_ListOfShape& LSO2,
					     const TopOpeBRepBuild_GTopo& G1,
					     TopOpeBRepBuild_ShellFaceSet& SFS)
{
  //modified by NIZHNY-MZV  Wed Apr 19 17:23:12 2000
  //see the comments at the top of file about this global variable
  if(!GLOBAL_USE_NEW_BUILDER) {
    TopOpeBRepBuild_Builder::GFillSolidSFS(SO1, LSO2, G1, SFS);
    return;
  }

  myMapOfEdgeFaces.Clear();

  TopExp::MapShapesAndAncestors(myShape1, TopAbs_EDGE, TopAbs_FACE, myMapOfEdgeFaces);
  TopExp::MapShapesAndAncestors(myShape2, TopAbs_EDGE, TopAbs_FACE, myMapOfEdgeFaces);

  TopAbs_State TB1,TB2; 
  G1.StatesON(TB1,TB2);
  
//  printf("TB1  =%d, TB2 = %d\n", TB1, TB2);

  Standard_Boolean RevOri1 = G1.IsToReverse1();
  
  TopoDS_Shape SOF = SO1; 
  mySolidToFill = TopoDS::Solid(SOF);
  
  TopOpeBRepTool_ShapeExplorer exShell(SOF,TopAbs_SHELL);
  for (; exShell.More(); exShell.Next()) {
    TopoDS_Shape SH = exShell.Current();
    Standard_Boolean hasshape = myDataStructure->HasShape(SH);
    
    if ( ! hasshape ) {
      // shell SH is not in DS : Get its state (to the LS02) from map and define to keep or not
      TopAbs_State shSt = myDataStructure -> DS().GetShapeWithState(SH).State();
      Standard_Boolean keep = (shSt == TB1) ? Standard_True : Standard_False;
      if (keep) {
	TopAbs_Orientation oriSH = SH.Orientation();
	TopAbs_Orientation neworiSH = Orient(oriSH,RevOri1);
	SH.Orientation(neworiSH);

	SFS.AddShape(SH);
      }
    }
    else { // shell SH has faces(s) with geometry : split SH faces
      GFillShellSFS(SH,LSO2,G1,SFS);
    }
  }
}

//=======================================================================
//function : GFillShellSFS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GFillShellSFS (const TopoDS_Shape& SH,
					      const TopTools_ListOfShape& LSO2,
					      const TopOpeBRepBuild_GTopo& G1,
					      TopOpeBRepBuild_ShellFaceSet& SFS)
{  
  //modified by NIZHNY-MZV  Wed Apr 19 17:23:12 2000
  //see the comments at the top of file about this global variable
  if(!GLOBAL_USE_NEW_BUILDER) {
    TopOpeBRepBuild_Builder::GFillShellSFS(SH, LSO2, G1, SFS);
    return;
  }

  TopAbs_State TB1,TB2; 
  G1.StatesON(TB1,TB2);
  Standard_Boolean RevOri1 = G1.IsToReverse1();
  
  TopOpeBRepTool_ShapeExplorer exFace;

  TopoDS_Shape SH1 = SH;// SH1.Orientation(TopAbs_FORWARD);
  
  //1) process firstly same domain faces and non-interference faces
  for (exFace.Init(SH1,TopAbs_FACE); exFace.More(); exFace.Next()) {
    TopoDS_Shape FOR = exFace.Current();
    if(!myDataStructure -> HasShape(FOR)) {
      //DS doesn't contain FACE , get its state and define to keep or not
      TopAbs_State shSt = myDataStructure -> DS().GetShapeWithState(FOR).State();
      Standard_Boolean keep = (shSt == TB1) ? Standard_True : Standard_False;
      if (keep) {
	TopAbs_Orientation oriF = FOR.Orientation();
	TopAbs_Orientation neworiF = Orient(oriF,RevOri1);
	FOR.Orientation(neworiF);
	SFS.AddElement(FOR);
      }
      continue;
    }
    Standard_Boolean hsd = myDataStructure->HasSameDomain(FOR);
    if ( hsd && !mySameDomMap.Contains(FOR)) 
      GFillFaceSameDomSFS(FOR,LSO2,G1,SFS);
  }

  //2 Process all other faces
  for (exFace.Init(SH1,TopAbs_FACE); exFace.More(); exFace.Next()) {
    TopoDS_Shape FOR = exFace.Current();
    if(!myDataStructure -> HasShape(FOR)
       ||
       myDataStructure->HasSameDomain(FOR))
      continue;
    GFillFaceNotSameDomSFS(FOR, LSO2, G1, SFS);
  }
} // GFillShellSFS

//=======================================================================
//function : GFillFaceNotSameDomSFS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GFillFaceNotSameDomSFS(const TopoDS_Shape& FOR,
						      const TopTools_ListOfShape& LSO2,
						      const TopOpeBRepBuild_GTopo& Gin,
						      TopOpeBRepBuild_ShellFaceSet& SFS)
{
  TopOpeBRepBuild_GTopo G1 = Gin;
  Standard_Boolean RevOri = Standard_False; 
  G1.SetReverse(RevOri);

  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  
  // work on a FORWARD face <FForward>
  TopoDS_Shape FF = FOR; FF.Orientation(TopAbs_FORWARD);
  
  // make a WireEdgeSet WES on face FF
  TopOpeBRepBuild_WireEdgeSet WES(FF,this);

  // Add ON parts (edges ON solid)
  GFillONPartsWES(FOR,G1,LSO2,WES);

  // save these edges
  TopTools_ListOfShape anEdgesON;
  TopTools_ListIteratorOfListOfShape it;
  if (myProcessON) {
    Standard_Boolean toRevOri = Opefus();
    for (it.Initialize(WES.StartElements()); it.More(); it.Next())
      anEdgesON.Append(toRevOri ? it.Value().Reversed() : it.Value());
    myONElemMap.Clear();
  }

  // split the edges of FF : add split edges to WES
  GFillFaceNotSameDomWES(FF,LSO2,G1,WES);

  // add edges built on curves supported by FF
  GFillCurveTopologyWES(FF,G1,WES);

  myEdgeAvoid.Clear();

  // mark FF as split TB1
  MarkSplit(FF,TB1);
  
  // build the new faces LOF on FF from the Wire/Edge set WES
  TopTools_ListOfShape LOF;
  GWESMakeFaces(FF,WES,LOF);

  if (myProcessON && (!anEdgesON.IsEmpty() || !myONElemMap.IsEmpty())) {
    // try to make patches with only ON parts.
    // prepare the map of used edges to not take the same matter two times
    TopTools_IndexedMapOfOrientedShape aMapOE;
    for (it.Initialize(LOF); it.More(); it.Next())
      for (TopExp_Explorer ex(it.Value(),TopAbs_EDGE); ex.More(); ex.Next())
        aMapOE.Add(ex.Current());

    FillOnPatches(anEdgesON,FOR,aMapOE);
    myONElemMap.Clear();
  }

  // LOFS : LOF faces located TB1 / LSclass = split faces of state TB1 of FF
  TopTools_ListOfShape& LOFS = ChangeSplit(FF,TB1);
  LOFS.Clear();
  GKeepShapes(FF,myEmptyShapeList,TB1,LOF,LOFS);

  GSplitFaceSFS(FOR, LSO2, Gin, SFS); 
} // GFillFaceSFS

//=======================================================================
//function : GFillFaceNotSameDomWES
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GFillFaceNotSameDomWES(const TopoDS_Shape& FOR1,
						      const TopTools_ListOfShape& LFclass,
						      const TopOpeBRepBuild_GTopo& G1,
						      TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  Standard_Boolean RevOri1 = G1.IsToReverse1();

  mySourceShapes.Clear();
  
  // work on a FORWARD face FF
  TopoDS_Shape FF = FOR1; FF.Orientation(TopAbs_FORWARD);
  
  TopOpeBRepTool_ShapeExplorer exWire(FF,TopAbs_WIRE);
  for (; exWire.More(); exWire.Next()) {
    TopoDS_Shape W = exWire.Current();
    Standard_Boolean hasshape = myDataStructure->HasShape(W);
    
    if ( ! hasshape ) {
      // wire W is not in DS : get its state and define to keep or not
      TopAbs_State shSt = myDataStructure -> DS().GetShapeWithState(W).State();
      Standard_Boolean keep = (shSt == TB1) ? Standard_True : Standard_False;
      if (keep || (myProcessON && shSt == TopAbs_ON)) {
	TopAbs_Orientation oriW = W.Orientation();
	TopAbs_Orientation neworiW = Orient(oriW,RevOri1);
	W.Orientation(neworiW);
	if (keep) WES.AddShape(W);
        else myONElemMap.Add(W);
	mySourceShapes.Add(W);
      }
    }
    else { // wire W has edges(s) with geometry : split W edges
      GFillWireNotSameDomWES(W,LFclass,G1,WES);
    }
  }
  return;
} // GFillFaceWES

//=======================================================================
//function : GFillWireNotSameDomWES
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GFillWireNotSameDomWES(const TopoDS_Shape& W,
						      const TopTools_ListOfShape& LSclass,
						      const TopOpeBRepBuild_GTopo& G1,
						      TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  Standard_Boolean RevOri1 = G1.IsToReverse1();

  TopoDS_Shape WW = W; //WW.Orientation(TopAbs_FORWARD);

  TopOpeBRepTool_ShapeExplorer exEdge(WW,TopAbs_EDGE);
  for (; exEdge.More(); exEdge.Next()) {
    TopoDS_Shape EOR = exEdge.Current();
    Standard_Boolean hasshape = myDataStructure->HasShape(EOR);
    
    if ( ! hasshape ) {
      // edge EOR is not in DS : get its state and define to keep or not
      TopAbs_State shSt = myDataStructure -> DS().GetShapeWithState(EOR).State();
      Standard_Boolean keep = (shSt == TB1) ? Standard_True : Standard_False;
      if (keep || (myProcessON && shSt == TopAbs_ON)) {
        TopAbs_Orientation oriE = EOR.Orientation();
	TopAbs_Orientation neworiE = Orient(oriE,RevOri1);
	EOR.Orientation(neworiE);
	if (keep) WES.AddElement(EOR);
        else myONElemMap.Add(EOR);
	mySourceShapes.Add(EOR);
      }
    }
    else { // wire W has edges(s) with geometry : split W edges
      GFillEdgeNotSameDomWES(EOR,LSclass,G1,WES);
    }
  }
} // GFillWireWES


//=======================================================================
//function : GFillEdgeNotSameDomWES
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GFillEdgeNotSameDomWES(const TopoDS_Shape& EOR,
						      const TopTools_ListOfShape& /*LSclass*/,
						      const TopOpeBRepBuild_GTopo& G1,
						      TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  Standard_Boolean RevOri1 = G1.IsToReverse1();



  TopAbs_Orientation oriE = EOR.Orientation();
  TopAbs_Orientation neworiE = Orient(oriE,RevOri1);
  //1) Get split parts of edge with state TB1
  const TopTools_ListOfShape& LSE = myDataStructure -> DS().GetShapeWithState(EOR).Part(TB1);
  TopTools_ListIteratorOfListOfShape  it (LSE);

  for(; it.More(); it.Next()) {
    TopoDS_Edge newE = TopoDS::Edge(it.Value()); 
    newE.Orientation(neworiE);
    WES.AddStartElement(newE);
    mySourceShapes.Add(newE);
  }
  
  //2) Get ON parts of the edge and define to keep it or not
  const TopTools_ListOfShape& LSEOn = myDataStructure -> DS().GetShapeWithState(EOR).Part(TopAbs_ON);
  TopTools_ListIteratorOfListOfShape  itON (LSEOn);
  for(; itON.More(); itON.Next()) {
    TopoDS_Edge newE = TopoDS::Edge(itON.Value()); 
    newE.Orientation(neworiE);
    if(mySplitsONtoKeep.Contains(newE)) {
      WES.AddStartElement(newE);      
      continue;
    }
    // we keep all degenerated edges here because FillONPartsWES can not process them
    if(BRep_Tool::Degenerated(newE)) {
      WES.AddStartElement(newE);
      mySourceShapes.Add(newE);
    }
    if (myProcessON) {
      myONElemMap.Add(newE);
      mySourceShapes.Add(newE);
    }
  }
} // GFillEdgeWES


/////////////////// ALL FUNCTIONS FOR SAME DOMAIN FACES
//=======================================================================
//function : GFillFaceSameDomSFS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GFillFaceSameDomSFS(const TopoDS_Shape& FOR,
						   const TopTools_ListOfShape& LSO2,
						   const TopOpeBRepBuild_GTopo& Gin,
						   TopOpeBRepBuild_ShellFaceSet& SFS)
{
  myProcessedPartsOut2d.Clear();
  myProcessedPartsON2d.Clear();
  myMapOfEdgeWithFaceState.Clear();
  mySDEdgeMap.Clear();
  mySourceShapes.Clear();

  //we process all same domain faces during cycling throught the Shape1
  if(myDataStructure -> DS().AncestorRank(FOR) != 1)
    return;

  TopOpeBRepBuild_GTopo G1 = Gin;

  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);

  // work on a FORWARD face <FForward>
  TopoDS_Shape FF = FOR; FF.Orientation(TopAbs_FORWARD);
  
  // make a WireEdgeSet WES on face FF
  TopOpeBRepBuild_WireEdgeSet WES(FF,this);
  
  // split the edges of FF : add split edges to WES
  GFillFaceSameDomWES(FOR,LSO2,G1,WES);

  myEdgeAvoid.Clear();

  // mark FF as split TB1
  MarkSplit(FF,TB1);
  
  // build the new faces LOF on FF from the Wire/Edge set WES
  TopTools_ListOfShape LOF, oriLOF;
  GWESMakeFaces(FF,WES,LOF);
  
  // LOFS : LOF faces located TB1 / LSclass = split faces of state TB1 of FF
  TopTools_ListOfShape& LOFS = ChangeSplit(FF,TB1);

  //orientate new faces by the right way
  Standard_Boolean OrigRev = (FOR.Orientation() == TopAbs_FORWARD ? Standard_False : Standard_True);
  TopTools_ListIteratorOfListOfShape LOFit(LOF);
  for(; LOFit.More(); LOFit.Next()) {
    TopoDS_Shape aFace = LOFit.Value();
    TopTools_IndexedMapOfShape aEM;
    TopExp::MapShapes(aFace, TopAbs_EDGE, aEM);
    Standard_Boolean rev = Standard_False;
    for(Standard_Integer i = 1; i <= aEM.Extent(); i++) {
      const TopoDS_Shape& anEdge = aEM(i);
      if (myMapOfEdgeWithFaceState.Find (anEdge, rev)) {
        break;
      }
    }
    if(OrigRev)
      aFace.Reverse();

    if(rev)
      aFace.Reverse();

    oriLOF.Append(aFace);
    SFS.AddStartElement(aFace);
  }

  LOFS.Clear();
  GKeepShapes(FF,myEmptyShapeList,TB1,oriLOF,LOFS);  
} // GFillFaceSFS

//=======================================================================
//function : GFillFaceSameDomWES
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GFillFaceSameDomWES(const TopoDS_Shape& FOR1,
						   const TopTools_ListOfShape& /*LFclass*/,
						   const TopOpeBRepBuild_GTopo& G1,
						   TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  
  myBaseFaceToFill = TopoDS::Face(FOR1);

  TopTools_IndexedMapOfShape curSameDomMap;
  curSameDomMap.Add(FOR1);

  Standard_Integer i, nF;
  for(i = 1; i <= curSameDomMap.Extent(); i++) {
    TopTools_ListIteratorOfListOfShape it = myDataStructure -> SameDomain(curSameDomMap(i));
    for(; it.More(); it.Next()) {
      const TopoDS_Shape& SDF = it.Value();
      curSameDomMap.Add(SDF);
      mySameDomMap.Add(SDF);
      
      TopExp::MapShapes(SDF, TopAbs_EDGE, mySDEdgeMap);
    }
  }
  
  nF = curSameDomMap.Extent();
  for(i = 1; i<= nF; i++) {
    TopoDS_Shape curF = curSameDomMap(i);
    
    TopoDS_Shape curFF = curF;
    curFF.Orientation(TopAbs_FORWARD);

    mySDFaceToFill = TopoDS::Face(curF);
    Standard_Integer iref = myDataStructure -> DS().AncestorRank(curFF);
    
    TopAbs_State TB;
    Standard_Boolean RevOri = Standard_False;
    TopOpeBRepBuild_GTopo GFTW = G1;
    if(iref == 1) {//object
      TB = TB1;
      RevOri = G1.IsToReverse1();      
    }
    else {//tool
      RevOri = G1.IsToReverse2();
      TB = TB2;
      if(RevOri) 
	GFTW = G1.CopyPermuted();
    }
    //we need to pass GTopo according to ancestor rank
    GFillCurveTopologyWES(curFF,GFTW,WES);

    //process ON parts from not SD faces
    PerformONParts(curFF, curSameDomMap, G1, WES);
    
    const TopTools_ListOfShape& LSF = myDataStructure -> DS().ShapeSameDomain(curFF);
    
    TopOpeBRepTool_ShapeExplorer exWire(curFF,TopAbs_WIRE);
    for (; exWire.More(); exWire.Next()) {
      TopoDS_Shape W = exWire.Current();
      Standard_Boolean hasshape = myDataStructure->HasShape(W);
      TopAbs_State shSt = myDataStructure -> DS().GetShapeWithState(W).State();
      
      if ( ! hasshape && (shSt != TopAbs_ON)) {
	// wire W is not in DS : get its state and define to keep or not
	Standard_Boolean keep = (shSt == TB) ? Standard_True : Standard_False;
	if (keep) {
	  TopAbs_Orientation oriW = W.Orientation();
	  TopAbs_Orientation neworiW = Orient(oriW,RevOri);

	  if(myBaseFaceToFill != mySDFaceToFill)
	    TopOpeBRepBuild_Tools::UpdatePCurves(TopoDS::Wire(W), 
						 TopoDS::Face(mySDFaceToFill), 
						 TopoDS::Face(myBaseFaceToFill));
	  else {
	    mySourceShapes.Add(W);
	  }

	  TopExp_Explorer we(W, TopAbs_EDGE);
	  Standard_Boolean stateOfFaceOri = Standard_False;
	  Standard_Boolean UseEdges = Standard_False;
	  for(; we.More(); we.Next()) {
	    TopoDS_Edge EOR = TopoDS::Edge(we.Current());

	    TopAbs_Orientation oldori = EOR.Orientation();
	    OrientateEdgeOnFace(EOR,TopoDS::Face(myBaseFaceToFill),
				TopoDS::Face(mySDFaceToFill), G1, stateOfFaceOri);

	    //	    OrientateEdgeOnFace(TopoDS::Edge(EOR), TopoDS::Face(myBaseFaceToFill), 
	    //			TopoDS::Face(mySDFaceToFill), G1, stateOfFaceOri);
	    if(EOR.Orientation() != oldori) {
	      UseEdges = Standard_True;
	      WES.AddStartElement(EOR);
	    }
	    
	    myMapOfEdgeWithFaceState.Bind (EOR, stateOfFaceOri);
	  }

	  if(!UseEdges) {
	    W.Orientation(neworiW);
	    WES.AddShape(W);
	  }
	}
      }
      else { // wire W has edges(s) with geometry : split W edges
	GFillWireSameDomWES(W, LSF,G1,WES);
      }
    }
  }
  return;
} // GFillFaceWES

//=======================================================================
//function : GFillWireSameDomWES
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GFillWireSameDomWES(const TopoDS_Shape& W,
						   const TopTools_ListOfShape& LSclass,
						   const TopOpeBRepBuild_GTopo& G1,
						   TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);

  TopoDS_Shape WW = W; //WW.Orientation(TopAbs_FORWARD);

  Standard_Integer iref = myDataStructure -> DS().AncestorRank(W);

  Standard_Boolean RevOri;
  TopAbs_State TB;
  if(iref == 1) {//object
    TB = TB1;
    RevOri = G1.IsToReverse1();
  }
  else {//tool
    RevOri = G1.IsToReverse2();
    TB = TB2;
  }


  TopOpeBRepTool_ShapeExplorer exEdge(WW,TopAbs_EDGE);
  for (; exEdge.More(); exEdge.Next()) {
    TopoDS_Shape EOR = exEdge.Current();
    Standard_Boolean hasshape = myDataStructure->HasShape(EOR);
    if ( ! hasshape ) {
      // edge EOR is not in DS : get its state and define to keep or not
      TopAbs_State shSt = myDataStructure -> DS().GetShapeWithState(EOR).State();
      Standard_Boolean keep = (shSt == TB) ? Standard_True : Standard_False;      
      if (keep) {
        TopAbs_Orientation oriE = EOR.Orientation();
	Orient(oriE,RevOri);  
	
	if(mySDFaceToFill != myBaseFaceToFill) {
	  TopOpeBRepBuild_Tools::UpdateEdgeOnFace(TopoDS::Edge(EOR), 
						  TopoDS::Face(mySDFaceToFill), 
						  TopoDS::Face(myBaseFaceToFill));  
	}
	else {
	  mySourceShapes.Add(EOR);
	}

	Standard_Boolean stateOfFaceOri = Standard_False;

	OrientateEdgeOnFace(TopoDS::Edge(EOR), TopoDS::Face(myBaseFaceToFill), 
			    TopoDS::Face(mySDFaceToFill), G1, stateOfFaceOri);
	myMapOfEdgeWithFaceState.Bind (EOR, stateOfFaceOri);
	WES.AddElement(EOR);
      }
    }
    else { // wire W has edges(s) with geometry : split W edges
      GFillEdgeSameDomWES(EOR,LSclass,G1,WES);
    }
  }
} // GFillWireWES

//=======================================================================
//function : GFillEdgeSameDomWES
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GFillEdgeSameDomWES(const TopoDS_Shape& EOR,
						   const TopTools_ListOfShape& LSclass,
						   const TopOpeBRepBuild_GTopo& G1,
						   TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2, TB; G1.StatesON(TB1,TB2);

  Standard_Integer iref = myDataStructure -> DS().AncestorRank(EOR);

  Standard_Boolean RevOri;
  if(iref == 1) {//object
    TB = TB1;
    RevOri = G1.IsToReverse1();
  }
  else {//tool
    RevOri = G1.IsToReverse2();
    TB = TB2;
  }

  TopAbs_Orientation oriE = EOR.Orientation();
  Orient(oriE,RevOri);

  //1) Get split parts of edge with state TB
  const TopTools_ListOfShape& LSE = myDataStructure -> DS().GetShapeWithState(EOR).Part(TB);
  TopTools_ListIteratorOfListOfShape  it (LSE);
  for(; it.More(); it.Next()) {

    TopoDS_Edge newE = TopoDS::Edge(it.Value());
    newE.Orientation(oriE);

    if(mySDFaceToFill != myBaseFaceToFill) {
      TopOpeBRepBuild_Tools::UpdateEdgeOnFace(newE, 
					      TopoDS::Face(mySDFaceToFill), 
					      TopoDS::Face(myBaseFaceToFill));
    }
    else {
      mySourceShapes.Add(newE);
    }

    Standard_Boolean stateOfFaceOri = Standard_False;
    OrientateEdgeOnFace(newE, TopoDS::Face(myBaseFaceToFill), 
			TopoDS::Face(mySDFaceToFill), G1, stateOfFaceOri);
    myMapOfEdgeWithFaceState.Bind (newE, stateOfFaceOri);

    WES.AddStartElement(newE);
  }
  
  //2) Get ON parts of the edge and define to keep it or not
  const TopTools_ListOfShape& LSEOn = myDataStructure -> DS().GetShapeWithState(EOR).Part(TopAbs_ON);
  it.Initialize(LSEOn);
  for(; it.More(); it.Next()) {
    
    TopoDS_Edge aSplitPart = TopoDS::Edge(it.Value());
    aSplitPart.Orientation(EOR.Orientation());

    
    //do ON 2D computation
    if(myDataStructure -> HasSameDomain(aSplitPart) || myDataStructure -> HasSameDomain(EOR)) {
      Standard_Integer flag = 0;
      //First of All : if SDFaceToFill is REVERSED we need to reverse aSplitPart
      TopoDS_Shape eON = aSplitPart;
      TopoDS_Shape nEOR = EOR;
      if(mySDFaceToFill.Orientation() == TopAbs_REVERSED) {
	eON.Reverse();
	nEOR.Reverse();
      }
      TopTools_ListOfShape aListOfPieces, aListOfFaces, aListOfPieceOut2d;
      //Out 2d pieces we compute only one time : for the Object
      if(myProcessedPartsOut2d.Contains(eON))
	continue;
      flag = PerformPieceOn2D (eON, mySDFaceToFill, nEOR, aListOfPieces, aListOfFaces, aListOfPieceOut2d);
      TopTools_ListIteratorOfListOfShape aPIt2d(aListOfPieceOut2d);
      for(; aPIt2d.More(); aPIt2d.Next()) {
	TopoDS_Shape aFP = aPIt2d.Value();
	TopoDS_Shape aRP =  aPIt2d.Value();
	aFP.Reverse();
	WES.AddStartElement(aFP);
	WES.AddStartElement(aRP);
	myProcessedPartsOut2d.Add(aFP);
      }
      TopTools_ListIteratorOfListOfShape aPIt(aListOfPieces), aFIt(aListOfFaces);
      for(; aPIt.More(); aPIt.Next()) {	
	TopoDS_Shape aPieceToKeep = aPIt.Value();
	const TopoDS_Shape& aPieceFace = aFIt.Value();
	if(aPieceFace == mySDFaceToFill) {
	  Standard_Boolean IsRev = (aPieceToKeep.Orientation() == nEOR.Orientation());

	  Standard_Boolean stateOfFaceOri = Standard_False;
	  aPieceToKeep.Orientation(oriE);
	  OrientateEdgeOnFace(TopoDS::Edge(aPieceToKeep), TopoDS::Face(myBaseFaceToFill), 
			      TopoDS::Face(mySDFaceToFill), G1, stateOfFaceOri);
	  //if edge was in not the right orientation we need to reverse it
	  if(!IsRev)
	    aPieceToKeep.Reverse();

	  myMapOfEdgeWithFaceState.Bind (aPieceToKeep, stateOfFaceOri);

	  WES.AddStartElement(aPieceToKeep);
	}
	aFIt.Next();
      }
      if(flag) //if flag == 0 we should go to the IN2D or OUT2D
	continue;
    }
        
    //do IN 2D computation
    TopoDS_Shape aSDShape;
    TopAbs_State aState = TopAbs_UNKNOWN;
    
    if(LSclass.Extent() == 1) {
      aSDShape = LSclass.First();
      aState = ClassifyEdgeToFaceByOnePoint(aSplitPart, TopoDS::Face(aSDShape));
    }
    else { //if face has more than one same domain faces we need to detect for each part complement same domain face
      TopTools_ListIteratorOfListOfShape LSClassIt(LSclass);
      for(; LSClassIt.More(); LSClassIt.Next()) {
	TopoDS_Face curSD = TopoDS::Face(LSClassIt.Value());
	aState = ClassifyEdgeToFaceByOnePoint(aSplitPart,curSD);
	//	aState = ClassifyEdgeToFaceByOnePoint(aSplitPart, TopoDS::Face(curSD));
	if(aState == TopAbs_IN || aState == TopAbs_ON) {
	  aSDShape = curSD;
	  break;
	}
      }
    }

    //we should process all same domain edges (ON2D) in the code above
    //and we can not proceess edges with UNKNOWN state
    if(aState == TopAbs_ON || aState == TopAbs_UNKNOWN) 
      continue;

    //OUT2D computation
    if(aState == TopAbs_OUT || aSDShape.IsNull()) { 
      //it means that SplitPart is ON 
      //comparing with myShape2 but Out of all this SD faces
      //so we need to find adjacent faces and they also MUST be SameDomain and compute all in reverse order
      
      Standard_Boolean keep = Standard_False;
      
      if(aSDShape.IsNull()) {
	aSDShape = LSclass.First();
      }
      
      //compute adjacents
      TopoDS_Shape aAdjSDFace;
      const TopTools_ListOfShape& aFEL = myMapOfEdgeFaces.FindFromKey(EOR);
      TopTools_ListIteratorOfListOfShape aEFIt(aFEL);
      if(aFEL.Extent() <= 2) { //we don't compute adjacent if we have more than one adjacent face
	for(; aEFIt.More(); aEFIt.Next()) {
	  if(mySDFaceToFill.IsSame(aEFIt.Value()))
	    continue;
	  else {
	    if(myDataStructure -> HasSameDomain(aEFIt.Value())) {
	      aAdjSDFace = aEFIt.Value();
	      break;
	    }
	  }
	}
      }
	
      if(!aAdjSDFace.IsNull()) {
	TopTools_IndexedMapOfShape aEAdjMap;
	TopExp::MapShapes(aAdjSDFace, TopAbs_EDGE, aEAdjMap);
	
	Standard_Integer index = aEAdjMap.FindIndex(EOR);
	TopoDS_Shape AdjEOR = aEAdjMap.FindKey(index);
	
	TopTools_ListIteratorOfListOfShape it1 = myDataStructure -> SameDomain(aAdjSDFace);
	TopoDS_Shape aSDToAdjFace = it1.Value();
	
	TopoDS_Edge aSplitP = aSplitPart; 
	aSplitP.Orientation(AdjEOR.Orientation());
	
	gp_Vec aTg, aN1, aN2,aN3, aBiN;
	TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(TopoDS::Face(aSDToAdjFace), aSplitP, aN2);
	if(aSDToAdjFace.Orientation() == TopAbs_REVERSED)
	  aN2.Reverse();
	
	TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(TopoDS::Face(aAdjSDFace), aSplitP, aN3);
	if(aAdjSDFace.Orientation() == TopAbs_REVERSED)
	  aN3.Reverse();
	
	TopOpeBRepBuild_Tools::GetTangentToEdge(aSplitP, aTg);
	if (aSplitP.Orientation() == TopAbs_REVERSED) {
	  aTg.Reverse();
	}
	  
	aBiN = aTg^aN2;
	
	Standard_Real scalarPr = 0.;
	
	TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(TopoDS::Face(mySDFaceToFill), aSplitP, aN1);
	if(mySDFaceToFill.Orientation() == TopAbs_REVERSED)
	    aN1.Reverse();
	scalarPr = aBiN*aN1;

        if (fabs (scalarPr) <= 1e-10) //try to step inside
        {
          TopOpeBRepBuild_Tools::GetNormalInNearestPoint (TopoDS::Face (mySDFaceToFill), aSplitP, aN1);
          if (mySDFaceToFill.Orientation () == TopAbs_REVERSED)
          {
            aN1.Reverse ();
          }
          scalarPr = aBiN*aN1;
          if (fabs (scalarPr) <= 1e-10) // this can not be
          {
            keep = (TB == TopAbs_IN); //just to do something
          }
        }

	TopAbs_State aPartState = (scalarPr > 0) ? TopAbs_IN : TopAbs_OUT;
	
	keep = (aPartState == TB) ? Standard_True : Standard_False;
      }
      else { //if aAdjFace.IsNull() - it must not happen
	keep = (TB == TopAbs_IN);
      }
      
      if(keep) {
	if(mySDFaceToFill != myBaseFaceToFill) {
	  TopOpeBRepBuild_Tools::UpdateEdgeOnFace(aSplitPart, 
						  TopoDS::Face(mySDFaceToFill), 
						  TopoDS::Face(myBaseFaceToFill));
	}
	else {
	  mySourceShapes.Add(aSplitPart);
	}

	Standard_Boolean stateOfFaceOri = Standard_False;
	OrientateEdgeOnFace(aSplitPart, TopoDS::Face(myBaseFaceToFill), 
			    TopoDS::Face(mySDFaceToFill), G1, stateOfFaceOri);
	myMapOfEdgeWithFaceState.Bind (aSplitPart, stateOfFaceOri);
	
	WES.AddStartElement(aSplitPart);	
      }
      continue;
    }
    //end case OUT2D

    //IN2D computation
    TopoDS_Edge aSplitP = aSplitPart; 
    aSplitP.Orientation(EOR.Orientation());
    TopoDS_Face aSDFace = TopoDS::Face(aSDShape);

    Standard_Boolean keep = Standard_False;
    PerformPieceIn2D(aSplitPart, TopoDS::Edge(EOR), 
		     TopoDS::Face(mySDFaceToFill), aSDFace, G1, keep);
    
    if(keep) { 	
      mySplitsONtoKeep.Add(aSplitPart);
      
      //compute orientation of the future face
      Standard_Boolean stateOfFaceOri = Standard_False;
      OrientateEdgeOnFace(aSplitPart, TopoDS::Face(myBaseFaceToFill), aSDFace, G1, stateOfFaceOri);
      myMapOfEdgeWithFaceState.Bind (aSplitPart, stateOfFaceOri);

      if(myBaseFaceToFill == mySDFaceToFill) {
	mySourceShapes.Add(aSplitPart);
      }

      WES.AddStartElement(aSplitPart);
    }
  }
} // GFillEdgeWES

extern Standard_Boolean TopOpeBRepBuild_FUN_aresamegeom(const TopoDS_Shape& S1,const TopoDS_Shape& S2);

//=======================================================================
//function : PerformONParts
//purpose  : 
//=======================================================================  
void TopOpeBRepBuild_Builder1::PerformONParts(const TopoDS_Shape& FOR1,
					      const TopTools_IndexedMapOfShape& /*SDFaces*/,
					      const TopOpeBRepBuild_GTopo& G1,
					      TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State ETB1,ETB2, ETB; G1.StatesON(ETB1,ETB2);
  TopAbs_State FTB1,FTB2, FTB; G1.StatesON(FTB1,FTB2);

  Standard_Integer iref = myDataStructure -> DS().AncestorRank(FOR1);
    
  if(iref == 1) {//object
    FTB = FTB1;
  }
  else {//tool
    FTB = FTB2;
  }

  //3 Process parts that can not be found on SD faces but must be included because they are ON the SD faces
  const TopOpeBRepDS_ListOfInterference& LI = myDataStructure -> DS().ShapeInterferences(FOR1); 
  for (TopOpeBRepDS_ListIteratorOfListOfInterference ILI(LI);ILI.More();ILI.Next() ) {
    const Handle(TopOpeBRepDS_Interference)& I=ILI.Value();
    Handle(TopOpeBRepDS_ShapeShapeInterference) SSI
      = Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I);

    if (SSI.IsNull()) 
      continue;

    TopOpeBRepDS_Kind GT,ST;
    Standard_Integer GI,SI;
    FDS_data(SSI,GT,GI,ST,SI);
    if (GT != TopOpeBRepDS_EDGE || ST != TopOpeBRepDS_FACE) 
      continue;

    const TopoDS_Edge& EG=TopoDS::Edge(myDataStructure -> DS().Shape(GI, Standard_False));
    //we process only the edges which are not from the current SD faces
    if(mySDEdgeMap.Contains(EG))
      continue;

    //take ON splits of the edge
    const TopTools_ListOfShape& splON = myDataStructure -> DS().GetShapeWithState(EG).Part(TopAbs_ON);
    if(!splON.Extent())
      continue;

    const TopOpeBRepDS_Transition& aTr = SSI -> Transition();

    Standard_Integer irefE = myDataStructure -> DS().AncestorRank(EG);
    
    Standard_Boolean RevOriE;
    if(irefE == 1) {//object
      ETB = ETB1;
      RevOriE = G1.IsToReverse1();
    }
    else {//tool
      RevOriE = G1.IsToReverse2();
      ETB = ETB2;
    }

      
    //take list of edge faces
    const TopTools_ListOfShape& EdgeFaces = myMapOfEdgeFaces.FindFromKey(EG); 
    TopExp_Explorer Exp;
    
    for(TopTools_ListIteratorOfListOfShape itON(splON); itON.More(); itON.Next()) {
      TopoDS_Shape newE = itON.Value();

      TopoDS_Shape aSDShape = FOR1;
      TopAbs_State aState = TopAbs_UNKNOWN;
  
      aState = ClassifyEdgeToFaceByOnePoint(TopoDS::Edge(newE), TopoDS::Face(FOR1));
      if(!(aState == TopAbs_IN || aState == TopAbs_ON)) 
	continue;
    
      Standard_Boolean keep = Standard_False;
      TopoDS_Face aSDFace = TopoDS::Face(aSDShape);
      
      TopAbs_Orientation oriE;
      TopAbs_Orientation neworiE;

      for(TopTools_ListIteratorOfListOfShape it(EdgeFaces); it.More(); it.Next()) {
	const TopoDS_Shape& FOR = it.Value();
	Exp.Init(FOR, TopAbs_EDGE);
	TopoDS_Shape EOR; 
	for(; Exp.More(); Exp.Next()) {
	  EOR = Exp.Current();
	  if(EG.IsSame(EOR))
	    break;
	}
	 
	if(EOR.IsNull())
	  continue;
 
	//else we have found a face , we process it
	oriE = EOR.Orientation();
	neworiE = Orient(oriE,RevOriE);

	newE.Orientation(oriE);
	gp_Vec aTg, aN2,aN3, aBiN;
	TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(TopoDS::Face(FOR), TopoDS::Edge(newE), aN2);
	if(FOR.Orientation() == TopAbs_REVERSED)
	  aN2.Reverse();
	TopOpeBRepBuild_Tools::GetTangentToEdge(TopoDS::Edge(newE), aTg);
	if (newE.Orientation() == TopAbs_REVERSED) {
	  aTg.Reverse();
	}
	TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(aSDFace, TopoDS::Edge(newE), aN3);
	if(aSDFace.Orientation() == TopAbs_REVERSED)
	  aN3.Reverse();
	keep = Standard_False;
	aBiN = aTg^aN2;
	Standard_Real scalarPr = 0.;
	scalarPr = aBiN*aN3;
	
	if(fabs(scalarPr) <= 1e-10) {//try to step inside
	  TopOpeBRepBuild_Tools::GetNormalInNearestPoint(TopoDS::Face(FOR), TopoDS::Edge(newE), aN2);
	  if(FOR.Orientation() == TopAbs_REVERSED)
	    aN2.Reverse();
	  aBiN = aTg^aN2;
	  scalarPr = aBiN*aN3;
	  if(fabs(scalarPr) <= 1e-10) 
	    continue;
	}
	TopAbs_State aPartState = (scalarPr > 0) ? TopAbs_IN : TopAbs_OUT;
	keep = (aPartState == ETB) ? Standard_True : Standard_False;
	if(keep)
	  break;
      }

      if(keep) {
	//compute orientation of the future face
	Standard_Boolean stateOfFaceOri = Standard_False;
	gp_Vec aNbf, aNsf , OrigNormalbf; //aTg, aBiN, aOut;
	TopoDS_Edge aLocalEdge  = TopoDS::Edge(newE);
	TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(aSDFace,aLocalEdge, aNsf);
	//	TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(TopoDS::Face(aSDFace), TopoDS::Edge(newE), aNsf);
	if(aSDFace.Orientation() == TopAbs_REVERSED)
	  aNsf.Reverse();
	TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(TopoDS::Face(myBaseFaceToFill), TopoDS::Edge(newE), OrigNormalbf);
	aNbf = OrigNormalbf;
	if(myBaseFaceToFill.Orientation() == TopAbs_REVERSED)
	  aNbf.Reverse();

	if(aNsf*aNbf < 0) {
	  stateOfFaceOri = Standard_True;
	}
	
	if(myDataStructure -> DS().AncestorRank(aSDFace) == 2) {//for tool we need to reverse face in cut
	  if(Opec12() || Opec21()) {
	    stateOfFaceOri = !stateOfFaceOri;
	  }
	}

	//adjust orientation of the edge
	neworiE = aTr.Orientation(FTB);
	Standard_Boolean samegeom = TopOpeBRepBuild_FUN_aresamegeom(FOR1,myBaseFaceToFill);
	if (!samegeom) {
	  neworiE = TopAbs::Complement(neworiE);
	}
	newE.Orientation(neworiE);

	myMapOfEdgeWithFaceState.Bind (newE, stateOfFaceOri);
	WES.AddStartElement(newE);
      }
    }//end iteration on splON
  }//end iteration of interferences
}

//=======================================================================
//function : GWESMakeFaces
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::GWESMakeFaces(const TopoDS_Shape& FF,
					     TopOpeBRepBuild_WireEdgeSet& WES,
					     TopTools_ListOfShape& LOF)  
{
  TopOpeBRepBuild_Builder::GWESMakeFaces(FF, WES, LOF);
  TopTools_ListIteratorOfListOfShape aLOFit(LOF);
  TopTools_ListOfShape corrLOF;
  if(myIsKPart == 4) {
    for(; aLOFit.More(); aLOFit.Next()) {
      const TopoDS_Shape& ff = aLOFit.Value();
      TopoDS_Shape corrFF;
      TopOpeBRepBuild_Tools::NormalizeFace(ff, corrFF);
      corrLOF.Append(corrFF);
    }
  }
  else
    corrLOF.Assign(LOF);

  LOF.Clear(); LOF.Assign(corrLOF);

  //corect face2d
  aLOFit.Initialize(corrLOF);
  TopTools_ListOfShape corrLOF1;
  for(; aLOFit.More(); aLOFit.Next()) {
    const TopoDS_Shape& ff = aLOFit.Value();
    TopoDS_Shape corrFF;
    TopOpeBRepBuild_Tools::CorrectFace2d(ff, corrFF, mySourceShapes, myMapOfCorrect2dEdges);
    corrLOF1.Append(corrFF);
  }

  LOF.Clear(); LOF.Assign(corrLOF1);
}

//=======================================================================
//function : PerformPieceIN2d
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::PerformPieceIn2D(const TopoDS_Edge& EdgeToPerform,
						const TopoDS_Edge& EOR,
						const TopoDS_Face& edgeFace,
						const TopoDS_Face& toFace,
						const TopOpeBRepBuild_GTopo& G1,
						Standard_Boolean& keep)
{
  keep = Standard_False;

  TopAbs_State TB1,TB2, TB; G1.StatesON(TB1,TB2);

  Standard_Integer iref = myDataStructure -> DS().AncestorRank(EOR);

  TB = (iref == 1) ? TB1 : TB2;
  
  gp_Vec aTg, aN1, aN2,aN3, aBiN;

  TopAbs_Orientation O1 = edgeFace.Orientation();
  TopAbs_Orientation O2 = toFace.Orientation();
  TopAbs_Orientation oriE = EdgeToPerform.Orientation();

  TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(toFace, EdgeToPerform, aN2);  
  if(O2 == TopAbs_REVERSED)
    aN2.Reverse();
    
  TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(edgeFace, EdgeToPerform, aN3);
  if(O1 == TopAbs_REVERSED)
    aN3.Reverse();

  TopOpeBRepBuild_Tools::GetTangentToEdge(EdgeToPerform, aTg);
  if (oriE == TopAbs_REVERSED) 
    aTg.Reverse();
  if(O1 == TopAbs_REVERSED)
    aTg.Reverse();

  
  aBiN = aTg^aN2;
  const TopTools_ListOfShape& aFEL = myMapOfEdgeFaces.FindFromKey(EOR);
  TopTools_ListIteratorOfListOfShape aEFIt(aFEL);
  Standard_Real scalarPr = 0.;

  /// Why ????? Need to be checked
  if(aFEL.Extent() <= 2) { //we don't compute adjacent if we have more than one adjacent face
    for(; aEFIt.More(); aEFIt.Next()) {
      if(edgeFace.IsSame(aEFIt.Value()))
	continue;
      else { //compute bi-normal state
	TopoDS_Face aAdjF = TopoDS::Face(aEFIt.Value());
	TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(aAdjF, EdgeToPerform, aN1);
	if(aAdjF.Orientation() == TopAbs_REVERSED)
	  aN1.Reverse();
	scalarPr = aBiN*aN1;
	  
	if(fabs(scalarPr) <= 1e-10) { //try to step inside
	  TopOpeBRepBuild_Tools::GetNormalInNearestPoint(aAdjF, EdgeToPerform, aN1);
	  //	  TopOpeBRepBuild_Tools::GetNormalInNearestPoint(TopoDS::Face(aAdjF), EdgeToPerform, aN1);
	  if(aAdjF.Orientation() == TopAbs_REVERSED)
	    aN1.Reverse();
	    
	  scalarPr = aBiN*aN1;
	  if(fabs(scalarPr) <= 1e-10) 
	    continue;
	}	    
	    
	TopAbs_State aPartState = (scalarPr > 0) ? TopAbs_IN : TopAbs_OUT;
	keep = (aPartState == TB) ? Standard_True : Standard_False;
	if(keep)
	  break;
      }
    }
  }
      
  //if scalar can not be found that means that adjacent face doesn't exist
  //WARNING !!! May be this code is not good but for the moment it is only one solution
  if(fabs(scalarPr) <= 1e-10) {
    if(Opefus())  {
      keep = aN3*aN2 > 0;
    }
    if(Opec12() || Opec21())
      keep = aN3*aN2 < 0;
    if(Opecom())
      keep = aN3*aN2 > 0;
  } 
}

//=======================================================================
//function : PerformPieceOn2D
//purpose  : 
//=======================================================================  
Standard_Integer TopOpeBRepBuild_Builder1::PerformPieceOn2D (const TopoDS_Shape& aPieceObj, 
							     const TopoDS_Shape& aFaceObj,
							     const TopoDS_Shape& anEdgeObj,
							     TopTools_ListOfShape& aListOfPieces,
							     TopTools_ListOfShape& aListOfFaces,
							     TopTools_ListOfShape& aListOfPiecesOut2d)
{
  // eap 30 May occ417, aCasesMap instead of aCase14 and aCase12
  Standard_Integer i, j, k, flag=0, priz;//, aCase14=0, aCase12=0;
  TColStd_MapOfInteger aCasesMap;

  Standard_Integer iRef = myDataStructure -> DS().AncestorRank(aFaceObj);

  if(!myDataStructure -> HasSameDomain(aFaceObj)) 
    return -1;
  // Main DataStructure
  TopOpeBRepDS_DataStructure& aDS= myDataStructure-> ChangeDS();

  // Main Map for Tool (relative Tool)
  TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithStateTool=
    (iRef == 1) ? aDS.ChangeMapOfShapeWithStateTool() : aDS.ChangeMapOfShapeWithStateObj();
  // Loop on faces same domain to aFaceObj
  TopTools_ListIteratorOfListOfShape anIt(myDataStructure->SameDomain(aFaceObj));
  for (i=1; anIt.More(); anIt.Next(), i++) {
    const TopoDS_Shape& aFaceTool=anIt.Value();
    
    TopTools_IndexedMapOfShape anEdgesToolMap;
    TopExp::MapShapes(aFaceTool, TopAbs_EDGE, anEdgesToolMap);

    if(myDataStructure -> HasSameDomain(anEdgeObj)) {
      TopTools_ListIteratorOfListOfShape anItE=myDataStructure->SameDomain(anEdgeObj);
      for (j=1; anItE.More(); anItE.Next(), j++) {
        TopoDS_Shape anEdgeTool=anItE.Value();

	if (anEdgesToolMap.Contains (anEdgeTool)) {
	  
	  TopExp_Explorer anExpEdges;
	  for (anExpEdges.Init (aFaceTool, TopAbs_EDGE); anExpEdges.More(); anExpEdges.Next()) {
	    const TopoDS_Shape& anExpEdgeTool=anExpEdges.Current();
	    if (!anExpEdgeTool.IsSame(anEdgeTool)) continue;
	    
	    anEdgeTool.Orientation(anExpEdgeTool.Orientation());

	    const TopOpeBRepDS_ShapeWithState& aSWSTool=
	      aMapOfShapeWithStateTool.FindFromKey(anEdgeTool);
	    
	    const TopTools_ListOfShape& aPartOnTool=aSWSTool.Part(TopAbs_ON);
	    
	    // we are looking for the same piece as aPieceObj among aPartOnTool
	    TopTools_ListIteratorOfListOfShape anItTool(aPartOnTool);
	    for (k=1; anItTool.More(); anItTool.Next(), k++) {
	      TopoDS_Shape& aPieceTool=anItTool.Value();
	      aPieceTool.Orientation(anEdgeTool.Orientation());

	      Standard_Boolean aIsSameCnd, IsDegFlag;
	      
	      IsDegFlag=
		BRep_Tool::Degenerated (TopoDS::Edge(aPieceObj)) &&
		  BRep_Tool::Degenerated (TopoDS::Edge(aPieceTool)) ;
	      
	      aIsSameCnd=IsDegFlag ? TopOpeBRepBuild_Tools::IsDegEdgesTheSame(aPieceObj, aPieceTool) : aPieceObj.IsSame(aPieceTool);
	      
	      if (aIsSameCnd) { 

		TopTools_SequenceOfShape aSeq;
		aSeq.Append(aFaceObj) ; aSeq.Append(anEdgeObj) ; aSeq.Append(aPieceObj) ;  
		aSeq.Append(aFaceTool); aSeq.Append(anEdgeTool); aSeq.Append(aPieceTool);

		flag++;
		priz=TwoPiecesON (aSeq, aListOfPieces, aListOfFaces, aListOfPiecesOut2d);

		//if (priz==14) aCase14=1;
		//if (priz==12) aCase12=1;
		aCasesMap.Add(priz);
		break;
	      }
	    }
	    
	    if (!flag) {
	      //printf("Warning : => aPieceTool is not found\n");
	      //modified by NIZHNY-MZV  Thu Dec 23 17:30:20 1999
	      //return -2;
	    }
	  }
	}
      }
    }
  }
  //this case dedicated for the computation then edge has sim (F and R at one time) SD edge 
  if ( flag>1 ) {
    if ( aCasesMap.Contains(14) && aCasesMap.Contains(12) && Opefus() )
      aListOfPieces.Clear();
    // eap 30 May occ417, add :
    if ( aCasesMap.Contains(11) && aCasesMap.Contains(13) && (Opec12() || Opec21()) )
      aListOfPieces.Clear();
  }
  return flag; //Ok
}

//=======================================================================
//function :  TwoPiecesON
//purpose  : 
//=======================================================================  
Standard_Integer TopOpeBRepBuild_Builder1::TwoPiecesON (const TopTools_SequenceOfShape& aSeq,
							TopTools_ListOfShape& aListOfPieces,
							TopTools_ListOfShape& aListOfFaces,
							TopTools_ListOfShape& aListOfPiecesOut2d)
{
  // Restore Data
  if (aSeq.Length() < 6) 
    return -2;
  TopoDS_Shape aFaceObj  =aSeq(1); 
  TopoDS_Shape anEObj    =aSeq(2); 
  TopoDS_Shape aPieceObj =aSeq(3); 
  TopoDS_Shape aFaceTool =aSeq(4); 
  TopoDS_Shape anETool   =aSeq(5); 
  TopoDS_Shape aPieceTool=aSeq(6); 
 
  // The two Maps for adjacent faces
  Standard_Integer iRef = myDataStructure -> DS().AncestorRank(aFaceObj);

  TopTools_IndexedDataMapOfShapeListOfShape anEdgeFaceMapObj, anEdgeFaceMapTool;

  if(iRef == 1) {
    TopExp::MapShapesAndAncestors(myShape1, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMapObj );
    TopExp::MapShapesAndAncestors(myShape2, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMapTool);
  }
  else {
    TopExp::MapShapesAndAncestors(myShape1, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMapObj );
    TopExp::MapShapesAndAncestors(myShape2, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMapTool);
    TopoDS_Shape tmpFace = aFaceObj, tmpPiece = aPieceObj, tmpEdge = anEObj;
    aFaceObj = aFaceTool; aPieceObj = aPieceTool; anEObj = anETool;
    aFaceTool = tmpFace; aPieceTool = tmpPiece; anETool = tmpEdge;
  }
  //  
  Standard_Boolean IsFacesDifOriented       , IsEdgesRevSense,
                   anAd1=Standard_False     , anAd2=Standard_False,
                   aScPrFlag1=Standard_False, aScPrFlag2=Standard_False,
                   Rejected1=Standard_True  , Rejected2=Standard_True;

  TopAbs_State     aStateObj =TopAbs_UNKNOWN, aStateTool=TopAbs_UNKNOWN;
  Standard_Real    aScProductObj =0.        , aScProductTool=0.,
                   aTol=1.e-5;

  
  Standard_Real    aScPrObj=0.,   aScPrTool=0.;              
  gp_Vec anyN;
  
  TopoDS_Shape anAdjFaceObj, anAdjFaceTool;
  
  
  // Faces
  TopoDS_Face aFObj     = TopoDS::Face(aFaceObj);
  TopoDS_Face aFTool    = TopoDS::Face(aFaceTool);
  // Pieces
  TopoDS_Edge anEdgeObj = TopoDS::Edge(aPieceObj);
  TopoDS_Edge anEdgeTool= TopoDS::Edge(aPieceTool);

  //OldEdges
  TopoDS_Edge aOriEObj = TopoDS::Edge(anEObj);
  TopoDS_Edge aOriETool = TopoDS::Edge(anETool);
 
  // Normals to the Faces 
  TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge (aFObj, anEdgeObj, anyN);
  if(aFObj.Orientation() == TopAbs_REVERSED)
    anyN.Reverse();
  gp_Dir aDNObj(anyN); 
 
  //  
  TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge (aFTool, anEdgeTool, anyN);
  if(aFTool.Orientation() == TopAbs_REVERSED)
    anyN.Reverse();
  gp_Dir aDNTool (anyN);

  // are aFaceObj & aFaceTool different oriented faces or not ?
  IsFacesDifOriented=aDNObj*aDNTool < 0.;
  // Sense of the Pieces
  Standard_Boolean RevObj = TopOpeBRepBuild_Tools::GetTangentToEdgeEdge (aFObj, anEdgeObj, aOriEObj, anyN);
  if(RevObj) {
    aPieceObj.Reverse();
    anEdgeObj.Reverse();
  }

  gp_Dir aDTObj(anyN);
  Standard_Boolean RevTool = TopOpeBRepBuild_Tools::GetTangentToEdgeEdge (aFTool, anEdgeTool, aOriETool, anyN);
  if(RevTool) {
    aPieceTool.Reverse();
    anEdgeTool.Reverse();
  }
  gp_Dir aDTTool(anyN);
  
  IsEdgesRevSense= aDTObj*aDTTool < 0.; 

  // try to get adjacent faces for Obj and Tool. Ad1, Ad2 indicate that the face exists.
  anAd1=TopOpeBRepBuild_Tools::GetAdjacentFace (aFaceObj, anEObj, anEdgeFaceMapObj, anAdjFaceObj);
  anAd2=TopOpeBRepBuild_Tools::GetAdjacentFace (aFaceTool, anETool, anEdgeFaceMapTool, anAdjFaceTool);
 
  if (anAd1 && anAd2)   {
    // both adjacents are found , so we can calculate the scalar products
    TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge (TopoDS::Face(anAdjFaceObj), anEdgeObj, anyN);
    if(anAdjFaceObj.Orientation() == TopAbs_REVERSED)
      anyN.Reverse();
    gp_Dir aDNAObj (anyN);
   
    TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge (TopoDS::Face(anAdjFaceTool), anEdgeTool, anyN);
    if(anAdjFaceTool.Orientation() == TopAbs_REVERSED)
      anyN.Reverse();
    gp_Dir aDNATool (anyN);
   
    aScPrObj =(aDTObj^aDNTool)*aDNAObj;
    aScPrTool=(aDTTool^aDNObj)*aDNATool;

    if(fabs(aScPrObj) <= aTol) {//if scalar product is == 0 try to move a little from this point
      TopOpeBRepBuild_Tools::GetNormalInNearestPoint (TopoDS::Face(anAdjFaceObj), anEdgeObj, anyN);
      if(anAdjFaceObj.Orientation() == TopAbs_REVERSED)
	anyN.Reverse();
      aDNAObj.SetXYZ (anyN.XYZ());
      aScPrObj =(aDTObj^aDNTool)*aDNAObj;
    }

    if(fabs(aScPrTool) <= aTol) {//if scalar product is == 0 try to move a little from this point
      TopOpeBRepBuild_Tools::GetNormalInNearestPoint (TopoDS::Face(anAdjFaceTool), anEdgeTool, anyN);
      if(anAdjFaceTool.Orientation() == TopAbs_REVERSED)
	anyN.Reverse();
      aDNATool.SetXYZ (anyN.XYZ());
      aScPrTool =(aDTTool^aDNObj)*aDNATool;
    }

    // Scalar prouducts must not have too small value: 
    aScProductObj=aScPrObj;
    aScProductTool=aScPrTool;
    /*
    // mine
    gp_Dir aDBNObj(aDNObj^aDTObj);
    aScProductObj=aDBNObj*aDNATool;
    gp_Dir aDBNTool(aDNTool^aDTTool);
    aScProductTool=aDBNTool*aDNAObj; 
    */
    // Scalar prouducts must not have too small value: 
    if (fabs(aScProductTool) > aTol) aScPrFlag1=Standard_True;
    if (fabs(aScProductObj ) > aTol) aScPrFlag2=Standard_True; 
  }

  // Management 
  if (!anAd1 || !anAd2 || !aScPrFlag1 || !aScPrFlag2) {
    // manage without adjacents.
    //  case a.  No==Nt , To!=Tt
    if (IsEdgesRevSense && !IsFacesDifOriented ) { 
      if (Opec12()) {
	Standard_Boolean poisc = BRep_Tool::IsClosed(anEdgeObj,aFObj);
	if(!poisc)
	  {
	    aListOfPieces.Append (aPieceObj);
	    aListOfFaces.Append (aFaceObj);
	  }
      }
      return 11; 
    }
    
    // case b.  No!=Nt , To!=Tt
    if (IsEdgesRevSense && IsFacesDifOriented) {
      if (Opec12()) {
	aListOfPieces.Append (aPieceObj);
	aListOfFaces.Append (aFaceObj);
      }
      if(!anAd1 || !anAd2)
	return 12;
      else
	return 10; //10 doesn't mean anything just to retutn something
    }
    
    // case c.  No==Nt , To==Tt
    if (!IsEdgesRevSense && !IsFacesDifOriented) {
      //Begin modified by NIZHNY-MZV  Mon Jan 24 10:03:58 2000
      // WRNG!!
      if(anAd1 && anAd2) {
	if(!Opecom()) {
	  if(!aScPrFlag2) {
	    aListOfPieces.Append (aPieceObj);
	    aListOfFaces.Append (aFaceObj);
	  }
	  if(!aScPrFlag1) {
	    aListOfPieces.Append (aPieceTool);
	    aListOfFaces.Append (aFaceTool);
	  }
	}
      }
      else {
	if(Opefus()) {
	  aListOfPieces.Append (aPieceObj);
	  aListOfFaces.Append (aFaceObj);
	}
	//End modified by NIZHNY-MZV  Mon Jan 24 11:21:17 2000
      }
      return 13; 
    }
    
    // case d.  No!=Nt , To==Tt
    if (!IsEdgesRevSense && IsFacesDifOriented) {
      //modified by NIZHNY-MZV  Fri Jan 21 18:16:01 2000
      // WRNG!!
      if(anAd1 && anAd2) {
	if(!Opecom()) {
	  if(!aScPrFlag2) {
	    aListOfPieces.Append (aPieceObj);
	    aListOfFaces.Append (aFaceObj);
	  }
	  if(!aScPrFlag1) {
	    aListOfPieces.Append (aPieceTool);
	    aListOfFaces.Append (aFaceTool);
	  }
	}
      }
      else {
	if(Opefus()) {
	  aListOfPieces.Append (aPieceObj);
	  aListOfFaces.Append (aFaceObj);
	}
      }
      if(!anAd1 || !anAd2) 
	 return 14;
       else
	 return 10; //10 doesn't mean anything just to retutn something
    }
    return 10;
  } // end of if (!anAd1 || !anAd2 || !aScPrFlag1 || !aScPrFlag2)

  else {
    // We can use adjacents .
    // The States :
    /*
    aStateObj =aScProductObj  < 0. ? TopAbs_IN: TopAbs_OUT ;
    aStateTool=aScProductTool < 0. ? TopAbs_IN: TopAbs_OUT ; 
    */
    aStateObj =aScProductObj  > 0. ? TopAbs_IN: TopAbs_OUT ;
    aStateTool=aScProductTool > 0. ? TopAbs_IN: TopAbs_OUT ; 
    //  case I  RevSense && DifOriented    
    if (IsEdgesRevSense && IsFacesDifOriented) {
      if (Opec12())       {
	aListOfPieces.Append (aPieceObj);
	aListOfFaces.Append (aFaceObj);
      }
      return 1;
    }
    
    //  case III SameSense && !DifOriented      
    if (!IsEdgesRevSense && !IsFacesDifOriented) {
      if (!Opec12())  {
	aListOfPieces.Append (aPieceObj);
	aListOfFaces.Append (aFaceObj);
      }
      return 3;
    }
    
    // case II  RevSense && !DifOriented         
    if (IsEdgesRevSense && !IsFacesDifOriented) {
      if (Opefus()) { // Fusion
	if (aStateObj==TopAbs_OUT && aStateTool==TopAbs_IN) {
	  Rejected1=Standard_False;
	}
	else if (aStateObj==TopAbs_IN && aStateTool==TopAbs_OUT) {
	  Rejected2=Standard_False;
	}
	//// ????
	else if (aStateObj==TopAbs_IN && aStateTool==TopAbs_IN) {
	  if(!myProcessedPartsON2d.Contains(aPieceObj)) {//we proceed IsSame only if we didn't it before
	    myProcessedPartsON2d.Add(aPieceObj);
	    IsSame2d (aSeq, aListOfPiecesOut2d); //Perform IsSame 2d and keep periodic parts
	  }
	}
	else if (aStateObj==TopAbs_OUT && aStateTool==TopAbs_OUT) {
	  if(!myProcessedPartsON2d.Contains(aPieceObj)) {//we proceed IsSame only if we didn't it before
	    myProcessedPartsON2d.Add(aPieceObj);
	    IsSame2d (aSeq, aListOfPiecesOut2d); //Perform IsSame 2d and keep periodic parts
	  }
	}
      }
      
      if (Opecom()) {// Common
	if (aStateObj==TopAbs_OUT && aStateTool==TopAbs_IN) {
	  Rejected2=Standard_False;
	}
	else if (aStateObj==TopAbs_IN && aStateTool==TopAbs_OUT) {
	  Rejected1=Standard_False;
	}
      }
      
      if (Opec12()) {// Cut
	if (aStateObj==TopAbs_OUT && aStateTool==TopAbs_OUT) {
	  Rejected2=Standard_False;
	}
	else if  (aStateObj==TopAbs_IN && aStateTool==TopAbs_IN) {
	  Rejected1=Standard_False;
	}
	else if (aStateObj==TopAbs_OUT && aStateTool == TopAbs_IN) {
	  Rejected1=Standard_False;
	  Rejected2=Standard_False;
	}
      }
      if (!Rejected1) {
	aListOfPieces.Append(aPieceObj);
	aListOfFaces.Append (aFaceObj);
      }
      if (!Rejected2) {
	aListOfPieces.Append(aPieceTool);
	aListOfFaces.Append (aFaceTool);
      }
      return 2;
    }
    
    // case IV   !RevSense && DifOriented         
    if (!IsEdgesRevSense && IsFacesDifOriented) {
      if (Opefus()) {// Fusion
        if (aStateObj==TopAbs_OUT && aStateTool==TopAbs_OUT) {
          Rejected1=Standard_False;
          Rejected2=Standard_False;
        }
        else if (aStateObj==TopAbs_OUT && aStateTool==TopAbs_IN) {
          Rejected2=Standard_False;
        }
        else if (aStateObj==TopAbs_IN && aStateTool==TopAbs_OUT) {
          Rejected1=Standard_False;
        }
        else if (aStateObj==TopAbs_IN && aStateTool==TopAbs_IN) {
          if(!myProcessedPartsON2d.Contains(aPieceObj)) {//we proceed IsSame only if we didn't it before
            myProcessedPartsON2d.Add(aPieceObj);
            IsSame2d (aSeq, aListOfPiecesOut2d); //Perform IsSame 2d and keep periodic parts
          }
        }
      }
      
      if (Opecom()) {// Common
        if (aStateObj==TopAbs_IN && aStateTool==TopAbs_IN) {
          Rejected1=Standard_False;
          Rejected2=Standard_False;
        }
        else if (aStateObj==TopAbs_OUT && aStateTool==TopAbs_IN) {
          Rejected1=Standard_False;
        }
        else if (aStateObj==TopAbs_IN && aStateTool==TopAbs_OUT) {
          Rejected2=Standard_False;
        }
      }
      
      if (Opec12()) { //Cut
        if (aStateObj==TopAbs_OUT && aStateTool==TopAbs_OUT) {
          Rejected1=Standard_False;
        }
        else if (aStateObj==TopAbs_IN && aStateTool==TopAbs_IN) {
          Rejected2=Standard_False;
        }
      }
      if (!Rejected1) {
        aListOfPieces.Append(aPieceObj);
        aListOfFaces.Append (aFaceObj);
      }
      if (!Rejected2) {
        aListOfPieces.Append(aPieceTool); 
        aListOfFaces.Append (aFaceTool);
      }
      return 4;
    }
    // Unknowm case for existing adjacents
    return 0;
  }
}

//=======================================================================
//function : IsSame2d
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_Builder1::IsSame2d (const TopTools_SequenceOfShape& aSeq,
						     TopTools_ListOfShape& aListOfPiecesOut2d)
{
  if (aSeq.Length() < 6)  return 0;

  TopoDS_Shape aFaceObj  =aSeq(1);   TopoDS_Shape anEdgeObj =aSeq(2); 
  TopoDS_Shape aPieceObj =aSeq(3);   TopoDS_Shape aFaceTool =aSeq(4); 
  TopoDS_Shape anEdgeTool=aSeq(5);   TopoDS_Shape aPieceTool=aSeq(6); 

  TopoDS_Face aFObj  =TopoDS::Face(aFaceObj)  ;  TopoDS_Face aFTool =TopoDS::Face(aFaceTool) ;
  TopoDS_Edge anEObj =TopoDS::Edge(anEdgeObj) ;  TopoDS_Edge anETool=TopoDS::Edge(anEdgeTool);
  TopoDS_Edge aPObj  =TopoDS::Edge(aPieceObj) ;  TopoDS_Edge aPTool =TopoDS::Edge(aPieceTool);

  BRepAdaptor_Surface aBAS(aFObj);
  if (!(aBAS.IsUPeriodic() || aBAS.IsVPeriodic())) return 1;

  //we process here only fully closed edges (Vf == Vl)
  if(!BRep_Tool::IsClosed(anEdgeObj) || !BRep_Tool::IsClosed(anEdgeTool))
    return 1;
  
  Standard_Real f = 0., l = 0., tolpc = 0. ,  
                par = 0., parOri = 0., f1 = 0., l1 = 0., parP = 0., gp_Resolution = 1.e-10;
  gp_Pnt2d aUV1;

  Handle(Geom2d_Curve) C2D;
  // C2DPieceTool
  Handle(Geom2d_Curve) C2DPieceTool = FC2D_CurveOnSurface (aPTool, aFObj, f1, l1, tolpc, Standard_True);

  parP= f1*PAR_T + (1 - PAR_T)*l1;
  gp_Pnt2d aPPiece;
  C2DPieceTool -> D0(parP, aPPiece);

  // Tool Edge
  C2D=FC2D_CurveOnSurface (anETool, aFObj, f, l, tolpc, Standard_True);
  Geom2dAPI_ProjectPointOnCurve aPP2d(aPPiece, C2D);
  parOri = aPP2d.LowerDistanceParameter();

  Standard_Boolean IsTrFirst = Standard_True;
  if(parOri < f ) { 
    parOri = 2*M_PI +  parOri;
  }
  if(parOri > l ) {
    parOri = parOri - 2*M_PI;
  }

  gp_Pnt2d aUV2; 
  C2D -> D0(parOri, aUV2);
  // C2DPieceObj
  Handle(Geom2d_Curve) C2DPieceObj=FC2D_CurveOnSurface (aPObj, aFObj, f, l, tolpc, Standard_True);

  par=f*PAR_T + (1 - PAR_T)*l;
  C2DPieceObj->D0 (par, aUV1);
  gp_Vec2d aTranslateV (aUV1, aUV2);
  if(aTranslateV.Magnitude() >= gp_Resolution) {

    Handle(Geom2d_Curve) aTrC2D = Handle(Geom2d_Curve)::DownCast(C2DPieceTool->Copy());
    aTrC2D->Translate(aTranslateV);
    gp_Pnt2d aTFuv, aTLuv;
    aTrC2D -> D0(f1, aTFuv);
    aTrC2D -> D0(l1, aTLuv);
    gp_Vec2d aTrVec (aTFuv, aTLuv);

    Standard_Real fo = 0., lo = 0.;
    Handle(Geom2d_Curve) C2DEdgeObj = FC2D_CurveOnSurface(anEObj, aFObj, fo, lo, tolpc, Standard_True);
    gp_Pnt2d aOFuv, aOLuv;
    C2DEdgeObj -> D0(fo, aOFuv);
    C2DEdgeObj -> D0(lo, aOLuv);
    gp_Vec2d aOVec (aOFuv, aOLuv);
    if(anEObj.Orientation() == TopAbs_REVERSED)
      aOVec.Reverse();
    IsTrFirst = (aTrVec*aOVec > 0) ? Standard_False : Standard_True;

    BRep_Builder BB; 
    Standard_Real tolE = BRep_Tool::Tolerance(aPTool);

    if(IsTrFirst)
      BB.UpdateEdge(aPTool , aTrC2D,  C2DPieceTool, aFObj , tolE);
    else
      BB.UpdateEdge(aPTool ,C2DPieceTool, aTrC2D, aFObj , tolE);

    aListOfPiecesOut2d.Append (aPTool);
    return 0;
  }

  return 1;
}

//=======================================================================
//function : OrientateEdgeOnFace
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder1::OrientateEdgeOnFace(TopoDS_Edge& EdgeToPerform,
						   const TopoDS_Face& baseFace,
						   const TopoDS_Face& edgeFace,
						   const TopOpeBRepBuild_GTopo& G1,
						   Standard_Boolean& stateOfFaceOri) const
{
  gp_Vec aN1, aN2;

  stateOfFaceOri = Standard_False;

  Standard_Integer currRef = myDataStructure -> DS().AncestorRank(mySDFaceToFill);
  Standard_Integer faceRef = myDataStructure -> DS().AncestorRank(edgeFace);
  Standard_Boolean RevOri = Standard_False;
  
  if(currRef == 1) {//object
    RevOri = G1.IsToReverse1();      
  }
  else {//tool
    RevOri = G1.IsToReverse2();
  }

  TopAbs_Orientation oriE = EdgeToPerform.Orientation();
  TopAbs_Orientation neworiE = Orient(oriE, RevOri);
  TopAbs_Orientation faceOri = edgeFace.Orientation();
  TopAbs_Orientation baseOri = baseFace.Orientation();
  TopAbs_Orientation currOri = mySDFaceToFill.Orientation();
  
  TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(edgeFace, EdgeToPerform, aN1);
  if(faceOri == TopAbs_REVERSED)
    aN1.Reverse();

  TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(baseFace, EdgeToPerform, aN2);
  if(baseOri == TopAbs_REVERSED)
    aN2.Reverse();
	
  if(aN1*aN2 < 0)
    stateOfFaceOri = Standard_True;

  if(faceRef == 2) //for tool we need to reverse face in cut
    if(Opec12() || Opec21())
      stateOfFaceOri = !stateOfFaceOri;

  //orientate edge with neworiE
  EdgeToPerform.Orientation(neworiE);

  if(currOri != baseOri)
    EdgeToPerform.Reverse();

  if(stateOfFaceOri)
    EdgeToPerform.Reverse();
}



/////////////// STATIC FUNCTIONS
static TopAbs_State ClassifyEdgeToFaceByOnePoint(const TopoDS_Edge& E,
						 const TopoDS_Face& F)
{
  Standard_Real  f2 = 0., l2 = 0., tolpc = 0. , par = 0.;
  Handle(Geom2d_Curve) C2D = FC2D_CurveOnSurface(E, F, f2, l2, tolpc, Standard_True);

  par = f2*PAR_T + (1 - PAR_T)*l2;
	
  gp_Pnt2d aP2d;

  if(C2D.IsNull())
    return TopAbs_UNKNOWN;

  C2D -> D0(par, aP2d);

  BRepTopAdaptor_FClass2d FC(F, 1e-7);
  TopAbs_State aState = FC.Perform(aP2d);

  return aState;
}
