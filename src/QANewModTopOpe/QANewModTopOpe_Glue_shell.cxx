// Created on: 2001-01-16
// Created by: Michael SAZONOV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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
#include <BRepAlgo_EdgeConnector.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepFeat_SplitShape.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <QANewModTopOpe_Glue.hxx>
#include <QANewModTopOpe_Tools.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : SplitEdgeComplete
//purpose  : static
//=======================================================================
static void
SplitEdgeComplete (const TopoDS_Edge& theEdge,
		   TopTools_ListOfShape& theListSplits)
{
  struct ParVer {
    Standard_Real myParF, myParL;
    TopoDS_Vertex myVerF, myVerL;
  };
  ParVer *aParVer = new ParVer[theListSplits.Extent()+1];
  TopTools_DataMapOfShapeInteger aMapEdgeIPV;
  TopTools_ListOfShape aListTodo;
  aListTodo = theListSplits;

  // prepare structures aMapEdgeIPV and aParVer

  aListTodo.Prepend(theEdge);
  TopTools_ListIteratorOfListOfShape aIter(aListTodo);
  Standard_Integer iPV;
  for (iPV=0; aIter.More(); aIter.Next(), iPV++) {
    const TopoDS_Edge& aEdge = TopoDS::Edge(aIter.Value());
    TopoDS_Vertex aVer1, aVer2;
    Standard_Real aPar1, aPar2;

    // compute parameters of first and last vertices of aEdge on theEdge

    TopExp::Vertices(aEdge, aVer1, aVer2);

    if(aVer1.IsNull() || aVer2.IsNull()) {
      aListTodo.Remove(aIter);
      TopTools_ListIteratorOfListOfShape aIter1(theListSplits);
      for (; aIter1.More(); aIter1.Next()) {
	if(aEdge.IsSame(aIter1.Value())) {
	  theListSplits.Remove(aIter1);
	  break;
	}
      }
      if(aIter.More()) continue;
      break;
    }

    if (iPV == 0) {
      // the vertices of theEdge
      aPar1 = BRep_Tool::Parameter(aVer1, theEdge);
      aPar2 = BRep_Tool::Parameter(aVer2, theEdge);
    }
    else {
      Standard_Real aDist;
      // the vertices of an edge from the source list
      if (!QANewModTopOpe_Glue::ProjPointOnEdge (BRep_Tool::Pnt(aVer1),
					    theEdge, aPar1, aDist)) {
	// the vertex is out of theEdge => take one from theEdge
	Standard_Real aPar;
	if (QANewModTopOpe_Glue::ProjPointOnEdge (BRep_Tool::Pnt(aParVer[0].myVerF),
					     aEdge, aPar, aDist)) {
	  aVer1 = aParVer[0].myVerF;
	  aPar1 = aParVer[0].myParF;
	}
	else {
	  aVer1 = aParVer[0].myVerL;
	  aPar1 = aParVer[0].myParL;
	}
      }
      if (!QANewModTopOpe_Glue::ProjPointOnEdge (BRep_Tool::Pnt(aVer2),
					    theEdge, aPar2, aDist)) {
	// the vertex is out of theEdge => take one from theEdge
	Standard_Real aPar;
	if (QANewModTopOpe_Glue::ProjPointOnEdge (BRep_Tool::Pnt(aParVer[0].myVerL),
					     aEdge, aPar, aDist)) {
	  aVer2 = aParVer[0].myVerL;
	  aPar2 = aParVer[0].myParL;
	}
	else {
	  aVer2 = aParVer[0].myVerF;
	  aPar2 = aParVer[0].myParF;
	}
      }
    }
    if (aPar1 < aPar2) {
      aParVer[iPV].myParF = aPar1; aParVer[iPV].myVerF = aVer1;
      aParVer[iPV].myParL = aPar2; aParVer[iPV].myVerL = aVer2;
    }
    else {
      aParVer[iPV].myParF = aPar2; aParVer[iPV].myVerF = aVer2;
      aParVer[iPV].myParL = aPar1; aParVer[iPV].myVerL = aVer1;
      if(iPV != 0) {
	TopTools_ListIteratorOfListOfShape aIterSplits(theListSplits);
	for(; aIterSplits.More(); aIterSplits.Next()) {
	  if(aEdge.IsSame(aIterSplits.Value())) {
	    aIterSplits.Value().Reverse();
	  }
	}
      }
    }
    aMapEdgeIPV.Bind(aEdge,iPV);
  }
  aListTodo.RemoveFirst();

  // find holes and make new edges

  BRep_Builder aBld;
  Standard_Integer iPVLast = 0;
  iPV = -1;
  while (iPV != 0) {

    // find the most left edge
    iPV = 0;
    TopTools_ListIteratorOfListOfShape aIterFound;
    Standard_Real aParF = aParVer[0].myParL;
    TopoDS_Vertex aVerF = aParVer[0].myVerL;
    for (aIter.Initialize(aListTodo); aIter.More(); aIter.Next()) {
      const TopoDS_Edge& aEdge = TopoDS::Edge(aIter.Value());
      Standard_Integer i = aMapEdgeIPV(aEdge);
      if (aParVer[i].myParF < aParF) {
	aParF = aParVer[i].myParF;
	aVerF = aParVer[i].myVerF;
	iPV = i;
	aIterFound = aIter;
      }
    }

    // get previous last parameter
    Standard_Real aParPrevL;
    TopoDS_Vertex aVerPrevL;
    if (iPVLast == 0) {
      aParPrevL = aParVer[0].myParF;
      aVerPrevL = aParVer[0].myVerF;
    }
    else {
      aParPrevL = aParVer[iPVLast].myParL;
      aVerPrevL = aParVer[iPVLast].myVerL;
    }

    if (aParF > aParPrevL && !BRepTools::Compare(aVerF, aVerPrevL)) {
      // make new edge to close the hole
      TopoDS_Edge aNewEdge = theEdge;
      aNewEdge.EmptyCopy();
      aNewEdge.Orientation(TopAbs_FORWARD);
      TopoDS_Vertex aV1 = aVerPrevL;
      TopoDS_Vertex aV2 = aVerF;
      aV1.Orientation(TopAbs_FORWARD);
      aV2.Orientation(TopAbs_REVERSED);
      aBld.Add(aNewEdge, aV1);
      aBld.Add(aNewEdge, aV2);
      aBld.UpdateVertex(aV1, aParPrevL, aNewEdge, BRep_Tool::Tolerance(aV1));
      aBld.UpdateVertex(aV2, aParF, aNewEdge, BRep_Tool::Tolerance(aV2));
      theListSplits.Append(aNewEdge);
    }

    iPVLast = iPV;
    if (aIterFound.More()) aListTodo.Remove(aIterFound);
  }

  delete [] aParVer;
}

//=======================================================================
//function : SplitEdge
//purpose  : static
//=======================================================================

static void
SplitEdge (const TopoDS_Edge          &theEdge,
	   const BOPAlgo_PPaveFiller  &thePDSFiller,
	   const TopTools_MapOfShape  &theEdgesValid,
	   const Standard_Boolean      useMap,
	         TopTools_ListOfShape &theListSplits)
{
  const TopAbs_State aStates[3] = {TopAbs_ON, TopAbs_IN, TopAbs_OUT};

  for (Standard_Integer i=0; i < 3; i++) {	// for each state {ON, IN, OUT}
    if (QANewModTopOpe_Tools::IsSplit(thePDSFiller, theEdge, aStates[i])) {
      // get splits according to this state
      // and append them to the substitution list
      TopTools_ListOfShape aListSplits;

      QANewModTopOpe_Tools::Splits(thePDSFiller, theEdge, aStates[i], aListSplits);
      TopTools_ListIteratorOfListOfShape aIterSplits(aListSplits);
      for(; aIterSplits.More(); aIterSplits.Next()) {
	const TopoDS_Shape& aEdge = aIterSplits.Value();
	if (!useMap || theEdgesValid.Contains (aEdge))
	  theListSplits.Append(aEdge.Oriented(TopAbs_FORWARD));
      }
    }
  }

  if (!theListSplits.IsEmpty()) {
    SplitEdgeComplete (theEdge, theListSplits);
  }
}

