// Created on: 1993-07-08
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepLib_MakeWire_HeaderFile
#define _BRepLib_MakeWire_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepLib_WireError.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepLib_MakeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_UBTree.hxx>

class TopoDS_Wire;

//! Provides methods to build wires.
//!
//! A wire may be built :
//!
//! * From a single edge.
//!
//! * From a wire and an edge.
//!
//! - A new wire  is created with the edges  of  the
//! wire + the edge.
//!
//! - If the edge is not connected  to the wire the
//! flag NotDone   is set and  the  method Wire will
//! raise an error.
//!
//! - The connection may be :
//!
//! . Through an existing vertex. The edge is shared.
//!
//! . Through a geometric coincidence of vertices.
//! The edge is  copied  and the vertices from the
//! edge are  replaced  by  the vertices from  the
//! wire.
//!
//! . The new edge and the connection vertices are
//! kept by the algorithm.
//!
//! * From 2, 3, 4 edges.
//!
//! - A wire is  created from  the first edge, the
//! following edges are added.
//!
//! * From many edges.
//!
//! - The following syntax may be used :
//!
//! BRepLib_MakeWire MW;
//!
//! // for all the edges ...
//! MW.Add(anEdge);
//!
//! TopoDS_Wire W = MW;

class BRepLib_MakeWire  : public BRepLib_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! NotDone MakeWire.
  Standard_EXPORT BRepLib_MakeWire();
  
  //! Make a Wire from an edge.
  Standard_EXPORT BRepLib_MakeWire(const TopoDS_Edge& E);
  
  //! Make a Wire from two edges.
  Standard_EXPORT BRepLib_MakeWire(const TopoDS_Edge& E1, const TopoDS_Edge& E2);
  
  //! Make a Wire from three edges.
  Standard_EXPORT BRepLib_MakeWire(const TopoDS_Edge& E1, const TopoDS_Edge& E2, const TopoDS_Edge& E3);
  
  //! Make a Wire from four edges.
  Standard_EXPORT BRepLib_MakeWire(const TopoDS_Edge& E1, const TopoDS_Edge& E2, const TopoDS_Edge& E3, const TopoDS_Edge& E4);
  
  //! Make a Wire from a Wire. Useful for adding later.
  Standard_EXPORT BRepLib_MakeWire(const TopoDS_Wire& W);
  
  //! Add an edge to a wire.
  Standard_EXPORT BRepLib_MakeWire(const TopoDS_Wire& W, const TopoDS_Edge& E);
  
  //! Add the edge <E> to the current wire.
  Standard_EXPORT void Add (const TopoDS_Edge& E);
  
  //! Add the edges of <W> to the current wire.
  Standard_EXPORT void Add (const TopoDS_Wire& W);
  
  //! Add the edges of <L> to the current wire.
  //! The edges are not to be consecutive.  But they are
  //! to be all connected geometrically or topologically.
  Standard_EXPORT void Add (const TopTools_ListOfShape& L);
  
  Standard_EXPORT BRepLib_WireError Error() const;
  
  //! Returns the new wire.
  Standard_EXPORT const TopoDS_Wire& Wire();
  Standard_EXPORT operator TopoDS_Wire();
  
  //! Returns the last edge added to the wire.
  Standard_EXPORT const TopoDS_Edge& Edge() const;
  
  //! Returns the last connecting vertex.
  Standard_EXPORT const TopoDS_Vertex& Vertex() const;

private:
  class BRepLib_BndBoxVertexSelector : public NCollection_UBTree <Standard_Integer,Bnd_Box>::Selector
  {
  public:
    BRepLib_BndBoxVertexSelector(const TopTools_IndexedMapOfShape& theMapOfShape)
    : BRepLib_BndBoxVertexSelector::Selector(),
      myMapOfShape (theMapOfShape),
      myTolP(0.0),
      myVInd(0)
    {
    }

    Standard_Boolean Reject (const Bnd_Box& theBox) const
    {
      return theBox.IsOut(myVBox);
    }

    Standard_Boolean Accept (const Standard_Integer& theObj);

    void SetCurrentVertex (const gp_Pnt& theP, Standard_Real theTol, 
                           Standard_Integer theVInd);

    const NCollection_List<Standard_Integer>& GetResultInds () const
    { 
      return myResultInd;
    }

    void ClearResInds()
    { 
      myResultInd.Clear();
    }

  private:

    BRepLib_BndBoxVertexSelector(const BRepLib_BndBoxVertexSelector& );
    BRepLib_BndBoxVertexSelector& operator=(const BRepLib_BndBoxVertexSelector& );

    const TopTools_IndexedMapOfShape& myMapOfShape; //vertices
    gp_Pnt myP;
    Standard_Real myTolP;
    Standard_Integer myVInd;
    Bnd_Box myVBox;
    NCollection_List<Standard_Integer> myResultInd; 
  };

  void CollectCoincidentVertices(const TopTools_ListOfShape& theL,
                                 NCollection_List<NCollection_List<TopoDS_Vertex>>& theGrVL);

  void CreateNewVertices(const NCollection_List<NCollection_List<TopoDS_Vertex>>& theGrVL, 
                         TopTools_DataMapOfShapeShape& theO2NV);

  void CreateNewListOfEdges(const TopTools_ListOfShape& theL,
                            const TopTools_DataMapOfShapeShape& theO2NV,
                            TopTools_ListOfShape& theNewEList);

  void Add(const TopoDS_Edge& E, Standard_Boolean IsCheckGeometryProximity);



protected:



private:

  BRepLib_WireError myError;
  TopoDS_Edge myEdge;
  TopoDS_Vertex myVertex;
  TopTools_IndexedMapOfShape myVertices;
  TopoDS_Vertex FirstVertex;
  TopoDS_Vertex VF;
  TopoDS_Vertex VL;

};







#endif // _BRepLib_MakeWire_HeaderFile
