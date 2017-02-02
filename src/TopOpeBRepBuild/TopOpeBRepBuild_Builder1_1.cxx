// Created on: 1999-10-07
// Created by: Peter KURNEV
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTool.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_ListOfListOfLoop.hxx>
#include <TopOpeBRepBuild_Loop.hxx>
#include <TopOpeBRepBuild_Pave.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_Tools.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_DataMapIteratorOfDataMapOfShapeState.hxx>
#include <TopOpeBRepDS_DataMapOfShapeState.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_IndexedDataMapOfShapeWithState.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_ShapeWithState.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopTools_DataMapOfShapeListOfInteger.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//define parameter division number as 10*e^(-PI) = 0.43213918
const Standard_Real PAR_T = 0.43213918;

static TopAbs_State ClassifyEdgeToSolidByOnePoint(const TopoDS_Edge& E,
						  TopOpeBRepTool_ShapeClassifier& SC);

//=======================================================================
//         : Definition the States of Shape's Entities for an Object
//         : and a Tool.                    Thu Oct  7 09:38:29 1999
//=======================================================================

static TopTools_IndexedMapOfShape processedEdges;
static TopTools_IndexedMapOfShape theUsedVertexMap;
static TopTools_MapOfShape theUnkStateVer;

extern Standard_Boolean GLOBAL_faces2d;

//=======================================================================
//function : ~TopOpeBRepBuild_Builder1
//purpose  : 
//=======================================================================
TopOpeBRepBuild_Builder1::~TopOpeBRepBuild_Builder1()
{
  processedEdges.Clear();
  theUsedVertexMap.Clear();
  theUnkStateVer.Clear();
}

/*
namespace {

void DumpMapOfShapeWithState (const Standard_Integer iP,
                              const TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithState)
{
  static Standard_Integer cnt=0;
  TCollection_AsciiString aFName1 ("/DEBUG/TOPOPE/"), postfix;

  Standard_CString ShapeType [9] = {"COMPO", "COMPS", "SOLID", "SHELL", "FACE ", "WIRE ", "EDGE ", "VERTX"};
  Standard_CString ShapeState[4] = {"IN ", "OUT", "ON ", "UNKNOWN"};
  
  printf("\n\n********************************\n");
  printf("*                              *\n");
  Standard_Integer i, n=aMapOfShapeWithState.Extent();
  if (!iP) {
    printf("*  Object comparing with TOOL  *\n");
    postfix=TCollection_AsciiString("Obj");
  }
    
  else {
    printf("*  Tool comparing with Object  *\n");
    postfix=TCollection_AsciiString("Tool");
  }
  
  printf("*                              *\n");
  printf("********************************\n");
  printf("***       aMapOfShapeWithState.Extent()=%d\n", n);
  printf("                 C O N T E N T S\n"); 

  TCollection_AsciiString aFName;
  aFName+=aFName1; 
  aFName+=postfix;

  for (i=1; i<=n; i++) {
    TCollection_AsciiString aI(i), aName;;
    aName+=aFName; aName+=aI;

    const TopoDS_Shape& aShape=aMapOfShapeWithState.FindKey(i);
    const TopOpeBRepDS_ShapeWithState& aShapeWithState=
      aMapOfShapeWithState.FindFromIndex(i);
    
    BRepTools::Write (aShape, aName.ToCString());

    TCollection_AsciiString ann;
    ann+=postfix; ann+=aI;

    printf("Key: %-8s , " , ann.ToCString());
    printf("%s, ", ShapeType[aShape.ShapeType()]);
    if (!iP) 
      printf("State comp.with Tool=%s\n",  ShapeState[aShapeWithState.State()]);

    else 
      printf("State comp.with Obj =%s\n",  ShapeState[aShapeWithState.State()]);
    
    if (aShapeWithState.IsSplitted()) {
      
      const TopTools_ListOfShape& aListOfShape=aShapeWithState.Part(TopAbs_IN);
      TopTools_ListIteratorOfListOfShape anIt(aListOfShape);
      for (;anIt.More(); anIt.Next()) {
	const TopoDS_Shape& aS=anIt.Value();
	
	TCollection_AsciiString cn(cnt), prefix("_S_"), sn;
	sn+=aFName; sn+=prefix; sn+=cn;
	BRepTools::Write (aS, sn.ToCString());
	
	TCollection_AsciiString an;//=postfix+prefix+cn;
	an+=postfix; an+=prefix; an+=cn;
	printf("  -> Splitted Part IN : %s\n",  an.ToCString());
	cnt++;
      }

      const TopTools_ListOfShape& aListOfShapeOut=aShapeWithState.Part(TopAbs_OUT);
      anIt.Initialize (aListOfShapeOut);
      for (;anIt.More(); anIt.Next()) {
	const TopoDS_Shape& aS=anIt.Value();

	TCollection_AsciiString cn(cnt), prefix("_S_"), sn;//=aFName+prefix+cn;
	sn+=aFName; sn+=prefix; sn+=cn;
	BRepTools::Write (aS, sn.ToCString());
	
	TCollection_AsciiString an;//=postfix+prefix+cn;
	an+=postfix; an+=prefix; an+=cn;
	printf("  -> Splitted Part OUT: %-s\n",  an.ToCString());
	cnt++;
      }

      const TopTools_ListOfShape& aListOfShapeOn=aShapeWithState.Part(TopAbs_ON);
      anIt.Initialize (aListOfShapeOn);
      for (;anIt.More(); anIt.Next()) {
	const TopoDS_Shape& aS=anIt.Value();

	TCollection_AsciiString cn(cnt), prefix("_S_"), sn;//=aFName+prefix+cn;
	sn+=aFName; sn+=prefix; sn+=cn;
	BRepTools::Write (aS, sn.ToCString());

	TCollection_AsciiString an;//=postfix+prefix+cn;
	an+=postfix; an+=prefix; an+=cn;
	printf("  -> Splitted Part ON : %s\n",  an.ToCString());
	cnt++;
      } 
    }
  
  }
  cnt=0;
}

} // anonymous namespace
*/