//=======================================================================
//function : IsEdgeOut
//purpose  : static
//remark   : the edge is expected to have a pcurve on this face
//=======================================================================

static Standard_Boolean
IsEdgeOut (const TopoDS_Edge& theEdge, const TopoDS_Face& theFace)
{
  Standard_Real aParF, aParL, aParM;
  Handle(Geom2d_Curve) aCrv = BRep_Tool::CurveOnSurface(theEdge, theFace, aParF, aParL);
  if (aCrv.IsNull()) return Standard_True;

  Standard_Real aPrec = Precision::PConfusion();
  BRepClass_FaceClassifier aClf;
  gp_Pnt2d aPnt;

  if (!Precision::IsNegativeInfinite(aParF)) {
    //check first point
    aPnt = aCrv->Value(aParF);
    aClf.Perform(theFace, aPnt, aPrec);
    if(aClf.State() == TopAbs_OUT) return Standard_True;
  }

  if (!Precision::IsPositiveInfinite(aParL)) {
    //check last point
    aPnt = aCrv->Value(aParL);
    aClf.Perform(theFace, aPnt, aPrec);
    if(aClf.State() == TopAbs_OUT) return Standard_True;
  }

  //check middle point
  if (!Precision::IsNegativeInfinite(aParF) &&
      !Precision::IsPositiveInfinite(aParL)) {
    aParM = aParF + 0.618 * (aParL - aParF);
  }
  else {
    if (Precision::IsNegativeInfinite(aParF) &&
	Precision::IsPositiveInfinite(aParL))
      aParM = 0.;
    else if (Precision::IsNegativeInfinite(aParF))
      aParM = aParL - 1.;
    else
      aParM = aParF + 1.;
  }
  aPnt = aCrv->Value(aParM);
  aClf.Perform(theFace, aPnt, aPrec);
  if(aClf.State() == TopAbs_OUT) return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : CorrectAncestorsList
//purpose  : static
//=======================================================================

static void
CorrectAncestorsList (const TopoDS_Edge& theEdge, TopTools_ListOfShape& aListF)
{
  // remove duplicates from list,
  // remove faces for which theEdge has the state "OUT"

  // get a middle point on edge
  TopTools_MapOfShape aMapF;
  TopTools_ListIteratorOfListOfShape aIter(aListF);
  while (aIter.More()) {
    const TopoDS_Face& aFace = TopoDS::Face(aIter.Value());
    if (!aMapF.Add(aFace)) {
      // duplicate -> remove
      aListF.Remove(aIter);
      continue;
    }
    if (IsEdgeOut(theEdge, aFace)) {
      // this face is not an ancestor -> remove
      aListF.Remove(aIter);
      continue;
    }
    aIter.Next();
  }
}

//=======================================================================
//function : PerformShell
//purpose  : 
//=======================================================================

void
QANewModTopOpe_Glue::PerformShell() 
{
  TopoDS_Shape& myS1=myArguments.First();
  TopoDS_Shape& myS2=myTools.First();

  Standard_Boolean isSolidShell = Standard_False;
  Standard_Integer i;

  TopTools_MapOfShape anEdges, aCommonEdges, anOldVertices, aGenEdges;
  TopExp_Explorer anExp;

  anExp.Init(myS1, TopAbs_EDGE);
  for(; anExp.More(); anExp.Next()) anEdges.Add(anExp.Current());

  anExp.Init(myS2, TopAbs_EDGE);
  for(; anExp.More(); anExp.Next()) {
    if(anEdges.Contains(anExp.Current())) aCommonEdges.Add(anExp.Current());
  }

  anExp.ReInit();
  for(; anExp.More(); anExp.Next()) anEdges.Add(anExp.Current());

  anExp.Init(myS1, TopAbs_VERTEX);
  for(; anExp.More(); anExp.Next()) anOldVertices.Add(anExp.Current());

  anExp.Init(myS2, TopAbs_VERTEX);
  for(; anExp.More(); anExp.Next()) anOldVertices.Add(anExp.Current());


  // initialization
  TopoDS_Shape aS1, aS2;
  Standard_Boolean aWire1 = Standard_False, aWire2 = Standard_False; 
  anExp.Init(myS1, TopAbs_WIRE, TopAbs_FACE);
  if(anExp.More()) {
    aS1 = myS1;
    aWire1 = Standard_True;
  }
  else {
    anExp.Init(myS1, TopAbs_EDGE, TopAbs_WIRE);
    if(anExp.More()) {
      aS1 = myS1;
      aWire1 = Standard_True;
    }
  }
   
  anExp.Init(myS2, TopAbs_WIRE, TopAbs_FACE);
  if(anExp.More()) {
    aS2 = myS2;
    aWire2 = Standard_True;
  }
  else {
    anExp.Init(myS2, TopAbs_EDGE, TopAbs_WIRE);
    if(anExp.More()) {
      aS2 = myS2;
      aWire2 = Standard_True;
    }
  }
 
  if(aWire1) {
    BRep_Builder aBld;
    myS1.Nullify();
    aBld.MakeCompound(TopoDS::Compound(myS1));
    anExp.Init(aS1, TopAbs_COMPSOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS1, anExp.Current());
    }
    
    anExp.Init(aS1, TopAbs_SOLID, TopAbs_COMPSOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS1, anExp.Current());
    }
    
    anExp.Init(aS1, TopAbs_SHELL, TopAbs_SOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS1, anExp.Current());
    }
    
    anExp.Init(aS1, TopAbs_FACE, TopAbs_SHELL);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS1, anExp.Current());
    }
    
  }

  if(aWire2) {
    BRep_Builder aBld;
    myS2.Nullify();
    aBld.MakeCompound(TopoDS::Compound(myS2));
    anExp.Init(aS2, TopAbs_COMPSOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS2, anExp.Current());
    }
    
    anExp.Init(aS2, TopAbs_SOLID, TopAbs_COMPSOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS2, anExp.Current());
    }
    
    anExp.Init(aS2, TopAbs_SHELL, TopAbs_SOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS2, anExp.Current());
    }
    
    anExp.Init(aS2, TopAbs_FACE, TopAbs_SHELL);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS2, anExp.Current());
    }
    
  }

  Standard_Boolean hasSolid1 = Standard_False;
  Standard_Boolean hasSolid2 = Standard_False;

  anExp.Init(myS1, TopAbs_SOLID);

  if (anExp.More())
    hasSolid1 = Standard_True;

  anExp.Init(myS2, TopAbs_SOLID);

  if (anExp.More())
    hasSolid2 = Standard_True;

  if (hasSolid1 && hasSolid2)
    myOperation = BOPAlgo_FUSE;
  else if (hasSolid1)
    myOperation = BOPAlgo_CUT21;
  else if (hasSolid2)
    myOperation = BOPAlgo_CUT;
  else
    myOperation = BOPAlgo_SECTION;

  BRepAlgoAPI_BooleanOperation::Build();
  if (!BuilderCanWork())
    return;

  if(aWire1) myS1 = aS1;
  if(aWire2) myS2 = aS2;

  TopTools_DataMapOfShapeListOfShape* aMapSEdgeFaces[2] =
    {&myMapSEdgeFaces1, &myMapSEdgeFaces2};
  TopTools_DataMapOfShapeShape* aMapSEdgeCrossFace[2] =
    {&myMapSEdgeCrossFace1, &myMapSEdgeCrossFace2};
  TopTools_MapOfShape aSetFaces[2];
  TopTools_MapOfShape aSetEdges[2];

  // fill myMapGener for new vertices
  TopTools_MapIteratorOfMapOfShape aMapIter(anEdges);
  const TopAbs_State aStates[3] = {TopAbs_ON, TopAbs_IN, TopAbs_OUT};
  for(; aMapIter.More(); aMapIter.Next()) {
    const TopoDS_Shape& aEdge = aMapIter.Key();

    if(aCommonEdges.Contains(aEdge)) continue;

    for (i=0; i < 3; i++) {	// for each state {ON, IN, OUT}
      if (QANewModTopOpe_Tools::IsSplit(myDSFiller, aEdge, aStates[i])) {
	TopTools_ListOfShape aListSplits;

	QANewModTopOpe_Tools::Splits(myDSFiller, aEdge, aStates[i], aListSplits);

	TopTools_ListIteratorOfListOfShape aIterSplits(aListSplits);
	for(; aIterSplits.More(); aIterSplits.Next()) {
	  const TopoDS_Shape& aE = aIterSplits.Value();
	  TopoDS_Iterator aTDSIter(aE);
	  for(; aTDSIter.More(); aTDSIter.Next()) {
	    const TopoDS_Shape& aV = aTDSIter.Value();

	    if(!anOldVertices.Add(aV)) continue;

	    if(!myMapGener.IsBound(aEdge)) {
	      // for Mandrake-10 - mkv,02.06.06 - myMapGener.Bind(aEdge, TopTools_ListOfShape());
	      TopTools_ListOfShape aListOfShape1;
	      myMapGener.Bind(aEdge, aListOfShape1);
	    }

	    myMapGener(aEdge).Append(aV);
	  }
	}
      }
    }
  }	  


  // get list of section edges
  const TopTools_ListOfShape& aListSE = SectionEdges();

  // for each section edge remember the face crossed by the edge
  // and the faces for which the edge coincides with a face's bound
  TopTools_ListIteratorOfListOfShape aIterSE(aListSE);
  Standard_Integer aNbUsedSecEdges = 0;
  for(; aIterSE.More(); aIterSE.Next()) {
    const TopoDS_Edge& aSecEdge = TopoDS::Edge(aIterSE.Value());

    if(!aCommonEdges.Contains(aSecEdge)) {

      aNbUsedSecEdges++;

      TopoDS_Face aFaces[2];
      QANewModTopOpe_Tools::EdgeCurveAncestors(myDSFiller, aSecEdge, aFaces[0],
					  aFaces[1]);

      TopTools_ListOfShape aListF[2], aListE[2];
      QANewModTopOpe_Tools::EdgeSectionAncestors(myDSFiller, aSecEdge, aListF[0],
					    aListF[1], aListE[0], aListE[1]);
      CorrectAncestorsList (aSecEdge, aListF[0]);
      CorrectAncestorsList (aSecEdge, aListF[1]);

      Standard_Integer nbCurveAncestors = 0;
      for (i = 0; i < 2; i++) {
	if (!aListF[i].IsEmpty()) {
	  aMapSEdgeFaces[i]->Bind(aSecEdge, aListF[i]);
	  if (aListE[i].IsEmpty())
	    aMapSEdgeCrossFace[i]->Bind(aSecEdge, aListF[i].First());
	  TopTools_ListIteratorOfListOfShape aIter (aListF[i]);
	  for (; aIter.More(); aIter.Next())
	    aSetFaces[i].Add(aIter.Value());
	}
	else if (!aFaces[i].IsNull()) {
	  TopTools_ListOfShape aList;
	  aList.Append(aFaces[i]);
	  aMapSEdgeFaces[i]->Bind(aSecEdge, aList);
	  aMapSEdgeCrossFace[i]->Bind(aSecEdge, aFaces[i]);
	  aSetFaces[i].Add(aFaces[i]);
	  nbCurveAncestors++;
	}
	else if (!aListE[i].IsEmpty()) {
	  myEdgesToLeave.Add (aSecEdge);
	  TopTools_ListIteratorOfListOfShape aIter (aListE[i]);
	  for (; aIter.More(); aIter.Next())
	    aSetEdges[i].Add(aIter.Value());
	}
      }
      if (nbCurveAncestors == 2) {
	// the edge was computed by intersection of 2 surfaces
	aGenEdges.Add(aSecEdge);
	if (isSolidShell && !myAllowCutting) {
	  // Shell goes inside Solid while it is forbidden
	  return;
	}
	else {
	  // force same parameter
	  const Standard_Real aTol = 1.e-5;
	  BRep_Builder aBld;
	  aBld.SameRange (aSecEdge, Standard_False);
	  aBld.SameParameter (aSecEdge, Standard_False);
	  BRepLib::SameParameter (aSecEdge, aTol);
	}
      }
    }
  }

  //--------------------------------------------------
  if(aNbUsedSecEdges == 0 && aListSE.Extent() != 0) {
    // all section edges are common edges - make compound
    BRep_Builder aBld;
    aBld.MakeCompound (TopoDS::Compound(myShape));
    aBld.Add(myShape, myS1);
    aBld.Add(myShape, myS2);
    Done();
    return;
  }
  //--------------------------------------------------

  // cut faces of shell if another shape is solid
  if (isSolidShell) {
    // split edges of shape by section edges which are got
    // due to coinciding of edges rather than intersecting of faces
    TopTools_MapIteratorOfMapOfShape aIter(aSetEdges[1]);
    for(; aIter.More(); aIter.Next()) {
      const TopoDS_Edge aEdge = TopoDS::Edge (aIter.Key());
      TopTools_ListOfShape aListSplitE;
      SplitEdge (aEdge, myDSFiller, myEdgesToLeave, Standard_False, aListSplitE);
      if (!aListSplitE.IsEmpty()) {
	mySubst.Substitute (aEdge, aListSplitE);
	if (mySubst.IsCopied (aEdge)) {
	  // for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(aEdge, TopTools_ListOfShape());
	  TopTools_ListOfShape aListOfShape2;
	  myMapModif.Bind(aEdge, aListOfShape2);
	  myMapModif(aEdge).Append (aListSplitE);
	}
      }
    }

    for(aIter.Initialize (aSetFaces[1]); aIter.More(); aIter.Next()) {
      const TopoDS_Face aFace = TopoDS::Face (aIter.Key());
      TopTools_ListOfShape aListSEOnFace;

      // select section edges on this face
      for (aIterSE.Initialize(aListSE); aIterSE.More(); aIterSE.Next()) {
	const TopoDS_Edge& aSecEdge = TopoDS::Edge(aIterSE.Value());
	// check if aFace is an ancestor of aSecEdge
	if (aMapSEdgeFaces[1]->IsBound(aSecEdge)) {
	  TopTools_ListIteratorOfListOfShape
	    aIterF (aMapSEdgeFaces[1]->Find(aSecEdge));
	  for (; aIterF.More(); aIterF.Next())
	    if (aIterF.Value().IsSame(aFace)) {
	      aListSEOnFace.Append(aSecEdge);
	      break;
	    }
	}
      }
      if (!aListSEOnFace.IsEmpty()) {
	Standard_Boolean isCut = CutFace (aFace, aListSEOnFace);
	if (isCut && !myAllowCutting) {
	  // Shell goes inside Solid while it is forbidden
	  return;
	}
      }
    }

    // remove from maps all the section edges which are not marked to leave
    for (aIterSE.Initialize (aListSE); aIterSE.More(); aIterSE.Next()) {
      const TopoDS_Shape& aSecEdge = aIterSE.Value();
      if (!myEdgesToLeave.Contains (aSecEdge)) {
	myMapSEdgeFaces1.UnBind (aSecEdge);
	myMapSEdgeFaces2.UnBind (aSecEdge);
	myMapSEdgeCrossFace1.UnBind (aSecEdge);
	myMapSEdgeCrossFace2.UnBind (aSecEdge);
      }
    }
  }
  else {	// not the case Solid-Shell
    for (aIterSE.Initialize (aListSE); aIterSE.More(); aIterSE.Next())
      myEdgesToLeave.Add (aIterSE.Value());
  }

  // process intersected faces
  for (i = 0; i < 2; i++) {
    if (i == 1 && isSolidShell) continue;

    // split edges of shape by section edges which are got
    // due to coinciding of edges rather than intersecting of faces
    TopTools_MapIteratorOfMapOfShape aIter(aSetEdges[i]);
    for(; aIter.More(); aIter.Next()) {
      const TopoDS_Edge aEdge = TopoDS::Edge (aIter.Key());
      TopTools_ListOfShape aListSplitE;
      SplitEdge (aEdge, myDSFiller, myEdgesToLeave, Standard_True, aListSplitE);
      if (!aListSplitE.IsEmpty()) {
	mySubst.Substitute (aEdge, aListSplitE);
	//Substitution of vertices at the ends of aEdge.
	TopoDS_Vertex aV1, aV2;
	TopExp::Vertices(aEdge, aV1, aV2);
	TopTools_ListIteratorOfListOfShape anIter(aListSplitE);
	for(; anIter.More(); anIter.Next()) {
	  const TopoDS_Edge& aSpE = TopoDS::Edge(anIter.Value());
	  TopoDS_Vertex aSpV1, aSpV2;
	  TopExp::Vertices(aSpE, aSpV1, aSpV2);
	  aSpV1.Orientation(TopAbs_FORWARD);
	  aSpV2.Orientation(TopAbs_FORWARD);
	  TopTools_ListOfShape aL;
	  if(BRepTools::Compare(aV1, aSpV1) && (!aV1.IsSame(aSpV1))) {
	    aL.Clear();
	    aL.Append(aSpV1);
	    if(!mySubst.IsCopied (aV1)) {
	      mySubst.Substitute(aV1, aL);
//	      if (mySubst.IsCopied (aV1)) {
		// for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(aV1, TopTools_ListOfShape());
	        TopTools_ListOfShape aListOfShape3;
		myMapModif.Bind(aV1, aListOfShape3);
		myMapModif(aV1).Append (aL);
//	      }
	    }
	  }
	  if(BRepTools::Compare(aV1, aSpV2) && (!aV1.IsSame(aSpV2))) {
	    aL.Clear();
	    aL.Append(aSpV2);
	    if(!mySubst.IsCopied (aV1)) {
	      mySubst.Substitute(aV1, aL);
//	      if (mySubst.IsCopied (aV1)) {
		// for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(aV1, TopTools_ListOfShape());
	        TopTools_ListOfShape aListOfShape4;
		myMapModif.Bind(aV1, aListOfShape4);
		myMapModif(aV1).Append (aL);
//	      }
	    }
	  }
	  if(BRepTools::Compare(aV2, aSpV1) && (!aV2.IsSame(aSpV1))) {
	    aL.Clear();
	    aL.Append(aSpV1);
	    if (!mySubst.IsCopied (aV2)) {
	      mySubst.Substitute(aV2, aL);
//	      if (mySubst.IsCopied (aV2)) {
		// for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(aV2, TopTools_ListOfShape());
	        TopTools_ListOfShape aListOfShape5;
		myMapModif.Bind(aV2, aListOfShape5);
		myMapModif(aV2).Append (aL);
//	      }
	    }
	  }
	  if(BRepTools::Compare(aV2, aSpV2) && (!aV2.IsSame(aSpV2))) {
	    aL.Clear();
	    aL.Append(aSpV2);
	    if (!mySubst.IsCopied (aV2)) {
	      mySubst.Substitute(aV2, aL);
//	      if (mySubst.IsCopied (aV2)) {
		// for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(aV2, TopTools_ListOfShape());
	        TopTools_ListOfShape aListOfShape6;
		myMapModif.Bind(aV2, aListOfShape6);
		myMapModif(aV2).Append (aL);
//	      }
	    }
	  }
	}

	if (mySubst.IsCopied (aEdge)) {
	  // for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(aEdge, TopTools_ListOfShape());
          TopTools_ListOfShape aListOfShape7;
	  myMapModif.Bind(aEdge, aListOfShape7);
	  myMapModif(aEdge).Append (aListSplitE);
	}
      }
    }

    for(aIter.Initialize (aSetFaces[i]); aIter.More(); aIter.Next()) {
      const TopoDS_Face aFace = TopoDS::Face (aIter.Key());
      TopTools_ListOfShape aListSEOnFace;

      // select section edges on this face
      for (aIterSE.Initialize(aListSE); aIterSE.More(); aIterSE.Next()) {
	const TopoDS_Edge& aSecEdge = TopoDS::Edge(aIterSE.Value());
	// check if aFace is an ancestor of aSecEdge
	if (aMapSEdgeFaces[i]->IsBound(aSecEdge)) {
	  TopTools_ListIteratorOfListOfShape
	    aIterF (aMapSEdgeFaces[i]->Find(aSecEdge));
	  for (; aIterF.More(); aIterF.Next())
	    if (aIterF.Value().IsSame(aFace)) {
	      aListSEOnFace.Append(aSecEdge);
	      break;
	    }
	}
      }
      if (!aListSEOnFace.IsEmpty())
	SectionInsideFace (aFace, aListSEOnFace, i, aGenEdges);
    }
  }

  // construct new shapes from myS1 and myS2
  // and result compound
  TopoDS_Shape aNewS[2];
  Standard_Integer nbModified = 0;
  Standard_Integer nbDeleted = 0;
  Standard_Integer iShape = 0;

  for (i = 0; i < 2; i++) {
    const TopoDS_Shape& aOldS = (i==0 ? myS1 : myS2);
    mySubst.Build(aOldS);
    if (mySubst.IsCopied(aOldS)) {
      if (!mySubst.Copy(aOldS).IsEmpty()) {
	aNewS[i] = mySubst.Copy(aOldS).First();
	aNewS[i].Orientation(aOldS.Orientation());
	nbModified++;
	iShape = i;
      }
      else if (!myAllowCutting) {
	// all Shell is inside Solid while it is forbidden
	return;
      }
      else
	nbDeleted++;
    }
    else {
      aNewS[i] = aOldS;
      iShape = i;
    }
    
    TopExp_Explorer aExp (aOldS, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aFace = aExp.Current();
      if(myMapModif.IsBound(aFace)) continue;
      if (mySubst.IsCopied(aFace)) {
	if (!mySubst.Copy(aFace).IsEmpty()) {	
	  myMapModif.Bind(aFace,mySubst.Copy(aFace));
	}
      }
    }
      
  }

  if (nbModified > 0 && nbDeleted == 0) {
    // the usual case
    isSolidShell = Standard_True; 
    BRep_Builder aBld;
    if(isSolidShell) {
      aBld.MakeCompound (TopoDS::Compound(myShape));
      aBld.Add(myShape, aNewS[0]);
      aBld.Add(myShape, aNewS[1]);
    }
    else {
      aBld.MakeShell (TopoDS::Shell(myShape));
      for(anExp.Init(aNewS[0], TopAbs_FACE); anExp.More(); anExp.Next()) {
	aBld.Add(myShape, anExp.Current());
      }
      for(anExp.Init(aNewS[1], TopAbs_FACE); anExp.More(); anExp.Next()) {
	aBld.Add(myShape, anExp.Current());
      }
    }
    Done();
  }
  else if (nbDeleted == 1) {
    // all Shell is inside Solid while it is permitted
    myShape = aNewS[iShape];
    Done();
  } else if (nbModified == 0) {
    // The case if nothing is changed.
    BRep_Builder aBld;

    aBld.MakeCompound (TopoDS::Compound(myShape));
    aBld.Add(myShape, aNewS[0]);
    aBld.Add(myShape, aNewS[1]);
    Done();
  }

  mySubst.Clear();
  TopExp_Explorer aExp (myShape, TopAbs_EDGE);
  Standard_Boolean IsSplit = Standard_False;
  for (; aExp.More(); aExp.Next()) {
    TopoDS_Edge aE = TopoDS::Edge(aExp.Current());
    if(mySubst.IsCopied(aE)) continue;
    aE.Orientation(TopAbs_FORWARD);
    TopTools_ListOfShape aListSplits;
    if(QANewModTopOpe_Tools::SplitE(aE, aListSplits)) {
      if(!IsSplit) IsSplit = Standard_True;

      BRep_Builder aBld;
      Standard_Real aTol = Precision::Confusion();
      TopTools_ListIteratorOfListOfShape anISpl(aListSplits);
      for(; anISpl.More(); anISpl.Next()) {
	const TopoDS_Shape& aSpE = anISpl.Value();
	Standard_Real tol = BRep_Tool::Tolerance(TopoDS::Edge(aSpE));
	if(tol > aTol) {
	  aBld.UpdateEdge(TopoDS::Edge(aSpE), 1.05*tol);
	}
      }
	  
      mySubst.Substitute(aE, aListSplits);
      myMapModif.Bind(aE, aListSplits);
    }
  }

  if(IsSplit) {
    mySubst.Build(myShape);
    if(mySubst.IsCopied(myShape)) {
      aExp.Init(myShape, TopAbs_FACE);
      for(; aExp.More(); aExp.Next()) {
	const TopoDS_Shape& aF = aExp.Current();
	if(mySubst.IsCopied(aF)) {
	  myMapModif.Bind(aF, mySubst.Copy(aF));
	}
      }
      myShape = mySubst.Copy(myShape).First();
    }
  }

}

