// Created on: 1993-07-29
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

#ifndef _BRepBuilderAPI_MakePolygon_HeaderFile
#define _BRepBuilderAPI_MakePolygon_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepLib_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
class gp_Pnt;
class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Wire;


//! Describes functions to build polygonal wires. A
//! polygonal wire can be built from any number of points
//! or vertices, and consists of a sequence of connected
//! rectilinear edges.
//! When a point or vertex is added to the  polygon if
//! it is identic  to the previous  point no  edge  is
//! built. The method added can be used to test it.
//! Construction of a Polygonal Wire
//! You can construct:
//! -   a complete polygonal wire by defining all its points
//! or vertices (limited to four), or
//! -   an empty polygonal wire and add its points or
//! vertices in sequence (unlimited number).
//! A MakePolygon object provides a framework for:
//! -   initializing the construction of a polygonal wire,
//! -   adding points or vertices to the polygonal wire under construction, and
//! -   consulting the result.
class BRepBuilderAPI_MakePolygon  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes an empty polygonal wire, to which points or
  //! vertices are added using the Add function.
  //! As soon as the polygonal wire under construction
  //! contains vertices, it can be consulted using the Wire function.
  Standard_EXPORT BRepBuilderAPI_MakePolygon();
  
  Standard_EXPORT BRepBuilderAPI_MakePolygon(const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT BRepBuilderAPI_MakePolygon(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3, const Standard_Boolean Close = Standard_False);
  
  //! Constructs a polygonal wire from 2, 3 or 4 points. Vertices are
  //! automatically created on the given points. The polygonal wire is
  //! closed if Close is true; otherwise it is open. Further vertices can
  //! be added using the Add function. The polygonal wire under
  //! construction can be consulted at any time by using the Wire function.
  //! Example
  //! //an open polygon from four points
  //! TopoDS_Wire W = BRepBuilderAPI_MakePolygon(P1,P2,P3,P4);
  //! Warning: The process is equivalent to:
  //! - initializing an empty polygonal wire,
  //! - and adding the given points in sequence.
  //! Consequently, be careful when using this function: if the
  //! sequence of points p1 - p2 - p1 is found among the arguments of the
  //! constructor, you will create a polygonal wire with two
  //! consecutive coincident edges.
  Standard_EXPORT BRepBuilderAPI_MakePolygon(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3, const gp_Pnt& P4, const Standard_Boolean Close = Standard_False);
  
  Standard_EXPORT BRepBuilderAPI_MakePolygon(const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepBuilderAPI_MakePolygon(const TopoDS_Vertex& V1, const TopoDS_Vertex& V2, const TopoDS_Vertex& V3, const Standard_Boolean Close = Standard_False);
  
  //! Constructs a polygonal wire from
  //! 2, 3 or 4 vertices. The polygonal wire is closed if Close is true;
  //! otherwise it is open (default value). Further vertices can be
  //! added using the Add function. The polygonal wire under
  //! construction can be consulted at any time by using the Wire function.
  //! Example
  //! //a closed triangle from three vertices
  //! TopoDS_Wire W = BRepBuilderAPI_MakePolygon(V1,V2,V3,Standard_True);
  //! Warning
  //! The process is equivalent to:
  //! -      initializing an empty polygonal wire,
  //! -      then adding the given points in sequence.
  //! So be careful, as when using this function, you could create a
  //! polygonal wire with two consecutive coincident edges if
  //! the sequence of vertices v1 - v2 - v1 is found among the
  //! constructor's arguments.
  Standard_EXPORT BRepBuilderAPI_MakePolygon(const TopoDS_Vertex& V1, const TopoDS_Vertex& V2, const TopoDS_Vertex& V3, const TopoDS_Vertex& V4, const Standard_Boolean Close = Standard_False);
  
  Standard_EXPORT void Add (const gp_Pnt& P);
  

  //! Adds the point P or the vertex V at the end of the
  //! polygonal wire under construction. A vertex is
  //! automatically created on the point P.
  //! Warning
  //! -   When P or V is coincident to the previous vertex,
  //! no edge is built. The method Added can be used to
  //! test for this. Neither P nor V is checked to verify
  //! that it is coincident with another vertex than the last
  //! one, of the polygonal wire under construction. It is
  //! also possible to add vertices on a closed polygon
  //! (built for example by using a constructor which
  //! declares the polygon closed, or after the use of the Close function).
  //! Consequently, be careful using this function: you might create:
  //! -      a polygonal wire with two consecutive coincident edges, or
  //! -      a non manifold polygonal wire.
  //! -      P or V is not checked to verify if it is
  //! coincident with another vertex but the last one, of
  //! the polygonal wire under construction. It is also
  //! possible to add vertices on a closed polygon (built
  //! for example by using a constructor which declares
  //! the polygon closed, or after the use of the Close function).
  //! Consequently, be careful when using this function: you might create:
  //! -   a polygonal wire with two consecutive coincident edges, or
  //! -   a non-manifold polygonal wire.
  Standard_EXPORT void Add (const TopoDS_Vertex& V);
  
  //! Returns true if the last vertex added to the constructed
  //! polygonal wire is not coincident with the previous one.
  Standard_EXPORT Standard_Boolean Added() const;
  
  //! Closes the polygonal wire under construction. Note - this
  //! is equivalent to adding the first vertex to the polygonal
  //! wire under construction.
  Standard_EXPORT void Close();
  
  Standard_EXPORT const TopoDS_Vertex& FirstVertex() const;
  
  //! Returns the first or the last vertex of the polygonal wire under construction.
  //! If the constructed polygonal wire is closed, the first and the last vertices are identical.
  Standard_EXPORT const TopoDS_Vertex& LastVertex() const;
  

  //! Returns true if this algorithm contains a valid polygonal
  //! wire (i.e. if there is at least one edge).
  //! IsDone returns false if fewer than two vertices have
  //! been chained together by this construction algorithm.
  Standard_EXPORT virtual Standard_Boolean IsDone() const Standard_OVERRIDE;
  
  //! Returns the edge built between the last two points or
  //! vertices added to the constructed polygonal wire under construction.
  //! Warning
  //! If there is only one vertex in the polygonal wire, the result is a null edge.
  Standard_EXPORT const TopoDS_Edge& Edge() const;
Standard_EXPORT operator TopoDS_Edge() const;
  

  //! Returns the constructed polygonal wire, or the already
  //! built part of the polygonal wire under construction.
  //! Exceptions
  //! StdFail_NotDone if the wire is not built, i.e. if fewer than
  //! two vertices have been chained together by this construction algorithm.
  Standard_EXPORT const TopoDS_Wire& Wire();
  Standard_EXPORT operator TopoDS_Wire();




protected:





private:



  BRepLib_MakePolygon myMakePolygon;


};







#endif // _BRepBuilderAPI_MakePolygon_HeaderFile