//=======================================================================
//function : PerformShapeWithStates
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder1::PerformShapeWithStates()
{
  theUsedVertexMap.Clear();
  theUnkStateVer.Clear();
  myDataStructure -> ChangeDS().ChangeMapOfShapeWithStateObj().Clear();
  myDataStructure -> ChangeDS().ChangeMapOfShapeWithStateTool().Clear();
    //modified by NIZHNY-MZV  Mon Feb 21 13:30:05 2000
  //process section curves 
  Standard_Integer i, nbC = myDataStructure -> DS().NbCurves();
  for(i = 1; i <= nbC; i++) {
    TopTools_ListOfShape& LSE = ChangeNewEdges(i);
    TopTools_ListIteratorOfListOfShape it(LSE);
    for(; it.More(); it.Next())  {
      const TopoDS_Shape& E = it.Value();
      TopoDS_Vertex Vf, Vl;
      TopExp::Vertices(TopoDS::Edge(E), Vf, Vl);
      theUsedVertexMap.Add(Vf);
      theUsedVertexMap.Add(Vl);
    }
  }
  
  //process section edges
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  Standard_Integer n = BDS.NbSectionEdges();
  for (i = 1; i <= n; i++) { 
    TopTools_ListIteratorOfListOfShape anIt;
    const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i));
    if(E.IsNull()) continue;
    
    const TopTools_ListOfShape& SplitsON = Splits(E, TopAbs_ON);
    anIt.Initialize (SplitsON);
    for (; anIt.More(); anIt.Next()) {
      TopoDS_Shape aNewEdge=anIt.Value();
      TopoDS_Vertex Vf, Vl;
      TopExp::Vertices(TopoDS::Edge(aNewEdge), Vf, Vl);
      theUsedVertexMap.Add(Vf);
      theUsedVertexMap.Add(Vl);
    }
      
      // IN
    const TopTools_ListOfShape& SplitsIN = Splits(E, TopAbs_IN);
    anIt.Initialize (SplitsIN);
    for (; anIt.More(); anIt.Next()) {
      TopoDS_Shape aNewEdge=anIt.Value();
      TopoDS_Vertex Vf, Vl;
      TopExp::Vertices(TopoDS::Edge(aNewEdge), Vf, Vl);
      theUsedVertexMap.Add(Vf);
      theUsedVertexMap.Add(Vl);
    }
      
    // OUT
    const TopTools_ListOfShape& SplitsOUT = Splits(E, TopAbs_OUT);
    anIt.Initialize (SplitsOUT);
    for (; anIt.More(); anIt.Next()) {
      TopoDS_Shape aNewEdge=anIt.Value();
      TopoDS_Vertex Vf, Vl;
      TopExp::Vertices(TopoDS::Edge(aNewEdge), Vf, Vl);
      theUsedVertexMap.Add(Vf);
      theUsedVertexMap.Add(Vl);
    } 
  } 

  //modified by NIZHNY-MZV  Tue Apr 11 17:32:05 2000
  //1) Add both arguments to facilitate the search
  TopOpeBRepDS_ShapeWithState aShapeWithState;
  TopOpeBRepDS_DataStructure& aDataStructure=myDataStructure->ChangeDS();
  
  TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithStateObj=
    aDataStructure.ChangeMapOfShapeWithStateObj();
  TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithStateTool=
    aDataStructure.ChangeMapOfShapeWithStateTool();

  aMapOfShapeWithStateObj.Add(myShape1, aShapeWithState);
  aMapOfShapeWithStateTool.Add(myShape2, aShapeWithState);

  //2) Add all rejected shapes as OUT

  TopTools_IndexedMapOfShape& aMapOfRejectedShapesObj=
    aDataStructure.ChangeMapOfRejectedShapesObj();
  TopTools_IndexedMapOfShape& aMapOfRejectedShapesTool=
    aDataStructure.ChangeMapOfRejectedShapesTool();

  aShapeWithState.SetIsSplitted (Standard_False);
  aShapeWithState.SetState (TopAbs_OUT);
 
  Standard_Integer iW, j, nW, nE, 
                   nRSObj = aMapOfRejectedShapesObj.Extent(), 
                   nRSTool = aMapOfRejectedShapesTool.Extent();
  
  for(i = 1; i <= nRSObj; i++) {
    const TopoDS_Shape& aFace = aMapOfRejectedShapesObj(i);
    if(aFace.ShapeType() != TopAbs_FACE)
      continue; 
    TopTools_IndexedMapOfShape aWiresMap;
    
    TopExp::MapShapes (aFace, TopAbs_WIRE, aWiresMap);
    nW=aWiresMap.Extent ();
    for (iW=1; iW<=nW; iW++) {
      const TopoDS_Shape& aWire=aWiresMap(iW);
      //
      TopTools_IndexedMapOfShape anEdgesMap;
      TopExp::MapShapes (aWire, TopAbs_EDGE, anEdgesMap);
      nE=anEdgesMap.Extent ();
      for (j=1; j<=nE; j++) 
	aMapOfShapeWithStateObj.Add(anEdgesMap(j), aShapeWithState); // add edge

      aMapOfShapeWithStateObj.Add(aWire, aShapeWithState); // add wire 
    }
    aMapOfShapeWithStateObj.Add(aFace, aShapeWithState); // add face
  }

  for(i = 1; i <= nRSTool; i++) {
    const TopoDS_Shape& aFace = aMapOfRejectedShapesTool(i);
    //modified by NIZHNY-MZV  Wed Apr  5 10:27:18 2000
    if(aFace.ShapeType() != TopAbs_FACE)
      continue; 
    TopTools_IndexedMapOfShape aWiresMap;
    TopExp::MapShapes (aFace, TopAbs_WIRE, aWiresMap);
    nW=aWiresMap.Extent ();
    for (iW=1; iW<=nW; iW++) {
      const TopoDS_Shape& aWire=aWiresMap(iW);
      //
      TopTools_IndexedMapOfShape anEdgesMap;
      TopExp::MapShapes (aWire, TopAbs_EDGE, anEdgesMap);
      nE=anEdgesMap.Extent ();
      for (j=1; j<=nE; j++) 
	aMapOfShapeWithStateTool.Add(anEdgesMap(j), aShapeWithState); // add edge

      aMapOfShapeWithStateTool.Add(aWire, aShapeWithState); // add wire 
    }
    aMapOfShapeWithStateTool.Add(aFace, aShapeWithState); // add face
  }
 
  PerformShapeWithStates (myShape1, myShape2);
  processedEdges.Clear();
  PerformShapeWithStates (myShape2, myShape1);
  processedEdges.Clear();
  // Print Block