//=======================================================================
//function : SplitFaceBoundary
//purpose  : static
//=======================================================================

static TopoDS_Face
SplitFaceBoundary (const TopoDS_Face& theFace,
		   BRepTools_Substitution& theSubst,
		   const BOPAlgo_PPaveFiller &thePDSFiller,
		   const TopTools_MapOfShape& theEdgesValid,
		   const Standard_Boolean useMap,
		   TopTools_DataMapOfShapeListOfShape& theMapModif)
{
  // split the face's edges by section edges lying on boundary
  BRepTools_Substitution aLocalSubst;
  TopExp_Explorer aExp (theFace, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aEdge = TopoDS::Edge (aExp.Current());
    if (theSubst.IsCopied (aEdge)) continue;

    TopTools_ListOfShape aListSplitE;
    SplitEdge (aEdge, thePDSFiller, theEdgesValid, useMap, aListSplitE);
    if (aListSplitE.IsEmpty()) continue;

    theSubst.Substitute (aEdge, aListSplitE);
    aLocalSubst.Substitute (aEdge, aListSplitE);
//-------------------------------------------------------------
	//Substitution of vertices at the ends of aEdge.
	TopoDS_Vertex aV1, aV2;
	TopExp::Vertices(aEdge, aV1, aV2);
	TopTools_ListIteratorOfListOfShape anIter(aListSplitE);
	for(; anIter.More(); anIter.Next()) {
	  const TopoDS_Edge& aSpE = TopoDS::Edge(anIter.Value());
	  TopoDS_Vertex aSpV1, aSpV2;
	  TopExp::Vertices(aSpE, aSpV1, aSpV2);
	  aSpV1.Orientation(TopAbs_FORWARD);
	  aSpV2.Orientation(TopAbs_FORWARD);
	  TopTools_ListOfShape aL;
	  if(BRepTools::Compare(aV1, aSpV1) && (!aV1.IsSame(aSpV1))) {
	    aL.Clear();
	    aL.Append(aSpV1);
	    aLocalSubst.Substitute(aV1, aL);
	    theSubst.Substitute(aV1, aL);
	    if (aLocalSubst.IsCopied (aV1)) {
	      // for Mandrake-10 - mkv,02.06.06 - theMapModif.Bind(aV1, TopTools_ListOfShape());
              TopTools_ListOfShape aListOfShape1;
	      theMapModif.Bind(aV1, aListOfShape1);
	      theMapModif(aV1).Append (aL);
	    }
	  }
	  if(BRepTools::Compare(aV1, aSpV2) && (!aV1.IsSame(aSpV2))) {
	    aL.Clear();
	    aL.Append(aSpV2);
	    aLocalSubst.Substitute(aV1, aL);
	    theSubst.Substitute(aV1, aL);
	    if (aLocalSubst.IsCopied (aV1)) {
	      // for Mandrake-10 - mkv,02.06.06 - theMapModif.Bind(aV1, TopTools_ListOfShape());
              TopTools_ListOfShape aListOfShape2;
	      theMapModif.Bind(aV1, aListOfShape2);
	      theMapModif(aV1).Append (aL);
	    }
	  }
	  if(BRepTools::Compare(aV2, aSpV1) && (!aV2.IsSame(aSpV1))) {
	    aL.Clear();
	    aL.Append(aSpV1);
	    aLocalSubst.Substitute(aV2, aL);
	    theSubst.Substitute(aV2, aL);
	    if (aLocalSubst.IsCopied (aV2)) {
	      // for Mandrake-10 - mkv,02.06.06 - theMapModif.Bind(aV2, TopTools_ListOfShape());
              TopTools_ListOfShape aListOfShape3;
	      theMapModif.Bind(aV2, aListOfShape3);
	      theMapModif(aV2).Append (aL);
	    }
	  }
	  if(BRepTools::Compare(aV2, aSpV2) && (!aV2.IsSame(aSpV2))) {
	    aL.Clear();
	    aL.Append(aSpV2);
	    aLocalSubst.Substitute(aV2, aL);
	    theSubst.Substitute(aV2, aL);
	    if (aLocalSubst.IsCopied (aV2)) {
	      // for Mandrake-10 - mkv,02.06.06 - theMapModif.Bind(aV2, TopTools_ListOfShape());
              TopTools_ListOfShape aListOfShape4;
	      theMapModif.Bind(aV2, aListOfShape4);
	      theMapModif(aV2).Append (aL);
	    }
	  }
	}
//-------------------------------------------------------------
    if (aLocalSubst.IsCopied (aEdge)) {
      // for Mandrake-10 - mkv,02.06.06 - theMapModif.Bind(aEdge, TopTools_ListOfShape());
      TopTools_ListOfShape aListOfShape5;
      theMapModif.Bind(aEdge, aListOfShape5);
      theMapModif(aEdge).Append (aListSplitE);
    }
  }

  aLocalSubst.Build (theFace);
  if (aLocalSubst.IsCopied (theFace)) {
//    TopoDS_Iterator aIterF (theFace);
//    for (; aIterF.More(); aIterF.Next()) {
//      const TopoDS_Shape& aWire = aIterF.Value();
//      if (aLocalSubst.IsCopied (aWire))
//	theSubst.Substitute (aWire, aLocalSubst.Copy(aWire));
//    }
    aExp.Init(theFace, TopAbs_EDGE);
    for(; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& anE = aExp.Current();

      if (aLocalSubst.IsCopied (anE)) {
	if(!theSubst.IsCopied (anE)) {
	  theSubst.Substitute (anE, aLocalSubst.Copy(anE));
	  theMapModif.Bind(anE, aLocalSubst.Copy(anE));
	}
      }
    }
    return TopoDS::Face (aLocalSubst.Copy(theFace).First());
  }
  return theFace;
}

