// Created on: 1995-01-04
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepLib_MakeEdge2d_HeaderFile
#define _BRepLib_MakeEdge2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepLib_EdgeError.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepLib_MakeShape.hxx>
#include <Standard_Real.hxx>
class StdFail_NotDone;
class TopoDS_Vertex;
class gp_Pnt2d;
class gp_Lin2d;
class gp_Circ2d;
class gp_Elips2d;
class gp_Hypr2d;
class gp_Parab2d;
class Geom2d_Curve;
class TopoDS_Edge;


//! Provides methods to build edges.
//!
//! The   methods have  the  following   syntax, where
//! TheCurve is one of Lin2d, Circ2d, ...
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
//! Create(C : TheCurve; P1, P2 : Pnt2d from gp)
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
class BRepLib_MakeEdge2d  : public BRepLib_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepLib_MakeEdge2d(const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Lin2d& L);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Lin2d& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Lin2d& L, const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Lin2d& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Circ2d& L);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Circ2d& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Circ2d& L, const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Circ2d& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Elips2d& L);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Elips2d& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Elips2d& L, const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Elips2d& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Hypr2d& L);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Hypr2d& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Hypr2d& L, const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Hypr2d& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Parab2d& L);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Parab2d& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Parab2d& L, const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const gp_Parab2d& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const Handle(Geom2d_Curve)& L);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const Handle(Geom2d_Curve)& L, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const Handle(Geom2d_Curve)& L, const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const Handle(Geom2d_Curve)& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const Handle(Geom2d_Curve)& L, const gp_Pnt2d& P1, const gp_Pnt2d& P2, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT BRepLib_MakeEdge2d(const Handle(Geom2d_Curve)& L, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const gp_Pnt2d& P1, const gp_Pnt2d& P2, const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C, const TopoDS_Vertex& V1, const TopoDS_Vertex& V2, const Standard_Real p1, const Standard_Real p2);
  
  //! Returns the error description when NotDone.
  Standard_EXPORT BRepLib_EdgeError Error() const;
  
  Standard_EXPORT const TopoDS_Edge& Edge();
  Standard_EXPORT operator TopoDS_Edge();
  
  //! Returns the first vertex of the edge. May be Null.
  Standard_EXPORT const TopoDS_Vertex& Vertex1() const;
  
  //! Returns the second vertex of the edge. May be Null.
  Standard_EXPORT const TopoDS_Vertex& Vertex2() const;




protected:





private:



  BRepLib_EdgeError myError;
  TopoDS_Vertex myVertex1;
  TopoDS_Vertex myVertex2;


};







#endif // _BRepLib_MakeEdge2d_HeaderFile