//  printf(" ..::PerformShapeWithStates() [Dump is off]\n");
  
/*  printf(" ..::PerformShapeWithStates() [Dump is on]\n");
  
  TopOpeBRepDS_DataStructure& aDS= myDataStructure-> ChangeDS();
  
  TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithStateObj=
    aDS.ChangeMapOfShapeWithStateObj();

  TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithStateTool=
    aDS.ChangeMapOfShapeWithStateTool();
 
  DumpMapOfShapeWithState(0, aMapOfShapeWithStateObj);
  DumpMapOfShapeWithState(1, aMapOfShapeWithStateTool);
*/  
 
  // Phase#2 Phase ON
//  PerformOn2D ();
}

//=======================================================================
//function :PerformShapeWithStates
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder1::PerformShapeWithStates (const TopoDS_Shape& anObj, 
							const TopoDS_Shape& aReference)
{
  myShapeClassifier.SetReference(aReference);
  TopOpeBRepDS_DataStructure& aDS= myDataStructure-> ChangeDS();
  // Get aMapOfShapeWithState for Obj
  Standard_Boolean aFlag;
  TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithState=
    aDS.ChangeMapOfShapeWithState(anObj, aFlag); 
  if (!aFlag) return;
  //
  Standard_Integer i, j, k, nS, nF, nE;
  
  TopTools_IndexedMapOfShape aFacesMap, aFacesWithInterferencesMap, aFacesToRestMap;
  TopOpeBRepDS_DataMapOfShapeState aSplFacesState;
  
  TopTools_IndexedMapOfShape aShellsMap;
  TopExp::MapShapes(anObj, TopAbs_SHELL, aShellsMap);

  nS=aShellsMap.Extent();
  for (i=1; i<=nS; i++) {
    const TopoDS_Shape& aShell = aShellsMap(i);

    if (aMapOfShapeWithState.Contains (aShell)) continue;

    else  if (!myDataStructure -> HasShape(aShell)) {
      // Shell has no interference. 
      // So, define its state and push into the Map as is.// A.1
      TopOpeBRepBuild_Tools::FindStateThroughVertex (aShell, myShapeClassifier,
                                                     aMapOfShapeWithState, theUnkStateVer);
      continue;
    }

    else {// A.2
      // Shell has interference. Try to separate it into FacesToRest and InterferredFace
      aFacesMap.Clear();
      aFacesWithInterferencesMap.Clear();
      aFacesToRestMap.Clear();
      aSplFacesState.Clear();
      
      TopExp::MapShapes (aShell, TopAbs_FACE, aFacesMap);
      nF=aFacesMap.Extent();
      for (j=1; j<=nF; j++) {
	const TopoDS_Shape& aFace = aFacesMap(j);
	
	if (aMapOfShapeWithState.Contains (aFace)) {
	 
	  // if the face is known, its edges are also known.
	  // We just insert this info. into aSplFacesState in order to 
	  // propagate the state for faces with unknown states.
	  TopTools_IndexedMapOfShape anEdgesMap;
	  TopExp::MapShapes (aFace, TopAbs_EDGE, anEdgesMap);
	  nE=anEdgesMap.Extent();
	  for (k=1; k<=nE; k++) {
	    const TopoDS_Shape& anEdge=anEdgesMap(k);
	    const TopOpeBRepDS_ShapeWithState& aSWS=
	      aMapOfShapeWithState.FindFromKey(anEdge);
	    TopAbs_State aState=aSWS.State();
	    aSplFacesState.Bind (anEdge, aState);
	  }
	  continue;
	} 
	else if (myDataStructure -> HasShape(aFace)) 	
	  aFacesWithInterferencesMap.Add (aFace);  
	else {	
	  aFacesToRestMap.Add (aFace);
	}
      } // ... next Face
      // work with aFacesWithInterferencesMap
      PerformFacesWithStates (anObj, aFacesWithInterferencesMap, aSplFacesState);
			      
      // Propagate the States  for all unknown faces from aFacesToRestMap  
      TopTools_MapOfShape anUnkStateEdge;
      TopOpeBRepBuild_Tools::PropagateState (aSplFacesState,aFacesToRestMap,
					     TopAbs_EDGE, TopAbs_FACE, myShapeClassifier,
                                             aMapOfShapeWithState, anUnkStateEdge);
      ///// Propagate on WIres from aFacesToRestMap  
      TopOpeBRepBuild_Tools::PropagateStateForWires (aFacesToRestMap, aMapOfShapeWithState);
    } // end of else A.2
  } // next Shell 
}