//=======================================================================
//function : CutFace
//purpose  : 
//=======================================================================

Standard_Boolean
QANewModTopOpe_Glue::CutFace(const TopoDS_Face& theFace,
			const TopTools_ListOfShape& theListSE)
{
  Standard_Boolean aRetValue = Standard_False;
  if (mySubst.IsCopied(theFace)) return aRetValue;

  // theFace may contain edges which need to be substituted
  mySubst.Build(theFace);
  TopoDS_Face aFace;
  if (mySubst.IsCopied(theFace)) {
    if (mySubst.Copy(theFace).IsEmpty()) return Standard_True;
    aFace = TopoDS::Face(mySubst.Copy(theFace).First());
  }
  else
    aFace = theFace;

  // split the face's edges by section edges lying on boundary
  TopoDS_Face aFace1 = SplitFaceBoundary (aFace, mySubst, myDSFiller,
					  myEdgesToLeave, Standard_False, myMapModif);

  // split face on subfaces by section edges lying inside the face
  BRepFeat_SplitShape aSpliter (aFace1);
  TopTools_ListIteratorOfListOfShape aIterSE (theListSE);
  for (; aIterSE.More(); aIterSE.Next()) {
    const TopoDS_Edge& aSEdge = TopoDS::Edge (aIterSE.Value());
    if (myMapSEdgeCrossFace2.IsBound(aSEdge))
      aSpliter.Add (aSEdge, aFace1);
  }
  aSpliter.Build();
  const TopTools_ListOfShape& aListSplit = aSpliter.Modified(aFace1);

  // get OUT splits and append them to the substitution list
  TopTools_ListOfShape aListToSubst;
  TopTools_ListIteratorOfListOfShape aIter(aListSplit);
  for(; aIter.More(); aIter.Next()) {
    const TopoDS_Face& aFaceNew = TopoDS::Face (aIter.Value());
    TopAbs_State aState = ClassifyFace (aFaceNew, theListSE);
    if (aState == TopAbs_OUT) {
      aListToSubst.Append(aFaceNew.Oriented(TopAbs_FORWARD));
      // remember in the map the section edges to leave
      TopExp_Explorer aExp (aFaceNew, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
	const TopoDS_Shape& aEdge = aExp.Current();
	if (myMapSEdgeFaces2.IsBound (aEdge))
	  myEdgesToLeave.Add (aEdge);
      }
    }
    else {
      aRetValue = Standard_True;
    }
  }
  mySubst.Substitute(aFace, aListToSubst);

  // update history
  if (mySubst.IsCopied(aFace)) {
    // for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(theFace, TopTools_ListOfShape());
    TopTools_ListOfShape aListOfShape;
    myMapModif.Bind(theFace, aListOfShape);
    myMapModif(theFace).Append (aListToSubst);
  }

  return aRetValue;
}

