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
#include <Geom_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <QANewModTopOpe_Glue.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : ProjPointOnEdge
//purpose  : 
//=======================================================================
Standard_Boolean
QANewModTopOpe_Glue::ProjPointOnEdge (const gp_Pnt& thePnt, const TopoDS_Edge& theEdge,
				 Standard_Real& thePar, Standard_Real& theDist)
{
  Standard_Real aParF, aParL;
  Handle(Geom_Curve) aCurv = BRep_Tool::Curve(theEdge, aParF, aParL);
  GeomAPI_ProjectPointOnCurve aProjector(thePnt, aCurv, aParF, aParL);
  if (aProjector.NbPoints() == 0)
    return Standard_False;
  thePar = aProjector.LowerDistanceParameter();
  theDist = aProjector.LowerDistance();
  return Standard_True;
}

//=======================================================================
//function : InsertVertexInEdge
//purpose  : 
//=======================================================================

void
QANewModTopOpe_Glue::InsertVertexInEdge (const TopoDS_Edge& theEdge,
				    const TopoDS_Vertex& theVer,
				    const Standard_Real thePar,
				    TopoDS_Edge& theNewEdge)
{
  BRep_Builder aBld;
  // construct new edge
  TopoDS_Shape aDummy = theEdge.EmptyCopied().Oriented(TopAbs_FORWARD);
  theNewEdge = TopoDS::Edge (aDummy);
  Standard_Real aParF, aParL;
  BRep_Tool::Range (theEdge, aParF, aParL);
  aBld.Range (theNewEdge, aParF, aParL);
  // add old vertices
  TopoDS_Iterator aIterE (theEdge, Standard_False);
  for (; aIterE.More(); aIterE.Next())
    aBld.Add (theNewEdge, aIterE.Value());
  // add new internal vertex
  aBld.Add (theNewEdge, theVer.Oriented(TopAbs_INTERNAL));
  Standard_Real aTol = Max (BRep_Tool::Tolerance(theVer),
			    BRep_Tool::Tolerance(theEdge));
  aBld.UpdateVertex (theVer, thePar, theNewEdge, aTol);
  theNewEdge.Orientation (theEdge.Orientation());
}

//=======================================================================
//function : SplitEdgeByVertex
//purpose  : 
//=======================================================================

void
QANewModTopOpe_Glue::SplitEdgeByVertex (const TopoDS_Edge& theEdge,
				   const TopoDS_Vertex& theVer,
				   const Standard_Real thePar,
				   TopTools_ListOfShape& theListE)
{
  BRep_Builder aBld;
  // construct 2 new edges
  TopoDS_Shape aDummy1 = theEdge.EmptyCopied().Oriented(TopAbs_FORWARD);
  TopoDS_Shape aDummy2 = theEdge.EmptyCopied().Oriented(TopAbs_FORWARD);
  TopoDS_Edge aNewE1 = TopoDS::Edge (aDummy1);
  TopoDS_Edge aNewE2 = TopoDS::Edge (aDummy2);
  Standard_Real aParF, aParL;
  BRep_Tool::Range (theEdge, aParF, aParL);
  aBld.Range (aNewE1, aParF, thePar);
  aBld.Range (aNewE2, thePar, aParL);
  // add old vertices
  TopoDS_Iterator aIterE (theEdge, Standard_False);
  for (; aIterE.More(); aIterE.Next()) {
    const TopoDS_Vertex& aVer = TopoDS::Vertex (aIterE.Value());
    Standard_Real aParV = BRep_Tool::Parameter (aVer, theEdge);
    if (aParV < thePar) aBld.Add (aNewE1, aVer);
    else aBld.Add (aNewE2, aVer);
  }
  // add new vertex
  aBld.Add (aNewE1, theVer.Oriented(TopAbs_REVERSED));
  aBld.Add (aNewE2, theVer.Oriented(TopAbs_FORWARD));
  Standard_Real aTol = Max (BRep_Tool::Tolerance(theVer),
			    BRep_Tool::Tolerance(theEdge));
  aBld.UpdateVertex (theVer, thePar, aNewE1, aTol);
  aBld.UpdateVertex (theVer, thePar, aNewE2, aTol);
  theListE.Append (aNewE1.Oriented(theEdge.Orientation()));
  theListE.Append (aNewE2.Oriented(theEdge.Orientation()));
}

//=======================================================================
//function : CompareVertices
//purpose  : 
//=======================================================================

Standard_Boolean
QANewModTopOpe_Glue::CompareVertices (const TopoDS_Vertex& theV1, const TopoDS_Vertex& theV2,
				 Standard_Real& theDist)
{
  theDist = BRep_Tool::Pnt(theV1).Distance(BRep_Tool::Pnt(theV2));
  if (theDist <= BRep_Tool::Tolerance(theV1) ||
      theDist <= BRep_Tool::Tolerance(theV2))
    return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : FindWireOrUpdateMap
//purpose  : 
//=======================================================================

const TopoDS_Shape&
QANewModTopOpe_Glue::FindWireOrUpdateMap (const TopoDS_Shape& theWire,
		       TopTools_IndexedDataMapOfShapeListOfShape& theMapELW)
{
  TopoDS_Iterator aIterW (theWire);
//  const TopoDS_Shape& aRefEdge = aIterW.Value();
  TopoDS_Shape aRefEdge = aIterW.Value();
  if (theMapELW.Contains (aRefEdge)) {
    // map edges of theWire
    Standard_Integer nbEdges = 0;
    TopTools_MapOfShape aMapE;
    for (; aIterW.More(); aIterW.Next()) {
      aMapE.Add (aIterW.Value());
      nbEdges++;
    }

    // find in the list a wire with the same set of edges
    const TopTools_ListOfShape& aListW = theMapELW.FindFromKey (aRefEdge);
    TopTools_ListIteratorOfListOfShape aIterLW (aListW);
    for (; aIterLW.More(); aIterLW.Next()) {
      const TopoDS_Shape& aWire = aIterLW.Value();
      Standard_Integer nbE = 0;
      for (aIterW.Initialize (aWire); aIterW.More(); aIterW.Next()) {
	nbE++;
	if (nbE > nbEdges || !aMapE.Contains(aIterW.Value())) break;
      }
      if (nbE == nbEdges && !aIterW.More())
	return aWire;	// found
    }
  }

  // remember the edges of this wire in the map for further reference
  for (aIterW.Initialize (theWire); aIterW.More(); aIterW.Next()) {
    const TopoDS_Shape& aEdge = aIterW.Value();
    if (!theMapELW.Contains (aEdge)) {
      // for Mandrake-10 - mkv,02.06.06 - theMapELW.Add (aEdge, TopTools_ListOfShape());
      TopTools_ListOfShape aListOfShape;
      theMapELW.Add (aEdge, aListOfShape);
    }
    theMapELW.ChangeFromKey(aEdge).Append (theWire);
  }
  return theWire;
}
