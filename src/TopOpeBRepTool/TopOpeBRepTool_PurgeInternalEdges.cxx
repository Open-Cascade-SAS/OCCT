// Created on: 1998-11-19
// Created by: Jean-Michel BOULCOURT
// Copyright (c) 1998-1999 Matra Datavision
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


#include <BRepTools_Substitution.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NullObject.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_PurgeInternalEdges.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : TopOpeBRepTool_PurgeInternalEdges
//purpose  : Initializes some variables for the algorithm and perform
// the computation of the internal edges of the shape
//=======================================================================
TopOpeBRepTool_PurgeInternalEdges::TopOpeBRepTool_PurgeInternalEdges(const TopoDS_Shape& theShape,const Standard_Boolean PerformNow):myShape(theShape),myIsDone(Standard_False)
{
//  if (theShape.ShapeType() != TopAbs_SHELL && theShape.ShapeType() != TopAbs_SOLID)
//    throw Standard_ConstructionError("PurgeInternalEdges");
  Standard_NullObject_Raise_if(theShape.IsNull(),"PurgeInternalEdges");
  
  if (PerformNow) {
    Perform();
  }
}

//=======================================================================
//function : Faces
//purpose  : 
//=======================================================================

 void TopOpeBRepTool_PurgeInternalEdges::Faces(TopTools_DataMapOfShapeListOfShape& theMapFacLstEdg) 
{

  if (!myIsDone) {
    BuildList();
  }

  theMapFacLstEdg = myMapFacLstEdg;
}


//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_PurgeInternalEdges::NbEdges() const
{

  Standard_NullObject_Raise_if(myShape.IsNull(),"PurgeInternalEdges : No Shape");
  Standard_Integer nbedges = 0;

  // if we have at least on internal (or external) edge to remove
  if (myMapFacLstEdg.Extent() > 0) {

    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itFacEdg;
//    TopTools_ListIteratorOfListOfShape itEdg;

    for (itFacEdg.Initialize(myMapFacLstEdg); itFacEdg.More(); itFacEdg.Next()) {
      const TopoDS_Shape& facecur = itFacEdg.Key();
      const TopTools_ListOfShape& LmapEdg = myMapFacLstEdg.Find(facecur);
     
      nbedges += LmapEdg.Extent();      
    }
    
  }

  return nbedges;

}




//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape& TopOpeBRepTool_PurgeInternalEdges::Shape() 
{

  Standard_NullObject_Raise_if(myShape.IsNull(),"PurgeInternalEdges : No Shape");

  return myShape;

}

//=======================================================================
//function : BuildList
//purpose  : Build the map of faces with the list of inernal edges.
//=======================================================================