//=======================================================================
//function : GetVecIntoFace
//purpose  : static
//=======================================================================

static Standard_Boolean
GetVecIntoFace (const TopoDS_Face& theFace,
		const TopoDS_Edge& theEdge,
		gp_Pnt& thePntOnEdge,
		gp_Vec& theVecIntoFace)
{
  TopoDS_Shape aDummy = theFace.Oriented(TopAbs_FORWARD);
  TopoDS_Face aFace = TopoDS::Face(aDummy);
  TopoDS_Edge aEdge;  // theEdge extracted from theFace (with orientation)

  TopExp_Explorer aExp (aFace, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    aEdge = TopoDS::Edge (aExp.Current());
    if (aEdge.IsSame(theEdge)) break;
  }
  if (!aExp.More()) return Standard_False;

  TopAbs_Orientation aOrient = aEdge.Orientation();
  if (aOrient != TopAbs_FORWARD && aOrient != TopAbs_REVERSED)
    return Standard_False;

  Standard_Real aParF, aParL;
  Handle(Geom2d_Curve) aCrv = BRep_Tool::CurveOnSurface(aEdge, aFace, aParF, aParL);
  if (aCrv.IsNull()) return Standard_False;
  if (aCrv->Continuity() < GeomAbs_C1) return Standard_False;

  // get middle point on edge and normal
  Standard_Real aParM = aParF + (aParL - aParF) * 0.618;
  gp_Pnt2d aPntOnCrv;
  gp_Vec2d aDeriv;
  aCrv->D1(aParM, aPntOnCrv, aDeriv);
  gp_Vec2d aNormal(-aDeriv.Y(), aDeriv.X());
  aNormal.Normalize();
  if (aOrient == TopAbs_REVERSED) aNormal.Reverse();

  // translate middle point along the normal
  Standard_Real uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(aFace, uMin, uMax, vMin, vMax);
  Standard_Real duv = Min(uMax - uMin, vMax - vMin) * 0.1;
  Standard_Real dtol = BRep_Tool::Tolerance(aEdge) * 100.;
  Standard_Real d = Min(duv, dtol);
  gp_Pnt2d aPntOnSurf = aPntOnCrv.Translated(aNormal * d);

  // get 3d points
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  gp_Pnt aPntOnFace;
  aSurf->D0(aPntOnSurf.X(), aPntOnSurf.Y(), aPntOnFace);
  aSurf->D0(aPntOnCrv.X(), aPntOnCrv.Y(), thePntOnEdge);

  // compute theVecIntoFace
  theVecIntoFace = gp_Vec(thePntOnEdge, aPntOnFace);
  Standard_Real aNorm = theVecIntoFace.Magnitude();
  if (aNorm < Precision::Confusion())
    return Standard_False;
  theVecIntoFace.Divide(aNorm);

  return Standard_True;
}

