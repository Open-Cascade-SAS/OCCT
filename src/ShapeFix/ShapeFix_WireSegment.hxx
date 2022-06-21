// Created on: 1999-04-27
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeFix_WireSegment_HeaderFile
#define _ShapeFix_WireSegment_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopAbs_Orientation.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Standard_Integer.hxx>
class ShapeExtend_WireData;
class TopoDS_Wire;
class TopoDS_Edge;


//! This class is auxiliary class (data storage) used in ComposeShell.
//! It is intended for representing segment of the wire
//! (or whole wire). The segment itself is represented by
//! ShapeExtend_WireData. In addition, some associated data
//! necessary for computations are stored:
//!
//! * Orientation flag - determines current use of the segment
//! and used for parity checking:
//!
//! TopAbs_FORWARD and TopAbs_REVERSED - says that segment was
//! traversed once in the corresponding direction, and hence
//! it should be traversed once more in opposite direction;
//!
//! TopAbs_EXTERNAL - the segment was not yet traversed in any
//! direction (i.e. not yet used as boundary)
//!
//! TopAbs_INTERNAL - the segment was traversed in both
//! directions and hence is out of further work.
//!
//! Segments of initial bounding wires are created with
//! orientation REVERSED (for outer wire) or FORWARD (for inner
//! wires), and segments of splitting seams - with orientation
//! EXTERNAL.
class ShapeFix_WireSegment 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates empty segment.
  Standard_EXPORT ShapeFix_WireSegment();
  
  //! Creates segment and initializes it with wire and orientation.
  Standard_EXPORT ShapeFix_WireSegment(const Handle(ShapeExtend_WireData)& wire, const TopAbs_Orientation ori = TopAbs_EXTERNAL);
  
  //! Creates segment and initializes it with wire and orientation.
  Standard_EXPORT ShapeFix_WireSegment(const TopoDS_Wire& wire, const TopAbs_Orientation ori = TopAbs_EXTERNAL);
  
  //! Clears all fields.
  Standard_EXPORT void Clear();
  
  //! Loads wire.
  Standard_EXPORT void Load (const Handle(ShapeExtend_WireData)& wire);
  
  //! Returns wire.
  Standard_EXPORT const Handle(ShapeExtend_WireData)& WireData() const;
  
  //! Sets orientation flag.
  Standard_EXPORT void Orientation (const TopAbs_Orientation ori);
  
  //! Returns orientation flag.
  Standard_EXPORT TopAbs_Orientation Orientation() const;
  
  //! Returns first vertex of the first edge in the wire
  //! (no dependance on Orientation()).
  Standard_EXPORT TopoDS_Vertex FirstVertex() const;
  
  //! Returns last vertex of the last edge in the wire
  //! (no dependance on Orientation()).
  Standard_EXPORT TopoDS_Vertex LastVertex() const;
  
  //! Returns True if FirstVertex() == LastVertex()
  Standard_EXPORT Standard_Boolean IsClosed() const;
  
  //! Returns Number of edges in the wire
  Standard_EXPORT Standard_Integer NbEdges() const;
  
  //! Returns edge by given index in the wire
  Standard_EXPORT TopoDS_Edge Edge (const Standard_Integer i) const;
  
  //! Replaces edge at index i by new one.
  Standard_EXPORT void SetEdge (const Standard_Integer i, const TopoDS_Edge& edge);
  
  //! Insert a new edge with index i and implicitly defined
  //! patch indices (indefinite patch).
  //! If i==0, edge is inserted at end of wire.
  Standard_EXPORT void AddEdge (const Standard_Integer i, const TopoDS_Edge& edge);
  
  //! Insert a new edge with index i and explicitly defined
  //! patch indices. If i==0, edge is inserted at end of wire.
  Standard_EXPORT void AddEdge (const Standard_Integer i, const TopoDS_Edge& edge, const Standard_Integer iumin, const Standard_Integer iumax, const Standard_Integer ivmin, const Standard_Integer ivmax);
  
  //! Set patch indices for edge i.
  Standard_EXPORT void SetPatchIndex (const Standard_Integer i, const Standard_Integer iumin, const Standard_Integer iumax, const Standard_Integer ivmin, const Standard_Integer ivmax);
  
  Standard_EXPORT void DefineIUMin (const Standard_Integer i, const Standard_Integer iumin);
  
  Standard_EXPORT void DefineIUMax (const Standard_Integer i, const Standard_Integer iumax);
  
  Standard_EXPORT void DefineIVMin (const Standard_Integer i, const Standard_Integer ivmin);
  
  //! Modify minimal or maximal patch index for edge i.
  //! The corresponding patch index for that edge is modified so
  //! as to satisfy eq. iumin <= myIUMin(i) <= myIUMax(i) <= iumax
  Standard_EXPORT void DefineIVMax (const Standard_Integer i, const Standard_Integer ivmax);
  
  //! Returns patch indices for edge i.
  Standard_EXPORT void GetPatchIndex (const Standard_Integer i, Standard_Integer& iumin, Standard_Integer& iumax, Standard_Integer& ivmin, Standard_Integer& ivmax) const;
  
  //! Checks patch indices for edge i to satisfy equations
  //! IUMin(i) <= IUMax(i) <= IUMin(i)+1
  Standard_EXPORT Standard_Boolean CheckPatchIndex (const Standard_Integer i) const;
  
  Standard_EXPORT void SetVertex (const TopoDS_Vertex& theVertex);
  
  Standard_EXPORT TopoDS_Vertex GetVertex() const;
  
  Standard_EXPORT Standard_Boolean IsVertex() const;




protected:





private:



  Handle(ShapeExtend_WireData) myWire;
  TopoDS_Vertex myVertex;
  TopAbs_Orientation myOrient;
  Handle(TColStd_HSequenceOfInteger) myIUMin;
  Handle(TColStd_HSequenceOfInteger) myIUMax;
  Handle(TColStd_HSequenceOfInteger) myIVMin;
  Handle(TColStd_HSequenceOfInteger) myIVMax;


};







#endif // _ShapeFix_WireSegment_HeaderFile