void TopOpeBRepTool_PurgeInternalEdges::BuildList()
{

  TopExp_Explorer ExpEdge;
  TopAbs_Orientation orien;
  

  //--------------------------------------------------------
  // Step One : Build the map of the faces ancestor of edges
  //--------------------------------------------------------
  
  // Clear the maps
  myMapEdgLstFac.Clear();
  
  TopExp::MapShapesAndAncestors(myShape,TopAbs_EDGE,TopAbs_FACE,myMapEdgLstFac);
  
  
  //----------------------------------------------------------------
  // Step Two : Explore the map myMapFacLstEdg to keep only the internal
  // or external edges which have no connex faces
  //----------------------------------------------------------------
  
  
  Standard_Boolean ToKeep;
  Standard_Integer iEdg;
  TopTools_ListIteratorOfListOfShape itFac,itFacToTreat;
  TopTools_ListOfShape LstFacToTreat;
  
  // for each edge of myMapEdgLstFac
  for (iEdg = 1; iEdg <= myMapEdgLstFac.Extent(); iEdg++) {
    const TopoDS_Shape& edgecur = myMapEdgLstFac.FindKey(iEdg);
    const TopTools_ListOfShape& LmapFac = myMapEdgLstFac.FindFromKey(edgecur);
    
    
    // if there are more than on face connex to the edge then examine the orientation of
    // the edge in the faces to see it is only Internal or External. In that case the edge
    // can be removed

    itFac.Initialize(LmapFac);
    LstFacToTreat.Clear();

    if (LmapFac.Extent() > 1) {

      ToKeep = Standard_False;
      // for each face in the list of the edge
      while (itFac.More() && !ToKeep) {
	const TopoDS_Shape& facecur = itFac.Value();
	
	// find the edge in the face to get its relative orientation to the face
	for (ExpEdge.Init(facecur,TopAbs_EDGE); ExpEdge.More(); ExpEdge.Next()) {
	  const TopoDS_Shape& edge = ExpEdge.Current();
	  
	  orien = edge.Orientation();

	  if (edge.IsSame(edgecur)) {
	    // we found the edge. Now check if its orientation is internal or external
	    // then the face is candidate to be modified. So put it in a temporary List of shapes
	    if (orien == TopAbs_INTERNAL || orien == TopAbs_EXTERNAL) {
	      LstFacToTreat.Append(facecur);
	    }
	    else {
	      // there is at least one face that contains that edge with a forward
	      // or reversed orientation. So we must keep that edge.
	      LstFacToTreat.Clear();
	      ToKeep = Standard_True;
	    }
	    break;
	  }
	}
	
	itFac.Next();
      }
      
    }

    else {
       if (edgecur.Orientation() == TopAbs_INTERNAL || edgecur.Orientation() == TopAbs_EXTERNAL) {
	 LstFacToTreat.Append(itFac.Value());
       }
    }
    
    // at that point, we have in the list LstFacToTreat the faces in which edgecur is
    // connex and is coded only Internal or External.
    if (!LstFacToTreat.IsEmpty()) {
      TopTools_MapOfShape mapUniqEdg;
      for (itFacToTreat.Initialize(LstFacToTreat); itFacToTreat.More(); itFacToTreat.Next()) {
	const TopoDS_Shape& face = itFacToTreat.Value();
	
	//we build the resulting map with a face as a key and a list of internal (or external)
	// edges to remove from the face.
	if (!myMapFacLstEdg.IsBound(face)) {
	  TopTools_ListOfShape LmapEdg;
	  if (!mapUniqEdg.Contains(edgecur)) {
	    mapUniqEdg.Add(edgecur);
	    LmapEdg.Append(edgecur);
	    myMapFacLstEdg.Bind(face,LmapEdg);
	  }
	}
	else { // just append the edge to the list of edges of the face
	  TopTools_ListOfShape& LmapEdg = myMapFacLstEdg.ChangeFind(face);
	  if (!mapUniqEdg.Contains(edgecur)) {
	    mapUniqEdg.Add(edgecur);
	    LmapEdg.Append(edgecur);
	  }
	} 
	
      }
    }
    
  } 

  myIsDone = Standard_True;

}



//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

 void TopOpeBRepTool_PurgeInternalEdges::Perform() 
{

  // if the list has not been yet built then do it
  if (!myIsDone) {
    BuildList();
  }

  // if we have at least on internal (or external) edge to remove
  if (myMapFacLstEdg.Extent() > 0) {

    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itFacEdg;
    TopTools_ListIteratorOfListOfShape itEdg;
    TopTools_ListOfShape EmptyList;
    BRepTools_Substitution Bsub;

    for (itFacEdg.Initialize(myMapFacLstEdg); itFacEdg.More(); itFacEdg.Next()) {
      const TopoDS_Shape& facecur = itFacEdg.Key();
      const TopTools_ListOfShape& LmapEdg = myMapFacLstEdg.Find(facecur);
      
      for (itEdg.Initialize(LmapEdg); itEdg.More(); itEdg.Next()) {
	Bsub.Substitute(itEdg.Value(),EmptyList);
      }
    }

    Bsub.Build(myShape);
    if (Bsub.IsCopied(myShape)) {
      myShape=(Bsub.Copy(myShape)).First();
    }
  }
}