//=======================================================================
//function : ClassifyFace
//purpose  : 
//=======================================================================

TopAbs_State
QANewModTopOpe_Glue::ClassifyFace(const TopoDS_Face& theFace,
			     const TopTools_ListOfShape& theListSE) const
{
  TopAbs_State aState = TopAbs_UNKNOWN;

  TopTools_ListIteratorOfListOfShape aIterSE (theListSE);
  for (; aIterSE.More(); aIterSE.Next()) {
    const TopoDS_Edge& aEdge = TopoDS::Edge (aIterSE.Value());

    // get a point on edge and a vector directed to inside face
    // relatively that point
    gp_Pnt aPntOnEdge;
    gp_Vec aVecIntoFace;
    if (BRep_Tool::Degenerated(aEdge) ||
	!GetVecIntoFace (theFace, aEdge, aPntOnEdge, aVecIntoFace))
      continue;

    // get faces from solid
    if (!myMapSEdgeFaces1.IsBound(aEdge)) continue;
    TopTools_ListIteratorOfListOfShape aIterF (myMapSEdgeFaces1(aEdge));
    for (; aIterF.More(); aIterF.Next()) {
      const TopoDS_Face& aFaceSol = TopoDS::Face(aIterF.Value());
      TopAbs_Orientation aOrient = aFaceSol.Orientation();
      if (aOrient != TopAbs_FORWARD && aOrient != TopAbs_REVERSED)
	continue;

      // classify theFace relatively aFaceSol

      // get normal to the surface at the point aPntOnEdge
      Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFaceSol);
      GeomAPI_ProjectPointOnSurf aProjector(aPntOnEdge, aSurf);
      if (!aProjector.IsDone()) continue;
      Standard_Real u,v;
      aProjector.LowerDistanceParameters(u,v);
      gp_Vec d1u,d1v;
      gp_Pnt aPntProj;
      aSurf->D1(u, v, aPntProj, d1u, d1v);
      gp_Vec aNormal = d1u.Crossed(d1v);
      if (aOrient == TopAbs_REVERSED) aNormal.Reverse();

      // compare normal and the vector "into face"
      Standard_Real aScalar = aVecIntoFace * aNormal;
      if (aScalar > Precision::Confusion()) {
	aState = TopAbs_OUT;
	break;
      }
      else if (aScalar < -Precision::Confusion())
	aState = TopAbs_IN;
      else
	aState = TopAbs_ON;
    }
    if (aState == TopAbs_IN || aState == TopAbs_ON)
      break;
  }

  return aState;
}