//=======================================================================
//function :PerformFacesWithStates
//purpose  :
//=======================================================================
  void TopOpeBRepBuild_Builder1::PerformFacesWithStates (const TopoDS_Shape& anObj,
							const TopTools_IndexedMapOfShape& aFacesWithInterferencesMap,
							TopOpeBRepDS_DataMapOfShapeState& aSplFacesState)
{
  TopOpeBRepDS_DataStructure& aDS= myDataStructure-> ChangeDS();
  // Get aMapOfShapeWithState for Obj
  Standard_Boolean aFlag;
  TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithState=aDS.ChangeMapOfShapeWithState(anObj, aFlag); 
  if (!aFlag) return;
  //

  Standard_Integer i, j, k, nF, nW, nE;
  
  nF=aFacesWithInterferencesMap.Extent();
  
  for (i=1; i<=nF; i++) {
    TopTools_IndexedMapOfShape anEdgesToSplitMap, anEdgesToRestMap;
    
    const TopoDS_Shape& aFace = aFacesWithInterferencesMap(i);
    
    TopTools_IndexedMapOfShape aWireMap;
    TopExp::MapShapes (aFace, TopAbs_WIRE, aWireMap);
    nW=aWireMap.Extent();
    for (j=1; j<=nW; j++) {
      const TopoDS_Shape& aWire=aWireMap(j);
      
      if (!myDataStructure -> HasShape(aWire)) {
	// Wire has no interference. 
	// So, define its state and push into the Map as is.
	TopOpeBRepBuild_Tools::FindStateThroughVertex (aWire, myShapeClassifier,
                                                       aMapOfShapeWithState, theUnkStateVer);
	continue;
      }
      
      else {
	// Wire has an interferences 
	TopTools_IndexedMapOfShape anEdgeMap;
	TopExp::MapShapes (aWire, TopAbs_EDGE, anEdgeMap);
	nE=anEdgeMap.Extent ();
	for (k=1; k<=nE; k++) {
	  const TopoDS_Shape& anEdge=anEdgeMap(k);

	  if (myDataStructure -> HasShape(anEdge)) {
	    anEdgesToSplitMap.Add(anEdge);
	  }
	  else {
	    anEdgesToRestMap.Add(anEdge);
	  }
	}
	
	// split edges and define the states for all edges and parts of edges
	StatusEdgesToSplit (anObj, anEdgesToSplitMap, anEdgesToRestMap);
      
	////// After StatusEdgesToSplit we can find  the status of each Rest Edge
	////// in aMapOfShapeWithState. So we can insert this info. into 
	////// aSplFacesState in order to propagate the state for faces.
	nE=anEdgesToRestMap.Extent();
	for (k=1; k<=nE; k++) {
	  const TopoDS_Shape anEdge=anEdgesToRestMap(k);
	  if (aMapOfShapeWithState.Contains (anEdge)) {
	    const TopOpeBRepDS_ShapeWithState& aSWS=aMapOfShapeWithState.FindFromKey(anEdge);
	    TopAbs_State aState=aSWS.State();
	    aSplFacesState.Bind (anEdge, aState);  
	  }
	}
      } //end of else {// Wire has an interferences 
    } // next Wire
  } // next interferred Face ... for (i=1; i<=nF; i++) ...
}

