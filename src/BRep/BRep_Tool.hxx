// Created on: 1993-07-07
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

#ifndef _BRep_Tool_HeaderFile
#define _BRep_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_Shape.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TopAbs_ShapeEnum.hxx>

class TopoDS_Shape;
class TopoDS_Face;
class TopLoc_Location;
class TopoDS_Edge;
class TopoDS_Vertex;


//! Provides class methods  to  access to the geometry
//! of BRep shapes.
class BRep_Tool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! If S is Shell, returns True if it has no free boundaries (edges).
  //! If S is Wire, returns True if it has no free ends (vertices).
  //! (Internal and External sub-shepes are ignored in these checks)
  //! If S is Edge, returns True if its vertices are the same.
  //! For other shape types returns S.Closed().
  Standard_EXPORT static Standard_Boolean IsClosed (const TopoDS_Shape& S);
  
  //! Returns the geometric surface of the face. Returns
  //! in <L> the location for the surface.
  Standard_EXPORT static const Handle(Geom_Surface)& Surface (const TopoDS_Face& F, TopLoc_Location& L);
  
  //! Returns the geometric  surface of the face. It can
  //! be a copy if there is a Location.
  Standard_EXPORT static Handle(Geom_Surface) Surface (const TopoDS_Face& F);
  
  //! Returns  the Triangulation of  the  face. It  is a
  //! null handle if there is no triangulation.
  Standard_EXPORT static const Handle(Poly_Triangulation)& Triangulation (const TopoDS_Face& F, TopLoc_Location& L);
  
  //! Returns the tolerance of the face.
  Standard_EXPORT static Standard_Real Tolerance (const TopoDS_Face& F);
  
  //! Returns the  NaturalRestriction  flag of the  face.
  Standard_EXPORT static Standard_Boolean NaturalRestriction (const TopoDS_Face& F);
  
  //! Returns True if <E> is a 3d curve or a curve on
  //! surface.
  Standard_EXPORT static Standard_Boolean IsGeometric (const TopoDS_Edge& E);
  
  //! Returns the 3D curve  of the edge.  May be  a Null
  //! handle. Returns in <L> the location for the curve.
  //! In <First> and <Last> the parameter range.
  Standard_EXPORT static const Handle(Geom_Curve)& Curve (const TopoDS_Edge& E, TopLoc_Location& L, Standard_Real& First, Standard_Real& Last);
  
  //! Returns the 3D curve  of the edge. May be a Null handle.
  //! In <First> and <Last> the parameter range.
  //! It can be a copy if there is a Location.
  Standard_EXPORT static Handle(Geom_Curve) Curve (const TopoDS_Edge& E, Standard_Real& First, Standard_Real& Last);
  
  //! Returns the 3D polygon of the edge. May be   a Null
  //! handle. Returns in <L> the location for the polygon.
  Standard_EXPORT static const Handle(Poly_Polygon3D)& Polygon3D (const TopoDS_Edge& E, TopLoc_Location& L);
  
  //! Returns the curve  associated to the  edge in  the
  //! parametric  space of  the  face.  Returns   a NULL
  //! handle  if this curve  does not exist.  Returns in
  //! <First> and <Last> the parameter range.
  //! If the surface is a plane the curve can be not stored but created a new
  //! each time. The flag pointed by <theIsStored> serves to indicate storage status. 
  //! It is valued if the pointer is non-null.
  Standard_EXPORT static Handle(Geom2d_Curve) CurveOnSurface (const TopoDS_Edge& E,
                                                              const TopoDS_Face& F,
                                                              Standard_Real& First,
                                                              Standard_Real& Last,
                                                              Standard_Boolean* theIsStored = NULL);
  
  //! Returns the  curve associated to   the edge in the
  //! parametric  space of the   surface. Returns a NULL
  //! handle  if this curve does  not exist.  Returns in
  //! <First> and <Last> the parameter range.
  //! If the surface is a plane the curve can be not stored but created a new
  //! each time. The flag pointed by <theIsStored> serves to indicate storage status. 
  //! It is valued if the pointer is non-null.
  Standard_EXPORT static Handle(Geom2d_Curve) CurveOnSurface(const TopoDS_Edge& E,
                                                              const Handle(Geom_Surface)& S,
                                                              const TopLoc_Location& L,
                                                              Standard_Real& First,
                                                              Standard_Real& Last,
                                                              Standard_Boolean* theIsStored = NULL);

  //! For the planar surface builds the 2d curve for the edge
  //! by projection of the edge on plane.
  //! Returns a NULL handle if the surface is not planar or
  //! the projection failed.
  Standard_EXPORT static Handle(Geom2d_Curve) CurveOnPlane (const TopoDS_Edge& E,
                                                            const Handle(Geom_Surface)& S,
                                                            const TopLoc_Location& L,
                                                            Standard_Real& First,
                                                            Standard_Real& Last);
  
  //! Returns in <C>, <S>, <L> a 2d curve, a surface and
  //! a location for the edge <E>. <C> and <S>  are null
  //! if the  edge has no curve on  surface.  Returns in
  //! <First> and <Last> the parameter range.
  Standard_EXPORT static void CurveOnSurface (const TopoDS_Edge& E, Handle(Geom2d_Curve)& C, Handle(Geom_Surface)& S, TopLoc_Location& L, Standard_Real& First, Standard_Real& Last);
  
  //! Returns in <C>, <S>, <L> the 2d curve, the surface
  //! and the location for the edge <E> of rank <Index>.
  //! <C> and <S> are null if the index is out of range.
  //! Returns in <First> and <Last> the parameter range.
  Standard_EXPORT static void CurveOnSurface (const TopoDS_Edge& E, Handle(Geom2d_Curve)& C, Handle(Geom_Surface)& S, TopLoc_Location& L, Standard_Real& First, Standard_Real& Last, const Standard_Integer Index);
  
  //! Returns the polygon associated to the  edge in  the
  //! parametric  space of  the  face.  Returns   a NULL
  //! handle  if this polygon  does not exist.
  Standard_EXPORT static Handle(Poly_Polygon2D) PolygonOnSurface (const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! Returns the polygon associated to the  edge in  the
  //! parametric  space of  the surface. Returns   a NULL
  //! handle  if this polygon  does not exist.
  Standard_EXPORT static Handle(Poly_Polygon2D) PolygonOnSurface (const TopoDS_Edge& E, const Handle(Geom_Surface)& S, const TopLoc_Location& L);
  
  //! Returns in <C>, <S>, <L> a 2d curve, a surface and
  //! a location for the edge <E>. <C> and <S>  are null
  //! if the  edge has no polygon on  surface.
  Standard_EXPORT static void PolygonOnSurface (const TopoDS_Edge& E, Handle(Poly_Polygon2D)& C, Handle(Geom_Surface)& S, TopLoc_Location& L);
  
  //! Returns in <C>, <S>, <L> the 2d curve, the surface
  //! and the location for the edge <E> of rank <Index>.
  //! <C> and <S> are null if the index is out of range.
  Standard_EXPORT static void PolygonOnSurface (const TopoDS_Edge& E, Handle(Poly_Polygon2D)& C, Handle(Geom_Surface)& S, TopLoc_Location& L, const Standard_Integer Index);
  
  //! Returns the polygon associated to the  edge in  the
  //! parametric  space of  the  face.  Returns   a NULL
  //! handle  if this polygon  does not exist.
  Standard_EXPORT static const Handle(Poly_PolygonOnTriangulation)& PolygonOnTriangulation (const TopoDS_Edge& E, const Handle(Poly_Triangulation)& T, const TopLoc_Location& L);
  
  //! Returns in <P>, <T>, <L> a polygon on triangulation, a
  //! triangulation and a location for the edge <E>.
  //! <P>  and  <T>  are null  if  the  edge has no
  //! polygon on  triangulation.
  Standard_EXPORT static void PolygonOnTriangulation (const TopoDS_Edge& E, Handle(Poly_PolygonOnTriangulation)& P, Handle(Poly_Triangulation)& T, TopLoc_Location& L);
  
  //! Returns   in   <P>,  <T>,    <L> a     polygon  on
  //! triangulation,   a triangulation  and a  location for
  //! the edge <E> for the range index.  <C> and <S> are
  //! null if the edge has no polygon on triangulation.
  Standard_EXPORT static void PolygonOnTriangulation (const TopoDS_Edge& E, Handle(Poly_PolygonOnTriangulation)& P, Handle(Poly_Triangulation)& T, TopLoc_Location& L, const Standard_Integer Index);
  
  //! Returns  True  if  <E>  has  two  PCurves  in  the
  //! parametric space of <F>. i.e.  <F>  is on a closed
  //! surface and <E> is on the closing curve.
  Standard_EXPORT static Standard_Boolean IsClosed (const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! Returns  True  if  <E>  has  two  PCurves  in  the
  //! parametric space  of <S>.  i.e.   <S>  is a closed
  //! surface and <E> is on the closing curve.
  Standard_EXPORT static Standard_Boolean IsClosed (const TopoDS_Edge& E, const Handle(Geom_Surface)& S, const TopLoc_Location& L);
  
  //! Returns  True  if <E> has two arrays of indices in
  //! the triangulation <T>.
  Standard_EXPORT static Standard_Boolean IsClosed (const TopoDS_Edge& E, const Handle(Poly_Triangulation)& T, const TopLoc_Location& L);
  
  //! Returns the tolerance for <E>.
  Standard_EXPORT static Standard_Real Tolerance (const TopoDS_Edge& E);
  
  //! Returns the SameParameter flag for the edge.
  Standard_EXPORT static Standard_Boolean SameParameter (const TopoDS_Edge& E);
  
  //! Returns the SameRange flag for the edge.
  Standard_EXPORT static Standard_Boolean SameRange (const TopoDS_Edge& E);
  
  //! Returns True  if the edge is degenerated.
  Standard_EXPORT static Standard_Boolean Degenerated (const TopoDS_Edge& E);
  
  //! Gets the range of the 3d curve.
  Standard_EXPORT static void Range (const TopoDS_Edge& E, Standard_Real& First, Standard_Real& Last);
  
  //! Gets the range  of the edge  on the pcurve on  the
  //! surface.
  Standard_EXPORT static void Range (const TopoDS_Edge& E, const Handle(Geom_Surface)& S, const TopLoc_Location& L, Standard_Real& First, Standard_Real& Last);
  
  //! Gets the range of the edge on the pcurve on the face.
  Standard_EXPORT static void Range (const TopoDS_Edge& E, const TopoDS_Face& F, Standard_Real& First, Standard_Real& Last);
  
  //! Gets the UV locations of the extremities of the edge.
  Standard_EXPORT static void UVPoints (const TopoDS_Edge& E, const Handle(Geom_Surface)& S, const TopLoc_Location& L, gp_Pnt2d& PFirst, gp_Pnt2d& PLast);
  
  //! Gets the UV locations of the extremities of the edge.
  Standard_EXPORT static void UVPoints (const TopoDS_Edge& E, const TopoDS_Face& F, gp_Pnt2d& PFirst, gp_Pnt2d& PLast);
  
  //! Sets the UV locations of the extremities of the edge.
  Standard_EXPORT static void SetUVPoints (const TopoDS_Edge& E, const Handle(Geom_Surface)& S, const TopLoc_Location& L, const gp_Pnt2d& PFirst, const gp_Pnt2d& PLast);
  
  //! Sets the UV locations of the extremities of the edge.
  Standard_EXPORT static void SetUVPoints (const TopoDS_Edge& E, const TopoDS_Face& F, const gp_Pnt2d& PFirst, const gp_Pnt2d& PLast);
  
  //! Returns True if the edge is on the surfaces of the
  //! two faces.
  Standard_EXPORT static Standard_Boolean HasContinuity (const TopoDS_Edge& E, const TopoDS_Face& F1, const TopoDS_Face& F2);
  
  //! Returns the continuity.
  Standard_EXPORT static GeomAbs_Shape Continuity (const TopoDS_Edge& E, const TopoDS_Face& F1, const TopoDS_Face& F2);
  
  //! Returns True if the edge is on the surfaces.
  Standard_EXPORT static Standard_Boolean HasContinuity (const TopoDS_Edge& E, const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const TopLoc_Location& L1, const TopLoc_Location& L2);
  
  //! Returns the continuity.
  Standard_EXPORT static GeomAbs_Shape Continuity (const TopoDS_Edge& E, const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const TopLoc_Location& L1, const TopLoc_Location& L2);
  
  //! Returns True if the edge has regularity on some
  //! two surfaces
  Standard_EXPORT static Standard_Boolean HasContinuity (const TopoDS_Edge& E);
  
  //! Returns the 3d point.
  Standard_EXPORT static gp_Pnt Pnt (const TopoDS_Vertex& V);
  
  //! Returns the tolerance.
  Standard_EXPORT static Standard_Real Tolerance (const TopoDS_Vertex& V);
  
  //! Returns the parameter of <V> on <E>.
  Standard_EXPORT static Standard_Real Parameter (const TopoDS_Vertex& V, const TopoDS_Edge& E);
  
  //! Returns the  parameters  of   the  vertex   on the
  //! pcurve of the edge on the face.
  Standard_EXPORT static Standard_Real Parameter (const TopoDS_Vertex& V, const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! Returns the  parameters  of   the  vertex   on the
  //! pcurve of the edge on the surface.
  Standard_EXPORT static Standard_Real Parameter (const TopoDS_Vertex& V, const TopoDS_Edge& E, const Handle(Geom_Surface)& S, const TopLoc_Location& L);
  
  //! Returns the parameters of the vertex on the face.
  Standard_EXPORT static gp_Pnt2d Parameters (const TopoDS_Vertex& V, const TopoDS_Face& F);

  //! Returns the maximum tolerance of input shape subshapes.
  //@param theShape    - Shape to search tolerance.
  //@param theSubShape - Search subshape, only Face, Edge or Vertex are supported.
  Standard_EXPORT static Standard_Real MaxTolerance (const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theSubShape);

};

#endif // _BRep_Tool_HeaderFile