//=======================================================================
//function : IsVertexOnFaceBound
//purpose  : static
//=======================================================================

static Standard_Boolean
IsVertexOnFaceBound (const TopoDS_Vertex& theVer,
		     const TopoDS_Face& theFace,
		     TopoDS_Edge& theEdgeContacted,
		     TopoDS_Vertex& theVerContacted,
		     Standard_Real& thePar,
		     Standard_Real& theDist)
{
  Standard_Real aDist, aPar, aTol2 = 0.;
  theDist = RealLast();
  gp_Pnt aPnt(BRep_Tool::Pnt(theVer));
  Standard_Boolean isContactByVer = Standard_False;

  TopExp_Explorer aExp (theFace.Oriented(TopAbs_FORWARD), TopAbs_EDGE);
  for (; aExp.More() && theDist > Precision::Confusion(); aExp.Next()) {
    const TopoDS_Edge& aEdge = TopoDS::Edge (aExp.Current());

    // first compare by vertices
    TopoDS_Iterator aIter (aEdge, Standard_False);
    for (; aIter.More(); aIter.Next()) {
      const TopoDS_Vertex& aVer = TopoDS::Vertex (aIter.Value());
      if (aVer.IsSame(theVer)) {
	theEdgeContacted = aEdge;
	theVerContacted = aVer;
	return Standard_True;
      }
      if (QANewModTopOpe_Glue::CompareVertices (aVer, theVer, aDist)) {
	if (aDist < theDist) {
	  theEdgeContacted = aEdge;
	  theVerContacted = aVer;
	  theDist = aDist;
	  thePar = BRep_Tool::Parameter (aVer, aEdge);
	  aTol2 = BRep_Tool::Tolerance (aVer);
	  isContactByVer = Standard_True;
	}
      }
    }

    if (!isContactByVer) {
      // project on edge
      if (!BRep_Tool::Degenerated(aEdge)) {
	if (QANewModTopOpe_Glue::ProjPointOnEdge (aPnt, aEdge, aPar, aDist)) {
	  if (aDist < theDist) {
	    theEdgeContacted = aEdge;
	    theVerContacted.Nullify();
	    theDist = aDist;
	    thePar = aPar;
	    aTol2 = BRep_Tool::Tolerance(aEdge);
	  }
	}
      }
    }
  }

  if (theDist <= BRep_Tool::Tolerance(theVer) || theDist <= aTol2)
    return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : UpdateMapNewOld
//purpose  : static
//=======================================================================

static void
UpdateMapNewOld (const TopoDS_Shape& theSh, const BRepTools_Substitution& theSubst,
		 TopTools_DataMapOfShapeShape& theMapNewOld)
{
  TopTools_IndexedMapOfShape aMapSh;
  TopExp::MapShapes (theSh, aMapSh);

  for (Standard_Integer i=1; i<=aMapSh.Extent(); i++) {
    const TopoDS_Shape& aSubSh = aMapSh(i);
    if (!aSubSh.IsSame(theSh) && theSubst.IsCopied (aSubSh)) {
      TopTools_ListIteratorOfListOfShape aIt (theSubst.Copy(aSubSh));
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aNewSubSh = aIt.Value();
	if (theMapNewOld.IsBound(aSubSh)) {
	  TopoDS_Shape aOldSubSh = theMapNewOld(aSubSh);
	  theMapNewOld.UnBind (aSubSh);
	  theMapNewOld.Bind (aNewSubSh, aOldSubSh);
	}
	else {
	  theMapNewOld.Bind (aNewSubSh, aSubSh);
	}
      }
    }
  }
}

//=======================================================================
//function : DoLocalSubstitution
//purpose  : static
//=======================================================================

static void
DoLocalSubstitution (TopoDS_Shape& theSh, const TopoDS_Shape& theSubSh,
		     const TopoDS_Shape& theNewSubSh,
		     TopTools_DataMapOfShapeShape& theMapNewOld)
{
  BRepTools_Substitution aLocalSubst;
  TopTools_ListOfShape aList;
  aList.Append (theNewSubSh.Oriented(TopAbs_FORWARD));
  aLocalSubst.Substitute (theSubSh, aList);
  aLocalSubst.Build(theSh);

  if (aLocalSubst.IsCopied(theSh)) {
    UpdateMapNewOld (theSh, aLocalSubst, theMapNewOld);
    theSh = aLocalSubst.Copy(theSh).First();
  }
}

//=======================================================================
//function : SectionInsideFace
//purpose  : 
//=======================================================================