//=======================================================================
//function :StatusEdgesToSplit
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder1::StatusEdgesToSplit (const TopoDS_Shape& anObj,
						    const TopTools_IndexedMapOfShape& anEdgesToSplitMap,
						    const TopTools_IndexedMapOfShape& anEdgesToRestMap)
{
 

  TopOpeBRepDS_DataStructure& aDS= myDataStructure-> ChangeDS();
  // Get aMapOfShapeWithState for Obj
  Standard_Boolean aFlag;
  TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithState=aDS.ChangeMapOfShapeWithState(anObj, aFlag); 
  if (!aFlag) return;
  //

  Standard_Integer i, nE=anEdgesToSplitMap.Extent();
  if (!nE) return;

  TopOpeBRepDS_DataMapOfShapeState aSplEdgesState;
  TopTools_ListIteratorOfListOfShape anIt;
  TopAbs_State aState;
 
  for (i=1; i<=nE; i++) {
    const TopoDS_Shape& anEdge=anEdgesToSplitMap(i);
    
    if(processedEdges.Contains(anEdge)) {
      if (aMapOfShapeWithState.Contains(anEdge)) {
	const TopOpeBRepDS_ShapeWithState& aSWS=
	  aMapOfShapeWithState.FindFromKey(anEdge);
	if (aSWS.IsSplitted()) {

	  const TopTools_ListOfShape& SplitsON=aSWS.Part(TopAbs_ON);
	  anIt.Initialize (SplitsON);
	  for (; anIt.More(); anIt.Next()) 
	    aSplEdgesState.Bind(anIt.Value(), TopAbs_ON);
	  
	  const TopTools_ListOfShape& SplitsOUT=aSWS.Part(TopAbs_OUT);
	  anIt.Initialize (SplitsOUT);
	  for (; anIt.More(); anIt.Next()) 
	    aSplEdgesState.Bind(anIt.Value(), TopAbs_OUT);
	  
	  const TopTools_ListOfShape& SplitsIN=aSWS.Part(TopAbs_IN);
	  anIt.Initialize (SplitsIN);
	  for (; anIt.More(); anIt.Next()) 
	    aSplEdgesState.Bind(anIt.Value(), TopAbs_IN);
	  
	}
      }
      continue;
    }

    processedEdges.Add(anEdge);
    
    TopOpeBRepDS_ShapeWithState aShapeWithState;
    
    //  if IsSplit - it is the case of edges from SameDomain faces
    Standard_Boolean IsSplitON = IsSplit(anEdge, TopAbs_ON);
    if(IsSplitON) {
      // ON
      const TopTools_ListOfShape& SplitsON = Splits(anEdge, TopAbs_ON);
      anIt.Initialize (SplitsON);
      for (; anIt.More(); anIt.Next()) {
	TopoDS_Shape aNewEdge=anIt.Value();
	aNewEdge.Orientation (anEdge.Orientation());
	aShapeWithState.AddPart (aNewEdge, TopAbs_ON);
	aSplEdgesState.Bind(anIt.Value(), TopAbs_ON);
      }
      
      // IN
      const TopTools_ListOfShape& SplitsIN = Splits(anEdge, TopAbs_IN);
      anIt.Initialize (SplitsIN);
      for (; anIt.More(); anIt.Next()) {
	TopoDS_Shape aNewEdge=anIt.Value();
	aNewEdge.Orientation (anEdge.Orientation());
	aShapeWithState.AddPart (aNewEdge, TopAbs_IN);
	aSplEdgesState.Bind(anIt.Value(), TopAbs_IN);
      }
      
      // OUT
      const TopTools_ListOfShape& SplitsOUT = Splits(anEdge, TopAbs_OUT);
      anIt.Initialize (SplitsOUT);
      for (; anIt.More(); anIt.Next()) {
	TopoDS_Shape aNewEdge=anIt.Value();
	aNewEdge.Orientation (anEdge.Orientation());
	aShapeWithState.AddPart (aNewEdge, TopAbs_OUT);
	aSplEdgesState.Bind(anIt.Value(), TopAbs_OUT);
      }
      
      aShapeWithState.SetIsSplitted(Standard_True);
      aMapOfShapeWithState.Add(anEdge, aShapeWithState);
      continue;
    }
    
    //  Attempt to split the Edge (for all other edges (from non SameDomain Faces))
    TopOpeBRepDS_DataMapOfShapeState aDataMapOfShapeState;
    TopTools_ListOfShape aLNew;

    Standard_Boolean oldState = GLOBAL_faces2d;

    GLOBAL_faces2d = Standard_True;
    SplitEdge (anEdge, aLNew, aDataMapOfShapeState);
    GLOBAL_faces2d = oldState;

    //
    if (!aLNew.Extent()) {
      // * It means that whole Edge is IN (see SplitEdge(...) at line 
      // G1=TopOpeBRepBuild_GTool::GFusSame(tf,tf); Operation  Fuse 
      // loses all parts of the Edge with IN  state, but  we  need 
      // to have all parts. So, we have to rest the Edge as is ...
      // ** But the edge itself will have UNKNOWN state and one split Part with state =IN.
      TopoDS_Vertex Vf, Vl;
      TopExp::Vertices(TopoDS::Edge(anEdge), Vf, Vl);
      
      Standard_Boolean HasSDV1 = myDataStructure->HasSameDomain(Vf);
      Standard_Boolean HasSDV2 = myDataStructure->HasSameDomain(Vl);

      TopoDS_Edge aNewEdge = TopoDS::Edge(anEdge);

      //if edge has SD edges , it is error because it must be processed in SplitSectionEdges
      //but if we here we don't do anything with it
      if(myDataStructure -> HasSameDomain(aNewEdge)) {
	HasSDV1 = Standard_False;
	HasSDV2 = Standard_False;
      }
      //if vertices has SD we must update edge, so we copy it
      if(HasSDV1 || HasSDV2) {
	TopoDS_Shape EOR = anEdge; 
	EOR.Orientation(TopAbs_FORWARD);

	Standard_Real ParF = BRep_Tool::Parameter(Vf, TopoDS::Edge(EOR));
	Standard_Real ParL = BRep_Tool::Parameter(Vl, TopoDS::Edge(EOR));
	myBuildTool.CopyEdge (EOR, aNewEdge);

	if (HasSDV1) { // on prend le vertex reference de V
	  Standard_Integer iref = myDataStructure->SameDomainReference(Vf);
	  Vf = TopoDS::Vertex(myDataStructure->Shape(iref));
	  Vf.Orientation(TopAbs_FORWARD);
	}
     
	if (HasSDV2) { // on prend le vertex reference de V
	  Standard_Integer iref = myDataStructure->SameDomainReference(Vl);
	  Vl = TopoDS::Vertex(myDataStructure->Shape(iref));
	  Vl.Orientation(TopAbs_REVERSED);
	}
      
	myBuildTool.AddEdgeVertex (aNewEdge, Vf);
	myBuildTool.Parameter     (aNewEdge, Vf, ParF);
	
	myBuildTool.AddEdgeVertex (aNewEdge, Vl);
	myBuildTool.Parameter     (aNewEdge, Vl, ParL);
	
	aNewEdge.Orientation (anEdge.Orientation()); 
      }

      aState= ClassifyEdgeToSolidByOnePoint(aNewEdge, myShapeClassifier);
      aShapeWithState.SetIsSplitted (Standard_True);

      aShapeWithState.AddPart (aNewEdge, aState);
      aSplEdgesState.Bind(aNewEdge, aState);

      TopExp::Vertices(aNewEdge, Vf, Vl);
      theUsedVertexMap.Add(Vf);
      theUsedVertexMap.Add(Vl);
      if (!BRep_Tool::Degenerated(TopoDS::Edge(aNewEdge))) {
        // MSV: it may be the case when an edge has one state but its vertex
        //      has another state. We should clarify this to avoid incorrect
        //      propagation of state.
        myShapeClassifier.StateP3DReference(BRep_Tool::Pnt(Vf));
        if (myShapeClassifier.State() != aState && myShapeClassifier.State() != TopAbs_ON)
          theUnkStateVer.Add(Vf);
        if (!Vf.IsSame(Vl)) {
          myShapeClassifier.StateP3DReference(BRep_Tool::Pnt(Vl));
          if (myShapeClassifier.State() != aState && myShapeClassifier.State() != TopAbs_ON)
            theUnkStateVer.Add(Vl);
        }
      }
    }
    else {
      // Usual case. The Edge was splitted onto several parts: 
      TopTools_ListIteratorOfListOfShape aLIt(aLNew);
      for (; aLIt.More(); aLIt.Next()) {
	const TopoDS_Shape& aS = aLIt.Value();
	aState = aDataMapOfShapeState(aS);
	////////////////////////////////////////////////////////////////////////////
	// **  When aState==TopAbs_IN it is not evidence that it is realy so.
	// There are some cases when JYL does not define ON parts completely.
	// So,  as we want to have right states,  we have to do it ourselves.  
	// PKV Mon 25 Oct 1999
	Standard_Boolean isdegen = BRep_Tool::Degenerated(TopoDS::Edge(aS));
	//if edge is degenerated we trust that it have IN state without classify 

	if (aState==TopAbs_IN && !isdegen)  
	  aState= ClassifyEdgeToSolidByOnePoint(TopoDS::Edge(aS), myShapeClassifier);

	////////////////////////////////////////////////////////////////////////////
	aShapeWithState.AddPart (aS, aState);
	aShapeWithState.SetIsSplitted (Standard_True);
	  
	aSplEdgesState.Bind(aS, aState);
	TopoDS_Vertex Vf, Vl;
	TopExp::Vertices(TopoDS::Edge(aS), Vf, Vl);
	theUsedVertexMap.Add(Vf);
	theUsedVertexMap.Add(Vl);
        if (!isdegen) {
          // MSV: clarify state of vertices (see my above comment)
          myShapeClassifier.StateP3DReference(BRep_Tool::Pnt(Vf));
          if (myShapeClassifier.State() != aState && myShapeClassifier.State() != TopAbs_ON)
            theUnkStateVer.Add(Vf);
          if (!Vf.IsSame(Vl)) {
            myShapeClassifier.StateP3DReference(BRep_Tool::Pnt(Vl));
            if (myShapeClassifier.State() != aState && myShapeClassifier.State() != TopAbs_ON)
              theUnkStateVer.Add(Vl);
          }
        }
      }
    }

    const TopTools_ListOfShape& EspON = aShapeWithState.Part(TopAbs_ON);

    Standard_Integer nON = EspON.Extent();
    if(!IsSplitON  && nON) {
      TopOpeBRepDS_ListOfShapeOn1State ONspl;
      TopTools_ListOfShape& lON = ONspl.ChangeListOnState();
      lON.Assign(EspON);
      ONspl.Split(Standard_True);
      mySplitON.Bind(anEdge, ONspl);
      myDataStructure -> ChangeDS().AddSectionEdge(TopoDS::Edge(anEdge)); 
    }

    aMapOfShapeWithState.Add(anEdge, aShapeWithState);
  } // end  for (i=1; i<=nE; i++) 

  nE=anEdgesToRestMap.Extent();
  for (i=1; i<=nE; i++) {
    const TopoDS_Shape& anEdge=anEdgesToRestMap.FindKey(i);
    if (aMapOfShapeWithState.Contains (anEdge)) {
      const TopOpeBRepDS_ShapeWithState& aSWS= 
	aMapOfShapeWithState.FindFromKey(anEdge);
      if (!aSWS.IsSplitted()) {
	// just in case
	aState=aSWS.State();
	aSplEdgesState.Bind (anEdge, aState);
	continue;
      }
    }
  }

  if (nE)
    //  Propagate the status for anEdgesToRestMap edges 
    TopOpeBRepBuild_Tools::PropagateState (aSplEdgesState, anEdgesToRestMap,
                                           TopAbs_VERTEX,  TopAbs_EDGE, myShapeClassifier,
                                           aMapOfShapeWithState, theUnkStateVer);

}



