// Created on: 1994-12-22
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepTools_Quilt_HeaderFile
#define _BRepTools_Quilt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopTools_IndexedDataMapOfShapeShape.hxx>
class TopoDS_Edge;
class TopoDS_Vertex;
class TopoDS_Shape;


//! A Tool to glue faces at common edges and reconstruct shells.
//!
//! The user designate pairs of common edges using the method Bind.
//! One edge is designated as the edge to use in place of the other one
//! (they are supposed to be geometrically confused, but this not checked).
//! They can be of opposite directions, this is specified by the orientations.
//!
//! The user can add shapes with the Add method, all the faces are registered and copies of faces
//! and edges are made to glue at the bound edges.
//!
//! The user can call the Shells methods to compute a compound of shells from the current set of faces.
//!
//! If no binding is made this class can be used to make shell from faces already sharing their edges.
class BRepTools_Quilt 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepTools_Quilt();
  
  //! Binds <Enew> to   be  the  new edge  instead   of
  //! <Eold>.
  //!
  //! The faces  of  the added  shape containing  <Eold>
  //! will be copied to substitute <Eold> by <Enew>.
  //!
  //! The vertices  of   <Eold> will   be bound to   the
  //! vertices of <Enew> with the same orientation.
  //!
  //! If <Eold>  and <Enew>  have different orientations
  //! the curves are considered  to be opposite  and the
  //! pcurves of <Eold>  will be copied  and reversed in
  //! the new faces.
  //!
  //! <Eold> must belong to the next added shape, <Enew> must belong
  //! to a Shape added before.
  Standard_EXPORT void Bind (const TopoDS_Edge& Eold, const TopoDS_Edge& Enew);
  
  //! Binds <VNew> to be a new vertex instead of <Vold>.
  //!
  //! The faces  of  the added  shape containing  <Vold>
  //! will be copied to substitute <Vold> by <Vnew>.
  Standard_EXPORT void Bind (const TopoDS_Vertex& Vold, const TopoDS_Vertex& Vnew);
  
  //! Add   the faces of  <S>  to  the Quilt,  the faces
  //! containing bounded edges are copied.
  Standard_EXPORT void Add (const TopoDS_Shape& S);
  
  //! Returns   True if <S> has   been  copied (<S> is a
  //! vertex, an edge or a face)
  Standard_EXPORT Standard_Boolean IsCopied (const TopoDS_Shape& S) const;
  
  //! Returns the shape substituted to <S> in the Quilt.
  Standard_EXPORT const TopoDS_Shape& Copy (const TopoDS_Shape& S) const;
  
  //! Returns a Compound of shells made from the current
  //! set of faces. The shells will be flagged as closed
  //! or not closed.
  Standard_EXPORT TopoDS_Shape Shells() const;




protected:





private:



  TopTools_IndexedDataMapOfShapeShape myBounds;
  Standard_Boolean hasCopy;


};







#endif // _BRepTools_Quilt_HeaderFile