void
QANewModTopOpe_Glue::SectionInsideFace(const TopoDS_Face& theFace,
				  const TopTools_ListOfShape& theListSE,
				  const Standard_Integer theShapeNum, 
				  const TopTools_MapOfShape& theGenEdges) 
{
  if (mySubst.IsCopied(theFace)) return;

  // theFace may contain edges which need to be substituted
  mySubst.Build(theFace);
  TopoDS_Face aFace;
  if (mySubst.IsCopied(theFace)) {
    if (mySubst.Copy(theFace).IsEmpty()) return;
    aFace = TopoDS::Face(mySubst.Copy(theFace).First());
  }
  else
    aFace = theFace;

  // split the face's edges by section edges lying on boundary
  TopoDS_Face aFace1 = SplitFaceBoundary (aFace, mySubst, myDSFiller,
					  myEdgesToLeave, Standard_True, myMapModif);
  TopTools_DataMapOfShapeShape aMapNewOrig;
  UpdateMapNewOld (theFace, mySubst, aMapNewOrig);

  // process section edges contacting the face boundary:
  // insert internal vertices in the boundary
  const TopTools_DataMapOfShapeShape& aMapEF =
    (theShapeNum==0 ? myMapSEdgeCrossFace1 : myMapSEdgeCrossFace2);
  BRep_Builder aBld;
  TopTools_DataMapOfShapeShape aMapNewOld;
  TopTools_ListIteratorOfListOfShape aIterSE (theListSE);

  TopTools_MapOfShape aVerGener;

  for (; aIterSE.More(); aIterSE.Next()) {
    const TopoDS_Edge& aSEdge = TopoDS::Edge (aIterSE.Value());
    // skip edges lying on the boundary
    if (!aMapEF.IsBound (aSEdge)) continue;

    // check if vertices of aSEdge contacts edges of aFace
    TopoDS_Iterator aIter (aSEdge, Standard_False);
    for (; aIter.More(); aIter.Next()) {
      TopoDS_Vertex aSVer = TopoDS::Vertex (aIter.Value());
      if (aSVer.Orientation() != TopAbs_FORWARD &&
	  aSVer.Orientation() != TopAbs_REVERSED) continue;

      TopoDS_Edge aEdge;
      TopoDS_Vertex aVer;
      Standard_Real aPar=0.0, aDist;
      if (IsVertexOnFaceBound (aSVer, aFace1, aEdge, aVer, aPar, aDist)) {
	// aSVer contacts aFace's boundary

	if (!aVer.IsNull()) { // vertex contacted
	  if (!aVer.IsSame(aSVer)) {
	    // the vertices are coincided but not the same
	    // => substitute aVer with aSVer
	    Standard_Real aTol = Max (BRep_Tool::Tolerance(aSVer),
				      BRep_Tool::Tolerance(aVer) + aDist);
	    TopAbs_Orientation aOri = aVer.Orientation();
	    if (aOri != TopAbs_FORWARD && aOri != TopAbs_REVERSED) {
	      TopoDS_Shape aDummy = aSVer.Oriented(aOri);
	      aBld.UpdateVertex (TopoDS::Vertex (aDummy),
				 aPar, aEdge, aTol);
	    }
	    DoLocalSubstitution (aFace1, aVer, aSVer, aMapNewOld);
	    // update history
	    TopoDS_Shape aOrig = aMapNewOld(aSVer);
	    if (aMapNewOrig.IsBound(aOrig)) aOrig = aMapNewOrig(aOrig);
	    // for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(aOrig, TopTools_ListOfShape());
            TopTools_ListOfShape aListOfShape1;
	    myMapModif.Bind(aOrig, aListOfShape1);
	    myMapModif(aOrig).Append (aSVer);
	    aVerGener.Add(aSVer);
	    continue;
	  }
	}

	else { // contact inside edge
	  // add the vertex as internal to the edge of the face
	  TopoDS_Edge aNewEdge;
	  InsertVertexInEdge (aEdge, aSVer, aPar, aNewEdge);
	  // substitute edge
	  DoLocalSubstitution (aFace1, aEdge, aNewEdge, aMapNewOld);
	  // update history
	  const TopoDS_Shape& aOld = aMapNewOld(aNewEdge);
	  TopoDS_Shape aOrig;
	  if (aMapNewOrig.IsBound(aOld)) {
	    aOrig = aMapNewOrig(aOld);
	    TopTools_ListOfShape& aListModif = myMapModif(aOrig);
	    TopTools_ListIteratorOfListOfShape aIt (aListModif);
	    for (; aIt.More(); aIt.Next())
	      if (aIt.Value().IsSame(aOld)) {
		aListModif.Remove (aIt);
		break;
	      }
	    aListModif.Append (aNewEdge);
	  }
	  else {
	    aOrig = aOld;
	    // for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(aOrig, TopTools_ListOfShape());
            TopTools_ListOfShape aListOfShape2;
	    myMapModif.Bind(aOrig, aListOfShape2);
	    myMapModif(aOrig).Append (aNewEdge);
	  }
	  if (!myMapGener.IsBound (aOrig)) {
	    // for Mandrake-10 - mkv,02.06.06 - myMapGener.Bind(aOrig, TopTools_ListOfShape());
            TopTools_ListOfShape aListOfShape3;
	    myMapGener.Bind(aOrig, aListOfShape3);
          }
	  myMapGener(aOrig).Append (aSVer);
	  aVerGener.Add(aSVer);
	}
      }
    }
  }

  // add the made replacements in the global substitution list
  TopTools_DataMapIteratorOfDataMapOfShapeShape aDIter (aMapNewOld);
  for (; aDIter.More(); aDIter.Next()) {
    TopTools_ListOfShape aList;
    const TopoDS_Shape& aOld = aDIter.Value();
    const TopoDS_Shape& aNew = aDIter.Key();
    aList.Append (aNew);
    mySubst.Substitute (aOld, aList);
  }

  // make wires from section edges
  Handle(BRepAlgo_EdgeConnector) aConnector = new BRepAlgo_EdgeConnector;
  TopTools_ListOfShape aListGener;
  TopoDS_Vertex aV1, aV2;
  for (aIterSE.Initialize (theListSE); aIterSE.More(); aIterSE.Next()) {
    const TopoDS_Edge& aSEdge = TopoDS::Edge (aIterSE.Value());
    if (aMapEF.IsBound (aSEdge)) {
      aConnector->Add (aSEdge);
      aConnector->AddStart (aSEdge);
      if(theGenEdges.Contains(aSEdge)) {
	aListGener.Append (aSEdge);
	TopExp::Vertices(aSEdge, aV1, aV2);
	if(aVerGener.Add(aV1)) aListGener.Append (aV1);
	if(aVerGener.Add(aV2)) aListGener.Append (aV2);
      }
    }
  }
  const TopTools_ListOfShape& aListW = aConnector->MakeBlock();
  if (aConnector->IsDone()) {
    // add new wires to face
    TopoDS_Face aNewFace = aFace1;
    aNewFace.EmptyCopy();
    aNewFace.Orientation(TopAbs_FORWARD);
    aBld.NaturalRestriction (aNewFace, BRep_Tool::NaturalRestriction(aFace1));
    // add old subshapes
    TopoDS_Iterator aIterF (aFace1, Standard_False);
    for (; aIterF.More(); aIterF.Next()) {
      aBld.Add (aNewFace, aIterF.Value());
    }
    // add new wires as internal
    TopTools_ListIteratorOfListOfShape aIterL (aListW);
    for (; aIterL.More(); aIterL.Next()) {
      TopoDS_Shape aWire = aIterL.Value();
      // check if there is a wire containing the same set of edges;
      // in this case use the old wire
      aWire = FindWireOrUpdateMap (aWire, myMapEdgeWires);
      aBld.Add (aNewFace, aWire.Oriented(TopAbs_INTERNAL));
    }
    // substitute face
    TopTools_ListOfShape aList;
    aList.Append(aNewFace);
    mySubst.Substitute(aFace, aList);
    // update history
    // for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(theFace, TopTools_ListOfShape());
    TopTools_ListOfShape aListOfShape4;
    myMapModif.Bind(theFace, aListOfShape4);
    myMapModif(theFace).Append (aList);
    if(!aListGener.IsEmpty()) {
      // for Mandrake-10 - mkv,02.06.06 - myMapGener.Bind(theFace, TopTools_ListOfShape());
      TopTools_ListOfShape aListOfShape5;
      myMapGener.Bind(theFace, aListOfShape5);
      myMapGener(theFace).Append (aListGener);
    }
  }
#ifdef OCCT_DEBUG
  else if (!aListW.IsEmpty()) {
    cout<<"QANewModTopOpe_Glue::SectionInsideFace : can't connect edges"<<endl;
  }
#endif
}