//=======================================================================
//function : SplitEdge
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder1::SplitEdge (const TopoDS_Shape& anEdge, 
                                           TopTools_ListOfShape& aLNew,
					   TopOpeBRepDS_DataMapOfShapeState& aDataMapOfShapeState)
{
  Standard_Real aPar1, aPar2;
  TopAbs_Orientation anOr1, anOr2;

  // Attention! If you didn't do the orientation of the Edge =FORWARD,
  // the GFillPointTopologyPVS() method will give you a garbage!  
  TopoDS_Shape EdgeF=anEdge; 
  EdgeF.Orientation(TopAbs_FORWARD);
  
  // Make a PaveSet PVS on edge EF
  TopOpeBRepBuild_PaveSet PVS (EdgeF);
  TopOpeBRepBuild_GTopo G1;
  TopAbs_ShapeEnum tf = TopAbs_FACE;
  G1=TopOpeBRepBuild_GTool::GFusSame(tf,tf); 
  myEdgeReference = TopoDS::Edge(EdgeF);

  GFillPointTopologyPVS(EdgeF, G1, PVS);

  PVS.InitLoop();

  //firstly we detect paves with equal params

  // MSV Oct 23, 2001:
  //  Add Paves to a standard list rather than to a PaveSet to avoid
  //  possible sequence disturbance in InitLoop.
  //  Check points for equality in both 3d and 1d spaces using maximum 
  //  of tolerances of the edge and compared vertices, in 1d using resolution
  //  on edge from that value

  TopOpeBRepBuild_ListOfPave aPVSlist;
  TopTools_DataMapOfShapeListOfInteger aVerOriMap;

  BRepAdaptor_Curve aCurveAdaptor(TopoDS::Edge(anEdge));
  Standard_Real tolEdge = BRep_Tool::Tolerance(TopoDS::Edge(anEdge));

  while (PVS.MoreLoop()) {
    Handle(TopOpeBRepBuild_Pave) aPave1=Handle(TopOpeBRepBuild_Pave)::DownCast(PVS.Loop());
    const TopoDS_Vertex& aV1= TopoDS::Vertex(aPave1->Vertex());
    aPar1    = aPave1->Parameter();
    
    PVS.NextLoop();
    if (!PVS.MoreLoop()) {
      aPVSlist.Append(aPave1);
      break;
    }
      
    Handle(TopOpeBRepBuild_Pave) aPave2=Handle(TopOpeBRepBuild_Pave)::DownCast(PVS.Loop());
    const TopoDS_Vertex& aV2= TopoDS::Vertex(aPave2->Vertex());
    aPar2    = aPave2->Parameter();

    Standard_Real tolV1 = BRep_Tool::Tolerance(aV1);
    Standard_Real tolV2 = BRep_Tool::Tolerance(aV2);
    Standard_Real tolMax = Max(tolEdge,Max(tolV1,tolV2));
    Standard_Real resol = aCurveAdaptor.Resolution(tolMax);
    Standard_Real delta = Abs(aPar1 - aPar2);

    if(delta < resol) {
      Standard_Real dist = BRep_Tool::Pnt(aV1).Distance(BRep_Tool::Pnt(aV2));
      if (dist < tolMax || delta < Precision::PConfusion()) {

        TopOpeBRepDS_Kind IntType1 = aPave1 -> InterferenceType();
        Standard_Boolean Int3d1 = (IntType1 == TopOpeBRepDS_FACE);
        Standard_Boolean HasSDV1 = myDataStructure->HasSameDomain(aV1);
        Standard_Boolean HasSDV2 = myDataStructure->HasSameDomain(aV2);
        Standard_Boolean UsedV1 = theUsedVertexMap.Contains(aV1);
        Standard_Boolean UsedV2 = theUsedVertexMap.Contains(aV2);

        Standard_Boolean takeFirst = Standard_True;
        if(HasSDV1)      ;
        else if(HasSDV2) takeFirst = Standard_False;
        else if(UsedV1)  ;
        else if(UsedV2)  takeFirst = Standard_False;
        else if(Int3d1)  ;
        else             takeFirst = Standard_False;
        TopoDS_Shape aVer;
        Standard_Boolean HasSDV;
        TopAbs_Orientation anOriOpp;
        if (takeFirst) {
          aPVSlist.Append(aPave1);
          aVer = aV1; HasSDV = HasSDV1; anOriOpp = aV2.Orientation();
        }
        else {
          aPVSlist.Append(aPave2);
          aVer = aV2; HasSDV = HasSDV2; anOriOpp = aV1.Orientation();
        }

        if (aV1.Orientation() != aV2.Orientation()) {
          // MSV: save orientation of removed vertex
          TColStd_ListOfInteger thelist;
          if (!aVerOriMap.IsBound(aVer)) aVerOriMap.Bind(aVer, thelist);
          TColStd_ListOfInteger& anOriList = aVerOriMap(aVer);
          anOriList.Append(takeFirst);
          anOriList.Append(anOriOpp);
          // mark this vertex as having unknown state
          if (HasSDV) {
            Standard_Integer iref = myDataStructure->SameDomainReference(aVer);
            aVer = myDataStructure->Shape(iref);
          }
          theUnkStateVer.Add(aVer);
        }

        PVS.NextLoop();
        continue;
      }
    }
    aPVSlist.Append(aPave1);
  }

  TopOpeBRepBuild_ListIteratorOfListOfPave aPVSit(aPVSlist);
  while (aPVSit.More()) {
    Handle(TopOpeBRepBuild_Pave) aPave1 = aPVSit.Value();
    TopoDS_Shape aV1= aPave1->Vertex();
    aV1.Orientation(TopAbs_FORWARD);
    aPar1    = aPave1->Parameter();
    anOr1=(aPave1->Vertex()).Orientation();
    if (aVerOriMap.IsBound(aV1)) {
      // MSV: restore orientation of removed vertex
      TColStd_ListOfInteger& anOriList = aVerOriMap(aV1);
      if (!anOriList.IsEmpty()) {
        if (anOriList.First()) {
          TColStd_ListIteratorOfListOfInteger it(anOriList); it.Next();
          anOr1 = (TopAbs_Orientation) it.Value();
        }
        anOriList.RemoveFirst(); anOriList.RemoveFirst();
      }
    }

    aPVSit.Next();

    if (!aPVSit.More()) break;

    Handle(TopOpeBRepBuild_Pave) aPave2 = aPVSit.Value();
    TopoDS_Shape aV2= aPave2->Vertex();
    aV2.Orientation(TopAbs_REVERSED);
    aPar2    = aPave2->Parameter();
    anOr2=(aPave2->Vertex()).Orientation();
    if (aVerOriMap.IsBound(aV2)) {
      TColStd_ListOfInteger& anOriList = aVerOriMap(aV2);
      if (!anOriList.IsEmpty()) {
        if (!anOriList.First()) {
          TColStd_ListIteratorOfListOfInteger it(anOriList); it.Next();
          anOr2 = (TopAbs_Orientation) it.Value();
        }
      }
    }

    // MSV: avoid creation of an edge with invalid range
    if (aPar1 > aPar2) continue;

    Standard_Boolean HasSDV1 = myDataStructure->HasSameDomain(aV1);
    Standard_Boolean HasSDV2 = myDataStructure->HasSameDomain(aV2);
    if (HasSDV1) { // on prend le vertex reference de V
      Standard_Integer iref = myDataStructure->SameDomainReference(aV1);
      aV1 = myDataStructure->Shape(iref);
      aV1.Orientation(TopAbs_FORWARD);
    }

    if (HasSDV2) { // on prend le vertex reference de V
      Standard_Integer iref = myDataStructure->SameDomainReference(aV2);
      aV2 = myDataStructure->Shape(iref);
      aV2.Orientation(TopAbs_REVERSED);
    }

    // Make new edge from EdgeF
    TopoDS_Edge aNewEdge;
    myBuildTool.CopyEdge (EdgeF, aNewEdge);

    myBuildTool.AddEdgeVertex (aNewEdge, aV1);
    myBuildTool.Parameter     (aNewEdge, aV1, aPar1);
    
    myBuildTool.AddEdgeVertex (aNewEdge, aV2);
    myBuildTool.Parameter     (aNewEdge, aV2, aPar2);
    // State of piece
    
    
    TopAbs_State aState=TopAbs_IN;
 
    if (anOr1==TopAbs_FORWARD  && anOr2==TopAbs_REVERSED) aState=TopAbs_OUT;
    if (anOr1==TopAbs_FORWARD  && anOr2==TopAbs_INTERNAL) aState=TopAbs_OUT;
    if (anOr1==TopAbs_INTERNAL && anOr2==TopAbs_REVERSED) aState=TopAbs_OUT;
    ///* Added
    if (anOr1==TopAbs_INTERNAL && anOr2==TopAbs_INTERNAL) aState=TopAbs_OUT;
    //printf(" anOr1=%d, anOr2=%d\n", anOr1, anOr2);

    // set old orientation to new edge;
    aNewEdge.Orientation (anEdge.Orientation()); 
    aLNew.Append(aNewEdge);
    aDataMapOfShapeState.Bind(aNewEdge, aState);
  }
  //GEDBUMakeEdges(EdgeF,EDBU,aListOfShape);
}

static TopAbs_State ClassifyEdgeToSolidByOnePoint(const TopoDS_Edge& E,
						  TopOpeBRepTool_ShapeClassifier& SC)
{
  Standard_Real f2 = 0., l2 = 0., par = 0.;

  Handle(Geom_Curve) C3D = BRep_Tool::Curve(E, f2, l2);
  gp_Pnt aP3d;

  if(C3D.IsNull()) {
    //it means that we are in degenerated edge
    const TopoDS_Vertex& fv = TopExp::FirstVertex(E);
    if(fv.IsNull())
      return TopAbs_UNKNOWN;
    aP3d = BRep_Tool::Pnt(fv);
  }
  else {//usual case
    par = f2*PAR_T + (1 - PAR_T)*l2;
    C3D -> D0(par, aP3d);
  }
    
  SC.StateP3DReference(aP3d);

  return SC.State();
}
