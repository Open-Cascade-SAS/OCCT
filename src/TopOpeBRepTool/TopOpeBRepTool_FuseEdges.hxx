// Created on: 1998-11-26
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

#ifndef _TopOpeBRepTool_FuseEdges_HeaderFile
#define _TopOpeBRepTool_FuseEdges_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfIntegerListOfShape.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Vertex;
class TopoDS_Edge;


//! This class can detect  vertices in a face that can
//! be considered useless and then perform the fuse of
//! the  edges and remove  the  useless vertices.  By
//! useles vertices,  we mean :
//! * vertices that  have  exactly two connex edges
//! * the edges connex to the vertex must have
//! exactly the same 2 connex faces .
//! * The edges connex to the vertex must have the
//! same geometric support.
class TopOpeBRepTool_FuseEdges 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initialise members  and build  construction of map
  //! of ancestors.
  Standard_EXPORT TopOpeBRepTool_FuseEdges(const TopoDS_Shape& theShape, const Standard_Boolean PerformNow = Standard_False);
  
  //! set edges to avoid being fused
  Standard_EXPORT void AvoidEdges (const TopTools_IndexedMapOfShape& theMapEdg);
  
  //! returns  all the list of edges to be fused
  //! each list of the map represent a set of connex edges
  //! that can be fused.
  Standard_EXPORT void Edges (TopTools_DataMapOfIntegerListOfShape& theMapLstEdg);
  
  //! returns all the fused edges. each integer entry in
  //! the   map  corresponds  to  the  integer   in the
  //! DataMapOfIntegerListOfShape  we    get in  method
  //! Edges.   That is to say, to  the list  of edges in
  //! theMapLstEdg(i) corresponds the resulting edge theMapEdge(i)
  Standard_EXPORT void ResultEdges (TopTools_DataMapOfIntegerShape& theMapEdg);
  
  //! returns the map of modified faces.
  Standard_EXPORT void Faces (TopTools_DataMapOfShapeShape& theMapFac);
  
  //! returns myShape modified with the list of internal
  //! edges removed from it.
  Standard_EXPORT TopoDS_Shape& Shape();
  
  //! returns the number of vertices candidate to be removed
  Standard_EXPORT Standard_Integer NbVertices();
  
  //! Using  map of list of connex  edges, fuse each list to
  //! one edge and then update myShape
  Standard_EXPORT void Perform();




protected:





private:
  
  //! Build the all the lists of edges that are to be fused
  Standard_EXPORT void BuildListEdges();
  
  //! Build result   fused edges according  to  the list
  //! builtin BuildLisEdges
  Standard_EXPORT void BuildListResultEdges();
  
  Standard_EXPORT void BuildListConnexEdge (const TopoDS_Shape& theEdge, TopTools_MapOfShape& theMapUniq, TopTools_ListOfShape& theLstEdg);
  
  Standard_EXPORT Standard_Boolean NextConnexEdge (const TopoDS_Vertex& theVertex, const TopoDS_Shape& theEdge, TopoDS_Shape& theEdgeConnex) const;
  
  Standard_EXPORT Standard_Boolean SameSupport (const TopoDS_Edge& E1, const TopoDS_Edge& E2) const;
  
  Standard_EXPORT Standard_Boolean UpdatePCurve (const TopoDS_Edge& theOldEdge, TopoDS_Edge& theNewEdge, const TopTools_ListOfShape& theLstEdg) const;


  TopoDS_Shape myShape;
  Standard_Boolean myShapeDone;
  Standard_Boolean myEdgesDone;
  Standard_Boolean myResultEdgesDone;
  TopTools_IndexedDataMapOfShapeListOfShape myMapVerLstEdg;
  TopTools_IndexedDataMapOfShapeListOfShape myMapEdgLstFac;
  TopTools_DataMapOfIntegerListOfShape myMapLstEdg;
  TopTools_DataMapOfIntegerShape myMapEdg;
  TopTools_DataMapOfShapeShape myMapFaces;
  Standard_Integer myNbConnexEdge;
  TopTools_IndexedMapOfShape myAvoidEdg;


};







#endif // _TopOpeBRepTool_FuseEdges_HeaderFile
