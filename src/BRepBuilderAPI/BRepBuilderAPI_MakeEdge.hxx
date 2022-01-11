// Created on: 1993-07-06
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

#ifndef _BRepBuilderAPI_MakeEdge_HeaderFile
#define _BRepBuilderAPI_MakeEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepLib_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Real.hxx>
#include <BRepBuilderAPI_EdgeError.hxx>
class TopoDS_Vertex;
class gp_Pnt;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Parab;
class Geom_Curve;
class Geom2d_Curve;
class Geom_Surface;
class TopoDS_Edge;


//! Provides methods to build edges.
//!
//! The   methods have  the  following   syntax, where
//! TheCurve is one of Lin, Circ, ...
//!
//! Create(C : TheCurve)
//!
//! Makes an edge on  the whole curve.  Add vertices
//! on finite curves.
//!
//! Create(C : TheCurve; p1,p2 : Real)
//!
//! Make an edge  on the curve between parameters p1
//! and p2. if p2 < p1 the edge will be REVERSED. If
//! p1  or p2 is infinite the  curve will be open in
//! that  direction. Vertices are created for finite
//! values of p1 and p2.
//!
//! Create(C : TheCurve; P1, P2 : Pnt from gp)
//!
//! Make an edge on the curve  between the points P1
//! and P2. The  points are projected on   the curve
//! and the   previous method is  used. An  error is
//! raised if the points are not on the curve.
//!
//! Create(C : TheCurve; V1, V2 : Vertex from TopoDS)
//!
//! Make an edge  on the curve  between the vertices
//! V1 and V2. Same as the  previous but no vertices
//! are created. If a vertex is  Null the curve will
//! be open in this direction.
class BRepBuilderAPI_MakeEdge  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepBuilderAPI_MakeEdge();
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Lin& L);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Lin& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Lin& L, const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Lin& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Circ& L);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Circ& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Circ& L, const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Circ& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Elips& L);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Elips& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Elips& L, const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Elips& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Hypr& L);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Hypr& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Hypr& L, const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Hypr& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Parab& L);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Parab& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Parab& L, const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const gp_Parab& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L, const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L, const gp_Pnt& P1, const gp_Pnt& P2, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L, const Handle(Geom_Surface)& S);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L, const Handle(Geom_Surface)& S, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L, const Handle(Geom_Surface)& S, const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L, const Handle(Geom_Surface)& S, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L, const Handle(Geom_Surface)& S, const gp_Pnt& P1, const gp_Pnt& P2, const Standard_Real p1, const Standard_Real p2);
  
  //! The general method to directly create an edge is to give
  //! -      a 3D curve C as the support (geometric domain) of the edge,
  //! -      two vertices V1 and V2 to limit the curve (definition of the restriction of
  //! the edge), and
  //! -      two real values p1 and p2 which are the parameters for the vertices V1 and V2
  //! on the curve.
  //! The curve may be defined as a 2d curve in the parametric space of a surface: a
  //! pcurve. The surface on which the edge is built is then kept at the level of the edge.
  //! The default tolerance will be associated with this edge.
  //! Rules applied to the arguments:
  //! For the curve:
  //! -      The curve must not be a 'null handle'.
  //! -      If the curve is a trimmed curve the basis curve is used.
  //! For the vertices:
  //! -      Vertices may be null shapes. When V1 or V2 is null the edge is open in the
  //! corresponding direction and the parameter value p1 or p2 must be infinite
  //! (remember that Precision::Infinite() defines an infinite value).
  //! -      The two vertices must be identical if they have the same 3D location.
  //! Identical vertices are used in particular when the curve is closed.
  //! For the parameters:
  //! -      The parameters must be in the parametric range of the curve (or the basis
  //! curve if the curve is trimmed). If this condition is not satisfied the edge is not
  //! built, and the Error function will return BRepAPI_ParameterOutOfRange.
  //! -      Parameter values must not be equal. If this condition is not satisfied (i.e.
  //! if | p1 - p2 | ) the edge is not built, and the Error function will return
  //! BRepAPI_LineThroughIdenticPoints.
  //! Parameter values are expected to be given in increasing order:
  //! C->FirstParameter()
  //! - If the parameter values are given in decreasing order the vertices are switched,
  //! i.e. the "first vertex" is on the point of parameter p2 and the "second vertex" is
  //! on the point of parameter p1. In such a case, to keep the original intent of the
  //! construction, the edge will be oriented "reversed".
  //! - On a periodic curve the parameter values p1 and p2 are adjusted by adding or
  //! subtracting the period to obtain p1 in the parametric range of the curve, and p2]
  //! such that [ p1 , where Period is the period of the curve.
  //! - A parameter value may be infinite. The edge is open in the corresponding
  //! direction. However the corresponding vertex must be a null shape. If this condition
  //! is not satisfied the edge is not built, and the Error function will return
  //! BRepAPI_PointWithInfiniteParameter.
  //! - The distance between the vertex and the point evaluated on the curve with the
  //! parameter, must be lower than the precision of the vertex. If this condition is not
  //! satisfied the edge is not built, and the Error function will return
  //! BRepAPI_DifferentsPointAndParameter.
  //! Other edge constructions
  //! - The parameter values can be omitted, they will be computed by projecting the
  //! vertices on the curve. Note that projection is the only way to evaluate the
  //! parameter values of the vertices on the curve: vertices must be given on the curve,
  //! i.e. the distance from a vertex to the curve must be less than or equal to the
  //! precision of the vertex. If this condition is not satisfied the edge is not built,
  //! and the Error function will return BRepAPI_PointProjectionFailed.
  //! -      3D points can be given in place of vertices. Vertices will be created from the
  //! points (with the default topological precision Precision::Confusion()).
  //! Note:
  //! -      Giving vertices is useful when creating a connected edge.
  //! -      If the parameter values correspond to the extremities of a closed curve,
  //! points must be identical, or at least coincident. If this condition is not
  //! satisfied the edge is not built, and the Error function will return
  //! BRepAPI_DifferentPointsOnClosedCurve.
  //! -      The vertices or points can be omitted if the parameter values are given. The
  //! points will be computed from the parameters on the curve.
  //! The vertices or points and the parameter values can be omitted. The first and last
  //! parameters of the curve will then be used.
  //!
  //! Auxiliary methods
  Standard_EXPORT BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L, const Handle(Geom_Surface)& S, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& C);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& C, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& C, const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& C, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& C, const gp_Pnt& P1, const gp_Pnt& P2, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& C, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const Handle(Geom_Surface)& S);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const Handle(Geom_Surface)& S, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const Handle(Geom_Surface)& S, const gp_Pnt& P1, const gp_Pnt& P2);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const Handle(Geom_Surface)& S, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const Handle(Geom_Surface)& S, const gp_Pnt& P1, const gp_Pnt& P2, const Standard_Real p1, const Standard_Real p2);
  
  //! Defines or redefines the arguments for the construction of an edge.
  //! This function is currently used after the empty constructor BRepAPI_MakeEdge().
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const Handle(Geom_Surface)& S, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2, const Standard_Real p1, const Standard_Real p2);
  
  //! Returns true if the edge is built.
  Standard_EXPORT virtual Standard_Boolean IsDone() const Standard_OVERRIDE;
  
  //! Returns the construction status
  //! -   BRepBuilderAPI_EdgeDone if the edge is built, or
  //! -   another value of the BRepBuilderAPI_EdgeError
  //! enumeration indicating the reason of construction failure.
  Standard_EXPORT BRepBuilderAPI_EdgeError Error() const;
  

  //! Returns the constructed edge.
  //! Exceptions StdFail_NotDone if the edge is not built.
  Standard_EXPORT const TopoDS_Edge& Edge();
  Standard_EXPORT operator TopoDS_Edge();
  
  //! Returns the first vertex of the edge. May be Null.
  Standard_EXPORT const TopoDS_Vertex& Vertex1() const;
  
  //! Returns the second vertex of the edge. May be Null.
  //!
  //! Warning
  //! The returned vertex in each function corresponds respectively to
  //! -   the lowest, or
  //! -   the highest parameter on the curve along which the edge is built.
  //! It does not correspond to the first or second vertex
  //! given at the time of the construction, if the edge is oriented reversed.
  //! Exceptions
  //! StdFail_NotDone if the edge is not built.
  Standard_EXPORT const TopoDS_Vertex& Vertex2() const;




protected:





private:



  BRepLib_MakeEdge myMakeEdge;


};







#endif // _BRepBuilderAPI_MakeEdge_HeaderFile
