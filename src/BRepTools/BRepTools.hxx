// Created on: 1992-08-28
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepTools_HeaderFile
#define _BRepTools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Standard_CString.hxx>

#include <Message_ProgressIndicator.hxx>

class TopoDS_Face;
class TopoDS_Wire;
class TopoDS_Edge;
class Bnd_Box2d;
class TopoDS_Vertex;
class TopoDS_Shell;
class TopoDS_Solid;
class TopoDS_CompSolid;
class TopoDS_Compound;
class TopoDS_Shape;
class Message_ProgressIndicator;
class BRep_Builder;
class BRepTools_WireExplorer;
class BRepTools_Modification;
class BRepTools_Modifier;
class BRepTools_TrsfModification;
class BRepTools_NurbsConvertModification;
class BRepTools_GTrsfModification;
class BRepTools_Substitution;
class BRepTools_Quilt;
class BRepTools_ShapeSet;
class BRepTools_ReShape;
class Geom_Curve;
class Geom2d_Curve;
class Geom_Surface;


//! The BRepTools package provides  utilities for BRep
//! data structures.
//!
//! * WireExplorer : A tool to explore the topology of
//! a wire in the order of the edges.
//!
//! * ShapeSet :  Tools used for  dumping, writing and
//! reading.
//!
//! * UVBounds : Methods to compute the  limits of the
//! boundary  of a  face,  a wire or   an edge in  the
//! parametric space of a face.
//!
//! *  Update : Methods  to call when   a topology has
//! been created to compute all missing data.
//!
//! * UpdateFaceUVPoints  :  Method to  update  the UV
//! points stored   with  the edges   on a face.  This
//! method ensure that connected  edges  have the same
//! UV point on their common extremity.
//!
//! * Compare : Method to compare two vertices.
//!
//! * Compare : Method to compare two edges.
//!
//! * OuterWire : A method to find the outer wire of a
//! face.
//!
//! * Map3DEdges : A method to map all the 3D Edges of
//! a Shape.
//!
//! * Dump : A method to dump a BRep object.
class BRepTools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns in UMin,  UMax, VMin,  VMax  the  bounding
  //! values in the parametric space of F.
  Standard_EXPORT static void UVBounds (const TopoDS_Face& F, Standard_Real& UMin, Standard_Real& UMax, Standard_Real& VMin, Standard_Real& VMax);
  
  //! Returns in UMin,  UMax, VMin,  VMax  the  bounding
  //! values of the wire in the parametric space of F.
  Standard_EXPORT static void UVBounds (const TopoDS_Face& F, const TopoDS_Wire& W, Standard_Real& UMin, Standard_Real& UMax, Standard_Real& VMin, Standard_Real& VMax);
  
  //! Returns in UMin,  UMax, VMin,  VMax  the  bounding
  //! values of the edge in the parametric space of F.
  Standard_EXPORT static void UVBounds (const TopoDS_Face& F, const TopoDS_Edge& E, Standard_Real& UMin, Standard_Real& UMax, Standard_Real& VMin, Standard_Real& VMax);
  
  //! Adds  to  the box <B>  the bounding values in  the
  //! parametric space of F.
  Standard_EXPORT static void AddUVBounds (const TopoDS_Face& F, Bnd_Box2d& B);
  
  //! Adds  to the box  <B>  the bounding  values of the
  //! wire in the parametric space of F.
  Standard_EXPORT static void AddUVBounds (const TopoDS_Face& F, const TopoDS_Wire& W, Bnd_Box2d& B);
  
  //! Adds to  the box <B>  the  bounding values  of the
  //! edge in the parametric space of F.
  Standard_EXPORT static void AddUVBounds (const TopoDS_Face& F, const TopoDS_Edge& E, Bnd_Box2d& B);
  
  //! Update a vertex (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_Vertex& V);
  
  //! Update an edge, compute 2d bounding boxes.
  Standard_EXPORT static void Update (const TopoDS_Edge& E);
  
  //! Update a wire (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_Wire& W);
  
  //! Update a Face, update UV points.
  Standard_EXPORT static void Update (const TopoDS_Face& F);
  
  //! Update a shell (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_Shell& S);
  
  //! Update a solid (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_Solid& S);
  
  //! Update a composite solid (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_CompSolid& C);
  
  //! Update a compound (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_Compound& C);
  
  //! Update a shape, call the corect update.
  Standard_EXPORT static void Update (const TopoDS_Shape& S);
  
  //! For  all the edges  of the face  <F> reset  the UV
  //! points to  ensure that  connected  faces  have the
  //! same point at there common extremity.
  Standard_EXPORT static void UpdateFaceUVPoints (const TopoDS_Face& F);
  
  //! Removes all the triangulations of the faces of <S>
  //! and removes all polygons on triangulations of the
  //! edges.
  Standard_EXPORT static void Clean (const TopoDS_Shape& S);
  
  //! Removes all the pcurves of the edges of <S> that
  //! refer to surfaces not belonging to any face of <S>
  Standard_EXPORT static void RemoveUnusedPCurves (const TopoDS_Shape& S);
  
  //! verifies that each face from the shape <S> has got
  //! a triangulation  with a  deflection <= deflec  and
  //! the edges a discretisation on  this triangulation.
  Standard_EXPORT static Standard_Boolean Triangulation (const TopoDS_Shape& S, const Standard_Real deflec);
  
  //! Returns  True if  the    distance between the  two
  //! vertices is lower than their tolerance.
  Standard_EXPORT static Standard_Boolean Compare (const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  //! Returns  True if  the    distance between the  two
  //! edges is lower than their tolerance.
  Standard_EXPORT static Standard_Boolean Compare (const TopoDS_Edge& E1, const TopoDS_Edge& E2);
  
  //! Returns the outer most wire of <F>. Returns a Null
  //! wire if <F> has no wires.
  Standard_EXPORT static TopoDS_Wire OuterWire (const TopoDS_Face& F);
  
  //! Stores in the map  <M> all the 3D topology edges
  //! of <S>.
  Standard_EXPORT static void Map3DEdges (const TopoDS_Shape& S, TopTools_IndexedMapOfShape& M);
  
  //! Verifies that the edge  <E> is found two  times on
  //! the face <F> before calling BRep_Tool::IsClosed.
  Standard_EXPORT static Standard_Boolean IsReallyClosed (const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! Dumps the topological structure and the geometry
  //! of <Sh> on the stream <S>.
  Standard_EXPORT static void Dump (const TopoDS_Shape& Sh, Standard_OStream& S);
  
  //! Writes <Sh> on <S> in an ASCII format.
  Standard_EXPORT static void Write (const TopoDS_Shape& Sh, Standard_OStream& S, const Handle(Message_ProgressIndicator)& PR = NULL);
  
  //! Reads a Shape  from <S> in  returns it in  <Sh>.
  //! <B> is used to build the shape.
  Standard_EXPORT static void Read (TopoDS_Shape& Sh, Standard_IStream& S, const BRep_Builder& B, const Handle(Message_ProgressIndicator)& PR = NULL);
  
  //! Writes <Sh> in <File>.
  Standard_EXPORT static Standard_Boolean Write (const TopoDS_Shape& Sh, const Standard_CString File, const Handle(Message_ProgressIndicator)& PR = NULL);
  
  //! Reads a Shape  from <File>,  returns it in  <Sh>.
  //! <B> is used to build the shape.
  Standard_EXPORT static Standard_Boolean Read (TopoDS_Shape& Sh, const Standard_CString File, const BRep_Builder& B, const Handle(Message_ProgressIndicator)& PR = NULL);

  //! Evals real tolerance of edge  <theE>.
  //! <theC3d>, <theC2d>, <theS>, <theF>, <theL> are
  //! correspondently 3d curve of edge, 2d curve on surface <theS> and
  //! rang of edge
  //! If calculated tolerance is more then current edge tolerance, edge is updated.
  //! Method returns actual tolerance of edge
  Standard_EXPORT static Standard_Real EvalAndUpdateTol(const TopoDS_Edge& theE, 
                                                        const Handle(Geom_Curve)& theC3d, 
                                                        const Handle(Geom2d_Curve) theC2d, 
                                                        const Handle(Geom_Surface)& theS,
                                                        const Standard_Real theF,
                                                        const Standard_Real theL);



protected:





private:




friend class BRepTools_WireExplorer;
friend class BRepTools_Modification;
friend class BRepTools_Modifier;
friend class BRepTools_TrsfModification;
friend class BRepTools_NurbsConvertModification;
friend class BRepTools_GTrsfModification;
friend class BRepTools_Substitution;
friend class BRepTools_Quilt;
friend class BRepTools_ShapeSet;
friend class BRepTools_ReShape;

};







#endif // _BRepTools_HeaderFile
